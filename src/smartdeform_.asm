;******************************************************************************
;*                               O P E N  T S
;******************************************************************************
;* SPDX-License-Identifier: GPL-3.0-or-later
;* Copyright 2026 OpenTS contributors
;*
;* See LICENSE.md for applicable additional terms and warranty disclaimers.
;******************************************************************************

.386
.model flat, C

FALSE equ 0
TRUE equ 1

FLAG_RIGID equ 1
FLAG_DONE equ 2

DeformPointStruct	struc

	Height	dd	0

	;NOTE C++ version of Ripple_Deform_Points passes a struct with bools
	;so this is out of date
	Flags	dd	0

DeformPointStruct	ends

externdef	C	DeformPoints:dword
externdef	C	DeformPointHeight:dword
externdef	C	DeformPointWidth:dword
externdef	C	DeformPointXAdd:dword
externdef	C	DeformPointYAdd:dword

RAMP_HALF_HEIGHT equ 12

PUBLIC C Asm_Ripple_Deform_Points

.CODE

; FUNCTION: GAMEMD 0x007C6620
Asm_Ripple_Deform_Points	PROC	NEAR	USES ESI EDI EBX ECX \
									startpointx:DWORD, startpointy:DWORD, general_direction:DWORD, forced:DWORD

	local	startheight:dword
	local	x:dword
	local	y:dword


		assume esi:ptr DeformPointStruct

		mov  eax, [startpointy]
		mov  esi, [DeformPoints]
		imul eax, [DeformPointWidth]
		add  eax, [startpointx]
		lea  esi, [esi+eax*sizeof(DeformPointStruct)]
		mov  eax, [esi].Height
		mov  [startheight], eax

		mov  edi, -1

	setx:
		mov  ecx, -1

	check:

		mov  eax, edi
		or   eax, ecx
		test eax, eax
		je   next

		mov  eax, [startpointx]
		add  eax, ecx
		mov  [x], eax
		mov  ebx, [startpointy]
		add  ebx, edi
		mov  [y], ebx
		imul ebx, [DeformPointWidth]
		mov  esi, [DeformPoints]
		add  ebx, eax
		lea  esi, [esi+ebx*sizeof(DeformPointStruct)]
		test [esi].Flags, FLAG_RIGID
		je   adjust1

		cmp  [forced], FALSE
		jne  next

		mov  eax, [esi].Height
		sub  eax, [startheight]
		cdq
		xor  eax, edx
		sub  eax, edx
		cmp  eax, RAMP_HALF_HEIGHT
		jbe  next

		xor  eax, eax

		ret

	adjust1:

		mov  eax, [esi].Height
		sub  eax, [startheight]
		cdq
		xor  eax, edx
		sub  eax, edx
		cmp  eax, RAMP_HALF_HEIGHT
		jbe  next

		cmp  [general_direction], TRUE
		jne  adjust2

		mov  eax, [esi].Height
		cmp  eax, [startheight]
		jnb  fixup

		mov  eax, [startheight]
		sub  eax, RAMP_HALF_HEIGHT
		mov  [esi].Height, eax
		or   [esi].Flags, FLAG_DONE
		jmp  fixup

	adjust2:

		mov  eax, [esi].Height
		cmp  eax, [startheight]
		jbe  fixup

		mov  eax, [startheight]
		add  eax, RAMP_HALF_HEIGHT
		mov  [esi].Height, eax
		or   [esi].Flags, FLAG_DONE

	fixup:
		push ecx
		push edi

		push [forced]
		push [general_direction]
		push [y]
		push [x]
		call Asm_Ripple_Deform_Points
		lea  esp, [esp+010h]
		pop  edi
		pop  ecx

		test eax, eax
		jne  next

		cmp  [forced], FALSE
		jne  next

		xor  eax, eax

		ret

	next:
		inc  ecx
		cmp  ecx, FLAG_DONE
		jl   check

		inc  edi
		cmp  edi, FLAG_DONE
		jl   setx

		mov  eax, TRUE

		ret

	Asm_Ripple_Deform_Points	ENDP


.DATA

	DeformPointHeight	dd 0
	DeformPointWidth		dd 0
	DeformPointXAdd		dd 0
	DeformPointYAdd		dd 0


END