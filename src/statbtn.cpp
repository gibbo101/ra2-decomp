// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "statbtn.h"
#include "_surface.h"
#include "blit.h"
#include "dialog.h"
#include "font.h"
#include "scheme.h"
#include "surface.h"
#include "vector.h"
#include "bitfont.h"
#include "geometry.h"
#include "dsurface.h"
#include "color.hh"

#include <math.h>

static inline Point2D * Scratch(Point2D const & p) { return (Point2D *)&p; }

// FUNCTION: GAMEMD 0x006C6540
StaticButtonClass::StaticButtonClass(unsigned , wchar_t const * text, TextPrintType style, int x, int y, int w, int h) :
	BASECLASS(x, y, w, h, FlagEnum(0)),
	String(NULL),
	PrintFlags(style)
{
	Set_Text(text, false);

	if (w == -1 || h == -1) {
		BitFont *font = Font_From_TPF(PrintFlags);
		if (w == -1) {
			Width = font->FUN_00433ED0(String, 0);
		}
		if (h == -1) {
			Height = font->field_1C;
		}
	}
}

// FUNCTION: GAMEMD 0x006C65D0
StaticButtonClass::StaticButtonClass(void) :
	BASECLASS(0, 0, 0, 0, FlagEnum(0)),
	String(NULL),
	PrintFlags(TPF_8POINT)
{
}

// FUNCTION: GAMEMD 0x006C6610
StaticButtonClass::~StaticButtonClass(void)
{
	if (String != NULL) {
		delete [] String;
	}
}

// FUNCTION: GAMEMD 0x006C6640
int StaticButtonClass::Draw_Me(int forced)
{
	if (BASECLASS::Draw_Me(forced)) {
		Draw_Background();
		Draw_Text(String);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006C6680
void StaticButtonClass::Set_Text(wchar_t const * text, bool resize)
{
	if (String != NULL) {
		delete [] String;
		String = NULL;
	}

	if (text != NULL) {
		String = new wchar_t[wcslen(text)+1];
		if (String != NULL) {
			wcscpy(String, text);
		}
	}

	Flag_To_Redraw();
	if (resize && String != NULL) {
		Draw_Background();
		BitFont *font = Font_From_TPF(PrintFlags);
		Width = font->FUN_00433ED0(String, 0);
		Height = font->field_1C;
		Background = Buffer();
	}
}

// FUNCTION: GAMEMD 0x006C6740
void StaticButtonClass::Draw_Background(void)
{
	if (Background.Get_Buffer() == NULL && Width > 0 && Height > 0) {
		new(&Background) Buffer(Width * Height * DSurface::Temp->Bytes_Per_Pixel());
		if (Background.Get_Buffer() != NULL) {
			To_Buffer(*DSurface::Temp, Rect(X, Y, Width, Height), Background);
		}
	}

	if (Background.Get_Buffer() != NULL) {
		From_Buffer(*DSurface::Temp, Rect(X, Y, Width, Height), Background);
	}
}

// FUNCTION: GAMEMD 0x006C67F0
void StaticButtonClass::Draw_Text(wchar_t const * text)
{
	if (String != NULL) {
		int x = X;

		if (PrintFlags & TPF_CENTER) {
			x += Width/2;
		}
		if (PrintFlags & TPF_RIGHT) {
			x += Width-1;
		}

		Fancy_Text_Print_Wide(Scratch(Point2D()), text, DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(x, Y), ColorSchemes[BASECLASS::Get_Color_Scheme()], TBLACK, PrintFlags);
	}
}


// SYNTHETIC: GAMEMD 0x006C6880
// ??_GStaticButtonClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x006C68C0 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x006C68F0 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x006C6910 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x006C6930 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x006C6950 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x006C6970 SYMBOL
// _$E46
// SYNTHETIC: GAMEMD 0x006C6990 SYMBOL
// _$E49
// SYNTHETIC: GAMEMD 0x006C69D0 SYMBOL
// _$E52
// SYNTHETIC: GAMEMD 0x006C6A00 SYMBOL
// _$E55
// SYNTHETIC: GAMEMD 0x006C6A30 SYMBOL
// _$E58
// SYNTHETIC: GAMEMD 0x006C6A60 SYMBOL
// _$E61
// SYNTHETIC: GAMEMD 0x006C6A70 SYMBOL
// _$E64
// SYNTHETIC: GAMEMD 0x006C6A90 SYMBOL
// _$E67

// SYNTHETIC: GAMEMD 0x006C6AB0 SYMBOL
// _$E70
const Rect RECT_NONE_2(0, 0, 0, 0);

