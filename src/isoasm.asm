;******************************************************************************
;*                               O P E N  T S
;******************************************************************************
;* SPDX-License-Identifier: GPL-3.0-or-later
;* Copyright 2026 OpenTS contributors
;*
;* See LICENSE.md for applicable additional terms and warranty disclaimers.
;******************************************************************************

		.386
		.model	flat, c


		.code

;-----------------------------------------------------------------------------
; Hand-written x86 inner-loop rasterizers for isometric terrain tile blitting.
; Both functions take a pointer to IsoBlitState (declared in isotype.cpp) and
; walk a rectangular source span, writing a destination surface pixel and a
; z-buffer word per non-transparent/non-rejected source byte. Translation
; pipeline: src byte -> optional shape remap (Func1) -> depth shading LUT
; -> palette-to-16bpp LUT -> dest.
;
; Struct field offsets used (see isotype.cpp IsoBlitState for full map):
;   +0x00  InnerCount (byte)      - inner span pixel count
;   +0x04  OuterCount (byte)      - outer row count
;   +0x08  ShapeBase              - Func1 shape LUT base
;   +0x0C  ShapeIndexBias         - Func1 shape index offset
;   +0x10  ZDepthBias             - added to z-source byte
;   +0x14  ZBufferRowStride       - added 2x per row to +0x40
;   +0x18  RemapSourceRowStride   - added 2x per row to +0x44
;   +0x1C  SurfaceRowStride       - added per row to +0x48
;   +0x20  SourceByteStride       - Func2 per-row stride for +0x50 and esi
;   +0x40  ZBufferCursor (u16*)   - walked z-buffer write pointer
;   +0x44  RemapIndexCursor (u16*)- walked remap-index source pointer
;   +0x48  SurfaceCursor (u16*)   - walked dest surface write pointer
;   +0x4C  SourceByteCursor       - walked palette-index source bytes
;   +0x50  ZSourceByteCursor      - walked z-source bytes
;   +0x54  TileShapeRowPtr        - Func1 tile-shape row base (advances 0x30)
;   +0x58  PaletteToHicolor       - palette->16bpp word LUT base
;   +0x5C  ExtraStrideTable1      - Func1 extra-block stride table (flag 1)
;   +0x60  ExtraStrideTable2      - Func1 extra-block stride table (flag 2)
;   +0x6C  (scratch: saved row ptr - Func1)
;   +0x74  (scratch: combined flag - Func1)
;   +0x88  ExtraBlockFlag2        - Func1 trigger for path 2
;   +0x8C  ExtraBlockFlag1        - Func1 trigger for path 1
;   +0x90  DepthShadingLUT        - word LUT (also referenced as [ebp+144] in Func2)
;
; NOTE: The C++ IsoBlitState ends at 0x6A but the ASM accesses up to 0x90.
; The declared struct is undersized; fields 0x74/0x88/0x8C/0x90 are missing.
;-----------------------------------------------------------------------------


