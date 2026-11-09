// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "rect.h"
#include "dsurface.h"
#include "srfcache.h"
#include "ccfile.h"
#include "ownrdraw.h"
#include "pcx.h"
#include <new>

// SYNTHETIC: GAMEMD 0x006B9350 SYMBOL
// _$E2

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

template<> char Dictionary<Wstring, SurfaceCacheEntry>::remove(Wstring &key)
{
	SurfaceCacheEntry value;
	return(remove(key, value));
}

// FUNCTION: GAMEMD 0x006B9D00
bool SurfaceCacheClass::CachePCX(char const *name, int bpp, int red_channel)
{
	int i;
	RGBClass pal[256];
	SurfaceCacheEntry new_entry;
	CCFileClass file(name);
	Surface *pcx = Read_PCX_File(file, (PaletteClass *)&pal);
	Surface *surf = pcx;
	if (pcx == NULL) {
		return(false);
	}
	if (bpp == 2) {
		unsigned short pal16[256];
		for (i = 0; i < 256; i++) {
			RGBClass &rgb = pal[i];
			pal16[i] = ((rgb.Get_Red() >> DSurface::RedLeft) << DSurface::RedRight)
				| ((rgb.Get_Green() >> DSurface::GreenLeft) << DSurface::GreenRight)
				| ((rgb.Get_Blue() >> DSurface::BlueLeft) << DSurface::BlueRight);
		}
		Rect rect = pcx->Get_Rect();
		surf = new BSurface(rect.Width, rect.Height, 2);
		unsigned short *s = (unsigned short *)surf->Lock();
		unsigned char *p = (unsigned char *)pcx->Lock();
		for (i = 0; i < rect.Width * rect.Height; i++) {
			s[i] = pal16[p[i]];
		}
		pcx->Unlock();
		surf->Unlock();
		delete pcx;
	} else if (bpp == 1) {
		memcpy(new_entry.palette, pal, sizeof(pal));
	}
	if (red_channel && bpp == 1) {
		Rect rect = surf->Get_Rect();
		unsigned char *s = (unsigned char *)surf->Lock();
		for (i = 0; i < rect.Width * rect.Height; i++) {
			s[i] = pal[s[i]].Get_Red();
		}
		surf->Unlock();
	}
	new_entry.surf = surf;
	Wstring key(name);
	key.toLower();
	SurfaceCacheEntry old_entry;
	SurfaceCacheEntry *old_pointer = NULL;
	char found = getPointer(key, &old_pointer);
	if (found && old_pointer) {
		old_entry = *old_pointer;
	}
	if (found) {
		remove(key);
		delete old_entry.surf;
	}
	add(key, new_entry);
	return(true);
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
		Surface * surf = entry.surf;
		remove(key, entry);
		delete surf;
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
