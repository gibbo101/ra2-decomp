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

// FUNCTION: GAMEMD 0x006C8FF0
int CALLBACK FUN_006C8FF0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int handled = FUN_00622B50(window, message, wparam, lparam);
	if (handled) {
		return handled;
	}
	int * result = (int *)GetWindowLong(window, 8);
	switch (message) {
	case WM_COMMAND:
		if (LOWORD(wparam) == 0x423) {
			*result = 1;
			DAT_00B0BF28.End();
			return 0;
		}
		break;
	case 2:
		if (DAT_00B0BF64) {
			DAT_00B0BF28.End();
			DAT_00B0BF28.FUN_00405C00();
			DAT_00B0BF64 = 0;
			return 0;
		}
		break;
	case 0x497:
	{
		DAT_00B0BF28.FUN_00405BE0();
		DAT_00B0BF64 = 1;
		int par = 0;
		if (Scen->Difficulty == 0) {
			par = Scen->ParTimeEasy;
		} else if (Scen->Difficulty == 1) {
			par = Scen->ParTimeMedium;
		} else if (Scen->Difficulty == 2) {
			par = Scen->ParTimeDifficult;
		}
		FUN_006C8F10(window, 0x6f1, par / 60);
		int elapsed = Scen->ElapsedTimer.Value();
		FUN_006C8F10(window, 0x6f2, elapsed / 60);
		char * title;
		char * message_text;
		if (elapsed / 60 <= par / 60) {
			title = Scen->UnderParTitle;
			message_text = Scen->UnderParMessage;
		} else {
			title = Scen->OverParTitle;
			message_text = Scen->OverParMessage;
		}
		wchar_t const * a = StringTable::LoadString(title, NULL, "D:\\ra2mdpost\\stlscore.cpp", 0x8a);
		wchar_t const * b = StringTable::LoadString(message_text, NULL, "D:\\ra2mdpost\\stlscore.cpp", 0x8b);
		HWND item = GetDlgItem(window, 0x6f3);
		SendMessage(item, 0x4b2, 0, (LPARAM)a);
		item = GetDlgItem(window, 0x6f4);
		SendMessage(item, 0x4b2, 0, (LPARAM)b);
		int total = 0;
		int count = Houses.Count();
		for (int i = 0; i < count; i++) {
			HouseClass * house = Houses[i];
			if (house->IsPlayerControl) {
				total += house->DestroyedAircraft.Get_Unit_Total();
				total += house->DestroyedInfantry.Get_Unit_Total();
				total += house->DestroyedUnits.Get_Unit_Total();
				total += house->DestroyedBuildings.Get_Unit_Total();
			}
		}
		item = GetDlgItem(window, 0x72b);
		PostMessage(item, 0x4d7, 0, (LPARAM)window);
		FUN_006C8FA0(window, 0x6f6, total);
		Theme.Play_Song(Theme.From_Name("SCORE"));
		return 0;
	}
	case 0x4d8:
		if (wparam == 0) {
			SendDlgItemMessage(window, 0x72b, 0x4d5, wparam, 0x1d);
		}
		break;
	case 0x4ec:
		DAT_00B0BF28.Stop();
		DAT_00B0BF28.SetEvent((AudioEvent *)VocClass::PlayGlobal(Rule->ScoreAnimSound, 1.0f, 0x2000), NULL);
		SendDlgItemMessage(window, 0x72b, 0x4d3, 0, 0);
		break;
	}
	return 0;
}