;-----------------------------------------------------------------------------
; IsoBlitState (ASM-side mirror)
; Documentation only -- the functions below still use raw [ebp+0xNN] offsets,
; not these field names. Packed (1-byte aligned) to match the C++ declaration.
; Extends beyond the C++ struct (which ends at 0x6A) to cover the missing
; fields at 0x74/0x88/0x8C/0x90 that the assembly actually accesses.
;-----------------------------------------------------------------------------
IsoBlitState STRUCT 1
    InnerCount              DD ?    ; +0x00  inner span pixel count (byte used)
    OuterCount              DD ?    ; +0x04  outer row count (byte used)
    ShapeBase               DD ?    ; +0x08  Func1 shape LUT base
    ShapeIndexBias          DD ?    ; +0x0C  Func1 shape index offset
    ZDepthBias              DD ?    ; +0x10  added to z-source byte
    ZBufferRowStride        DD ?    ; +0x14  applied 2x per row to ZBufferCursor
    RemapSourceRowStride    DD ?    ; +0x18  applied 2x per row to RemapIndexCursor
    SurfaceRowStride        DD ?    ; +0x1C  applied per row to SurfaceCursor
    SourceByteStride        DD ?    ; +0x20  Func2 per-row stride (esi + ZSourceByteCursor)
    _unused_24              DD ?    ; +0x24
    _unused_28              DD ?    ; +0x28
    _unused_2C              DD ?    ; +0x2C
    _unused_30              DD ?    ; +0x30
    _unused_34              DD ?    ; +0x34
    _unused_38              DD ?    ; +0x38
    _unused_3C              DD ?    ; +0x3C
    ZBufferCursor           DD ?    ; +0x40  walked z-buffer write ptr (u16*)
    RemapIndexCursor        DD ?    ; +0x44  walked remap-index source ptr (u16*)
    SurfaceCursor           DD ?    ; +0x48  walked dest surface write ptr (u16*)
    SourceByteCursor        DD ?    ; +0x4C  walked palette-index source bytes
    ZSourceByteCursor       DD ?    ; +0x50  walked z-source bytes
    TileShapeRowPtr         DD ?    ; +0x54  Func1 tile-shape row base (advances 0x30 per row)
    PaletteToHicolor        DD ?    ; +0x58  palette->16bpp word LUT base
    ExtraStrideTable1       DD ?    ; +0x5C  Func1 extra-block stride table (flag path 1)
    ExtraStrideTable2       DD ?    ; +0x60  Func1 extra-block stride table (flag path 2)
    _unused_64              DD ?    ; +0x64
    HalfbrightMask          DW ?    ; +0x68  (2 bytes)
    _pad_6A                 DW ?    ; +0x6A  (2 bytes pad to 0x6C)
    _scratch_6C             DD ?    ; +0x6C  Func1 saved row ptr scratch
    _unused_70              DD ?    ; +0x70
    _scratch_74             DD ?    ; +0x74  Func1 combined flag scratch (flag1 OR flag2)
    _unused_78              DD ?    ; +0x78
    _unused_7C              DD ?    ; +0x7C
    _unused_80              DD ?    ; +0x80
    _unused_84              DD ?    ; +0x84
    ExtraBlockFlag2         DD ?    ; +0x88  Func1 trigger for extra path 2
    ExtraBlockFlag1         DD ?    ; +0x8C  Func1 trigger for extra path 1
    DepthShadingLUT         DD ?    ; +0x90  word LUT (Func1 [ebp+90h], Func2 [ebp+144])
IsoBlitState ENDS


; Shape-aware iso tile rasterizer (48x23 diamond blit). Per pixel:
;   1. Read span-run byte from [[ebp+54h]]; zero = transparent (skip).
;   2. Read z-byte from [[ebp+50h]], add z-bias, compare vs current z at [[ebp+40h]].
;   3. If passing depth test, write new z and remap source index through two LUTs
;      (shape remap [ebp+90h], then palette->16bpp [ebp+58h]).
;   4. Advance all walked cursors by 2 bytes (pixels) / 1 byte (source).
; At end of row, applies strides and bumps shape-row pointer by 0x30 (48 - tile
; row size). Flags at [ebp+88h]/[ebp+8Ch] trigger extra-block stepping via
; tables at [ebp+5Ch]/[ebp+60h] - shape-diamond pixel layout stepping.
; STATUS: Zero xrefs in binary - dead code / replaced by C++ implementation.
Iso_Blit_Asm1 PROC C uses ebx esi edi ecx ebp \
	arg:DWORD

	mov     ebp, [arg]

	mov     eax, [ebp+8Ch]
	or      eax, [ebp+88h]
	mov     [ebp+74h], eax
	xor     ch, ch
	mov     eax, [ebp+54h]
	mov     [ebp+6Ch], eax

??loc_6B24F2:
	xor     cl, cl
	mov     esi, [ebp+54h]

??loc_6B24F7:
	mov     al, [esi]
	inc     esi
	test    al, al
	jz      short ??loc_6B253D

	xor     ebx, ebx
	mov     edi, [ebp+50h]
	mov     edx, [ebp+40h]
	mov     bl, [edi]
	add     ebx, [ebp+10h]
	xor     eax, eax
	cmp     bx, [edx]
	jnb     short ??loc_6B2537

	mov     [edx], bx
	mov     ebx, [ebp+44h]
	mov     edx, [ebp+4Ch]
	movzx   eax, word ptr [ebx]
	mov     ebx, [ebp+90h]
	mov     ax, [ebx+eax*2]
	mov     al, [edx]
	mov     edx, [ebp+58h]
	mov     edi, [ebp+48h]
	mov     ax, [edx+eax*2]
	mov     [edi], ax

??loc_6B2537:
	inc     dword ptr [ebp+50h]
	inc     dword ptr [ebp+4Ch]

