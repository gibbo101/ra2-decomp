import csv
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


class AttemptsTableTest(unittest.TestCase):
    def test_every_row_has_the_header_width(self):
        with open(ROOT / "data/attempts.csv", newline="", encoding="utf-8") as table:
            reader = csv.reader(table)
            width = len(next(reader))
            ragged = [reader.line_num for row in reader if len(row) != width]
        self.assertEqual(ragged, [], f"data/attempts.csv rows without {width} columns")


if __name__ == "__main__":
    unittest.main()
