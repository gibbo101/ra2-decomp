import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

try:
    import similar
except ImportError:  # capstone or pefile missing
    similar = None


@unittest.skipIf(similar is None, "capstone and pefile are needed")
class SimilarScore(unittest.TestCase):
    def test_identical_features_score_one(self):
        f = {"grams": ["a b c d"], "calls": [1], "data": [2], "length": 10}
        self.assertAlmostEqual(similar.score(f, f), 1.0)

    def test_disjoint_features_score_only_size(self):
        a = {"grams": ["a b c d"], "calls": [1], "data": [2], "length": 10}
        b = {"grams": ["e f g h"], "calls": [3], "data": [4], "length": 20}
        self.assertAlmostEqual(similar.score(a, b), 0.15 * 0.5)

    def test_jaccard_of_empty_sets_is_zero(self):
        self.assertEqual(similar.jaccard(set(), set()), 0.0)


if __name__ == "__main__":
    unittest.main()