??loc_6B253D:
	inc     cl
	add     dword ptr [ebp+48h], 2
	add     dword ptr [ebp+40h], 2
	add     dword ptr [ebp+44h], 2
	cmp     cl, [ebp+0]
	jl      short ??loc_6B24F7

	mov     eax, [ebp+1Ch]
	add     [ebp+48h], eax
	mov     eax, [ebp+14h]
	add     [ebp+40h], eax
	add     [ebp+40h], eax
	mov     eax, [ebp+18h]
	add     [ebp+44h], eax
	add     [ebp+44h], eax
	mov     eax, [ebp+6Ch]
	add     eax, 48
	mov     [ebp+54h], eax
	mov     [ebp+6Ch], eax
	cmp     dword ptr [ebp+74h], 0
	jnz     short ??loc_6B258C

	inc     ch
	cmp     ch, [ebp+4]
	jl      ??loc_6B24F2

	ret

??loc_6B258C:
	movzx   eax, ch
	add     eax, [ebp+0Ch]
	shl     eax, 4
	lea     ebx, [eax+eax*2]
	add     ebx, [ebp+8]
	mov     edx, ebx
	cmp     dword ptr [ebp+8Ch], 0
	jz      short ??loc_6B25B5

	mov     esi, [ebp+60h]
	add     ebx, [ebp+0]
	mov     eax, [esi+ebx*4]
	add     [ebp+4Ch], eax
	add     [ebp+50h], eax

??loc_6B25B5:
	cmp     dword ptr [ebp+88h], 0
	jz      short ??loc_6B25CD

	mov     esi, [ebp+5Ch]
	add     edx, 48
	mov     eax, [esi+edx*4]
	add     [ebp+4Ch], eax
	add     [ebp+50h], eax

??loc_6B25CD:
	inc     ch
	cmp     ch, [ebp+4]
	jl      ??loc_6B24F2

	ret

	Iso_Blit_Asm1 endp


; Simpler rectangular iso tile rasterizer. No shape-aware stepping, no shape LUT,
; no flag-triggered block skips. Per pixel:
;   1. Read byte from walked esi source; zero = transparent (skip).
;   2. Depth test against [[ebp+40h]] using byte from [[ebp+50h]] + bias.
;   3. If passing, remap through depth-shading LUT at [ebp+144h] (= +0x90), then
;      palette->16bpp LUT at [ebp+58h]. Write result to [[ebp+48h]].
; Per row advances walked cursors by strides at [ebp+14h/18h/1Ch/20h].
; STATUS: Live code. Called from IsometricTileTypeClass::Draw_Tile.
; FUNCTION: GAMEMD 0x007C61CF
Iso_Blit_Asm2 PROC C uses ebp \
	arg:DWORD

	mov     ebp,[arg]

	xor     ch, ch
	mov     esi, [ebp+4Ch]


??loc_6B25EB:
	xor     cl, cl


??loc_6B25ED:
	movzx   eax, byte ptr [esi]
	inc     esi
	test    al, al
	jz      short ??loc_6B262E

	xor     ebx, ebx
	mov     edi, [ebp+50h]
	mov     edx, [ebp+40h]
	mov     bl, [edi]
	add     ebx, [ebp+10h]
	and     eax, 0FFh
	cmp     bx, [edx]
	jnb     short ??loc_6B262E

	mov     [edx], bx
	mov     ebx, [ebp+44h]
	mov     edi, [ebp+48h]
	movzx   edx, word ptr [ebx]
	mov     ebx, [ebp+144]
	mov     dx, [ebx+edx*2]
	mov     dl, al
	mov     eax, [ebp+58h]
	mov     ax, [eax+edx*2]
	mov     [edi], ax


??loc_6B262E:
	inc     cl
	inc     dword ptr [ebp+50h]
	add     dword ptr [ebp+48h], 2
	add     dword ptr [ebp+40h], 2
	add     dword ptr [ebp+44h], 2
	cmp     cl, [ebp+0]
	jl      short ??loc_6B25ED

	mov     eax, [ebp+1Ch]
	add     [ebp+48h], eax
	mov     eax, [ebp+14h]
	add     [ebp+40h], eax
	add     [ebp+40h], eax
	mov     eax, [ebp+18h]
	add     [ebp+44h], eax
	add     [ebp+44h], eax
	mov     eax, [ebp+20h]
	add     esi, [ebp+20h]
	add     [ebp+50h], eax
	inc     ch
	cmp     ch, [ebp+4]
	jl      ??loc_6B25EB

	ret

	Iso_Blit_Asm2 endp


		end