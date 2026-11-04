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

    def test_adopt_complete_legacy_aggregate_before_counting_new_checks(self):
        from coord.attempts_rows import merge
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            table, journal = root / "table", root / "journal"
            a, b = "0x00565C10", "0x00468670"
            header = "date,address,function,model,result,best,checks,minutes,blocker,notes\n"
            legacy = header + f"round,{a},,model,partial,99.7,106,,last,\nround,{b},,model,partial,98.9,3736,,last,\n"
            table.write_text(legacy)
            def observation(addr, score, checks, blocker):
                return dict(address=addr, result="partial", best_percent=score, checks=checks, blocker=blocker)
            first = [observation(a, 99.7, 99, "best"), observation(b, 98.9, 3735, "best")]
            last = [observation(a, 95, 7, "last"), observation(b, 95, 1, "last")]
            def run(items):
                journal.write_text(json.dumps({"type": "result", "result": {"functions": items}}))
                merge(table, "round", [journal], {a, b}, set())
            run(last)
            self.assertEqual(table.read_text(), legacy)
            run(first + last)
            with table.open() as f:
                rows = list(csv.DictReader(f))
            self.assertEqual([r["checks"] for r in rows], ["106", "3736"])
            self.assertTrue(all(len(r["attempt_ids"].split()) == 2 for r in rows))
            before = table.read_bytes()
            run(first + last)
            self.assertEqual(table.read_bytes(), before)
            run([observation(a, 99.8, 2, "new")])
            with table.open() as f:
                self.assertEqual(next(csv.DictReader(f))["checks"], "108")

    def test_ragged_rows_are_rejected_before_writing_and_legacy_rows_get_full_width(self):
        from coord.attempts_rows import merge
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            table, journal = root / "table", root / "journal"
            data = "date,address,function,model,result,best,checks,minutes,blocker,notes\n"
            row = "old,0x00000010,,model,partial,80,2,,block,note"
            for bad in (row + ",extra", row.rsplit(",", 1)[0]):
                table.write_text(data + bad + "\n")
                before = table.read_bytes()
                with self.assertRaisesRegex(ValueError, "expected 10 columns"):
                    merge(table, "new", [], set(), set())
                self.assertEqual(table.read_bytes(), before)
            table.write_text(data + row + "\n")
            item = dict(address="0x10", best_percent=90, checks=2, result="partial")
            journal.write_text(json.dumps({"type": "result", "result": {"functions": [item]}}))
            merge(table, "new", [journal], {"0x00000010"}, set())
            with table.open() as f:
                self.assertEqual([len(r) for r in csv.reader(f)], [12, 12, 12])
