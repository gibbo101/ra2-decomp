// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "zbuffer.h"
#include "assert.h"
#include "bsurface.h"
#include <cstdlib>
#define ZBUFFER_COLOR  0xFFFF
#define ZBUFFER_BPP    2

// FUNCTION: 0x007BCAE0
void ZBuffer::Release_Surface(void)
{
	delete SurfacePtr;
}

// FUNCTION: 0x007BCAF0
void ZBuffer::Set(unsigned int dst, int size, unsigned short value)
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
		int val = (value * (2 * ZBUFFER_MAX)) + value;
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

// FUNCTION: 0x007BCF50
bool ZBuffer::Fill(unsigned short value)
{
	return(SurfacePtr->Fill_Rect(Rect(0, 0, BufferWidth, BufferHeight), value));
}

// FUNCTION: 0x007BCF90
bool ZBuffer::Fill(unsigned short value, Rect rect)
{
	return(SurfacePtr->Fill_Rect(rect, value));
}
