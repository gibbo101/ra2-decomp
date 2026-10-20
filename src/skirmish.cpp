// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "skirmish.h"
#include "_rules.h"
#include "globals.h"
#include "houstype.h"
#include "init.h"
#include "multiplayer.h"
#include "netshare.h"
#include "rules.h"
#include "session.h"
#include "windlg.h"

#include <math.h>
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "stringtable.h"

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007F3138;
extern const double DAT_007F3140;

// FUNCTION: 0x006AC970 $E70
double DAT_00B0B540 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// FUNCTION: 0x006AC9A0 $E73
double DAT_00B0B590 = DAT_00B0B540 / DAT_007F3138;

// FUNCTION: 0x006AC9C0 $E76
double DAT_00B0B578 = 1.0 / DAT_00B0B590;

// FUNCTION: 0x006AC9E0 $E79
double DAT_00B0B570 = DAT_007F3140 * 45.0;

// FUNCTION: 0x006ACA00 $E82
double DAT_00B0B548 = DAT_007F3140 * 60.0;

// FUNCTION: 0x006ACA20 $E85
double DAT_00B0B520 = DAT_007F3140 * 90.0;

// FUNCTION: 0x006ACA40 $E88
int DAT_00B0B598 = (int)(FUN_004CAD50(DAT_00B0B520 - DAT_00B0B548) * DAT_00B0B540 * 0.5);

// FUNCTION: 0x006ACA80 $E91
double DAT_00B0B550 = FUN_004CADE0(DAT_00B0B598 * 0.00390625);

// FUNCTION: 0x006ACAB0 $E94
double DAT_00B0B568 = FUN_004CADE0((DAT_00B0B598 * 2) / DAT_00B0B540);

// FUNCTION: 0x006ACAE0 $E97
int DAT_00B0B58C = (int)(DAT_00B0B598 * 4 + 0.5);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x006ACB10 $E100
ShortPair DAT_00B0B538;

// FUNCTION: 0x006ACB20 $E103
IntTriple DAT_00B0B558;

extern int DAT_00B0B580;
extern int DAT_00B0B584;
extern int DAT_00B0B588;
extern HWND DAT_00B0B59C;

int __fastcall FUN_004E3320(int index);
int __fastcall FUN_004E37D0(int index);
int __fastcall FUN_004E41D0(int index);
int __fastcall FUN_004E4E60(int index);
int __fastcall FUN_004E5940(int index);
void __fastcall FUN_004E3F70(HWND dialog, int id, int value);
void __fastcall FUN_004E49A0(HWND dialog, int id, int value);
void __fastcall FUN_004E5480(HWND dialog, int id, int value);
void __fastcall FUN_004E5ED0(HWND dialog, int id, int value);
void __fastcall FUN_006ADF00(HWND dialog, int first, int count);
void __fastcall FUN_006AE080(HWND dialog, int first);

// FUNCTION: 0x006ACB40
void FUN_006ACB40(void)
{
	DAT_00B0B588 = 0;
	DAT_00B0B580 = 0x80;
	DAT_00B0B584 = 0x80;
}

// FUNCTION: 0x006ACB60
int __fastcall FUN_006ACB60(int slot)
{
	if (slot == 0) return 0x50b;
	if (slot == 1) return 0x50e;
	if (slot == 2) return 0x516;
	if (slot == 3) return 0x51a;
	if (slot == 4) return 0x51b;
	if (slot == 5) return 0x51c;
	if (slot == 6) return 0x51d;
	return -1;
}

// FUNCTION: 0x006ACBC0
int __fastcall FUN_006ACBC0(int id)
{
	if (id == 0x50b) return 0;
	if (id == 0x50e) return 1;
	if (id == 0x516) return 2;
	if (id == 0x51a) return 3;
	if (id == 0x51b) return 4;
	if (id == 0x51c) return 5;
	if (id == 0x51d) return 6;
	return -1;
}

// FUNCTION: 0x006ACC20
wchar_t const * __fastcall FUN_006ACC20(int level)
{
	if (level == -1) return StringTable::LoadString("STT:PlayerNone", NULL, "D:\\ra2mdpost\\Skirmish.cpp", 0x87);
	if (level == 2) return StringTable::LoadString("STT:PlayerDumbAI", NULL, "D:\\ra2mdpost\\Skirmish.cpp", 0x89);
	if (level == 1) return StringTable::LoadString("STT:PlayerSmartAI", NULL, "D:\\ra2mdpost\\Skirmish.cpp", 0x8b);
	if (level == 0) return StringTable::LoadString("STT:PlayerGeniusAI", NULL, "D:\\ra2mdpost\\Skirmish.cpp", 0x8d);
	return NULL;
}

