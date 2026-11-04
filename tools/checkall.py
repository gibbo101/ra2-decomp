"""Re-check every annotated function under src/, C++ and assembly: the merge gate.

Usage: uv run tools/checkall.py [--record] [--baseline COMMIT]

Prints one line per function. Fails if a function data/progress.csv records as MATCH no
longer matches or disappears, since a header edit can change the code of functions it never touches,
if two annotations in one file claim the same compiled function, or if a template
instance's label names a class whose vtables in the exe do not hold the function
(check.py's class check). The class check's warnings are counted.
An incomplete checker run fails; a completed run containing new partials is allowed.
--record rewrites data/progress.csv with the results.
"""

import argparse
import csv
import io
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PROGRESS = ROOT / "data/progress.csv"
LINE = re.compile(r"^(0x[0-9A-F]{8}) (.+?): (MATCH|WRONG TARGETS|[\d.]+%) \((\d+)/(\d+) bytes[^)]*\) (\S+)")
DUPLICATE = re.compile(r"^(0x[0-9A-F]{8}) .+: same compiled function as ")
WRONG_CLASS = re.compile(r"^(0x[0-9A-F]{8}) .+: a slot of .+ in the exe, not of ")
WARNING = re.compile(r"^(0x[0-9A-F]{8}) .+: warning: ")
SUMMARY = re.compile(r"^CHECK-SUMMARY: checked=(\d+) failed=(\d+)$")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--record", action="store_true")
    parser.add_argument("--baseline", metavar="COMMIT", help="protect matches from this revision's progress table")
    args = parser.parse_args()
    before = set()
    if args.baseline:
        try:
            commit = subprocess.check_output(["git", "rev-parse", "--verify", "--end-of-options",
                                              args.baseline + "^{commit}"], cwd=ROOT, text=True).strip()
            table = subprocess.check_output(["git", "show", commit + ":data/progress.csv"], cwd=ROOT, text=True)
        except subprocess.CalledProcessError:
            parser.exit(1, "could not load the requested baseline progress table\n")
        with io.StringIO(table) as f:
            before = {r["address"] for r in csv.DictReader(f) if r["result"] == "MATCH"}
    elif PROGRESS.exists():
        with PROGRESS.open() as f:
            before = {r["address"] for r in csv.DictReader(f) if r["result"] == "MATCH"}
    sources = sorted(f for ext in ("*.cpp", "*.asm") for f in (ROOT / "src").rglob(ext))
    rows, broken, warnings = [], [], 0
    for src in sources:
        proc = subprocess.run([sys.executable, str(ROOT / "tools/check.py"), str(src), "--summary"],
                              capture_output=True, text=True, cwd=ROOT)
        found = False
        checked, partials, summaries = 0, 0, []
        for line in proc.stdout.splitlines():
            summary = SUMMARY.fullmatch(line)
            if summary:
                summaries.append(tuple(map(int, summary.groups())))
                continue
            if WARNING.match(line):
                print(line)
                warnings += 1
                continue
            if DUPLICATE.match(line) or WRONG_CLASS.match(line):
                print(line)
                broken.append(line)
                continue
            m = LINE.match(line)
            if not m:
                continue
            found = True
            checked += 1
            print(line)
            addr, name, result, size, symbol = m.group(1), m.group(2), m.group(3), m.group(5), m.group(6)
            rows.append([addr, name, result, size, src.relative_to(ROOT).as_posix(), symbol])
            partials += result != "MATCH"
            if addr in before and result != "MATCH":
                broken.append(f"{addr} {name}")
        if (len(summaries) != 1 or summaries[0] != (checked, partials)
                or proc.returncode != (1 if partials else 0)):
            problem = f"{src.relative_to(ROOT)}: incomplete or failed checker run (exit {proc.returncode})"
            print(problem)
            if proc.stderr.strip():
                print(proc.stderr.strip())
            broken.append(problem)
        if not found:
            print(f"{src.relative_to(ROOT)}: {(proc.stdout + proc.stderr).strip()}")
            broken.append(str(src.relative_to(ROOT)))
    seen = {r[0] for r in rows}
    for addr in sorted(before - seen):
        problem = f"{addr}: previously matched function missing from results"
        print(problem)
        broken.append(problem)
    matched = [r for r in rows if r[2] == "MATCH"]
    if warnings:
        print(f"{warnings} class warnings: labels or stored vftables of a class the exe's RTTI does not name there")
    print(f"{len(matched)} of {len(rows)} functions match "
          f"({sum(int(r[3]) for r in matched)} of {sum(int(r[3]) for r in rows)} bytes)")
    if broken:
        print("no longer matching: " + ", ".join(broken))
    if args.record and not broken:
        with PROGRESS.open("w", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            w.writerow(["address", "name", "result", "size", "source", "symbol"])
            w.writerows(rows)
    sys.exit(1 if broken else 0)


if __name__ == "__main__":
    main()
