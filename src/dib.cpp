// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "dib.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x004A6820 SYMBOL
// _$E2

// FUNCTION: GAMEMD 0x004A6840
DIB8Class::DIB8Class(HWND hwnd,int width,int height,PaletteClass & pal):
	IsZombie(false),
	Info(NULL),
	Handle(0),
	Pixels(NULL),
	Width(width),
	Height(height),
	PixelBase(NULL),
	Pitch(NULL),
	Surface(NULL)
{
	Info = (BITMAPINFO *) new char [sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD)];

	if (Info == NULL) {
		IsZombie = true;
		return;
	}

	Info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info->bmiHeader.biWidth = width;
	Info->bmiHeader.biHeight = -height;
	Info->bmiHeader.biPlanes = 1;
	Info->bmiHeader.biBitCount = 8;
	Info->bmiHeader.biCompression = BI_RGB;
	Info->bmiHeader.biSizeImage = 0;
	Info->bmiHeader.biXPelsPerMeter = 0;
	Info->bmiHeader.biYPelsPerMeter = 0;
	Info->bmiHeader.biClrUsed = 256;
	Info->bmiHeader.biClrImportant = 256;

	for (int i=0; i<256; i++) {
		Info->bmiColors[i].rgbBlue 		= (unsigned char)pal[i].Get_Blue();
		Info->bmiColors[i].rgbGreen 	= (unsigned char)pal[i].Get_Green();
		Info->bmiColors[i].rgbRed 		= (unsigned char)pal[i].Get_Red();
		Info->bmiColors[i].rgbReserved 	= 0;
	}

	HDC hdc = GetDC(hwnd);
	Handle = CreateDIBSection(hdc, Info, DIB_RGB_COLORS,(void**)&Pixels, NULL, 0);
	ReleaseDC(hwnd, hdc);

	if (!Handle) {
		IsZombie = true;
		return;
	}

	Width = Info->bmiHeader.biWidth;
	Height = abs(Info->bmiHeader.biHeight);
	Pitch = (Width + 3) & 0xfffffffC;

	if (Info->bmiHeader.biHeight > 0) {
		PixelBase = (Pixels + (Height - 1) * Width);
		Pitch = -Pitch;

	} else {
		PixelBase = Pixels;
		Pitch = Pitch;
	}

	int surfwid = abs(Pitch);
	Surface = new BSurface(surfwid,Height,1,Pixels);

	if (Surface == NULL) {
		IsZombie = true;
		return;
	}
}

// FUNCTION: GAMEMD 0x004A69F0
DIB8Class::~DIB8Class(void)
{
	if (Info) delete [] Info;
	if (Handle) DeleteObject(Handle);
	if (Surface) delete Surface;
}

// FUNCTION: GAMEMD 0x004A6A20
void DIB8Class::Clear(unsigned char color)
{
	if (Pixels) {
		memset(Pixels, color, Width*Height);
	}
}
