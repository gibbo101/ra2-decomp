# ra2-decomp

A matching decompilation of *Command & Conquer: Red Alert 2 – Yuri's Revenge*
(`gamemd.exe` 1.001): C++ source that the original compiler builds back into the shipped
executable, function by function.

**Status:** planning. See `docs/PLAN.md`.

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

Neither licence grants any right to Electronic Arts' trademarks or to the game itself.

Every file in `src/` and `include/` starts with this header; the second copyright line
and the last line appear only when the file carries Electronic Arts or OpenTS material:

```cpp
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
```
