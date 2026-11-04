"""Prepare a round's generated tables and publish them after verifying the gate."""
import argparse
from collections import Counter
import csv
import fcntl
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from checkall import LINE
from landing_inputs import fingerprint


def check_log(root, path):
    lines = path.read_text().splitlines()
    stamp = "LANDING-GATE " + fingerprint(root)
    if not lines or lines[-1] != stamp or lines.count(stamp) != 1:
        raise ValueError("landing requires the completed final gate for this checkout")
    with (root / "data/progress.csv").open() as f:
        rows = list(csv.DictReader(f))
    expected = Counter((r["address"], r["name"], r["result"], r["size"], r["symbol"]) for r in rows)
    found = Counter(m.group(1, 2, 3, 5, 6) for line in lines if (m := LINE.match(line)))
    matched = [r for r in rows if r["result"] == "MATCH"]
    summary = (f"{len(matched)} of {len(rows)} functions match "
               f"({sum(int(r['size']) for r in matched)} of {sum(int(r['size']) for r in rows)} bytes)")
    if not rows or found != expected or len(lines) < 2 or lines[-2] != summary:
        raise ValueError("gate results do not cover the recorded progress table")
    return stamp


def generate(root, stage, date, journals):
    data = stage / "data"
    data.mkdir()
    for name in ("functions.csv", "progress.csv"):
        (data / name).symlink_to(root / "data" / name)
    shutil.copy2(root / "data/attempts.csv", data / "attempts.csv")
    subprocess.run([sys.executable, root / "tools/coord/attempts_rows.py", date, *journals], cwd=stage, check=True)
    for name in ("modules", "manifest"):
        subprocess.run([sys.executable, root / f"tools/{name}.py", "--out", data / f"{name}.csv"],
                       cwd=root, check=True)


def publish(root, stage):
    replaced = []
    try:
        for name in ("attempts.csv", "modules.csv", "manifest.csv"):
            dest = root / "data" / name
            backup = stage / (name + ".old")
            shutil.copy2(dest, backup)
            os.replace(stage / "data" / name, dest)
            replaced.append((backup, dest))
    except BaseException:
        for backup, dest in reversed(replaced):
            os.replace(backup, dest)
        raise


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("date")
    parser.add_argument("gate", type=Path)
    parser.add_argument("journals", nargs="+", type=Path)
    args = parser.parse_args()
    journals = [p.resolve(strict=True) for p in args.journals]
    gate = args.gate.resolve(strict=True)
    data = ROOT / "data"
    lock = os.open(data, os.O_RDONLY)
    try:
        fcntl.flock(lock, fcntl.LOCK_EX)
        stamp = check_log(ROOT, gate)
        subprocess.run(["bash", "tools/coord/landscan.sh"], cwd=ROOT, check=True)
        subprocess.run([sys.executable, "tools/coord/tally.py", "--gate", gate, *journals], cwd=ROOT, check=True)
        with tempfile.TemporaryDirectory(dir=data) as tmp:
            stage = Path(tmp)
            generate(ROOT, stage, args.date, journals)
            if "LANDING-GATE " + fingerprint(ROOT) != stamp:
                raise ValueError("checkout changed while preparing the landing")
            publish(ROOT, stage)
        subprocess.run([sys.executable, "tools/coord/stale.py"], cwd=ROOT, check=True)
    finally:
        os.close(lock)


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, subprocess.CalledProcessError) as exc:
        sys.exit(str(exc))
