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

// FUNCTION: GAMEMD 0x007BC970
ZBuffer::ZBuffer(Rect rect) :
	BufferWidth(rect.Width),
	BufferHeight(rect.Height)
{
	Bounds = rect;
	BufferSize = BufferWidth * BufferHeight * ZBUFFER_BPP;
	SurfacePtr = new BSurface(BufferWidth, BufferHeight, ZBUFFER_BPP);

	Fill(ZBUFFER_COLOR);

	BufferStart = (unsigned int)(SurfacePtr->Lock());
	SurfaceOffset = 0;
	BufferEnd = BufferStart + BufferWidth * BufferHeight * ZBUFFER_BPP;
	ScrollOffset = ZBUFFER_MAX;

	SurfacePtr->Unlock();
}

// FUNCTION: GAMEMD 0x007BCA50
void ZBuffer::Copy_To(Surface *surface, Rect rect)
{

	unsigned short *surfbuffptr = (unsigned short *)(surface->Lock(Point2D(rect.X, rect.Y)));

	unsigned short *pixptr = (unsigned short *)(BufferStart + SurfaceOffset);

	int steps = (surface->Stride() / ZBUFFER_BPP) - rect.Width;

	if (surfbuffptr != NULL) {
		for (int i = 0; i < rect.Height; ++i) {
			for (int j = 0; j < rect.Width; ++j) {
				*surfbuffptr = *pixptr;
				++surfbuffptr;
				pixptr = (unsigned short *)((unsigned char *)pixptr + ZBUFFER_BPP);
				pixptr = (unsigned short *)Wrap_Overflow((unsigned int)pixptr);
			}
			surfbuffptr += steps;
		}
	}
	surface->Unlock();
}

// FUNCTION: GAMEMD 0x007BCAE0
void ZBuffer::Release_Surface(void)
{
	delete SurfacePtr;
}

// FUNCTION: GAMEMD 0x007BCAF0
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

// FUNCTION: GAMEMD 0x007BCB50
void ZBuffer::Pan(int x, int y, unsigned short value)
{
	int target_col;
	int target_row;
	int x_delta = x;
	int y_delta = y;

	int current_col = (SurfaceOffset / ZBUFFER_BPP) % BufferWidth;

	if (abs(x_delta) > BufferWidth || abs(y_delta) > BufferHeight) {
		Fill(value);
		ScrollOffset = ZBUFFER_MAX;

	} else {
		target_col = current_col + x_delta;

		if (x_delta != 0) {

			SurfaceOffset += x_delta * ZBUFFER_BPP;

			unsigned int new_offset = Wrap_Underflow(SurfaceOffset + BufferStart);
			new_offset = Wrap_Overflow(new_offset);
			SurfaceOffset = new_offset - BufferStart;

			if (x_delta < 0) {
				if (target_col < 0) {
					Fill(value, Rect(0, 0, current_col, BufferHeight));
					target_col += BufferWidth;
					Fill(value, Rect(target_col, 0, BufferWidth - target_col, BufferHeight));

				} else {
					Fill(value, Rect(target_col, 0, -x_delta, BufferHeight));
				}

			} else if (target_col >= BufferWidth) {
				Fill(value, Rect(current_col, 0, BufferWidth - current_col, BufferHeight));
				target_col -= BufferWidth;
				Fill(value, Rect(0, 0, target_col, BufferHeight));

			} else {
				Fill(value, Rect(current_col, 0, x_delta, BufferHeight));
			}
		}

		int current_row = (SurfaceOffset / ZBUFFER_BPP) / BufferWidth;

		if (y_delta != 0) {
			target_row = current_row + y_delta;

			ScrollOffset -= y_delta;

			int prev_offset = SurfaceOffset;

			SurfaceOffset += y_delta * BufferWidth * ZBUFFER_BPP;

			unsigned int new_offset = Wrap_Underflow(SurfaceOffset + BufferStart);
			new_offset = Wrap_Overflow(new_offset);
			SurfaceOffset = new_offset - BufferStart;

			if (y_delta < 0) {
				if (target_row < 0) {
					Set(BufferStart, prev_offset / ZBUFFER_BPP, value);
					Set(BufferStart + SurfaceOffset, ((BufferWidth * BufferHeight * ZBUFFER_BPP) - SurfaceOffset) / ZBUFFER_BPP, value);
					return;
				}
				Set(BufferStart + SurfaceOffset, (prev_offset - SurfaceOffset) / ZBUFFER_BPP, value);
				return;
			}
			if (target_row >= BufferHeight) {
				Set(BufferStart + prev_offset, ((BufferWidth * BufferHeight * ZBUFFER_BPP) - prev_offset) / ZBUFFER_BPP, value);
				Set(BufferStart, SurfaceOffset / ZBUFFER_BPP, value);
				return;
			}
			Set(BufferStart + prev_offset, (SurfaceOffset - prev_offset) / ZBUFFER_BPP, value);
		}
	}
}


// FUNCTION: GAMEMD 0x007BCF50
bool ZBuffer::Fill(unsigned short value)
{
	return(SurfacePtr->Fill_Rect(Rect(0, 0, BufferWidth, BufferHeight), value));
}

// FUNCTION: GAMEMD 0x007BCF90
bool ZBuffer::Fill(unsigned short value, Rect rect)
{
	return(SurfacePtr->Fill_Rect(rect, value));
}

// FUNCTION: GAMEMD 0x007BCFB0
void ZBuffer::Update(Rect rect)
{
	unsigned int buffptr = Get_Buffer_Offset(Point2D(rect.X, rect.Y));

	for (int i = 0; i < rect.Height; ++i) {

		unsigned int size;
		if ((buffptr + rect.Width * ZBUFFER_BPP) >= BufferEnd) {
			size = (BufferEnd - buffptr) / ZBUFFER_BPP;
			Set(buffptr, size, ZBUFFER_COLOR);
			Set(BufferStart, rect.Width - size, ZBUFFER_COLOR);
		} else {
			Set(buffptr, rect.Width, ZBUFFER_COLOR);
		}

		buffptr += BufferWidth * ZBUFFER_BPP;
		buffptr = Wrap_Overflow(buffptr);
	}
}

// FUNCTION: GAMEMD 0x007BD130
unsigned int ZBuffer::Get_Buffer_Offset(Point2D pos)
{
	unsigned int buffptr = (unsigned int)SurfacePtr->Lock(pos);

	SurfacePtr->Unlock();

	buffptr += SurfaceOffset;
	return(Wrap_Overflow(buffptr));
}
