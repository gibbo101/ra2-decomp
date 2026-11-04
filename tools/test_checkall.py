"""Exercise the merge gate with real child processes and synthetic checker output."""

import contextlib
import csv
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import checkall


class MergeGateTests(unittest.TestCase):
    def setUp(self):
        self.root = Path(tempfile.mkdtemp(prefix="checkall-test-"))
        for directory in ("src", "tools", "data"):
            (self.root / directory).mkdir()
        self.files = []
        self.write("tools/check.py", """import json, pathlib, sys
assert sys.argv[2:] == ['--summary']
case = json.loads(pathlib.Path(sys.argv[1]).read_text())
print(case['stdout'], end='')
print(case.get('stderr', ''), end='', file=sys.stderr)
sys.exit(case['exit'])
""")
        self.progress = self.root / "data/progress.csv"
        self.write("data/progress.csv", "address,name,result,size,source,symbol\n"
                   "0x00401000,First,MATCH,1,src/example.cpp,_First\n"
                   "0x00401010,Second,MATCH,1,src/example.cpp,_Second\n")

    def tearDown(self):
        for path in self.files:
            path.unlink(missing_ok=True)
        for directory in ("src", "tools", "data"):
            (self.root / directory).rmdir()
        self.root.rmdir()

    def write(self, relative, text):
        path = self.root / relative
        path.write_text(text)
        if path not in self.files:
            self.files.append(path)

    def child(self, output, exit_code=0, name="example.cpp", stderr=""):
        self.write(f"src/{name}", json.dumps(dict(stdout=output, exit=exit_code, stderr=stderr)))

    def match(self, address="0x00401000", name="First"):
        return f"{address} {name}: MATCH (1/1 bytes) _{name}\n"

    def complete(self, count, failures=0):
        return f"CHECK-SUMMARY: checked={count} failed={failures}\n"

    def baseline(self):
        return self.match() + self.match("0x00401010", "Second")

    def run_gate(self, record=False, baseline=None):
        original = self.progress.read_bytes()
        output = io.StringIO()
        argv = ["checkall.py", *(["--record"] if record else []), *(["--baseline", baseline] if baseline else [])]
        with patch.object(checkall, "ROOT", self.root), \
             patch.object(checkall, "PROGRESS", self.progress), \
             patch.object(checkall.sys, "argv", argv), contextlib.redirect_stdout(output):
            with self.assertRaises(SystemExit) as result:
                checkall.main()
        if result.exception.code != 0:
            self.assertEqual(original, self.progress.read_bytes())
        return result.exception.code, output.getvalue()

    def test_all_prior_matches_pass_and_record(self):
        self.child(self.baseline() + self.complete(2))
        code, _ = self.run_gate(record=True)
        self.assertEqual(code, 0)
        with self.progress.open() as stream:
            self.assertEqual(len(list(csv.DictReader(stream))), 2)

    def test_disappearing_match_fails_without_rewriting_progress(self):
        self.child(self.match() + self.complete(1))
        code, output = self.run_gate(record=True)
        self.assertEqual(code, 1)
        self.assertIn("0x00401010: previously matched function missing", output)

    def test_base_protects_match_removed_from_working_table(self):
        table = self.progress.read_text()
        self.progress.write_text(table.splitlines(True)[0] + table.splitlines(True)[1])
        self.child(self.match() + self.complete(1))
        with patch.object(checkall.subprocess, "check_output", side_effect=["a" * 40, table]) as git:
            code, output = self.run_gate(record=True, baseline="upstream/main")
        self.assertEqual(code, 1)
        self.assertIn("0x00401010: previously matched function missing", output)
        self.assertEqual(git.call_args.args[0][-1], "a" * 40 + ":data/progress.csv")

    def test_duplicate_across_sources_fails_without_recording(self):
        self.child(self.baseline() + self.complete(2))
        self.child(self.match(name="Other") + self.complete(1), name="other.cpp")
        code, out = self.run_gate(record=True)
        self.assertEqual(code, 1)
        self.assertIn("duplicate retail address: src/example.cpp _First, src/other.cpp _Other", out)
        self.assertIn("2 of 2 functions match (2 of 2 bytes)", out)

    def test_acknowledged_pair_counts_once_and_does_not_hide_a_third_owner(self):
        for module, symbol in checkall.SHARED["0x004F2C50"]:
            self.child(f"0x004F2C50 Init: MATCH (23/23 bytes) {symbol}\n" + self.complete(1),
                       name=Path(module).name)
        self.child(self.baseline() + self.complete(2))
        code, out = self.run_gate(record=True)
        self.assertEqual(code, 0)
        self.assertIn("3 of 3 functions match (25 of 25 bytes)", out)
        with self.progress.open() as f:
            self.assertEqual(len(list(csv.DictReader(f))), 4)
        self.child("0x004F2C50 Init: MATCH (23/23 bytes) _Third\n" + self.complete(1), name="extra.cpp")
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_deleted_source_fails_without_rewriting_progress(self):
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_matches_may_move_between_sources(self):
        self.child(self.match() + self.complete(1), name="moved.cpp")
        self.child(self.match("0x00401010", "Second") + self.complete(1), name="second.asm")
        self.assertEqual(self.run_gate(record=True)[0], 0)

    def test_new_partial_is_allowed_and_recorded(self):
        partial = "0x00401020 Third: 82.1% (3/4 bytes) _Third\n"
        self.child(self.baseline() + partial + self.complete(3, 1), exit_code=1)
        self.assertEqual(self.run_gate(record=True)[0], 0)
        self.assertIn("82.1%", self.progress.read_text())

    def test_previous_match_regressing_fails(self):
        partial = "0x00401010 Second: 82.1% (3/4 bytes) _Second\n"
        self.child(self.match() + partial + self.complete(2, 1), exit_code=1)
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_crash_after_matches_fails(self):
        self.child(self.baseline(), exit_code=1, stderr="synthetic traceback\n")
        code, output = self.run_gate(record=True)
        self.assertEqual(code, 1)
        self.assertIn("synthetic traceback", output)

    def test_crash_after_new_partial_is_not_a_completed_partial_run(self):
        self.child(self.baseline() + "0x00401020 Third: 82.1% (3/4 bytes) _Third\n", exit_code=1)
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_error_exit_with_success_summary_fails(self):
        self.child(self.baseline() + self.complete(2), exit_code=1)
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_nonstandard_exit_with_complete_output_fails(self):
        self.child(self.baseline() + self.complete(2), exit_code=137)
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_zero_exit_without_completion_fails(self):
        self.child(self.baseline())
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_missing_result_from_completed_run_fails(self):
        self.child(self.baseline() + self.complete(3))
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_multiple_completion_records_fail(self):
        self.child(self.baseline() + self.complete(2) + self.complete(2))
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_unaccounted_failure_fails(self):
        self.child(self.baseline() + self.complete(2, 1), exit_code=1)
        self.assertEqual(self.run_gate(record=True)[0], 1)

    def test_warnings_remain_nonfatal(self):
        warning = "0x00401000 First: warning: synthetic class warning\n"
        self.child(self.baseline() + warning + self.complete(2))
        self.assertEqual(self.run_gate(record=True)[0], 0)

    def test_duplicate_claim_remains_fatal(self):
        diagnostic = "0x00401020 Third: same compiled function as 0x00401000 First\n"
        self.child(diagnostic + self.baseline() + self.complete(2, 1), exit_code=1)
        code, output = self.run_gate(record=True)
        self.assertEqual(code, 1)
        self.assertIn(diagnostic.strip(), output)

    def test_wrong_template_class_remains_fatal(self):
        diagnostic = "0x00401000 First: a slot of Other in the exe, not of First\n"
        self.child(self.baseline() + diagnostic + self.complete(2, 1), exit_code=1)
        code, output = self.run_gate(record=True)
        self.assertEqual(code, 1)
        self.assertIn(diagnostic.strip(), output)

    def test_new_wrong_target_is_a_completed_partial(self):
        partial = "0x00401020 Third: WRONG TARGETS (4/4 bytes) _Third\n"
        self.child(self.baseline() + partial + self.complete(3, 1), exit_code=1)
        self.assertEqual(self.run_gate(record=True)[0], 0)
        self.assertIn("WRONG TARGETS", self.progress.read_text())


    def test_lost_reference_cannot_be_offset_by_a_new_one(self):
        import os
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            exe = self.root / "orig/gamemd.exe"
            exe.parent.mkdir()
            exe.write_bytes(b"fixture")

            path = str(Path(tmp) / "base.json")
            def output(site):
                return (self.match() + "CHECK-REFERENCES 00401000 " + site + "\n"
                        + self.match("0x00401010", "Second") + "CHECK-REFERENCES 00401010 \n"
                        + self.complete(2))
            self.child(output("1/6/0/405000"))
            with patch.dict(os.environ, RA2_SAVE_REFERENCES=path):
                self.assertEqual(self.run_gate()[0], 0)
            with patch.dict(os.environ, RA2_REQUIRE_REFERENCES=path):
                self.assertEqual(self.run_gate()[0], 0)
                self.child(output("2/6/0/406000"))
                code, log = self.run_gate(record=True)
                self.assertEqual(code, 1)
                self.assertIn("lost proven reference 00401000 1/6/0/405000", log)
            exe.unlink()
            exe.parent.rmdir()


if __name__ == "__main__":
    unittest.main()
