# ra2-decomp

A matching decompilation of *Command & Conquer: Red Alert 2 – Yuri's Revenge*
(`gamemd.exe` 1.001): C++ source that the original compiler builds back into the shipped
executable, function by function.

**Made with AI assistance.** Functions are matched by AI agents (Claude) working in batches
under human direction. That does not loosen what counts as a match: a function counts only
when the checker shows its compiled bytes equal the original's, and stand-ins such as casts
over real objects or hard-coded addresses are rejected however well they score.
`docs/AGENTS.md` holds the rules the agents follow and every lesson learned.

**Status (27 October 2026):** 16,107 of the game's 19,366 functions match byte for byte
(83%), covering 2.15 MB of its 3.81 MB of game code (56%). Each function is checked on
its own; linking a whole executable that matches comes later. `docs/PLAN.md` has the
method and a record of every batch.

## Getting started

1. Run `tools/setup_toolchain.sh`. It fetches Visual C++ 6.0 with Service Pack 3, sets it
   up under Wine, and clones the OpenTS TibSun archive (whose headers the build falls back
   on) into `reference/tibsun`. It also clones [YRpp](https://github.com/Phobos-developers/YRpp)
   and builds two local tables from it (`data/symbols.csv`, `data/layouts.csv`, needs
   `clang`). YRpp serves only as a guide to names and layouts; nothing derived from it is
   shipped here.
2. Copy your Yuri's Revenge 1.001 `gamemd.exe` (the Steam copy, MD5
   `2a1307f6f57c9c492f985b4e3afb88e6`) to `orig/gamemd.exe`.
3. Check one file, or everything:

   ```sh
   uv run tools/check.py src/house.cpp            # every annotated function in the file
   uv run tools/check.py src/house.cpp 0x004F5190 # one function
   uv run tools/checkall.py                       # the whole tree
   ```

   Each function prints MATCH, or how close it came and why.

## Layout

- `src/`, `include/`: the reconstructed source. A marker in reccmp's format
  (`// FUNCTION: GAMEMD 0x...`) ties each function to its address in the original.
- `data/`: the map of the executable (`functions.csv`, `modules.csv`), what matches
  (`progress.csv`), proven names for globals (`names.csv`), and every attempt that fell
  short (`attempts.csv`).
- `tools/`: the checker and its helpers.
- `docs/`: `PLAN.md` (method, facts about the binary, progress), `AGENTS.md` (how a
  function is matched, and the lessons learned), `COMMIT_GUARD.md`.

## Verifying a claim

`data/manifest.csv` lists every matched function: address, size, the decorated symbol our
object gives it, the source file, and the SHA-256 of the original's bytes at that address
(so you know exactly which bytes a claim covers). Every file compiles with Visual C++ 6.0
SP3 (`C2.DLL` 12.00.8447) and `/O2 /Ob2 /Gr /GR /MT /DWIN32`; `tools/manifest.py`
regenerates the list. Disagreements with another decompilation's labels at the same
address are welcome as issues.

## Contributing

Read `docs/AGENTS.md` first: it says what counts as a match and what does not (no inline
asm, hard-coded addresses or stand-in structs to force one). Turn on the commit guard
before your first commit (`docs/COMMIT_GUARD.md`); it keeps game files and personal data
out of history.

## You need your own copy of the game

This repository is **source code only**. It contains no game files: no executables, art,
audio, maps, strings or other assets, and it never will. You must own a legal copy of
*Red Alert 2* and *Yuri's Revenge* (for example from Steam) to build or run anything here.

- **Building** reads your own `gamemd.exe`: the checker compares against it, and the build
  takes the executable's resources from it.
- **Running** the result needs your installed game for all of its data.

The compiler is not included either. `tools/setup_toolchain.sh` fetches Visual C++ 6.0 and
Service Pack 3 from archive.org, checks them against pinned hashes and sets them up under
Wine (needs `curl`, `7z`, `cabextract`, `wine` and `python3`).

## Not affiliated with EA

This is an independent, unofficial project. It is not made, endorsed or supported by
Electronic Arts. *Command & Conquer*, *Red Alert* and *Yuri's Revenge* are trademarks of
Electronic Arts Inc.

## Licence

- **Game source** (`src/`, `include/`): GNU GPL version 3 or later, in `COPYING.md`. Part
  of it derives from source Electronic Arts released under the GPL (Command & Conquer,
  Red Alert, Renegade) and from [OpenTS](https://github.com/OpenTS-Developers), so
  Electronic Arts' additional terms at the end of `COPYING.md` apply to that material.
- **Tooling and data** (everything else): MIT, in `LICENSE`.
  `tools/permute.py` and `tools/permute_mutate.py` are adapted from the MIT-licensed
  tools of [Byte Tactics](https://github.com/HectorBailey/byte-tactics) and keep their
  notice.

Neither licence grants any right to Electronic Arts' trademarks or to the game itself.

Every file in `src/` and `include/` starts with this header; the second copyright line
and the last line appear only when the file carries Electronic Arts or OpenTS material:

```cpp
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
```
