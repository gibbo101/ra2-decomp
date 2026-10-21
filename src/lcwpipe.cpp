// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "lcwpipe.h"
#include "assert.h"
#include "lcw.h"
#include <cstring>

// FUNCTION: GAMEMD 0x00551FF0
LCWPipe::LCWPipe(CompControl control, int blocksize) :
		Control(control),
		Counter(0),
		Buffer(NULL),
		Buffer2(NULL),
		BlockSize(blocksize)
{
	int margin = BlockSize/128+1;
	SafetyMargin = margin < 128 ? 128 : margin;
	Buffer = new char[BlockSize+SafetyMargin];
	Buffer2 = new char[BlockSize+SafetyMargin];
	BlockHeader.CompCount = 0xFFFF;
}

// FUNCTION: GAMEMD 0x00552060
LCWPipe::~LCWPipe(void)
{
	delete [] Buffer;
	Buffer = NULL;

	delete [] Buffer2;
	Buffer2 = NULL;
}

// FUNCTION: GAMEMD 0x005520A0
int LCWPipe::Put(void const * source, int slen)
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
					LCW_Uncomp(Buffer, Buffer2);
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
				int len = LCW_Comp(Buffer, Buffer2, BlockSize);

				BlockHeader.CompCount = (unsigned short)len;
				BlockHeader.UncompCount = (unsigned short)BlockSize;
				total += BASECLASS::Put(&BlockHeader, sizeof(BlockHeader));
				total += BASECLASS::Put(Buffer2, len);
				Counter = 0;
			}
		}

		while (slen >= BlockSize) {
			int len = LCW_Comp(source, Buffer2, BlockSize);

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

// FUNCTION: GAMEMD 0x005522D0
int LCWPipe::Flush(void)
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

			int len = LCW_Comp(Buffer, Buffer2, Counter);

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

// SYNTHETIC: GAMEMD 0x00552390
// ??_GLCWPipe@@UAEPAXI@Z
