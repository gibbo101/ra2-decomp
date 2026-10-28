// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include	"always.h"
#include	"lcw.h"

// FUNCTION: GAMEMD 0x00551C60
int LCW_Uncomp(void const * source, void * dest, unsigned long )
{
	unsigned char * source_ptr, * dest_ptr, * copy_ptr;
	unsigned char op_code, data;
	unsigned count;
	unsigned * word_dest_ptr;
	unsigned word_data;

	source_ptr = (unsigned char*) source;
	dest_ptr   = (unsigned char*) dest;

	for (;;) {
		op_code = *source_ptr++;

		if (!(op_code & 0x80)) {
			count = (op_code >> 4) + 3;
			copy_ptr = dest_ptr - ((unsigned) *source_ptr++ + (((unsigned) op_code & 0x0f) << 8));

			while (count--) *dest_ptr++ = *copy_ptr++;

		} else {
			if (!(op_code & 0x40)) {
				if (op_code == 0x80) {
					return((unsigned long) (dest_ptr - (unsigned char*) dest));

				} else {
					count = op_code & 0x3f;

					while (count--) *dest_ptr++ = *source_ptr++;
				}

			} else {
				if (op_code == 0xfe) {
					count = *source_ptr + ((unsigned) *(source_ptr + 1) << 8);
					word_data = data = *(source_ptr + 2);
					word_data  = (word_data << 24) + (word_data << 16) + (word_data << 8) + word_data;
					source_ptr += 3;

					copy_ptr = dest_ptr + 4 - ((unsigned) dest_ptr & 0x3);
					count -= (copy_ptr - dest_ptr);
					while (dest_ptr < copy_ptr) *dest_ptr++ = data;

					word_dest_ptr = (unsigned*) dest_ptr;

					dest_ptr += (count & 0xfffffffc);

					while (word_dest_ptr < (unsigned*) dest_ptr) {
						*word_dest_ptr		= word_data;
						*(word_dest_ptr + 1) = word_data;
						word_dest_ptr += 2;
					}

					copy_ptr = dest_ptr + (count & 0x3);
					while (dest_ptr < copy_ptr) *dest_ptr++ = data;

				} else {
					if (op_code == 0xff) {
						count = *source_ptr + ((unsigned) *(source_ptr + 1) << 8);
						copy_ptr = (unsigned char*) dest + *(source_ptr + 2) + ((unsigned) *(source_ptr + 3) << 8);
						source_ptr += 4;

						while (count--) *dest_ptr++ = *copy_ptr++;

					} else {
						count = (op_code & 0x3f) + 3;
						copy_ptr = (unsigned char*) dest + *source_ptr + ((unsigned) *(source_ptr + 1) << 8);
						source_ptr += 2;

						while (count--) *dest_ptr++ = *copy_ptr++;
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00551E50
int LCW_Comp(void const * source, void * dest, int datasize)
{
	int inlen = 0;
	int a1stdest = 0;
	int a1stsrc = 0;
	int lenoff = 0;
	int ndest = 0;
	int count = 0;
	int matchoff = 0;
	int end_of_data = 0;
	int retval = 0;
#ifdef _DEBUG
	inlen = inlen;
	a1stdest = a1stdest;
	a1stsrc = a1stsrc;
	lenoff = lenoff;
	ndest = ndest;
	count = count;
	matchoff = matchoff;
	end_of_data = end_of_data;
#endif

	__asm {
		cld
		mov	edi,[dest]
		mov	esi,[source]
		mov	edx,[datasize]


		mov	ebx,esi
		add	ebx,edx
		mov	[end_of_data],ebx
		mov	[inlen],1
		mov	[a1stdest],edi
		mov	[a1stsrc],esi
		mov	[lenoff],edi
		sub	eax,eax
		mov	al,081h
		stosb
		lodsb
		stosb
	}

loopstart:
	__asm {
		mov	[ndest],edi
		mov	edi,[a1stsrc]
		mov	[count],1
	}
searchloop:
	__asm {
		sub	eax,eax
		mov	al,[esi]
		cmp	al,[esi+64]
		jne	short notrunlength

		mov	ebx,edi

		mov	edi,esi
		mov	ecx,[end_of_data]
		sub	ecx,edi
		repe	scasb
		dec	edi
		mov	ecx,edi
		sub	ecx,esi
		cmp	ecx,65
		jb	short notlongenough

		mov	[inlen],0
		mov	esi,edi
		mov	edi,[ndest]

		mov	ah,al
		mov	al,0FEh
		stosb
		xchg	ecx,eax
		stosw
		mov	al,ch
		stosb

		mov	[ndest],edi
		mov	edi,ebx
		jmp	searchloop
	}
notlongenough:
	__asm {
		mov	edi,ebx
	}
notrunlength:
oploop:
	__asm {
		mov	ecx,esi
		sub	ecx,edi
		jz	short searchdone

		repne	scasb
		jne	short searchdone

		mov	ebx,[count]
		mov	ah,[esi+ebx-1]
		cmp	ah,[edi+ebx-2]

		jne	oploop

		mov	edx,esi
		mov	ebx,edi
		dec	edi
		mov	ecx,[end_of_data]
		sub	ecx,esi

		repe	cmpsb

		jne	short notend

		inc	edi
	}
notend:
	__asm {
		mov	esi,edx
		mov	eax,edi
		sub	eax,ebx
		mov	edi,ebx
		cmp	eax,[count]
		jb	searchloop

		mov	[count],eax
		dec	ebx
		mov	[matchoff],ebx
		jmp	searchloop
	}
searchdone:
	__asm {
		mov	ecx,[count]
		mov	edi,[ndest]
		cmp	ecx,2
		jbe	short lenin

		cmp	ecx,10
		ja	short medrun

		mov	eax,esi
		sub	eax,[matchoff]
		cmp	eax,0FFFh
		ja	short medrun
	}
	__asm {
		sub	ebx,ebx
		mov	bl,cl
		sub	bl,3
		shl	bl,4
		add	ah,bl
		xchg	ah,al
		jmp	short srunnxt
	}
medrun:
	__asm {
		cmp	ecx,64
		ja	short longrun

		sub	cl,3
		or	cl,0C0h
		mov	al,cl
		stosb
		jmp	short medrunnxt
	}
lenin:
	__asm {
		cmp	[inlen],0
		jnz	short len
	}
lenin1:
	__asm {
		mov	[lenoff],edi
		mov	al,80h
		stosb
	}
len:
	__asm {
		mov	ebx,[lenoff]
		cmp	[ebx],0BFh
		je	lenin1
	}
	__asm {
		inc	[ebx]
		lodsb
		stosb
		mov	[inlen],1
		jmp	short nxt
	}
longrun:
	__asm {
		mov	al,0ffh
		stosb

		mov	eax,[count]
		stosw
	}
medrunnxt:
	__asm {
		mov	eax,[matchoff]
		sub	eax,[a1stsrc]
	}
srunnxt:
	__asm {
		stosw
		add	esi,[count]
		mov	[inlen],0
	}
nxt:
	__asm {
		cmp	esi,[end_of_data]
		jae	short outofhere

		jmp	loopstart
	}
outofhere:
	__asm {
		mov	ax,080h
		stosb
		mov	eax,edi
		sub	eax,[a1stdest]
		mov	[retval],eax
	}

	return(retval);
}
