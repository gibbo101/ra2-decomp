# SPDX-License-Identifier: MIT
"""Where the link pipeline finds the toolchain, its generated inputs and its outputs.

Everything the pipeline writes goes under build/exelink/ (ignored by git): the inputs
prepare.py derives from data/ and the player's exe, the object build, and the links.
"""
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
VC6 = ROOT / "toolchain/vc6sp3"
BIN = VC6 / "BIN"
LIBDIR = VC6 / "LIB"
WINEPREFIX = ROOT / "toolchain/wineprefix"
WORK = ROOT / "build/exelink"
INPUTS = WORK / "inputs"
OBJDIR = WORK / "obj"
RETAIL = WORK / "gamemd_1001.exe"    # orig/gamemd.exe with the build's post-link changes put back
BINKDEF = Path(__file__).resolve().parent / "binkw32.def"
MANIFEST = INPUTS / "match_manifest.json"
TOTALS = INPUTS / "progress_totals.json"
BINDINGS = INPUTS / "symbol_bindings.json"
EXTENTS = INPUTS / "function_extents.json"
TAIL_LAYOUT = INPUTS / "crt_tail_layout.json"
RTTI_CLASSES = INPUTS / "rtti_classes.json"

# Every module that reads the original reads this copy unless GAMEMD_BINARY names another.
os.environ.setdefault("GAMEMD_BINARY", str(RETAIL))
