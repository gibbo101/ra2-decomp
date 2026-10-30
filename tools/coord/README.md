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
- `batchT.js` and `nearT.js` are the workflow templates; the generators fill in the
  repository path, the other batches' modules and the description. The workflow's args are
  the generated `build/coord/batchN.json`.
- `segfiles.json` maps the unnamed `seg_` ranges to the src/ file each lands in;
  `segnames.md` records how they were named.

## Landing a round

1. `uv run tools/checkall.py` (the gate; no regressions allowed), then `--record`, then a
   plain re-run.
2. `bash tools/coord/land.sh DATE GATE.txt JOURNAL...`: rule scans (`landscan.sh`), the
   round's claimed matches against the tree (`tally.py`), attempts.csv rows
   (`attempts_rows.py`), modules and manifest, stale banked attempts (`stale.py`), stray
   files and the game totals.
3. Update the status lines and the round's section in README.md and docs/PLAN.md, add any
   lessons to docs/AGENTS.md, then commit and push.

Other helpers: `comments.py` (comment-rule scan), `near.py` (near-miss list), `vtowner.py`
and `vtname.py` (vtable owners from RTTI), `rank.py`, `pick.py`, `mkbatch.py`, `jsum.py`
and `segcount.py` from earlier rounds.
