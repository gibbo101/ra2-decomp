// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "abuffer.h"
#include "assert.h"
#include "bsurface.h"
#include <cstdlib>
#define ABUFFER_COLOR  0x007F
#define ABUFFER_MAX    0x8000
#define ABUFFER_BPP    2

// FUNCTION: 0x00410E50
void ABuffer::Release_Surface(void)
{
	delete SurfacePtr;
	SurfacePtr = NULL;
}

// FUNCTION: 0x00410E70
void ABuffer::Set(unsigned int dst, int size, unsigned short value)
{
	if ((unsigned int)dst & 2) {
		if (size != 0) {
			*(unsigned short *)dst = value;
			dst = (unsigned int)((unsigned short *)dst + 1);
			size--;
		}
	}

	int size_half = size >> 1;
	unsigned int *dst_ptr = NULL;

	if (size_half) {
		int val = (value * (2 * ABUFFER_MAX)) + value;
		dst_ptr = (unsigned int *)dst;
		while (size_half) {
			*dst_ptr = val;
			dst_ptr++;
			size_half--;
		}
	}

	if (size & 1) {
		if (dst_ptr != NULL) {
			*(unsigned short *)dst_ptr = value;
		}
	}
}

// FUNCTION: 0x004112D0
bool ABuffer::Fill(unsigned short value)
{
	return(SurfacePtr->Fill_Rect(Rect(0, 0, BufferWidth, BufferHeight), value));
}

// FUNCTION: 0x00411310
bool ABuffer::Fill(unsigned short value, Rect rect)
{
	return(SurfacePtr->Fill_Rect(rect, value));
}
