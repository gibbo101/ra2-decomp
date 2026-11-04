"""List unrecorded C++ functions already emitted in their owning module.

Run after a successful cold gate: uv run tools/emitted.py [--objects build/obj]
A hit is a lead. Verify the identity, unverified references, and the annotated module
with check.py before recording it. No source files or progress records are written.
"""
import argparse
from collections import Counter, defaultdict
import csv
from pathlib import Path

import check
from tier0 import MIN_SIZE, module_lookup


def same_bytes(code, original, masked):
    return len(code) == len(original) and all(a == b or i in masked for i, (a, b) in enumerate(zip(code, original)))


def scan(objects):
    orig, names = check.Original(), check.known_names()
    with (check.ROOT / "data/progress.csv").open() as f:
        recorded = {int(r["address"], 16) for r in csv.DictReader(f) if r["result"] == "MATCH"}
    with (check.ROOT / "data/library.csv").open() as f:
        cutoff = min(int(r["address"], 16) for r in csv.DictReader(f))
    owner = module_lookup()
    targets = defaultdict(list)
    for addr, size in orig.sizes.items():
        if addr < cutoff and addr not in recorded and size >= MIN_SIZE:
            targets[owner(addr).lower(), size].append((addr, orig.read(addr, size)))
    for src in sorted((check.ROOT / "src").rglob("*.cpp")):
        rel = src.relative_to(check.ROOT)
        module = src.relative_to(check.ROOT / "src").with_suffix("").as_posix().lower()
        if not any(key[0] == module for key in targets):
            continue
        raw = (objects / rel.with_suffix(".obj")).read_bytes()
        obj = check.CoffObject(raw)
        local = None
        held = check.vftable_holders(obj)
        annotated = dict(check.annotations(src))
        for pick in obj.functions():
            sym = pick[0].name
            code, masked = check.function_bytes(pick)
            for addr, original in targets.get((module, len(code)), ()):
                if addr in annotated or not same_bytes(code, original, masked):
                    continue
                if local is None:
                    local, duplicates = check.bind_annotations(orig, obj, raw, src)
                    if duplicates:
                        raise ValueError("\n".join(duplicates))
                if sym in local:
                    continue
                result = check.compare_one(orig, obj, raw, addr, "=" + sym, names, {**local, sym: addr}, pick)
                problem = check.class_problem(orig.rtti(), held, sym, addr)
                if result.matched and not (problem and problem[1]):
                    yield addr, rel, sym, result.unverified, (problem[0] if problem else "")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--objects", type=Path, default=check.ROOT / "build/obj")
    args = parser.parse_args()
    try:
        rows = sorted(scan(args.objects))
    except (ValueError, OSError) as exc:
        parser.exit(1, str(exc) + "\n")
    by_addr = Counter(r[0] for r in rows)
    by_symbol = Counter((r[1], r[2]) for r in rows)
    for addr, src, sym, unverified, warning in rows:
        ambiguous = by_addr[addr] > 1 or by_symbol[src, sym] > 1
        print(f"0x{addr:08X} {src} {sym} ({unverified} unverified)"
              + (" [ambiguous]" if ambiguous else "") + (" " + warning if warning else ""))


if __name__ == "__main__":
    main()
