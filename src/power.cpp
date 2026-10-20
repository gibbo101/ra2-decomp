// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EFF40;
extern const double DAT_007EFF48;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0063F4C0 $E2
double DAT_00AC4D18 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0063F4F0 $E5
double DAT_00AC4E68 = DAT_00AC4D18 / DAT_007EFF40;

// FUNCTION: 0x0063F510 $E8
double DAT_00AC4E50 = DAT_007E1718 / DAT_00AC4E68;

// FUNCTION: 0x0063F530 $E11
double DAT_00AC4E48 = DAT_007E1720 * DAT_007EFF48;

// FUNCTION: 0x0063F550 $E14
double DAT_00AC4D20 = DAT_007E1728 * DAT_007EFF48;

// FUNCTION: 0x0063F570 $E17
double DAT_00AC4CF8 = DAT_007E1730 * DAT_007EFF48;

// FUNCTION: 0x0063F590 $E20
int DAT_00AC4E70 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00AC4CF8 - DAT_00AC4D20) * DAT_00AC4D18));

// FUNCTION: 0x0063F5D0 $E23
double DAT_00AC4D28 = FUN_004CADE0(DAT_00AC4E70 * DAT_007E1740);

// FUNCTION: 0x0063F600 $E26
double DAT_00AC4E40 = FUN_004CADE0((DAT_00AC4E70 * 2) / DAT_00AC4D18);

// FUNCTION: 0x0063F630 $E29
int DAT_00AC4E64 = (int)(DAT_00AC4E70 * 4 + DAT_007E1738);

// FUNCTION: 0x0063F660 $E32
ShortPair DAT_00AC4D10;

// FUNCTION: 0x0063F670 $E35
IntTriple DAT_00AC4E30;

struct PowerGlobalA {
	int A, B, C;
	PowerGlobalA() { C = 0; A = 128; B = 128; }
};

// FUNCTION: 0x0063F690 $E38
PowerGlobalA DAT_00AC4E58;

#include "rect.h"
#include "geometry.h"
#include "power.h"
#include "_convert.h"
#include "_map.h"
#include "_surface.h"
#include "building.h"
#include "builtype.h"
#include "data.h"
#include "draw.h"
#include "dsurface.h"
#include "globals.h"
#include "house.h"
#include "scenario.h"
#include "shapeset.h"
#include "surface.h"

extern int DAT_00B0B504;
extern bool DAT_00AC4E78;
extern bool DAT_00884B8D;
extern bool DAT_00B0B518;
extern int DAT_00886F94;
extern DSurface * DAT_00887300;
void * __fastcall FileSystem_LoadWholeFileEx(char const * name, bool & loaded);

struct FileSystem {
	static ConvertClass * SIDEBAR_PAL;
};

void const * PowerClass::PowerPipShape;

// FUNCTION: 0x0063F6B0
PowerClass::PowerClass(void) :
	BASECLASS(),
	IsToRedraw(false),
	FlashTimer(0),
	FlashCount(0),
	UpdateTimer(0),
	GreenPipCount(0),
	YellowPipCount(0),
	RedPipCount(0),
	HasChanged(0),
	RecordedDrain(-1),
	RecordedPower(-1)
{
}

// FUNCTION: 0x0063F730
void PowerClass::Init_Clear(void)
{
	BASECLASS::Init_Clear();
	RecordedDrain = -1;
	RecordedPower = -1;
	FlashTimer = 0;
	FlashCount = 0;
	UpdateTimer = 0;
	GreenPipCount = 0;
	YellowPipCount = 0;
	RedPipCount = 0;
	HasChanged = 0;
}

// FUNCTION: 0x0063F7B0
void PowerClass::One_Time(void)
{
	BASECLASS::One_Time();
}

// FUNCTION: 0x0063F7C0
void PowerClass::Init_For_House(void)
{
	BASECLASS::Init_For_House();
	PowerPipShape = FileSystem_LoadWholeFileEx("POWERP.SHP", DAT_00AC4E78);
}

// FUNCTION: 0x0063F7E0
void PowerClass::DisposeOfArt(void)
{
	BASECLASS::DisposeOfArt();
	if (DAT_00AC4E78) {
		delete [] (char *)PowerPipShape;
		DAT_00AC4E78 = false;
	}
}

// FUNCTION: 0x0063F810
void PowerClass::Flash_Power(void)
{
	FlashCount = POWER_FLASH_COUNT;
	FlashTimer = POWER_FLASH_RATE;
}

// FUNCTION: 0x0063F850
int PowerClass::Desired_Power_Height(void)
{
	int max_pips = Max_Power_Height();
	int unused = PlayerPtr->Buildings.Count() + PlayerPtr->Buildings.Count();
	int drain = 0;
	int power = 0;

	for (int i = 0; i < PlayerPtr->Buildings.Count(); i++) {
		BuildingClass * bptr = PlayerPtr->Buildings[i];
		drain += bptr->Class->Drain;
		power += bptr->Class->Power;
	}

	int empty_pips = (400.0 / ((drain + power) + 400.0) * max_pips);
	empty_pips = MAX(empty_pips, 0);
	empty_pips = MIN(empty_pips, max_pips - 1);

	return(max_pips - empty_pips);
}

// FUNCTION: 0x0063F8F0
int PowerClass::Update_Delay(void)
{
	int desired_pips = Desired_Power_Height();

	int current_pips = GreenPipCount + YellowPipCount + RedPipCount;
	if (current_pips > desired_pips) {
		current_pips = desired_pips;
	}

	return((double)current_pips / (double)desired_pips * 5.0);
}

// FUNCTION: 0x0063F940
int PowerClass::Max_Power_Height(void)
{
	return((DAT_00B0B504 + 3) / 3);
}
