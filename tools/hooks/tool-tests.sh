#!/usr/bin/env bash
# Runs the tool test suite before a commit that changes a Python file under tools/.
set -euo pipefail

git diff --cached --name-only --diff-filter=ACMRD -- 'tools/*.py' 'tools/**/*.py' | grep -q . || exit 0

cd "$(git rev-parse --show-toplevel)"
if command -v uv >/dev/null 2>&1; then
  run=(uv run python)
else
  run=(python3)
fi
if ! out=$("${run[@]}" -m unittest discover -s tools 2>&1); then
  printf '%s\n' "$out" | tail -40 >&2
  echo "tool-tests: the tool test suite fails; fix it before committing a change under tools/" >&2
  exit 1
fi
