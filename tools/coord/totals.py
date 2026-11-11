"""Game-code totals for the status lines: python3 tools/coord/totals.py

Game code and matching are as tools/gamecode.py defines them. Fuzzy adds each unmatched
function at its best score in attempts.csv, weighted by size, with untried functions at zero.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import gamecode  # noqa: E402

fn, _ = gamecode.split_functions()
m = gamecode.matched() & fn.keys()
best = gamecode.best_scores(fn, m)
b, total = sum(fn[a] for a in m), sum(fn.values())
fuzzy = b + sum(fn[a] * s / 100 for a, s in best.items())
print(f"{len(m):,} of {len(fn):,} functions ({100 * len(m) / len(fn):.1f}%), "
      f"{b / 1e6:.2f} MB of {total / 1e6:.2f} MB ({100 * b / total:.1f}%), gate {b / 1024:,.1f} KB, "
      f"fuzzy {100 * fuzzy / total:.1f}%")
