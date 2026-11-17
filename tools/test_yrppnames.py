"""A name is YRpp-only when YRpp supplies it and the TibSun archive does not have it."""
from pathlib import Path
import tempfile
import unittest

import yrppnames

PROGRESS = [
    {"address": "0x00401000", "name": "HouseClass::AI", "result": "MATCH"},
    {"address": "0x00401010", "name": "HouseClass::Spend_Money", "result": "MATCH"},
    {"address": "0x00401020", "name": "HouseClass::FUN_00401020", "result": "MATCH"},
    {"address": "0x00401030", "name": "HouseClass::Guess", "result": "75.0%"},
]
SYMBOLS = [
    {"address": "0x00401000", "name": "HouseClass::AI"},
    {"address": "0x00401010", "name": "HouseClass::Spend_Money"},
    {"address": "0x00401030", "name": "HouseClass::Guess"},
    {"address": "0x00401010", "name": "MixFileClass::Other"},
]
NAMES = [
    {"address": "0x00A80228", "name": "Houses", "evidence": "YRpp HouseClass::Array"},
    {"address": "0x00A80300", "name": "YRppOnlyGlobal", "evidence": "YRpp Something"},
    {"address": "0x00A80400", "name": "MatchedGlobal", "evidence": "matched with TibSun source"},
]


class YrppNamesTests(unittest.TestCase):
    def test_tibsun_definitions_and_words(self):
        root = Path(tempfile.mkdtemp(prefix="yrpp-test-"))
        src = root / "house.cpp"
        src.write_text("void HouseClass::AI(void)\n{\n\tHouses.Count();\n}\nint Free_Function(int x)\n{\n}\n")
        functions, words = yrppnames.tibsun_names(root)
        src.unlink()
        root.rmdir()
        self.assertIn("HouseClass::AI", functions)
        self.assertIn("Free_Function", functions)
        self.assertIn("Houses", words)

    def test_lists_only_uncorroborated_yrpp_names(self):
        rows = yrppnames.yrpp_only(PROGRESS, SYMBOLS, NAMES, {"HouseClass::AI"}, {"Houses"})
        self.assertEqual(rows, [("0x00401010", "HouseClass::Spend_Money", "symbols.csv"),
                                ("0x00A80300", "YRppOnlyGlobal", "names.csv")])

    def test_render_has_a_header(self):
        self.assertEqual(yrppnames.render([("0x00401010", "A::B", "function")]),
                         "address,name,source\n0x00401010,A::B,function\n")


if __name__ == "__main__":
    unittest.main()
