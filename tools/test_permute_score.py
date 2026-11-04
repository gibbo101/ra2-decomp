"""Regression tests for the permuter. Use its inline dependencies when running this suite."""

import contextlib
import importlib.util
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

from coff import CoffObject, Section, Symbol, Reloc, CODE, REL_REL32

if importlib.util.find_spec("tree_sitter_cpp") and importlib.util.find_spec("tree_sitter"):
    import permute
else:
    permute = None


@unittest.skipIf(permute is None, "requires the permuter's tree-sitter dependencies")
class PermuteScoreTests(unittest.TestCase):
    address = 0x401000

    def scorer(self, ours, theirs, relocs=()):
        obj = CoffObject.__new__(CoffObject)
        obj.sections = [Section(1, ".text", ours, CODE, list(relocs))]
        obj.symbols = [Symbol(0, "_target", 0, 1, 0x20, 2),
                       Symbol(1, "_other", 0, 0, 0x20, 2)]
        obj.by_index = {s.index: s for s in obj.symbols}
        orig = SimpleNamespace(base=0x400000, end=0x500000,
                               sizes={self.address: len(theirs)}, read=lambda addr, size: theirs[:size])
        with patch.object(permute, "Original", return_value=orig), patch.object(permute, "known_names", return_value={}):
            scorer = permute.Scorer(self.address, "=_target", {"_other": 0x403000}, [])
        return obj, scorer

    def test_wrong_call_target_invalidates_guard(self):
        # The original calls 0x402000. The COFF relocation names a different function.
        obj, scorer = self.scorer(bytes.fromhex("e8 00000000 c3"), bytes.fromhex("e8 fb0f0000 c3"),
                                  [Reloc(1, 1, REL_REL32)])
        with patch.object(permute, "CoffObject", return_value=obj):
            self.assertEqual(scorer.score(b"").status, "bytes")
            scorer.guards = [(self.address, "=_target")]
            self.assertEqual(scorer.score(b"").status, "guard")
            scorer.local["_other"] = 0x402000
            self.assertEqual(scorer.score(b"").status, "match")

    def test_stack_differences_have_distinct_scores_and_lines(self):
        load = bytes.fromhex("8b442404")
        other = bytes.fromhex("8b442408")
        theirs = load * 8 + b"\xc3"
        for count in (1, 4, 8):
            ours = other * count + load * (8 - count) + b"\xc3"
            obj, scorer = self.scorer(ours, theirs)
            table = [(4 * i, i + 10) for i in range(8)]
            with patch.object(permute, "CoffObject", return_value=obj), patch.object(permute, "line_table", return_value=table):
                score = scorer.score(b"")
            self.assertAlmostEqual(score.value, 1 + count * permute.PENALTY_SLOT)
            self.assertEqual(score.hot, tuple(range(10, 10 + count)))

    def test_bytes_only_start_does_not_report_success(self):
        with tempfile.TemporaryDirectory() as tmp, contextlib.ExitStack() as patches:
            root = Path(tmp)
            src = root / "example.cpp"
            src.write_text("// FUNCTION: GAMEMD 0x00401000\nint target() { return 1; }\n")
            obj = root / "example.obj"
            obj.write_bytes(b"")
            base = SimpleNamespace(symbol="_target", matched=False)
            for name, value in {
                "OUT": root, "Original": lambda: None, "CoffObject": lambda raw: None,
                "compile_source": lambda *a, **kw: (obj, ""), "known_names": lambda: {},
                "bind_annotations": lambda *a: ({"_target": self.address}, []),
                "compare": lambda *a, **kw: base,
            }.items():
                patches.enter_context(patch.object(permute, name, value))
            pool = patches.enter_context(patch.object(permute, "ProcessPoolExecutor"))
            worker = pool.return_value.__enter__.return_value
            worker.submit.return_value.result.return_value = {"score": permute.Score(0.5, 1, 1, "bytes")}
            self.assertEqual(permute.permute(src, self.address, None, 0, 1, 7, focus=False), 1)


if __name__ == "__main__":
    unittest.main()
