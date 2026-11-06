# Batch coordination

Tools for running matching rounds as parallel agent workflows and landing their results.
Running a workflow needs the maintainer's opt-in each time (CLAUDE.md). Run every tool from
the repository root; generated round files go to `build/coord/`, which is not tracked.

## Making a round

- `uv run python tools/coord/mkround.py tools/coord/batchT.js FIRST NBATCHES [module=cap ...] [big=K]`:
  small untried functions grouped by module, and with `big=K`, batches of eight Opus jobs,
  each the smallest untried function of 1.5 KB or more in a module no other job takes.
- `python3 tools/coord/mknear.py tools/coord/nearT.js FIRST NBATCHES MIN [PER [MAX]]`: near
  misses whose best attempt is MIN% or more (and under MAX%), grouped by module.
- [Evidence-driven retries](CHOICES.md) group a caller with its blocked helpers and test bounded source alternatives.
- `batchT.js` and `nearT.js` are the workflow templates; the generators fill in the
  repository path, the other batches' modules and the description. The workflow's args are
  the generated `build/coord/batchN.json`.
- Both generators skip functions claimed by outside work: `claims.py` reads the open pull
  requests' diffs and open issues labelled `claims`, and stops the generator if GitHub
  cannot be read. Run it alone to list the claims.
- `segfiles.json` maps the unnamed `seg_` ranges to the src/ file each lands in;
  `segnames.md` records how they were named.

## Landing a round

When checking a contribution, pass `--baseline BASE_COMMIT` to `tools/checkall.py`.
Use the trusted base revision even if the contribution edits its own progress table.

1. `uv run tools/checkall.py` (the gate; no regressions allowed), then `--record`, then a
   final re-run with `uv run tools/coord/landing_gate.py` and save its output as GATE.txt.
2. `bash tools/coord/land.sh DATE GATE.txt JOURNAL...`: rule scans (`landscan.sh`), the
   round's claimed matches against the tree (`tally.py`), attempts.csv rows
   (`attempts_rows.py`), modules and manifest, stale banked attempts (`stale.py`), stray
   files and the game totals (`totals.py`, which also gives the fuzzy figure).
3. Update the status lines in README.md and docs/PLAN.md, add the round's section at the
   top of the current month's file in docs/rounds/ (a new month starts a new file, listed
   under Rounds in docs/PLAN.md), add any lessons to docs/AGENTS.md, then commit and push.

Other helpers: `comments.py` (comment-rule scan), `near.py` (near-miss list), `vtowner.py`
and `vtname.py` (vtable owners from RTTI), `rank.py`, `pick.py`, `mkbatch.py`, `jsum.py`
and `segcount.py` from earlier rounds.

For a landing gate with unchanged compilation inputs, `tools/build_receipt.py capture build/gates/BATCH`
runs a cold build and saves verified objects. Use `tools/build_receipt.py recheck build/gates/BATCH --record`
and then `tools/build_receipt.py recheck build/gates/BATCH` to check the current address tables from those
objects. A changed input or damaged object fails the gate.

After source edits, capture into a new directory with `tools/build_receipt.py capture build/gates/next --reuse build/gates/previous`.
Every current module is checked. Changed modules compile again. A shared input change rebuilds all modules.
