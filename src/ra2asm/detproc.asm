; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright 2026 ra2-decomp contributors
; Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
; Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

; Detect_MMX_Availability of reference/tibsun/code/detproc.asm as Red Alert 2 revised it:
; it also keeps edx and records the processor model.

		.586
		.model	flat


		.code

externdef	C UseMMX:byte
externdef	C CPUType:byte
externdef	C DAT_0084E864:byte
externdef	C VendorID:byte


; RECONSTRUCTED: GAMEMD 0x007C5F3D
Detect_MMX_Availability proc C uses ebx ecx edx

		local	idflag:byte
		local	local_cputype:byte

		mov	[DAT_0084E864],0
		mov	[idflag],0

		pushfd			;get Eflags in EAX
		pop	eax
		mov	ecx,eax		;save eflags
		xor	eax,40000h	;toggle AC bit in eflags
		push	eax		;new eflags on stack
		popfd			;move new value into eflags
		pushfd			;get new eflags back into eax
		pop	eax
		xor	eax,ecx		;if AC bit not toggled then CPU=386
		mov	[local_cputype],3
		jz	@@end_get_cpu	;cpu is 386

		push	ecx
		popfd			;restore AC bit in eflags

		mov	[local_cputype],4
		mov	eax,ecx		;original EFLAGS
		xor	eax,200000h	;toggle ID bit
		push	eax
		popfd
		pushfd
		pop	eax
		xor	eax,ecx		;check if still toggled
		jz	@@end_get_cpu

		mov	[idflag],1	;flag ID is available

		xor	eax,eax
		cpuid

		mov	dword ptr [VendorID],ebx
		mov	dword ptr [VendorID+4],edx
		mov	dword ptr [VendorID+8],ecx
		mov	dword ptr [VendorID+12]," "

		cmp	eax,1		;check if 1 is valid
		jl	@@end_get_cpu	;inp for cpuid inst.

		xor	eax,eax
		inc	eax

		cpuid		;get stepping, model and family

		mov	edx,eax
		and	edx,0f0H
		shr	edx,04H
		mov	[DAT_0084E864],dl

		and	eax,0f00H
		shr	eax,08H

		mov	[local_cputype],al

@@end_get_cpu:	mov	al,[local_cputype]
		mov	[CPUType],al

		cmp	al,5
		jb	@@no_mmx

		mov	eax,1
		cpuid
		test	edx,00800000h
		jz	@@no_mmx

		mov	eax,1
		mov UseMMX, 1
		ret


@@no_mmx:	xor	eax,eax
		mov UseMMX, 0
		ret


Detect_MMX_Availability endp

		end
