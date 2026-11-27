#!/usr/bin/env python3
"""header_local.py BASE HEAD [GITDIR]: run the header source checks (H3 H4 H6 H7 H8) exactly as ci/header/collect.py's
preview step does, from this checkout's tools/gates, on BASE..HEAD (merge-tree of the two). Untrusted H4/H6 observations
are absent locally, so rules that need Boat's authenticated observations report held."""
import json, subprocess, sys
sys.dont_write_bytecode = True
from pathlib import Path
ROOT = Path.cwd()
sys.path.insert(0, str(ROOT / "ci" / "header"))
import collect
base, head = sys.argv[1], sys.argv[2]
repo = Path(sys.argv[3]) if len(sys.argv) > 3 else ROOT / ".git"
g = lambda *a: subprocess.check_output(["git", "--git-dir", str(repo), *a], text=True).strip()
base, head = g("rev-parse", base), g("rev-parse", head)
tree = g("merge-tree", "--write-tree", base, head).splitlines()[0]
paths = collect.header_paths(repo, base, tree)
print("header paths:", paths)
res = collect.source_checks(repo, base, head, tree)
print(json.dumps(res, indent=1, sort_keys=True, default=str)[:6000])
