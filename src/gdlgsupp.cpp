// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "stringtable.h"
#include "houstype.h"
#include "multiplayer.h"
#include "session.h"

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E94E0;
extern const double DAT_007E94E8;

// SYNTHETIC: GAMEMD 0x004E3130 SYMBOL
// _$E70
double DAT_008B3FA8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x004E3160 SYMBOL
// _$E73
double DAT_008B40B8 = DAT_008B3FA8 / DAT_007E94E0;

// SYNTHETIC: GAMEMD 0x004E3180 SYMBOL
// _$E76
double DAT_008B40A0 = DAT_007E1718 / DAT_008B40B8;

// SYNTHETIC: GAMEMD 0x004E31A0 SYMBOL
// _$E79
double DAT_008B4098 = DAT_007E1720 * DAT_007E94E8;

// SYNTHETIC: GAMEMD 0x004E31C0 SYMBOL
// _$E82
double DAT_008B3FB0 = DAT_007E1728 * DAT_007E94E8;

// SYNTHETIC: GAMEMD 0x004E31E0 SYMBOL
// _$E85
double DAT_008B3F28 = DAT_007E1730 * DAT_007E94E8;

// SYNTHETIC: GAMEMD 0x004E3200 SYMBOL
// _$E88
int DAT_008B40C0 = (int)(fastmath::tan(DAT_008B3F28 - DAT_008B3FB0) * DAT_007E1738 * DAT_008B3FA8);

