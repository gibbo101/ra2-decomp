# exelink: link the whole executable at the original's addresses

These tools link every object built from `src/` into one `gamemd.exe`, with each matched
function at its original address, and compare the result with the original section by section.
They answer two questions the per-function checker cannot: does the matched code still sit
where the original has it once everything is linked together, and how much of the executable
already comes from source?

Three levels, each a superset of the last:

| level | command | what you get |
|---|---|---|
| stub link | `link.py` | Every object linked with LINK 6, with `/ORDER` laying `.text` out in the original's order. Each function no matched row provides is an int3 stub of its original size. `mapcheck.json` reports, for every matched function, its linked address against its manifest address, and the first place the layout drifts. The image does not run. |
| hybrid | `link.py --hybrid` | An image meant to run (it was not run for this port). Unmatched code, gaps, `.rdata`, `.data`, the import tables and `.rsrc` take the original's bytes. Every reference in matched code is pinned to the address the original uses, and the CRT, COMSUPP and LIBCPMT tail is laid out as the original's. `compare.json` compares every section with the original. |
| source data | `link.py --hybrid --source-data --search-literals` | Data the objects define (globals, vtables, RTTI, literals, initialiser tables) comes from the objects wherever its bytes and relocation targets equal the original's. |

## Running it

Run the toolchain setup first (`tools/setup_toolchain.sh`), and put your exe at `orig/gamemd.exe`
as for the checker.

```sh
uv run tools/exelink/build.py                 # src/ -> build/exelink/obj
uv run tools/exelink/prepare.py --objdir build/exelink/obj
uv run tools/exelink/link.py --hybrid --source-data --search-literals
```

- `prepare.py` writes everything the link reads to `build/exelink/inputs/`, derived from `data/`
  and your exe: the manifest, the function map's slots, the exe's RTTI graph, and the library
  tail (from the toolchain's own `LIB/`). It also writes `build/exelink/gamemd_1001.exe`, your
  exe with the post-link changes in `data/exe_patches.csv` put back. Every comparison is
  against that file, so the target is retail 1.001 whichever build you own.
- `build.py` compiles exactly as `tools/check.py` does, plus `/Gy`. `/Gy` puts every function
  in its own COMDAT so `/ORDER` can place it; it changes how code is packaged, not the code.
  Archived `.asm` objects get `comdatize.py`, which marks their `.text` a COMDAT keyed on the
  routine at offset 0.
- Everything written goes under `build/exelink/` and contains bytes of the original: never
  commit it. `link.py` refuses an `--out` outside it.

## How it keeps the image honest

- **Only bodies that equal the original are placed.** A matched row whose compiled bytes differ
  from the original outside relocation sites gives its slot back to the original's bytes, and
  is listed in `link_report.json` (`hybrid_rows_demoted_to_retail_bytes`). In an assembly
  object, every routine must also sit at the offset the original has it at.
- **References are pinned from the original, not from names.** `pindata.py` reads each
  relocation site of each placed matched function in the original: the dword there, less the
  object's addend, is where the original's reference goes. A name with one address everywhere
  is pinned once. A name the original keeps per module (a COMDAT, a file-local, an inline body
  each module kept its own copy of, a const that a header defines with external linkage) is
  pinned per object. A name whose sites disagree is pinned site by site. No external symbol
  table is needed or read.
- **The linker resolves everything.** There is no `/FORCE`. A name no placed code explains
  gets a stub, and `link_report.json` lists every one.

## Result at upstream `3814b14`

This was measured with the EA app exe and a VC6 install other than the one
`tools/setup_toolchain.sh` fetches (the same `C1`, `C1XX`, `C2`, `LINK`, `LIB` and `CVTRES`, but
different headers and MASM):

- The stub link places every one of the manifest's 18,974 addresses at its address, with no drift:
  18,954 as entries of their own, and 21 assembly routines inside their object's entry.
- The hybrid image has `.text`, `.rdata`, `.data` and `.rsrc` byte-identical to retail 1.001.
  80.05% of `.text` comes from compiled source and 2.15% from the toolchain's libraries.
- With source data the four sections are still identical, and 73.78% of all their bytes
  (8,391,297) come from source.
- Seven rows were demoted to the original's bytes:
  - `0x5A6510`, `0x640A40`, `0x64DEA0` and `0x66D530` don't match under this header set
    (`tools/check.py` agrees), so they should place on the pinned toolchain.
  - `0x7C60C0` is the first routine of `isoasm.asm`, which no row claims.
  - `0x7C6D40` (`_ASM_UnVQ_4x2`) and `0x7C8110` (`_OLD_VQA_LCW_Uncompress`) are each assembled
    one byte away from where the original has them inside their object. Each routine matches
    on its own, so check whether the pinned MASM does the same.

The PE headers are not compared: the linker writes its own (timestamp, checksum, debug
directory). Whole-file identity is the next step, not something this claims.

## What the link found in the source

`link_report.json` lists these so they can be fixed in the source. The link works around them
meanwhile:

- `include/geometry.h` defines 14 constants (`ONE_RAD`, `CELL_NONE`, `COORD_NONE` and others)
  that `include/globals.h` declares `extern`, so each of the 55 objects that includes both
  defines them with external linkage. LINK refuses that (LNK2005). The original has a separate
  copy per module, as file-local constants would give.
- `RandomStraw::~RandomStraw` is defined out of line in both `src/rle.cpp` and
  `src/rndstraw.cpp`, and `WWMouseClass::~WWMouseClass` in both `src/wwmous.cpp` and
  `src/wwmouse.cpp`.
- `data/functions.csv` lists `0x005F5B4C` as a 1-byte function. It is one `nop` of padding
  (`data/attempts.csv` says so too), and as a function it shrinks `ObjectClass::Paradrop`'s
  slot. `prepare.py` leaves padding-only entries out of the layout and prints them.

## Files

| file | role |
|---|---|
| `build.py`, `prepare.py`, `link.py`, `paths.py` | the driver and the inputs |
| `stubgen.py` | plans the layout: providers, stubs, fillers, renames, the `/ORDER` file |
| `pindata.py` | derives every pinned address from the original's relocation sites |
| `hybrid.py` | the original's data as an object; checks and trims compiled bodies |
| `datasource.py`, `rttimap.py` | source data: which data pieces equal the original's, and RTTI names |
| `taillink.py`, `tail_layout.py`, `crt_ident.py` | find and order the library tail |
| `mapcheck.py`, `compare_image.py` | the address check and the section comparison |
| `coffgen.py`, `comdatize.py`, `implib.py`, `binkw32.def`, `link_diag.py`, `coff_equivalence.py`, `verify_vtable.py`, `verify_relocations.py` | COFF reading and writing, the Bink import library, LINK plumbing |

The `test_*.py` files run with the rest of `tools/` (`python -m unittest discover -s tools`).
The LINK probes run when Wine and the toolchain are installed.

## Origin and licence

Ported from the link pipeline of [gagarin](https://github.com/r-melvin/gagarin), another
matching decompilation of the same executable. Its author contributes it here under the MIT
licence, as for the rest of `tools/`. The `M1`/`M2`/`M3` that some comments mention are
gagarin's names for the stub link, the hybrid image and source data.
