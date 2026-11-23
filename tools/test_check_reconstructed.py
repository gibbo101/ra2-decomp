"""Assembly under src/ra2asm/ is marked RECONSTRUCTED, checked as written, and never MATCHes."""
import contextlib
import io
from pathlib import Path
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import check
from coff import CODE, Section, Symbol


class ReconstructedTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix="check-ra2asm-test-")
        self.root = Path(self.tmp.name)
        self.ra2asm = self.root / "ra2asm"
        self.ra2asm.mkdir()
        stack = contextlib.ExitStack()
        stack.enter_context(patch.object(check, "RA2ASM", self.ra2asm))
        self.addCleanup(stack.close)
        self.addCleanup(self.tmp.cleanup)

    def write(self, path, text):
        path.write_text(text)
        return path

    def test_reconstructed_markers_are_read_only_under_ra2asm(self):
        text = ("; RECONSTRUCTED: GAMEMD 0x00401000\nFirst proc C\n"
                "; FUNCTION: GAMEMD 0x00401010\nSecond proc C\n"
                "; RECONSTRUCTED: GAMEMD 0x00401020\n?Third@@YAXXZ proc\n")
        inside = self.write(self.ra2asm / "x.asm", text)
        outside = self.write(self.root / "x.asm", text)
        self.assertEqual(check.annotations(inside), [(0x401000, "First"), (0x401020, "=?Third@@YAXXZ")])
        self.assertEqual(check.annotations(outside), [(0x401010, "Second")])

    def test_function_marker_in_a_reconstruction_is_refused(self):
        src = self.write(self.ra2asm / "x.asm", "; FUNCTION: GAMEMD 0x00401000\nFirst proc C\n")
        with patch.object(check.subprocess, "run") as run:
            obj, log = check.assemble_source(src, self.root / "x.obj")
        self.assertIsNone(obj)
        self.assertIn("never FUNCTION", log)
        run.assert_not_called()

    def test_reconstruction_outside_ra2asm_must_still_copy_the_archive(self):
        src = self.write(self.root / "x.asm", "; RECONSTRUCTED: GAMEMD 0x00401000\nFirst proc C\n")
        with patch.object(check, "archived_asm", return_value=None), \
                patch.object(check.subprocess, "run") as run:
            obj, log = check.assemble_source(src, self.root / "x.obj")
        self.assertIsNone(obj)
        self.assertIn("not a copy", log)
        run.assert_not_called()

    def test_unmarked_procs_join_the_marked_routine_before_them(self):
        sec = Section(1, ".text", bytes(30), CODE)
        symbols = [Symbol(0, "_First", 0, 1, 0x20, 2), Symbol(1, "_Helper", 10, 1, 0x20, 2),
                   Symbol(2, "_Second", 20, 1, 0x20, 2)]
        obj = SimpleNamespace(functions=lambda: iter([(symbols[0], sec, 0, 10), (symbols[1], sec, 10, 20),
                                                      (symbols[2], sec, 20, 30)]))
        check.merge_unmarked(obj, [(0x401000, "First"), (0x401020, "Second")])
        self.assertEqual([(s.name, a, b) for s, _, a, b in obj.__dict__["_functions"]],
                         [("_First", 0, 20), ("_Second", 20, 30)])

    def report(self, src, matched, approved=frozenset({0x401000})):
        result = check.Result(0x401000, "First", 1, 1, matched, "_First")
        output = io.StringIO()
        with contextlib.ExitStack() as stack:
            for name, value in {"annotations": [(0x401000, "First")], "CoffObject": object(),
                                "merge_unmarked": None, "bind_annotations": ({}, []),
                                "function_symbols": set(), "vftable_holders": {},
                                "class_problem": None, "mark_asm_object_starts": None,
                                "approved_reconstructions": set(approved)}.items():
                stack.enter_context(patch.object(check, name, return_value=value))
            stack.enter_context(patch.object(check, "compare", return_value=result))
            stack.enter_context(contextlib.redirect_stdout(output))
            code = check.report(src, b"", SimpleNamespace(rtti=lambda: None), {},
                                [(0x401000, "First")], summary=True)
        return code, output.getvalue()

    def test_a_reconstruction_that_agrees_prints_reconstructed_not_match(self):
        code, out = self.report(self.ra2asm / "x.asm", True)
        self.assertEqual(code, 0)
        self.assertIn(": RECONSTRUCTED (1/1 bytes)", out)
        self.assertNotIn("MATCH", out)

    def test_an_unapproved_reconstruction_fails(self):
        code, out = self.report(self.ra2asm / "x.asm", True, approved=())
        self.assertEqual(code, 1)
        self.assertIn("needs the maintainer's approval", out)
        self.assertNotIn("RECONSTRUCTED (", out)

    def test_an_archive_copy_still_prints_match(self):
        code, out = self.report(self.root / "x.asm", True)
        self.assertEqual(code, 0)
        self.assertIn(": MATCH (1/1 bytes)", out)

    def test_a_reconstruction_that_differs_fails(self):
        code, out = self.report(self.ra2asm / "x.asm", False)
        self.assertEqual(code, 1)
        self.assertNotIn("RECONSTRUCTED", out)


if __name__ == "__main__":
    unittest.main()
