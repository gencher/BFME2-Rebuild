# Fork ledger overlay

`ledger.json` holds the fork's reviewed corrections to upstream's generated ledger (`config/map.json`). It is **fork
only**: upstream never sees it, `config/map.json` itself stays byte-identical to upstream, and the judge applies the
overlay only through the private judge view built by `tools/fork/judge_view.sh` (see `docs/FORK.md`).

## Entry format

```json
{
  "id": "detachableriderupdate-update-virtual",       // unique, stable
  "rva": "0x004AEC9B",                                 // the ledger row
  "op": "add_name",                                    // add_name (default): append to accepted_names
                                                       // set_name: replace the primary name ("replace": true; the
                                                       //   old name stays accepted)
  "name": "?update@DetachableRiderUpdate@@UAE?AW4UpdateSleepTime@@XZ",
  "shared_with": ["0x..."],                            // only if the name is already listed at other RVAs
  "change": "what changes and why, in words",
  "pin": {"size": 618, "role": "owned", "name": "..."},// the upstream row as reviewed (fold rows add fold_group)
  "evidence": {"kind": "vtable-slot",                  // vtable-slot | retail-bytes | identical-copy | judge-reason
               "detail": "the retail fact: vtable+slot, bytes, the judge's reason",
               "verified": "the command that proved it"},
  "before": "status of the affected rows without the entry",
  "after": "status of the affected rows through the overlay"
}
```

`tools/fork/apply_overlay.py` enforces the rules and writes nothing if any entry fails:

- **CONFLICT**: the RVA has no upstream row, a pinned field changed upstream, or the name is already listed at another
  RVA without `shared_with`.
- **INVALID**: missing evidence, `change`, `before` or `after`; an address-derived name (G8 shapes, judge placeholders);
  a void-pointer spelling (`PAX`/`PBX`) on an owned row (the gate derives G4's exemption from those).
- **OBSOLETE**: upstream already lists the name. Delete the entry (`--strict` turns this into a failure).

## Admission standard

An entry is added only when `verify_tu.py BRANCH TU... --base dev --overlay=FILE --overlay-effect` shows the claimed
rows reaching the claimed status and **no other row changing**, in the row's own unit and in the unit that owns any
patched sidecar. Fold and COMDAT aliases need `vtable-slot` or `identical-copy` evidence. A name the retail binary
cannot have carried (for example a COMDAT name that is already bound to a different surviving copy) is not admitted,
even if it would make a row pass.

Each entry is also a ready-made upstream ledger change request (`docs/ledger-and-checks.md`). When upstream adopts
it, `apply_overlay.py` reports OBSOLETE and the entry is deleted.
