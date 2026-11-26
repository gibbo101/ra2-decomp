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

// FUNCTION: GAMEMD 0x006B9740
bool SurfaceCacheClass::CacheBMP(char const * name, void * bitmap, int bytes, int bpp)
{
	unsigned char *bitmap_palette_data;
	unsigned char *bitmap_data;
	BITMAPFILEHEADER filehead;
	BITMAPINFO *bitmap_info_header;
	int bitmap_width;
	int surface_width;
	BSurface *surface;
	unsigned short *surface_data;
	BITMAPINFOHEADER header;
	SurfaceCacheEntry new_entry;

	bitmap_palette_data = (unsigned char *)bitmap;
	filehead = *(BITMAPFILEHEADER *)bitmap_palette_data;
	bitmap_palette_data += sizeof(BITMAPFILEHEADER);
	header = *(BITMAPINFOHEADER *)bitmap_palette_data;
	bitmap_palette_data += sizeof(BITMAPINFOHEADER);

	bitmap_info_header = (BITMAPINFO *)operator new(4 * (1 << header.biBitCount) + sizeof(BITMAPINFOHEADER));
	bitmap_info_header->bmiHeader.biSize = header.biSize;
	bitmap_info_header->bmiHeader.biWidth = header.biWidth;
	bitmap_info_header->bmiHeader.biHeight = header.biHeight;
	bitmap_info_header->bmiHeader.biPlanes = header.biPlanes;
	bitmap_info_header->bmiHeader.biBitCount = header.biBitCount;
	bitmap_info_header->bmiHeader.biCompression = header.biCompression;
	bitmap_info_header->bmiHeader.biSizeImage = header.biSizeImage;
	bitmap_info_header->bmiHeader.biXPelsPerMeter = header.biXPelsPerMeter;
	bitmap_info_header->bmiHeader.biYPelsPerMeter = header.biYPelsPerMeter;
	bitmap_info_header->bmiHeader.biClrUsed = header.biClrUsed;
	bitmap_info_header->bmiHeader.biClrImportant = header.biClrImportant;
	memcpy(bitmap_info_header->bmiColors, bitmap_palette_data, header.biClrUsed * sizeof(RGBQUAD));
	bitmap_palette_data += header.biClrUsed * sizeof(RGBQUAD);

	unsigned short pal16[256];
	unsigned char *p = (unsigned char *)bitmap_info_header->bmiColors;
	for (int i = 0; i < 256; i++) {
		int blue = *p++;
		int green = *p++;
		pal16[i] = ((*p >> DSurface::RedLeft) << DSurface::RedRight) | ((green >> DSurface::GreenLeft) << DSurface::GreenRight) | ((blue >> DSurface::BlueLeft) << DSurface::BlueRight);
		p += 2;
	}

	bitmap_data = (unsigned char *)operator new(filehead.bfSize - filehead.bfOffBits);
	memcpy(bitmap_data, bitmap_palette_data, filehead.bfSize - filehead.bfOffBits);

	int width = header.biWidth;
	int height = header.biHeight;
	surface = new BSurface(width, height, bpp);
	surface_data = (unsigned short *)surface->Lock();
	surface_width = surface->Stride() / 2;
	bitmap_width = width - (width & 3) + 4;

	for (int y = 0; y < height; y++) {
		int source_row = (height - 1 - y) * bitmap_width;
		for (int x = 0; x < width; x++) {
			if (bpp == 2) {
				surface_data[y * surface_width + x] = pal16[bitmap_data[source_row + x]];
			} else {
				surface_data[y * surface_width + x] = bitmap_data[source_row + x];
			}
		}
	}

	if (bpp == 1) {
		memcpy(new_entry.palette, bitmap_info_header->bmiColors, sizeof(new_entry.palette));
	}
	new_entry.surf = surface;

	surface->Unlock();
	operator delete(bitmap_info_header);
	operator delete(bitmap_data);

	Wstring key(name);
	key.toLower();

	SurfaceCacheEntry old_entry;
	if (getValue(key, old_entry)) {
		remove(key);
		delete old_entry.surf;
	}

	add(key, new_entry);

	return(true);
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

// FUNCTION: GAMEMD 0x006BA3E0
bool SurfaceCacheClass::Draw(Rect const & rect, Surface & tosurface, Surface & fromsurface, int x, int y)
{
	unsigned short * dest = (unsigned short *)tosurface.Lock();
	if (dest == NULL) {
		return(false);
	}
	unsigned short const * src = (unsigned short const *)fromsurface.Lock();
	if (src == NULL) {
		tosurface.Unlock();
		return(false);
	}

	int dest_pitch = tosurface.Stride() / 2;
	int src_pitch = fromsurface.Stride() / 2;
	Rect image = fromsurface.Get_Rect();
	Rect trect = tosurface.Get_Rect();

	int row = rect.Height;
	int x_margin = MAX((image.Width - rect.Width) / 2, 0);
	int y_margin = MAX((image.Height - row) / 2, 0);
	for (row = 0; row < rect.Height; ++row) {
		int offset = (rect.Y + row) * dest_pitch + rect.X;
		for (int column = 0; column < rect.Width; ++column) {
			int src_row = (y_margin + y + row) % image.Height;
			int src_column = (x_margin + x + column) % image.Width;
			dest[offset + column] = src[src_row * src_pitch + src_column];
		}
	}
	fromsurface.Unlock();
	tosurface.Unlock();
	return(true);
}

// FUNCTION: GAMEMD 0x006BA580
bool SurfaceCacheClass::DrawTrans(Rect const & rect, Surface & tosurface, Surface & fromsurface, short trans)
{
	unsigned short * dest = (unsigned short *)tosurface.Lock();
	if (dest == NULL) {
		return(false);
	}
	unsigned short * src = (unsigned short *)fromsurface.Lock();
	if (src == NULL) {
		tosurface.Unlock();
		return(false);
	}
	Rect area = rect;
	area.Width = fromsurface.Get_Width();
	area.Height = fromsurface.Get_Height();
	int pitch = tosurface.Stride() / 2;
	int index = 0;
	for (int y = 0; y < area.Height; y++) {
		int offset = (y + area.Y) * pitch + area.X;
		for (int x = 0; x < area.Width; x++) {
			unsigned short value = src[index];
			if (value != (unsigned short)trans) {
				dest[offset + x] = value;
			}
			index++;
		}
	}
	tosurface.Unlock();
	fromsurface.Unlock();
	return(true);
}

// FUNCTION: GAMEMD 0x006BA680
bool SurfaceCacheClass::DrawNullsub(Rect const & rect, Surface & tosurface, Surface & fromsurface)
{
	return(false);
}

// FUNCTION: GAMEMD 0x006BA690
bool SurfaceCacheClass::DrawMasked(Rect const & rect, Surface & tosurface, Surface & fromsurface, Surface & masksurface, void * palette, int center, int x_offset, int y_offset)
{
	unsigned short pal16[256];

	Rect image_rect = fromsurface.Get_Rect();
	int src_x = 0;
	int top_clip = 0;
	int right_clip = 0;
	int src_y = 0;
	int dst_x_center = 0;
	int dst_y_center = 0;

	if (image_rect.Width > rect.Width) {
		if (center) {
			src_x = (image_rect.Width - rect.Width + 1) / 2;
			right_clip = (image_rect.Width - rect.Width) / 2;
		} else {
			right_clip = image_rect.Width - rect.Width;
		}
	} else if (center) {
		dst_x_center = (rect.Width - image_rect.Width) / 2;
	}

	if (image_rect.Height > rect.Height) {
		if (center) {
			top_clip = (image_rect.Height - rect.Height + 1) / 2;
			src_y = (image_rect.Height - rect.Height) / 2;
		} else {
			src_y = image_rect.Height - rect.Height;
		}
	} else if (center) {
		dst_y_center = (rect.Height - image_rect.Height) / 2;
	}

	if (x_offset > src_x) {
		right_clip += src_x - x_offset;
		src_x = x_offset;
	}
	if (y_offset > top_clip) {
		src_y += top_clip - y_offset;
		top_clip = y_offset;
	}

	unsigned short * dest = (unsigned short *)tosurface.Lock();
	if (dest == NULL) {
		return(false);
	}

	unsigned short * source = NULL;
	unsigned char * palsource = NULL;
	if (palette == NULL) {
		source = (unsigned short *)fromsurface.Lock();
	} else {
		palsource = (unsigned char *)fromsurface.Lock();
	}
	if (source == NULL && palsource == NULL) {
		tosurface.Unlock();
		return(false);
	}

	unsigned char * mask = (unsigned char *)masksurface.Lock();

	if (palette != NULL) {
		unsigned char * pal = (unsigned char *)palette;
		for (int i = 0; i < 256; i++) {
			pal16[i] = DSurface::Build_Hicolor_Pixel(pal[i * 3], pal[i * 3 + 1], pal[i * 3 + 2]);
		}
	}

	int dst_stride = tosurface.Stride() / 2;
	unsigned int src_stride = fromsurface.Stride();
	if (palette == NULL) {
		src_stride >>= 1;
	}

	for (int y = top_clip; y < image_rect.Height - src_y; y++) {
		int dst_index = dst_stride * (rect.Y - top_clip + y + dst_y_center) + rect.X + dst_x_center;
		int src_index = y * src_stride + src_x;
		if (palette == NULL) {
			for (int x = src_x; x < image_rect.Width - right_clip; x++) {
				if (mask[src_index] != 0) {
					dest[dst_index] = OD_Blend_Color(dest[dst_index], source[src_index], mask[src_index]);
				}
				dst_index++;
				src_index++;
			}
		} else {
			for (int x = src_x; x < image_rect.Width - right_clip; x++) {
				if (mask[src_index] != 0) {
					dest[dst_index] = OD_Blend_Color(dest[dst_index], pal16[palsource[src_index]], mask[src_index]);
				}
				dst_index++;
				src_index++;
			}
		}
	}

	tosurface.Unlock();
	fromsurface.Unlock();
	masksurface.Unlock();
	return(true);
}

// TEMPLATE: GAMEMD 0x006BAC40
// ?remove@?$Dictionary@VWstring@@USurfaceCacheEntry@@@@QAEDAAVWstring@@AAUSurfaceCacheEntry@@@Z
template class Dictionary<Wstring, SurfaceCacheEntry>;

// TEMPLATE: GAMEMD 0x006BAEF0
// ?shrink@?$Dictionary@VWstring@@USurfaceCacheEntry@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x006BAFA0
// ?expand@?$Dictionary@VWstring@@USurfaceCacheEntry@@@@AAEXXZ
