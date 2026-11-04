#!/bin/bash
# Publish a verified round, then report the new totals.
set -eu
cd "$(git rev-parse --show-toplevel)"
uv run tools/coord/land.py "$@"
echo "### untracked files outside build/"
git status --short --untracked-files=all | grep '^??' | grep -v '^?? build/' || true
echo "### game totals"
python3 - <<'EOF'
import csv
lib = min(int(r["address"], 16) for r in csv.DictReader(open("data/library.csv")))
fn = {int(r["address"], 16): int(r["size"]) for r in csv.DictReader(open("data/functions.csv")) if int(r["address"], 16) < lib}
m = [int(r["address"], 16) for r in csv.DictReader(open("data/progress.csv")) if r["result"] == "MATCH"]
m = {a for a in m if a in fn}
b, total = sum(fn[a] for a in m), sum(fn.values())
print(f"{len(m):,} of {len(fn):,} functions ({100 * len(m) / len(fn):.1f}%), "
      f"{b / 1e6:.2f} MB of {total / 1e6:.2f} MB ({100 * b / total:.1f}%), gate {b / 1024:,.1f} KB")
EOF
