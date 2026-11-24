"""check.output runs the checker in-process and returns what it prints."""
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
import check  # noqa: E402
import declprobe  # noqa: E402
import inlineprobe  # noqa: E402
import localprobe  # noqa: E402

FIXTURE = Path("fixture.cpp")
HEAD = "0x00401000 A: 98.0% (5/10 bytes) _A\n-call 0x00401100\n+call 0x00401200\n+mov eax, 1\n"


class OutputTests(unittest.TestCase):
    def test_a_failed_build_gives_its_log(self):
        with patch.object(check, "annotations", return_value=[(0x401000, "A")]), \
                patch.object(check, "compile_source", return_value=(None, "error C2065")) as compile_source:
            self.assertEqual(check.output(FIXTURE, {0x401000}, out=Path("x.obj")), "error C2065\n")
        compile_source.assert_called_once_with(FIXTURE, None, out=Path("x.obj"))

    def test_an_early_exit_gives_what_was_printed(self):
        with patch.object(check, "annotations", return_value=[(0x401000, "A")]):
            self.assertEqual(check.output(FIXTURE, {0x401010}), "")

    def test_report_gets_the_loaded_exe_and_names(self):
        def report(src, raw, orig, names, targets, score, summary):
            print(f"{orig} {names} {targets}")
            return 0
        with patch.object(check, "annotations", return_value=[(0x401000, "A"), (0x401010, "B")]), \
                patch.object(check, "compile_source", return_value=(Path(__file__), "")), \
                patch.object(check, "report", side_effect=report):
            self.assertEqual(check.output(FIXTURE, {0x401010}, orig="exe", names={"n": 1}),
                             "exe {'n': 1} [(4198416, 'B')]\n")


class ProbeCheckTests(unittest.TestCase):
    """The probes read check.output's text; none of them starts another process."""

    def test_localprobe_reads_score_and_size(self):
        with patch.object(check, "output", return_value=HEAD) as output:
            self.assertEqual(localprobe.checked(Path("/b/a.cpp"), 0x401000, "exe", {}), "98.0% 5/10")
        output.assert_called_once_with(Path("/b/a.cpp"), {0x401000}, orig="exe", names={}, out=Path("/b/a.obj"))

    def test_inlineprobe_counts_differing_call_lines(self):
        with patch.object(check, "output", return_value=HEAD):
            self.assertEqual(inlineprobe.checked(Path("/b/a.cpp"), 0x401000, "exe", {}), ("98.0%", "5/10", 2))
        with patch.object(check, "output", return_value="error\n"):
            self.assertEqual(inlineprobe.checked(Path("/b/a.cpp"), 0x401000, "exe", {}), ("build failed", "", -1))

    def test_no_probe_imports_subprocess(self):
        for module in (declprobe, inlineprobe, localprobe):
            self.assertFalse(hasattr(module, "subprocess"), module.__name__)


if __name__ == "__main__":
    unittest.main()
