; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright 2026 ra2-decomp contributors
; Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
; Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

; The voxel routines of reference/tibsun/code/winasm.asm as Red Alert 2 revised them: each
; pixel is written once, not twice, and each routine starts on a 32-byte boundary.

.686P	; Pentium Pro and above enables CMOV
.mmx
.model	flat, C ;MODEL USE32 FLAT

		; alignment has to be 'page' so that I can use 'align 32' below
		_TEXT$mmx segment page public use32 'CODE';codeseg


; externs
EXTERN C VoxelPixelDeltaTable : PTR WORD ; short [256][2]	 ; Defined in voxlib.cpp
EXTERN C VoxelNormalTranslateTable : BYTE ; uchar [256]		; Defined in voxlib.cpp
EXTERN C VoxelDrawBuffer : BYTE ; uchar [65536]				; Defined in voxdrsys.cpp
EXTERN C VoxelPaletteTranslateTable : BYTE ;  uchar [32][256]  ; Defined in voxdrsys.cpp
EXTERNDEF C UseCMOV : BYTE
EXTERNDEF C UseMMX : BYTE


; structs
Vector3i16 STRUCT
	I	SWORD	?
	J	SWORD	?
	K	SWORD	?
Vector3i16 ENDS

IF SIZEOF Vector3i16 NE 6
	;.ERR <Vector3i16 size mismatch! Expected 6 bytes>
	ECHO *** Warning: Vector3i16 is not 6 bytes as expected ***
	vecstrucSize DWORD SIZEOF Vector3i16
ENDIF


VoxelFuncArgumentStruct STRUCT ; WARNING: Make sure this struct is updated if the version in voxdrsys.h is modified!
	StartOffset		DWORD   ?				; unsigned char *
	EndOffset		DWORD   ?				; unsigned char *
	DataOffset		DWORD   ?				; unsigned char *
	StartIndex		SDWORD  ?				; int
	StrideX			SDWORD  ?				; int
	StrideY			SDWORD  ?				; int
	TransformMatrix Vector3i16 4 DUP (<>)	; Vector3i16[4]
	XSize			BYTE	?				; unsigned char
	YSize			BYTE	?				; unsigned char
	ZSize			BYTE	?				; unsigned char
	Padding			BYTE	?				; pad to make struct size divisible by 4, matching the C++ implementation layout
VoxelFuncArgumentStruct ENDS


IF SIZEOF VoxelFuncArgumentStruct NE 52
	;.ERR <VoxelFuncArgumentStruct size mismatch! Expected 48 bytes>
	ECHO *** Warning: VoxelFuncArgumentStruct is not 48 bytes as expected ***
	voxstrucSize DWORD SIZEOF VoxelFuncArgumentStruct
ENDIF

TRANSFORM_VECTOR MACRO index:req
	EXITM <dword ptr ((VoxelFuncArgumentStruct.TransformMatrix) + type Vector3i16 * index)>
ENDM

TRANSFORM_COMPONENT MACRO index:req, component:req
	LOCAL offset
	IFIDNI <component>, <I>
		offset = (Vector3i16.I)
	ELSEIFIDNI <component>, <J>
		offset = (Vector3i16.J)
	ELSEIFIDNI <component>, <K>
		offset = (Vector3i16.K)
	ELSE
		.ERR <Invalid component! Use I, J, or K>
	ENDIF

	EXITM <word ptr ((VoxelFuncArgumentStruct.TransformMatrix) + type Vector3i16 * index + offset)>
ENDM

align 32

; VOID cdecl Func(VoxelFuncArgumentStruct * arg1);
; RECONSTRUCTED: GAMEMD 0x007DF7C0
Draw_Voxel_Regular_Normals_ASM proc C uses esi edi ebx ecx edx \
		arg1:DWORD

		LOCAL local3:DWORD
		LOCAL local2:DWORD
		LOCAL local1:DWORD

		assume esi:ptr VoxelFuncArgumentStruct

		mov	esi, arg1
		push	ebp
		movsx ebp, [esi + TRANSFORM_COMPONENT(3,I)]
		movsx edx, [esi + TRANSFORM_COMPONENT(3,J)]
		xor	ecx, ecx
		mov	cl, [esi].ZSize
		mov	edi, offset VoxelPixelDeltaTable
		xor	eax, eax
		xor	ebx, ebx
		mov	esi, 4

