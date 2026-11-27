#!/usr/bin/env python3
"""patch_sidecars.py OLD_MAP NEW_MAP GITDIR REV OUTDIR

The judge requires expected/<tu>.json "rows" to equal the map rows of the TU's object for every emitted role
(comdat, icf_fold, thunk, ...). For each row the overlay changed, rewrite that row in its object's sidecar at REV.
Prints "<repo path>\t<file in OUTDIR>" per patched sidecar. Exit 1 (CONFLICT) if a sidecar row did not equal the
upstream map row before patching."""
import json, os, subprocess, sys
EMITTED = {'comdat', 'icf_fold', 'ehhandler', 'funclet', 'unwind', 'initializer', 'terminator', 'thunk', 'import_thunk'}
old_m, new_m = (json.load(open(p, encoding="utf-8")) for p in sys.argv[1:3])
gitdir, rev, outdir = sys.argv[3:6]
old = {r["rva"]: r for r in old_m["rows"]}
tu_of = {o["object_id"]: o.get("tu") for o in new_m["objects"]}
changed = {}
for r in new_m["rows"]:
    if r != old[r["rva"]] and r["role"] in EMITTED:
        changed.setdefault(tu_of.get(r["object"]), []).append(r)
bad = 0
for tu, rows in sorted(changed.items(), key=lambda kv: str(kv[0])):
    if not tu:
        print("note: changed rows %s have no TU object; no sidecar" % [r["rva"] for r in rows], file=sys.stderr); continue
    path = "expected/" + tu[4:] + ".json"
    try:
        side = json.loads(subprocess.check_output(["git", "--git-dir", gitdir, "show", "%s:%s" % (rev, path)]))
    except subprocess.CalledProcessError:
        print("note: %s absent at %s" % (path, rev), file=sys.stderr); continue
    for r in rows:
        idx = [i for i, s in enumerate(side["rows"]) if s["rva"] == r["rva"]]
        if len(idx) != 1 or side["rows"][idx[0]] != old[r["rva"]]:
            print("CONFLICT sidecar %s row %s differs from the upstream map row" % (path, r["rva"]), file=sys.stderr); bad = 1; continue
        side["rows"][idx[0]] = r
    out = os.path.join(outdir, path.replace("/", "__"))
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write(json.dumps(side, indent=2) + "\n")
    print("%s\t%s" % (path, out))
sys.exit(bad)
