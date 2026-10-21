// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "lzopipe.h"
#include "assert.h"
#include "lzo.h"
#include <cstring>

// FUNCTION: GAMEMD 0x0055C2B0
LZOPipe::LZOPipe(CompControl control, int blocksize) :
		Control(control),
		Counter(0),
		Buffer(NULL),
		Buffer2(NULL),
		BlockSize(blocksize)
{
	SafetyMargin = BlockSize;
	Buffer = new char[BlockSize+SafetyMargin];
	Buffer2 = new char[BlockSize+SafetyMargin];
	BlockHeader.CompCount = 0xFFFF;
}

// FUNCTION: GAMEMD 0x0055C310
LZOPipe::~LZOPipe(void)
{
	delete [] Buffer;
	Buffer = NULL;

	delete [] Buffer2;
	Buffer2 = NULL;
}

// FUNCTION: GAMEMD 0x0055C350
int LZOPipe::Put(void const * source, int slen)
{
	if (source == NULL || slen < 1) {
		return(BASECLASS::Put(source, slen));
	}
	int total = 0;
	if (Control ==  DECOMPRESS) {
		while (slen > 0) {
			if (BlockHeader.CompCount == 0xFFFF) {
				int len = ((unsigned)slen < (sizeof(BlockHeader)-Counter)) ? slen : (sizeof(BlockHeader)-Counter);
				memmove(&Buffer[Counter], source, len);
				source = ((char *)source) + len;
				slen -= len;
				Counter += len;
				if (Counter == sizeof(BlockHeader)) {
					memmove(&BlockHeader, Buffer, sizeof(BlockHeader));
					Counter = 0;
				}
			}
			if (slen > 0) {
				int len = (slen < (BlockHeader.CompCount-Counter)) ? slen : (BlockHeader.CompCount-Counter);
				memmove(&Buffer[Counter], source, len);
				slen -= len;
				source = ((char *)source) + len;
				Counter += len;
				if (Counter == BlockHeader.CompCount) {
					unsigned int length = sizeof (Buffer2);
					lzo1x_decompress ((unsigned char*)Buffer, BlockHeader.CompCount, (unsigned char*)Buffer2, &length, NULL);
					total += BASECLASS::Put(Buffer2, BlockHeader.UncompCount);
					Counter = 0;
					BlockHeader.CompCount = 0xFFFF;
				}
			}
		}
	} else {
		if (Counter > 0) {
			int tocopy = (slen < (BlockSize-Counter)) ? slen : (BlockSize-Counter);
			memmove(&Buffer[Counter], source, tocopy);
			source = ((char *)source) + tocopy;
			slen -= tocopy;
			Counter += tocopy;
			if (Counter == BlockSize) {
				unsigned int len = sizeof (Buffer2);
				char *dictionary = new char [64*1024];
				lzo1x_1_compress ((unsigned char*)Buffer, BlockSize, (unsigned char*)Buffer2, &len, dictionary);
				delete [] dictionary;
				BlockHeader.CompCount = (unsigned short)len;
				BlockHeader.UncompCount = (unsigned short)BlockSize;
				total += BASECLASS::Put(&BlockHeader, sizeof(BlockHeader));
				total += BASECLASS::Put(Buffer2, len);
				Counter = 0;
			}
		}
		while (slen >= BlockSize) {
			unsigned int len = 0;
			char *dictionary = new char [64*1024];
			lzo1x_1_compress ((unsigned char*)source, BlockSize, (unsigned char*)Buffer2, &len, dictionary);
			delete [] dictionary;
			source = ((char *)source) + BlockSize;
			slen -= BlockSize;
			BlockHeader.CompCount = (unsigned short)len;
			BlockHeader.UncompCount = (unsigned short)BlockSize;
			total += BASECLASS::Put(&BlockHeader, sizeof(BlockHeader));
			total += BASECLASS::Put(Buffer2, len);
		}
		if (slen > 0) {
			memmove(Buffer, source, slen);
			Counter = slen;
		}
	}
	return(total);
}
// FUNCTION: GAMEMD 0x0055C5E0
int LZOPipe::Flush(void)
{
	int total = 0;
	if (Counter > 0) {
		if (Control == DECOMPRESS) {
			if (BlockHeader.CompCount == 0xFFFF) {
				total += BASECLASS::Put(Buffer, Counter);
				Counter = 0;
			}
			if (Counter > 0) {
				total += BASECLASS::Put(&BlockHeader, sizeof(BlockHeader));
				total += BASECLASS::Put(Buffer, Counter);
				Counter = 0;
				BlockHeader.CompCount = 0xFFFF;
			}
		} else {
			unsigned int len = 0;
			char *dictionary = new char [64*1024];
			lzo1x_1_compress ((unsigned char*)Buffer, Counter, (unsigned char *)Buffer2, &len, dictionary);
			delete [] dictionary;
			BlockHeader.CompCount = (unsigned short)len;
			BlockHeader.UncompCount = (unsigned short)Counter;
			total += BASECLASS::Put(&BlockHeader, sizeof(BlockHeader));
			total += BASECLASS::Put(Buffer2, len);
			Counter = 0;
		}
	}
	total += BASECLASS::Flush();
	return(total);
}

// SYNTHETIC: GAMEMD 0x0055C6D0
// ??_GLZOPipe@@UAEPAXI@Z
