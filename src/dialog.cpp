// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "dialog.h"
#include "_font.h"
#include "_surface.h"
#include "bitfont.h"
#include "data.h"
#include "dsurface.h"
#include "font.h"
#include "gadget.h"
#include "hsv.h"
#include "lightcon.h"
#include "rgb.h"
#include "scheme.h"
#include "surface.h"
#include "vector.h"
#include "color.hh"
#include "dialog.hh"
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

// SYNTHETIC: GAMEMD 0x004A59C0 SYMBOL
// _$E4

// FUNCTION: GAMEMD 0x004A59E0
Rect * Drawing_GetTextDimensions(Rect * out, wchar_t const * text, Point2D loc, unsigned short flags, int marginX, int marginY)
{
	BitFont * font = BitFont::Instance;
	int width = font->FUN_00433ED0(text, 0);
	int height = font->field_1C;
	int x = loc.X;

	if (flags & 0x100) {
		x -= width / 2;
	} else if (flags & 0x200) {
		x -= width;
	}
	*out = Rect(x - marginX, loc.Y - marginY, width + marginX * 2, height + marginY * 2);
	return out;
}

// FUNCTION: GAMEMD 0x004A5A50
void Draw_Box(Rect const & rect, BoxStyleEnum up, bool filled)
{
	ColorScheme * scheme = ColorSchemes[GadgetClass::Get_Color_Scheme()];

	BoxStyleType const ButtonColors[BOXSTYLE_COUNT] = {
		{ scheme->Background, scheme->Highlight, scheme->Shadow,  scheme->Corners},
		{ scheme->Background, scheme->Shadow, scheme->Highlight,  scheme->Corners},
		{ DKGREY, WHITE,  BLACK,  DKGREY},
		{ DKGREY, BLACK,  LTGREY, DKGREY},
		{ scheme->Background,  scheme->Box, scheme->Box,  scheme->Background},
		{ scheme->Background,  scheme->Box, scheme->Box,  scheme->Background},
	};

	BoxStyleType const &style = ButtonColors[up];

	if (filled) {
		LogicalSurface->Fill_Rect(rect, scheme->Converter->Convert_Pixel(style.Filler));
	}

	switch (up) {
		case (BOXSTYLE_BOX):
			LogicalSurface->Draw_Rect(rect, scheme->Converter->Convert_Pixel(style.Highlight));
			break;

		case (BOXSTYLE_BORDER):
			LogicalSurface->Draw_Rect(Rect(rect.X+1, rect.Y+1, rect.Width-1, rect.Height-1), scheme->Converter->Convert_Pixel(style.Highlight));
			break;

		default:
			LogicalSurface->Draw_Line(Point2D(rect.X, rect.Y+rect.Height), Point2D(rect.X+rect.Width, rect.Y+rect.Height), scheme->Converter->Convert_Pixel(style.Shadow));
			LogicalSurface->Draw_Line(Point2D(rect.X+rect.Width, rect.Y), Point2D(rect.X+rect.Width, rect.Y+rect.Height), scheme->Converter->Convert_Pixel(style.Shadow));

			LogicalSurface->Draw_Line(Point2D(rect.X, rect.Y), Point2D(rect.X+rect.Width, rect.Y), scheme->Converter->Convert_Pixel(style.Highlight));
			LogicalSurface->Draw_Line(Point2D(rect.X, rect.Y), Point2D(rect.X, rect.Y+rect.Height), scheme->Converter->Convert_Pixel(style.Highlight));

			LogicalSurface->Put_Pixel(Point2D(rect.X, rect.Y+rect.Height), scheme->Converter->Convert_Pixel(style.Corner));
			LogicalSurface->Put_Pixel(Point2D(rect.X+rect.Width, rect.Y), scheme->Converter->Convert_Pixel(style.Corner));
			break;
	}
}

