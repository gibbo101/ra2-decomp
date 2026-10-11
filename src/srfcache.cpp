// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "srfcache.h"
#include "ccfile.h"
#include "dsurface.h"
#include "ownrdraw.h"
#include "pcx.h"
#include <new>
#pragma pack(push, 2)

// FUNCTION: 0x006B9370
int SurfaceCacheConvertPixel(int red, int green, int blue)
{
	int boosted = (green * 15) / 10;
	green = MIN(255, boosted);
	return(DSurface::Build_Hicolor_Pixel(green, red, blue / 4));
}

// FUNCTION: 0x006B93E0
void SurfaceCacheConvertPalette(unsigned char *pal)
{
	unsigned char *p = pal;
	for (int i = 0; i < 256; i++) {
		int red = p[0];
		int green = p[1];
		int blue = p[2];

		p[0] = MIN(255, ((green * 15) / 10));
		p[1] = red;
		p[2] = blue / 4;
		p += 3;
	}
}

// FUNCTION: 0x006BA120
bool SurfaceCacheClass::CachePalettedPCX(char const *name)
{
	return(CachePCX(name, 1, true));
}