// SYNTHETIC: GAMEMD 0x004E3240 SYMBOL
// _$E91
double DAT_008B3FB8 = fastmath::atan(DAT_008B40C0 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x004E3270 SYMBOL
// _$E94
double DAT_008B4030 = fastmath::atan((DAT_008B40C0 * 2) / DAT_008B3FA8);

// SYNTHETIC: GAMEMD 0x004E32A0 SYMBOL
// _$E97
int DAT_008B40B4 = (int)(DAT_008B40C0 * 4 + DAT_007E1738);

struct GDShortPair { short a; short b; GDShortPair(void) { a = 0; b = 0; } };
struct GDIntTriple { int a; int b; int c; GDIntTriple(void) { a = 0; b = 0; c = 0; } };

// SYNTHETIC: GAMEMD 0x004E32D0 SYMBOL
// _$E100
GDShortPair DAT_008B3FA0;

// SYNTHETIC: GAMEMD 0x004E32E0 SYMBOL
// _$E103
GDIntTriple DAT_008B4020;

struct PCX
{
	static PCX Instance;
	int GetSurface(char const *name, void *palette);
};

extern int DAT_008B40B0;
extern int DAT_008B40A8;
extern int DAT_008B40AC;
extern char *DAT_00844AE8;
extern char *DAT_00844AEC;
extern char *DAT_00844AF0;
extern char *DAT_00844AF4;
extern char *DAT_00844AF8;
extern char *DAT_00844AFC;
extern char *DAT_00844B00;
extern char *DAT_00844B04;
extern char *DAT_00844B08;
extern char *DAT_00844B0C;
extern char *DAT_00844B10;
extern char *DAT_00844B14;
extern char *DAT_00844B18;
extern char *DAT_00844B1C;
extern char *DAT_00844B20;

void __fastcall FUN_00603D30(HWND dialog, int value);
int __fastcall FUN_004E37D0(int side);

// FUNCTION: GAMEMD 0x004E3300
void FUN_004E3300(void)
{
	DAT_008B40B0 = 0;
	DAT_008B40A8 = 0x80;
	DAT_008B40AC = 0x80;
}

// FUNCTION: GAMEMD 0x004E3320
int __fastcall FUN_004E3320(int side)
{
	if (side == 0) return 0x6da;
	if (side == 1) return 0x6db;
	if (side == 2) return 0x6dc;
	if (side == 3) return 0x6dd;
	if (side == 4) return 0x6de;
	if (side == 5) return 0x6df;
	if (side == 6) return 0x6e0;
	if (side == 7) return 0x6e1;
	return -1;
}

// FUNCTION: GAMEMD 0x004E3380
int __fastcall FUN_004E3380(int id)
{
	if (id == 0x6da) return 0;
	if (id == 0x6db) return 1;
	if (id == 0x6dc) return 2;
	if (id == 0x6dd) return 3;
	if (id == 0x6de) return 4;
	if (id == 0x6df) return 5;
	if (id == 0x6e0) return 6;
	if (id == 0x6e1) return 7;
	return -1;
}

// FUNCTION: GAMEMD 0x004E33F0
int __fastcall FUN_004E33F0(int index)
{
	int result = 0;
	if (index == 0) {
		result = PCX::Instance.GetSurface(DAT_00844B18, NULL);
	} else if (index == 1) {
		result = PCX::Instance.GetSurface(DAT_00844B1C, NULL);
	} else if (index == 2) {
		result = PCX::Instance.GetSurface(DAT_00844B20, NULL);
	}
	return result;
}

// FUNCTION: GAMEMD 0x004E3440
int __fastcall FUN_004E3440(int side)
{
	if (side == 0) return 0x754;
	if (side == 1) return 0x755;
	if (side == 2) return 0x756;
	if (side == 3) return 0x757;
	if (side == 4) return 0x758;
	if (side == 5) return 0x759;
	if (side == 6) return 0x75a;
	if (side == 7) return 0x75b;
	return -1;
}

// FUNCTION: GAMEMD 0x004E34A0
void __fastcall FUN_004E34A0(HWND dialog, int side, int flag)
{
	HWND item = GetDlgItem(dialog, FUN_004E3440(side));
	int surface = FUN_004E33F0(flag);
	if (item != (HWND)-1) {
		FUN_00603D30(item, surface);
	}
}

// FUNCTION: GAMEMD 0x004E3560
int __fastcall FUN_004E3560(int index)
{
	int result = 0;
	if (index == -2) {
		result = PCX::Instance.GetSurface(DAT_00844AE8, NULL);
	} else if (index == 0) {
		result = PCX::Instance.GetSurface(DAT_00844AF0, NULL);
	} else if (index == 1) {
		result = PCX::Instance.GetSurface(DAT_00844AF4, NULL);
	} else if (index == 2) {
		result = PCX::Instance.GetSurface(DAT_00844AF8, NULL);
	} else if (index == 3) {
		result = PCX::Instance.GetSurface(DAT_00844AFC, NULL);
	} else if (index == 4) {
		result = PCX::Instance.GetSurface(DAT_00844B00, NULL);
	} else if (index == 5) {
		result = PCX::Instance.GetSurface(DAT_00844B04, NULL);
	} else if (index == 6) {
		result = PCX::Instance.GetSurface(DAT_00844B08, NULL);
	} else if (index == 7) {
		result = PCX::Instance.GetSurface(DAT_00844B0C, NULL);
	} else if (index == 8) {
		result = PCX::Instance.GetSurface(DAT_00844B10, NULL);
	} else if (index == -3) {
		result = PCX::Instance.GetSurface(DAT_00844AEC, NULL);
	} else if (index == 9) {
		result = PCX::Instance.GetSurface(DAT_00844B14, NULL);
	}
	return result;
}

// FUNCTION: GAMEMD 0x004E3690
void __fastcall FUN_004E3690(HWND dialog, int side)
{
	HWND item = GetDlgItem(dialog, FUN_004E3320(side));
	int combo = FUN_004E37D0(side);
	int selection = SendDlgItemMessage(dialog, combo, CB_GETCURSEL, 0, 0);
	selection = SendDlgItemMessage(dialog, combo, CB_GETITEMDATA, selection, 0);
	if (selection < -3 || selection > 9) {
		if (Session.MPGameMode) {
			selection = Session.MPGameMode->vt_entry_28();
		} else {
			selection = -2;
		}
	}
	FUN_00603D30(item, FUN_004E3560(selection));
}

// FUNCTION: GAMEMD 0x004E37D0
int __fastcall FUN_004E37D0(int side)
{
	if (side == 0) return 0x6a1;
	if (side == 1) return 0x510;
	if (side == 2) return 0x513;
	if (side == 3) return 0x51e;
	if (side == 4) return 0x514;
	if (side == 5) return 0x51f;
	if (side == 6) return 0x520;
	if (side == 7) return 0x521;
	return -1;
}

// FUNCTION: GAMEMD 0x004E3830
int __fastcall FUN_004E3830(int id)
{
	if (id == 0x6a1) return 0;
	if (id == 0x510) return 1;
	if (id == 0x513) return 2;
	if (id == 0x51e) return 3;
	if (id == 0x514) return 4;
	if (id == 0x51f) return 5;
	if (id == 0x520) return 6;
	if (id == 0x521) return 7;
	return -1;
}

// FUNCTION: GAMEMD 0x004E38A0
wchar_t const * __fastcall FUN_004E38A0(int side)
{
	if (side == -2) return StringTable::LoadString("STT:PlayerSideRandom", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xed);
	if (side == -3) return StringTable::LoadString("STT:PlayerSideObserver", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xef);
	if (side == 0) return StringTable::LoadString("STT:PlayerSideAmerica", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xf1);
	if (side == 1) return StringTable::LoadString("STT:PlayerSideKorea", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xf3);
	if (side == 2) return StringTable::LoadString("STT:PlayerSideFrance", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xf5);
	if (side == 3) return StringTable::LoadString("STT:PlayerSideGermany", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xf7);
	if (side == 4) return StringTable::LoadString("STT:PlayerSideBritain", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xf9);
	if (side == 5) return StringTable::LoadString("STT:PlayerSideLibya", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xfb);
	if (side == 6) return StringTable::LoadString("STT:PlayerSideIraq", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xfd);
	if (side == 7) return StringTable::LoadString("STT:PlayerSideCuba", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0xff);
	if (side == 8) return StringTable::LoadString("STT:PlayerSideRussia", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x101);
	if (side == 9) return StringTable::LoadString("STT:PlayerSideYuriCountry", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x103);
	return NULL;
}

// FUNCTION: GAMEMD 0x004E3A00
void __fastcall FUN_004E3A00(HWND dialog, int id)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	SendMessage(item, CB_RESETCONTENT, 0, 0);
	SendMessage(item, 0x4de, 0, 7);
	int index = SendMessage(item, 0x4c2, 0, (LPARAM)StringTable::LoadString("GUI:RandomEx", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x114));
	SendMessage(item, CB_SETITEMDATA, index, -2);
	int count = HouseTypeClass::Array.Count();
	for (int i = 0; i < count; i++) {
		HouseTypeClass *type = HouseTypeClass::Array[i];
		if (type && type->IsMultiplay && type->House > -3 && type->House <= 9 && *type->UIName) {
			index = SendMessage(item, 0x4c2, 0, (LPARAM)type->UIName);
			SendMessage(item, CB_SETITEMDATA, index, type->House);
		}
	}
	SendMessage(item, 0x4f1, 0, 0);
	SendMessage(item, CB_SETCURSEL, 0, 0);
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
}

