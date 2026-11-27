#!/usr/bin/env python3
"""test_apply_overlay.py [REF]: exercise apply_overlay's refusal rules on the first entry of REF's overlay
(REF defaults to HEAD; "." reads the working tree; run from the repository root). Prints one line per case; the expected outcome is in brackets."""
import copy, json, os, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))
from apply_overlay import apply, ADDRESS, VOIDPTR
ref = sys.argv[1] if len(sys.argv) > 1 else "HEAD"
show = lambda p: json.load(open(p, encoding="utf-8")) if ref == "." else json.loads(subprocess.check_output(["git", "show", "%s:%s" % (ref, p)]))
up, base = show("config/map.json"), show("config/overlay/ledger.json")
first = base["entries"][0]
other = next(r for r in up["rows"] if r["rva"] != first["rva"] and len(r["accepted_names"]) == 1
             and r["accepted_names"][0].startswith("?") and not ADDRESS.search(r["accepted_names"][0])
             and not VOIDPTR.search(r["accepted_names"][0]))
other_rva, other_name = other["rva"], other["accepted_names"][0]
cases = {
    "ok [APPLIED]": lambda e: None,
    "pin drift [CONFLICT]": lambda e: e["pin"].update(size=e["pin"]["size"] + 1),
    "upstream renamed [CONFLICT]": lambda e: e["pin"].update(name="?old@@"),
    "no row [CONFLICT]": lambda e: e.update(rva="0x00000001"),
    "address name [INVALID]": lambda e: e.update(name="?rva001F081A@X@@QAEXXZ"),
    "obsolete [OBSOLETE]": lambda e: e.update(name=e["pin"]["name"]),
    "name at other rva [CONFLICT]": lambda e: e.update(name=other_name),
    "shared declared [APPLIED]": lambda e: e.update(name=other_name, shared_with=[other_rva]),
    "no evidence [INVALID]": lambda e: e.pop("evidence"),
    "no before/after [INVALID]": lambda e: e.pop("after"),
    "set_name without replace [INVALID]": lambda e: e.update(op="set_name"),
}
for k, f in cases.items():
    e = copy.deepcopy(first); f(e)
    p, n = apply(copy.deepcopy(up), {"schema": 1, "entries": [e]})
    print("%-36s %s" % (k, (p or n)[0].split(":")[0] if (p or n) else "-"))
