#!/usr/bin/env python3
"""judge_raw.py GITDIR REV TU [--psdk]: run tools/judge with BFME_JUDGE_REPO=GITDIR and print per-row status JSON lines."""
import json, os, subprocess, sys
sys.path.insert(0, os.path.expanduser("~/bin"))
import importlib.util
spec = importlib.util.spec_from_file_location("v", os.path.expanduser("~/bin/verify_tu.py")); v = importlib.util.module_from_spec(spec); spec.loader.exec_module(v)
args = sys.argv[1:]; psdk = "--psdk" in args; args = [a for a in args if a != "--psdk"]
gitdir, rev, tus = args[0], args[1], args[2:]
e = v.env(psdk); e["BFME_JUDGE_REPO"] = os.path.abspath(gitdir)
v.wait_for_capacity()
p = subprocess.run([sys.executable, v.JUDGE, "--json", rev, *tus], cwd=v.REPO, env=e, capture_output=True, text=True)
if not p.stdout.strip(): sys.exit("judge failed:\n" + p.stderr[-3000:])
for r in json.loads(p.stdout)["results"]:
    for row in r["rows"]:
        print("%s %-16s %-6s %s | %s" % (row["rva"], row["status"], row.get("pct") or "", row.get("symbol", "")[:70], ((row.get("relocation_check") or {}).get("why") or "")[:200]))
    for k in ("error", "gates"):
        if r.get(k): print(k, str(r[k])[:500])