??loc_6C8E6A:
		mov	[edi], ax
		add	edi, 2
		add	eax, ebp
		mov	[edi], bx
		add	edi, 2
		add	ebx, edx
		dec	ecx
		jnz	??loc_6C8E6A

		pop	ebp
		mov	esi, arg1
		xor	eax, eax
		mov	eax, [esi + TRANSFORM_VECTOR(0)]
		mov	local1, eax
		mov	ch, [esi].YSize

??loc_6C8E8C:
		mov	eax, [esi].StartIndex
		mov	local3, eax
		mov	eax, local1
		mov	local2, eax
		mov	cl, [esi].XSize

??loc_6C8E9B:
		mov	eax, local1
		mov	[esi + TRANSFORM_VECTOR(0)], eax
		xor	edi, edi
		mov	eax, [esi].StartIndex
		mov	ebx, [esi].StartOffset
		or	 edi, [ebx + eax*4]
		jns	??loc_6C8EE0

??loc_6C8EAD:
		mov	eax, [esi + TRANSFORM_VECTOR(1)]
		add	local1, eax
		mov	eax, [esi].StrideX
		add	[esi].StartIndex, eax
		dec	cl
		jnz	??loc_6C8E9B

		mov	eax, local2
		add	eax, [esi + TRANSFORM_VECTOR(2)]
		mov	local1, eax
		mov	eax, local3
		add	eax, [esi].StrideY
		mov	[esi].StartIndex, eax
		dec	ch
		jnz	??loc_6C8E8C

		ret

		align 8

??loc_6C8EE0:
		push	esi
		push	ecx
		push	ebp
		xor	ecx, ecx
		add	edi, [esi].DataOffset
		mov	cl, [esi].ZSize
		mov	bx, [esi + TRANSFORM_COMPONENT(0,J)]
		shl	ebx, 16
		mov	ebp, [esi + TRANSFORM_VECTOR(3)]
		mov	bx, [esi + TRANSFORM_COMPONENT(0,I)]

??loc_6C8EF9:
		xor	eax, eax
		cmp	ecx, 0
		jz	 ??loc_6C8F36

		mov	al, [edi]
		inc	edi
		sub	cl, al

		add	ebx, [VoxelPixelDeltaTable + eax*4]
		mov	ch, [edi]
		inc	edi
		test	ch, ch
		jz	 ??loc_6C8F33

??loc_6C8F13:
		mov	eax, ebx
		dec	cl
		shr	eax, 16
		mov	dl, [edi]
		mov	al, bh
		add	edi, 2
		add	ebx, ebp
		dec	ch
		mov	[VoxelDrawBuffer + eax], dl
		jnz	??loc_6C8F13

??loc_6C8F33:
		inc	edi
		jmp	??loc_6C8EF9

??loc_6C8F36:
		pop	ebp
		pop	ecx
		pop	esi
		jmp	??loc_6C8EAD

Draw_Voxel_Regular_Normals_ASM endp

align 32

; VOID cdecl Func(VoxelFuncArgumentStruct * arg1);
; RECONSTRUCTED: GAMEMD 0x007DF8C0
Draw_Voxel_Reverse_Normals_ASM proc C uses esi edi ebx ecx edx \
		arg1:DWORD

		LOCAL local3:DWORD
		LOCAL local2:DWORD
		LOCAL local1:DWORD

		assume eax:ptr VoxelFuncArgumentStruct

		mov	esi, arg1
		push	ebp
		movsx ebp, [esi + TRANSFORM_COMPONENT(3,I)]
		movsx edx, [esi + TRANSFORM_COMPONENT(3,J)]
		xor	ecx, ecx
		mov	cl, [esi].ZSize
		mov	edi, offset VoxelPixelDeltaTable
		xor	eax, eax
		xor	ebx, ebx
		mov	esi, 4

??loc_6C8F6A:
		mov	[edi], ax
		add	edi, 2
		add	eax, ebp
		mov	[edi], bx
		add	edi, 2
		add	ebx, edx
		dec	ecx
		jnz	??loc_6C8F6A

		pop	ebp
		mov	esi, arg1
		xor	eax, eax
		mov	eax, [esi + TRANSFORM_VECTOR(0)]
		mov	local1, eax
		mov	ch, [esi].YSize

??loc_6C8F8C:
		mov	eax, [esi].StartIndex
		mov	local3, eax
		mov	eax, local1
		mov	local2, eax
		mov	cl, [esi].XSize

