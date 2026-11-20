"""Matched functions whose code most resembles an unmatched one, as source to start from.

Usage: uv run tools/similar.py 0x00410A60 [count]

Scores each matched game function by shared instruction 4-grams (mnemonics only), shared
callees and shared data references. A high score is a sibling or a near copy of the
target: read its source before writing the target's.
"""

import hashlib
import json
import sys

from exeindex import ROOT, ExeIndex

CACHE = ROOT / "build/similar_cache.json"


def features(index: ExeIndex, address: int) -> dict:
    ops = [i.mnemonic for i in index.instructions(address)]
    grams = sorted({" ".join(ops[k:k + 4]) for k in range(max(0, len(ops) - 3))})
    return {"grams": grams, "calls": sorted(set(index.calls(address))),
            "data": sorted(index.data_refs(address)), "length": len(ops)}


def jaccard(a: set, b: set) -> float:
    return len(a & b) / len(a | b) if a or b else 0.0


def score(target: dict, other: dict) -> float:
    grams = jaccard(set(target["grams"]), set(other["grams"]))
    calls = jaccard(set(target["calls"]), set(other["calls"]))
    data = jaccard(set(target["data"]), set(other["data"]))
    shorter, longer = sorted((target["length"], other["length"]))
    size = shorter / longer if longer else 0.0
    return 0.5 * grams + 0.2 * calls + 0.15 * data + 0.15 * size


def matched_features(index: ExeIndex) -> dict[str, dict]:
    stamp = hashlib.sha256(index.code + (ROOT / "data/progress.csv").read_bytes()).hexdigest()
    if CACHE.exists():
        cached = json.loads(CACHE.read_text())
        if cached.get("stamp") == stamp:
            return cached["functions"]
    table = {f"0x{a:08X}": features(index, a) for a in index.progress
             if index.matched(a) and a in index.functions and index.functions[a] >= 16}
    CACHE.parent.mkdir(exist_ok=True)
    CACHE.write_text(json.dumps({"stamp": stamp, "functions": table}))
    return table


def main() -> None:
    if len(sys.argv) not in (2, 3):
        raise SystemExit(__doc__)
    address = int(sys.argv[1], 16)
    count = int(sys.argv[2]) if len(sys.argv) == 3 else 10
    index = ExeIndex()
    target = features(index, address)
    table = matched_features(index)
    ranked = sorted(((score(target, f), a) for a, f in table.items() if int(a, 16) != address),
                    reverse=True)[:count]
    print(f"0x{address:08X} {index.name(address)} ({index.functions[address]} bytes): "
          f"closest matched functions")
    for value, a in ranked:
        where = index.source_line(int(a, 16))
        print(f"  {value:.3f}  {a} {index.name(int(a, 16))} ({index.functions[int(a, 16)]} bytes)"
              + (f"  {where[0]}" if where else ""))


if __name__ == "__main__":
    main()
