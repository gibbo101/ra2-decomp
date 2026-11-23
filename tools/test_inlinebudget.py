"""inlinebudget lines up both sides' calls, finds the differing inline decisions and reads the listing."""
import sys
import unittest
from collections import namedtuple
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

try:
    import inlinebudget
    from inlinebudget import Call
except ImportError:  # capstone or pefile missing
    inlinebudget = None

Ins = namedtuple("Ins", "address")


def seq(keys, defined=False):
    return [Call(n, 0x1000 + 0x10 * n, k, "", defined) for n, k in enumerate(keys)]


@unittest.skipIf(inlinebudget is None, "capstone and pefile are needed")
class InlineBudgetTests(unittest.TestCase):
    def test_calls_read_targets_and_unresolved_symbols(self):
        listing = ["push eax", "call 0x005AD790 (?Delete_Index@@QAE_NH@Z)", "jmp <retail:0x00401000>",
                   "call <unresolved:?Foo@@YAXXZ>", "jmp 0x00402000", "call dword ptr [eax]"]
        insns = [Ins(0x401000 + n) for n in range(len(listing))]
        out = inlinebudget.calls(listing, insns, lambda s: 0x500000 if s == "?Foo@@YAXXZ" else None,
                                 {"?Delete_Index@@QAE_NH@Z"})
        self.assertEqual([(c.index, c.key, c.defined) for c in out],
                         [(1, 0x5AD790, True), (3, 0x500000, False), (4, 0x402000, False)])
        self.assertEqual(inlinebudget.calls(listing[3:4], insns[:1])[0].key, "?Foo@@YAXXZ")

    def test_matching_calls_differ_nowhere(self):
        self.assertEqual(inlinebudget.differing(seq([1, 2, 3]), seq([1, 2, 3], True)), [])

    def test_exe_call_ours_inlined(self):
        sites = inlinebudget.differing(seq([1, 2, 3, 4]), seq([1, 4], True))
        self.assertEqual([(s.side, s.callee, s.anchor) for s in sites], [("exe", 2, 0x1000), ("exe", 3, 0x1000)])
        self.assertEqual(sites[0].ident, ("exe", 0x1010))

    def test_ours_call_counts_only_with_a_body(self):
        exe = seq([1, 3])
        self.assertEqual([(s.side, s.callee) for s in inlinebudget.differing(exe, seq([1, 2, 3], True))],
                         [("ours", 2)])
        self.assertEqual(inlinebudget.differing(exe, seq([1, 2, 3], False)), [])

    def test_reordered_calls_are_not_decisions(self):
        self.assertEqual(inlinebudget.differing(seq([1, 2, 3]), seq([1, 3, 2], True)), [])

    def test_unresolved_callee_pairs_with_the_exe_target_in_its_place(self):
        self.assertEqual(inlinebudget.differing(seq([1, 0x5981F0, 3]), seq([1, "?Load@@QAEXXZ", 3], True)), [])

    def test_probe_calls_are_left_out(self):
        self.assertEqual(inlinebudget.differing(seq([1]), seq([1, "?Inline_Probe_Spend_@@YAXH@Z"], True)), [])

    def test_listing_lines_by_offset(self):
        text = ("_x$ = 8\n?A@@YAXXZ PROC NEAR\n\n; 12 : {\n\n  00000\t56\t\t push esi\n"
                "; 13 : \tB();\n  00001\te8 00 00 00\n\t00\t\t call ?B@@YAXXZ\n  00006\t5e\t\t pop esi\n"
                "?A@@YAXXZ ENDP\n  00007\t90\t\t nop\n")
        lines = inlinebudget.listing_lines(text, "?A@@YAXXZ")
        self.assertEqual(lines, {0: 12, 1: 13, 6: 13})
        self.assertEqual(inlinebudget.line_at(lines, 3), 13)
        self.assertIsNone(inlinebudget.line_at({4: 1}, 3))

    def test_statement_end(self):
        lines = ["\tCleanup();", "\tif (x) {", "\t\ty();", "\t}", "\tFoo(a,", "\t    b);", "\tif (p) p();"]
        self.assertEqual(inlinebudget.statement_end(lines, 0), 0)
        self.assertEqual(inlinebudget.statement_end(lines, 1), 3)
        self.assertEqual(inlinebudget.statement_end(lines, 4), 5)
        self.assertEqual(inlinebudget.statement_end(lines, 6), 6)

    def test_spend_probe_body_is_one_unit_calls(self):
        kind = inlinebudget.spend_kind(3)
        self.assertEqual(kind.stmt, "Inline_Probe_Spend_({k});")
        self.assertEqual(kind.decls[1], "static inline void Inline_Probe_Spend_(int a) "
                         "{ Inline_Probe_Sink_(1); Inline_Probe_Sink_(1); Inline_Probe_Sink_(1); }")


if __name__ == "__main__":
    unittest.main()
