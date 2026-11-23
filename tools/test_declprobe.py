"""declprobe builds unused declarations of one kind, places them by marker and probes a copy."""
import contextlib
import io
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

import declprobe

LINES = ["#include \"x.h\"", "", "// FUNCTION: GAMEMD 0x00401000", "void A(void) {}",
         "", "// FUNCTION: GAMEMD 0x00401010", "void B(void) {}"]


class DeclProbeTests(unittest.TestCase):
    def test_one_line_per_declaration(self):
        self.assertEqual(declprobe.decls("var", 2), ["extern int probe0;", "extern int probe1;"])
        self.assertEqual(declprobe.decls("func", 1), ["void probe0(void);"])
        self.assertEqual(declprobe.decls("var", 0), [])

    def test_members_share_one_struct(self):
        self.assertEqual(declprobe.decls("mem", 2), ["struct probeStruct {", "\tint probe0;", "\tint probe1;", "};"])
        self.assertEqual(declprobe.decls("mem", 0), [])

    def test_positions(self):
        self.assertEqual(declprobe.position(LINES, 0x401010, "before"), 5)
        self.assertEqual(declprobe.position(LINES, 0x401000, "after"), 5)
        self.assertEqual(declprobe.position(LINES, 0x401010, "after"), len(LINES))
        self.assertEqual(declprobe.position(LINES, 0x401000, "head"), 0)
        self.assertEqual(declprobe.position(LINES, 0x401000, "line:3"), 2)

    def test_outcome_fingerprints_only_mismatches(self):
        self.assertEqual(declprobe.outcome("0x00401000 A: MATCH (1/1 bytes) _A\n"), "MATCH")
        mismatch = declprobe.outcome("0x00401000 A: 98.0% (5/5 bytes) _A\n-mov eax, 1\n+mov ecx, 1\n")
        self.assertTrue(mismatch.startswith("98.0% "))
        self.assertEqual(declprobe.outcome("error\n"), "FAIL")


class DeclProbeCopyTests(unittest.TestCase):
    """main() writes probes only into copies under its own build directory."""

    SOURCE = "\n".join(LINES)
    HEADER = "#pragma once\nstruct X {\n};\n"

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix="declprobe-test-")
        self.root = Path(self.tmp.name)
        (self.root / "src").mkdir()
        (self.root / "include/sub").mkdir(parents=True)
        self.src = self.root / "src/a.cpp"
        self.header = self.root / "include/sub/x.h"
        self.src.write_text(self.SOURCE)
        self.header.write_text(self.HEADER)
        self.out = self.root / "build/declprobe"
        self.seen = []

    def tearDown(self):
        self.tmp.cleanup()

    def fake_output(self, src, only, overlay, orig, names, obj):
        probe = overlay / "sub/x.h" if overlay else src
        self.seen.append((src, overlay, obj, probe.read_text()))
        return "0x00401010 B: MATCH (1/1 bytes) _B\n"

    def probe(self, *args):
        printed = io.StringIO()
        with contextlib.ExitStack() as stack:
            stack.enter_context(patch.object(declprobe, "ROOT", self.root))
            stack.enter_context(patch.object(declprobe, "OUT", self.out))
            stack.enter_context(patch.object(declprobe.check, "Original", return_value=None))
            stack.enter_context(patch.object(declprobe.check, "known_names", return_value={}))
            stack.enter_context(patch.object(declprobe.check, "output", side_effect=self.fake_output))
            stack.enter_context(patch("sys.argv", ["declprobe.py", "src/a.cpp", "0x00401010", *args]))
            stack.enter_context(contextlib.redirect_stdout(printed))
            declprobe.main()
        return printed.getvalue()

    def test_source_probe_compiles_a_copy(self):
        printed = self.probe("--counts", "0,2")
        self.assertEqual(printed, "var x0\tMATCH\nvar x2\tMATCH\n")
        self.assertEqual([s[0] for s in self.seen], [self.out / "a.cpp"] * 2)
        self.assertNotIn("probe0", self.seen[0][3])
        self.assertIn("extern int probe1;\n// FUNCTION: GAMEMD 0x00401010", self.seen[1][3])
        self.assertEqual(self.src.read_text(), self.SOURCE)
        self.assertFalse((self.out / "a.cpp").exists())

    def test_header_probe_goes_in_an_overlay(self):
        self.probe("--into", "include/sub/x.h", "--pos", "head", "--counts", "1")
        src, overlay, _, text = self.seen[0]
        self.assertEqual((src, overlay), (self.src, self.out / "hdr"))
        self.assertTrue(text.startswith("extern int probe0;\n#pragma once"))
        self.assertEqual(self.header.read_text(), self.HEADER)
        self.assertFalse((self.out / "hdr/sub/x.h").exists())

    def test_header_outside_include_is_refused(self):
        with self.assertRaises(SystemExit):
            self.probe("--into", "src/a.cpp", "--pos", "head")
        self.assertEqual(self.seen, [])

    def test_refuses_a_file_holding_probe_declarations(self):
        self.header.write_text("extern int probe3;\n" + self.HEADER)
        with self.assertRaises(SystemExit):
            self.probe("--into", "include/sub/x.h", "--pos", "head")
        self.src.write_text("struct probeStruct {\n};\n" + self.SOURCE)
        with self.assertRaises(SystemExit):
            self.probe()
        self.assertEqual(self.seen, [])


if __name__ == "__main__":
    unittest.main()
