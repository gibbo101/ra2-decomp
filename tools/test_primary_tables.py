import unittest
from rtti import Rtti


class PrimaryTableTests(unittest.TestCase):
    def test_only_single_primary_table_identifies_the_class(self):
        rtti = Rtti.__new__(Rtti)
        rtti.vtables = {1: (".?AVFoo@@", 0, []), 2: (".?AVBar@@", 0, []),
                        3: (".?AVBar@@", 4, []), 4: (".?AVSecondary@@", 8, [])}
        self.assertEqual(rtti.unique_primary_tables, {"Foo@@": 1})
