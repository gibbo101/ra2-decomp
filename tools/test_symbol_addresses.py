import unittest
from types import SimpleNamespace

from check import check_ref, known_decorated


class CopiesTests(unittest.TestCase):
    def test_known_copies_require_local_evidence(self):
        target = SimpleNamespace(name="_copy", section=0)
        sec = SimpleNamespace(number=1)
        orig = SimpleNamespace(decorated_addresses={"_copy": {100, 200}})
        self.assertEqual(known_decorated(orig.decorated_addresses), {})
        def verdict(addr, local):
            return check_ref(orig, None, None, target, sec, 0, 0, addr, {"copy": 100}, local)[0]
        self.assertEqual(verdict(200, {}), "unverified")
        self.assertEqual(verdict(300, {}), "mismatch")
        self.assertEqual(verdict(100, {"_copy": 200}), "mismatch")
        self.assertEqual(verdict(200, {"_copy": 200}), "ok")
