#!/usr/bin/env python3
"""apply_overlay.py MAP_IN OVERLAY MAP_OUT [--strict]

Apply the fork's reviewed ledger overlay (config/overlay/ledger.json) to upstream's config/map.json, deterministically.
See config/overlay/README.md and docs/FORK.md.

Overlay format (JSON):
  {"schema": 1,
   "entries": [
     {"id": "ocl-create-typed",                  # unique, stable
      "rva": "0x001F081A",
      "op": "add_name",                           # add_name (default, additive) | set_name (replace primary name)
      "name": "?create@ObjectCreationList@@...",
      "pin": {"size": 94, "role": "owned",       # what the row looked like when the entry was reviewed;
              "name": "?create@...PAX000H@Z"},    #   any difference after an upstream sync is a CONFLICT
      "change": "add accepted name: typed parameters",   # what changes, in words
      "evidence": {"kind": "judge-reason",        # vtable-slot | retail-bytes | identical-copy | judge-reason
                   "detail": "...",               # retail bytes / vtable+slot / judge why line
                   "verified": "verify_tu ... at <main sha>"},
      "before": "MISNAMED (judge reason ...)",    # the row(s) at the base without the entry
      "after": "MATCH"}]}                        # the row(s) through the overlay; no other row changes

Rules (exit 1 on any CONFLICT or INVALID; nothing is written then):
  - the row must exist and match every pinned field;
  - a name must not be address-derived (G8 shapes, judge placeholders); on an owned row it must not spell a void
    pointer (PAX/PBX): gate.py derives G4's void-ABI exemption from exactly those rows, and G8's accepted set from
    address shapes, so an overlay can never widen either even if a gate were run against an overlaid base;
  - a name already listed at another RVA is a CONFLICT unless the entry sets "shared_with": ["0x..."] naming them;
  - a name already in the row is OBSOLETE (upstream adopted it): reported, skipped; --strict turns it into a failure;
  - set_name keeps the old primary name in accepted_names (additive), and requires "replace": true.
Output: upstream formatting (indent=2, key order kept, trailing newline); the snapshot field is not touched.
"""
import json, re, sys

# tools/gates/config.json "address_identifiers" (G8) plus tools/judge PLACEHOLDER spellings (never evidence of a name)
ADDRESS = re.compile(r"(?:_?G?Rva|_?rva|RVA)_?(?:0[xX])?[0-9A-Fa-f]{6,8}"
                     r"|(?:sub|g|m|unk|off|loc|byte|word|dword|qword|fn|data|ptr|vtable|float|int|spell|func)_(?:0[xX])?[0-9A-Fa-f]{6,8}"
                     r"|(?:FUN|DAT|LAB|PTR|UNK)_[0-9A-Fa-f]{6,8}|Bfme|Gen_t_|_0x[0-9A-Fa-f]{6}|\?A0x")
VOIDPTR = re.compile(r"P[AB]X")
KINDS = {"vtable-slot", "retail-bytes", "identical-copy", "judge-reason"}


def apply(ledger, overlay, strict=False):
    rows = {int(r["rva"], 16): r for r in ledger["rows"]}
    where = {}
    for a, r in rows.items():
        for n in r["accepted_names"]:
            where.setdefault(n, set()).add(a)
    problems, notes, seen = [], [], set()
    for e in overlay.get("entries", []):
        eid = e.get("id")
        tag = "%s %s" % (eid, e.get("rva"))
        if not eid or eid in seen:
            problems.append("INVALID %s: missing or duplicate id" % tag); continue
        seen.add(eid)
        ev = e.get("evidence") or {}
        if ev.get("kind") not in KINDS or not ev.get("detail") or not ev.get("verified"):
            problems.append("INVALID %s: evidence needs kind (%s), detail and verified" % (tag, "/".join(sorted(KINDS)))); continue
        if not e.get("change") or not e.get("before") or not e.get("after"):
            problems.append("INVALID %s: entry needs change, before and after" % tag); continue
        op, name = e.get("op", "add_name"), e.get("name", "")
        if op not in ("add_name", "set_name") or not name.startswith("?"):
            problems.append("INVALID %s: bad op or name" % tag); continue
        rva = int(e["rva"], 16)
        row = rows.get(rva)
        if row is None:
            problems.append("CONFLICT %s: upstream has no row at this RVA" % tag); continue
        for k, v in (e.get("pin") or {}).items():
            if row.get(k) != v:
                problems.append("CONFLICT %s: upstream %s is now %r, entry was reviewed against %r" % (tag, k, row.get(k), v))
        if any(p.startswith("CONFLICT %s:" % tag) for p in problems):
            continue
        if name in row["accepted_names"] and (op == "add_name" or row["name"] == name):
            (problems if strict else notes).append("OBSOLETE %s: upstream already lists %s; drop the entry" % (tag, name)); continue
        if ADDRESS.search(name) or (row["role"] == "owned" and VOIDPTR.search(name)):
            problems.append("INVALID %s: address-derived name, or void-pointer name on an owned row (G4 exemption source) %s" % (tag, name)); continue
        others = sorted(where.get(name, set()) - {rva})
        allowed = {int(x, 16) for x in e.get("shared_with", [])}
        if others and set(others) != allowed:
            problems.append("CONFLICT %s: %s is also listed at %s (declare them in shared_with after review)"
                            % (tag, name, ", ".join("0x%08X" % a for a in others))); continue
        if op == "set_name":
            if e.get("replace") is not True:
                problems.append("INVALID %s: set_name needs \"replace\": true" % tag); continue
            row["name"] = name
        if name not in row["accepted_names"]:
            row["accepted_names"].append(name)
        where.setdefault(name, set()).add(rva)
        notes.append("APPLIED %s: %s %s" % (tag, op, name))
    return problems, notes


def main():
    args = [a for a in sys.argv[1:] if a != "--strict"]
    if len(args) != 3:
        sys.exit(__doc__)
    with open(args[0], encoding="utf-8") as f:
        ledger = json.load(f)
    with open(args[1], encoding="utf-8") as f:
        overlay = json.load(f)
    problems, notes = apply(ledger, overlay, "--strict" in sys.argv)
    for n in notes:
        print(n)
    for p in problems:
        print(p, file=sys.stderr)
    if problems:
        sys.exit(1)
    with open(args[2], "w", encoding="utf-8", newline="\n") as f:
        f.write(json.dumps(ledger, indent=2) + "\n")


if __name__ == "__main__":
    main()
