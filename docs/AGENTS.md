# Matching one function

You are given one address in `gamemd.exe` (Yuri's Revenge 1.001). Write C++ that the
original compiler turns into that function's exact bytes. MATCH means
`tools/check.py` printed MATCH; nothing else counts.

## Workspace

- Work only in `build/spike/<ADDR>/` (ADDR as `0x00410A60`). Put your source there,
  named after the module `tools/ctx.py` reports (`abstype.cpp`).
- A header you need to change: copy it from `include/` if it is there, else from
  `reference/tibsun/code/`, into `build/spike/<ADDR>/include/` and edit the copy.
- Never edit `src/`, `include/`, `reference/`, `data/`, `tools/` or `docs/`. Other
  agents work at the same time; the coordinator merges results.

## Loop

1. `uv run tools/ctx.py <ADDR>`: extent, module, names, annotated disassembly,
   callers, and the TibSun file to start from.
2. If a TibSun file is listed, find the function there that does what the disassembly
   does (same calls, strings, shape) and start from it, with its `#include` lines. YR
   changed many classes: member offsets are in `data/layouts.csv` (YRpp's names), and
   `include/abstract.h` and `include/abstype.h` show how YR headers differ from TS.
   No TibSun file: write the function from the disassembly.
3. Mark the definition with `// FUNCTION: <ADDR>` on the line before it.
4. `uv run tools/check.py build/spike/<ADDR>/<module>.cpp <ADDR> --overlay build/spike/<ADDR>/include`
5. Read the diff, change the source, repeat.

Stop at MATCH, or after 30 checks without the score rising, or after 45 minutes.

## Rules

- The flags are fixed (`/O2 /Ob2 /Gr /GR /MT /DWIN32`). No inline assembly, `__emit`,
  `#pragma optimize`, hard-coded addresses or `volatile` tricks.
- Name callees and globals as `tools/ctx.py` names them (YRpp or TibSun names); an
  unnamed one is `FUN_<ADDR>` / `DAT_<ADDR>`. Declare each with the calling convention
  the disassembly shows: `this` in ECX is `__thiscall` (any member function); a free or
  static function is `__fastcall` by default under /Gr (first two arguments in ECX and
  EDX), so a CRT function or a `qsort` comparator must be declared `__cdecl`.
- `WRONG TARGETS` means the bytes match but a call or global is declared as the wrong
  thing: fix the declaration, not the code.

## What has worked

- When `__FILE__` and a line number are pushed, pin them:
  `#line 215 "D:\\ra2mdpost\\AbsType.cpp"` on the line before the statement.
- A test of a string's first character compiled to the original's order as
  `if (*name)`, not `if (name[0])`.
- A function placed after its neighbours in the original may be an out-of-line
  definition where TibSun has it inline in the header.
- Register-choice differences: try another form of the same expression, a temporary,
  the order of declarations, or `const` on parameters.

## Report

Return: the result (`MATCH`, `partial` or `failed`), the best percentage the checker
printed, how many checks you ran, the path of your best source, the kind of blocker if
not matched (for example "register allocation", "unknown callee", "header layout"),
and what you learned that would help the next function, in a sentence or two.
