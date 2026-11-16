; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright 2026 ra2-decomp contributors

		.386
		.model	flat

externdef	?LoadFile@FileSystem@@SIPBXPBD_N@Z:near
externdef	?GetFrameBounds@SHPStruct@@QBE?AV?$TRect@H@@H@Z:near
externdef	?GetPixels@SHPStruct@@QBEPAEH@Z:near
externdef	?Get_Buffer_Offset@ABuffer@@QAEIVPoint2D@@@Z:near
externdef	?Scen@@3PAVScenarioClass@@A:dword
externdef	?AlphaBuffer@@3PAVABuffer@@A:dword
externdef	?TacticalRect@@3V?$TRect@H@@A:dword
externdef	?DAT_007E4EB8@@3HB:dword

		.const

??_C@_07BIFH@FOG?4SHP?$AA@		db	"FOG.SHP", 0
??_C@_0L@HLOI@SHROUD?4SHP?$AA@	db	"SHROUD.SHP", 0

		.data?

_?shapes_loaded@?1??Draw_Shroud_Or_Fog_Shape@CellClass@@QAEXABVPoint2D@@ABV?$TRect@H@@H@Z@4_NA	db	?
_?shadow_shapes@?1??Draw_Shroud_Or_Fog_Shape@CellClass@@QAEXABVPoint2D@@ABV?$TRect@H@@H@Z@4PBVSHPStruct@@B	dd	?
_?fog_shapes@?1??Draw_Shroud_Or_Fog_Shape@CellClass@@QAEXABVPoint2D@@ABV?$TRect@H@@H@Z@4PBVSHPStruct@@B	dd	?

shapes_loaded	textequ	<_?shapes_loaded@?1??Draw_Shroud_Or_Fog_Shape@CellClass@@QAEXABVPoint2D@@ABV?$TRect@H@@H@Z@4_NA>
shadow_shapes	textequ	<_?shadow_shapes@?1??Draw_Shroud_Or_Fog_Shape@CellClass@@QAEXABVPoint2D@@ABV?$TRect@H@@H@Z@4PBVSHPStruct@@B>
fog_shapes	textequ	<_?fog_shapes@?1??Draw_Shroud_Or_Fog_Shape@CellClass@@QAEXABVPoint2D@@ABV?$TRect@H@@H@Z@4PBVSHPStruct@@B>
LoadFile	textequ	<?LoadFile@FileSystem@@SIPBXPBD_N@Z>
GetFrameBounds	textequ	<?GetFrameBounds@SHPStruct@@QBE?AV?$TRect@H@@H@Z>
GetPixels	textequ	<?GetPixels@SHPStruct@@QBEPAEH@Z>
Get_Buffer_Offset	textequ	<?Get_Buffer_Offset@ABuffer@@QAEIVPoint2D@@@Z>
Scen		textequ	<?Scen@@3PAVScenarioClass@@A>
AlphaBuffer	textequ	<?AlphaBuffer@@3PAVABuffer@@A>
TacticalRect	textequ	<?TacticalRect@@3V?$TRect@H@@A>
SKIP_PIXEL	textequ	<?DAT_007E4EB8@@3HB>

		.code

; Fills the alpha buffer from a frame of SHROUD.SHP, or of FOG.SHP under fog of war, clipped
; to the rectangle; pixel 0FEh leaves the buffer as it is.
; RECONSTRUCTED: GAMEMD 0x0047EFE0
?Draw_Shroud_Or_Fog_Shape@CellClass@@QAEXABVPoint2D@@ABV?$TRect@H@@H@Z proc

		push	ebp
		mov	ebp, esp
		sub	esp, 34h
		mov	al, byte ptr [shapes_loaded]
		test	al, al
		jnz	loaded
		xor	dl, dl
		mov	ecx, offset ??_C@_0L@HLOI@SHROUD?4SHP?$AA@
		mov	byte ptr [shapes_loaded], 1
		call	LoadFile
		xor	dl, dl
		mov	ecx, offset ??_C@_07BIFH@FOG?4SHP?$AA@
		mov	[shadow_shapes], eax
		call	LoadFile
		mov	[fog_shapes], eax
loaded:
		mov	eax, [Scen]
		mov	ecx, [eax]
		test	ch, 10h
		jz	shroud
		mov	ecx, [fog_shapes]
		mov	[ebp-4], ecx
		jmp	have_shapes
shroud:
		mov	edx, [shadow_shapes]
		mov	[ebp-4], edx
have_shapes:
		mov	eax, [ebp+10h]
		push	ebx
		push	esi
		push	edi
		lea	ecx, [ebp-34h]
		push	eax
		push	ecx
		mov	ecx, [ebp-4]
		call	GetFrameBounds
		mov	edx, [eax]
		mov	ebx, [ebp+8]
		mov	edi, [ebp+0Ch]
		mov	[ebp-24h], edx
		mov	edx, [eax+4]
		mov	esi, [edi+4]
		mov	[ebp-20h], edx
		mov	ecx, [eax+8]
		mov	[ebp-1Ch], ecx
		mov	ecx, [ebx+4]
		mov	eax, [eax+0Ch]
		add	eax, ecx
		add	eax, edx
		lea	edx, [esi+1]
		cmp	eax, edx
		mov	[ebp-10h], eax
		jl	done
		mov	eax, [edi+0Ch]
		add	eax, esi
		cmp	ecx, eax
		mov	[ebp+0Ch], eax
		jge	done
		mov	eax, [ebx]
		mov	edx, [ebp-1Ch]
		mov	ebx, [ebp-24h]
		add	edx, eax
		add	edx, ebx
		mov	ebx, [edi]
		mov	[ebp+8], ebx
		inc	ebx
		cmp	edx, ebx
		mov	[ebp-14h], edx
		jl	done
		mov	edx, [edi+8]
		mov	ebx, [ebp+8]
		add	edx, ebx
		cmp	eax, edx
		mov	[ebp-0Ch], edx
		jge	done
		mov	edx, [ebp-20h]
		mov	[ebp+8], esi
		lea	edi, [edx+ecx]
		cmp	esi, edi
		jg	top_clipped
		mov	[ebp+8], edi
