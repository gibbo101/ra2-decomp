# Red Alert 2: a matching decompilation of Yuri's Revenge

Status, 11 October 2026: spike in progress (see "First session").

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
- [Open-BFME-1](https://github.com/Open-BFME/Open-BFME-1) (Battle for Middle-earth 1,
  MSVC 7.1, GPL-3): an AI agent fleet pushing one verified function per commit, 59k
  commits by October 2026; progress self-reported, anywhere from 8% to over 50%. Different
  compiler and engine (SAGE, ported from Generals source), so no code carries over; its
  `AGENTS.md` and `docs/matching.md` are the reference for agent logistics. Taken from it:
  relocation targets are checked, not just skipped (step 4); attempts are logged and near
  misses banked (step 6); unproven functions keep address-based names ("Name
  precedence"); a vague agent prompt "produced zero progress" in six measured sessions.
  Not taken: it commits its compiler and the retail exes, and its 218 tools and 40-60
  minute full gate are a warning about tooling sprawl.

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

**The Steam exe is not pristine 1.001** (found 11 October 2026). Ares identifies YR builds
by whole-file size and CRC-32: 1.001 retail CD is 4,813,072 bytes, CRC `0x098465B3`;
First Decade `0xEB903080`; Ultimate Collection (Origin) 4,816,864 bytes, `0x1B499086`;
all share link timestamp `0x3BDF544E`. The Steam exe has that timestamp but is 5,286,208
bytes with CRC `0xA3F19485`, and its PE checksum is stale, so it was changed after
linking:
- **Code patched in place.** The CD-drive scan at 0x4E60E1 (a loop over drive letters
  'a' to 'z') has its loop branch overwritten with `nop`s; 0x751C8E has six bytes
  blanked; the function at 0x55CFD0 starts with `nop`s and jumps over its own body. These
  are the sites found so far, from stray `nop` runs; others may not leave `nop`s.
- **Resources replaced.** `.text`, `.rdata` and `.data` sit at the same file offsets as a
  4.8 MB exe would have them; the growth is all in `.rsrc` (565 KB against about 100 KB).

So patched functions cannot match compiled source, and the MD5 goal as written (the Steam
exe's) needs EA's post-link patch reproduced. A pristine 1.001 would be the clean target,
but Westwood's 1.001 patch (`YR1001EN.EXE`, 2.38 MB) is an RTPatch 5.20 binary diff that
only updates an installed retail 1.000 `gamemd.exe`; it does not carry the exe.

**Decided 11 October 2026: the target is the Steam exe.**
- The source is Westwood's 1.001, CD check included; v1 still means the game as shipped.
- Each EA patch site is recorded in the repo: address, the bytes Westwood's code would
  have, the bytes EA wrote, and the evidence. EA likely changed more than the CD check
  (compatibility for newer systems), so sites also surface during matching: a function
  that differs from the Steam bytes only in a small patch-shaped spot.
- The checker counts a function as matched when its only differences lie at recorded
  sites. The whole-exe build applies EA's patches after linking, as EA did, so the MD5
  goal stays the Steam exe's.
- A clean 1.001 copy can still replace the guesswork later: Ares' size and CRC verify
  one if the community has it.

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
- private through the spike; talk to the Phobos community before it goes public. The
  OpenTS Discord already favours byte matching (October 2026: "verify identical ASM with
  the same compiler and options" is how to do it) and is cool on non-matching AI ports
  and the recomp, so open with the checker, not with the agents.

Licence to decide, with one constraint: OpenTS is GPL-3, so tier 0 (compiling OpenTS's
function unchanged) copies GPL code into `src/`, and `src/` must then be GPL-3. Byte
Tactics' no-licence stance only stands if tier 0 is dropped. YRpp and Ares carry no licence
file: use them for facts (addresses, layouts, behaviour), never paste their headers verbatim.

## First session: the spike

Goal: a cost per matched function, so the whole job can be priced before committing.

1. **Toolchain. Done 11 October 2026.** No original disc is available, so the compiler is
   downloaded, the same way the other decomps get theirs. `tools/setup_toolchain.sh`
   fetches the base VC6 disc and the SP3 disc, checks both against pinned SHA-256s, lays
   SP3's compiler files over the base `VC98/` tree in the gitignored `toolchain/vc6sp3/`,
   checks the installed compiler and linker files against pinned SHA-256s, and runs the
   gate. `tools/wcl` runs `cl` under Wine (`tools/wlink` and `tools/wlib` the linker and
   librarian), with the repo mapped to drive `Y:` because the compiler's output depends on
   the paths it sees (Byte Tactics measured this). The compiler is never committed.
   - **Gate: an object's `@comp.id` reads product 11, build 8447,** the Rich header's entry
     for the game's 513 C++ modules (`tools/compid.py`). Not the `cl` banner: no service
     pack updates `CL.EXE`, so it prints 12.00.8168 whatever sits behind it.
   - **What SP3 changes**, read from `sp398ent.inf` and the files' version info:
     `C2.DLL` 12.00.8447 (the code generator, shipped disguised as `msvcep.dll` in
     `VS6SP3_1.EXE`, and the only file that sets the 8447 stamp), `C1.DLL` and `C1XX.DLL`
     12.00.8472, `LINK.EXE` 6.00.8447, `CVTRES.EXE` 5.00.1736, the CRT libraries (`LIBC`,
     `LIBCMT`, `MSVCRT` and debug variants), `ATL`, `OLEAUT32` and `UUID` libs and
     `OLEAUTO.H`. All but `C2.DLL` sit in `VS6sp3_4.cab` inside `VS6SP3_5.EXE`. `CL.EXE`,
     `LIB.EXE` and `MSPDB60.DLL` stay at the base disc's 8168. The setup script installs
     the whole SP3 set, as SP3's installer would have on Westwood's machines. On a probe
     the 8168 and 8472 front ends emit identical objects; if a function ever differs
     between them, that is the place to test.
   - The same package holds two other `C2.DLL` builds for other editions (`msse.dll`
     11.99.8447, `intro.dll` 11.98.8447); not ours. Linker 6.00.8447 matches the Rich
     header's product-4 entry.
   - Sources (archive.org). SHA-256 pinned in the script for the two discs used; SP4 and
     SP5 are kept for reference only:

     | What | Item | File | SHA-256 |
     |---|---|---|---|
     | Base VS6 Enterprise CD1 | `en_vs6_ent` | `en_vs6_ent_cd1.iso` (643 MB) | `d38250b8…ae0347` |
     | SP3 (May 1999) | `ms-technet-tnsb9908` | `TNSB9908.iso` (594 MB), in `VSTUDIO/SP3/` | `f3eefbd0…a004a928` |
     | SP4 (reference) | `X05-86592` | `2000-08_X05-86592_X05-78367_vs6sp4.7z` (379 MB) | `09ea2026…82ba831` |
     | SP5 (reference) | `vs6sp5` | `vs6sp5.exe` (136 MB) | `10a83404…6a604` |

     Downloads retry with stall detection (`curl --speed-limit 10240 --speed-time 30 -C -`):
     archive.org transfers stall on network changes and resume cleanly.
   - MASM 6.13 (build 7299) is only needed for the 48 assembly objects; source it later.
   - Risk: Open-BFME reports Ubuntu 24.04's Wine 9.0 (the one here) hanging MSVC 7.1's
     `cl.exe` on some files. Not seen with VC6 yet. If it happens: WineHQ 11, or
     [Wibo](https://github.com/decompals/wibo), a small Win32 loader that is also faster
     per compile.
2. **Flags. Done 11 October 2026: `cl /c /O2 /Ob2 /Gr /GR /MT`.** The CRT is linked as
   prebuilt objects, so matching it only identifies the library. The flags came from
   compiling Westwood's own code and diffing it against the exe:
   - **/MT, SP3's LIBCMT.** No CRT DLL is imported; the entry point is LIBCMT's
     `WinMainCRTStartup`. Base and SP3 LIBCMT differ only in `strftime.obj` and
     `tzset.obj`, and the exe holds SP3's (`__Strftime` and `_cvtdate` match only SP3's).
   - **/O2 /Ob2.** `RawFileClass::Open(name, rights)`, `Size`, `Bias` and `Seek` (464
     bytes, with `Raw_Seek` auto-inlined twice) match byte for byte, from wwlib's source
     as OpenTS and Renegade carry it, on the first compile. /Ob1 does not inline
     `Raw_Seek`; /O1 and /G6 change the code; /G5 is the same as the default blend.
     /O2 also implies /Gf, which fits: game string literals are pooled into `.data`.
   - **/Gr.** Static members such as `CampaignClass::FindIndex` (0x46CC90) take their
     argument in ECX and return with a plain `ret`, and the game's qsort comparators are
     marked `__cdecl` explicitly. /Gr reproduces `FindIndex` except for one register
     choice (EAX against ECX for the element pointer): a source-shape question for step 6.
   - **/GR, no /GX.** 962 RTTI type descriptors, no `__CxxFrameHandler`; the only
     exception frames are `__try` blocks (`_except_handler3`, mostly CRT, one game
     function at 0x6BB9A3).
   - **No /Op, no /QIfist.** `ObjectClass::Is_Red`-style health check at 0x5F5CD0 (double
     compare against `Rule->ConditionRed`) matches without /Op. Float-to-int goes through
     2,029 calls to a helper at 0x7C5F00, which is not LIBCMT's `__ftol`: the linker took
     `__ftol` from another library first. Identify it before the whole-exe link.
   - Still to confirm when a function needs it: `/Zp` (default 8 assumed).
   - **Most MASM objects are the CRT's.** LIBCMT carries 73 MASM 6.13.7299 objects, and
     the code of 34 of them is in the exe (`memcpy`, `strlen`, `stricmp`, `llmul`,
     `chkstk`, the x87 helpers and so on), linked prebuilt. So at most about 14 of the Rich
     header's 48 MASM objects are Westwood's or third-party assembly, far less MASM work
     than "Scale" assumed.
3. **Symbols.** Pull YRpp's addresses and class layouts into a `symbols.csv`; cross-check
   function bounds with Ghidra. **Done 11 October 2026.**
   - `tools/yrpp_symbols.py` extracts YRpp's facts (address, kind, qualified name,
     convention, header) into `data/symbols.csv`: 1,808 functions, 511 globals, 22 vtable
     constants, 103 import slots. Two of its function addresses are wrong (they fall
     inside other instructions: `MPSiegeClass::SpawnBaseUnits` 0x5CAA50,
     `TeamTypeClass::CanRecruitUnit` 0x6F1320).
   - `tools/functions.py` maps every function into `data/functions.csv`: 19,826 starts,
     each with its evidence (alignment padding, call target, vtable or callback pointer,
     YRpp, Ghidra). VC6 pads game functions with `nop` to 16 bytes, which alone finds
     about 18,000; a start without padding must follow a return, jump, no-return call or
     switch table. It re-synchronises its sweep after the switch tables VC6 keeps in
     `.text`, and drops 158 data values that only look like code pointers.
   - `tools/yrpp_layouts.py` has clang lay out YRpp's classes for 32-bit Windows with
     Microsoft bitfields and writes `data/layouts.csv`: 283 classes, 4,966 members and
     bases with offsets and types. YRpp's 50 `sizeof` assertions all hold, and the
     offsets used by the functions matched in step 2 agree (`RawFileClass::Handle` +0x14,
     `ObjectClass::Health` +0x6C, `RulesClass::ConditionRed` +0x1708). Names are YRpp's;
     Westwood's take precedence when known.
   - Ghidra 12.1.4's default analysis (`tools/ghidra.sh`, 93 s) finds only 8,584
     functions: it misses most code reached only through vtables. Where it does find a
     function, its extent equals the map's for 8,114 of 8,581.
4. **Checker. Done 11 October 2026.** `tools/check.py <file> [addr]` compiles a source
   file with the step-2 flags and checks every function annotated `// FUNCTION: 0x...`
   on the line before its definition. Written on Byte Tactics' design (their checker
   is tied to their project; the core is small): length from `data/functions.csv`,
   EA's patch sites restored from `data/exe_patches.csv` first, a diff of the
   disassembly when it fails. `tools/checkall.py` re-checks everything under `src/`: the
   merge gate. reccmp needs a linked exe and a PDB, objdiff a split original; both fit
   step 5 better. Verified on the step-2 functions: the four `RawFileClass` ones and
   0x5F5CD0 MATCH, `CampaignClass::FindIndex` shows its one register difference, and
   swapping `GetLastError` for `GetTickCount` (identical bytes) reports WRONG TARGETS.
   `tools/libmatch.py` places 558 LIBCMT functions from 216 objects
   (`data/library.csv`), so CRT calls are verified by name and the queue can skip
   prebuilt code; the CRT block starts at 0x7C8A8E.
   - Linker-filled bytes are not compared, but what they point at is: a jump or table
     must land at the same place in the function, a string literal or constant must equal
     the original's target bytes, a call or global must reach its known address. A name
     with no known address yet is reported as unverified, not as a mismatch.
   - Matched sources stay out of `src/` until its licence is decided: the first ones are
     Westwood code via OpenTS and Renegade, both GPL-3 (see "Legal footing").
5. **Whole-exe build.** Split the original into one object per module so every commit
   links a complete, launchable `gamemd.exe`, each function swapping in as it matches.
   Try matching one module's data section too: the first evidence on whether the MD5 goal
   is reachable.
6. **Twenty functions.** Mixed sizes, half with an OpenTS counterpart and half RA2-only.
   Record for each: matched or not, best %, check runs, tokens spent, minutes, model,
   and for a stall the kind of blocker. A near miss is saved with its score for the next
   attempt to start from.
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
  invented ones. Fixed once in `symbols.csv` before agents start naming. A byte match
  proves nothing about a name: a function whose identity is unproven keeps an
  address-based name rather than a plausible guess.

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
  only target?
- What fraction of YR functions have an OpenTS counterpart? Measure it in the spike by
  comparing function lengths and call graphs.
- Is there any surviving `tsun.dbg` or `gamemd` debug build? A symbol file would make
  a whole-exe match possible.
- Is the whole-exe MD5 reachable? Matching one module's data section during the spike
  would show early whether data layout is a wall.
- v2's port route: OpenTS's and Vanilla Conquer's, as `opents-linux` took?
