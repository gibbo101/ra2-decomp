"""Re-check every annotated function under src/, C++ and assembly: the merge gate.

Usage: uv run tools/checkall.py [--record]

Prints one line per function. Fails if a function data/progress.csv records as MATCH no
longer matches, since a header edit can change the code of functions it never touches,
or if two annotations in one file claim the same compiled function.
--record rewrites data/progress.csv with the results.
"""

import csv
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PROGRESS = ROOT / "data/progress.csv"
LINE = re.compile(r"^(0x[0-9A-F]{8}) (.+?): (MATCH|WRONG TARGETS|[\d.]+%) \((\d+)/(\d+) bytes[^)]*\) (\S+)")
DUPLICATE = re.compile(r"^(0x[0-9A-F]{8}) .+: same compiled function as ")


def main() -> None:
    record = "--record" in sys.argv[1:]
    before = {}
    if PROGRESS.exists():
        with PROGRESS.open() as f:
            before = {r["address"]: r["result"] for r in csv.DictReader(f)}
    sources = sorted(f for ext in ("*.cpp", "*.asm") for f in (ROOT / "src").rglob(ext))
    rows, broken = [], []
    for src in sources:
        proc = subprocess.run([sys.executable, str(ROOT / "tools/check.py"), str(src)],
                              capture_output=True, text=True, cwd=ROOT)
        found = False
        for line in proc.stdout.splitlines():
            if DUPLICATE.match(line):
                print(line)
                broken.append(line)
                continue
            m = LINE.match(line)
            if not m:
                continue
            found = True
            print(line)
            addr, name, result, size, symbol = m.group(1), m.group(2), m.group(3), m.group(5), m.group(6)
            rows.append([addr, name, result, size, src.relative_to(ROOT).as_posix(), symbol])
            if before.get(addr) == "MATCH" and result != "MATCH":
                broken.append(f"{addr} {name}")
        if not found:
            print(f"{src.relative_to(ROOT)}: {(proc.stdout + proc.stderr).strip()}")
            broken.append(str(src.relative_to(ROOT)))
    matched = [r for r in rows if r[2] == "MATCH"]
    print(f"{len(matched)} of {len(rows)} functions match "
          f"({sum(int(r[3]) for r in matched)} of {sum(int(r[3]) for r in rows)} bytes)")
    if broken:
        print("no longer matching: " + ", ".join(broken))
    if record and not broken:
        with PROGRESS.open("w", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            w.writerow(["address", "name", "result", "size", "source", "symbol"])
            w.writerows(rows)
    sys.exit(1 if broken else 0)


if __name__ == "__main__":
    main()
