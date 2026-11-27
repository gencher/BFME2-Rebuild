#!/usr/bin/env python3
"""verify_tu.py: judge one or more translation units at a branch against local main, on the verification server.

    verify_tu.py BRANCH TU [TU...] [--rva 0x00ABCDEF] [--psdk] [--why] [--base REF] [--overlay[=FILE]] [--overlay-effect]

- BRANCH: a committed branch in ~/BFME2-Rebuild's git dir (worktrees share it). Local main must be the upstream main
  that BRANCH is based on (the judge reads the ledger from main).
- Prints every row whose status changed between main and BRANCH, and an ACCEPT or REJECT verdict:
    REJECT when a row loses MATCH, MISSING becomes PARTIAL, any row becomes WRONG_REFERENCE/MISNAMED/COMPILE_ERROR,
    or a row's PARTIAL percentage drops. Otherwise ACCEPT.
- --rva: also prints the non-relocation instruction differences of that function at BRANCH.
- --base REF: compare against REF instead of main (for example dev). The ledger still comes from local main, so REF
  must not change config/.
- --overlay[=FILE]: judge both sides through the fork's ledger overlay (docs/FORK.md). Builds a private judge view
  (tools/fork/judge_view.sh: view main = REF + overlaid config/map.json, BRANCH = BRANCH + the same) and points the
  judge at it. FILE defaults to REF:config/overlay/ledger.json (REF = --base, default main). The fork scripts are
  taken from REF:tools/fork, else BRANCH:tools/fork, else $FORK_TOOLS. A source change is still credited only for its
  own effect, because both sides carry the same overlay. Without --overlay nothing changes.
- --overlay-effect: with --overlay, compare BRANCH without the overlay against BRANCH with it, i.e. show exactly what
  the overlay entries change (use it to prove an entry: the target row reaches its claimed status, nothing else moves).
- --why: for every row that is not MATCH at BRANCH, print the judge's reason (relocation_check.why).
- --psdk: use the toolchain root that includes the Windows Server 2003 SP1 Platform SDK (needed for // psdk,
  // platformsdk and // dxsdk units; harmless for the others).

The compiler is Toolkit 1.0 (13.10.3052), calibrated against Boat on 41,443/41,443 rows. It is not hash-identical to
Boat's 13.10.3077: a local ACCEPT is a forecast; the hosted check stays the authority.
"""
import json, os, re, subprocess, sys

HOME = os.path.expanduser("~")
REPO = os.path.join(HOME, "BFME2-Rebuild")
JUDGE = os.path.join(REPO, "tools/judge/bin/judge.py")


JUDGE_REPO = os.path.join(REPO, ".git")   # replaced by the overlay view with --overlay


def env(psdk, repo=None):
    e = dict(os.environ)
    e.update({
        "BFME_JUDGE_RUNNER": os.path.join(HOME, "open-bfme-2"),
        "BFME_JUDGE_REPO": repo or JUDGE_REPO,
        "BFME_JUDGE_RETAIL": os.path.join(REPO, "baseline/game.dat"),
        "BFME_JUDGE_STATE": os.path.join(HOME, ".cache/bfme2-judge"),
        "BFME_JUDGE_WINEPREFIX": os.path.join(HOME, ".cache/bfme2-judge/wine"),
        "BFME_JUDGE_DEMANGLER": os.path.join(HOME, ".cache/bfme2-judge/demangle"),
        "VC71_ROOT": os.path.join(HOME, "vc71-3052-psdk" if psdk else "vc71-3052"),
        "WINEDEBUG": "-all", "PILOT_REMOTE": "0",
    })
    return e


MAX_LOAD = float(os.environ.get("VERIFY_MAX_LOAD", "14"))   # 1-minute load average; the server has 8 threads
MIN_FREE_GB = float(os.environ.get("VERIFY_MIN_FREE_GB", "2"))


def wait_for_capacity():
    """Several agents share the server: wait while it is saturated instead of piling on more Wine compilers."""
    import time
    waited = 0
    while True:
        load1 = os.getloadavg()[0]
        free_gb = 0.0
        with open("/proc/meminfo") as f:
            for line in f:
                if line.startswith("MemAvailable:"):
                    free_gb = int(line.split()[1]) / 1048576
        if load1 <= MAX_LOAD and free_gb >= MIN_FREE_GB:
            if waited:
                print("(waited %ds for server capacity)" % waited, file=sys.stderr)
            return
        time.sleep(10); waited += 10


def judge(rev, tus, psdk, extra=(), repo=None):
    wait_for_capacity()
    p = subprocess.run([sys.executable, JUDGE, *extra, rev, *tus], cwd=REPO, env=env(psdk, repo),
                       capture_output=True, text=True)
    return p


def rows(rev, tus, psdk, repo=None):
    p = judge(rev, tus, psdk, ("--json",), repo)
    if p.returncode != 0 and not p.stdout.strip():
        sys.exit("judge failed at %s:\n%s" % (rev, p.stderr[-2000:]))
    out = {}
    for r in json.loads(p.stdout)["results"]:
        for row in r["rows"]:
            out[(r["tu"], int(row["rva"], 16))] = (row["status"], row.get("pct"), row.get("symbol", ""),
                                                   (row.get("relocation_check") or {}).get("why", ""))
    return out


