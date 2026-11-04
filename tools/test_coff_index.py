import unittest

from coff import CODE, CoffObject, Section, Symbol


class FunctionIndexTests(unittest.TestCase):
    def test_code_sections_aliases_and_fresh_iterators(self):
        obj = CoffObject.__new__(CoffObject)
        obj.sections = [Section(1, ".data", bytes(4), 0), Section(2, ".text", bytes(8), CODE)]
        obj.symbols = [
            Symbol(0, "data", 0, 1, 0, 2), Symbol(1, "proc", 4, 2, 0, 2),
            Symbol(2, "alias", 4, 2, 0x20, 3), Symbol(3, "first", 0, 2, 0x20, 3),
            Symbol(4, "label", 7, 2, 0, 3), Symbol(5, "external", 0, 0, 0x20, 2),
        ]
        first = obj.functions()
        self.assertEqual(next(first)[0].name, "first")
        self.assertEqual([(s.name, lo, hi) for s, _, lo, hi in obj.functions()],
                         [("first", 0, 4), ("proc", 4, 4), ("alias", 4, 8)])
        self.assertEqual(next(first)[0].name, "proc")
