import unittest

from coord.choices import plan, variants, waiting_groups


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

    def test_suggestions_use_latest_blocker_and_exclude_claims(self):
        rows = [{"address": hex(addr), "blocker": blocker} for addr, blocker in
                [(1, "waits on caller 0x00000010"), (1, "registers"), (2, "waits on caller 0x00000010")]]
        self.assertEqual(waiting_groups(rows, set(), set(), {1, 2, 16}),
                         [(16, [(2, "waits on caller 0x00000010")])])
        self.assertFalse(waiting_groups(rows, {16}, set(), {1, 2, 16}))

    def test_budget_resets_only_on_improvement(self):
        from coord.retry_budget import exhausted_retries
        def row(score, checks):
            return {"address": "0x10", "best": str(score), "checks": str(checks)}
        rows = [row(80, 30), row(80, 0), row(70, 29)]
        self.assertFalse(exhausted_retries(rows, {16: 2000}))
        self.assertEqual(exhausted_retries(rows + [row(80, 1)], {16: 2000}), {16})
        self.assertFalse(exhausted_retries(rows + [row(80, 1)], {16: 2001}))
        self.assertFalse(exhausted_retries(rows + [row(81, 60)], {16: 2000}))

    def test_coherent_group_precedes_mixed_exceptions(self):
        sites = [{"find": c, "alternatives": [c.upper()], "group": "access"} for c in "abc"]
        trial = [s for _, s in variants("abc", sites)]
        self.assertEqual(trial[:2], ["abc", "ABC"])
        self.assertEqual(len(set(trial)), 8)
        sites[0]["alternatives"].append("x")
        with self.assertRaisesRegex(ValueError, "corresponding"):
            list(variants("abc", sites))

    def test_region_report_retains_insertions_and_distinct_helper_outcomes(self):
        from coord.regions import differences, representatives
        a = ["mov eax, [esp+4]", "ret"]
        self.assertEqual(differences(a, [a[0], "nop", a[1], "nop"], [100, 104, 105]),
                         [[104, 104, 0, 1], [105, 105, 0, 1]])
        def row(i, regions, raw, helper):
            return {"compile": i, "targets": {"main": {"status": "partial", "regions": regions, "raw": raw},
                                              "h": {"status": helper}}}
        x = row(1, [[100, 105, 2, 2]], .995, "match")
        y = row(2, [[104, 105, 1, 1]], .75, "match")
        z = row(3, [[104, 105, 1, 1]], .99, "absent")
        self.assertEqual(representatives([x, y, z, {**y, "compile": 4}, {"error": "failed"}], "main"), [y, z, x])
        edge = row(5, [[104, 105, 1, 1]], .75, "match")
        edge["targets"]["main"]["changed_targets"] = [[104, "je retail:a", "je retail:b"]]
        self.assertEqual(representatives([y, edge], "main"), [y, edge])