// FUNCTION: GAMEMD 0x004E3B00
void __fastcall FUN_004E3B00(HWND dialog, int id)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	SendMessage(item, CB_RESETCONTENT, 0, 0);
	SendMessage(item, 0x4de, 0, 7);
	int index = SendMessage(item, 0x4c2, 0, (LPARAM)StringTable::LoadString("GUI:Observer", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x135));
	SendMessage(item, CB_SETITEMDATA, index, -3);
	SendMessage(item, CB_SETCURSEL, 0, 0);
	SendMessage(item, 0x4f1, 0, 1);
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
}

extern NodeNameType * DAT_00AC11B4;
int __fastcall FUN_005E9B20(HWND dialog, int slot, int arg);

// FUNCTION: GAMEMD 0x004E3B90
void __fastcall FUN_004E3B90(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		NodeNameType * owner = (NodeNameType *)FUN_005E9B20(dialog, i, -1);
		bool mine = false;
		if (DAT_00AC11B4 && owner == DAT_00AC11B4) {
			mine = true;
		}
		int id = FUN_004E37D0(i);
		if (mine) {
			FUN_004E3B00(dialog, id);
		} else {
			FUN_004E3A00(dialog, id);
		}
	}
}


// FUNCTION: GAMEMD 0x004E3CE0
void __fastcall FUN_004E3CE0(HWND dialog, int side)
{
	int wanted = Session.Side;
	if (Session.Side2 == -2) {
		wanted = Session.Side2;
	} else if (Session.Side2 == -3) {
		wanted = Session.Side2;
	}
	int combo = FUN_004E37D0(side);
	HWND item = GetDlgItem(dialog, combo);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	int count = SendDlgItemMessage(dialog, combo, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++) {
		if (SendDlgItemMessage(dialog, combo, CB_GETITEMDATA, i, 0) == wanted) {
			SendDlgItemMessage(dialog, combo, CB_SETCURSEL, i, 0);
			break;
		}
	}
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
	FUN_004E3690(dialog, FUN_004E3830(combo));
}

