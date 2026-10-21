# Crosscheck manifest

A crosscheck manifest records, for each function a project claims to match, exactly which
bytes of `gamemd.exe` 1.001 the claim covers, where every linker-filled field in them points,
and what else the executable proves about the function. Two decompilations of the same exe
exchange manifests to compare their claims without exchanging source. `tools/xmanifest.py`
writes ours:

```sh
uv run tools/xmanifest.py --from data/crosscheck/pilot_addresses.txt --out data/crosscheck/pilot_manifest.json
uv run tools/xmanifest.py 0x004101F0 0x00557A20         # to stdout
uv run tools/xmanifest.py --all --out build/xmanifest.json
```

Every function listed must be a MATCH row of `data/progress.csv`. Its source is compiled
again with the checker's flags and compared as `tools/check.py` compares it, so the manifest
states what the tree holds now, not what the CSV once recorded. Running it twice on the same
inputs gives byte-identical files: no timestamps, fixed key order, functions sorted by address.

This document is the format, version 1, normalization `reloc-zero-v1`.

## File

UTF-8 JSON, `json.dumps(manifest, indent=1)` followed by one LF. Keys appear in the order
below. Addresses are strings `0x` + 8 uppercase hex digits; hashes are lowercase hex SHA-256;
sizes and offsets are decimal integers.

### Header

| key | value |
|---|---|
| `format` | `"ra2-decomp crosscheck manifest"` |
| `format_version` | `1` |
| `normalization` | `"reloc-zero-v1"` (below) |
| `project`, `repository` | `"ra2-decomp"`, `"https://github.com/gibbo101/ra2-decomp"` |
| `commit` | the commit HEAD names. The working tree may hold uncommitted changes, so `sources` pins each source file too |
| `exe` | `file`, `version`, `md5`, `sha256`, `size` of the executable the bytes were read from |
| `compiler` | `"Visual C++ 6.0 SP3 (C2.DLL 12.00.8447)"` |
| `flags` | the compiler options, in order: `/O2 /Ob2 /Gr /GR /MT /DWIN32` (every compile also has `/c`) |
| `inputs` | SHA-256 of what decides the result besides the sources: `include_tree_sha256` (below), each of `data/functions.csv`, `progress.csv`, `names.csv`, `symbols.csv`, `library.csv`, `exe_patches.csv`, and the commits of `reference/tibsun` (headers the build falls back on) and `reference/YRpp` (from which the local `data/symbols.csv` is built) |
| `sources` | `{path: sha256}` of every source file a listed function comes from, sorted by path |
| `functions` | one object per function, by address |

`include_tree_sha256` is the SHA-256 of the text `sha256sum` would print for every file under
`include/`: one line `<sha256 hex>  <path>\n` per file, path relative to the repository root
with `/` separators, lines sorted by path.

Our exe is the Steam copy: 5,286,208 bytes, MD5 `2a1307f6f57c9c492f985b4e3afb88e6`, SHA-256
`3e81a61775d2745d1dabe397325ef663cd994ffc194da4e998e3bf5d2d308600`. The 1.001 builds (retail CD,
The First Decade, Ultimate Collection, Steam) share one link timestamp and differ in code only
where a publisher patched it after linking, so per-function hashes are comparable across them
outside those sites; a function holding one carries an `exe_patches` key (below).

### Function

| key | value |
|---|---|
| `address` | entry point |
| `size` | bytes from the entry point to the end of the function's last byte: its code, then any jump table and byte index table the compiler placed after the code; alignment padding (`0xCC`, `0x90`) after it is not included. This is the length of the function our compiled object emits, which the checker found equal to the original's |
| `symbol` | the decorated name our object gives the function |
| `source` | the source file that compiles it |
| `byte_match` | the checker's verdict on the bytes: same length, and every byte outside the relocation fields identical |
| `raw_sha256` | SHA-256 of the `size` bytes at `address` in the pinned exe, read through its PE section table (virtual address to file offset), as the file has them |
| `normalized_sha256` | SHA-256 of the same bytes with every relocation field zeroed (below) |
| `relocations` | the function's relocation fields, by offset |
| `verification` | what each claim rests on (below) |
| `exe_patches` | only where `data/exe_patches.csv` has a site inside the function: `[{offset, retail, restored}]`, the exe's bytes there and the compiler's. The checker compares against the restored bytes; both hashes are of the exe's bytes |

A function whose source failed to compile when the manifest was written has only `address`,
`size`, `symbol`, `source`, `byte_match: false` and `compile_error: true`.

## Normalization `reloc-zero-v1`

1. Take the `size` bytes at `address` from the exe, as for `raw_sha256`.
2. Take every relocation our COFF object records for the function: each entry of the
   relocation table of the section (a COMDAT under `/O2`, which implies `/Gy`) that holds the
   function's symbol, whose 4-byte field lies wholly inside `[0, size)` measured from the
   function's start. VC6 emits two types here: `DIR32` (`0x06`, an absolute address) and
   `REL32` (`0x14`, relative to the end of the field). Every such field is listed under
   `relocations`, whatever its type.
3. Set the 4 bytes at each field's offset to `0x00`.
4. `normalized_sha256` is the SHA-256 of the result.