// FUNCTION: GAMEMD 0x004A5E50
int Count_Text_Lines(wchar_t const * text, int unused, int columns, int * width, int * height)
{
	*width = 0;
	*height = 0;
	BitFont * font = BitFont::Instance;
	if (text && font) {
		font->GetTextDimension(text, width, height, font->FUN_004349B0(0x57) * columns);
		return *height / font->field_1C;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x004A5EB0
Point2D Simple_Text_Print_Wide(wchar_t const * text, Surface * surface, Rect const * bounds, Point2D const * pt, unsigned fore, unsigned back, TextPrintType flags, int unused)
{
	BitFont * font = BitFont::Instance;
	int width;
	int height;
	int x = bounds->X + pt->X;
	int y = bounds->Y + pt->Y;

	font->GetTextDimension(text, &width, &height, bounds->Width);
	if (flags & TPF_CENTER) {
		x -= width / 2;
	} else if (flags & TPF_RIGHT) {
		x -= width;
	}
	if (flags & 0x8000) {
		Rect box(x - 1, y - 1, width + 2, height + 2);
		surface->Fill_Rect_Trans(box, RGBClass(0, 0, 0), 60);
	} else if (flags & 0x400) {
		Rect box(x - 1, y - 1, width + 2, height + 2);
		surface->Fill_Rect(box, 0);
	}
	LTRBStruct clip = { bounds->X, bounds->Y, bounds->X + bounds->Width, bounds->Y + bounds->Height };
	font->SetClipMode(true);
	font->SetBounds(&clip);
	font->SetColor(fore);
	BitText::Instance->Print(font, surface, text, x, y, 0, 0);
	return Point2D(x, y);
}

// FUNCTION: GAMEMD 0x004A6010
Point2D Simple_Text_Print_Wide(wchar_t const * text, Surface * surface, Rect const * bounds, Point2D const * pt, ColorScheme * scheme, unsigned back, TextPrintType flags, int unused)
{
	RGBClass rgb = scheme->HSV;
	unsigned fore = (rgb.Get_Red() >> DSurface::RedLeft << DSurface::RedRight) | (rgb.Get_Green() >> DSurface::GreenLeft << DSurface::GreenRight) | (rgb.Get_Blue() >> DSurface::BlueLeft << DSurface::BlueRight);
	return Simple_Text_Print_Wide(text, surface, bounds, pt, fore, back, flags, unused);
}

// FUNCTION: GAMEMD 0x004A60D0
BitFont * Font_From_TPF(TextPrintType flags)
{
	return BitFont::Instance;
}

// FUNCTION: GAMEMD 0x004A60E0
Point2D * __cdecl Fancy_Text_Print_Wide(Point2D * ret, wchar_t const * text, Surface * surface, Rect const * bounds, Point2D const * pt, unsigned fore, unsigned back, TextPrintType flags, ...)
{
	wchar_t buffer[512];

	if (text) {
		va_list args;
		va_start(args, flags);
		vswprintf(buffer, text, args);
		va_end(args);
		*ret = Simple_Text_Print_Wide(buffer, surface, bounds, pt, fore, back, flags, 1);
	} else {
		*ret = Simple_Text_Print_Wide(0, surface, bounds, pt, fore, back, flags, 1);
	}
	return ret;
}

// FUNCTION: GAMEMD 0x004A61C0
Point2D * __cdecl Fancy_Text_Print_Wide(Point2D * ret, wchar_t const * text, Surface * surface, Rect const * bounds, Point2D const * pt, ColorScheme * scheme, unsigned back, TextPrintType flags, ...)
{
	wchar_t buffer[512];

	if (text) {
		va_list args;
		va_start(args, flags);
		vswprintf(buffer, text, args);
		va_end(args);
		*ret = Simple_Text_Print_Wide(buffer, surface, bounds, pt, scheme, back, flags, 1);
	} else {
		*ret = Simple_Text_Print_Wide(0, surface, bounds, pt, scheme, back, flags, 1);
	}
	return ret;
}

// FUNCTION: GAMEMD 0x004A6360
Point2D Conquer_Clip_Text_Print_Wide(wchar_t const * text, Surface * surface, Rect const * rect, Point2D const * pt, unsigned fore, unsigned back, TextPrintType flag, int width, int const * tabs)
{
	wchar_t buffer[512];

	Point2D point = *pt;

	if (text) {
		wcscpy(buffer, text);

		BitFont * font = BitFont::Instance;

		wchar_t * source = &buffer[0];
		unsigned offset = 0;
		int processing = true;
		while (processing && offset < (unsigned)width) {
			wchar_t * ptr = wcschr(source, '\t');

			if (ptr) {
				*ptr = L'\0';
			}

			if (*source) {
				int w = 0;
				wchar_t * bptr = source;
				do {
					w += font->FUN_004349B0(*bptr++);
				} while (*bptr && offset+w < (unsigned)width);

				if (offset+w >= (unsigned)width) {
					bptr--;
					w -= font->FUN_004349B0(*bptr);
					*bptr = L'\0';
					processing = 0;
				}

				point = Simple_Text_Print_Wide(source, surface, rect, &Point2D(pt->X+offset, pt->Y), fore, back, flag, 1);
				offset += w;
			}

			if (ptr) {
				if (tabs) {
					while (offset > (unsigned)*tabs) {
						tabs++;
					}
					offset = *tabs;
				} else {
					offset = ((offset+1 / 50) + 1) * 50;
				}
				source = ptr+1;
			} else {
				break;
			}
		}
	}
	return point;
}

// FUNCTION: GAMEMD 0x004A6500
Point2D Conquer_Clip_Text_Print_Wide(wchar_t const * text, Surface * surface, Rect const * rect, Point2D const * pt, ColorScheme * scheme, unsigned back, TextPrintType flag, int width, int const * tabs)
{
	RGBClass rgb = scheme->HSV;
	unsigned fore = (rgb.Get_Red() >> DSurface::RedLeft << DSurface::RedRight) | (rgb.Get_Green() >> DSurface::GreenLeft << DSurface::GreenRight) | (rgb.Get_Blue() >> DSurface::BlueLeft << DSurface::BlueRight);
	return Conquer_Clip_Text_Print_Wide(text, surface, rect, pt, fore, back, flag, width, tabs);
}

// FUNCTION: GAMEMD 0x004A67E0
unsigned char * Font_Palette(int color)
{
	static unsigned char _fpalette[16];

	memset(_fpalette, '\0', sizeof(_fpalette));
	memset(&_fpalette[11], color, 5);
	return(_fpalette);
}