// FUNCTION: GAMEMD 0x004E3F70
void __fastcall FUN_004E3F70(HWND dialog, int combo, int wanted)
{
	HWND item = GetDlgItem(dialog, combo);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	int count = SendDlgItemMessage(dialog, combo, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++) {
		if (SendDlgItemMessage(dialog, combo, CB_GETITEMDATA, i, 0) == wanted) {
			SendDlgItemMessage(dialog, combo, CB_SETCURSEL, i, 0);
			break;
		}
	}
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
	FUN_004E3690(dialog, FUN_004E3830(combo));
}

// FUNCTION: GAMEMD 0x004E4170
int __fastcall FUN_004E4170(HWND dialog, int combo, int selection)
{
	if (selection == -1) {
		selection = SendDlgItemMessage(dialog, combo, CB_GETCURSEL, 0, 0);
	}
	selection = SendDlgItemMessage(dialog, combo, CB_GETITEMDATA, selection, 0);
	if (selection < -3 || selection > 9) {
		if (Session.MPGameMode) {
			selection = Session.MPGameMode->vt_entry_28();
		} else {
			selection = -2;
		}
	}
	return selection;
}

// FUNCTION: GAMEMD 0x004E41D0
int __fastcall FUN_004E41D0(int side)
{
	if (side == 0) return 0x6a2;
	if (side == 1) return 0x522;
	if (side == 2) return 0x523;
	if (side == 3) return 0x524;
	if (side == 4) return 0x525;
	if (side == 5) return 0x526;
	if (side == 6) return 0x527;
	if (side == 7) return 0x528;
	return -1;
}

// FUNCTION: GAMEMD 0x004E4230
int __fastcall FUN_004E4230(int id)
{
	if (id == 0x6a2) return 0;
	if (id == 0x522) return 1;
	if (id == 0x523) return 2;
	if (id == 0x524) return 3;
	if (id == 0x525) return 4;
	if (id == 0x526) return 5;
	if (id == 0x527) return 6;
	if (id == 0x528) return 7;
	return -1;
}

// FUNCTION: GAMEMD 0x004E42A0
wchar_t const * __fastcall FUN_004E42A0(int color)
{
	if (color == -2) return StringTable::LoadString("STT:PlayerColorRandom", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1c1);
	if (color == 0) return StringTable::LoadString("STT:PlayerColorGold", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1c3);
	if (color == 1) return StringTable::LoadString("STT:PlayerColorRed", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1c5);
	if (color == 2) return StringTable::LoadString("STT:PlayerColorBlue", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1c7);
	if (color == 3) return StringTable::LoadString("STT:PlayerColorGreen", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1c9);
	if (color == 4) return StringTable::LoadString("STT:PlayerColorOrange", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1cb);
	if (color == 5) return StringTable::LoadString("STT:PlayerColorSkyBlue", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1cd);
	if (color == 6) return StringTable::LoadString("STT:PlayerColorPurple", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1cf);
	if (color == 7) return StringTable::LoadString("STT:PlayerColorPink", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1d1);
	if (color == 8) return StringTable::LoadString("STT:PlayerColorObserver", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1d3);
	return NULL;
}

struct GDColorEntry { wchar_t const *Abbrev; int Value; int Owner; };
extern GDColorEntry DAT_008B4038[9];
extern int DAT_008316A8[9];

// FUNCTION: GAMEMD 0x004E43C0
void FUN_004E43C0(void)
{
	DAT_008B4038[0].Abbrev = StringTable::LoadString("GUI:GoldAbbrev", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1db);
	DAT_008B4038[1].Abbrev = StringTable::LoadString("GUI:RedAbbrev", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1dc);
	DAT_008B4038[2].Abbrev = StringTable::LoadString("GUI:BlueAbbrev", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1dd);
	DAT_008B4038[3].Abbrev = StringTable::LoadString("GUI:GreenAbbrev", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1de);
	DAT_008B4038[4].Abbrev = StringTable::LoadString("GUI:OrangeAbbrev", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1df);
	DAT_008B4038[5].Abbrev = StringTable::LoadString("GUI:SkyBlueAbbrev", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1e0);
	DAT_008B4038[6].Abbrev = StringTable::LoadString("GUI:PurpleAbbrev", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1e1);
	DAT_008B4038[7].Abbrev = StringTable::LoadString("GUI:PinkAbbrev", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1e2);
	DAT_008B4038[8].Abbrev = StringTable::LoadString("GUI:GrayAbbrev", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x1e3);
	for (int i = 0; i < 9; i++) {
		DAT_008B4038[i].Value = DAT_008316A8[i];
		DAT_008B4038[i].Owner = -1;
	}
}

