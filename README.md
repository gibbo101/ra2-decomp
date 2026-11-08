# ra2-decomp

A matching decompilation of *Command & Conquer: Red Alert 2 – Yuri's Revenge*
(`gamemd.exe` 1.001): C++ source that the original compiler builds back into the shipped
executable, function by function.

**Made with AI assistance.** Functions are matched by AI agents (Claude) working in batches
under human direction. That does not loosen what counts as a match: a function counts only
when the checker shows its compiled bytes equal the original's, and stand-ins such as casts
over real objects or hard-coded addresses are rejected however well they score.
`docs/AGENTS.md` holds the rules the agents follow and every lesson learned. Pull requests
from people are just as welcome and go through the same checker and rules (`CONTRIBUTING.md`).

**Status (8 November 2026):** 3.06 MB of the game's 3.81 MB of code matches byte for byte
(80%), in 18,909 of its 19,362 functions (98%). Counting each unmatched function at its best
attempt so far, the fuzzy match is 92%. Each function is checked on its own; linking a whole
executable that matches comes later. `docs/PLAN.md` has the method, and `docs/rounds/`
records every batch.

## Getting started

1. Run `tools/setup_toolchain.sh`. It fetches Visual C++ 6.0 with Service Pack 3, sets it
   up under Wine, and clones the OpenTS TibSun archive (whose headers the build falls back
   on) into `reference/tibsun`. It also clones [YRpp](https://github.com/Phobos-developers/YRpp)
   and builds two local tables from it (`data/symbols.csv`, `data/layouts.csv`, needs
   `clang`). YRpp serves only as a guide to names and layouts; nothing derived from it is
   shipped here.
2. Copy your Yuri's Revenge 1.001 `gamemd.exe` to `orig/gamemd.exe`. Any of these builds
   will do; the checker puts back the few bytes the later ones changed after linking:

   | build | size | MD5 |
   |---|---:|---|
   | retail CD, updated with the official 1.001 patch | 4,813,072 | `937b14db32f07ee47e92a8114ebdbdde` |
   | The First Decade | 4,813,072 | `6604fd6f4ab71a11c708ea423dc0a29e` |
   | Steam | 5,286,208 | `2a1307f6f57c9c492f985b4e3afb88e6` |
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
- `docs/`: `PLAN.md` (method, facts about the binary, status), `rounds/` (a record of
  each round of batches and each set of merged pull requests, one file per month, newest
  first), `AGENTS.md` (how a function is matched, and the lessons learned), `BUGS.md`
  (bugs in the original game, kept for the match and listed for fixing later),
  `MANIFEST.md` (the crosscheck manifest's format) and `COMMIT_GUARD.md`.

## Verifying a claim

`data/manifest.csv` lists every matched function: address, size, the decorated symbol our
object gives it, the source file, and the SHA-256 of the original's bytes at that address
(so you know exactly which bytes a claim covers). C++ uses Visual C++ 6.0 SP3
(`C2.DLL` 12.00.8447) with `/O2 /Ob2 /Gr /GR /MT /DWIN32`. Archived assembly uses
MASM with `/c /coff /Cx`. `tools/manifest.py` regenerates the list. Disagreements with another decompilation's labels at the same
address are welcome as issues.

## Contributing

Only matching work is accepted; see `CONTRIBUTING.md` before you start.

Read `docs/AGENTS.md` first: it says what counts as a match and what does not (no inline
asm, hard-coded addresses or stand-in structs to force one). Turn on the commit guard
before your first commit (`docs/COMMIT_GUARD.md`); it keeps game files and personal data
out of history.

## Credits

- [dk8827](https://github.com/dk8827): matched functions in jumpjet, the capture manager,
  house, team, foot, planning, tactical, isotype, techno, the random map generator,
  netshare, the co-op campaign, text and subtitles, the sidebar and radar, custom dialogs
  and load progress, cells, the display, pathfinding, the INI readers, UI commands, the
  World Domination Tour and the dropship and score screens; brought Westwood's archived
  assembly into the build with MASM; and added the checker's completion checks and
  function-map callbacks.
- [meastes](https://github.com/meastes): matched functions in the map, VQA loading,
  static-sound saving and Techno line drawing, ported from their matching decompilation
  of Red Alert 2 ([meastes/redalert2-decomp](https://github.com/meastes/redalert2-decomp),
  GPL-3.0-or-later).

## You need your own copy of the game

This repository is **source code only**. It contains no game files: no executables, art,
audio, maps, strings or other assets, and it never will. You must own a legal copy of
*Red Alert 2* and *Yuri's Revenge* (for example from Steam) to build or run anything here.

- **Building** reads your own `gamemd.exe`: the checker compares against it, and the build
  takes the executable's resources from it.
- **Running** the result needs your installed game for all of its data.

`tools/setup_toolchain.sh` fetches Visual C++ 6.0, Service Pack 3 and the Processor Pack
from archive.org, checks them against pinned hashes and sets them up under Wine (needs
`curl`, `7z`, `cabextract`, `wine` and `python3`). Only MASM 6.15 and its diagnostics
come from the Processor Pack. The C++ compiler stays at SP3.

## Not affiliated with EA

This is an independent, unofficial project. It is not made, endorsed or supported by
Electronic Arts. *Command & Conquer*, *Red Alert* and *Yuri's Revenge* are trademarks of
Electronic Arts Inc.

## Licence

- **Game source** (`src/`, `include/`): GNU GPL version 3 or later, in `COPYING.md`. Part
  of it derives from source Electronic Arts released under the GPL (Command & Conquer,
  Red Alert, Renegade, Generals) and from [OpenTS](https://github.com/OpenTS-Developers), so
  Electronic Arts' additional terms at the end of `COPYING.md` apply to that material.
- **Tooling and data** (everything else): MIT, in `LICENSE`.
  `tools/permute.py` and `tools/permute_mutate.py` are adapted from the MIT-licensed
  tools of [Byte Tactics](https://github.com/HectorBailey/byte-tactics) and keep their
  notice.

Neither licence grants any right to Electronic Arts' trademarks or to the game itself.

The standalone assembly files retain the pinned archive's licence headers. C++ files in
`src/` and `include/` start with this header; the second copyright line and the last line
appear only when the file carries Electronic Arts or OpenTS material:

```cpp
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
```
