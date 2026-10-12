# Red Alert 2: a matching decompilation of Yuri's Revenge

Status, 11 October 2026: spike done and priced (see "Budget"); going ahead: tier 0 and a permuter first, then batches module by module.

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

**The First Decade's exe, 11 October 2026.** TFD's `gamemd.exe` (from archive.org's TFD
disc image, kept only in `orig/gamemd-tfd.exe`) is Ares's "1.001 (TFD)": 4,813,072 bytes,
CRC `0xEB903080`, the CD build's size and timestamp, retail-sized resources. Its code
differs from Steam's in only 118 bytes across 12 functions, nearly all the CD check:
five `counter` functions and one at 0x4A80D0 stubbed to `mov al,1; ret` and the like,
`GetDriveType == DRIVE_CDROM` tests blanked in GDlgSupp, MainLoop and voc, branches forced
in netdlg2 and voc. Where TFD holds compiler-shaped code, its bytes are the original:
those 21 code sites, plus Steam's "TUC" (Ultimate Collection) version suffix and a 2-byte
`.rdata` size field, are now `data/exe_patches.csv`, and the function map is built on the
restored code (three stubs had faked function boundaries). TFD carries its own no-CD
patch at 0x4A81D7/0x4A820D, where Steam has the compiler's code; any edit TFD and Steam
share stays invisible to this comparison. TFD's TS and RA2 exes are kept too
(`orig/ts-game-tfd.exe`, `orig/game-tfd.exe`); TFD's TS exe carries two extra
`stxt` sections, a SafeDisc wrapper.

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

**Licence, decided 11 October 2026: `src/` and `include/` GPL-3.0-or-later, tooling and
data MIT** (`COPYING.md`, `LICENSE`, header convention in the README). Tier 0 compiles
OpenTS's functions unchanged, and Westwood's GPL'd wwlib code is the source of many
matches, so the game source must be GPL-3 anyway; a no-licence stance like Byte
Tactics' would mean keeping agents away from OpenTS, its biggest head start. GPL-3 also
matches OpenTS, Vinifera, Phobos and `opents-linux`, so fixes can flow both ways. No
licence choice changes EA's position; the footing above does that work.

