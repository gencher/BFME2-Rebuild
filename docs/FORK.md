# What this fork changes

This fork's `dev` branch is upstream `main` plus source work in progress plus three fork-only additions. None of them
modify an upstream file, so an upstream sync never conflicts with them:

| path | what |
|---|---|
| `config/overlay/ledger.json`, `config/overlay/README.md` | the **ledger overlay**: reviewed, evidenced corrections to upstream's generated `config/map.json` |
| `tools/fork/` | the scripts that apply the overlay and judge through it, and the local header checks |
| `docs/FORK.md` | this file |

Upstream's `config/map.json`, `expected/`, `tools/judge`, `tools/gates` and `ci/` stay byte-identical on `dev`.

## Why an overlay

The judge credits a body only if the compiled symbol is one of the ledger row's `accepted_names`. Some upstream rows
carry a name the retail binary contradicts: an invented owner class, a non-virtual spelling for a vtable slot, a
`void *`/`int` stand-in that G4 refuses in source, or a missing alias on an ICF fold row. Correct source then stays
MISNAMED or WRONG_REFERENCE until upstream changes its ledger. The overlay lets the fork judge such source against the
corrected ledger now, keeps the evidence in one reviewed file, and doubles as the ledger change request for upstream.

## How the judge sees it

The upstream judge (`tools/judge/bin/judge.py`) reads the ledger from the judged revision's tree, and refuses a
revision whose `config/map.json` differs from `main:config/map.json` in the git dir named by `BFME_JUDGE_REPO`. The
fork does not touch the judge. Instead `tools/fork/judge_view.sh OVERLAY BASE [BRANCH...]` builds a private bare git
dir (objects borrowed through git alternates, nothing written to the shared repository) in which

- `main` = `BASE` + one deterministic commit replacing `config/map.json` with the overlaid ledger, and patching the
  owning object's `expected/<tu>.json` row for overlaid rows of an emitted role (fold, comdat, thunk...);
- each `BRANCH` = that branch + the same commit (refused if the branch changed `config/map.json` or a patched sidecar).

The judge then runs unchanged with `BFME_JUDGE_REPO` pointing at the view. Results are cached under different keys
from upstream results, because the cache key covers the config blobs. An empty overlay reproduces upstream verdicts
exactly.

`tools/fork/apply_overlay.py` applies the overlay and refuses anything that could widen the source rules: names that
are address-derived (G8) or void-pointer spellings on owned rows (G4's exemption source), pinned rows that changed
upstream, and names already bound to another RVA. `tools/fork/test_apply_overlay.py .` exercises those refusals.

## Verify flow

`tools/fork/verify_tu.py` is `~/bin/verify_tu.py` with two extra options (the copy in `~/bin` is the same file).
Without them its behaviour is unchanged.

```sh
# ordinary fork work: credit a source change against dev, both sides through dev's overlay
python3 ~/bin/verify_tu.py laptop/xx src/.../File.cpp --base dev --overlay [--why] [--psdk]

# prove a new overlay entry: the same branch without vs with the overlay; only the claimed rows may change
python3 ~/bin/verify_tu.py laptop/xx src/.../File.cpp [owner units of patched sidecars] \
    --base dev --overlay=config/overlay/ledger.json --overlay-effect --why

# raw per-row verdicts through a view
tools/fork/judge_view.sh config/overlay/ledger.json dev laptop/xx      # VIEW_GIT=... to choose the view dir
python3 tools/fork/judge_raw.py ~/fork-judge/view.git laptop/xx src/.../File.cpp
```

`--overlay` with no file uses `BASE:config/overlay/ledger.json`, and takes the scripts from `BASE:tools/fork` (else
`BRANCH:tools/fork`, else `$FORK_TOOLS`). Each branch gets its own view dir (`~/fork-judge/<branch>.git`), so agents do
not rewrite each other's refs. Keep a change only if the overlay run says ACCEPT with a gained MATCH, exactly as for
the plain run.

Rules for overlay-dependent source:

- A source change whose MATCH needs an overlay entry is REJECTed by the plain run and by Boat. Keep it on `dev` and say
  so in its commit message ("needs overlay entry <id>").
- **Upstream pull requests are cut without `config/overlay/`, `tools/fork/` and `docs/FORK.md`**; G1 refuses those
  paths. Hold overlay-dependent source back from upstream until upstream adopts the entry. File the entry as a ledger
  change request (`docs/ledger-and-checks.md`, "Request a ledger change"); it already carries the RVA, the evidence and
  the before and after verdicts.
- Run the gate for fork branches against `dev` (`gate.py . dev HEAD`). A commit that edits the overlay or the fork
  tools fails G1 by design; review those commits by hand instead.

## Header approval checklist

Upstream's header rules H1-H10 are partly administrative (H2, H9, H10 need Boat's receipts or a maintainer). On the
fork, a new or promoted header is approved when the commit body records:

1. **One definition:** `git grep -n "class X\b\|struct X\b" include src` shows a single definition;
   `python3 tools/fork/header_local.py BASE HEAD` passes H3; a promoted Draft is deleted and every includer moved (H7).
2. **Vtable:** for every virtual, the retail vtable symbol or RVA, the slot index and the slot's target RVA (from
   `config/map.json` rows or objdump of `.rdata`), and that declaration order equals slot order, including inherited
   slots and the destructor slot.
3. **Layout:** size and offsets of every member a matched function touches (an objdump displacement such as
   `[ecx+0x160]`). Every moved or new member is used by at least one newly matching function; a size or base change
   needs a function that depends on the size (operator new argument, array stride).
4. **Names:** every declared method with a ledger row compiles to an accepted name (H2), or to an overlay entry with
   evidence, marked "upstream: ledger request pending".
5. **All includers judged:** `python3 ~/bin/includers.py include/X.h` (or `tools/fork/includers.py`) and
   `verify_tu.py BRANCH <all units> --base dev --overlay`: ACCEPT, no lost MATCH; `gate.py` and `header_local.py`
   (H3, H7, H8) pass.
6. **Inline bodies:** trivial getters and setters only, or every includer judged in step 5; no `__forceinline` or other
   forcing constructs (H8).
7. **Removals:** a `Removed-Declaration: <Qualified::Name>` trailer, and `git grep` shows no users left.

`docs/anti-patterns.md` applies unchanged: the overlay corrects names, never code generation.

## After an upstream sync

1. Merge or rebase `dev` onto the new upstream `main`. `config/overlay/`, `tools/fork/` and `docs/FORK.md` do not
   conflict, because upstream has no such paths.
2. Check the overlay against the new ledger:
   `python3 tools/fork/apply_overlay.py config/map.json config/overlay/ledger.json /tmp/map.json --strict`
   - OBSOLETE: upstream adopted the name; delete the entry.
   - CONFLICT: upstream changed the pinned row; re-review the entry against the new row, re-prove it with
     `--overlay-effect`, and update its `pin`, or delete it.
3. If upstream changed `tools/judge` so that it no longer reads `main` from `BFME_JUDGE_REPO`, adapt
   `tools/fork/judge_view.sh`; the judge itself stays unmodified.
4. The views rebuild themselves on every `--overlay` run; nothing else is cached by hand.
