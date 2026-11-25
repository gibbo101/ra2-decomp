"""crossexe builds masked patterns, grades hits, finds function starts and lists callers."""
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

try:
    import crossexe
except ImportError:  # capstone or pefile missing
    crossexe = None

T0 = 0x401000

# mov eax, [ecx+0x10]; add eax, 5; call rel32 (relocated); ret
BODY = bytes.fromhex("8b4110" "83c005" "e800000000" "c3")
RELOCS = {7, 8, 9, 10}


def call(site: int, target: int) -> bytes:
    return b"\xe8" + struct.pack("<i", target - (site + 5))


def image(code: bytes, data: bytes = b"") -> "crossexe.Exe":
    """An Exe whose .text is code at 0x401000, followed by data."""
    return crossexe.Exe("t", 0x400000, b"\0" * 0x1000 + code + data, 0x1000, len(code))


@unittest.skipIf(crossexe is None, "capstone and pefile are needed")
class PatternTests(unittest.TestCase):
    def test_exact_masks_only_relocations(self):
        self.assertEqual(crossexe.masked_offsets(BODY, RELOCS, "exact"), RELOCS)

    def test_disp_masks_displacements_and_loose_immediates(self):
        disp = crossexe.masked_offsets(BODY, RELOCS, "disp")
        self.assertEqual(disp, RELOCS | {2})
        self.assertEqual(crossexe.masked_offsets(BODY, RELOCS, "loose"), disp | {5})

    def test_branch_targets_stay_unmasked_by_immediates(self):
        body = bytes.fromhex("7405" "e800000000")
        self.assertEqual(crossexe.masked_offsets(body, set(), "loose"), set())

    def test_pattern_finds_overlapping_copies(self):
        pat = crossexe.pattern(b"\x90\x90", set())
        self.assertEqual([m.start() for m in pat.finditer(b"\x90\x90\x90")], [0, 1])

    def test_masked_bytes_match_anything(self):
        pat = crossexe.pattern(b"\x01\x02\x03", {1})
        self.assertIsNotNone(pat.match(b"\x01\x0a\x03"))
        self.assertIsNone(pat.match(b"\x01\x0a\x04"))


@unittest.skipIf(crossexe is None, "capstone and pefile are needed")
class GradeTests(unittest.TestCase):
    def test_each_copy_gets_its_strongest_level(self):
        exact = bytes.fromhex("8b4110" "83c005" "e812345678" "c3")
        disp = bytes.fromhex("8b4114" "83c005" "e800000000" "c3")
        loose = bytes.fromhex("8b4114" "83c007" "e800000000" "c3")
        other = bytes.fromhex("8b4110" "83e805" "e800000000" "c3")
        code = exact + b"\xcc" * 3 + disp + b"\xcc" * 3 + loose + b"\xcc" * 3 + other
        got = crossexe.grade(code, T0, BODY, RELOCS)
        step = len(BODY) + 3
        self.assertEqual(got, {T0: "exact", T0 + step: "disp", T0 + 2 * step: "loose"})


@unittest.skipIf(crossexe is None, "capstone and pefile are needed")
class StartTests(unittest.TestCase):
    def test_aligned_after_padding_or_return(self):
        code = bytearray(b"\x00" * 0x40)
        code[0x0F] = 0xCC
        code[0x1F] = 0xC3
        code[0x2D:0x30] = b"\xc2\x08\x00"
        code = bytes(code)
        self.assertTrue(crossexe.follows_padding(code, T0, T0))
        self.assertTrue(crossexe.follows_padding(code, T0, T0 + 0x10))
        self.assertTrue(crossexe.follows_padding(code, T0, T0 + 0x20))
        self.assertTrue(crossexe.follows_padding(code, T0, T0 + 0x30))
        self.assertFalse(crossexe.follows_padding(code, T0, T0 + 0x11))
        code = code[:0x0F] + b"\x00" + code[0x10:]
        self.assertFalse(crossexe.follows_padding(code, T0, T0 + 0x10))
        self.assertFalse(crossexe.follows_padding(code, T0, T0 + 0x40))

    def test_found_starts_come_from_calls_and_pointers(self):
        code = bytearray(b"\xcc" * 0x40)
        code[0:5] = call(T0, T0 + 0x20)
        code[0x10] = 0xC3
        e = image(bytes(code), struct.pack("<II", T0 + 0x30, T0 + 0x11))
        self.assertEqual(e.starts, [T0 + 0x20, T0 + 0x30])
        self.assertEqual(e.containing(T0 + 0x25), T0 + 0x20)
        self.assertIsNone(e.containing(T0 + 0x10))
        self.assertEqual(e.size(T0 + 0x20), 0x10)

    def test_known_starts_bound_their_functions(self):
        e = image(b"\xcc" * 0x40)
        e.known = {T0: 0x10, T0 + 0x20: 0x08}
        self.assertTrue(e.is_start(T0 + 0x20))
        self.assertFalse(e.is_start(T0 + 0x30))
        self.assertEqual(e.containing(T0 + 0x04), T0)
        self.assertIsNone(e.containing(T0 + 0x18))


