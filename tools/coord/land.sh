#!/bin/bash
# The landing steps after the gate, --record and the plain re-run have passed:
#   bash tools/coord/land.sh DATE GATE.txt JOURNAL...
# Prints the rule scans and the claim tally, appends attempts.csv rows, regenerates
# modules and manifest, deletes stale banks, and prints the game totals for the docs.
set -u
cd "$(git rev-parse --show-toplevel)" || exit 1
DATE=$1 GATE=$2; shift 2
echo "### rule scans (licence-header lines at :3 omitted)"
bash tools/coord/landscan.sh 2>&1 | grep -v ':3$'
echo "### claims against the tree"
python3 tools/coord/tally.py --gate "$GATE" "$@"
echo "### attempts.csv"
python3 tools/coord/attempts_rows.py "$DATE" "$@"
echo "### modules and manifest"
uv run tools/modules.py > /dev/null 2>&1 || echo "modules.py failed"
uv run tools/manifest.py | tail -n 1
echo "### stale banks"
python3 tools/coord/stale.py
echo "### untracked files outside build/"
git status --short --untracked-files=all | grep '^??' | grep -v '^?? build/'
echo "### game totals"
python3 - <<'EOF'
import csv
lib = min(int(r["address"], 16) for r in csv.DictReader(open("data/library.csv")))
fn = {int(r["address"], 16): int(r["size"]) for r in csv.DictReader(open("data/functions.csv")) if int(r["address"], 16) < lib}
m = [int(r["address"], 16) for r in csv.DictReader(open("data/progress.csv")) if r["result"] == "MATCH"]
m = [a for a in m if a in fn]
b, total = sum(fn[a] for a in m), sum(fn.values())
print(f"{len(m):,} of {len(fn):,} functions ({100 * len(m) / len(fn):.1f}%), "
      f"{b / 1e6:.2f} MB of {total / 1e6:.2f} MB ({100 * b / total:.1f}%), gate {b / 1024:,.1f} KB")
EOF
