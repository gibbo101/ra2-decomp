// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "dib.h"
#include <math.h>

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

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E5EC8;
extern const double DAT_007E5ED0;

// SYNTHETIC: GAMEMD 0x004A6A60 SYMBOL
// _$E5
double DAT_008A0238 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x004A6A90 SYMBOL
// _$E8
double DAT_008A0288 = DAT_008A0238 / DAT_007E5EC8;

// SYNTHETIC: GAMEMD 0x004A6AB0 SYMBOL
// _$E11
double DAT_008A0270 = DAT_007E1718 / DAT_008A0288;

// SYNTHETIC: GAMEMD 0x004A6AD0 SYMBOL
// _$E14
double DAT_008A0268 = DAT_007E1720 * DAT_007E5ED0;

// SYNTHETIC: GAMEMD 0x004A6AF0 SYMBOL
// _$E17
double DAT_008A0240 = DAT_007E1728 * DAT_007E5ED0;

// SYNTHETIC: GAMEMD 0x004A6B10 SYMBOL
// _$E20
double DAT_008A0200 = DAT_007E1730 * DAT_007E5ED0;

// SYNTHETIC: GAMEMD 0x004A6B30 SYMBOL
// _$E23
int DAT_008A0290 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_008A0200 - DAT_008A0240) * DAT_008A0238));

// SYNTHETIC: GAMEMD 0x004A6B70 SYMBOL
// _$E26
double DAT_008A0248 = FUN_004CADE0(DAT_008A0290 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x004A6BA0 SYMBOL
// _$E29
double DAT_008A0260 = FUN_004CADE0((DAT_008A0290 * 2) / DAT_008A0238);

// SYNTHETIC: GAMEMD 0x004A6BD0 SYMBOL
// _$E32
int DAT_008A0284 = (int)(DAT_008A0290 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntTriple2 {
	int A, B, C;
	IntTriple2() { C = 0; A = 128; B = 128; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// SYNTHETIC: GAMEMD 0x004A6C00 SYMBOL
// _$E35
ShortPair DAT_008A0230;

// SYNTHETIC: GAMEMD 0x004A6C10 SYMBOL
// _$E38
IntTriple DAT_008A0250;

// SYNTHETIC: GAMEMD 0x004A6C30 SYMBOL
// _$E41
IntTriple2 DAT_008A0278;

// SYNTHETIC: GAMEMD 0x004A6C50 SYMBOL
// _$E44
IntQuad DAT_008A0220;
