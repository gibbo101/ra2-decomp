"""Rank unresolved references by corroborating translation units.

Usage: uv run tools/refaudit.py [--objects build/obj] [--min-units 3]
With --objects, read the objects of a completed gate instead of compiling the sources.
Report disagreements separately. A shared name or repeated address alone cannot prove
identity. Corroborate with layout, symbol, or executable evidence before adding a name.
"""
import argparse
from collections import Counter, defaultdict
import csv
from pathlib import Path

import check
from savedobj import SavedObjects


def collect(objects):
    with (check.ROOT / "data/progress.csv").open() as f:
        sources = defaultdict(list)
        for row in csv.DictReader(f):
            if row["result"] == "MATCH":
                sources[row["source"]].append((int(row["address"], 16), row["symbol"]))
    saved = SavedObjects(objects) if objects is not None else None
    bases = defaultdict(lambda: defaultdict(Counter))
    orig, names = check.Original(), check.known_names()
    for path, targets in sorted(sources.items()):
        src = check.ROOT / path
        if saved is None:
            p, log = check.compile_source(src)
            if p is None:
                raise ValueError(log)
            raw = p.read_bytes()
        else:
            raw = saved.read(path)
        obj = check.CoffObject(raw)
        local, dup = check.bind_annotations(orig, obj, raw, src)
        if dup:
            raise ValueError("\n".join(dup))
        functions = {pick[0].name: pick for pick in obj.functions()}
        for addr, symbol in targets:
            pick = functions.get(symbol)
            if not pick or local.get(symbol) != addr:
                raise ValueError(f"{src}: rebuild and gate the object for 0x{addr:08X}")
            res = check.compare_one(orig, obj, raw, addr, symbol, names, local, pick)
            if not res.matched:
                raise ValueError(f"{src}: {symbol} no longer matches 0x{addr:08X}")
            if not res.unverified:
                continue
            _, sec, lo, _ = pick
            code = sec.data[lo:lo + res.ours]
            for ref in check.ref_checks(orig, obj, raw, pick, code, orig.read(addr, res.size), addr, names, local):
                if ref.status == "unverified":
                    bases[ref.target.name][ref.got][path] += 1
    return bases


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--objects", type=Path)
    parser.add_argument("--min-units", type=int, default=3)
    args = parser.parse_args()
    try:
        bases = collect(args.objects)
    except (ValueError, OSError) as exc:
        parser.exit(1, str(exc) + "\n")
    ranked = []
    for symbol, addresses in bases.items():
        if len(addresses) == 1:
            (base, units), = addresses.items()
            if len(units) >= args.min_units:
                ranked.append((-len(units), -sum(units.values()), symbol, base))
    print("Consistent base across translation units")
    for count, uses, symbol, base in sorted(ranked):
        print(f"0x{base:08X} {symbol} ({-count} units, {-uses} references)")
    print("Multiple bases")
    for symbol, addresses in sorted(bases.items()):
        if len(addresses) > 1:
            print(symbol)
            for base, units in sorted(addresses.items()):
                print(f"  0x{base:08X} ({sum(units.values())} references) {', '.join(sorted(units))}")


if __name__ == "__main__":
    main()
