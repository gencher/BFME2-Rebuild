#!/usr/bin/env bash
# judge_view.sh OVERLAY BASE [BRANCH...]
#
# Build the fork's private judge repository: a bare git dir (objects borrowed read-only from the shared repository
# through git alternates) in which
#   main    = BASE   + one commit replacing config/map.json with the overlaid ledger
#   BRANCH  = BRANCH + the same commit
# plus, for overlaid rows of an emitted role (comdat, icf_fold, thunk...), the matching row of the owning object's
# expected/<tu>.json sidecar, which the judge requires to equal the map row.
#
# The upstream judge then runs unchanged with BFME_JUDGE_REPO pointing at the view: its "candidate config equals main"
# check holds, and every row is judged against the overlaid accepted names. Nothing is written to the shared repository.
#
#   OVERLAY  path of an overlay file (config/overlay/ledger.json schema); not a pipe, the path is realpath'ed
#   BASE     the revision whose tree becomes the view's main (normally dev)
#   BRANCH   revisions to add; a BRANCH whose config/map.json or patched sidecars differ from BASE's is refused
#            (rebase it first), exactly like the judge refuses a stale config.
# Environment: SRC_GIT (default ~/BFME2-Rebuild/.git), VIEW_GIT (default ~/fork-judge/view.git; use one per agent or
# per branch, because every run rewrites the refs).
# An empty overlay ({"schema":1,"entries":[]}) yields a byte-identical map.json, i.e. the base's verdicts.
set -euo pipefail
[ $# -ge 2 ] || { sed -n '2,20p' "$0"; exit 2; }
OVERLAY=$(realpath "$1"); BASE=$2; shift 2
SRC=${SRC_GIT:-$HOME/BFME2-Rebuild/.git}
VIEW=${VIEW_GIT:-$HOME/fork-judge/view.git}
HERE=$(dirname "$(realpath "$0")")
if [ ! -d "$VIEW" ]; then
  mkdir -p "$(dirname "$VIEW")"
  git init -q --bare "$VIEW"
  echo "$(realpath "$SRC")/objects" > "$VIEW/objects/info/alternates"
fi
G() { git --git-dir "$VIEW" "$@"; }
UP=$(git --git-dir "$SRC" rev-parse --verify "$BASE^{commit}")
TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT
G cat-file blob "$UP:config/map.json" > "$TMP/map.json"
python3 "$HERE/apply_overlay.py" "$TMP/map.json" "$OVERLAY" "$TMP/map.out.json"
UPBLOB=$(G rev-parse "$UP:config/map.json")
NEWBLOB=$(G hash-object -w "$TMP/map.out.json")
mkdir "$TMP/side"
python3 "$HERE/patch_sidecars.py" "$TMP/map.json" "$TMP/map.out.json" "$VIEW" "$UP" "$TMP/side" > "$TMP/side.list"
SIDE=()   # "path base-blob new-blob"
while IFS=$'\t' read -r p f; do
  SIDE+=("$p $(G rev-parse "$UP:$p") $(G hash-object -w "$f")"); echo "sidecar $p patched"
done < "$TMP/side.list"
OVSHA=$(sha256sum "$OVERLAY" | cut -c1-16)
overlaid() {  # overlaid REV REFNAME
  local rev=$1 ref=$2
  if [ "$(G rev-parse "$rev:config/map.json")" != "$UPBLOB" ]; then
    echo "refused: $ref has a config/map.json different from $BASE; rebase it" >&2; return 1
  fi
  rm -f "$TMP/index"
  GIT_INDEX_FILE="$TMP/index" G read-tree "$rev"
  GIT_INDEX_FILE="$TMP/index" G update-index --cacheinfo "100644,$NEWBLOB,config/map.json"
  local s p ub nb
  for s in "${SIDE[@]+"${SIDE[@]}"}"; do
    read -r p ub nb <<<"$s"
    if [ "$(G rev-parse "$rev:$p")" != "$ub" ]; then echo "refused: $ref changed $p" >&2; return 1; fi
    GIT_INDEX_FILE="$TMP/index" G update-index --cacheinfo "100644,$nb,$p"
  done
  local tree; tree=$(GIT_INDEX_FILE="$TMP/index" G write-tree)
  local c; c=$(GIT_AUTHOR_NAME=overlay GIT_AUTHOR_EMAIL=overlay@local GIT_COMMITTER_NAME=overlay \
      GIT_COMMITTER_EMAIL=overlay@local GIT_AUTHOR_DATE='@0 +0000' GIT_COMMITTER_DATE='@0 +0000' \
      G commit-tree "$tree" -p "$rev" -m "ledger overlay $OVSHA on $rev")
  G update-ref "refs/heads/$ref" "$c"
  echo "$ref -> $c (base $rev, map blob $NEWBLOB)"
}
overlaid "$UP" main
for b in "$@"; do overlaid "$(git --git-dir "$SRC" rev-parse --verify "$b^{commit}")" "$b"; done