YRpp and Ares carry no licence file: use them for facts (addresses, layouts, behaviour),
never paste their headers verbatim.

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
     SP5 are not kept (re-fetch them from the table if needed):

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
   - `tools/yrpp_layouts.py` has clang lay out YRpp's classes with the Microsoft 32-bit
     ABI (MinGW's headers, MSVC record layout; an Itanium-ABI first version put members
     in a base's tail padding, which MSVC never does) and writes `data/layouts.csv`: 283 classes, 4,966 members and
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
5. **Module layout and a data probe.** Rescoped 11 October 2026: the whole-exe build
   moves after the spike (see "After the spike"), since it prices nothing and has little
   to link yet.
   - **Layout, drafted:** `tools/modules.py` writes `data/modules.csv`. The exe keeps 74
     of Westwood's `__FILE__` paths (`D:\ra2mdpost\Techno.CPP`), and the functions using
     them show modules linked in case-insensitive alphabetical order, as OpenTS found for
     TS. Most modules also end with the same 23-byte initialiser: a common header defines
     a static 16-byte object, so every module including it gets its own copy and
     initialiser (312 of them; none falls inside any of the 73 anchored modules). Those
     cut the code into 313 segments of about one module each. Names come from the
     `__FILE__` paths, OpenTS's class-to-file map and RTTI (each virtual belongs to the
     most basic class whose vtable holds it), kept only in alphabetical order and never
     spanning a segment end: 14,348 of 19,283 game functions are named; 4,935 are not,
     mostly whole segments with no evidence yet (the twelve before `abstract`). Direct
     calls barely help (8% go to a function within 20 places).
   - **Data probe: AbsType.cpp (11 October 2026).** `src/abstype.cpp`, from the TibSun
     archive's version with YR's changes, matches 7 of the module's 8 written functions
     (691 of 990 bytes); `Read_INI` is a register-allocation near miss (42.6%, `this` in
     a different register). Headers: YR versions of `abstract.h` (two more interface
     bases, two fields, one virtual fewer), `abstype.h`, `ini.h`, `rect.h` in `include/`,
     laid over TibSun's by the checker. What the module's data shows:
     - **Falls out by itself:** the string literals come out in the original's order and
       contiguous, and the four vtables (each after its RTTI locator) land at exactly the
       original's addresses relative to one another.
     - **Include graph:** Westwood's file included `rect.h` (indirectly), whose
       header-defined `RECT_NONE` gives every including module its own copy and
       initialiser; TS's AbsType has it too. TibSun's slimmed headers lost it, so the
       initialisers fingerprint each module's include graph.
     - **Placement inside the module:** `Init_Theater` sits after `Compute_CRC` only when
       defined out of line at the end of the file. The compiler still emits the
       scalar deleting destructor and the `RECT_NONE` initialiser first, where the
       original has them last; LINK keeps each object's own order (tested with a map).
     - **`.bss` order:** ours puts `RECT_NONE` before the module's own 4-byte global;
       the original has them the other way round. If VC6 orders `.bss` by a hash of
       names (OpenTS's note), the global's real name decides it.
     Verdict so far: code and initialised data line up naturally; the MD5 goal hinges on
     the compiler's emission order (generated functions, initialisers, `.bss`), which
     is specific and testable but unsolved.
6. **Twenty functions. First run 11 October 2026: 20 of 20 matched.** A seeded draw of
   game functions from named modules, 19 to 1,238 bytes (5,983 in all), ten in modules TS
   has and ten RA2-only, none already matched by tier 0 and none with patch-like `nop`s.
   One workflow (`docs/AGENTS.md`, `tools/ctx.py`, a header overlay per agent): Sonnet 5.5
   at medium effort first, Opus 5.5 at high effort on its misses, starting from Sonnet's
   best source and notes. Every attempt is in `data/attempts.csv`; every MATCH was
   re-checked by the coordinator.
   - **Tier 1 matched 15, tier 2 four of the five it got.** The fifth was a scalar
     deleting destructor the checker could not select; both agents proved the bytes
     equal without gaming the checker, which now considers `??_G` and `??_E`.
   - **Opus's four were register-allocation and scheduling stalls** (460, 205, 474 and
     776 bytes), solved by source shape: writing a member access out at each use instead
     of caching a pointer, temporaries built with the TibSun constructor, sums kept
     inline. These go into `docs/AGENTS.md` as the guide grows.
   - **Cost:** about $16-20 at API rates (Sonnet input side $6.24, Opus $5.14, 440k
     output tokens derived from the workflow's total; transcripts log a streamed
     message's usage before it ends, so per-agent output is not measurable), 15 minutes
     wall-clock, 57 agent-minutes.
   - **Caveats:** agents matched against self-contained class stubs (padding, dummy
     virtuals), not shared YR headers, so the sources are proof, not `src/` material;
     about 200 references stay unverified for want of names; the draw excluded functions
     over 1,500 bytes, EA's patch sites and the 26% of code with no module name.
   - **Consolidation pilot (11 October 2026): four of the twenty moved into `src/` on
     real headers, all MATCH.** Three (TeamClass's play-music mission, the options
     struct's `Save`, `MapClass::In_Local_Radar`) were TibSun's code unchanged once the
     headers carried YR's layout: minutes each by hand. The fourth,
     `BaseClass::Mark_Node_Placed` (name invented), needed YR's object and type
     hierarchies; on them it matched in three checks with no stubs (Sonnet, 74k
     tokens).
   - **Headers are the real cost, once.** `tools/headercheck.py` proves a class's size,
     member offsets and every vtable slot against the exe through RTTI; Sonnet alone
     took ObjectClass, MissionClass, RadioClass, TechnoClass, BuildingClass,
     ObjectTypeClass, TechnoTypeClass and BuildingTypeClass to PASS (YR added about
     100 virtuals to the object chain and about 250 members to TechnoTypeClass). About
     0.8M agent tokens for the checker, the layout fix and those eight classes. Names
     the agents mapped from TS to YR by position are marked as guesses for matches to
     settle.
   - **Per-function consolidation is cheap once a class's header exists;** a header is a
     one-off that every function on that class shares. YR has about 950 classes, most
     far smaller than these.
   - **Large-function sample (11 October 2026): 2 of 5 matched.** Seeded draw of 2-6 KB
     functions with the proven headers available. Sonnet matched `UnitTypeClass::
     Read_INI` (2,188 bytes) in 6 checks; Opus matched a map-generator dialog procedure
     (2,404). Two stalled close: a 5,648-byte dialog procedure at 85.5% (full length,
     about 98% of instructions) and `AnimClass`'s constructor at 87.9%, each on a couple
     of register choices after 60-90 Opus checks; an RA2-only PlanMgr function stayed
     at 39.9%. About 2.3M agent tokens, roughly $55-65 at API rates, 47 minutes: about
     $4 per KB attempted and $12-14 per KB matched, against $3 per KB for the small set.
     Large functions are reconstructed almost whole and stall on register allocation:
     the tier-3 residue, for Fable by hand or a permuter (Byte Tactics' at 90% and up),
     with near misses banked as `partial` in the meantime.
7. **Price it.** Scale the measured cost to about 20k game functions (less what OpenTS
   carries over), and decide.

### After the spike

The whole-exe build, in Byte Tactics' order (their `docs/linking.md` is the map):

- **Placement:** compiled pieces put at the original's addresses over the original
  image, every reference resolved by name and checked. Cheap, and worth it once there is
  enough source that the placed build differs from the original only where intended.
- **An ordinary `LINK.EXE` link:** the unmatched code and data carved out of the
  original as relocatable objects. The exe has no relocation table, so every address in
  code and data is recovered by analysis; globals come from source, the import tables
  are rebuilt, and EA's patch sites are applied after linking.

Steps 1 to 5 are one interactive session with an Opus coordinator: tooling, no fan-out.
Step 6 is where parallel agents pay, if the maintainer opts in to a workflow for it.

### Tier 0, measured (11 October 2026)

- **Use the TibSun archive, not OpenTS.**
  [OpenTS-Developers/TibSun](https://github.com/OpenTS-Developers/TibSun) is the frozen
  matching baseline (about 95% of TS 2.03's functions byte-exact); OpenTS has since moved
  to C++20 and VS2022 and no longer compiles under VC6. The archive does, unchanged: 378
  of its 394 files compile with our toolchain and flags in 11 seconds (the 16 failures
  need SDK headers: DirectX, WOLAPI, VQA). Cloned to `reference/tibsun`.
- **TS was built with the same toolchain.** TS 2.03's Rich header shows the same VC6 SP3
  (438 C++ objects at 8447), the same 8168 CRT and MASM 7299. Compiled TibSun
  `abstype.cpp` is found byte for byte in TS's exe; of 8,714 TibSun functions of 16 bytes
  or more, 7,949 are found in TS (4,655 at one address). The installed Steam TS exe is not
  retail 2.03 either (MD5 `d46f79b9…`, timestamp 2000-06-05 21:26:42 like retail): EA
  patched it too.
- **Yield in YR:** 1,321 TibSun functions (173 KB) are found unchanged at exactly one
  address, 1,266 of them exactly on a mapped function. Another 3,012 are found at several
  addresses: small functions YR repeats (trivial getters, destructors); the checker's
  reference checks tell those apart. Classes RA2 changed (new members shift every
  offset) do not match unchanged; `AbstractTypeClass` is one. Those are tier-1 work
  starting from near-identical source.

**Tier 0 landed, 11 October 2026:** `tools/tier0.py` (re-runnable in seconds) compiles
403 of 404 TibSun files, finds 1,531 functions at exactly one YR start and resolves 54 more
by their relocation targets, copies each into its YR module's `src/` file and proves it
there against YR's headers: **946 functions, 105 KB, match**, with the merge gate at 957
of 958. Feeding tier 0's landed functions back into the module map as evidence then split
small modules out of their neighbours' segments (wstring, vqalib, mpmath, palette, ...;
335 module names, 15,324 of 19,280 game functions placed), and a module filter settled
ambiguous candidates: **1,317 tier-0 functions, 144 KB**, merge gate 1,328 of 1,329.
Per-function outcomes are in `data/tier0.csv`. The first run's leftovers, since cut to
13 conflicts, 53 failures in `src/` and 2,998 ambiguous:
- 185 conflicts with `data/modules.csv`, nearly all modules with no `RECT_NONE`
  initialiser folded into a neighbour's segment (vqalib into `zbuffer`, wstring into
  `wspudp`, ...): splitting those segments lands about 178 more (32 KB).
- 45 that fail in `src/` (29 differ under YR's headers, 2 WRONG TARGETS, 11 the checker
  cannot name: namespaced classes, conversion operators, statics).
- 3,206 ambiguous, mostly header inlines and compiler-generated functions; module
  placement would resolve about 200 more.
- Review: `src/netdlg2.cpp` carries about 730 lines of an unannotated static helper one
  matched function calls; small relocation-free functions are identified by bytes alone,
  so a few names may be wrong (one false identity was caught).

### The permuter (12 October 2026)

`tools/permute.py` (Byte Tactics' MIT-licensed tree-sitter mutation catalogue and search,
ported onto check.py; uncommitted pending the maintainer's call on keeping their
copyright line, which names their author) and `check.py --score`, an instruction-level
score with addresses, local jump targets and stack slots masked. It recovers a
deliberately broken matched function in under a second, and runs about 2,400-4,500
variants a minute on 14 workers. On the near misses Opus left (the campaign-score
dialog at 97.8% normalised, `AnimClass`'s constructor at 93.4%, a dialog text function
at 97.8%) it found nothing in about 260,000 compiles: what remains is register and x87
scheduling ties no source rearrangement reaches, as Byte Tactics found for functions
agents had already worked over. Use: a token-free sweep after Opus stops at 95% or more
with only register or order differences left (`--stall 5`); never before Opus, where the
gaps are semantic. A lower penalty score with a falling raw percentage is noise.

### Batch 1 (12 October 2026)

110 small functions (under 1.5 KB) in nine TS-derived modules, one agent per module
writing straight into `src/` on the real headers, Sonnet then Opus on near misses; an
agent could edit only its own module's header. Sonnet landed 69; about 30 more matched
byte for byte but only with YR's layout in shared headers the agents could not touch,
so Opus could not land them either. One coordinator-run agent then gave the shared
headers YR's real layout (RulesClass, HouseClass, CellClass, FootClass, the file
classes, GadgetClass and its subclasses, the surfaces, ConvertClass, ...; headercheck
PASS where YRpp has the class) and landed all 30. **99 of 110 matched**; 11 stay banked
in `build/batch/` (register near misses, three wrong extents in `data/functions.csv`,
one overload the checker resolves by undecorated name). Cost: about 1.6M tokens for the
batch and 0.5M for the header merge. Merge gate: 1,426 of 1,427.

Lesson: shared headers, not matching, held back a quarter of the batch. Later batches
merge requested header changes between the Sonnet and Opus tiers, so header-gated
functions land without spending Opus on them.

Backlog (12 October 2026, one Opus agent, about 0.3M tokens): eight of the eleven
landed, plus AbstractTypeClass::Read_INI (a missing `else`, not a regression). The
three wrong extents had one cause: a function nothing calls directly, aligned straight
after a `ret` with no padding. `functions.py` now follows each function's flow and
splits off code it never reaches (evidence `F`): 98 new starts, 92 extents changed, 53
of them confirmed by Ghidra. The checker treats an undecorated name as ambiguous when
another overload is in play, and takes a named marker for a function a header defines.
StageClass (28 bytes) and BANIM_COUNT (21) are fixed from the exe. Still banked: the
two stats queue functions (an inline-and-call mix of Grow and Shrink) and the 0x004A65C0
/ 0x004A66D0 twins at 95.5%. Merge gate: 1,435 of 1,435.

Spike consolidation (12 October 2026, one Sonnet agent, about 0.44M tokens): 15 of the
18 stub-based spike matches now match in `src/` on real headers, plus one function
found on the way; twelve new headers (AnimClass, AnimTypeClass and UnitTypeClass PASS
headercheck). Three identities came from the vtable the constructor stores last:
0x00779690 is PlayerProfile's constructor, 0x00628E80 and 0x0063EE30 are template
vector destructors. Banked: the map-seed dialog (needs YR's ScenarioClass, whose TS
layout agrees only to 0x218), a uicmnds function whose original calls one template
instance's members and inlines another's, and the three spike near misses. `build/spike/`
is gone. Merge gate: 1,452 of 1,452 (170.8 KB).

### Batch 2 (12 October 2026)

108 small functions, the first twelve unmatched in each of nine modules whose classes
already had proven headers (anim, builtype, cell, foot, House, object, rules, team,
techtype). Same shape as batch 1 plus the lesson from it: after the Sonnet tier, one
Opus agent landed every shared-header change the module agents asked for, verified
against the exe, and the functions those changes gated. **101 of 108 matched:**
Sonnet 74, the header merge 25, Opus 1, and one landed once a callee's name was
recorded. One more the rules agent reported as MATCH was missing from `src/` at the
end, which is why the coordinator re-checks every claim. The
merge gave nine more classes YR's full layout (UnitClass, InfantryTypeClass,
AircraftTypeClass, TeamTypeClass, HouseTypeClass, WarheadTypeClass and three small
ones) and settled YR signatures TS lacks (Take_Damage's seventh argument, QueueMission
and Commence's slots). Agents also landed about a dozen unlisted functions their
targets inlined or needed. Banked: three cell functions (one matches once ScenarioClass
has YR's layout; the other two need TibSun's wrapper calls instead of the direct ones),
a rules function on Is_Present's inlining, and team's 0x006EAA90 at about 82%; the
rules agent skipped one function and lost another. A wrong
extent there showed the map missed functions that follow a switch table and are named
only by a vtable pointer; `functions.py` now takes those where the table ends exactly
(7 new starts). Cost: about 2.65M tokens, 1 hour 45 minutes. Merge gate: 1,562 of 1,562
(202.6 KB).

Lesson: the header merge between tiers worked. It turned a quarter of the batch from
blocked into matched without spending Opus per function, and left the Opus tier only
the true near misses.

### Batch 3 (12 October 2026)

The same shape on nine more modules: Techno, building, aircraft, Infantry, Unit,
teamtype, houstype, warhead, mission. **99 of 107 matched** (one of the 108 was a jump
table the map had taken for a function). The header merge gave WeaponTypeClass and
BulletTypeClass YR's full layout, replaced six opaque manager stubs (spawn, slave,
capture, temporal, airstrike, electric bolt) with proven classes, and fixed YR
signatures: Fetch_Object_Size takes no arguments, Techno_Draw_Object takes 16 dwords,
the storage class holds floats. Two map faults surfaced and are fixed in
`functions.py`: an address pushed as a callback argument (atexit) was followed as flow,
hiding the callback, and a switch table preceded by padding was taken for a function
(10 starts added, 16 removed, each checked by hand). The checker resolved `toupper` to
the CRT's `__toupper`; a C symbol now loses one prefix character, not all. Banked: four
register or layout near misses (93.9% to 97.6%), two building functions waiting on
SuperClass and a CounterClass, and the initialiser now split from its callback. Cost:
about 3.2M tokens, 82 minutes. Merge gate: 1,670 of 1,670 (234.2 KB).

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
| 0 | A script, no model | Every YR function with a TS counterpart: compile the TibSun archive's version unchanged and diff it. Exact matches are free. |
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
"weeks of spare budget".

**First measure, 11 October 2026:** the 20-function run moved weekly usage from 2% to 3%
(the meter shows whole percents, and the coordinator's own work is in it), so about 1%
of a week per 20 matched functions. With 16-18k functions left after tier 0, first
matches alone come to roughly 8-9 weeks of a full weekly allowance, or about 8 months at
a quarter of each week. A floor, not a price: consolidation onto real headers, the large
functions and EA's patch sites are not in it yet.

The meter counts every session on the account, and other agents ran alongside the spike
(the week read 4% after the consolidation pilot), so these percentages are upper bounds.
Agent token counts are this project's own: about 1.75M for the 20 matches, 0.8M for the
pilot's one-off headers and tools, 74k to consolidate a function once its headers exist.
Measure the weekly share again in a window with nothing else running.

**First price, 11 October 2026 (API-rate equivalents, first matches).** Game code left
after tier 0 and the CRT is 3.64 MB in 17,991 functions: 17,590 under 1.5 KB (2.34 MB),
133 of 1.5-2 KB (0.23 MB), 228 of 2-6 KB (0.71 MB), 40 over 6 KB (0.35 MB). At the
measured rates: small functions about $7k (about $3 per KB, all matched in the sample);
1.5-6 KB about $8-12k (about $4 per KB attempted, 40% matched, residue extra); over 6 KB
unmeasured, at least $3-5k; class headers, one-off, about $1-3k. Roughly $20-27k in all.
Twenty small functions cost at most 1% of a week, so a week's allowance is worth at
least about $1.6-2k of agent work: roughly 10-15 full weeks, or about a year at a quarter
of each week. The largest uncertainties: the register-allocation residue (a permuter
could cut it sharply), how much tier 0 saves (1,300 to 4,300 functions), and the
weekly-meter calibration.

## Open questions

- Does `game.exe` (RA2 without YR) share enough to be worth matching too, or is YR the
  only target?
- What fraction of YR functions have a TS counterpart? First measure in the spike (see
  "Tier 0, measured"): at least 1,321 unchanged, up to about 4,300 once duplicates are
  told apart. Near misses (TS code with RA2's shifted offsets) are not counted yet.
- Is there any surviving `tsun.dbg` or `gamemd` debug build? A symbol file would make
  a whole-exe match possible.
- Is the whole-exe MD5 reachable? Matching one module's data section during the spike
  would show early whether data layout is a wall.
- v2's port route: OpenTS's and Vanilla Conquer's, as `opents-linux` took?
