"""Partial-match listings use the candidate's relocations even when instructions move."""
import struct
import unittest
from types import SimpleNamespace

import check
from coff import CODE, REL_REL32, STORAGE_EXTERNAL, CoffObject, Reloc, Section, Symbol


ADDRESS = 0x402000
CALLEE = 0x405000


def compare_call(original_prefix, compiled_prefix, original_suffix=b"\xc3", target=CALLEE):
    disp = CALLEE - (ADDRESS + len(original_prefix) + 5)
    original = original_prefix + b"\xe8" + struct.pack("<i", disp) + original_suffix
    compiled = compiled_prefix + b"\xe8\0\0\0\0\xc3"
    orig = SimpleNamespace(base=0x400000, end=0x500000, sizes={ADDRESS: len(original)},
                           read=lambda va, size: original[va - ADDRESS:va - ADDRESS + size])
    obj = CoffObject.__new__(CoffObject)
    obj.sections = [Section(1, ".text", compiled, CODE, [Reloc(len(compiled_prefix) + 1, 1, REL_REL32)])]
    obj.symbols = [Symbol(0, "_caller", 0, 1, 0x20, STORAGE_EXTERNAL),
                   Symbol(1, "_callee", 0, 0, 0x20, STORAGE_EXTERNAL)]
    obj.by_index = {s.index: s for s in obj.symbols}
    return check.compare(orig, obj, b"", ADDRESS, "caller", {}, {"_callee": target})


class RelocationDiffTests(unittest.TestCase):
    def test_shifted_call_keeps_stack_difference_visible(self):
        # The nop moves across the call, so both functions have the same length.
        result = compare_call(bytes.fromhex("8b4424 10"), bytes.fromhex("8b4424 14 90"), b"\x90\xc3")
        self.assertFalse(result.matched)
        self.assertIn("-mov eax, dword ptr [esp + 0x10]", result.diff)
        self.assertIn("+mov eax, dword ptr [esp + 0x14]", result.diff)
        calls = [line[1:] for line in result.diff if "call " in line]
        self.assertTrue(calls)
        self.assertEqual(set(calls), {"call 0x00405000 (_callee)"})

    def test_different_inc_encoding_moves_the_call(self):
        result = compare_call(b"\x40", b"\xff\xc0")
        self.assertFalse(result.matched)
        self.assertEqual(result.diff, ["0x00402000 inc eax: original 40, ours ffc0"])

    def test_local_edges_survive_motion_but_not_retargeting(self):
        orig = SimpleNamespace(base=0x400000, end=0x500000)
        def edges(left, right):
            sec = Section(1, ".text", right, CODE, [])
            return check.normalised_listings(orig, None, sec, 0, right, ADDRESS, left, False)
        left = bytes.fromhex("7401 40 c3")
        a, b = edges(left, bytes.fromhex("7402 90 40 c3"))
        self.assertEqual(a[0], b[0])
        a, b = edges(left, bytes.fromhex("7400 40 c3"))
        self.assertNotEqual(a[0], b[0])
        self.assertLess(check.similarity(a, b), 1)
        a, b = edges(left, bytes.fromhex("7400 90 40 c3"))
        self.assertIn("unaligned-ours", b[0])

    def test_changed_or_unknown_external_target_is_visible(self):
        result = compare_call(b"\x90", b"", target=CALLEE + 1)
        self.assertIn("-call 0x00405000", result.diff)
        self.assertIn("+call 0x00405001 (_callee)", result.diff)
        result = compare_call(b"\x90", b"", target=None)
        self.assertIn("+call <unresolved:_callee>", result.diff)

    def test_matching_bytes_still_require_the_right_call_target(self):
        self.assertTrue(compare_call(b"", b"").matched)
        wrong = compare_call(b"", b"", target=CALLEE + 1)
        self.assertTrue(wrong.bytes_match)
        self.assertFalse(wrong.matched)
        self.assertEqual(wrong.mismatched_refs, 1)


if __name__ == "__main__":
    unittest.main()
