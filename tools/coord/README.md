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
- Both generators skip functions claimed by outside work: `claims.py` reads the open pull
  requests' diffs and open issues labelled `claims`, and stops the generator if GitHub
  cannot be read. Run it alone to list the claims.
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

## Retrying functions with new evidence

`mknear.py` leaves addresses with two or more attempt records out of automatic
percentage-based rounds. Put a caller, its dependent helpers, a concrete hypothesis
and the new evidence in a JSON list instead. For example:

```json
[{
  "name": "missile",
  "address": "0x005B20F0",
  "helpers": {"0x005B2930": "DirType::operator+"},
  "hypothesis": "Materializing the boost before the turn may restore the missing call.",
  "evidence": "The retail caller calls the addition helper once.",
  "source": "build/spike/0x005B20F0/missile.cpp",
  "choices": [{"find": "(rot + 256) / 2", "alternatives": ["(rot + DirType(256)) / 2"]}]
}]
```

`uv run tools/coord/choices.py plan jobs.json` writes near-workflow job arguments,
keeping callers and helpers together and excluding groups claimed on GitHub.
`uv run tools/coord/choices.py run jobs.json missile build/trials/missile --budget 12`
tests the original and combinations of the choices, up to 12 compiler invocations
including errors. Each find string must be unique in the source. Optional `overlay`
selects the header overrides. Each distinct source and its per-target code/relocation
fingerprints are kept locally. Use the regular whole-module checker on winning sources.
To benchmark a search, start with a perturbed known match, conceal the winning choice
among the alternatives, and count recovered functions and actual compiler invocations.
