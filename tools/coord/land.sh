#!/bin/bash
# Publish a verified round, then report the new totals.
set -eu
cd "$(git rev-parse --show-toplevel)"
uv run tools/coord/land.py "$@"
echo "### untracked files outside build/"
git status --short --untracked-files=all | grep '^??' | grep -v '^?? build/' || true
echo "### game totals"
python3 tools/coord/totals.py
