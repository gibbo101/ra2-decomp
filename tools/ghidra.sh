#!/usr/bin/env bash
# Analyses orig/gamemd.exe in a Ghidra project under build/ghidra and exports its
# function list to build/ghidra/functions.csv. Re-running reuses the analysed project.
# Ghidra is found through GHIDRA_HOME, else the newest $HOME/opt/ghidra_*; needs Java 21.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
GHIDRA_HOME="${GHIDRA_HOME:-$(ls -d "$HOME"/opt/ghidra_* 2>/dev/null | sort -V | tail -n 1)}"
HEADLESS="$GHIDRA_HOME/support/analyzeHeadless"
[ -x "$HEADLESS" ] || { echo "Ghidra not found: set GHIDRA_HOME" >&2; exit 1; }
PROJ="$ROOT/build/ghidra"
mkdir -p "$PROJ"

if [ ! -d "$PROJ/gamemd.rep" ]; then
    "$HEADLESS" "$PROJ" gamemd -import "$ROOT/orig/gamemd.exe" \
        -scriptPath "$ROOT/tools/ghidra" \
        -postScript ExportFunctions.java "$PROJ/functions.csv"
else
    "$HEADLESS" "$PROJ" gamemd -process gamemd.exe -noanalysis \
        -scriptPath "$ROOT/tools/ghidra" \
        -postScript ExportFunctions.java "$PROJ/functions.csv"
fi