??loc_6C8F9B:
		mov	eax, local1
		mov	[esi + TRANSFORM_VECTOR(0)], eax
		xor	edi, edi
		mov	eax, [esi].StartIndex
		mov	ebx, [esi].EndOffset
		or	 edi, [ebx + eax*4]
		jns	??loc_6C8FE0

??loc_6C8FAE:
		mov	eax, [esi + TRANSFORM_VECTOR(1)]
		add	local1, eax
		mov	eax, [esi].StrideX
		add	[esi].StartIndex, eax
		dec	cl
		jnz	??loc_6C8F9B

		mov	eax, local2
		add	eax, [esi + TRANSFORM_VECTOR(2)]
		mov	local1, eax
		mov	eax, local3
		add	eax, [esi].StrideY
		mov	[esi].StartIndex, eax
		dec	ch
		jnz	??loc_6C8F8C

		ret

		align 8

??loc_6C8FE0:
		push	esi
		push	ecx
		push	ebp
		xor	ecx, ecx
		add	edi, [esi].DataOffset
		mov	cl, [esi].ZSize
		mov	bx, [esi + TRANSFORM_COMPONENT(0,J)]
		shl	ebx, 16
		mov	ebp, [esi + TRANSFORM_VECTOR(3)]
		mov	bx, [esi + TRANSFORM_COMPONENT(0,I)]

??loc_6C8FF9:
		cmp	ecx, 0
		jz	 ??loc_6C9035

		mov	ch, [edi]
		dec	edi
		test	ch, ch
		jz	 ??loc_6C9024

??loc_6C9005:
		mov	eax, ebx
		dec	cl
		dec	edi
		shr	eax, 16
		mov	dl, [edi]
		mov	al, bh
		dec	edi
		add	ebx, ebp
		dec	ch
		mov	[VoxelDrawBuffer + eax], dl
		jnz	??loc_6C9005

??loc_6C9024:
		dec	edi
		xor	eax, eax
		mov	al, [edi]
		dec	edi
		sub	cl, al

		add	ebx, [VoxelPixelDeltaTable + eax*4]
		jmp	??loc_6C8FF9

??loc_6C9035:
		pop	ebp
		pop	ecx
		pop	esi
		jmp	??loc_6C8FAE

Draw_Voxel_Reverse_Normals_ASM endp

align 32

; VOID cdecl Func(VoxelFuncArgumentStruct * arg1);
Draw_Voxel_Regular_Lighting_Normals_ASM  proc C uses esi edi ebx ecx edx \
		arg1:DWORD

		LOCAL local3:DWORD
		LOCAL local2:DWORD
		LOCAL local1:DWORD

		assume eax:ptr VoxelFuncArgumentStruct

		mov	esi, arg1
		push	ebp
		movsx ebp, [esi + TRANSFORM_COMPONENT(3,I)]
		movsx edx, [esi + TRANSFORM_COMPONENT(3,J)]
		xor	ecx, ecx
		mov	cl, [esi].ZSize
		mov	edi, offset VoxelPixelDeltaTable
		xor	eax, eax
		xor	ebx, ebx
		mov	esi, 4

??loc_6C906A:
		mov	[edi], ax
		add	edi, 2
		add	eax, ebp
		mov	[edi], bx
		add	edi, 2
		add	ebx, edx
		dec	ecx
		jnz	??loc_6C906A

		pop	ebp
		mov	esi, arg1
		xor	eax, eax
		mov	eax, [esi + TRANSFORM_VECTOR(0)]
		mov	local1, eax
		mov	ch, [esi].YSize

??loc_6C908C:
		mov	eax, [esi].StartIndex
		mov	local3, eax
		mov	eax, local1
		mov	local2, eax
		mov	cl, [esi].XSize

??loc_6C909B:
		mov	eax, local1
		mov	[esi + TRANSFORM_VECTOR(0)], eax
		xor	edi, edi
		mov	eax, [esi].StartIndex
		mov	ebx, [esi].StartOffset
		or	 edi, [ebx + eax*4]
		jns	??loc_6C90E0

??loc_6C90AD:
		mov	eax, [esi + TRANSFORM_VECTOR(1)]
		add	local1, eax
		mov	eax, [esi].StrideX
		add	[esi].StartIndex, eax
		dec	cl
		jnz	??loc_6C909B

		mov	eax, local2
		add	eax, [esi + TRANSFORM_VECTOR(2)]
		mov	local1, eax
		mov	eax, local3
		add	eax, [esi].StrideY
		mov	[esi].StartIndex, eax
		dec	ch
		jnz	??loc_6C908C

		ret

		align 8

