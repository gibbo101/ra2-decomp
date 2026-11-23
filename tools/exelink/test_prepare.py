#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""prepare: the link's rows are the manifest's matches plus the reconstructed functions."""

from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import prepare


class ManifestRowTests(unittest.TestCase):
    def test_reconstructed_rows_link_from_source_and_partials_do_not(self):
        with tempfile.TemporaryDirectory() as tmp:
            data = Path(tmp)
            (data / "manifest.csv").write_text("address,size,symbol,source,sha256\n"
                                               "0x00401000,16,_A,src/a.cpp,00\n")
            progress = data / "progress.csv"
            progress.write_text("address,name,result,size,source,symbol\n"
                                "0x00401000,A,MATCH,16,src/a.cpp,_A\n"
                                "0x00401010,B,RECONSTRUCTED,32,src/ra2asm/b.asm,_B\n"
                                "0x00401030,C,82.1%,8,src/c.cpp,_C\n")
            with patch.object(prepare, "DATA", data):
                rows = prepare.manifest_rows(progress)
        self.assertEqual([(r["addr"], r["sym"], r["len"], r["tu"]) for r in rows],
                         [("0x401000", "_A", "0x10", "src/a.cpp"), ("0x401010", "_B", "0x20", "src/ra2asm/b.asm")])


if __name__ == "__main__":
    unittest.main()
