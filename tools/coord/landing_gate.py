"""Verify the checkout and stamp a successful gate log for land.sh."""
import argparse
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from landing_inputs import fingerprint


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--receipt", type=Path, help="recheck an existing build receipt")
    parser.add_argument("--baseline", help="protect the matches of this revision when supported by the checker")
    args = parser.parse_args()
    stamp = fingerprint(ROOT)
    checker = [ROOT / "tools/checkall.py"]
    if args.receipt:
        checker = [ROOT / "tools/build_receipt.py", "recheck", args.receipt.resolve()]
    if args.baseline:
        checker.extend(("--baseline", args.baseline))
    subprocess.run([sys.executable, *checker], cwd=ROOT, check=True)
    if fingerprint(ROOT) != stamp:
        sys.exit("checkout inputs changed during the landing gate")
    print("LANDING-GATE " + stamp)


if __name__ == "__main__":
    main()