@unittest.skipIf(crossexe is None, "capstone and pefile are needed")
class CallerTests(unittest.TestCase):
    def test_calls_jumps_and_absolute_references(self):
        code = bytearray(b"\xcc" * 0x60)
        code[0x00:0x05] = call(T0, T0 + 0x40)
        code[0x05] = 0xC3
        code[0x10:0x15] = b"\xe9" + struct.pack("<i", 0x40 - 0x15)
        code[0x20:0x25] = b"\x68" + struct.pack("<I", T0 + 0x40)
        code[0x25] = 0xC3
        code[0x30:0x35] = call(T0 + 0x30, T0 + 0x1000)
        e = image(bytes(code), struct.pack("<I", T0 + 0x40))
        self.assertEqual(crossexe.rel_refs(e.code, T0)[T0 + 0x40], [(T0, "call"), (T0 + 0x10, "jmp")])
        self.assertNotIn(T0 + 0x1000, e.rel_refs)
        self.assertEqual(e.refs(T0 + 0x40), [(T0, "call"), (T0 + 0x10, "jmp"),
                                             (T0 + 0x21, "code-imm"), (T0 + 0x60, "data")])
        e.known = {T0: 0x10, T0 + 0x10: 0x10, T0 + 0x20: 0x10, T0 + 0x40: 0x10}
        self.assertEqual(crossexe.callers(e, T0 + 0x40), [T0, T0 + 0x10])

    def test_describe_lists_start_hits_first(self):
        e = image(b"\xcc" * 0x40)
        e.known = {T0 + 0x20: 0x10}
        e.names = {T0 + 0x20: "Helper"}
        hits = [(T0 + 0x04, "exact", False, []), (T0 + 0x20, "disp", True, [(T0 + 0x24, "call")])]
        lines = crossexe.describe(e, hits, 8)
        self.assertEqual(lines[0], "  t: 2 hit(s), 1 at a function start")
        self.assertEqual(lines[1], f"    0x{T0 + 0x20:08X} disp  start Helper")
        self.assertEqual(lines[2], f"        call     from 0x{T0 + 0x24:08X} in Helper")
        self.assertEqual(lines[3], f"    0x{T0 + 0x04:08X} exact inside ?")


@unittest.skipIf(crossexe is None, "capstone and pefile are needed")
class SourceTests(unittest.TestCase):
    LINES = ["#include \"x.h\"", "static int table[4] = {0};", "static int Helper(int a)", "{", "}",
             "inline void Other(void) {}", "// FUNCTION: GAMEMD 0x00401000", "// Marked", "int Marked(void)",
             "{", "\tstatic int Local(void);", "}", "static int Declared(int a);"]

    def test_unstatic_drops_storage_from_top_level_definitions_only(self):
        out = crossexe.unstatic(self.LINES)
        self.assertEqual(out[1], self.LINES[1])
        self.assertEqual(out[2], "int Helper(int a)")
        self.assertEqual(out[5], "void Other(void) {}")
        self.assertEqual(out[10], self.LINES[10])

    def test_unmarked_names_skip_marked_and_declared_functions(self):
        self.assertEqual(crossexe.unmarked_names(self.LINES), {"Helper", "Other"})

    def test_recovered_names_qualify_by_class_folder(self):
        folder = Path("recovered/classes/Surface")
        self.assertEqual(crossexe.recovered_name(folder, "Surface_Get_Rect", "x"), "Surface::Get_Rect [Surface]")
        self.assertEqual(crossexe.recovered_name(Path("recovered/Rect"), "Unknown", "Unknown_0042bcd0"),
                         "Unknown_0042bcd0 [Rect]")


if __name__ == "__main__":
    unittest.main()
