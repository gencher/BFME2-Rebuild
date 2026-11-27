#!/usr/bin/env python3
"""includers.py: list every translation unit that includes the given headers, directly or through other headers.

    includers.py [--ref REF] include/Path/A.h [include/Path/B.h ...]

Follows #include lines across the tree at REF (default: the working tree of ~/BFME2-Rebuild), matching by file name as
the compiler's include path would. Prints the .c/.cpp files under src/, space separated, for verify_tu.py:

    python3 ~/bin/verify_tu.py BRANCH $(python3 ~/bin/includers.py include/Common/Geometry.h) --psdk
"""
import os
import re
import subprocess
import sys

REPO = os.path.expanduser("~/BFME2-Rebuild")
INC = re.compile(r'^\s*#\s*include\s*["<]([^">]+)[">]', re.M)


def main():
    args = sys.argv[1:]
    ref = None
    if args[:1] == ["--ref"]:
        ref, args = args[1], args[2:]
    if ref:
        names = subprocess.run(["git", "-C", REPO, "ls-tree", "-r", "--name-only", "-z", ref],
                               capture_output=True, text=True).stdout.split("\0")
        read = lambda f: subprocess.run(["git", "-C", REPO, "show", "%s:%s" % (ref, f)],
                                        capture_output=True, text=True, errors="ignore").stdout
    else:
        names = subprocess.run(["git", "-C", REPO, "ls-files", "-z"], capture_output=True, text=True).stdout.split("\0")
        read = lambda f: open(os.path.join(REPO, f), errors="ignore").read()
    src = [f for f in names if f.endswith((".c", ".cpp", ".h", ".hpp", ".inl"))]
    by_base = {}
    for f in src:
        by_base.setdefault(os.path.basename(f).lower(), []).append(f)
    users = {}
    for f in src:
        try:
            text = read(f)
        except OSError:
            continue
        for m in INC.findall(text):
            for g in by_base.get(os.path.basename(m).lower(), []):
                users.setdefault(g, set()).add(f)
    seen, todo = set(), list(args)
    while todo:
        for u in users.get(todo.pop(), ()):
            if u not in seen:
                seen.add(u)
                todo.append(u)
    tus = sorted(u for u in seen if u.startswith("src/") and u.endswith((".c", ".cpp")))
    print(" ".join(tus))
    print("%d units" % len(tus), file=sys.stderr)


if __name__ == "__main__":
    main()
