## Replaying a near match

Run `uv run tools/attempt.py save CANDIDATE.cpp ADDRESS attempt.json --module src/MODULE.cpp`
to bank a C++ candidate as a source patch. Add `--overlay DIR` when the attempt uses header
overrides. Save measures the candidate and rebuilds the patch in isolation before writing
the bundle. To retry it on the recorded commit, run `uv run tools/attempt.py replay attempt.json`.
Replay succeeds when the checker reproduces the saved result, including a saved partial.

A journal function may supply a repository-relative `replay` bundle and a stable `attempt_id`.
The daily CSV retains every distinct bundle path and SHA-256, even if another attempt has a higher score.
Version 2 bundles record matching siblings, lost prior matches and module failures alongside the target verdict.
