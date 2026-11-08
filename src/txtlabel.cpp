// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "txtlabel.h"
#include "_surface.h"
#include "dialog.h"
#include "dsurface.h"
#include "scheme.h"
#include "geometry.h"
#include "vector.h"
#include "bitfont.h"
#include "rules.h"
#include "voc.h"
#include <mmsystem.h>
#include <math.h>

// SYNTHETIC: GAMEMD 0x0072A230 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0072A250 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x0072A280 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x0072A2A0 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x0072A2C0 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x0072A2E0 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x0072A300 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x0072A320 SYMBOL
// _$E46
// SYNTHETIC: GAMEMD 0x0072A360 SYMBOL
// _$E49
// SYNTHETIC: GAMEMD 0x0072A390 SYMBOL
// _$E52
// SYNTHETIC: GAMEMD 0x0072A3C0 SYMBOL
// _$E55
// SYNTHETIC: GAMEMD 0x0072A3F0 SYMBOL
// _$E58
// SYNTHETIC: GAMEMD 0x0072A400 SYMBOL
// _$E61
// SYNTHETIC: GAMEMD 0x0072A420 SYMBOL
// _$E64

// SYNTHETIC: GAMEMD 0x0072A690 SYMBOL
// _$E82
const Rect RECT_NONE_2(0, 0, 0, 0);

// FUNCTION: GAMEMD 0x0072A440
TextLabelClass::TextLabelClass(wchar_t * txt, int x, int y, int color, TextPrintType style) :
	GadgetClass(x, y, 1, 1, 0, 0),
	Text(txt),
	UserData1(0),
	UserData2(0),
	Style(style),
	Color(color),
	PixWidth(-1),
	AnimDword3C(0),
	SkipDraw(false),
	Animate(false),
	AnimPos(0),
	AnimTiming(0)
{
}

extern RulesClass *Rule;

void __fastcall FUN_00623880(Surface *surface, Rect *rect, wchar_t *text, int cursor, BitFont *font, unsigned color,
                             int *scroll, int focused, int mask, int draw, int unknown);

// FUNCTION: GAMEMD 0x0072A4A0
int TextLabelClass::Draw_Me(int forced)
{
	if (!SkipDraw && GadgetClass::Draw_Me(forced)) {
		int index = Color;
		if (ColorSchemes.Count() != 0) {
			if (index <= 0 || index >= ColorSchemes.Count()) {
				index = 0;
			}
			ColorScheme *scheme = ColorSchemes[index];
			if (scheme != 0) {
				BitFont *font = Font_From_TPF(Style);
				Rect rect;
				rect.X = X;
				rect.Y = Y;
				rect.Width = PixWidth < 0 ? LogicalSurface->Get_Width() : PixWidth;
				rect.Height = font->field_1C;
				int over = rect.X + rect.Width - LogicalSurface->Get_Width();
				if (over > 0) {
					rect.Width -= over;
					if (!rect.Is_Valid()) {
						return false;
					}
				}
				RGBClass rgb = RGBClass(scheme->HSV);
				unsigned color = rgb.Get_Red() | (rgb.Get_Green() << 8) | (rgb.Get_Blue() << 16);
				int length = wcslen(Text);
				if (Animate) {
					DWORD now = timeGetTime();
					int count = AnimPos;
					if (AnimPos == 0) {
						AnimPos = 1;
					} else {
						AnimPos += (now - AnimTiming) >> 4;
					}
					if (count != AnimPos) {
						AnimTiming = now;
					}
					if (AnimPos <= length) {
						VocClass::PlayGlobal(Rule->MessageCharTyped, 1.0f, 0x2000, 0);
					}
				}
				FUN_00623880(LogicalSurface, &rect, Text, length, font, color, (int *)&AnimDword3C, Has_Focus(), 0, 1,
				             AnimPos);
				return true;
			}
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x0072A660
// TextLabelClass::Set_Text

// SYNTHETIC: GAMEMD 0x0072A670
// ??_GTextLabelClass@@UAEPAXI@Z
