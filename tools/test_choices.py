import unittest

from coord.choices import plan, variants


class TrialsTests(unittest.TestCase):
    def test_choices_apply_to_original_sites_once(self):
        sites = [{"find": "a", "alternatives": ["b", "b"]}, {"find": "b", "alternatives": ["c"]}]
        self.assertEqual([s for _, s in variants("a b", sites)], ["a b", "a c", "b b", "b c"])
        with self.assertRaises(ValueError):
            list(variants("a b", [*sites, {"find": "a b", "alternatives": ["x"]}]))

    def test_claim_excludes_whole_group_and_retry_needs_evidence(self):
        job = dict(name="x", address="0x10", helpers={"0x20": "f"}, hypothesis="h", evidence="e")
        self.assertEqual(plan([job], {32}, set(), {16, 32}, lambda a: "module"), {"jobs": []})
        grouped = plan([job], set(), set(), {16, 32}, lambda a: "module")["jobs"][0]
        self.assertEqual(grouped["modules"]["module"], ["0x00000010", "0x00000020"])
        with self.assertRaises(ValueError):
            plan([{**job, "evidence": ""}], set(), set(), {16, 32}, lambda a: "module")