// FUNCTION: GAMEMD 0x004E44E0
int __fastcall FUN_004E44E0(int id)
{
	int color = FUN_004E4230(id);
	for (int i = 0; i < 9; i++) {
		if (DAT_008B4038[i].Owner == color) return i;
	}
	return -1;
}

// FUNCTION: GAMEMD 0x004E4580
bool __fastcall FUN_004E4580(int index)
{
	return DAT_008B4038[index].Owner != -1;
}

// FUNCTION: GAMEMD 0x004E45A0
void __fastcall FUN_004E45A0(HWND dialog, int id)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	SendMessage(item, CB_RESETCONTENT, 0, 0);
	SendMessage(item, 0x4dd, 0, 1);
	SendMessage(item, 0x4de, 0, 9);
	int index = SendMessage(item, 0x4c2, 0, (LPARAM)StringTable::LoadString("GUI:RandomAsSymbols", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x20a));
	SendMessage(item, CB_SETITEMDATA, index, -2);
	SendMessage(item, 0x498, index, -1);
	int owner = FUN_004E4230(id);
	int selected = 0;
	for (int i = 0; i < 8; i++) {
		bool mine = DAT_008B4038[i].Owner == owner;
		if (mine || DAT_008B4038[i].Owner == -1) {
			index = SendMessage(item, 0x4c2, 0, (LPARAM)"ab");
			SendMessage(item, 0x498, index, DAT_008B4038[i].Value);
			SendMessage(item, CB_SETITEMDATA, index, i);
			if (mine) {
				selected = index;
			}
		}
	}
	SendMessage(item, CB_SETCURSEL, selected, 0);
	SendMessage(item, 0x4f1, 0, 0);
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
}


// FUNCTION: GAMEMD 0x004E4770
void __fastcall FUN_004E4770(HWND dialog, int id)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	SendMessage(item, CB_RESETCONTENT, 0, 0);
	SendMessage(item, 0x4dd, 0, 1);
	SendMessage(item, 0x4de, 0, 9);
	int index = SendMessage(item, 0x4c2, 0, (LPARAM)StringTable::LoadString("GUI:RandomAsSymbols", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x237));
	SendMessage(item, CB_SETITEMDATA, index, -2);
	SendMessage(item, 0x498, index, -1);
	SendMessage(item, CB_SETCURSEL, 0, 0);
	SendMessage(item, 0x4f1, 0, 1);
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
}

// FUNCTION: GAMEMD 0x004E4820
void __fastcall FUN_004E4820(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int id = FUN_004E41D0(i);
		if ((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && (Session.Slots[i] == DAT_00AC11B4 || Session.Slots[i]->Slot.SpectatorFlag == -1)) {
			FUN_004E4770(dialog, id);
		} else {
			FUN_004E45A0(dialog, id);
		}
	}
}

void __fastcall FUN_004E49A0(HWND dialog, int id, int wanted);

// FUNCTION: GAMEMD 0x004E48E0
void __fastcall FUN_004E48E0(HWND dialog, int side)
{
	int wanted = Session.Color;
	if (Session.Color2 == -2) {
		wanted = -2;
	}
	int id = FUN_004E41D0(side);
	if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		if (Session.Slots[side] == DAT_00AC11B4 && Session.Slots[side]->Slot.SpectatorFlag == -1) {
			wanted = 8;
		}
	}
	FUN_004E49A0(dialog, id, wanted);
}

// FUNCTION: GAMEMD 0x004E49A0
void __fastcall FUN_004E49A0(HWND dialog, int id, int wanted)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	int owned = FUN_004E44E0(id);
	if (owned != -1) {
		DAT_008B4038[owned].Owner = -1;
	}
	int count = SendDlgItemMessage(dialog, id, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++) {
		int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, i, 0);
		if (data == wanted) {
			SendDlgItemMessage(dialog, id, CB_SETCURSEL, i, 0);
			if (data != -2) {
				DAT_008B4038[data].Owner = FUN_004E4230(id);
			}
			break;
		}
	}
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
	FUN_004E4820(dialog);
}

