// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "house.h"
#include "scenario.h"
#include "theme.h"
#include "voc.h"
#include "audioctl.h"
#include "rules.h"
#include "_rules.h"

#include <math.h>
#include <wchar.h>
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x006C8C60 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x006C8C80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006C8CB0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006C8CD0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006C8CF0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006C8D10 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006C8D30 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006C8D50 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006C8D90 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006C8DC0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006C8DF0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006C8E20 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006C8E30 SYMBOL
// _$E35

extern int DAT_00B0BF48;
extern int DAT_00B0BF4C;
extern int DAT_00B0BF50;

class Surface;
struct Drawing {
	static int RenderWidth;
	static int RenderHeight;
};
extern Surface * DAT_0088730C;
void __fastcall FUN_004F4780(bool flag, Surface * surface, bool other);
void __fastcall FUN_0072D300(int side);
void FUN_0072D350(void);
void __fastcall FUN_0072E9F0(Surface * surface, Rect * rect);
int __fastcall FUN_0060D380(int resource, DLGPROC proc, bool focus);
int CALLBACK FUN_006C8FF0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
int __fastcall FUN_00622B50(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x006C8E50
void FUN_006C8E50(void)
{
	DAT_00B0BF50 = 0;
	DAT_00B0BF48 = 128;
	DAT_00B0BF4C = 128;
}

class STLScoreScreen
{
	public:
		STLScoreScreen(void);
		void Present(void);
};

// FUNCTION: GAMEMD 0x006C8E70
STLScoreScreen::STLScoreScreen(void)
{
}

// FUNCTION: GAMEMD 0x006C8E80
void STLScoreScreen::Present(void)
{
	FUN_0072D300(Scen->PlayerSideIndex);
	Rect rect(0, 0, Drawing::RenderWidth, Drawing::RenderHeight);
	FUN_0072E9F0(DAT_0088730C, &rect);
	FUN_004F4780(true, DAT_0088730C, false);
	FUN_0060D380(0x103, FUN_006C8FF0, false);
	FUN_0072E9F0(DAT_0088730C, &rect);
	FUN_004F4780(true, DAT_0088730C, false);
	FUN_0072D350();
}

// FUNCTION: GAMEMD 0x006C8F10
void __fastcall FUN_006C8F10(HWND dialog, int id, int seconds)
{
	wchar_t text[32];
	int minutes = seconds / 60;
	int hours = minutes / 60;
	swprintf(text, L"%02d:%02d:%02d", hours, minutes - hours * 60, seconds - minutes * 60);
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4b2, 0, (LPARAM)text);
}

// FUNCTION: GAMEMD 0x006C8FA0
void __fastcall FUN_006C8FA0(HWND dialog, int id, int value)
{
	wchar_t text[32];
	swprintf(text, L"%d", value);
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4b2, 0, (LPARAM)text);
}

extern AudioController DAT_00B0BF28;
extern int DAT_00B0BF64;
extern HouseClass ** DAT_00A8022C;
extern int DAT_00A80238;
