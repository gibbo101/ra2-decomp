import importlib.util
import random
import unittest

if importlib.util.find_spec("tree_sitter_cpp") and importlib.util.find_spec("tree_sitter"):
    import permute_mutate as m
else:
    m = None


@unittest.skipUnless(m, "requires the permuter dependencies")
class OperatorTests(unittest.TestCase):
    def mutate(self, body, kind):
        source = "// FUNCTION: GAMEMD 0x00401000\n" + body
        weights = {name: int(name == kind) for name in m.MUTATIONS}
        mutator = m.Mutator(source, m.TargetSpec(0x401000), weights)
        return mutator.mutate(source, random.Random(2), 1)[1]

    def test_swap_requires_scalar_operands(self):
        for ty in ("Quaternion", "Unknown"):
            self.assertFalse(self.mutate(f"{ty} F({ty} a, {ty} b) {{ return a * b; }}", "swap_commutative"))
        self.assertTrue(self.mutate("int F(int a, int b) { return a * b; }", "swap_commutative"))

    def test_negation_does_not_substitute_an_overload(self):
        for ty, eligible in (("Flag", False), ("int", True)):
            body = f"int F({ty} a, {ty} b) {{ if (a == b) return 1; else return 2; }}"
            self.assertEqual(bool(self.mutate(body, "negate_if")), eligible)
