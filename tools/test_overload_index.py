import unittest

from check import ReferenceNames, overloaded, qualified


class IndexTests(unittest.TestCase):
    def test_agrees_with_lookup_across_both_symbol_sources(self):
        sig = "?Tick@Foo@@QAEHXZ"
        other = "?Tick@Foo@@QAEHH@Z"
        name = qualified(sig)
        for local, declared in [({}, []), ({sig: 1}, [sig]), ({other: 2}, []),
                                ({sig: 1}, [other]), ({other: 2}, [sig])]:
            index = ReferenceNames({name: 1}, local, declared)
            for symbol in [sig, other]:
                self.assertEqual(index.ambiguous(symbol, name), overloaded(symbol, local, declared))
