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

The compiler is not included either. A setup script will fetch it.

## Not affiliated with EA

This is an independent, unofficial project. It is not made, endorsed or supported by
Electronic Arts. *Command & Conquer*, *Red Alert* and *Yuri's Revenge* are trademarks of
Electronic Arts Inc.

## Licence

Not yet decided. See "Legal footing" in `docs/PLAN.md`.
