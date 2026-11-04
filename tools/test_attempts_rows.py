import csv
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


class JournalTests(unittest.TestCase):
    def test_lower_score_does_not_replace_best_blocker(self):
        script = Path(__file__).parent / "coord/attempts_rows.py"
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            data = root / "data"
            data.mkdir()
            (data / "functions.csv").write_text("address\n0x00637270\n")
            (data / "progress.csv").write_text("address,result\n")
            attempts = data / "attempts.csv"
            attempts.write_text("date,address,function,model,result,best,checks,minutes,blocker,notes\n")
            with (root / "journal").open("w") as journal:
                for score, blocker in ((95.9, "stack"), (81.4, "inline")):
                    target = dict(address="0x00637270", result="partial", best_percent=score,
                                  checks=3, blocker=blocker, note=blocker + " candidate")
                    journal.write(json.dumps({"type": "result", "result": {"functions": [target]}}) + "\n")
            subprocess.run([sys.executable, script, "2026-11-04", "journal"], cwd=root, check=True, capture_output=True)
            journal = root / "journal"
            later = dict(address="0x00637270", result="partial", best_percent=97.4, checks=4,
                         blocker="remaining stores", note="changed candidate")
            second = root / "second"
            second.write_text(json.dumps({"type": "result", "result": {"functions": [later]}}) + "\n")
            subprocess.run([sys.executable, script, "2026-11-04", str(second)], cwd=root, check=True, capture_output=True)
            before = attempts.read_bytes()
            subprocess.run([sys.executable, script, "2026-11-04", str(second), str(journal)], cwd=root,
                           check=True, capture_output=True)
            self.assertEqual(attempts.read_bytes(), before)
            with attempts.open() as f:
                row, = csv.DictReader(f)
            self.assertEqual((row["best"], row["blocker"], row["notes"], row["checks"]),
                             ("97.4", "remaining stores", "changed candidate", "10"))
