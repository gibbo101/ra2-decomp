# Retrying functions with new evidence

Run `uv run tools/coord/choices.py suggest` to list unclaimed callers with explicitly waiting helpers in the latest attempt records. Review each dependency before planning work.

`mknear.py` leaves exhausted retries out of automatic percentage-based rounds. Count recorded
compiler checks in non-improving rounds since the best score rose (30 checks, or 60
for functions over 2,000 bytes). Checks in a round that raised the best are excluded
because the ledger does not say when the gain occurred. Put a caller, its dependent helpers, a concrete hypothesis
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