top_clipped:
		mov	esi, [ebp+8]
		sub	esi, ecx
		mov	ecx, [ebp+0Ch]
		sub	esi, edx
		mov	edx, [ebp-10h]
		cmp	ecx, edx
		mov	[ebp-8], ecx
		jl	bottom_clipped
		mov	[ebp-8], edx
bottom_clipped:
		mov	edx, [ebp-24h]
		lea	ecx, [edx+eax]
		cmp	ebx, ecx
		jg	left_clipped
		mov	ebx, ecx
left_clipped:
		mov	ecx, [ebp-0Ch]
		mov	edi, ebx
		sub	edi, eax
		sub	edi, edx
		mov	edx, [ebp-14h]
		cmp	ecx, edx
		jge	right_shape
		mov	[ebp+0Ch], ecx
		jmp	right_clipped
right_shape:
		mov	[ebp+0Ch], edx
		mov	ecx, edx
right_clipped:
		mov	edx, [ebp-1Ch]
		sub	eax, ecx
		add	eax, edi
		add	eax, edx
		mov	edx, [ebp-24h]
		add	eax, edx
		mov	[ebp-0Ch], eax
		mov	eax, [AlphaBuffer]
		mov	eax, [eax+28h]
		sub	eax, ecx
		mov	ecx, [ebp+10h]
		add	eax, ebx
		push	ecx
		mov	ecx, [ebp-4]
		mov	[ebp-10h], eax
		call	GetPixels
		mov	edx, dword ptr [TacticalRect+4]
		mov	ecx, [AlphaBuffer]
		mov	[ebp+10h], eax
		mov	eax, [ebp+8]
		sub	eax, edx
		push	eax
		push	ebx
		call	Get_Buffer_Offset
		imul	esi, [ebp-1Ch]
		mov	ecx, [ebp-8]
		mov	edx, [ebp+8]
		sub	ecx, edx
		mov	edx, [AlphaBuffer]
		add	esi, edi
		mov	edi, [ebp+10h]
		mov	edx, [edx+28h]
		add	edi, esi
		imul	edx, ecx
		mov	esi, ebx
		mov	[ebp+10h], edi
		mov	edi, [ebp+0Ch]
		mov	[ebp-4], eax
		neg	esi
		shl	esi, 1Fh
		sub	esi, ebx
		add	edi, edx
		add	esi, edi
		lea	edx, [eax+esi*2+4]
		mov	esi, [AlphaBuffer]
		cmp	edx, [esi+1Ch]
		jb	unwrapped
		mov	edx, [ebp-8]
		mov	edi, [ebp+8]
		cmp	edi, edx
		jge	done
		mov	[ebp+8], ecx
wrapped_row:
		mov	edx, [ebp+0Ch]
		cmp	ebx, edx
		jge	wrapped_row_end
		mov	edi, edx
		sub	edi, ebx
wrapped_pixel:
		mov	ecx, [ebp+10h]
		mov	dl, [ecx]
		inc	ecx
		cmp	dl, 0FEh
		mov	[ebp+10h], ecx
		jz	wrapped_skip
		xor	cx, cx
		mov	cl, dl
		mov	[eax], cx
		mov	esi, [AlphaBuffer]
wrapped_skip:
		mov	ecx, [esi+1Ch]
		add	eax, 2
		cmp	eax, ecx
		jb	wrapped_next
		sub	eax, [esi+20h]
wrapped_next:
		dec	edi
		jnz	wrapped_pixel
wrapped_row_end:
		mov	ecx, [ebp-10h]
		mov	edx, [ebp-0Ch]
		mov	edi, [ebp+10h]
		lea	eax, [eax+ecx*2]
		mov	ecx, [esi+1Ch]
		add	edi, edx
		cmp	eax, ecx
		mov	[ebp+10h], edi
		jb	wrapped_row_next
		sub	eax, [esi+20h]
wrapped_row_next:
		mov	ecx, [ebp+8]
		dec	ecx
		mov	[ebp+8], ecx
		jnz	wrapped_row
		pop	edi
		pop	esi
		pop	ebx
		mov	esp, ebp
		pop	ebp
		ret	0Ch

unwrapped:
		mov	eax, [ebp+0Ch]
		mov	[ebp-14h], ecx
		sub	eax, ebx
		mov	[ebp+8], eax
		mov	esi, [ebp+10h]
		mov	edi, [ebp-4]
		mov	edx, [ebp-14h]
		mov	ebx, [ebp-10h]
		add	ebx, ebx
row:
		mov	ecx, [ebp+8]
pixel:
		xor	eax, eax
		mov	al, [esi]
		inc	esi
		cmp	eax, [SKIP_PIXEL]
		jz	skip
		mov	[edi], ax
skip:
		add	edi, 2
		dec	ecx
		jg	pixel
		add	esi, [ebp-0Ch]
		add	edi, ebx
		dec	edx
		jg	row
done:
		pop	edi
		pop	esi
		pop	ebx
		mov	esp, ebp
		pop	ebp
		ret	0Ch

?Draw_Shroud_Or_Fog_Shape@CellClass@@QAEXABVPoint2D@@ABV?$TRect@H@@H@Z endp

		end