??loc_6C90E0:
		push	esi
		push	ecx
		push	ebp
		xor	ecx, ecx
		add	edi, [esi].DataOffset
		mov	cl, [esi].ZSize
		mov	bx, [esi + TRANSFORM_COMPONENT(0,J)]
		shl	ebx, 16
		mov	ebp, [esi + TRANSFORM_VECTOR(3)]
		mov	bx, [esi + TRANSFORM_COMPONENT(0,I)]

??loc_6C90F9:
		xor	eax, eax
		cmp	ecx, 0
		jz	 ??loc_6C9147

		mov	al, [edi]
		inc	edi
		sub	cl, al

		add	ebx, [VoxelPixelDeltaTable + eax*4]
		mov	ch, [edi]
		inc	edi
		test	ch, ch
		jz	 ??loc_6C9144

??loc_6C9113:
		mov	eax, ebx
		dec	cl
		shr	eax, 16
		xor	edx, edx
		mov	dl, [edi+1]
		mov	al, bh
		mov	dh, VoxelNormalTranslateTable[edx]
		mov	dl, [edi]
		add	ebx, ebp
		add	edi, 2
		dec	ch
		mov	dl, VoxelPaletteTranslateTable[edx]
		mov	[VoxelDrawBuffer + eax], dl
		jnz	??loc_6C9113

??loc_6C9144:
		inc	edi
		jmp	??loc_6C90F9

??loc_6C9147:
		pop	ebp
		pop	ecx
		pop	esi
		jmp	??loc_6C90AD

Draw_Voxel_Regular_Lighting_Normals_ASM endp

align 32

; VOID cdecl Func(VoxelFuncArgumentStruct * arg1);
Draw_Voxel_Reverse_Lighting_Normals_ASM proc C uses esi edi ebx ecx edx \
		arg1:DWORD

		LOCAL local3:DWORD
		LOCAL local2:DWORD
		LOCAL local1:DWORD

		assume eax:ptr VoxelFuncArgumentStruct

		mov	esi, arg1
		push	ebp
		movsx ebp, [esi + TRANSFORM_COMPONENT(3,I)]
		movsx edx, [esi + TRANSFORM_COMPONENT(3,J)]
		xor	ecx, ecx
		mov	cl, [esi].ZSize
		mov	edi, offset VoxelPixelDeltaTable
		xor	eax, eax
		xor	ebx, ebx
		mov	esi, 4

??loc_6C918A:
		mov	[edi], ax
		add	edi, 2
		add	eax, ebp
		mov	[edi], bx
		add	edi, 2
		add	ebx, edx
		dec	ecx
		jnz	??loc_6C918A

		pop	ebp
		mov	esi, arg1
		xor	eax, eax
		mov	eax, [esi + TRANSFORM_VECTOR(0)]
		mov	local1, eax
		mov	ch, [esi].YSize

??loc_6C91AC:
		mov	eax, [esi].StartIndex
		mov	local3, eax
		mov	eax, local1
		mov	local2, eax
		mov	cl, [esi].XSize

??loc_6C91BB:
		mov	eax, local1
		mov	[esi + TRANSFORM_VECTOR(0)], eax
		xor	edi, edi
		mov	eax, [esi].StartIndex
		mov	ebx, [esi].EndOffset
		or	 edi, [ebx + eax*4]
		jns	??loc_6C9200

??loc_6C91CE:
		mov	eax, [esi + TRANSFORM_VECTOR(1)]
		add	local1, eax
		mov	eax, [esi].StrideX
		add	[esi].StartIndex, eax
		dec	cl
		jnz	??loc_6C91BB

		mov	eax, local2
		add	eax, [esi + TRANSFORM_VECTOR(2)]
		mov	local1, eax
		mov	eax, local3
		add	eax, [esi].StrideY
		mov	[esi].StartIndex, eax
		dec	ch
		jnz	??loc_6C91AC

		ret

		align 8

