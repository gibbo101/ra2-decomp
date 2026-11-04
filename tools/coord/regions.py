"""Compare candidate shapes by half-open retail byte ranges, never candidate offsets."""
import difflib
import json


def differences(left, right, offsets):
    """offsets has one retail address per instruction plus the end of the listing."""
    return [[offsets[i], offsets[j], j - i, l - k] for op, i, j, k, l in
            difflib.SequenceMatcher(None, left, right, autojunk=False).get_opcodes() if op != "equal"]


def representatives(rows, caller):
    """Prefer fewer missing retail instructions, then fewer extra instructions.
    Retain distinct helper outcomes and insertion points even if the raw score drops."""
    keep = {}
    for row in rows:
        targets = row.get("targets", {})
        target = targets.get(caller, {})
        regions = target.get("regions")
        if regions is None:
            continue  # Old logs and failed/ambiguous caller observations cannot be aligned.
        helpers = tuple(sorted((a, r["status"]) for a, r in targets.items() if a != caller))
        edges = tuple(tuple(e) for e in target.get("changed_targets", ()))
        key = target["status"], tuple(tuple(r) for r in regions), edges, helpers
        rank = (target["status"] != "match", sum(r[2] for r in regions), len(edges), sum(r[3] for r in regions),
                -sum(status == "match" for _, status in helpers), -target["raw"], row["compile"])
        if key not in keep or rank < keep[key][0]:
            keep[key] = rank, row
    return [row for _, row in sorted(keep.values(), key=lambda item: item[0])]


def report(dest, caller):
    with (dest / "results.jsonl").open() as f:
        rows = [json.loads(line) for line in f]
    selected = representatives(rows, caller)
    print(f"{len(selected)} distinct caller-region/helper outcomes from {len(rows)} trials; "
          f"{sum('regions' not in r.get('targets', {}).get(caller, {}) for r in rows)} without alignment")
    for row in selected:
        target = row["targets"][caller]
        regions = ", ".join(f"0x{a:08X}..0x{b:08X} ({n}/{m} instructions)" for a, b, n, m in target["regions"])
        helpers = ", ".join(f"{a} {r['status']}" for a, r in sorted(row["targets"].items()) if a != caller)
        print(f"{dest / (row['source'] + '.cpp')} [{target['status']}, raw {target['raw']:.2%}] "
              f"{regions or 'no aligned instruction differences'}" + (f"; helpers: {helpers}" if helpers else ""))
        for addr, old, new in target.get("changed_targets", ()):
            print(f"  0x{addr:08X}: {old} -> {new}")