// FUNCTION: GAMEMD 0x004E4C20
void __fastcall FUN_004E4C20(HWND dialog, int id)
{
	int owned = FUN_004E44E0(id);
	if (owned != -1) {
		DAT_008B4038[owned].Owner = -1;
	}
	int index = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
	int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, index, 0);
	int color = FUN_004E4230(id);
	if (data != -2) {
		DAT_008B4038[data].Owner = color;
	}
	FUN_004E4820(dialog);
}

// FUNCTION: GAMEMD 0x004E4E20
int __fastcall FUN_004E4E20(HWND dialog, int id, int index)
{
	if (index == -1) {
		index = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
	}
	return SendDlgItemMessage(dialog, id, CB_GETITEMDATA, index, 0);
}

// FUNCTION: GAMEMD 0x004E4E60
int __fastcall FUN_004E4E60(int slot)
{
	if (slot == 0) return 0x6a3;
	if (slot == 1) return 0x6a4;
	if (slot == 2) return 0x6a5;
	if (slot == 3) return 0x6a6;
	if (slot == 4) return 0x6a7;
	if (slot == 5) return 0x6a8;
	if (slot == 6) return 0x6aa;
	if (slot == 7) return 0x6ab;
	return -1;
}

// FUNCTION: GAMEMD 0x004E4EC0
int __fastcall FUN_004E4EC0(int id)
{
	if (id == 0x6a3) return 0;
	if (id == 0x6a4) return 1;
	if (id == 0x6a5) return 2;
	if (id == 0x6a6) return 3;
	if (id == 0x6a7) return 4;
	if (id == 0x6a8) return 5;
	if (id == 0x6aa) return 6;
	if (id == 0x6ab) return 7;
	return -1;
}

// FUNCTION: GAMEMD 0x004E4F30
wchar_t const * FUN_004E4F30(void)
{
	return StringTable::LoadString("STT:HostComboStart", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x2eb);
}

struct GDNameEntry { char const *Name; int Value; int Owner; };
extern GDNameEntry DAT_008B3F30[9];

// FUNCTION: GAMEMD 0x004E4F50
void FUN_004E4F50(void)
{
	DAT_008B3F30[0].Name = "1";
	DAT_008B3F30[1].Name = "2";
	DAT_008B3F30[2].Name = "3";
	DAT_008B3F30[3].Name = "4";
	DAT_008B3F30[4].Name = "5";
	DAT_008B3F30[5].Name = "6";
	DAT_008B3F30[6].Name = "7";
	DAT_008B3F30[7].Name = "8";
	DAT_008B3F30[8].Name = "0";
	for (int i = 0; i < 9; i++) {
		DAT_008B3F30[i].Owner = -1;
	}
}

// FUNCTION: GAMEMD 0x004E4FC0
void __fastcall FUN_004E4FC0(int count)
{
	for (int i = 0; i < count; i++) {
		DAT_008B3F30[i].Owner = -1;
	}
	for (int j = count; j < 9; j++) {
		DAT_008B3F30[j].Owner = -2;
	}
}

// FUNCTION: GAMEMD 0x004E5000
int __fastcall FUN_004E5000(int id)
{
	int slot = FUN_004E4EC0(id);
	for (int i = 0; i < 9; i++) {
		if (DAT_008B3F30[i].Owner == slot) return i;
	}
	return -1;
}

// FUNCTION: GAMEMD 0x004E50A0
bool __fastcall FUN_004E50A0(int slot)
{
	return DAT_008B3F30[slot].Owner != -1;
}

static inline int Add_Item(HWND item, char const *name, int data)
{
	int index = SendMessage(item, 0x4c2, 0, (LPARAM)name);
	SendMessage(item, CB_SETITEMDATA, index, data);
	return index;
}

