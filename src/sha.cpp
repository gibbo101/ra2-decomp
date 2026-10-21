// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "sha.h"
#include <cstdlib>
#if !defined(MIN)
#define	MIN(a, b)		((a)<(b))?(a):(b)
#endif

#define	MIN(a, b)		((a)<(b))?(a):(b)
#define	Reverse_LONG(a)	((a>>24)&0x000000FFL) | ((a>>8)&0x0000FF00L) | ((a<<8)&0x00FF0000L) | ((a<<24)&0xFF000000L)

// FUNCTION: GAMEMD 0x0069D8C0
void SHAEngine::Process_Partial(void const * & data, int & length)
{
	if (length == 0 || data == NULL) return;

	if (PartialCount == 0 && length >= SRC_BLOCK_SIZE) return;

	int add_count = MIN((int)length, SRC_BLOCK_SIZE - PartialCount);
	memcpy(&Partial[PartialCount], data, add_count);
	data = ((char const *&)data) + add_count;
	PartialCount += add_count;
	length -= add_count;

	if (PartialCount == SRC_BLOCK_SIZE) {
		Process_Block(&Partial[0], Acc);
		Length += (int)SRC_BLOCK_SIZE;
		PartialCount = 0;
	}
}

// FUNCTION: GAMEMD 0x0069D960
void SHAEngine::Hash(void const * data, int length)
{
	IsCached = false;

	Process_Partial(data, length);

	if (length == 0) return;

	int blocks = (length / SRC_BLOCK_SIZE);
	int const * source = (int const *)data;
	for (int bcount = 0; bcount < blocks; bcount++) {
		Process_Block(source, Acc);
		Length += (int)SRC_BLOCK_SIZE;
		source += SRC_BLOCK_SIZE/sizeof(int);
		length -= (int)SRC_BLOCK_SIZE;
	}

	data = source;
	Process_Partial(data, length);
}

// FUNCTION: GAMEMD 0x0069D9E0
int SHAEngine::Result(void * result) const
{
	if (IsCached) {
		memcpy(result, &FinalResult, sizeof(FinalResult));
	}

	int length = Length + PartialCount;
	int partialcount = PartialCount;
	unsigned char partial[SRC_BLOCK_SIZE];
	memcpy(partial, Partial, sizeof(Partial));

	partial[partialcount] = (unsigned char)0x80;

	SHADigest acc = Acc;
	if ((SRC_BLOCK_SIZE - partialcount) < 9) {
		if (partialcount+1 < SRC_BLOCK_SIZE) {
			memset(&partial[partialcount+1], '\0', SRC_BLOCK_SIZE - (partialcount+1));
		}
		Process_Block(&partial[0], acc);
		partialcount = 0;
	} else {
		partialcount++;
	}

	memset(&partial[partialcount], '\0', SRC_BLOCK_SIZE - partialcount);
	*(int *)(&partial[SRC_BLOCK_SIZE-4]) = Reverse_LONG((length*8));
	Process_Block(&partial[0], acc);

	memcpy((char *)&FinalResult, &acc, sizeof(acc));
	for (int index = 0; index < sizeof(FinalResult)/sizeof(int); index++) {
		(int &)FinalResult.Long[index] = Reverse_LONG(FinalResult.Long[index]);
	}
	(bool&)IsCached = true;
	memcpy(result, &FinalResult, sizeof(FinalResult));
	return(sizeof(FinalResult));
}
