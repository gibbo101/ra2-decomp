"""A reference fails where the original's target is bound to another function this object compiles."""
from types import SimpleNamespace
import unittest

import check
from coff import CODE, Section, Symbol

PUSHER = Section(1, ".text", b"", CODE)


def obj(*symbols):
    sections = [Section(n, ".text", b"\xc3", CODE) for n in range(1, 4)]
    return SimpleNamespace(symbols=list(symbols), sections=sections)


def stub(name, section):
    return Symbol(0, name, 0, section, 0x20, 3)


def ref(target, got, local, o):
    return check.check_ref(SimpleNamespace(decorated_addresses={}), o, b"", target, PUSHER, 0, 0x401000, got, {}, local)


class BoundTargetTests(unittest.TestCase):
    def test_target_at_another_bound_function_is_a_mismatch(self):
        claimed, pushed = stub("_$E41", 2), stub("_$E171", 3)
        o = obj(claimed, pushed)
        self.assertEqual(ref(pushed, 0x4666B0, {"_$E41": 0x4666B0}, o), ("mismatch", None))

    def test_bound_target_still_verifies(self):
        pushed = stub("_$E171", 3)
        self.assertEqual(ref(pushed, 0x4666B0, {"_$E171": 0x4666B0}, obj(pushed)), ("ok", 0x4666B0))

    def test_unbound_address_stays_unverified(self):
        claimed, pushed = stub("_$E41", 2), stub("_$E171", 3)
        o = obj(claimed, pushed)
        self.assertEqual(ref(pushed, 0x4666C0, {"_$E41": 0x4666B0}, o), ("unverified", None))

    def test_address_bound_outside_this_object_is_not_a_conflict(self):
        pushed = stub("_$E171", 3)
        self.assertEqual(ref(pushed, 0x4666B0, {"?Other@@YAXXZ": 0x4666B0}, obj(pushed)), ("unverified", None))


if __name__ == "__main__":
    unittest.main()
