// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "textbtn.h"
#include "_surface.h"
#include "dsurface.h"
#include "geometry.h"
#include "dialog.h"
#include "bitfont.h"
#include "lightcon.h"
#include "scheme.h"
#include "surface.h"
#include "vector.h"

// FUNCTION: 0x0071FF20
TextButtonClass::TextButtonClass(unsigned id, wchar_t const * text, TextPrintType style, int x, int y, int w, int h, bool blackborder, bool nobackground) :
	BASECLASS(id, x, y, w, h),
	IsNoBackground(nobackground),
	IsBlackBorder(blackborder),
	String(text),
	PrintFlags(style)
{
	if (w == -1 || h == -1) {
		BitFont * font = Font_From_TPF(PrintFlags);
		if (w == -1) {
			Width = font->FUN_00433ED0(String, 0) + 8;
		}
		if (h == -1) {
			Height = font->field_1C + 2;
		}
	}
}

// FUNCTION: 0x0071FFB0
TextButtonClass::TextButtonClass(void) :
	BASECLASS(0, 0, 0, 0, 0)
{
	IsBlackBorder = false;
	String = NULL;
	PrintFlags = TPF_8POINT;
}

// FUNCTION: 0x0071FFE0
int TextButtonClass::Draw_Me(int forced)
{
	if (ControlClass::Draw_Me(forced)) {
		Draw_Background();
		Draw_Text(String);

		return(true);
	}
	return(false);
}

// FUNCTION: 0x00720020
void TextButtonClass::Set_Text(wchar_t const * text, bool resize)
{
	String = text;
	Flag_To_Redraw();
	if (resize && String) {
		BitFont * font = Font_From_TPF(PrintFlags);

		Width = font->FUN_00433ED0(String, 0) + 8;
		Height = font->field_1C + 2;
	}
}

// FUNCTION: 0x00720070
void TextButtonClass::Draw_Background(void)
{
	if (IsNoBackground) return;

	if (IsBlackBorder) {
		DSurface::Temp->Draw_Rect(Rect(X-1, Y-1, X+Width+2, Y+Height+2), ColorSchemes[Get_Color_Scheme()]->Converter->Convert_Pixel(BLACK));
	}

	BoxStyleEnum style;

	if (IsDisabled) {
		style = BOXSTYLE_DIS_RAISED;
	} else {
		if (IsPressed) {
			style = BOXSTYLE_DOWN;
		} else {
			style = BOXSTYLE_RAISED;
		}
	}

	Draw_Box(Rect(X, Y, Width, Height), style, true);
}

// FUNCTION: 0x00720140
void TextButtonClass::Draw_Text(wchar_t const * text)
{
	int scheme = GadgetClass::Get_Color_Scheme();

	if (String) {
		TextPrintType flags;

		if (IsDisabled) {
			flags = (TextPrintType)0;
		} else {
			if (IsPressed || IsOn) {
				flags = TextPrintType(TPF_USE_GRAD_PAL|TPF_BRIGHT_COLOR);
			} else {
				flags = TextPrintType(TPF_USE_GRAD_PAL|TPF_MEDIUM_COLOR);
			}
		}

		if (ColorSchemes.Count() != 0) {
			if (scheme <= 0 || scheme >= ColorSchemes.Count()) {
				scheme = 0;
			}
			ColorScheme * colors = ColorSchemes[scheme];
			if (colors) {
				Point2D pt(X+(Width>>1)-1, Y+1);
				Simple_Text_Print_Wide(text, DSurface::Temp, &DSurface::Temp->Get_Rect(), &pt, colors, TBLACK, TextPrintType(PrintFlags|flags|TPF_CENTER), true);
			}
		}
	}
}

// FUNCTION: 0x00720200 TextButtonClass::Set_Style

// FUNCTION: 0x00720210 TextButtonClass::~TextButtonClass

// FUNCTION: 0x00720230 $E27
// FUNCTION: 0x00720260 $E30
// FUNCTION: 0x00720280 $E33
// FUNCTION: 0x007202A0 $E36
