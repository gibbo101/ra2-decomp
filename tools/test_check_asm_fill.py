"""An assembly routine ends before MASM's alignment filler only where the original began a new object."""
import unittest

import check
from coff import CODE, Section, Symbol

ROUTINE = b"\x55\x8b\xec\xc9\xc3"


def pick(code, object_starts=()):
    sec = Section(1, ".text", code, CODE, [], set(object_starts))
    return Symbol(0, "_routine", 0, 1, 0x20, 2), sec, 0, len(code)


class AsmFillTests(unittest.TestCase):
    def test_masm_fill_matches_what_ml_emits(self):
        self.assertEqual(check.masm_fill(7), b"\x8d\xa4\x24\x00\x00\x00\x00")
        self.assertEqual(check.masm_fill(10), b"\x8d\xa4\x24\x00\x00\x00\x00\x8d\x49\x00")
        self.assertEqual(check.masm_fill(14), b"\x8d\xa4\x24\x00\x00\x00\x00" * 2)

    def test_fill_before_an_object_start_is_not_the_routines(self):
        code = ROUTINE + check.masm_fill(11)
        self.assertEqual(check.function_bytes(pick(code, {len(code)}))[0], ROUTINE)

    def test_fill_stays_where_the_next_routine_shares_the_object(self):
        code = ROUTINE + check.masm_fill(11)
        self.assertEqual(check.function_bytes(pick(code))[0], code)

    def test_other_trailing_code_is_kept(self):
        code = ROUTINE + b"\x8d\x64\x24\x01"
        self.assertEqual(check.function_bytes(pick(code, {len(code)}))[0], code)


if __name__ == "__main__":
    unittest.main()
