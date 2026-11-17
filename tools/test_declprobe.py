"""declprobe builds unused declarations of one kind and places them by marker."""
import unittest

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


if __name__ == "__main__":
    unittest.main()