// FUNCTION: GAMEMD 0x004E50C0
void __fastcall FUN_004E50C0(HWND dialog, int id)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	SendMessage(item, CB_RESETCONTENT, 0, 0);
	SendMessage(item, 0x4dd, 0, 1);
	SendMessage(item, 0x4de, 0, 9);
	int index = SendMessage(item, 0x4c2, 0, (LPARAM)StringTable::LoadString("GUI:RandomAsSymbols", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x341));
	SendMessage(item, CB_SETITEMDATA, index, -2);
	int owner = FUN_004E4EC0(id);
	int selected = 0;
	for (int i = 0; i < 8; i++) {
		bool mine = DAT_008B3F30[i].Owner == owner;
		if (mine || DAT_008B3F30[i].Owner == -1) {
			int pos = Add_Item(item, DAT_008B3F30[i].Name, i);
			if (mine) {
				selected = pos;
			}
		}
	}
	SendMessage(item, CB_SETCURSEL, selected, 0);
	SendMessage(item, 0x4f1, 0, 0);
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
}

// FUNCTION: GAMEMD 0x004E5260
void __fastcall FUN_004E5260(HWND dialog, int id)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	SendMessage(item, CB_RESETCONTENT, 0, 0);
	SendMessage(item, 0x4dd, 0, 1);
	SendMessage(item, 0x4de, 0, 9);
	int index = SendMessage(item, 0x4c2, 0, (LPARAM)StringTable::LoadString("GUI:RandomAsSymbols", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x36c));
	SendMessage(item, CB_SETITEMDATA, index, -2);
	SendMessage(item, 0x498, index, -1);
	SendMessage(item, CB_SETCURSEL, 0, 0);
	SendMessage(item, 0x4f1, 0, 1);
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
}

// FUNCTION: GAMEMD 0x004E5310
void __fastcall FUN_004E5310(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int id = FUN_004E4E60(i);
		if ((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && (Session.Slots[i] == DAT_00AC11B4 || Session.Slots[i]->Slot.SpectatorFlag == -1)) {
			FUN_004E5260(dialog, id);
		} else {
			FUN_004E50C0(dialog, id);
		}
	}
}

void __fastcall FUN_004E5480(HWND dialog, int id, int wanted);

// FUNCTION: GAMEMD 0x004E53D0
void __fastcall FUN_004E53D0(HWND dialog, int slot)
{
	int wanted = -2;
	int id = FUN_004E4E60(slot);
	if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		if (Session.Slots[slot] == DAT_00AC11B4 && Session.Slots[slot]->Slot.SpectatorFlag == -1) {
			wanted = 8;
		}
	}
	FUN_004E5480(dialog, id, wanted);
}

// FUNCTION: GAMEMD 0x004E5480
void __fastcall FUN_004E5480(HWND dialog, int id, int wanted)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	int owned = FUN_004E5000(id);
	if (owned != -1) {
		DAT_008B3F30[owned].Owner = -1;
	}
	int count = SendDlgItemMessage(dialog, id, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++) {
		int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, i, 0);
		if (data == wanted) {
			SendDlgItemMessage(dialog, id, CB_SETCURSEL, i, 0);
			if (data != -2) {
				DAT_008B3F30[data].Owner = FUN_004E4EC0(id);
			}
			break;
		}
	}
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
	FUN_004E5310(dialog);
}

// FUNCTION: GAMEMD 0x004E5700
void __fastcall FUN_004E5700(HWND dialog, int id)
{
	int owned = FUN_004E5000(id);
	if (owned != -1) {
		DAT_008B3F30[owned].Owner = -1;
	}
	int index = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
	int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, index, 0);
	int slot = FUN_004E4EC0(id);
	if (data != -2) {
		DAT_008B3F30[data].Owner = slot;
	}
	FUN_004E5310(dialog);
}

// FUNCTION: GAMEMD 0x004E5900
int __fastcall FUN_004E5900(HWND dialog, int id, int index)
{
	if (index == -1) {
		index = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
	}
	return SendDlgItemMessage(dialog, id, CB_GETITEMDATA, index, 0);
}

// FUNCTION: GAMEMD 0x004E5940
int __fastcall FUN_004E5940(int slot)
{
	if (slot == 0) return 0x76d;
	if (slot == 1) return 0x76e;
	if (slot == 2) return 0x76f;
	if (slot == 3) return 0x770;
	if (slot == 4) return 0x771;
	if (slot == 5) return 0x772;
	if (slot == 6) return 0x773;
	if (slot == 7) return 0x774;
	return -1;
}

