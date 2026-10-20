// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "dict.h"
#include "surface.h"
#include "wstring.h"

struct SurfaceCacheEntry
{

	Surface * surf;

	char palette[768];

	SurfaceCacheEntry(void) {
		surf = NULL;
	}
};

class SurfaceCacheClass : public Dictionary<Wstring, SurfaceCacheEntry>
{
	public:
		SurfaceCacheClass(void);
		~SurfaceCacheClass(void);

		void Remove(char const *name);
		bool CacheBMP(char const *name, void *bitmap, int bytes, int bpp=2);

		bool CachePCX(char const *name, int bpp=2, int red_channel=false);
		bool CachePalettedPCX(char const *name);

		Surface * GetSurface(char const * name, void * palette = NULL);

		bool Draw(Rect const & rect, Surface & tosurface, Surface & fromsurface, int x = 0, int y = 0);
		bool DrawTrans(Rect const & rect, Surface & tosurface, Surface & fromsurface, short trans);
		bool DrawNullsub(Rect const & rect, Surface & tosurface, Surface & fromsurface);
		bool DrawMasked(Rect const & rect, Surface & tosurface, Surface & fromsurface, Surface & masksurface, void * palette, int center, int x_offset, int y_offset);
};

int SurfaceCacheConvertPixel(int red, int green, int blue);
void SurfaceCacheConvertPalette(unsigned char *pal);

extern SurfaceCacheClass SurfaceCache;
