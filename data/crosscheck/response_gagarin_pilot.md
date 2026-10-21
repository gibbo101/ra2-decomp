# Response to gagarin's 20-function pilot

Their bundle: `gagarin-verification-pilot`, repository commit
`a1cdb5b96a690f6b0488e32889e9c83196acf785`, SHA256SUMS verified on receipt.

## Image

Their `verify_image.py`, run against our image with `--allow-different-image`:
`binary_identity_equal: false`, `all_spans_equal: true`. All 20 reference spans, their
normalized forms and every relocation target are identical in our image.

| | gagarin | ra2-decomp |
|---|---|---|
| gamemd.exe SHA-256 | `1cdd1180e490…` | `3e81a61775d2…` |
| size | 5,286,504 | 5,286,208 |

The 296-byte difference lies outside these spans. Which build each image is (retail
1.001, Steam, The First Decade, a patched copy) is still open.

## Toolchain

`C1.DLL`, `C1XX.DLL`, `C2.DLL`, `LINK.EXE`, `LIB.EXE` and `CVTRES.EXE` hash identically on
both sides. Only the `CL.EXE` driver differs (ours 65,536 bytes, theirs 51,200), and the
driver does not generate code. Our flags: `/O2 /Ob2 /Gr /GR /MT /DWIN32`; theirs:
`/O2 /DNDEBUG /Gy /GX`. Both reproduce these 20 functions.

## The 20 functions

`response_gagarin_pilot.json` is our independent manifest for the same addresses, from
our own source and object (format in `docs/MANIFEST.md`). For all 20:

- boundaries agree (same sizes);
- relocation masks agree (same offsets and kinds);
- every relocation target agrees;
- normalized span hashes agree.

Names differ only by convention (YRpp-style against TibSun-style), except:

- **0x007B1FD0 and 0x007B2050:** we have free `__stdcall` functions (`Build_Packet_CRC`,
  `Passes_CRC_Check`); the exe's vtable makes them virtual members of
  `WinsockInterfaceClass`, as gagarin has them. **gagarin is right; we will fix ours.**
- 0x007B1760 and 0x007B2740: their placeholders `Slot20` and `Slot13` are our
  `Set_NetCard` and `Get_Protocol`.

References our checker marks `some-unverified` are targets we have no proven name for
yet; their bindings resolve them to the same addresses.
