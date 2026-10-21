// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "rect.h"

class Surface;

struct SurfaceRegion {

	Point2D Point;

	Rect Bounds;
};

class StaticBufferClass
{
	public:
		StaticBufferClass(int size);
		~StaticBufferClass(void);

		struct Entry {

			short X;
			short Y;

			unsigned short Width;
			unsigned short Height;

			unsigned char * Data;
		};

		StaticBufferClass::Entry * Add(Surface & surface, SurfaceRegion const & region);
		StaticBufferClass::Entry * Add(Surface & surface, Rect const & cliprect, short x, short y);

		unsigned char * Reserve(int size)
		{
			unsigned char * new_cursor = Cursor + size;
			if (new_cursor <= Buffer + Size) {
				unsigned char * old_cursor = Cursor;
				Cursor = new_cursor;
				return(old_cursor);
			}
			return(NULL);
		}

		void Reset(void) { Cursor = Buffer; }

	private:

		unsigned int Size;

		unsigned char * Buffer;

		unsigned char * Cursor;
};