So the hash keeps every byte the compiler decides and drops every byte the linker fills in.
Another project reproduces it from its own object if its compile matches: the relocation
offsets are then the same. gagarin's `raw_match.py` masks the same fields (the 4 bytes of each
`DIR32` and `REL32` relocation in the function's span), so its normalization is this one.

## Relocations

Each entry, by ascending `offset`:

| key | value |
|---|---|
| `offset` | from the function's start to the field |
| `type` | `"DIR32"` or `"REL32"` (another type would be written by name or as `0xNN`) |
| `addend` | the signed 32-bit value our object stores in the field before linking: for `DIR32` the displacement into the target (`[g_object+0x10]` has addend 16), for `REL32` normally 0 |
| `target_symbol` | the COFF symbol the relocation names, or `null` for a label inside the function's own section (a jump-table entry, a `switch` case) |
| `target_address` | the target `S` the original's field encodes, with the addend taken off: for `DIR32`, `u32(field) - addend`; for `REL32`, `address + offset + 4 + s32(field) - addend`, modulo 2^32. `null` when `byte_match` is false |
| `binding` | whether `target_address` is the address of `target_symbol` (below) |
| `rtti` | only on a `DIR32` to a vftable (`??_7...`): the RTTI type name the exe's complete object locator gives that vtable, or `null` if none |

`binding`, as `tools/check.py` resolves each reference:

- `ok`: our records give the symbol an address, and it equals `target_address`. A label in the
  function: its place in the function. A string literal (`??_C@...`) or floating constant
  (`__real@...`) our object defines: the exe's bytes at `target_address` equal the constant's.
  An import (`__imp_...`): the exe's import slot for that name. A `FUN_xxxxxxxx` or
  `DAT_xxxxxxxx` name: the address it carries. Another function of the same source file: its
  marked address. Any other symbol: its address by decorated name in `data/progress.csv` (our
  matches) or `data/names.csv` (addresses proven by matches); failing that, by qualified name in
  `data/names.csv`, `data/symbols.csv` (YRpp's table, a third-party reference) or
  `data/library.csv` (identified CRT functions), in that order, unless the name is an overload
  the tables cannot tell apart.
- `mismatch`: an address is known and differs. A MATCH never has one.
- `unverified`: no address is known for the symbol, so `target_address` is what the original
  holds there, unconfirmed to be that symbol.
- `unchecked`: `byte_match` is false, so the fields need not line up with the original's.

## Verification

| key | values |
|---|---|
| `bytes` | `true` / `false`: the same as `byte_match` |
| `references` | `all-ok`: every relocation's binding is `ok`. `some-unverified`: none is `mismatch`, at least one is `unverified`. `mismatch`: at least one is. `n-a`: the function has no relocation fields. `unchecked`: `bytes` is false |
| `layout` | `n-a`: every memory operand addresses the function's own stack frame (`esp`, or `ebp` after `push ebp; mov ebp, esp`), so no class or global layout is involved. `unchecked`: otherwise. The manifest never claims a layout verified: `tools/headercheck.py` compares a class's header with YRpp's layout table and with the exe's vtable, which is evidence about a header, not a per-function proof |
| `vtable` | `verified`: every `DIR32` field naming a vftable `??_7<class>@@6B...` points at a vtable whose complete object locator (the dword before the vtable) leads to the type descriptor `.?AV<class>@@` or `.?AU<class>@@`. `mismatch`: at least one leads elsewhere (the `rtti` key shows where). `n-a`: no vftable field |
| `virtual` | whether the exe agrees with the symbol about being a virtual of its class (below) |

`virtual`. The exe's vtables are found through RTTI: every complete object locator in
`.rdata` (signature 0, then offset, constructor-displacement offset, type descriptor, class
hierarchy descriptor), every dword in `.rdata` that points at one (the vtable starts 4 bytes
after it), and the vtable's slots up to the first entry outside `.text`. The class of a symbol
is the longest RTTI type name `.?AV<scope>` whose `<scope>` the decorated symbol carries as its
class; its access code (the letter after the scope) says virtual (`E F G H M N O P U V W X`) or
not.

- `verified`: a virtual whose address is a slot of one of its class's vtables; a virtual
  destructor (`??1`) whose class's vtables hold a deleting destructor (a slot that tests bit 0
  of its flag and calls `operator delete`), or a slot that calls it; or a non-virtual member
  that is no vtable slot at all.
- `mismatch`: a virtual that is no slot of its class's vtables, where those hold no
  `_purecall`; a non-virtual or free function that is a slot of its class's vtable, or of one
  class hierarchy (a slot is a virtual).
- `unchecked`: the class has no RTTI in the exe under that name, or the class has a pure
  virtual (a pure virtual with a body is called directly and is no slot), or the address is a
  slot of unrelated hierarchies (code the linker may share).
- `n-a`: a free function that is no vtable slot.

## Exchange

A pilot of 20 functions both projects match is in `data/crosscheck/`: the addresses and why
each was chosen (`pilot_addresses.txt`), our manifest (`pilot_manifest.json`), and the
differences between the projects' full claims (`discrepancies.md`, `discrepancies.csv`).
