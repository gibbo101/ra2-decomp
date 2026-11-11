# SPDX-License-Identifier: MIT
"""Whole-executable link pipeline (see README.md). Its modules import each other by plain name."""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
