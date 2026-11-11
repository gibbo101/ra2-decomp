"""The progress report agrees with the status totals and never reports an unlanded match."""

import unittest

import bytematch_report
import gamecode


class BytematchReportTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.report = bytematch_report.build()

    def test_game_category_matches_the_manifest(self):
        game, _ = gamecode.split_functions()
        done = gamecode.matched() & game.keys()
        m = next(c for c in self.report["categories"] if c["id"] == "game")["measures"]
        self.assertEqual(m["total_functions"], len(game))
        self.assertEqual(m["matched_functions"], len(done))
        self.assertEqual(m["matched_code"], sum(game[a] for a in done))

    def test_every_function_is_in_one_unit(self):
        game, library = gamecode.split_functions()
        seen = [f["metadata"]["virtual_address"] for u in self.report["units"] for f in u["functions"]]
        self.assertEqual(sorted(seen), sorted([*game, *library]))

    def test_only_manifest_rows_score_100_in_game_units(self):
        done = gamecode.matched()
        for u in self.report["units"]:
            if u["name"].startswith("game/"):
                for f in u["functions"]:
                    self.assertEqual(f["fuzzy_match_percent"] == 100, f["metadata"]["virtual_address"] in done)


if __name__ == "__main__":
    unittest.main()
