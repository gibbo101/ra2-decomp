// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "stbuffer.h"
#include "rle.h"
#include "surface.h"
#include <cstring>
#include "win.h"

// FUNCTION: 0x006C89B0
StaticBufferClass::StaticBufferClass(int size)
{
	Size = size;
	Buffer = new unsigned char[size];
	Cursor = Buffer;
}

// FUNCTION: 0x006C89D0
StaticBufferClass::~StaticBufferClass(void)
{
	if (Buffer != NULL) {
		delete [] Buffer;
	}
}




struct SystemTimer {
	static unsigned long GetTime(void);
};

extern int DAT_00B0BED8;
extern int DAT_00B0BEDC;
extern int DAT_00B0BEE0;
extern int DAT_00B0BEE4;

static char CompressionBuffer[256];

// FUNCTION: 0x006C89E0
StaticBufferClass::Entry * StaticBufferClass::Add(Surface & surface, SurfaceRegion const & region)
{
	Entry * header = (Entry *)Reserve(sizeof(Entry));
	if (header == NULL) {
		return(NULL);
	}

	header->X = region.Point.X;
	header->Y = region.Point.Y;
	header->Width = region.Bounds.Width;
	header->Height = region.Bounds.Height;
	header->Data = Cursor;

	unsigned char * data = (unsigned char *)surface.Lock(region.Bounds.TopLeft);

	RLEEngine rle;
	int line = 0;
	while (line < region.Bounds.Height) {
		int comp_size = rle.Line_Compress(data, CompressionBuffer, region.Bounds.Width);
		unsigned char * buffer = Reserve(comp_size);
		if (buffer == NULL) {
			surface.Unlock();
			return(NULL);
		}

		memcpy(buffer, CompressionBuffer, comp_size);
		line++;
		data += surface.Stride();
	}

	surface.Unlock();
	return(header);
}


// FUNCTION: 0x006C8B10
StaticBufferClass::Entry * StaticBufferClass::Add(Surface & surface, Rect const & cliprect, short x, short y)
{
	Entry * header = (Entry *)Reserve(sizeof(Entry));
	if (header == NULL) {
		return(NULL);
	}

	header->X = x;
	header->Y = y;
	header->Width = cliprect.Width;
	header->Height = cliprect.Height;
	header->Data = Cursor;

	unsigned char * data = (unsigned char *)surface.Lock(cliprect.TopLeft);

	RLEEngine rle;
	int line = 0;
	while (line < cliprect.Height) {
		int comp_size = rle.Line_Compress(data, CompressionBuffer, cliprect.Width);
		unsigned char * buffer = Reserve(comp_size);
		if (buffer == NULL) {
			surface.Unlock();
			return(NULL);
		}

		memcpy(buffer, CompressionBuffer, comp_size);
		line++;
		data += surface.Stride();
	}

	surface.Unlock();
	return(header);
}

// FUNCTION: 0x006C8C40
unsigned long SystemTimer::GetTime(void)
{
	return timeGetTime() >> 4;
}

// FUNCTION: 0x006C8C50
unsigned long FUN_006C8C50(void)
{
	return timeGetTime() >> 4;
}

// FUNCTION: 0x006C8C60
void FUN_006C8C60(void)
{
	DAT_00B0BED8 = 0;
	DAT_00B0BEDC = 0;
	DAT_00B0BEE0 = 0;
	DAT_00B0BEE4 = 0;
}