??loc_6C9200:
		push	esi
		push	ecx
		push	ebp
		xor	ecx, ecx
		add	edi, [esi].DataOffset
		mov	cl, [esi].ZSize
		mov	bx, [esi + TRANSFORM_COMPONENT(0,J)]
		shl	ebx, 16
		mov	ebp, [esi + TRANSFORM_VECTOR(3)]
		mov	bx, [esi + TRANSFORM_COMPONENT(0,I)]

??loc_6C9219:
		cmp	ecx, 0
		jz	 ??loc_6C9267

		mov	ch, [edi]
		dec	edi
		test	ch, ch
		jz	 ??loc_6C9256

??loc_6C9225:
		mov	eax, ebx
		dec	cl
		shr	eax, 16
		xor	edx, edx
		mov	dl, [edi]
		mov	al, bh
		mov	dh, VoxelNormalTranslateTable[edx]
		mov	dl, [edi-1]
		add	ebx, ebp
		sub	edi, 2
		dec	ch
		mov	dl, VoxelPaletteTranslateTable[edx]
		mov	[VoxelDrawBuffer + eax], dl
		jnz	??loc_6C9225

??loc_6C9256:
		dec	edi
		xor	eax, eax
		mov	al, [edi]
		dec	edi
		sub	cl, al

		add	ebx, [VoxelPixelDeltaTable + eax*4]
		jmp	??loc_6C9219

??loc_6C9267:
		pop	ebp
		pop	ecx
		pop	esi
		jmp	??loc_6C91CE

Draw_Voxel_Reverse_Lighting_Normals_ASM endp

align 32

; VOID cdecl Func(VoxelFuncArgumentStruct * arg1);
Draw_Voxel_Regular_ASM proc C uses esi edi ebx ecx edx \
		arg1:DWORD

		LOCAL local3:DWORD
		LOCAL local2:DWORD
		LOCAL local1:DWORD

		assume eax:ptr VoxelFuncArgumentStruct

		mov	esi, arg1
		push	ebp
		movsx ebp, [esi + TRANSFORM_COMPONENT(3,I)]
		movsx edx, [esi + TRANSFORM_COMPONENT(3,J)]
		mov	ecx, 0FFh
		sub	cl, [esi].ZSize
		mov	edi, offset VoxelPixelDeltaTable
		xor	eax, eax
		xor	ebx, ebx
		mov	esi, 4

??loc_6C92AD:
		mov	[edi], ax
		add	edi, 2
		add	eax, ebp
		mov	[edi], bx
		add	edi, 2
		add	ebx, edx
		dec	ecx
		jnz	??loc_6C92AD

		pop	ebp
		mov	esi, arg1
		xor	eax, eax
		mov	eax, [esi + TRANSFORM_VECTOR(0)]
		mov	local1, eax
		mov	ch, [esi].YSize

??loc_6C92CF:
		mov	eax, [esi].StartIndex
		mov	local3, eax
		mov	eax, local1
		mov	local2, eax
		mov	cl, [esi].XSize

??loc_6C92DE:
		mov	eax, local1
		mov	[esi + TRANSFORM_VECTOR(0)], eax
		xor	edi, edi
		mov	eax, [esi].StartIndex
		mov	ebx, [esi].StartOffset
		or	 edi, [ebx + eax*4]
		jns	??loc_6C9320

??loc_6C92F0:
		mov	eax, [esi + TRANSFORM_VECTOR(1)]
		add	local1, eax
		mov	eax, [esi].StrideX
		add	[esi].StartIndex, eax
		dec	cl
		jnz	??loc_6C92DE

		mov	eax, local2
		add	eax, [esi + TRANSFORM_VECTOR(2)]
		mov	local1, eax
		mov	eax, local3
		add	eax, [esi].StrideY
		mov	[esi].StartIndex, eax
		dec	ch
		jnz	??loc_6C92CF

		ret

		align 8

??loc_6C9320:
		push	esi
		push	ecx
		push	ebp
		xor	ecx, ecx
		add	edi, [esi].DataOffset
		mov	cl, [esi].ZSize
		mov	bx, [esi + TRANSFORM_COMPONENT(0,J)]
		shl	ebx, 16
		mov	ebp, [esi + TRANSFORM_VECTOR(3)]
		mov	bx, [esi + TRANSFORM_COMPONENT(0,I)]

??loc_6C9339:
		xor	eax, eax
		cmp	ecx, 0
		jz	 ??loc_6C936E

		mov	al, [edi]
		inc	edi
		sub	cl, al

		add	ebx, [VoxelPixelDeltaTable + eax*4]
		mov	ch, [edi]
		inc	edi
		test	ch, ch
		jz	 ??loc_6C936B

