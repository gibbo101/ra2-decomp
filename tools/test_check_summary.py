"""Completion records distinguish a finished comparison from interrupted output."""

import contextlib
import io
from pathlib import Path
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import check


class CompletionTests(unittest.TestCase):
    def setUp(self):
        self.root = Path(tempfile.mkdtemp(prefix="check-summary-test-"))
        self.obj = self.root / "fixture.obj"
        self.obj.write_bytes(b"")

    def tearDown(self):
        self.obj.unlink()
        self.root.rmdir()

    def invoke(self, results, summary=True):
        output = io.StringIO()
        argv = ["check.py", "fixture.cpp", *(["--summary"] if summary else [])]
        with contextlib.ExitStack() as stack:
            for name, value in {
                "annotations": [(0x401000, "First"), (0x401010, "Second")],
                "compile_source": (self.obj, ""), "CoffObject": object(),
                "Original": SimpleNamespace(rtti=lambda: None), "known_names": {},
                "bind_annotations": ({}, []),
                "function_symbols": set(), "vftable_holders": {}, "class_problem": None,
            }.items():
                stack.enter_context(patch.object(check, name, return_value=value))
            stack.enter_context(patch.object(check, "compare", side_effect=results))
            stack.enter_context(patch.object(check.sys, "argv", argv))
            stack.enter_context(contextlib.redirect_stdout(output))
            try:
                check.main()
            except (SystemExit, RuntimeError) as result:
                return result, output.getvalue()
        self.fail("checker did not exit")

    def result(self, matched=True):
        return check.Result(0x401000, "First", 1, 1, matched, "_First")

    def test_success_emits_completion_and_zero_exit(self):
        result, output = self.invoke([self.result(), self.result()])
        self.assertEqual(result.code, 0)
        self.assertTrue(output.endswith("CHECK-SUMMARY: checked=2 failed=0\n"))

    def test_partial_emits_completion_and_one_exit(self):
        result, output = self.invoke([self.result(), self.result(False)])
        self.assertEqual(result.code, 1)
        self.assertTrue(output.endswith("CHECK-SUMMARY: checked=2 failed=1\n"))

    def test_interrupted_comparison_does_not_emit_completion(self):
        result, output = self.invoke([self.result(), RuntimeError("synthetic interruption")])
        self.assertIsInstance(result, RuntimeError)
        self.assertIn("MATCH", output)
        self.assertNotIn("CHECK-SUMMARY", output)

    def test_ordinary_output_remains_unchanged_without_flag(self):
        result, output = self.invoke([self.result(), self.result()], summary=False)
        self.assertEqual(result.code, 0)
        self.assertNotIn("CHECK-SUMMARY", output)


if __name__ == "__main__":
    unittest.main()
