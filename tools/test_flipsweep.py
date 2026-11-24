"""flipsweep builds numbered probe statements, places them in the target's body and compares diff sets."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

try:
    import flipsweep
except ImportError:  # capstone or pefile missing
    flipsweep = None

LINES = ["#include \"x.h\"", "", "// FUNCTION: GAMEMD 0x00401000", "int A(int a)", "{",
         "\tint b = a;", "\tif (b) {", "\t\tb++;", "\t}", "\treturn b;", "}"]


@unittest.skipIf(flipsweep is None, "capstone and pefile are needed")
class FlipSweepTests(unittest.TestCase):
    def test_copies_are_numbered_from_zero(self):
        self.assertEqual(flipsweep.copies(flipsweep.KINDS["local"], 2),
                         ["\tint flip_local_0 = 0;", "\tint flip_local_1 = 1;"])
        self.assertEqual(flipsweep.copies(flipsweep.KINDS["store"], 1), ["\tFlip_Probe_Store_[0] = 0x7F00 + 0;"])
        self.assertEqual(flipsweep.copies(flipsweep.KINDS["call"], 0), [])

    def test_free_form_statement_keeps_its_braces(self):
        kind = flipsweep.kind_of("stmt", "if (x) { y[{k}] = 1; }", ["extern int y[];"])
        self.assertEqual(flipsweep.copies(kind, 1), ["\tif (x) { y[0] = 1; }"])
        self.assertEqual(kind.decls, ("extern int y[];",))
        with self.assertRaises(SystemExit):
            flipsweep.kind_of("stmt", None)

    def test_declarations_go_above_the_first_marker_once(self):
        lines, added = flipsweep.with_decls(LINES, ["extern int p;", "extern int p;", "int q;"])
        self.assertEqual(added, 2)
        self.assertEqual(lines[2:4], ["extern int p;", "int q;"])
        self.assertEqual(lines[4], LINES[2])

    def test_positions(self):
        span = (4, 10)
        self.assertEqual(flipsweep.position(LINES, span, "start"), 5)
        self.assertEqual(flipsweep.position(LINES, span, "end"), 9)
        self.assertEqual(flipsweep.position(LINES, span, "line:7"), 6)
        self.assertEqual(flipsweep.position(LINES, span, "line:7", shift=2), 8)
        self.assertEqual(flipsweep.position(LINES, span, r"before:if \(b\)"), 6)
        self.assertEqual(flipsweep.position(LINES, span, r"after:if \(b\)"), 7)
        with self.assertRaises(SystemExit):
            flipsweep.position(LINES, span, "after:nothing here")

    def test_insert_all_keeps_block_order_at_one_index(self):
        out = flipsweep.insert_all(["a", "b"], [(1, ["x"]), (1, ["y"]), (0, ["z"])])
        self.assertEqual(out, ["z", "a", "x", "y", "b"])

    def test_diff_set_counts_lines_and_drops_ignored_additions(self):
        diff = ["--- original", "+++ ours", "@@ -1 +1 @@", "-mov eax, 1", "+mov ecx, 1",
                "+mov ecx, 1", "+mov dword ptr [<addr>], 0x7f01", "-mov dword ptr [<addr>], 0x7f02"]
        self.assertEqual(flipsweep.diff_set(diff, r"\b0x7f[0-9a-f]{2}\b"),
                         (("+mov ecx, 1", 2), ("-mov dword ptr [<addr>], 0x7f02", 1), ("-mov eax, 1", 1)))
        self.assertEqual(flipsweep.diff_set([]), ())

    def test_letters_name_sets_in_order(self):
        tags = flipsweep.letters({0: ("a",), 1: ("a",), 2: ("b",), 3: ("a",), 4: ()})
        self.assertEqual(tags, {0: "A", 1: "A", 2: "B", 3: "A", 4: "C"})


if __name__ == "__main__":
    unittest.main()
