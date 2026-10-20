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

// FUNCTION: 0x00410CE0
ABuffer::ABuffer(Rect rect) :
	BufferWidth(rect.Width),
	BufferHeight(rect.Height)
{
	Bounds = rect;
	BufferSize = BufferWidth * BufferHeight * ABUFFER_BPP;
	SurfacePtr = new BSurface(BufferWidth, BufferHeight, ABUFFER_BPP);

	Fill(ABUFFER_COLOR);

	BufferStart = (unsigned int)(SurfacePtr->Lock());
	SurfaceOffset = 0;
	BufferEnd = BufferStart + BufferWidth * BufferHeight * ABUFFER_BPP;
	ScrollOffset = ABUFFER_MAX;

	SurfacePtr->Unlock();
}

// FUNCTION: 0x00410DC0
void ABuffer::Copy_To(Surface *surface, Rect rect)
{
	unsigned short *surfbuffptr = (unsigned short *)(surface->Lock(Point2D(rect.X, rect.Y)));

	unsigned short *pixptr = (unsigned short *)(BufferStart + SurfaceOffset);

	int steps = (surface->Stride() / ABUFFER_BPP) - rect.Width;

	if (surfbuffptr != NULL) {
		for (int i = 0; i < rect.Height; ++i) {
			for (int j = 0; j < rect.Width; ++j) {
				*surfbuffptr = *pixptr;
				++surfbuffptr;
				pixptr = (unsigned short *)((unsigned char *)pixptr + ABUFFER_BPP);
				pixptr = (unsigned short *)Wrap_Overflow((unsigned int)pixptr);
			}
			surfbuffptr += steps;
		}
	}
	surface->Unlock();
}

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

// FUNCTION: 0x00410ED0
void ABuffer::Pan(int x, int y, unsigned short value)
{
	int target_col;
	int target_row;
	int x_delta = x;
	int y_delta = y;

	int current_col = (SurfaceOffset / ABUFFER_BPP) % BufferWidth;

	if (abs(x_delta) > BufferWidth || abs(y_delta) > BufferHeight) {
		Fill(value);
		ScrollOffset = ABUFFER_MAX;

	} else {
		target_col = current_col + x_delta;

		if (x_delta != 0) {

			SurfaceOffset += x_delta * ABUFFER_BPP;

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

		int current_row = (SurfaceOffset / ABUFFER_BPP) / BufferWidth;

		if (y_delta != 0) {
			target_row = current_row + y_delta;

			ScrollOffset -= y_delta;

			int prev_offset = SurfaceOffset;

			SurfaceOffset += y_delta * BufferWidth * ABUFFER_BPP;

			unsigned int new_offset = Wrap_Underflow(SurfaceOffset + BufferStart);
			new_offset = Wrap_Overflow(new_offset);
			SurfaceOffset = new_offset - BufferStart;

			if (y_delta < 0) {
				if (target_row < 0) {
					Set(BufferStart, prev_offset / ABUFFER_BPP, value);
					Set(BufferStart + SurfaceOffset, ((BufferWidth * BufferHeight * ABUFFER_BPP) - SurfaceOffset) / ABUFFER_BPP, value);
					return;

				} else {
					Set(BufferStart + SurfaceOffset, (prev_offset - SurfaceOffset) / ABUFFER_BPP, value);
				}

			} else if (target_row >= BufferHeight) {
				Set(BufferStart + prev_offset, ((BufferWidth * BufferHeight * ABUFFER_BPP) - prev_offset) / ABUFFER_BPP, value);
				Set(BufferStart, SurfaceOffset / ABUFFER_BPP, value);
				return;

			} else {
				Set(BufferStart + prev_offset, (SurfaceOffset - prev_offset) / ABUFFER_BPP, value);
			}
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

// FUNCTION: 0x00411330
void ABuffer::Update(Rect rect)
{
	unsigned int buffptr = Get_Buffer_Offset(Point2D(rect.X, rect.Y));

	for (int i = 0; i < rect.Height; ++i) {

		unsigned int size;
		if ((buffptr + rect.Width * ABUFFER_BPP) >= BufferEnd) {
			size = (BufferEnd - buffptr) / ABUFFER_BPP;
			Set(buffptr, size, ABUFFER_COLOR);
			Set(BufferStart, rect.Width - size, ABUFFER_COLOR);
		} else {
			Set(buffptr, rect.Width, ABUFFER_COLOR);
		}

		buffptr += BufferWidth * ABUFFER_BPP;
		buffptr = Wrap_Overflow(buffptr);
	}
}

// FUNCTION: 0x004114B0
unsigned int ABuffer::Get_Buffer_Offset(Point2D pos)
{
	unsigned int buffptr = (unsigned int)SurfacePtr->Lock(pos);

	SurfacePtr->Unlock();

	buffptr += SurfaceOffset;
	return(Wrap_Overflow(buffptr));
}

// FUNCTION: 0x004114F0 Surface::Can_Lock
// FUNCTION: 0x00411500 Surface::entry_64
// FUNCTION: 0x00411510 Surface::Get_Rect
// FUNCTION: 0x00411540 Surface::Get_Width
// FUNCTION: 0x00411550 Surface::Get_Height
// FUNCTION: 0x00411560 XSurface::Lock
// FUNCTION: 0x00411570 XSurface::Unlock
