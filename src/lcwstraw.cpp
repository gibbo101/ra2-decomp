// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "lcwstraw.h"
#include "assert.h"
#include "lcw.h"
#include "rect.h"
#include <cstring>

// FUNCTION: GAMEMD 0x005523E0
LCWStraw::LCWStraw(CompControl control, int blocksize) :
		Control(control),
		Counter(0),
		Buffer(NULL),
		Buffer2(NULL),
		BlockSize(blocksize)
{
	SafetyMargin = BlockSize/128+1;
	Buffer = new char[BlockSize+SafetyMargin];
	if (control == COMPRESS) {
		Buffer2 = new char[BlockSize+SafetyMargin];
	}
}

// FUNCTION: GAMEMD 0x00552450
LCWStraw::~LCWStraw(void)
{
	delete [] Buffer;
	Buffer = NULL;

	delete [] Buffer2;
	Buffer2 = NULL;
}

// FUNCTION: GAMEMD 0x00552490
int LCWStraw::Get(void * destbuf, int slen)
{
	int total = 0;

	if (destbuf == NULL || slen < 1) {
		return(0);
	}

	while (slen > 0) {

		if (Counter > 0) {
			int len = (slen < Counter) ? slen : Counter;
			if (Control == DECOMPRESS) {
				memmove(destbuf, &Buffer[BlockHeader.UncompCount-Counter], len);
			} else {
				memmove(destbuf, &Buffer2[(BlockHeader.CompCount+sizeof(BlockHeader))-Counter], len);
			}
			destbuf = ((char *)destbuf) + len;
			slen -= len;
			Counter -= len;
			total += len;
		}
		if (slen == 0) break;

		if (Control == DECOMPRESS) {
			int incount = BASECLASS::Get(&BlockHeader, sizeof(BlockHeader));
			if (incount != sizeof(BlockHeader)) break;

			void * ptr = &Buffer[(BlockSize+SafetyMargin) - BlockHeader.CompCount];
			incount = BASECLASS::Get(ptr, BlockHeader.CompCount);
			if (incount != BlockHeader.CompCount) break;

			LCW_Uncomp(ptr, Buffer);
			Counter = BlockHeader.UncompCount;
		} else {
			BlockHeader.UncompCount = (unsigned short)BASECLASS::Get(Buffer, BlockSize);
			if (BlockHeader.UncompCount == 0) break;
			BlockHeader.CompCount = (unsigned short)LCW_Comp(Buffer, &Buffer2[sizeof(BlockHeader)], BlockHeader.UncompCount);
			memmove(Buffer2, &BlockHeader, sizeof(BlockHeader));
			Counter = BlockHeader.CompCount+sizeof(BlockHeader);
		}
	}

	return(total);
}

// SYNTHETIC: GAMEMD 0x005525F0
// ??_GLCWStraw@@UAEPAXI@Z

extern "C" {

// FUNCTION: GAMEMD 0x00552640
unsigned long __cdecl LCW_Uncompress (void * source, void * dest, unsigned long )
{
	unsigned char * source_ptr, * dest_ptr, * copy_ptr, op_code, data;
	unsigned	  count, * word_dest_ptr, word_data;

	source_ptr = (unsigned char*) source;
	dest_ptr   = (unsigned char*) dest;

	while (1 /*TRUE*/) {

		op_code = *source_ptr++;

		if (!(op_code & 0x80)) {

			count	 = (op_code >> 4) + 3;
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

						count	 = *source_ptr + ((unsigned) *(source_ptr + 1) << 8);
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

}

// SYNTHETIC: GAMEMD 0x00552830 SYMBOL
// _$E2
