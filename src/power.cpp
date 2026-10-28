// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x0063F4A0 SYMBOL
// _$E41

// SYNTHETIC: GAMEMD 0x0063F4C0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0063F4F0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0063F510 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0063F530 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0063F550 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0063F570 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0063F590 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0063F5D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0063F600 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0063F630 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0063F660 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0063F670 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0063F690 SYMBOL
// _$E38

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
#include "tooltip.h"
#include <stddef.h>
#include <wchar.h>
#include <windows.h>
#undef LoadString
#include "stringtable.h"

extern int DAT_00B0B504;
extern bool DAT_00AC4E78;
extern bool DAT_00884B8D;
extern bool DAT_00B0B518;
extern int DAT_00886F90;
extern int DAT_00886F94;
extern DSurface * DAT_00887300;
struct CCToolTip {
	static ToolTipManager * Instance;
};

void * __fastcall FileSystem_LoadWholeFileEx(char const * name, bool & loaded);

struct FileSystem {
	static ConvertClass * SIDEBAR_PAL;
};

void const * PowerClass::PowerPipShape;

// FUNCTION: GAMEMD 0x0063F6B0
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

// FUNCTION: GAMEMD 0x0063F730
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

// FUNCTION: GAMEMD 0x0063F7B0
void PowerClass::One_Time(void)
{
	BASECLASS::One_Time();
}

// FUNCTION: GAMEMD 0x0063F7C0
void PowerClass::Init_For_House(void)
{
	BASECLASS::Init_For_House();
	PowerPipShape = FileSystem_LoadWholeFileEx("POWERP.SHP", DAT_00AC4E78);
}

// FUNCTION: GAMEMD 0x0063F7E0
void PowerClass::DisposeOfArt(void)
{
	BASECLASS::DisposeOfArt();
	if (DAT_00AC4E78) {
		delete [] (char *)PowerPipShape;
		DAT_00AC4E78 = false;
	}
}

// FUNCTION: GAMEMD 0x0063F810
void PowerClass::Flash_Power(void)
{
	FlashCount = POWER_FLASH_COUNT;
	FlashTimer = POWER_FLASH_RATE;
}

// FUNCTION: GAMEMD 0x0063F850
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

// FUNCTION: GAMEMD 0x0063F8F0
int PowerClass::Update_Delay(void)
{
	int desired_pips = Desired_Power_Height();

	int current_pips = GreenPipCount + YellowPipCount + RedPipCount;
	if (current_pips > desired_pips) {
		current_pips = desired_pips;
	}

	return((double)current_pips / (double)desired_pips * 5.0);
}

// FUNCTION: GAMEMD 0x0063F940
int PowerClass::Max_Power_Height(void)
{
	return((DAT_00B0B504 + 3) / 3);
}

// FUNCTION: GAMEMD 0x0063FDC0
void PowerClass::Remove_Pip(void)
{
	int green;
	int red;
	int yellow;

	Desired_Levels(green, yellow, red);

	if (GreenPipCount > green) {
		GreenPipCount--;
	} else if (RedPipCount > red) {
		RedPipCount--;
	} else if (YellowPipCount > yellow) {
		YellowPipCount--;
	}
}

// FUNCTION: GAMEMD 0x0063FE30
void PowerClass::Add_Pip(void)
{
	int green;
	int red;
	int yellow;

	Desired_Levels(green, yellow, red);

	if (RedPipCount < red) {
		RedPipCount++;
	} else if (GreenPipCount < green) {
		GreenPipCount++;
	} else if (YellowPipCount < yellow) {
		YellowPipCount++;
	}
}

// FUNCTION: GAMEMD 0x0063FEA0
void PowerClass::AI(KeyNumType & input, Point2D const & xy)
{
	if (Map.IsSidebarActive) {

		if (!HasChanged && FlashCount > 0) {
			if (FlashTimer == 0) {
				IsToRedraw = true;
				FlashCount--;
				Map.SidebarClass::IsToRedraw = true;
				Map.Flag_To_Redraw();
				FlashTimer = POWER_FLASH_RATE;
			}
		}

		if (PlayerPtr->Power_Drain() != RecordedDrain || PlayerPtr->Power_Output() != RecordedPower || HasChanged) {

			IsToRedraw = true;
			Map.SidebarClass::IsToRedraw = true;
			Map.Flag_To_Redraw();

			if (PlayerPtr->Power_Drain() != RecordedDrain || PlayerPtr->Power_Output() != RecordedPower) {
				HasChanged = true;
				Flash_Power();
			}

			RecordedDrain = PlayerPtr->Power_Drain();
			RecordedPower = PlayerPtr->Power_Output();

			int green, yellow, red;
			Desired_Levels(green, yellow, red);

			if (UpdateTimer == 0) {
				HasChanged = false;

				if (RedPipCount != red) {
					HasChanged = true;

					if (RedPipCount > red) {
						RedPipCount--;
						Add_Pip();
					} else {
						RedPipCount++;
						Remove_Pip();
					}
				} else if (GreenPipCount != green) {
					HasChanged = true;

					if (GreenPipCount > green) {
						GreenPipCount--;
						Add_Pip();
					} else {
						GreenPipCount++;
						Remove_Pip();
					}
				} else if (YellowPipCount != yellow) {
					HasChanged = true;

					if (YellowPipCount > yellow) {
						YellowPipCount--;
						Add_Pip();
					} else {
						YellowPipCount++;
						Remove_Pip();
					}
				}

				if (HasChanged) {
					UpdateTimer = Update_Delay();
				}
			}
		}
	}

	BASECLASS::AI(input, xy);
}

// FUNCTION: GAMEMD 0x00640450
char const * PowerClass::Help_Text(int id)
{
	static wchar_t _str[128];

	if (id == GADGET_POWER) {
		swprintf(_str, StringTable::LoadString("TXT_POWER_DRAIN", NULL, "D:\\ra2mdpost\\Power.CPP", 670), PlayerPtr->Power, PlayerPtr->Drain);
		return((char const *)_str);
	}

	return(BASECLASS::Help_Text(id));
}

// SYNTHETIC: GAMEMD 0x006404B0
// ??_GPowerClass@@UAEPAXI@Z
