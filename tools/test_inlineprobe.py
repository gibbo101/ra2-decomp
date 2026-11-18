import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import inlineprobe


class HelperTest(unittest.TestCase):
    def test_default_body_folds_away(self):
        self.assertEqual(inlineprobe.helper(), "static inline int Inline_Probe_(int a) { return a & 0; }")

    def test_spend_gives_one_term_each(self):
        self.assertEqual(inlineprobe.helper(3),
                         "static inline int Inline_Probe_(int a) { return (a ^ 1) + (a ^ 2) + (a ^ 3); }")


if __name__ == "__main__":
    unittest.main()
