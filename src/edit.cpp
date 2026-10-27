// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "edit.h"
#include "_keyboar.h"
#include "_surface.h"
#include "dialog.h"
#include "dsurface.h"
#include "font.h"
#include "globals.h"
#include "keyboard.h"
#include "scheme.h"
#include "vector.h"
#include "color.hh"
#include "bitfont.h"
#include "keyboard.h"
#include <math.h>
#include <ctype.h>
#include <wchar.h>


class InputManagerClass : public KeyboardClass {
	public:
		static InputManagerClass * Instance;
};

// FUNCTION: GAMEMD 0x004C2FC0
EditClass::EditClass(int id, wchar_t * text, int max_len, TextPrintType flags, int x, int y, int w, int h, EditStyle style) :
	BASECLASS(id, x, y, w, h, LEFTPRESS)
{
	TextFlags = (TextPrintType)(flags & ~(TPF_CENTER));
	EditFlags = style;
	String = text;
	MaxLength = max_len-1;
	Length = wcslen(String);
	((EditClass *)this)->Flag_To_Redraw();
	Color = GadgetClass::Get_Color_Scheme();

	if (w == -1 || h == -1) {
		BitFont * font = Font_From_TPF(flags);

		if (h == -1) {
			Height = font->field_1C+1;
		}
		if (w == -1) {
			if (wcslen(String) > 0) {
				Width = font->FUN_00433ED0(String, 0) + 6;
			} else {
				Width = (font->FUN_00433ED0(L"X", 0) * (MaxLength+1)) + 2;
			}
		}
	}

	IsReadOnly = 0;
}

// FUNCTION: GAMEMD 0x004C30B0
EditClass::~EditClass(void)
{
	if (Has_Focus()) {
		Clear_Focus();
	}
}

// FUNCTION: GAMEMD 0x004C30E0
void EditClass::Set_Text(wchar_t * text, int max_len)
{
	String = text;
	MaxLength = max_len-1;
	Length = wcslen(String);
	Flag_To_Redraw();
}

