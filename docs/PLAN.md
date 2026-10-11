# Red Alert 2: a matching decompilation of Yuri's Revenge

Status, 6 October 2026: parked, waiting for token budget. Folder holds this plan only:
no code, no tools, no binaries. Next step is the spike under "First session".

## The goal

Rebuild the source of `gamemd.exe` (Yuri's Revenge 1.001) as C++ that the original
compiler turns back into the shipped bytes, function by function. Matched source is
readable, provably faithful, and portable afterwards: a Linux and Steam Deck build of
Westwood's own engine, in the mould of `../opents-linux` and `../opents-pad`, and an
authoritative reference for RA2 unit ports into Tiberian Factions.

## Scope: v1 is the game as shipped

Decided 11 October 2026.

- **Required: every function matches.** Every function's compiled bytes equal the
  original's, apart from bytes the linker fills in. The checker decides; nothing is called
  matched on judgement. This includes the 48 MASM objects (reassembled to match) and the
  third-party library code (linked as the original objects, not rewritten).
- **Goal, not a gate: a matching MD5.** The rebuilt `gamemd.exe` hashes to
  `2a1307f6f57c9c492f985b4e3afb88e6`. That also needs data sections, BSS order,
  vtable/RTTI placement, link order, imports and resources to line up. Track it as a
  separate score next to the function count; v1 ships without it if it proves out of reach.
- **No changes to the game.** No enhancements, features or bug fixes. Every bug, desync and
  bit of undefined behaviour stays as shipped. Matching enforces most of this by itself,
  because a fixed function no longer matches.
- **v1 is then frozen.** Once v1 is done, this repo becomes an unmaintained reference.
  It can keep taking pure matching work toward the MD5 goal, but nothing else.
- **v2 onwards lives in a separate repo** that starts from v1. That is where changes go:
  the Linux and Steam Deck port, controller support, fixes and features. This is the same
  split as OpenTS (the reference) and `opents-linux` / `opents-pad` (the forks).

## The model: Byte Tactics (Total Annihilation)

<https://github.com/HectorBailey/byte-tactics>, announced on r/TotalAnnihilation in
October 2026. 100% matched: all 3,267 functions of `TotalA.exe` v3.1 (about 850 KB),
whole exe rebuilds to the shipped MD5, 5,630 commits.

How they did it:

1. **Compiler.** Read from the binary: Visual C++ 5.0 SP3, flags `/O2 /Ob2 /MT /Gz`,
   confirmed by matching the C runtime byte for byte.
2. **Function map.** Debug records left in the exe gave start, size and argument count
   for 3,782 functions.
3. **Agents.** Ghidra pseudo-C for every function; work split into GitHub issues of a few
   functions each. Claude Code, OpenCode and Codex agents claimed an issue, wrote C++,
   compiled with the real compiler under Wine, diffed against the original, retried until
   MATCH or no progress.
4. **Hand-on.** Cheap models for easy functions; stalled ones left with notes for a bigger
   model. Every working trick went into a shared guide. A human orchestrator re-checked each
   function before merge.

Their agent guide (`AGENTS.md`) in short:

- Claim: `gh issue list --label decomp --state open --search "no:assignee"`, self-assign,
  comment with timestamp and host.
- Per function: `tools/ctx.py <addr>` (disassembly, callees, calling convention, Ghidra
  pseudo-C); search `src/` for a matched look-alike to adapt; `tools/sources.py` /
  `tools/modules.py` pick the file (`src/<folder>/<module>_<address>.cpp`); loop
  `tools/check.py <addr>` until MATCH; `tools/checkall.py` for batches.
- Stuck = no score gain in 30 runs or 60 minutes. At 90% or better, `tools/permute.py`
  tries 15 minutes of meaning-preserving rewrites.
- Rules: touch only your issue's files; exact names from `data/symbols.csv`; no inline asm,
  `#pragma optimize`, hard-coded addresses or `volatile` tricks; MATCH only when the checker
  says so.
- One PR per issue with a table: address, model, result, best %, check runs, notes.
- Layout: `toolchain/msvc5-sp3/BIN/CL.EXE`, `orig/TotalA.exe`, `build/scratch/<addr>/`.
  The setup script fetches the compiler; the player brings the game.
- Licence: tooling MIT, reconstructed `src/` under no licence.

## What we have that they didn't

### OpenTS is most of the way there already

`../cnc-remastered-mods/reference/OpenTS` (and the maintainer's forks `../opents-linux`,
`../opents-pad`) is a per-function matching reconstruction of Tiberian Sun 2.03. Its
`docs/RATIONALE.md` says the goal was "to binary-match every function, the data they
reference"; about 500 functions don't match and most data sections don't. It also says
a future RA2 decompilation may reveal how to match some of those.

Yuri's Revenge is the Tiberian Sun engine carried forward: `gamemd.exe`'s debug record
still names the image `SUN.exe` with `tsun.dbg`. So a large share of YR's functions
will be OpenTS functions with RA2 edits, and OpenTS also gives us:

- module file names and order (recovered from debug strings and the alphabetical link
  order; RATIONALE.md explains the method);
- class layouts, enums and the `.hh` definition-header scheme;
- string and dialog names, many of which it recovered *from* RA2's string table;
- a working toolchain recipe: Rich-header compiler identification, Ghidra-style pseudo-C,
  objects split from the binary, and [objdiff](https://github.com/encounter/objdiff) for
  comparison.

Read `RATIONALE.md`'s "Why the binary does not match exactly" before scoping: MSVC-era
inlining, `static const` values, BSS ordering by name hash, include order and vtable/RTTI
placement are the known traps, and a whole-exe match may be out of reach without the
original `.dbg`. That is why v1 requires per-function matching, as OpenTS chose, and
treats the whole-exe MD5 as a goal (see "Scope").

### The YR modding community's maps

- **YRpp** (Phobos-developers on GitHub): reverse-engineered headers for `gamemd.exe`
  1.001, class layouts and function addresses, built up over about 20 years. The starting
  `symbols.csv`.
- **Phobos** and **Ares**: engine extensions hooked into `gamemd.exe` via Syringe. Their
  hook sites document what many functions do.
- **Vinifera** (Tiberian Sun's equivalent of Phobos) and its TSpp headers: the TS-side
  cross-check.

### The Westwood lineage in the open

EA's GPL source for TD and RA (`../CnC_Red_Alert`, `../CnC_Tiberian_Dawn`), Renegade
(`../CnC_Renegade`, sharing `wwlib`) and Generals (`../CnC_Generals_Zero_Hour`, same
compiler era). Many RA2 classes are the RA classes grown up: same names, same patterns.

### Other matching decomps to learn from

- [homm1-decomp](https://github.com/sushi-shi/homm1-decomp) (Heroes of Might and Magic 1):
  the closest prior art. Built with **VC6 SP5 under Wine**, agent-driven (`AGENTS.md`,
  `CLAUDE.md`, `.claude/`, `.agents/skills/`), complete: 1,057/1,057 functions in
  `HEROES.EXE` and 489/489 in `EDITOR.EXE`, and both exes rebuild byte-identical to retail.
  `homm1 build verify` checks every gate including the linked-exe byte compare, in a
  "strict data references" mode. Proof that a whole-exe VC6 match is reachable; about a
  fifth of YR's size. Its author reports Opus 5.5 reaching byte-exact output with clean
  names and little hand-holding.
- [reccmp](https://pypi.org/project/reccmp/) (from the LEGO Island decomp, MSVC 4.20):
  the most mature open toolset for MSVC matching. Functions are tied to addresses by
  source comments (`// FUNCTION: <module> 0x...`); it compares original against rebuilt
  using the rebuilt PDB and reports per-function accuracy. Its maintainers note that an
  unused inline or enum added to a header shifts the codegen of about 5% of functions:
  OpenTS's include-order trap again.

### Further references

- **ModEnc** (RA2/TS INI wiki): every `rules.ini` tag is a string the INI readers look up,
  so tags map almost directly to member names.
- **XCC Utilities source**: open parsers for MIX, SHP, VXL, HVA, CSF and maps; confirms
  what the file-format functions do.
- **CnCNet's YR spawner and patches**: more reversed addresses, mostly networking and
  startup.
- **DirectX 7/8 SDK and WOLAPI headers**: needed to compile at all, since struct layouts
  must match the era. WOLAPI may already be in `../CnC_Renegade`.
- **VC6 CRT/STL Function ID (FLIRT) signatures** for Ghidra: tag the ~200 prebuilt library
  objects so no agent spends tokens on them.
- **`game.exe`** (RA2 without YR), already installed: a second build of nearly the same
  code five months earlier. Diffing a function across both separates compiler quirks
  from source differences.
- **The Phobos / C&C Mod Haven Discord**: twenty years of IDA and Ghidra databases for
  `gamemd.exe` may be shareable, and it's the place to test the community's feeling about
  a matching decomp early.

## The binary (measured 6 October 2026)

Steam install: `~/.steam/steam/steamapps/common/Command & Conquer Red Alert II/`
(app 2229850).

| | `gamemd.exe` (YR) | `game.exe` (RA2) |
|---|---|---|
| Size | 5,286,208 | 5,077,312 |
| MD5 | `2a1307f6f57c9c492f985b4e3afb88e6` | `799c80c40f28a350b64e52f44f02ea9a` |
| Link date (UTC) | 2001-10-31 01:30:54 | 2001-06-06 23:25:33 |
| Linker | 6.0 | 6.0 |
| `.text` | 4,060,045 bytes | 3,769,805 bytes |
| Debug record | MISC: `tsun.dbg`, `SUN.exe` | same |

The Steam copy is the 1.001 build YRpp and the recomp target; confirm the MD5 against
YRpp's before starting. No FPO records, so unlike TA the exe does not hand over function
bounds: take them from YRpp, the recomp's catalogue, and Ghidra.

Rich header of `gamemd.exe`, decoded with the VC6-era product IDs (verify the names
against a current Rich-header table):

| Product ID | Build | Objects | Likely meaning |
|---|---|---|---|
| 11 (C++ compiler, VC6) | 8447 | 513 | the game's own C++ modules |
| 10 (C compiler, VC6) | 8447 | 1 | one C module |
| 10 / 11 | 8168 | 189 / 12 | prebuilt libraries (CRT and the like), VC6 RTM |
| 14 (MASM 6.13) | 7299 | 48 | Westwood's assembly: blitters, LCW, etc. |
| 19 | 8022, 8034 | 13, 27 | third-party library objects, to identify |
| 4 (linker 6.0) | 8447 | 15 | |
| 1 (imports) | 0 | 338 | |
| 12 / 6 | 7291 / 1735 | 18 / 1 | alias objects, resource converter |

So the compiler is VC6, `cl` 12.00.8447 (a VC6 service pack; pin down which). Flags still
to find, by matching CRT functions as Byte Tactics did. About 513 C++ modules is a fair
first count of source files. The 48 MASM objects are reassembled, not decompiled.

The MD5 goal needs the whole toolchain the header records, not just `cl`: the prebuilt
libraries at build 8168 (VC6 RTM CRT), MASM 6.13.7299, the product-19 third-party objects,
and linker 6.00.8447. The PE timestamp and checksum must also come out identical. The
565 KB `.rsrc` (EA's dialogs, icons and strings) cannot go in the repo, so the build
extracts it from the player's own exe.

No DRM wrapper (checked 11 October 2026): the exe has only `.text`, `.rdata`, `.data` and
`.rsrc`, none of SafeDisc's extra sections or helper files. Westwood's own CD lookup is
still compiled in (`Calling Force_CD_Available`, `Init CDROM`, `TXT_INSERT_CD_DIALOG`), the
same logic as `Force_CD_Available` in EA's GPL Red Alert source. v1 matches it like any
other code. It is plain game logic, not protection, but tag those functions in the queue
so a refusal goes to a by-hand batch instead of stalling a bulk run.

Scale against Byte Tactics: `.text` is about 4.8 times TA's whole exe. The recomp project
counted about 24,900 functions and 954 classes in `gamemd.exe`, including library code.

## Other RA2 projects (none of them a matching decomp)

- [redalert2-recomp](https://github.com/sp00nznet/redalert2-recomp): static
  recompilation of `gamemd.exe` to 5.8M lines of machine-lifted C. Playable (menus,
  skirmish, both campaigns), D3D11 presenter, 2× voxel units. Windows only. The lifted C
  is generated locally, never distributed. Useful for its function and class catalogue.
- [VERA20k](https://github.com/ckbk456/vera20k): a Rust rewrite of the YR engine, one
  mechanic at a time, aiming at 30 players and 20,000 units.
- A Codex (GPT-5.6) behavioural rebuild of YR, 624k lines of C++ in 26 days, about
  $400, September 2026
  ([Incrypted](https://incrypted.com/en/codex-recreated-the-video-game-red-alert-2-in-26-days-without-the-original-source-code/)).
  Matched by watching, not by bytes; no public repo found.
- OpenRA's RA2 mod: a separate engine reading the assets.

## Legal footing

EA released TD, RA, Renegade and Generals as GPL and has not released TS or RA2.
Reverse-engineered reconstructions of this engine family are published openly and stay
up: OpenTS ships builds from a public GitHub org with a Patreon; Ares, Phobos and
Vinifera have published reverse-engineered YR and TS code for years. Follow the same
footing:

- no game files, no compiler binaries in the repo; the player brings the game, a setup
  script fetches the compiler;
- source code only: the README says plainly that you must own the game to build or run
  anything, and that the project is not affiliated with EA;
- a repo of its own, kept separate from Tiberian Factions' Workshop presence;
- private through the spike; talk to the Phobos community before it goes public.

Licence to decide, with one constraint: OpenTS is GPL-3, so tier 0 (compiling OpenTS's
function unchanged) copies GPL code into `src/`, and `src/` must then be GPL-3. Byte
Tactics' no-licence stance only stands if tier 0 is dropped. YRpp and Ares carry no licence
file: use them for facts (addresses, layouts, behaviour), never paste their headers verbatim.

## First session: the spike

Goal: a cost per matched function, so the whole job can be priced before committing.

1. **Toolchain.** Get VC6 build 12.00.8447 running under Wine. Decided 11 October 2026:
   no original disc is available, so the compiler is downloaded, the same way the other
   decomps get theirs.
   - A setup script fetches the base VC6 install and the service pack whose `cl` is
     12.00.8447 (believed to be SP3, possibly also SP4: confirm), checks every download
     against a SHA-256 pinned in the script, and installs into the gitignored `toolchain/`.
     The compiler is never committed or redistributed.
   - Gate: `cl` must print **12.00.8447** before anything else runs. A near miss is not
     good enough, because each service pack can emit different code.
   - Not the SP6 portable that TheSuperHackers' CI pulls (`itsmattkc/MSVC600`, pinned commit
     and SHA-256): wrong build for us, but its fetch-and-verify step is the pattern to copy.
     The base install's libraries (build 8168) match the Rich header's prebuilt objects.
   - Sources found 11 October 2026 (archive.org; SHA-1 as archive.org lists it, pin SHA-256
     after the first download):

     | What | Item | File | SHA-1 |
     |---|---|---|---|
     | Base VS6 Enterprise | `en_vs6_ent` | `en_vs6_ent_cd1.iso` (643 MB) | `25e382cabcf6c648a327db3b90ff38962bfc0e84` |
     | SP3 (May 1999) | `ms-technet-tnsb9908` | `TNSB9908.iso` (594 MB), in `VSTUDIO/SP3/` | `bf599ed15861b69ea89ffa52626eb252df2053be` |
     | SP4 (fallback) | `X05-86592` | `2000-08_X05-86592_X05-78367_vs6sp4.7z` (379 MB) | `05a28ded15429ae0ea10b27ef646056c565f5284` |
     | SP5 (reference only) | `vs6sp5` | `vs6sp5.exe` (136 MB) | `6af12fb3e2c7c271be67c9c9833c63b82bb7b9bf` |

     CD1 should hold Visual C++; CDs 2 and 3 are not expected to be needed. The SP3 files
     on the TechNet disc are `VS6SP3_1.EXE` to `VS6SP3_10.EXE` plus `VS6SP3S1/S2.EXE`.
   - MASM 6.13 (build 7299) is only needed for the 48 assembly objects; source it later.
   - Read homm1-decomp's `AGENTS.md` and Wine setup first (VC6 SP5, CC0, already working).
     The Generals Docker/Wine route (`../generals-pad/docs/PLAN.md`) and Byte Tactics'
     setup script are the other references.
2. **Flags.** Match a handful of CRT functions to fix the compiler flags.
3. **Symbols.** Pull YRpp's addresses and class layouts into a `symbols.csv`; cross-check
   function bounds with Ghidra.
4. **Checker.** A `check.py <addr>` that compiles one function and diffs it against
   `gamemd.exe`, ignoring linker-filled bytes. reccmp, objdiff, or a port of Byte
   Tactics' or homm1-decomp's tools: adopt before building.
5. **Whole-exe build.** Split the original into one object per module so every commit
   links a complete, launchable `gamemd.exe`, each function swapping in as it matches.
   Try matching one module's data section too: the first evidence on whether the MD5 goal
   is reachable.
6. **Twenty functions.** Mixed sizes, half with an OpenTS counterpart and half RA2-only.
   Record for each: matched or not, best %, check runs, tokens spent.
7. **Price it.** Scale the measured cost to about 20k game functions (less what OpenTS
   carries over), and decide.

Steps 1 to 5 are one interactive session with an Opus coordinator: tooling, no fan-out.
Step 6 is where parallel agents pay, if the maintainer opts in to a workflow for it.

### Decide before the first batch

- **Module layout.** Which function lives in which source file, and the link order, decide
  whether the MD5 can ever match. Start from OpenTS's module list for the TS side. Moving
  one function later is cheap; reshuffling data layout across 500 modules is not.
- **Shared headers.** A header edit can change the codegen of matched functions elsewhere
  (reccmp's 5%). The merge gate re-checks every matched function, not just the claimed
  one, and shared class headers need an owner or a claim, or parallel agents will break
  each other's matches.
- **Name precedence.** Westwood's own names (assert strings, OpenTS) over YRpp's, over
  invented ones. Fixed once in `symbols.csv` before agents start naming.

## Playtest install

Matching never runs the game: the checker diffs against a copy of the original in `orig/`.
A second install is only for playing the rebuilt exe, and is needed once the build links a
whole `gamemd.exe`, not before.

- **Never touch the Steam install.** Steam's file verification or an update restores the
  original exe without warning, and the Steam install is the untouched reference.
- **Copy the whole folder** (1.9 GB) to a gitignored location. Keep Steam's extras:
  `ddraw.dll` (DDrawCompat, the renderer fix), `wsock32.dll` (IPXEmu, for LAN play) and
  the INIs, so the copy behaves exactly like the Steam version.
- **Launch `gamemd.exe` directly** as a non-Steam game under Proton, or from a script.
  Steam's own entry goes through the `RA2MD.exe` launcher; confirm `gamemd.exe` starts on
  its own.
- **Baseline first.** Run the original `gamemd.exe` from the copy before swapping ours in,
  so any difference is down to the build. After that the copy is an A/B rig: the same
  folder, swapping between the two exes.
- The same copy goes to the Steam Deck later.

## Agent line-up

The checker is the judge, so coordination is bookkeeping (queue, claim, check, record,
escalate) and needs no premium model. Prices are API rates per million tokens
(input/output), cached 2026-09-25; on the Max plan they show up as relative burn rate.

| Tier | Who | Takes |
|---|---|---|
| 0 | A script, no model | Every YR function with an OpenTS counterpart: compile OpenTS's version unchanged and diff it. Exact matches are free. |
| 1 | Sonnet 5.5, medium effort ($2/$10) | Fresh functions and tier-0 near misses. Expected to land most small and mid-sized ones. |
| 2 | Opus 5.5, high or xhigh ($4/$20) | Tier-1 stalls (no gain in about 30 checks) and large functions. |
| 3 | Fable 5.1 ($10/$50) | The residue: register allocation and instruction order. |

Alongside them:

- **Coordinator:** Opus 5.5. Builds the tools, triages, reviews PRs. Fable is not needed here.
- **Guide-keeper:** Opus 5.5, after each batch, folds new techniques into the shared
  matching guide, so later agents start from solved patterns.
- **Merge gate:** a script, not an agent: re-run the checker on every claimed MATCH before
  merge.
- **Haiku 4.5:** no role. Matching needs hard reasoning over assembly, and 200K context is
  tight beside a large function plus the guide.

The spike runs the 20 functions two ways: the tier ladder, and Opus 5.5 alone at medium
effort (one model, one prompt cache, often fewer retries). Cost per matched function
picks the line-up.

## Budget: run it piecemeal

The plan's usage is shared with other work, so this project never runs flat out against
the weekly limit. It is built to be stopped and resumed at any point:

- **The queue lives in the repo** (one GitHub issue, or one row in a tracked CSV, per few
  functions), never in a session. Any session picks up where the last stopped.
- **Batches, not marathons.** One batch = a fixed number of issues at a fixed concurrency
  (start at 3 to 5 agents). Check `/usage` before each batch; stop at the share of the week
  set aside for it.
- **Off-peak first.** Batches run when the plan isn't needed for other work.
- **Fable only by hand.** Tier 3 runs only when the maintainer OKs a residue batch.

Sizing comes from the spike, not a guess: plan usage spent on the 20 test functions,
divided by the number matched, times the functions left after tier 0, gives the total in
"weeks of spare budget". Record that figure here once measured.

## Open questions

- Does `game.exe` (RA2 without YR) share enough to be worth matching too, or is YR the
  only target? (Which service pack 8447 is gets settled in spike step 1.)
- What fraction of YR functions have an OpenTS counterpart? Measure it in the spike by
  comparing function lengths and call graphs.
- Is there any surviving `tsun.dbg` or `gamemd` debug build? A symbol file would make
  a whole-exe match possible.
- Is the whole-exe MD5 reachable? Matching one module's data section during the spike
  would show early whether data layout is a wall.
- v2's port route: OpenTS's and Vanilla Conquer's, as `opents-linux` took?
