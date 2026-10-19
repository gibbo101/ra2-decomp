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

// FUNCTION: 0x004E3130 $E5
double DAT_008B3FA8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004E3160 $E8
double DAT_008B40B8 = DAT_008B3FA8 / DAT_007E94E0;

// FUNCTION: 0x004E3180 $E11
double DAT_008B40A0 = DAT_007E1718 / DAT_008B40B8;

// FUNCTION: 0x004E31A0 $E14
double DAT_008B4098 = DAT_007E1720 * DAT_007E94E8;

// FUNCTION: 0x004E31C0 $E17
double DAT_008B3FB0 = DAT_007E1728 * DAT_007E94E8;

// FUNCTION: 0x004E31E0 $E20
double DAT_008B3F28 = DAT_007E1730 * DAT_007E94E8;

// FUNCTION: 0x004E3200 $E23
int DAT_008B40C0 = (int)(fastmath::tan(DAT_008B3F28 - DAT_008B3FB0) * DAT_007E1738 * DAT_008B3FA8);

// FUNCTION: 0x004E3240 $E26
double DAT_008B3FB8 = fastmath::atan(DAT_008B40C0 * DAT_007E1740);

// FUNCTION: 0x004E3270 $E29
double DAT_008B4030 = fastmath::atan((DAT_008B40C0 * 2) / DAT_008B3FA8);

// FUNCTION: 0x004E32A0 $E32
int DAT_008B40B4 = (int)(DAT_008B40C0 * 4 + DAT_007E1738);

struct GDShortPair { short a; short b; GDShortPair(void) { a = 0; b = 0; } };
struct GDIntTriple { int a; int b; int c; GDIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x004E32D0 $E35
GDShortPair DAT_008B3FA0;

// FUNCTION: 0x004E32E0 $E38
GDIntTriple DAT_008B4020;

struct PCX
{
	static PCX Instance;
	int GetSurface(char const *name, void *palette);
};

class GDVirtuals
{
	public:
		virtual void Slot0(void);
		virtual void Slot1(void);
		virtual void Slot2(void);
		virtual void Slot3(void);
		virtual void Slot4(void);
		virtual void Slot5(void);
		virtual void Slot6(void);
		virtual void Slot7(void);
		virtual void Slot8(void);
		virtual void Slot9(void);
		virtual int Selected(void);
};

extern int DAT_008B40B0;
extern int DAT_008B40A8;
extern int DAT_008B40AC;
extern GDVirtuals *DAT_00A8B23C;
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

// FUNCTION: 0x004E3300
void FUN_004E3300(void)
{
	DAT_008B40B0 = 0;
	DAT_008B40A8 = 0x80;
	DAT_008B40AC = 0x80;
}

// FUNCTION: 0x004E3320
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

// FUNCTION: 0x004E3380
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

// FUNCTION: 0x004E33F0
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

// FUNCTION: 0x004E3440
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

// FUNCTION: 0x004E34A0
void __fastcall FUN_004E34A0(HWND dialog, int side, int flag)
{
	HWND item = GetDlgItem(dialog, FUN_004E3440(side));
	int surface = FUN_004E33F0(flag);
	if (item != (HWND)-1) {
		FUN_00603D30(item, surface);
	}
}

// FUNCTION: 0x004E3560
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

// FUNCTION: 0x004E3690
void __fastcall FUN_004E3690(HWND dialog, int side)
{
	HWND item = GetDlgItem(dialog, FUN_004E3320(side));
	int combo = FUN_004E37D0(side);
	int selection = SendDlgItemMessage(dialog, combo, CB_GETCURSEL, 0, 0);
	selection = SendDlgItemMessage(dialog, combo, CB_GETITEMDATA, selection, 0);
	if (selection < -3 || selection > 9) {
		if (DAT_00A8B23C) {
			selection = DAT_00A8B23C->Selected();
		} else {
			selection = -2;
		}
	}
	FUN_00603D30(item, FUN_004E3560(selection));
}

// FUNCTION: 0x004E37D0
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

// FUNCTION: 0x004E3830
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

// FUNCTION: 0x004E38A0
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

// FUNCTION: 0x004E3A00
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

// FUNCTION: 0x004E3B00
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

extern int DAT_00AC11B4;
int __fastcall FUN_005E9B20(HWND dialog, int slot, int arg);

// FUNCTION: 0x004E3B90
void __fastcall FUN_004E3B90(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int owner = FUN_005E9B20(dialog, i, -1);
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

extern int DAT_00A8B3BC;
extern int DAT_00A8B3C0;

// FUNCTION: 0x004E3CE0
void __fastcall FUN_004E3CE0(HWND dialog, int side)
{
	int wanted = DAT_00A8B3BC;
	if (DAT_00A8B3C0 == -2) {
		wanted = DAT_00A8B3C0;
	} else if (DAT_00A8B3C0 == -3) {
		wanted = DAT_00A8B3C0;
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

// FUNCTION: 0x004E3F70
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

// FUNCTION: 0x004E4170
int __fastcall FUN_004E4170(HWND dialog, int combo, int selection)
{
	if (selection == -1) {
		selection = SendDlgItemMessage(dialog, combo, CB_GETCURSEL, 0, 0);
	}
	selection = SendDlgItemMessage(dialog, combo, CB_GETITEMDATA, selection, 0);
	if (selection < -3 || selection > 9) {
		if (DAT_00A8B23C) {
			selection = DAT_00A8B23C->Selected();
		} else {
			selection = -2;
		}
	}
	return selection;
}

// FUNCTION: 0x004E41D0
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

// FUNCTION: 0x004E4230
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

// FUNCTION: 0x004E42A0
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

// FUNCTION: 0x004E43C0
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

// FUNCTION: 0x004E44E0
int __fastcall FUN_004E44E0(int id)
{
	int color = FUN_004E4230(id);
	for (int i = 0; i < 9; i++) {
		if (DAT_008B4038[i].Owner == color) return i;
	}
	return -1;
}

// FUNCTION: 0x004E4580
bool __fastcall FUN_004E4580(int index)
{
	return DAT_008B4038[index].Owner != -1;
}

// FUNCTION: 0x004E45A0
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