// FUNCTION: 0x006ACCA0
bool __fastcall FUN_006ACCA0(int first)
{
	HWND dialog = DAT_00B0B59C;
	int i = first;
	if (i < 8) {
		do {
			int id = FUN_006ACB60(i - 1);
			int sel = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
			int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, sel, 0);
			if (data == 0 || data == 1 || data == 2) {
				return true;
			}
			i++;
		} while (i < 8);
	}
	return false;
}

// FUNCTION: 0x006ACD60
void __fastcall FUN_006ACD60(HWND dialog)
{
	if (dialog == NULL) {
		dialog = WS_Find_Dialog(0x102);
	}
	bool flag = false;
	if (Session.MPGameMode != NULL) {
		flag = Session.MPGameMode->AlliesAllowed;
	}
	HWND item = GetDlgItem(dialog, 0x76d);
	if (item != NULL) {
		if (flag) {
			EnableWindow(item, TRUE);
		} else {
			EnableWindow(item, FALSE);
		}
		for (int i = 1; i < 8; i++) {
			int id = FUN_006ACB60(i - 1);
			int sel = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
			int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, sel, 0);
			int team = FUN_004E5940(i);
			if (team >= 0) {
				HWND teamItem = GetDlgItem(dialog, team);
				if (teamItem != NULL) {
					if (data == -1) {
						EnableWindow(teamItem, FALSE);
					} else if (!flag) {
						EnableWindow(teamItem, FALSE);
					} else {
						EnableWindow(teamItem, TRUE);
					}
				}
			}
		}
	}
}

// FUNCTION: 0x006ACE80
void __fastcall FUN_006ACE80(HWND dialog, int first)
{
	if (dialog != NULL && Session.MPGameMode != NULL) {
		int begin = 1;
		int end = 8;
		if (first > 0 && first < end) {
			begin = first;
			end = first + 1;
		}
		for (int i = begin; i < end; i++) {
			int team = FUN_004E5940(i);
			if (Session.MPGameMode != NULL && Session.MPGameMode->AlliesAllowed) {
				FUN_004E5ED0(dialog, team, 3);
			} else {
				FUN_004E5ED0(dialog, team, -2);
			}
		}
	}
}

// FUNCTION: 0x006ADC20
void __fastcall FUN_006ADC20(HWND dialog, int id)
{
	int slot = FUN_006ACBC0(id) + 1;
	int color = FUN_004E41D0(slot);
	int country = FUN_004E37D0(slot);
	int start = FUN_004E4E60(slot);
	int team = FUN_004E5940(slot);

	FUN_006ACE80(dialog, slot);

	int sel = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
	int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, sel, 0);
	bool enable = (data == 0 || data == 1 || data == 2);
	if (!enable) {
		FUN_004E49A0(dialog, color, -2);
		FUN_004E3F70(dialog, country, -2);
		FUN_004E5480(dialog, start, -2);
	}

	HWND colorItem = GetDlgItem(dialog, color);
	HWND countryItem = GetDlgItem(dialog, country);
	HWND startItem = GetDlgItem(dialog, start);
	HWND teamItem = GetDlgItem(dialog, team);
	EnableWindow(colorItem, enable);
	EnableWindow(countryItem, enable);
	EnableWindow(startItem, enable);
	EnableWindow(teamItem, enable);
}

// FUNCTION: 0x006ADDF0
void __fastcall FUN_006ADDF0(HWND dialog, int from, int to)
{
	int fromCount;
	if (from == -1) {
		fromCount = 8;
	} else {
		fromCount = RandomMapWaypointCount(from);
	}
	int toCount = RandomMapWaypointCount(to);
	int delta = toCount - fromCount;

	if (from >= 0 && to >= 0 && from < Session.Scenarios.Count() && to < Session.Scenarios.Count()) {
		bool fromRandom = Session.Scenarios[from]->Is_Random_Map();
		bool toRandom = Session.Scenarios[to]->Is_Random_Map();
		if (fromRandom && toRandom) {
			FUN_006AE080(dialog, 1);
			FUN_006ADF00(dialog, 1, toCount - 1);
		}
	}

	if (delta > 0) {
		FUN_006ADF00(dialog, fromCount, delta);
	} else if (delta < 0) {
		FUN_006AE080(dialog, toCount);
	}

	FUN_006ACE80(dialog, 0);
}

