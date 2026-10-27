// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "srfcache.h"
#include "ccfile.h"
#include "dsurface.h"
#include "ownrdraw.h"
#include "pcx.h"
#include <new>

// FUNCTION: GAMEMD 0x006B9370
int SurfaceCacheConvertPixel(int red, int green, int blue)
{
	int boosted = (green * 15) / 10;
	green = MIN(255, boosted);
	return(DSurface::Build_Hicolor_Pixel(green, red, blue / 4));
}

// FUNCTION: GAMEMD 0x006B93E0
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

static unsigned int SurfaceCache_Wstring_Hash(Wstring & string);

// FUNCTION: GAMEMD 0x006B9450
SurfaceCacheClass::SurfaceCacheClass(void) :
	Dictionary<Wstring, SurfaceCacheEntry>(SurfaceCache_Wstring_Hash)
{

}

// FUNCTION: GAMEMD 0x006B94E0
static unsigned int SurfaceCache_Wstring_Hash(Wstring & string)
{
	unsigned int hash = 0;

	hash = string.length();

	for (unsigned int i = 0; i < string.length(); i++) {
		hash += *(string.get() + i);
		hash += i;
		hash = (hash << 8) ^ (hash >> 24);
	}
	return(hash);
}

// FUNCTION: GAMEMD 0x006B9530
SurfaceCacheClass::~SurfaceCacheClass(void)
{
	Wstring key;
	SurfaceCacheEntry value;
	while (removeAny(key, value)) {
		delete value.surf;
	}
}

// FUNCTION: GAMEMD 0x006BA120
bool SurfaceCacheClass::CachePalettedPCX(char const *name)
{
	return(CachePCX(name, 1, true));
}

// FUNCTION: GAMEMD 0x006BA140
Surface * SurfaceCacheClass::GetSurface(char const * name, void * palette)
{
	Wstring n(name);
	static SurfaceCacheEntry _x;

	_x.surf = NULL;

	SurfaceCacheEntry *p = &_x;
	if (getPointer(n, &p)) {
		if (p != NULL) {
			_x = *p;
		}
		if (palette != NULL) {
			memcpy(palette, _x.palette, sizeof(_x.palette));
		}
		return(_x.surf);
	}
	return(NULL);
}

// SYNTHETIC: GAMEMD 0x006BA220 SYMBOL
// _$E27

// FUNCTION: GAMEMD 0x006BA230
void SurfaceCacheClass::Remove(char const *name)
{
	Wstring key(name);
	SurfaceCacheEntry entry;
	if (getValue(key, entry)) {
		remove(key);
		delete entry.surf;
	}
	getEntries();
}

// FUNCTION: GAMEMD 0x006BA680
bool SurfaceCacheClass::DrawNullsub(Rect const & rect, Surface & tosurface, Surface & fromsurface)
{
	return(false);
}

// TEMPLATE: GAMEMD 0x006BAC40
// ?remove@?$Dictionary@VWstring@@USurfaceCacheEntry@@@@QAEDAAVWstring@@AAUSurfaceCacheEntry@@@Z
template class Dictionary<Wstring, SurfaceCacheEntry>;

// TEMPLATE: GAMEMD 0x006BAEF0
// ?shrink@?$Dictionary@VWstring@@USurfaceCacheEntry@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x006BAFA0
// ?expand@?$Dictionary@VWstring@@USurfaceCacheEntry@@@@AAEXXZ
