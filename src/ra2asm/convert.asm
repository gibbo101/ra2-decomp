; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright 2026 ra2-decomp contributors

		.386
		.model	flat


		.code

; Blends each nonzero source pixel, through the translation table, half and half with the
; 16-bit destination pixel under the mask.
; RECONSTRUCTED: GAMEMD 0x00492830
?BlitForward@?$BlitTransLucent50@G@@UBEXPAXPBXHH00HH@Z proc

		push	ebp
		mov	ebp, esp
		sub	esp, 8
		mov	eax, [ebp+10h]
		xor	edx, edx
		mov	dx, word ptr [ecx+8]
		mov	[ebp+10h], eax
		mov	eax, [ecx+4]
		push	ebx
		push	esi
		push	edi
		mov	[ebp-8], edx
		mov	[ebp-4], eax

		mov	esi, [ebp+0Ch]
		mov	edi, [ebp+8]
		mov	ecx, [ebp-8]
		mov	edx, [ebp-4]

next_pixel:
		xor	eax, eax
		mov	al, [esi]
		test	eax, eax
		jz	skip_pixel
		mov	ax, [edx+eax*2]
		xor	ebx, ebx
		shr	eax, 1
		mov	bx, [edi]
		shr	ebx, 1
		and	ebx, ecx
		and	eax, ecx
		add	eax, ebx
		mov	[edi], ax
skip_pixel:
		inc	esi
		add	edi, 2
		dec	dword ptr [ebp+10h]
		jg	next_pixel

		pop	edi
		pop	esi
		pop	ebx
		mov	esp, ebp
		pop	ebp
		ret	20h

?BlitForward@?$BlitTransLucent50@G@@UBEXPAXPBXHH00HH@Z endp

		end
