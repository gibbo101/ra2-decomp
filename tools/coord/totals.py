"""Game-code totals for the status lines: python3 tools/coord/totals.py

Game code is every function outside the library's span (the CRT and the import thunks):
everything below the first library function, and Westwood's assembly after the last one.
Unidentified functions inside the span are library code. Exact counts functions
progress.csv records as MATCH; fuzzy adds each unmatched function at its best score in
attempts.csv, weighted by size, with untried functions at zero.
"""
import csv
import re

libs = [int(r["address"], 16) for r in csv.DictReader(open("data/library.csv"))]
first, last = min(libs), max(libs)
fn = {int(r["address"], 16): int(r["size"])
      for r in csv.DictReader(open("data/functions.csv"))
      if not first <= int(r["address"], 16) <= last}
m = {int(r["address"], 16) for r in csv.DictReader(open("data/progress.csv"))
     if r["result"] == "MATCH"} & fn.keys()
best = {}
for r in csv.DictReader(open("data/attempts.csv")):
    score = re.match(r"\s*(\d+(?:\.\d+)?)", r["best"] or "")
    try:
        a = int(r["address"], 16)
    except ValueError:
        continue
    if score and a in fn and a not in m and float(score.group(1)) <= 100:
        best[a] = max(best.get(a, 0.0), float(score.group(1)))
b, total = sum(fn[a] for a in m), sum(fn.values())
fuzzy = b + sum(fn[a] * s / 100 for a, s in best.items())
print(f"{len(m):,} of {len(fn):,} functions ({100 * len(m) / len(fn):.1f}%), "
      f"{b / 1e6:.2f} MB of {total / 1e6:.2f} MB ({100 * b / total:.1f}%), gate {b / 1024:,.1f} KB, "
      f"fuzzy {100 * fuzzy / total:.1f}%")