??loc_6C9353:
		mov	eax, ebx
		dec	cl
		shr	eax, 16
		mov	dl, [edi]
		mov	al, bh
		inc	edi
		add	ebx, ebp
		dec	ch
		mov	[VoxelDrawBuffer + eax], dl
		jnz	??loc_6C9353

??loc_6C936B:
		inc	edi
		jmp	??loc_6C9339

??loc_6C936E:
		pop	ebp
		pop	ecx
		pop	esi
		jmp	??loc_6C92F0

Draw_Voxel_Regular_ASM endp

align 32

; VOID cdecl Func(VoxelFuncArgumentStruct * arg1);
Draw_Voxel_Reverse_ASM proc C uses esi edi ebx ecx edx \
		arg1:DWORD

		LOCAL local3:DWORD
		LOCAL local2:DWORD
		LOCAL local1:DWORD

		assume eax:ptr VoxelFuncArgumentStruct

		mov	esi, arg1
		push	ebp
		movsx ebp, [esi + TRANSFORM_COMPONENT(3,I)]
		movsx edx, [esi + TRANSFORM_COMPONENT(3,J)]
		mov	ecx, 0FFh
		sub	cl, [esi].ZSize
		mov	edi, offset VoxelPixelDeltaTable
		xor	eax, eax
		xor	ebx, ebx
		mov	esi, 4

??loc_6C93AD:
		mov	[edi], ax
		add	edi, 2
		add	eax, ebp
		mov	[edi], bx
		add	edi, 2
		add	ebx, edx
		dec	ecx
		jnz	??loc_6C93AD

		pop	ebp
		mov	esi, arg1
		xor	eax, eax
		mov	eax, [esi + TRANSFORM_VECTOR(0)]
		mov	local1, eax
		mov	ch, [esi].YSize

??loc_6C93CF:
		mov	eax, [esi].StartIndex
		mov	local3, eax
		mov	eax, local1
		mov	local2, eax
		mov	cl, [esi].XSize

??loc_6C93DE:
		mov	eax, local1
		mov	[esi + TRANSFORM_VECTOR(0)], eax
		xor	edi, edi
		mov	eax, [esi].StartIndex
		mov	ebx, [esi].EndOffset
		or	 edi, [ebx + eax*4]
		jns	??loc_6C9420

??loc_6C93F1:
		mov	eax, [esi + TRANSFORM_VECTOR(1)]
		add	local1, eax
		mov	eax, [esi].StrideX
		add	[esi].StartIndex, eax
		dec	cl
		jnz	??loc_6C93DE

		mov	eax, local2
		add	eax, [esi + TRANSFORM_VECTOR(2)]
		mov	local1, eax
		mov	eax, local3
		add	eax, [esi].StrideY
		mov	[esi].StartIndex, eax
		dec	ch
		jnz	??loc_6C93CF

		ret

		align 8

??loc_6C9420:
		push	esi
		push	ecx
		push	ebp
		xor	ecx, ecx
		add	edi, [esi].DataOffset
		mov	cl, [esi].ZSize
		mov	bx, [esi + TRANSFORM_COMPONENT(0,J)]
		shl	ebx, 16
		mov	ebp, [esi + TRANSFORM_VECTOR(3)]
		mov	bx, [esi + TRANSFORM_COMPONENT(0,I)]

??loc_6C9439:
		cmp	ecx, 0
		jz	 ??loc_6C946E

		mov	ch, [edi]
		dec	edi
		test	ch, ch
		jz	 ??loc_6C945D

??loc_6C9445:
		mov	eax, ebx
		dec	cl
		shr	eax, 16
		mov	dl, [edi]
		mov	al, bh
		dec	edi
		add	ebx, ebp
		dec	ch
		mov	[VoxelDrawBuffer + eax], dl
		jnz	??loc_6C9445

??loc_6C945D:
		dec	edi
		xor	eax, eax
		mov	al, [edi]
		dec	edi
		sub	cl, al

		add	ebx, [VoxelPixelDeltaTable + eax*4]
		jmp	??loc_6C9439

??loc_6C946E:
		pop	ebp
		pop	ecx
		pop	esi
		jmp	??loc_6C93F1

Draw_Voxel_Reverse_ASM endp


align 32

		_TEXT$mmx ends

end
