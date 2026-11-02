;******************************************************************************
;*                               O P E N  T S
;******************************************************************************
;* SPDX-License-Identifier: GPL-3.0-or-later
;* Copyright 2025 Electronic Arts Inc.
;* Copyright 2026 OpenTS contributors
;*
;* Contains material derived from Electronic Arts source code.
;* Modified by OpenTS contributors, 2026.
;* EA's GPLv3 Section 7 additional terms and supplemental warranty
;* disclaimers apply; see LICENSE.md.
;******************************************************************************

;***************************************************************************
;**   C O N F I D E N T I A L --- W E S T W O O D   S T U D I O S  I N C  **
;***************************************************************************
;*                                                                         *
;*                 Project Name : Command & Conquer                        *
;*                                                                         *
;*                    File Name : MMX.ASM                                  *
;*                                                                         *
;*                   Programmer : Steve Tall                               *
;*                                                                         *
;*                   Start Date : May 19th, 1996                           *
;*                                                                         *
;*                  Last Update : May 19th 1996 [ST]                       *
;*                                                                         *
;*-------------------------------------------------------------------------*
;* Functions:                                                              *
;* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *


 ;		include	<mmx.inc>


		.586 ;.686
		.model	flat


		.code

externdef	C UseCMOV:byte
externdef	C HasCMOV:byte
externdef	C UseMMX:byte
externdef	C CPUType:byte
externdef	C VendorID:byte



;*********************************************************************************************
;* Detect_MMX_Availability -- Detect the presence of MMX technology.                         *
;*                                                                                           *
;*                                                                                           *
;* INPUT:	Nothing																			 *
;*                                                                                           *
;* OUTPUT:      True if MMX technology is available.                                         *
;*                                                                                           *
;* Warnings:                                                                                 *
;*                                                                                           *
;* Note: Based in part on CPUID32.ASM by Intel                                               *
;*                                                                                           *
;* HISTORY:                                                                                  *
;*   05/19/96 ST : Created.                                                                  *
;*===========================================================================================*

Detect_MMX_Availability proc C uses ebx ecx

		local	idflag:byte
		local	local_cputype:byte

;assume processor is at least 386
;
;check whether AC bit in eflags can be toggled.
;If not then processor is 386

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


;processor is at least 486
;
;Check for ability to set/clear ID flag in EFLAGS
;ID flag indicates ability of processor to execute the CPUID instruction.
;486 not guaranteed to have CPUID inst?
;
		mov	[local_cputype],4
		mov	eax,ecx		;original EFLAGS
		xor	eax,200000h	;toggle ID bit
		push	eax
		popfd
		pushfd
		pop	eax
		xor	eax,ecx		;check if still toggled
		jz	@@end_get_cpu


;       Execute CPUID instruction to determine vendor, family,
;       model and stepping.
;

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

		and     ax,0f00H
		shr     ax,08H

		mov	[local_cputype],al

@@end_get_cpu:	mov	al,[local_cputype]
		mov	[CPUType],al


;
; We have the CPU type in al now.
; If we arent on at least a pentium then we can assume there is no MMX
;
		cmp	al,5
		jb	@@no_mmx

		mov	eax,1
		cpuid
		test	edx,00800000h
		jz	@@no_mmx

;
; MMX detected - return true
;
		mov	eax,1
		mov UseMMX, 1
		ret


@@no_mmx:	xor	eax,eax
		mov UseMMX, 0
		ret


Detect_MMX_Availability endp



; FUNCTION: GAMEMD 0x007C5FFF
Detect_CMOV_Availability proc C uses ebx ecx
		cmp CPUType, 5
		jb @@no_cmov

		mov eax, 1
		cpuid
		test edx, 00008000h
		jz @@no_cmov

		cmp CPUType, 5
		jbe @@supports_cmov

@@use_cmov:
		mov UseCMOV, 1
		mov HasCMOV, 1
		mov eax, 1
		ret

@@supports_cmov:
		mov UseCMOV, 0
		mov HasCMOV, 1
		mov eax, 1
		ret

@@no_cmov:
		mov UseCMOV, 0
		mov HasCMOV, 0
		mov eax, 0
		ret

Detect_CMOV_Availability endp



;***********************************************************************************************
;* Get_CPU_Clock -- Fetches the current CPU clock time.                                        *
;*                                                                                             *
;*    This routine will return the internal Pentium clock accumulator. This accumulator is     *
;*    incremented every clock tick. Since this clock value can get very very large, the value  *
;*    returned is in 64 bits. The low half is returned directly, the high half is stored in    *
;*    location specified.                                                                      *
;*                                                                                             *
;* INPUT:   high  -- Reference to the high value of the 64 bit clock number.                   *
;*                                                                                             *
;* OUTPUT:  Returns with the low half of the CPU clock value.                                  *
;*                                                                                             *
;* WARNINGS:   This instruction is only available on Pentium or later processors.              *
;*                                                                                             *
;* HISTORY:                                                                                    *
;*   07/17/1996 JLB : Created.                                                                 *
;*=============================================================================================*

; FUNCTION: GAMEMD 0x007C6064
Get_CPU_Clock proc C uses esi _high:dword

		rdtsc
		mov esi, _high
		mov [esi], edx
		ret

Get_CPU_Clock endp



; FUNCTION: GAMEMD 0x007C6072
Processor proc C uses ebx

		local	idflag:word

;assume processor is at least 386
;
;check whether AC bit in eflags can be toggled.
;If not then processor is 386
		pushfd

		mov	[idflag],0

		pushfd			;get Eflags in EAX
		pop	eax
		mov	ebx,eax		;save eflags
		xor	eax,40000h	;toggle AC bit in eflags
		push	eax		;new eflags on stack
		popfd			;move new value into eflags
		pushfd			;get new eflags back into eax
		pop	eax
		xor	eax,ebx		;if AC bit not toggled then CPU=386
		jz	@@end_get_cpu	;cpu is 386


;processor is at least 486
;
;Check for ability to set/clear ID flag in EFLAGS
;ID flag indicates ability of processor to execute the CPUID instruction.
;486 not guaranteed to have CPUID inst?
;
		mov	[idflag],1	;flag ID is available
		pushfd
		pop eax
		mov	ebx,eax		;original EFLAGS
		xor	eax,200000h	;toggle ID bit
		push	eax
		popfd
		pushfd
		pop	eax
		xor	eax,ebx		;check if still toggled
		jz	@@end_get_cpu


;       Execute CPUID instruction to determine vendor, family,
;       model and stepping.
;
		mov	[idflag],2

@@end_get_cpu:
		popfd
		sub eax, eax
		mov ax, [idflag]
		ret

Processor endp



		.data

UseCMOV		db	0
HasCMOV		db	0
UseMMX		db	0
CPUType		db	0
VendorID	db	"Not available",0,0,0,0,0,0


end