// FUNCTION: GAMEMD 0x004E59A0
int __fastcall FUN_004E59A0(int id)
{
	if (id == 0x76d) return 0;
	if (id == 0x76e) return 1;
	if (id == 0x76f) return 2;
	if (id == 0x770) return 3;
	if (id == 0x771) return 4;
	if (id == 0x772) return 5;
	if (id == 0x773) return 6;
	if (id == 0x774) return 7;
	return -1;
}

// FUNCTION: GAMEMD 0x004E5A10
wchar_t const * __fastcall FUN_004E5A10(int color)
{
	if (color == -2) return StringTable::LoadString("STT:PlayerColorRandom", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x422);
	if (color == 0) return StringTable::LoadString("STT:PlayerColorGold", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x424);
	if (color == 1) return StringTable::LoadString("STT:PlayerColorRed", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x426);
	if (color == 2) return StringTable::LoadString("STT:PlayerColorBlue", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x428);
	if (color == 3) return StringTable::LoadString("STT:PlayerColorGreen", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x42a);
	if (color == 4) return StringTable::LoadString("STT:PlayerColorObserver", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x42c);
	return NULL;
}

struct GDLetterEntry { void const *Text; int Value; int Owner; };
extern GDLetterEntry DAT_008B3FC0[5];

// FUNCTION: GAMEMD 0x004E5AC0
void FUN_004E5AC0(void)
{
	DAT_008B3FC0[0].Text = StringTable::LoadString("LETTER_A", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x437);
	DAT_008B3FC0[1].Text = StringTable::LoadString("LETTER_B", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x438);
	DAT_008B3FC0[2].Text = StringTable::LoadString("LETTER_C", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x439);
	DAT_008B3FC0[3].Text = StringTable::LoadString("LETTER_D", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x43a);
	DAT_008B3FC0[4].Text = "0";
}

// FUNCTION: GAMEMD 0x004E5B40
bool __fastcall FUN_004E5B40(int slot)
{
	return DAT_008B3FC0[slot].Owner != -1;
}

MPGameModeClass * FUN_005E2F80(void);
bool __cdecl FUN_0077D970(void);

// FUNCTION: GAMEMD 0x004E5B60
void __fastcall FUN_004E5B60(HWND dialog, int id)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	SendMessage(item, CB_RESETCONTENT, 0, 0);
	SendMessage(item, 0x4dd, 0, 1);
	SendMessage(item, 0x4de, 0, 9);
	if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		if (Session.MPGameMode && Session.MPGameMode->vt_entry_2C() < 0) {
			Add_Item(item, (char const *)StringTable::LoadString("GUI:NoneAsSymbols", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x456), -2);
		}
	} else {
		MPGameModeClass * mode = FUN_005E2F80();
		if (mode && mode->vt_entry_2C() < 0) {
			Add_Item(item, (char const *)StringTable::LoadString("GUI:NoneAsSymbols", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x45f), -2);
		}
	}
	for (int i = 0; i < 4; i++) {
		Add_Item(item, (char const *)DAT_008B3FC0[i].Text, i);
	}
	SendMessage(item, CB_SETCURSEL, 0, 0);
	SendMessage(item, 0x4f1, 0, 0);
	if (Session.Type == GAME_INTERNET && FUN_0077D970()) {
		ShowWindow(item, SW_HIDE);
		HWND other = GetDlgItem(dialog, 0x794);
		ShowWindow(other, SW_HIDE);
		return;
	}
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
}

// FUNCTION: GAMEMD 0x004E5CB0
void __fastcall FUN_004E5CB0(HWND dialog, int id)
{
	HWND item = GetDlgItem(dialog, id);
	bool was_hidden = !ShowWindow(item, SW_HIDE);
	SendMessage(item, CB_RESETCONTENT, 0, 0);
	SendMessage(item, 0x4dd, 0, 1);
	SendMessage(item, 0x4de, 0, 9);
	int index = Add_Item(item, (char const *)StringTable::LoadString("GUI:NoneAsSymbols", NULL, "D:\\ra2mdpost\\GDlgSupp.cpp", 0x48d), -2);
	SendMessage(item, 0x498, index, -1);
	SendMessage(item, CB_SETCURSEL, 0, 0);
	SendMessage(item, 0x4f1, 0, 1);
	if (!was_hidden) {
		ShowWindow(item, SW_SHOW);
	}
}