// FUNCTION: GAMEMD 0x004C3110
int EditClass::Draw_Me(int forced)
{
	if (BASECLASS::Draw_Me(forced)) {
		Conditional_Hide_Mouse(Rect(X, Y, Width, Height));
		Draw_Background();
		Draw_Text(String);
		Conditional_Show_Mouse();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004C3190
int EditClass::Action(unsigned flags, KeyNumType & key, int modifier)
{
	if (IsReadOnly) {
		return(false);
	}

	if ((flags & LEFTPRESS)) {
		flags &= ~LEFTPRESS;
		Set_Focus();
		Flag_To_Redraw();
	}

	if ((flags & KEYBOARD) && Has_Focus()) {
		if (key == KN_ESC) {
			Clear_Focus();
			flags = 0;
		} else {
			KeyASCIIType ascii = (KeyASCIIType)(InputManagerClass::Instance->To_ASCII(key) & 0xff);

			if ((key & WWKEY_VK_BIT) && ascii >= '0' && ascii <= '9') {
				key = (KeyNumType)(key & ~WWKEY_VK_BIT);
				if ((!(flags & LEFTRELEASE)) && (!(flags & RIGHTRELEASE))) {
					if (Handle_Key(ascii)) {
						flags &= ~KEYBOARD;
						key = KN_NONE;
					}
				}
			} else {
				if ((!(key & WWKEY_VK_BIT) && ascii >= ' ' && ascii <= 255)
					|| key == KN_RETURN || key == KN_BACKSPACE) {
					if ((!(flags & LEFTRELEASE)) && (!(flags & RIGHTRELEASE))) {
						if (Handle_Key((KeyASCIIType)InputManagerClass::Instance->To_ASCII(key))) {
							flags &= ~KEYBOARD;
							key = KN_NONE;
						}
					}
				} else {
					flags &= ~KEYBOARD;
					key = KN_NONE;
				}
			}
		}
	}

	return(BASECLASS::Action(flags, key, 0));
}

// FUNCTION: GAMEMD 0x004C32A0
void EditClass::Draw_Background(void)
{
	Draw_Box (Rect(X, Y, Width, Height), BOXSTYLE_BOX, true);
}

// FUNCTION: GAMEMD 0x004C32E0
void EditClass::Draw_Text(wchar_t const * text)
{
	TextPrintType flags;

	if (Has_Focus()) {
		flags = TPF_BRIGHT_COLOR;
	} else {
		flags = (TextPrintType)0;
	}

	Conquer_Clip_Text_Print_Wide(text, DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(X+1, Y+1), ColorSchemes[Color], TBLACK, (TextPrintType)(TextFlags | flags), Width-2, 0);
	BitFont * font = Font_From_TPF(TextFlags);
	if (Has_Focus() && wcslen(text) < (unsigned)MaxLength &&
		(font->FUN_00433ED0(text, 0) + font->FUN_00433ED0(L"_", 0) < Width-2)) {
		Conquer_Clip_Text_Print_Wide(L"_", DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(X+1+font->FUN_00433ED0(text, 0), Y+1), ColorSchemes[Color], TBLACK, (TextPrintType)(TextFlags | flags), -1, 0);
	}
}

// FUNCTION: GAMEMD 0x004C3420
bool EditClass::Handle_Key(KeyASCIIType ascii)
{
	BitFont * font = Font_From_TPF(TextFlags);

	switch (ascii) {
		case 0:
			break;

		case KA_RETURN:
			Clear_Focus();
			return(false);

		case KA_BACKSPACE:
			if (Length) {
				Length--;
				String[Length] = L'\0';
				Flag_To_Redraw();
			}
			break;

		default:
			if ((font->FUN_00433ED0(String, 0) + font->FUN_004349B0(ascii)) >= (Width-2)) {
				break;
			}

			if ((unsigned)Length >= (unsigned)MaxLength) break;

			if (!isgraph(ascii) && ascii != ' ') break;
			if (ascii == ' ' && Length == 0) break;

			if ((EditFlags & UPPERCASE) && isalpha(ascii)) {
				ascii = (KeyASCIIType)toupper(ascii);
			}

			if ((!(EditFlags & NUMERIC) || !isdigit(ascii)) &&
				(!(EditFlags & ALPHA) || !isalpha(ascii)) &&
				(!(EditFlags & MISC) || isalnum(ascii)) &&
				ascii != ' ') {
					break;
			}

			String[Length++] = ascii;
			String[Length] = L'\0';
			Flag_To_Redraw();
			break;
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004C3570
void EditClass::Set_Focus(void)
{
	Length = 0;
	if (String) {
		Length = wcslen(String);
	}
	BASECLASS::Set_Focus();
}

#include "geometry.h"

// SYNTHETIC: GAMEMD 0x004C35A0 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x004C35D0 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x004C35F0 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x004C3610 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x004C3630 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x004C3650 SYMBOL
// _$E46
// SYNTHETIC: GAMEMD 0x004C3670 SYMBOL
// _$E49
// SYNTHETIC: GAMEMD 0x004C36B0 SYMBOL
// _$E52
// SYNTHETIC: GAMEMD 0x004C36E0 SYMBOL
// _$E55
// SYNTHETIC: GAMEMD 0x004C3710 SYMBOL
// _$E58
// SYNTHETIC: GAMEMD 0x004C3740 SYMBOL
// _$E61
// SYNTHETIC: GAMEMD 0x004C3750 SYMBOL
// _$E64
// SYNTHETIC: GAMEMD 0x004C3770 SYMBOL
// _$E67

// SYNTHETIC: GAMEMD 0x004C3790 SYMBOL
// _$E70
const Rect RECT_NONE_2(0, 0, 0, 0);

