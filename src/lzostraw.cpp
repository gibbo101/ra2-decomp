// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "lzostraw.h"
#include "assert.h"
#include "lzo.h"
#include <cstring>

// FUNCTION: 0x0055C720
LZOStraw::LZOStraw(CompControl control, int blocksize) :
		Control(control),
		Counter(0),
		Buffer(NULL),
		Buffer2(NULL),
		BlockSize(blocksize)
{
	SafetyMargin = BlockSize;
	Buffer = new char[BlockSize+SafetyMargin];
	if (control == COMPRESS) {
		Buffer2 = new char[BlockSize+SafetyMargin];
	}
}

// FUNCTION: 0x0055C780
LZOStraw::~LZOStraw(void)
{
	delete [] Buffer;
	Buffer = NULL;

	delete [] Buffer2;
	Buffer2 = NULL;
}

// FUNCTION: 0x0055C7C0
int LZOStraw::Get(void * destbuf, int slen)
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

			char *staging_buffer = new char [BlockHeader.CompCount];
			incount = BASECLASS::Get(staging_buffer, BlockHeader.CompCount);
			if (incount != BlockHeader.CompCount) {
				if (staging_buffer != NULL) delete [] staging_buffer;
				break;
			}
			unsigned int length = sizeof(Buffer);
			lzo1x_decompress ((unsigned char*)staging_buffer, BlockHeader.CompCount, (unsigned char*)Buffer, &length, NULL);
			delete [] staging_buffer;
			Counter = BlockHeader.UncompCount;
		} else {
			BlockHeader.UncompCount = (unsigned short)BASECLASS::Get(Buffer, BlockSize);
			if (BlockHeader.UncompCount == 0) break;
			char *dictionary = new char [64*1024];
			unsigned int length = sizeof (Buffer2) - sizeof (BlockHeader);
			lzo1x_1_compress ((unsigned char*)Buffer, BlockHeader.UncompCount, (unsigned char*)(&Buffer2[sizeof(BlockHeader)]), &length, dictionary);
			BlockHeader.CompCount = (unsigned short)length;
			delete [] dictionary;
			memmove(Buffer2, &BlockHeader, sizeof(BlockHeader));
			Counter = BlockHeader.CompCount+sizeof(BlockHeader);
		}
	}

	return(total);
}

// FUNCTION: 0x0055C990 LZOStraw::~LZOStraw

struct DAT_00ABCD68_Type {
	int A, B, C, D;
	DAT_00ABCD68_Type() : A(0), B(0), C(0), D(0) {}
};

// FUNCTION: 0x0055C9E0 $E2
DAT_00ABCD68_Type DAT_00ABCD68;
