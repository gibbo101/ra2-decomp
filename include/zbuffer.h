// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "rect.h"

class Surface;

#define ZBUFFER_MAX    0x8000

class ZBuffer
{
		friend class IsometricTileTypeClass;
	public:

		ZBuffer(Rect rect);
		~ZBuffer(void) { Release_Surface(); }

		unsigned short Get_Scroll(void) const { return(ScrollOffset); }
		void Set_Scroll(int position) { ScrollOffset = position; }
		int Get_Scroll_Delta(int position) const { return(ScrollOffset - position); }

		void Copy_To(Surface * surface, Rect rect);

		void Set(unsigned int dst, int size, unsigned short value);

		void Pan(int x_delta, int y_delta, unsigned short value);

		bool Fill(unsigned short value);
		bool Fill(unsigned short value, Rect rect);

		void Update(Rect rect);

		unsigned int Get_Buffer_Offset(Point2D position);

		unsigned int Wrap_Overflow(unsigned int position) const;
		unsigned int Wrap_Underflow(unsigned int position) const;

		Surface * Get_Surface(void) const { return(SurfacePtr); }

		Rect const & Get_Bounds(void) const { return(Bounds); }
		unsigned int Get_Buffer_Width(void) const { return(BufferWidth); }
		unsigned int Get_Buffer_End(void) const { return(BufferEnd); }
		int Get_Buffer_Height(void) const { return(BufferHeight); }

	private:
		void Release_Surface(void);

	public:

		Rect Bounds;

	private:

		int SurfaceOffset;

		Surface *SurfacePtr;

		unsigned int BufferStart;
		unsigned int BufferEnd;
		unsigned int BufferSize;

		unsigned int ScrollOffset;

		int BufferWidth;
		int BufferHeight;
};

inline unsigned int ZBuffer::Wrap_Overflow(unsigned int position) const
{
	if (position >= BufferEnd) {
		position -= BufferSize;
	}
	return(position);
}

inline unsigned int ZBuffer::Wrap_Underflow(unsigned int position) const
{
	if (position < BufferStart) {
		position += BufferSize;
	}
	return(position);
}

extern ZBuffer *DepthBuffer;

inline unsigned short *Blit_Wrap_Z_Buffer(unsigned short *buf)
{
	return((unsigned short *)DepthBuffer->Wrap_Overflow((unsigned int)buf));
}