// FUNCTION: 0x006ADF00
void __fastcall FUN_006ADF00(HWND dialog, int first, int count)
{
	int end = first + count;
	for (int i = first; i < end; i++) {
		int id = FUN_006ACB60(i - 1);
		int country = FUN_004E3320(i);
		int color = FUN_004E41D0(i);
		int start = FUN_004E37D0(i);
		int team = FUN_004E4E60(i);
		int flag = FUN_004E5940(i);
		HWND hId = GetDlgItem(dialog, id);
		HWND hCountry = GetDlgItem(dialog, country);
		HWND hColor = GetDlgItem(dialog, color);
		HWND hStart = GetDlgItem(dialog, start);
		HWND hTeam = GetDlgItem(dialog, team);
		HWND hFlag = GetDlgItem(dialog, flag);
		ShowWindow(hId, SW_SHOW);
		ShowWindow(hCountry, SW_SHOW);
		ShowWindow(hColor, SW_SHOW);
		ShowWindow(hStart, SW_SHOW);
		ShowWindow(hTeam, SW_SHOW);
		ShowWindow(hFlag, SW_SHOW);
		FUN_006ADC20(dialog, id);
	}
}

// FUNCTION: 0x006AE080
void __fastcall FUN_006AE080(HWND dialog, int first)
{
	int i;
	for (i = first; i < 8; i++) {
		int id = FUN_006ACB60(i - 1);
		int count = SendDlgItemMessage(dialog, id, CB_GETCOUNT, 0, 0);
		for (int k = 0; k < count; k++) {
			if (SendDlgItemMessage(dialog, id, CB_GETITEMDATA, k, 0) == -1) {
				SendDlgItemMessage(dialog, id, CB_SETCURSEL, k, 0);
				break;
			}
		}
		FUN_006ADC20(dialog, id);
	}
	for (i = first; i < 8; i++) {
		int id = FUN_006ACB60(i - 1);
		int a = FUN_004E3320(i);
		int b = FUN_004E37D0(i);
		int c = FUN_004E41D0(i);
		int d = FUN_004E4E60(i);
		int e = FUN_004E5940(i);
		HWND hId = GetDlgItem(dialog, id);
		HWND hA = GetDlgItem(dialog, a);
		HWND hB = GetDlgItem(dialog, b);
		HWND hC = GetDlgItem(dialog, c);
		HWND hD = GetDlgItem(dialog, d);
		HWND hE = GetDlgItem(dialog, e);
		ShowWindow(hId, SW_HIDE);
		ShowWindow(hA, SW_HIDE);
		ShowWindow(hB, SW_HIDE);
		ShowWindow(hC, SW_HIDE);
		ShowWindow(hD, SW_HIDE);
		ShowWindow(hE, SW_HIDE);
	}
}

class Surface;

BOOL CALLBACK FUN_006AE3F0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void FUN_0072CF40(void);
void FUN_0072CF90(void);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);

namespace UI {
	HWND __fastcall BeginDialog(int resource, DLGPROC proc, int param);
	void __fastcall FocusOnWindow(HWND window);
	void __fastcall EndDialog(HWND window);
	bool Updated(void);
}

// FUNCTION: 0x006AE2C0
bool FUN_006AE2C0(void)
{
	int result = -1;

	Rule->Do_HouseTypes(*RuleINI);
	Rule->Do_Sides(*RuleINI);
	for (int i = 0; i < HouseTypes.Count(); i++) {
		HouseTypes[i]->Read_INI(*RuleINI);
	}

	FUN_0072CF40();

	HWND dialog = UI::BeginDialog(0x102, FUN_006AE3F0, 0);
	if (dialog) {
		DAT_00B0B59C = dialog;
		SetWindowLong(dialog, DWL_USER, (LONG)&result);
		UI::FocusOnWindow(dialog);
		while (result != 0x617 && result != 0x5c0) {
			if (UI::Updated() == 1) {
				break;
			}
			Title_Screen_Restore(false);
		}
		UI::EndDialog(dialog);
		DAT_00B0B59C = NULL;
	}

	if (MultiplayerMapPreview != NULL) {
		delete MultiplayerMapPreview;
		MultiplayerMapPreview = NULL;
	}

	FUN_0072CF90();
	Session.Write_MultiPlayer_Settings();

	if (result == 0x5c0) {
		FUN_0052FEC0(NULL, true);
	}

	if (result == 0x617) return true;
	return false;
}
