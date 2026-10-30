"""Delete banked attempts whose function progress.csv now records as MATCH: python3 tools/coord/stale.py [--dry]

Matches build/batch/**/<addr>.cpp and <addr>_*.cpp, with or without the 0x prefix; overlay
include/ directories are left alone.
"""
import csv, os, re, sys
from pathlib import Path

dry = "--dry" in sys.argv
done = {int(r["address"], 16) for r in csv.DictReader(open("data/progress.csv")) if r["result"] == "MATCH"}
n = 0
for p in sorted(Path("build/batch").rglob("*.cpp")):
    if "include" in p.parts:
        continue
    m = re.match(r"^(?:0x)?([0-9A-Fa-f]{6,8})(?:\.cpp|_)", p.name)
    if m and int(m.group(1), 16) in done:
        print(p)
        n += 1
        if not dry:
            os.remove(p)
print(n, "stale banked files", "found" if dry else "deleted")