def realdiff(rev, tu, rva, psdk):
    p = judge(rev, [tu], psdk, ("--diff", rva))
    lines = p.stdout.splitlines()
    minus = [l for l in lines if l.startswith("-") and not l.startswith("---")]
    plus = [l for l in lines if l.startswith("+") and not l.startswith("+++")]
    norm = lambda l: re.sub(r"(call|jmp)\s+0x[0-9a-f]+|ds:0x[0-9a-f]+|0x[0-9a-f]{5,}", "<R>", l[1:]).strip()
    print("\n".join(lines[:2]))
    shown = 0
    for a, b in zip(minus, plus):
        if norm(a) != norm(b):
            print("  retail  :", norm(a)); print("  compiled:", norm(b)); shown += 1
            if shown >= 40: break
    if len(minus) != len(plus):
        print("  (retail has %d differing lines, compiled %d: lengths or layout differ)" % (len(minus), len(plus)))
    if not shown and len(minus) == len(plus):
        print("  (no non-relocation differences)")


def git(*a, **kw):
    return subprocess.run(["git", "--git-dir", os.path.join(REPO, ".git"), *a], capture_output=True, **kw)


def overlay_view(spec, base_ref, branch, tmp):
    """Build the judge view for --overlay and return its git dir. spec is True (overlay from base_ref) or a path."""
    tools = None
    for ref in (base_ref, branch):
        if git("cat-file", "-e", "%s:tools/fork/judge_view.sh" % ref).returncode == 0:
            arch = git("archive", ref, "tools/fork").stdout
            subprocess.run(["tar", "-x", "-C", tmp], input=arch, check=True)
            tools = os.path.join(tmp, "tools/fork"); break
    tools = tools or os.environ.get("FORK_TOOLS")
    if not tools:
        sys.exit("--overlay: no tools/fork at %s or %s, and FORK_TOOLS is unset" % (base_ref, branch))
    if spec is True:
        p = git("show", "%s:config/overlay/ledger.json" % base_ref)
        if p.returncode:
            sys.exit("--overlay: %s has no config/overlay/ledger.json; pass --overlay=FILE" % base_ref)
        spec = os.path.join(tmp, "ledger.json")
        with open(spec, "wb") as f:
            f.write(p.stdout)
    view = os.environ.get("VIEW_GIT") or os.path.join(HOME, "fork-judge", re.sub(r"[^A-Za-z0-9_.-]", "_", branch) + ".git")
    p = subprocess.run(["bash", os.path.join(tools, "judge_view.sh"), os.path.abspath(spec), base_ref, branch],
                       env=dict(os.environ, VIEW_GIT=view, SRC_GIT=os.path.join(REPO, ".git")),
                       capture_output=True, text=True)
    if p.returncode:
        sys.exit("--overlay: judge view failed:\n" + p.stdout + p.stderr)
    for line in p.stdout.splitlines():
        if line.startswith(("APPLIED", "OBSOLETE", "sidecar")):
            print("overlay:", line)
    for line in p.stderr.splitlines():
        print("overlay:", line)
    return view


def main():
    args = sys.argv[1:]
    psdk = "--psdk" in args
    why = "--why" in args
    args = [a for a in args if a not in ("--psdk", "--why")]
    base_ref = "main"
    if "--base" in args:
        i = args.index("--base"); base_ref = args[i + 1]; del args[i:i + 2]
    overlay = None
    effect = "--overlay-effect" in args
    for a in list(args):
        if a == "--overlay" or a.startswith("--overlay="):
            overlay = True if a == "--overlay" else a.split("=", 1)[1]; args.remove(a)
    if effect:
        args.remove("--overlay-effect")
        if not overlay:
            sys.exit("--overlay-effect needs --overlay")
    rva = None
    if "--rva" in args:
        i = args.index("--rva"); rva = args[i + 1]; del args[i:i + 2]
    branch, tus = args[0], args[1:]
    if overlay:
        import tempfile
        global JUDGE_REPO
        with tempfile.TemporaryDirectory() as tmp:
            view = overlay_view(overlay, base_ref, branch, tmp)
        if effect:
            base = rows(branch, tus, psdk)            # BRANCH, upstream ledger (shared repository)
            JUDGE_REPO = view
            head = rows(branch, tus, psdk)            # BRANCH, overlaid ledger
        else:
            JUDGE_REPO = view
            base = rows("main", tus, psdk)            # view main = base_ref + overlay
            head = rows(branch, tus, psdk)
    else:
        base = rows(base_ref, tus, psdk)
        head = rows(branch, tus, psdk)
    reject = []
    gained = 0
    for key in sorted(set(base) | set(head)):
        b = base.get(key, ("MISSING", None, "")); h = head.get(key, ("MISSING", None, ""))
        if b[:2] == h[:2]:
            continue
        tu, addr = key
        print("%s 0x%08X %-40s %s %s -> %s %s" % (tu.split("/")[-1], addr, (h[2] or b[2])[:40], b[0], b[1] or "", h[0], h[1] or ""))
        if b[0] == "MATCH" and h[0] != "MATCH": reject.append("lost MATCH at 0x%08X" % addr)
        elif b[0] == "MISSING" and h[0] == "PARTIAL": reject.append("new PARTIAL body at 0x%08X" % addr)
        elif h[0] in ("WRONG_REFERENCE", "MISNAMED", "COMPILE_ERROR") and b[0] != h[0]:
            reject.append("%s at 0x%08X" % (h[0], addr))
        elif b[0] == h[0] == "PARTIAL" and (h[1] or 0) < (b[1] or 0): reject.append("PARTIAL dropped at 0x%08X" % addr)
        if h[0] == "MATCH" and b[0] != "MATCH": gained += 1
    if why:
        for (tu, addr), h in sorted(head.items()):
            if h[0] != "MATCH" and h[3]:
                print("why %s 0x%08X %s %s: %s" % (tu.split("/")[-1], addr, h[0], h[2][:50], h[3][:400]))
    if rva:
        realdiff(branch, tus[0], rva, psdk)
    print("gained MATCH: %d" % gained)
    print(("REJECT: " + "; ".join(reject)) if reject else "ACCEPT")


if __name__ == "__main__":
    main()
