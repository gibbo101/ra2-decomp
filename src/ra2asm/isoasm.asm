; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright 2026 ra2-decomp contributors
; Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
; Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

; Iso_Blit_Asm1 of reference/tibsun/code/isoasm.asm as Red Alert 2 revised it: each row
; steps the tile shape by the span width at +0, where Tiberian Sun steps it by 48.

		.386
		.model	flat, c


		.code

; RECONSTRUCTED: GAMEMD 0x007C60C0
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
	add     eax, [ebp+0]
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
	add     edx, [ebp+0]
	mov     eax, [esi+edx*4]
	add     [ebp+4Ch], eax
	add     [ebp+50h], eax

??loc_6B25CD:
	inc     ch
	cmp     ch, [ebp+4]
	jl      ??loc_6B24F2

	ret

	Iso_Blit_Asm1 endp

		end
