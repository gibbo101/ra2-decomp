// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "sidebar.h"
#include "_bench.h"
#include "_convert.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tooltip.h"
#include "bench.h"
#include "building.h"
#include "cctooltip.h"
#include "conquer.h"
#include "convert.h"
#include "data.h"
#include "dialog.h"
#include "draw.h"
#include "dsurface.h"
#include "event.h"
#include "factory.h"
#include "font.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "language\language.h"
#include "map.h"
#include "mixfile.h"
#include "movie.h"
#include "palette.h"
#include "queue.h"
#include "rect.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "scheme.h"
#include "shapeset.h"
#include "stringtable.h"
#include "super.h"
#include "suprtype.h"
#include "surface.h"
#include "voc.h"
#include "vox.h"
#include "bench.hh"
#include "color.hh"
#include <cmath>

// SYNTHETIC: GAMEMD 0x006A4A00 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006A4A20 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006A4A50 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006A4A70 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006A4A90 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006A4AB0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006A4AD0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006A4AF0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006A4B30 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006A4B60 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006A4B90 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006A4BC0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006A4BD0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006A4BF0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x006AACB0
SidebarClass::StripClass::SelectClass::SelectClass(void) :
	ControlClass(0, 0, 0, 60, 48, LEFTPRESS|RIGHTPRESS|LEFTUP),
	Strip(0),
	Index(0)
{
}

// SYNTHETIC: GAMEMD 0x006A4C10 SYMBOL
// _$E175
// SYNTHETIC: GAMEMD 0x006A4C40 SYMBOL
// _$E172
SidebarClass::SBGadgetClass SidebarClass::Background;

// FUNCTION: GAMEMD 0x006A4E40
SidebarClass::SBGadgetClass::SBGadgetClass(void) :
	GadgetClass(0, 0, 1, 1, LEFTUP)
{
}

// SYNTHETIC: GAMEMD 0x006A4C60 SYMBOL
// _$E181
// SYNTHETIC: GAMEMD 0x006A4C80 SYMBOL
// _$E178
ShapeButtonClass SidebarClass::Power;
// SYNTHETIC: GAMEMD 0x006A4CA0 SYMBOL
// _$E186
// SYNTHETIC: GAMEMD 0x006A4CC0 SYMBOL
// _$E184
ShapeButtonClass SidebarClass::Repair;
// SYNTHETIC: GAMEMD 0x006A4CE0 SYMBOL
// _$E191
// SYNTHETIC: GAMEMD 0x006A4D10 SYMBOL
// _$E189
ShapeButtonClass SidebarClass::TabButtons[4];
// SYNTHETIC: GAMEMD 0x006A4D40 SYMBOL
// _$E196
// SYNTHETIC: GAMEMD 0x006A4D60 SYMBOL
// _$E194
ShapeButtonClass SidebarClass::Upgrade;
// SYNTHETIC: GAMEMD 0x006A4D80 SYMBOL
// _$E201
// SYNTHETIC: GAMEMD 0x006A4DA0 SYMBOL
// _$E199
ShapeButtonClass SidebarClass::Waypoint;

// SYNTHETIC: GAMEMD 0x006A4DC0 SYMBOL
// _$E206
// SYNTHETIC: GAMEMD 0x006A4E10 SYMBOL
// _$E204
SidebarClass::StripClass::SelectClass SidebarClass::StripClass::SelectButton[SidebarClass::StripClass::SELECT_BUTTONS];

// FUNCTION: GAMEMD 0x006A4E60
SidebarClass::SidebarClass(void) :
	BASECLASS(),
	ActiveTabIndex(0),
	unknown_53A0(0),
	IsCameoText(false),
	IsSidebarActive(false),
	IsToRedraw(true),
	IsToRedrawCredits(false),
	unknown_bool_5514(false),
	unknown_bool_5515(false)
{
	for (int index = 0; index < STRIPS; index++) {
		new (&Column[index]) StripClass(InitClass());
	}
}

// FUNCTION: GAMEMD 0x006A4F20
SidebarClass::SidebarClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x006A4FF0
void SidebarClass::Set_Cameo_Text(bool state)
{
}


struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
	static void const * DARKEN_SHP;
	static ConvertClass * SIDEBAR_PAL;
	static void * GCLOCK2_SHP;
	static void * __fastcall LoadWholeFileEx(char const * name, bool & allocated);
};

// FUNCTION: GAMEMD 0x006A5000
void SidebarClass::One_Time(void)
{
	BASECLASS::One_Time();

	for (int index = 0; index < STRIPS; index++) {
		Column[index].One_Time(index);
	}
}

// FUNCTION: GAMEMD 0x006A5030
void SidebarClass::Init_Clear(void)
{
	BASECLASS::Init_Clear();

	IsToRedraw = true;
	unknown_bool_5515 = false;
	unknown_bool_5514 = false;

	for (int index = 0; index < STRIPS; index++) {
		Column[index].Init_Clear();
	}

	ActiveTabIndex = 0;
	unknown_53A0 = 0;
	Activate(false);
}

extern int DAT_00886F94;
extern int DAT_00B0B4E0;
extern int DAT_00B0B4E4;
extern int DAT_00B0B4EC;
extern int DAT_00B0B4F0;
extern int DAT_00B0B4F8;
extern int DAT_00B0B4FC;
extern int DAT_00B0B500;

// FUNCTION: GAMEMD 0x006A5090
void SidebarClass::FUN_006A5090(void)
{
	int side = Scen->PlayerSideIndex;
	int span = DAT_00886F94 * 3 + side * 5 + DAT_00886F94 * 7 + side * 9 + DAT_00886F94 * 11 + side * 13;

	if (side == 0) {
		int y = DAT_00886F94;
		DAT_00B0B4E4 = 0x40;
		DAT_00B0B4F0 = 0x1d;
		DAT_00B0B4FC = 0x3f;
		DAT_00B0B4E0 = y + 8;
		DAT_00B0B4EC = DAT_00B0B4E0 + 0x1f;
		DAT_00B0B4F8 = DAT_00B0B4EC + 0x1e;
	} else {
		int y;
		if (side == 1) {
			y = DAT_00886F94 + 7;
		} else {
			y = DAT_00886F94 + 7;
		}
		DAT_00B0B4E0 = y;
		DAT_00B0B4EC = y + 0x20;
		DAT_00B0B4E4 = 0x34;
		DAT_00B0B4F0 = 0x20;
		DAT_00B0B4F8 = DAT_00B0B4EC + 0x1e;
		DAT_00B0B4FC = 0x40;
	}
}

namespace Unsorted { extern bool ArmageddonMode; extern int CurrentFrame; }
extern HouseClass * DAT_00AC1198;
extern int DAT_00B0B4DC;
extern int DAT_00B0B4E8;
extern int DAT_00B0B4F4;
extern int DAT_00B0B504;

inline HouseClass * Nth_Valid_House(int n)
{
	int found = 0;
	for (int index = 0; index < Houses.Count(); index++) {
		HouseClass * house = Houses[index];
		if (house && house->Class->IsMultiplayPassive != true && house != DAT_00AC1198 && house->IsHuman) {
			if (found == n) {
				return(house);
			}
			found++;
		}
	}
	return(NULL);
}

inline int Kills_Of(HouseClass * house)
{
	if (house == NULL) {
		return(0);
	}
	int kills = 0;
	for (int j = 0; j < 20; j++) {
		kills += house->UnitsKilled[j];
	}
	for (int b = 0; b < 20; b++) {
		kills += house->BuildingsKilled[b];
	}
	return(kills);
}

inline int Owned_Of(HouseClass * house)
{
	if (house == NULL) {
		return(0);
	}
	return(house->CurAircraft + house->CurInfantry + house->CurBuildings + house->CurUnits);
}

inline int Money_Of(HouseClass * house)
{
	if (house == NULL) {
		return(0);
	}
	int money = house->Available_Money();
	if (money < 0) {
		money = 0;
	}
	return(money);
}

inline ColorScheme * Color_Of(HouseClass * house)
{
	if (house == NULL) {
		return(Fetch_Scheme_By_Name("Grey", 1));
	}
	if (house == DAT_00AC1198) {
		return(Fetch_Scheme_By_Name("LightGrey", 1));
	}
	if (house->IsDefeated) {
		return(Fetch_Scheme_By_Name("Grey", 1));
	}
	return(ColorSchemes[house->Scheme]);
}

int * __fastcall FUN_0077DA50(wchar_t * name);

// FUNCTION: GAMEMD 0x006A5310
void SidebarClass::Init_IO(void)
{
	BASECLASS::Init_IO();
	FUN_006A5090();
	FUN_006A5130(false);

	if (!Unsorted::ArmageddonMode) {
		Repair.IsSticky = 1;
		Repair.ID = 101;
		Repair.X = DAT_00B0B4DC;
		Repair.Y = DAT_00B0B4E0;
		Repair.DrawOffset.X = -480;
		Repair.DrawOffset.Y = 0;
		Repair.DrawOnSidebar = 1;
		Repair.ShapeDrawer = FileSystem::SIDEBAR_PAL;
		Repair.IsOn = false;
		Repair.IsToggleType = 1;
		Repair.ReflectButtonState = 1;
		Repair.StopFlashing();

		Power.X = DAT_00B0B4E4 + Repair.X;
		Power.IsSticky = 1;
		Power.ID = 102;
		Power.Y = Repair.Y;
		Power.DrawOffset.X = -480;
		Power.DrawOffset.Y = 0;
		Power.DrawOnSidebar = 1;
		Power.ShapeDrawer = FileSystem::SIDEBAR_PAL;
		Power.IsOn = false;
		Power.IsToggleType = 1;
		Power.ReflectButtonState = 1;
		Power.StopFlashing();

		for (int index = 0; index < STRIPS; index++) {
			TabButtons[index].IsSticky = 1;
			TabButtons[index].ID = 203 + index;
			TabButtons[index].X = DAT_00B0B4E8 + DAT_00B0B4F0 * index;
			TabButtons[index].Y = DAT_00B0B4EC;
			TabButtons[index].DrawOffset.X = -480;
			TabButtons[index].DrawOffset.Y = 0;
			TabButtons[index].DrawOnSidebar = 1;
			TabButtons[index].ShapeDrawer = FileSystem::SIDEBAR_PAL;
			TabButtons[index].IsOn = false;
			TabButtons[index].IsToggleType = 2;
			TabButtons[index].ReflectButtonState = 1;
			TabButtons[index].Disable();
			TabButtons[index].StopFlashing();
		}

		Upgrade.IsSticky = 1;
		Upgrade.ID = 201;
		Upgrade.DrawOnSidebar = 1;
		Upgrade.ShapeDrawer = FileSystem::SIDEBAR_PAL;
		Upgrade.Set_Flags(0x55);
		Upgrade.StopFlashing();

		Waypoint.IsSticky = 1;
		Waypoint.ID = 200;
		Waypoint.DrawOnSidebar = 1;
		Waypoint.ShapeDrawer = FileSystem::SIDEBAR_PAL;
		Waypoint.Set_Flags(0x55);
		Waypoint.StopFlashing();

		for (int strip = 0; strip < STRIPS; strip++) {
			Column[strip].X = DAT_00B0B4F4;
			Column[strip].Y = DAT_00B0B4F8;
			Column[strip].ObjectRect = Rect(0, 0, 60, DAT_00B0B504);
			Column[strip].Init_IO(strip);
		}

		Reposition_Sidebar();
		FUN_006A5130(true);

		if (IsSidebarActive) {
			Activate(false);
			Activate(true);
		}

		DiplomacyNumHouses = 0;
		if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
			DiplomacyHouses[0] = Nth_Valid_House(0);

			while (DiplomacyNumHouses < 8 && DiplomacyHouses[DiplomacyNumHouses] != NULL) {
				HouseClass * house = DiplomacyHouses[DiplomacyNumHouses];

				DiplomacyKills[DiplomacyNumHouses] = Kills_Of(house);
				DiplomacyOwned[DiplomacyNumHouses] = Owned_Of(house);
				DiplomacyPowerDrain[DiplomacyNumHouses] = Money_Of(house);
				DiplomacyColors[DiplomacyNumHouses] = Color_Of(house);

				unknown_544C[DiplomacyNumHouses] = 0;
				unknown_546C[DiplomacyNumHouses] = 0;
				unknown_548C[DiplomacyNumHouses] = 0;
				unknown_54AC[DiplomacyNumHouses] = 0;
				unknown_54CC[DiplomacyNumHouses] = 0;
				unknown_54EC[DiplomacyNumHouses] = 0;

				if (Session.Type == GAME_INTERNET) {
					int * stats = FUN_0077DA50(house->UIName);
					if (stats) {
						unknown_544C[DiplomacyNumHouses] = stats[7];
						unknown_546C[DiplomacyNumHouses] = stats[4];
						unknown_548C[DiplomacyNumHouses] = stats[2];
						unknown_54AC[DiplomacyNumHouses] = stats[3];
						unknown_54CC[DiplomacyNumHouses] = stats[8];
						unknown_54EC[DiplomacyNumHouses] = stats[12];
						delete stats;
					}
				}

				DiplomacyNumHouses++;
				DiplomacyHouses[DiplomacyNumHouses] = Nth_Valid_House(DiplomacyNumHouses);
			}
		}
		unknown_550C = 0;
	}
}

extern void * DAT_00B0B490;
extern bool DAT_00B0B4CC;
extern void * DAT_00B0B494;
extern bool DAT_00B0B4CD;
extern void * DAT_00B0B498;
extern bool DAT_00B0B4CE;
extern void * DAT_00B0B49C;
extern bool DAT_00B0B4CF;
extern void * DAT_00B0B4A0;
extern bool DAT_00B0B4D0;
extern void * DAT_00B0B4A4;
extern bool DAT_00B0B4D1;
extern void * DAT_00B0B4A8;
extern bool DAT_00B0B4D2;
extern void * DAT_00B0B4AC;
extern bool DAT_00B0B4D3;
extern void * DAT_00B0B4B0;
extern bool DAT_00B0B4D4;
extern void * DAT_00B0B4B4;
extern bool DAT_00B0B4D5;
extern void * DAT_00B0B4B8;
extern bool DAT_00B0B4D6;
extern void * DAT_00B0B4BC;
extern bool DAT_00B0B4D7;
extern void * DAT_00B0B4C0;
extern bool DAT_00B0B4D8;
extern void * DAT_00B0B4C4;
extern bool DAT_00B0B4D9;
extern void * DAT_00B0B4C8;
extern bool DAT_00B0B4DA;
extern void * DAT_00B0B468;
extern void * DAT_00B0B46C;
extern void * DAT_00B0B470;
extern void * DAT_00B0B474;
extern bool DAT_00B0B488;
extern bool DAT_00B0B489;
extern bool DAT_00B0B48A;
extern bool DAT_00B0B48B;
extern bool DAT_00B0B48F;
extern ConvertClass * DAT_0087F6D0;

extern DSurface * DAT_00887308;
PaletteClass * __fastcall FUN_0072F4A0(void);
PaletteClass * __fastcall FUN_0072F4E0(void);

// FUNCTION: GAMEMD 0x006A5840
void SidebarClass::Init_For_House(void)
{
	BASECLASS::Init_For_House();

	PaletteClass palette = *FUN_0072F4A0();
	if (FileSystem::SIDEBAR_PAL) {
		delete FileSystem::SIDEBAR_PAL;
		FileSystem::SIDEBAR_PAL = NULL;
	}
	FileSystem::SIDEBAR_PAL = new ConvertClass(palette, palette, *DAT_00887308, 1, false);

	FileSystem::GCLOCK2_SHP = FileSystem::LoadWholeFileEx("GCLOCK2.SHP", DAT_00B0B48F);

	bool loaded = false;
	Power.Set_Shape((ShapeSet const *)FileSystem::LoadWholeFileEx("SELL.SHP", loaded), 0, 0);
	Power.SetLoadedStatus(loaded);
	Power.ShapeDrawer = FileSystem::SIDEBAR_PAL;

	Repair.Set_Shape((ShapeSet const *)FileSystem::LoadWholeFileEx("REPAIR.SHP", loaded), 0, 0);
	Repair.SetLoadedStatus(loaded);
	Repair.ShapeDrawer = FileSystem::SIDEBAR_PAL;

	for (int index = 0; index < STRIPS; index++) {
		char name[256];
		sprintf(name, "TAB%02d.SHP", index);
		TabButtons[index].Set_Shape((ShapeSet const *)FileSystem::LoadWholeFileEx(name, loaded), 0, 0);
		TabButtons[index].SetLoadedStatus(loaded);
		TabButtons[index].ShapeDrawer = FileSystem::SIDEBAR_PAL;
	}

	Upgrade.Set_Shape((ShapeSet const *)FileSystem::LoadWholeFileEx("R-DN.SHP", loaded), 0, 0);
	Upgrade.SetLoadedStatus(loaded);
	Upgrade.ShapeDrawer = FileSystem::SIDEBAR_PAL;

	Waypoint.Set_Shape((ShapeSet const *)FileSystem::LoadWholeFileEx("R-UP.SHP", loaded), 0, 0);
	Waypoint.SetLoadedStatus(loaded);
	Waypoint.ShapeDrawer = FileSystem::SIDEBAR_PAL;

	DAT_00B0B468 = FileSystem::LoadWholeFileEx("SIDE1.SHP", DAT_00B0B488);
	DAT_00B0B46C = FileSystem::LoadWholeFileEx("SIDE2.SHP", DAT_00B0B489);
	DAT_00B0B470 = FileSystem::LoadWholeFileEx("SIDE3.SHP", DAT_00B0B48A);
	DAT_00B0B474 = FileSystem::LoadWholeFileEx("ADDON.SHP", DAT_00B0B48B);

	PaletteClass observer = *FUN_0072F4E0();
	if (DAT_0087F6D0) {
		delete DAT_0087F6D0;
		DAT_0087F6D0 = NULL;
	}
	DAT_0087F6D0 = new ConvertClass(observer, observer, *DAT_00887308, 1, false);

	DAT_00B0B490 = FileSystem::LoadWholeFileEx("OBSALLI.SHP", DAT_00B0B4CC);
	DAT_00B0B494 = FileSystem::LoadWholeFileEx("OBSSOVI.SHP", DAT_00B0B4CD);
	DAT_00B0B498 = FileSystem::LoadWholeFileEx("OBSYURI.SHP", DAT_00B0B4CE);
	DAT_00B0B49C = FileSystem::LoadWholeFileEx("RANI.SHP", DAT_00B0B4CF);
	DAT_00B0B4A0 = FileSystem::LoadWholeFileEx("OBSI.SHP", DAT_00B0B4D0);
	DAT_00B0B4A4 = FileSystem::LoadWholeFileEx("USAI.SHP", DAT_00B0B4D1);
	DAT_00B0B4A8 = FileSystem::LoadWholeFileEx("JAPI.SHP", DAT_00B0B4D2);
	DAT_00B0B4AC = FileSystem::LoadWholeFileEx("FRAI.SHP", DAT_00B0B4D3);
	DAT_00B0B4B0 = FileSystem::LoadWholeFileEx("GERI.SHP", DAT_00B0B4D4);
	DAT_00B0B4B4 = FileSystem::LoadWholeFileEx("GBRI.SHP", DAT_00B0B4D5);
	DAT_00B0B4B8 = FileSystem::LoadWholeFileEx("DJBI.SHP", DAT_00B0B4D6);
	DAT_00B0B4BC = FileSystem::LoadWholeFileEx("ARBI.SHP", DAT_00B0B4D7);
	DAT_00B0B4C0 = FileSystem::LoadWholeFileEx("LATI.SHP", DAT_00B0B4D8);
	DAT_00B0B4C4 = FileSystem::LoadWholeFileEx("RUSI.SHP", DAT_00B0B4D9);
	DAT_00B0B4C8 = FileSystem::LoadWholeFileEx("YRII.SHP", DAT_00B0B4DA);
}

extern void * DAT_00B0B478;
extern bool DAT_00884B8F;
extern bool DAT_00B0B518;
bool __fastcall FUN_004E1470(GadgetClass * gadget);

static inline int First_Active_Tab(SidebarClass & sidebar)
{
	for (int index = 0; index < SidebarClass::STRIPS; index++) {
		if (sidebar.Column[index].BuildableCount > 0) {
			return index;
		}
	}
	return -1;
}

extern void * DAT_00B0B47C;
extern void * DAT_00B0B480;
extern bool DAT_00B0B48C;
extern bool DAT_00B0B48D;
extern bool DAT_00B0B48E;

// FUNCTION: GAMEMD 0x006A5BF0
void SidebarClass::DisposeOfArt(void)
{
	BASECLASS::DisposeOfArt();

	if (DAT_00B0B488) { delete DAT_00B0B468; DAT_00B0B488 = false; }
	if (DAT_00B0B489) { delete DAT_00B0B46C; DAT_00B0B489 = false; }
	if (DAT_00B0B48A) { delete DAT_00B0B470; DAT_00B0B48A = false; }
	if (DAT_00B0B48B) { delete DAT_00B0B474; DAT_00B0B48B = false; }
	if (DAT_00B0B48C) { delete DAT_00B0B478; DAT_00B0B48C = false; }
	if (DAT_00B0B48D) { delete DAT_00B0B47C; DAT_00B0B48D = false; }
	if (DAT_00B0B48E) { delete DAT_00B0B480; DAT_00B0B48E = false; }
	if (DAT_00B0B48F) { delete FileSystem::GCLOCK2_SHP; DAT_00B0B48F = false; }
	if (DAT_00B0B4CC) { delete DAT_00B0B490; DAT_00B0B4CC = false; }
	if (DAT_00B0B4CD) { delete DAT_00B0B494; DAT_00B0B4CD = false; }
	if (DAT_00B0B4CE) { delete DAT_00B0B498; DAT_00B0B4CE = false; }
	if (DAT_00B0B4CF) { delete DAT_00B0B49C; DAT_00B0B4CF = false; }
	if (DAT_00B0B4D1) { delete DAT_00B0B4A4; DAT_00B0B4D1 = false; }
	if (DAT_00B0B4D2) { delete DAT_00B0B4A8; DAT_00B0B4D2 = false; }
	if (DAT_00B0B4D3) { delete DAT_00B0B4AC; DAT_00B0B4D3 = false; }
	if (DAT_00B0B4D4) { delete DAT_00B0B4B0; DAT_00B0B4D4 = false; }
	if (DAT_00B0B4D5) { delete DAT_00B0B4B4; DAT_00B0B4D5 = false; }
	if (DAT_00B0B4D6) { delete DAT_00B0B4B8; DAT_00B0B4D6 = false; }
	if (DAT_00B0B4D7) { delete DAT_00B0B4BC; DAT_00B0B4D7 = false; }
	if (DAT_00B0B4D8) { delete DAT_00B0B4C0; DAT_00B0B4D8 = false; }
	if (DAT_00B0B4D9) { delete DAT_00B0B4C4; DAT_00B0B4D9 = false; }
	if (DAT_00B0B4D0) { delete DAT_00B0B4A0; DAT_00B0B4D0 = false; }
	if (DAT_00B0B4DA) { delete DAT_00B0B4C8; DAT_00B0B4DA = false; }

	Power.ClearShape();
	Repair.ClearShape();
	for (int index = 0; index < STRIPS; index++) {
		TabButtons[index].ClearShape();
	}
	Upgrade.ClearShape();
	Waypoint.ClearShape();
}

// FUNCTION: GAMEMD 0x006A5ED0
void SidebarClass::FUN_006A5ED0(void)
{
	IsToRedraw = true;
	Column[ActiveTabIndex].IsToRedraw = true;
	Map.Flag_To_Redraw();
	if (unknown_53A0 <= Unsorted::CurrentFrame) {
		unknown_53A0 = Unsorted::CurrentFrame + 2;
	}
}

// FUNCTION: GAMEMD 0x006A5F20
void SidebarClass::OnTechnoDestroyed(TechnoClass * techno)
{
	if (IsSidebarActive && techno) {
		TechnoTypeClass const * type = techno->Techno_Type_Class();
		if (PlayerPtr) {
			RTTIType rtti = techno->Fetch_RTTI();
			int tab = GetObjectTabIdx(rtti, techno->Fetch_Heap_ID(), false);
			if (tab == ActiveTabIndex) {
				if (PlayerPtr->FUN_0050B370(type)) {
					FUN_006A5ED0();
				}
			} else if (rtti == RTTI_BUILDING) {
				if (((BuildingClass *)techno)->Class->IsHelipad && ActiveTabIndex == 3) {
					FUN_006A5ED0();
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x006A60A0
void SidebarClass::RepaintSidebar(int tab)
{
	if (tab == ActiveTabIndex) {
		IsToRedraw = true;
		Column[tab].IsToRedraw = true;
		Map.Flag_To_Redraw();
	}
}

// FUNCTION: GAMEMD 0x006A60E0
void SidebarClass::FUN_006A60E0(int tab)
{
	if (tab == ActiveTabIndex) {
		FUN_006A5ED0();
	}
}

int __fastcall FUN_005004E0(RTTIType type, int id);

// FUNCTION: GAMEMD 0x006A6140
bool SidebarClass::Factory_Link(FactoryClass * factory, RTTIType type, int id)
{
	return Column[GetObjectTabIdx(type, id, false)].Factory_Link(factory, type, id);
}

// FUNCTION: GAMEMD 0x006A6220
bool SidebarClass::FUN_006A6220(int control)
{
	bool old = unknown_bool_5515;
	if (control == -1) {
		control = unknown_bool_5515 ? 0 : 1;
	}
	switch (control) {
		case 1:
			unknown_bool_5515 = true;
			break;

		default:
		case 0:
			unknown_bool_5515 = false;
			break;
	}
	if (old != unknown_bool_5515) {
		Flag_To_Redraw();
		IsToRedraw = true;
		if (!unknown_bool_5515) {
			Set_Default_Mouse(MOUSE_NORMAL, false);
		}
	}
	return old;
}

// FUNCTION: GAMEMD 0x006A6290
bool SidebarClass::FUN_006A6290(int control)
{
	bool old = unknown_bool_5514;
	if (control == -1) {
		control = unknown_bool_5514 ? 0 : 1;
	}
	switch (control) {
		case 1:
			unknown_bool_5514 = true;
			break;

		default:
		case 0:
			unknown_bool_5514 = false;
			break;
	}
	if (old != unknown_bool_5514) {
		Flag_To_Redraw();
		IsToRedraw = true;
		if (!unknown_bool_5514) {
			Set_Default_Mouse(MOUSE_NORMAL, false);
		}
	}
	return old;
}

// FUNCTION: GAMEMD 0x006A6300
bool SidebarClass::AddCameo(RTTIType type, int id)
{
	if (!Unsorted::ArmageddonMode) {
		int previous = First_Active_Tab(*this);
		int tab = GetObjectTabIdx(type, id, false);
		StripClass & strip = Column[tab];

		if (strip.BuildableCount <= StripClass::MAX_BUILDABLES) {
			for (int index = 0; index < strip.BuildableCount; index++) {
				if (strip.Buildables[index].BuildableType == type && strip.Buildables[index].BuildableID == id) {
					return false;
				}
			}

			if (!ScenarioInit && type != RTTI_SPECIAL) {
				VoxClass::Play("EVA_NewConstructionOptions");
			}
			strip.FUN_006A8710(type, id);
			strip.IsToRedraw = true;
			DAT_00884B8F = true;
			FUN_006A67A0(tab);
			Activate(true);

			if (previous == -1) {
				int first = First_Active_Tab(*this);
				if (first != ActiveTabIndex) {
					TabButtons[ActiveTabIndex].Turn_Off();
					StripClass & old = Column[ActiveTabIndex];
					old.AllowedToDraw = false;
					for (int slot = 0; slot < 60; slot++) {
						Map.Remove_A_Button(StripClass::SelectButton[old.ID * 60 + slot]);
					}
					ActiveTabIndex = first;
					TabButtons[first].Turn_On();
					StripClass & current = Column[ActiveTabIndex];
					current.AllowedToDraw = true;
					int count = Map.GetVisibleCameoCount();
					for (int added = 0; added < count; added++) {
						StripClass::SelectButton[current.ID * 60 + added].Zap();
						Map.Add_A_Button(StripClass::SelectButton[current.ID * 60 + added]);
					}
					DAT_00884B8F = true;
					UpdateScrollButtons();
					IsForceCompleteRedraw = true;
				}
			}

			IsToRedraw = true;
			Flag_To_Redraw();
			if (tab == ActiveTabIndex) {
				DAT_00B0B518 = true;
			}
			return true;
		}
	}
	return false;
}

extern int DAT_00886F9C;
extern int DAT_00884CF8;

static inline int Visible_Cameos(void)
{
	int side = Scen->PlayerSideIndex;
	int reserved;
	if (side == 0) {
		reserved = 0x1a;
	} else if (side == 1) {
		reserved = 0x12;
	} else {
		reserved = 0x12;
	}
	return (DAT_00886F94 + DAT_00886F9C - DAT_00B0B4F8 - reserved - 7) / 50 * 2;
}

// FUNCTION: GAMEMD 0x006A6610
void SidebarClass::UpdateScrollButtons(void)
{
	if (PlayerPtr != DAT_00AC1198) {
		int visible = Visible_Cameos();
		if (Column[ActiveTabIndex].BuildableCount > visible) {
			if (!FUN_004E1470(&Upgrade)) {
				Upgrade.Enable();
			}
			if (!FUN_004E1470(&Waypoint)) {
				Waypoint.Enable();
			}
		} else {
			if (FUN_004E1470(&Upgrade)) {
				Upgrade.Disable();
			}
			if (FUN_004E1470(&Waypoint)) {
				Waypoint.Disable();
			}
		}
	} else {
		int visible = Visible_Cameos();
		if (DAT_00884CF8 * 4 > visible) {
			if (!FUN_004E1470(&Upgrade)) {
				Upgrade.Enable();
			}
			if (!FUN_004E1470(&Waypoint)) {
				Waypoint.Enable();
			}
		} else {
			if (FUN_004E1470(&Upgrade)) {
				Upgrade.Disable();
			}
			if (FUN_004E1470(&Waypoint)) {
				Waypoint.Disable();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x006A67A0
void SidebarClass::FUN_006A67A0(int tab)
{
	if (First_Active_Tab(*this) == -1 && DAT_00B0B478) {
		unknown_5398 = 1;
		unknown_5394 = 0;
	}
	if (!FUN_004E1470(&TabButtons[tab])) {
		TabButtons[tab].Enable();
	}
	if (tab == ActiveTabIndex) {
		UpdateScrollButtons();
	}
}

// FUNCTION: GAMEMD 0x006A6820
void SidebarClass::FUN_006A6820(int tab)
{
	if (Column[tab].BuildableCount > 0) {
		UpdateScrollButtons();
		return;
	}
	TabButtons[tab].Disable();

	int first = First_Active_Tab(*this);
	if (first == -1) {
		UpdateScrollButtons();
		if (DAT_00B0B478) {
			unknown_5398 = -1;
			unknown_5394 = ((short *)DAT_00B0B478)[3];
		}
	} else if (first != ActiveTabIndex) {
		TabButtons[ActiveTabIndex].Turn_Off();
		StripClass & old = Column[ActiveTabIndex];
		old.AllowedToDraw = false;
		for (int slot = 0; slot < 60; slot++) {
			Map.Remove_A_Button(StripClass::SelectButton[old.ID * 60 + slot]);
		}
		ActiveTabIndex = first;
		TabButtons[first].Turn_On();
		StripClass & current = Column[ActiveTabIndex];
		current.AllowedToDraw = true;
		int count = Visible_Cameos();
		for (int added = 0; added < count; added++) {
			StripClass::SelectButton[current.ID * 60 + added].Zap();
			Map.Add_A_Button(StripClass::SelectButton[current.ID * 60 + added]);
		}
		DAT_00884B8F = true;
		UpdateScrollButtons();
		IsForceCompleteRedraw = true;
	}
}

static inline int Max_Visible_Cameos(void)
{
	int side = Scen->PlayerSideIndex;
	int reserved;
	if (side == 0) {
		reserved = 0x1a;
	} else if (side == 1) {
		reserved = 0x12;
	} else {
		reserved = 0x12;
	}
	return (DAT_00886F94 - DAT_00B0B4F8 - reserved + DAT_00886F9C - 7) / 50;
}

extern Rect DAT_008809F4;
extern Rect DAT_00B07DC8;
extern DSurface * DAT_00887300;
extern CCToolTip * ToolTips;

// FUNCTION: GAMEMD 0x006A6C30
void SidebarClass::Draw_It(bool complete)
{
	DAT_008809F4 = DAT_00B07DC8;

	Surface * old = DSurface::Temp;
	DSurface::Temp = DAT_00887300;
	bool redrawn = false;

	Rect window(0, 0, DAT_00887300->Get_Width(), DAT_00887300->Get_Height());

	complete = complete || IsToRedraw || IsForceCompleteRedraw;

	if (IsSidebarActive && complete && !Unsorted::ArmageddonMode) {
		if (IsForceCompleteRedraw) {
			redrawn = true;

			Draw_Shape(*DAT_00887300, *FileSystem::SIDEBAR_PAL, (ShapeSet const *)DAT_00B0B468, 0, Point2D(0, DAT_00886F94), window, SHAPE_WIN_REL);

			int y = DAT_00B0B4F8;
			for (int i = 0; i < Max_Visible_Cameos(); i++) {
				Draw_Shape(*DAT_00887300, *FileSystem::SIDEBAR_PAL, (ShapeSet const *)DAT_00B0B46C, 0, Point2D(0, y), window, SHAPE_WIN_REL);
				y += ((ShapeSet const *)DAT_00B0B46C)->Get_Height();
			}

			Draw_Shape(*DAT_00887300, *FileSystem::SIDEBAR_PAL, (ShapeSet const *)DAT_00B0B470, 0, Point2D(0, y), window, SHAPE_WIN_REL);
			Draw_Shape(*DAT_00887300, *FileSystem::SIDEBAR_PAL, (ShapeSet const *)DAT_00B0B474, 0, Point2D(0, y + ((ShapeSet const *)DAT_00B0B470)->Get_Height()), window, SHAPE_WIN_REL);
		}

		Power.Draw_Me(redrawn);
		Repair.Draw_Me(redrawn);
		for (int index = 0; index < STRIPS; index++) {
			TabButtons[index].Draw_Me(true);
		}
		Upgrade.Draw_Me(redrawn);
		Waypoint.Draw_Me(redrawn);

		if (unknown_5398 != 0) {
			Draw_Shape(*DAT_00887300, *FileSystem::SIDEBAR_PAL, (ShapeSet const *)DAT_00B0B478, unknown_5394, Point2D(0, DAT_00B0B4F8), window, SHAPE_WIN_REL);
			Draw_Shape(*DAT_00887300, *FileSystem::SIDEBAR_PAL, (ShapeSet const *)DAT_00B0B47C, unknown_5394, Point2D(0, DAT_00B0B4F8), window, SHAPE_WIN_REL);
			Draw_Shape(*DAT_00887300, *FileSystem::SIDEBAR_PAL, (ShapeSet const *)DAT_00B0B480, unknown_5394, Point2D(0, DAT_00B0B4F8), window, SHAPE_WIN_REL);
		}
		DAT_00B0B518 = true;
	}

	Column[ActiveTabIndex].Draw_It(IsForceCompleteRedraw);
	BASECLASS::Draw_It(complete || redrawn);

	if (Power.IsDrawn) {
		Power.IsDrawn = false;
		DAT_00B0B518 = true;
	}
	if (Repair.IsDrawn) {
		Repair.IsDrawn = false;
		DAT_00B0B518 = true;
	}
	for (int index = 0; index < STRIPS; index++) {
		if (TabButtons[index].IsDrawn) {
			DAT_00B0B518 = true;
			TabButtons[index].IsDrawn = false;
		}
	}
	if (Upgrade.IsDrawn) {
		Upgrade.IsDrawn = false;
		DAT_00B0B518 = true;
	}
	if (Waypoint.IsDrawn) {
		Waypoint.IsDrawn = false;
		DAT_00B0B518 = true;
	}
	if (ToolTips != NULL) {
		ToolTips->Draw_Current(true);
	}

	Blit_Sidebar(DAT_00B0B518 || IsForceCompleteRedraw);
	IsToRedraw = false;
	IsForceCompleteRedraw = false;
	DSurface::Temp = (DSurface *)old;
}

namespace Game { extern bool IsActive; extern HWND hWnd; }
namespace Unsorted { extern bool ScenarioStarted; }
extern bool DAT_00B0B519;
extern int DAT_00886F98;

class MouseSlots {
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
		virtual void Slot10(void);
		virtual void Slot11(void);
		virtual void Slot12(void);
		virtual void Slot13(void);
		virtual void Slot14(void);
		virtual void Draw_Mouse(Surface * scr, bool issidebarsurface);
};
extern MouseSlots * DAT_00887640;

// FUNCTION: GAMEMD 0x006A70E0
void SidebarClass::Blit_Sidebar(bool complete)
{
	if (IsSidebarActive && Game::IsActive && Unsorted::ScenarioStarted) {

		RECT crect;
		GetClientRect(Game::hWnd, &crect);

		POINT scrnpt;
		scrnpt.x = crect.left;
		scrnpt.y = crect.top;
		ClientToScreen(Game::hWnd, &scrnpt);

		if (!DAT_00B0B518 && !complete) {
			DAT_00B0B518 = false;
			if (DAT_008809F4 == DAT_00B07DC8) {
				if (IsToRedrawCredits) {
					DAT_00887640->Draw_Mouse(DAT_00887300, true);
					DAT_00887308->Blit_From(
						Rect(scrnpt.x + (Options.IsSidebarOnRight ? DSurface::ViewBounds.Width : 0), scrnpt.y, DAT_00886F98, 16),
						*DAT_00887300,
						Rect(0, 0, DAT_00886F98, 16),
						false,
						true
					);
					IsToRedrawCredits = false;
					DAT_00B0B519 = true;
				}
				DAT_00B0B518 = false;
				return;
			}
		} else {
			DAT_00B0B518 = true;
		}

		DAT_00887640->Draw_Mouse(DAT_00887300, true);
		if (DAT_008809F4 == DAT_00B07DC8 && !complete) {
			if (IsToRedrawCredits) {
				DAT_00887308->Blit_From(Rect(scrnpt.x + (Options.IsSidebarOnRight ? DSurface::ViewBounds.Width : 0), scrnpt.y, DAT_00886F98, 16), *DAT_00887300, Rect(0, 0, DAT_00886F98, 16), false, true);
				IsToRedrawCredits = false;
			}
			DAT_00887308->Blit_From(Rect(scrnpt.x + (Options.IsSidebarOnRight ? DSurface::ViewBounds.Width : 0), scrnpt.y + DAT_00886F94, DAT_00886F98, DAT_00887300->Get_Height() - DAT_00886F94), *DAT_00887300, Rect(0, DAT_00886F94, DAT_00886F98, DAT_00886F9C), false, true);
		} else if (DAT_00B0B518) {
			Rect sb_rect = DAT_00887300->Get_Rect();
			DAT_00887308->Blit_From(Rect(scrnpt.x + (Options.IsSidebarOnRight ? DSurface::ViewBounds.Width : 0), scrnpt.y, sb_rect.Width, sb_rect.Height), *DAT_00887300, Rect(0, 0, sb_rect.Width, sb_rect.Height), false, true);
		} else {
			DAT_00887308->Blit_From(Rect(scrnpt.x + DAT_008809F4.X + (Options.IsSidebarOnRight ? DSurface::ViewBounds.Width : 0), scrnpt.y + DAT_008809F4.Y, DAT_008809F4.Width, DAT_008809F4.Height), *DAT_00887300, DAT_008809F4, false, true);
		}
		DAT_00B0B519 = true;
	}
	DAT_00B0B518 = false;
}

// FUNCTION: GAMEMD 0x006A74A0
void SidebarClass::FUN_006A74A0(int tab)
{
	ActiveTabIndex = tab;
	TabButtons[tab].Turn_On();
	StripClass & current = Column[ActiveTabIndex];
	current.AllowedToDraw = true;
	int count = Max_Visible_Cameos() * 2;
	for (int added = 0; added < count; added++) {
		StripClass::SelectButton[current.ID * 60 + added].Zap();
		Map.Add_A_Button(StripClass::SelectButton[current.ID * 60 + added]);
	}
	DAT_00884B8F = true;
	UpdateScrollButtons();
}

static inline void Select_Tab(SidebarClass & sb, int tab)
{
	if (tab != sb.ActiveTabIndex) {
		SidebarClass::TabButtons[sb.ActiveTabIndex].Turn_Off();
		SidebarClass::StripClass & old = sb.Column[sb.ActiveTabIndex];
		old.AllowedToDraw = false;
		for (int slot = 0; slot < 60; slot++) {
			Map.Remove_A_Button(SidebarClass::StripClass::SelectButton[old.ID * 60 + slot]);
		}
		sb.ActiveTabIndex = tab;
		SidebarClass::TabButtons[tab].Turn_On();
		SidebarClass::StripClass & current = sb.Column[sb.ActiveTabIndex];
		current.AllowedToDraw = true;
		int count = Max_Visible_Cameos() * 2;
		for (int added = 0; added < count; added++) {
			SidebarClass::StripClass::SelectButton[current.ID * 60 + added].Zap();
			Map.Add_A_Button(SidebarClass::StripClass::SelectButton[current.ID * 60 + added]);
		}
		DAT_00884B8F = true;
		sb.UpdateScrollButtons();
		sb.IsForceCompleteRedraw = true;
	}
}

// FUNCTION: GAMEMD 0x006A7590
void SidebarClass::SetTab(int tab)
{
	Select_Tab(*this, tab);
}

// FUNCTION: GAMEMD 0x006A76F0
void SidebarClass::FUN_006A76F0(int tab)
{
	int delay = 10 - Unsorted::CurrentFrame % 10;
	TabButtons[tab].StartFlashing(10, delay, (Unsorted::CurrentFrame + delay) / 10 % 2 == 0);
	DAT_00884B8F = true;
	Map.Flag_To_Redraw();
}

// FUNCTION: GAMEMD 0x006A7760
void SidebarClass::FUN_006A7760(int tab)
{
	TabButtons[tab].StopFlashing();
}

// FUNCTION: GAMEMD 0x006A7D20
void SidebarClass::Recalc(void)
{
	bool redraw = false;
	for (int index = 0; index < STRIPS; index++) {
		if (Column[index].Recalc() && index == ActiveTabIndex) {
			Column[index].IsToRedraw = true;
			redraw = true;
		}
	}
	if (redraw) {
		IsToRedraw = true;
		Flag_To_Redraw();
	}
}

namespace Game { extern unsigned char RecordingFlag; }
void FUN_005BF450(void);

// FUNCTION: GAMEMD 0x006A7D70
bool SidebarClass::Activate(int control)
{
	bool old = IsSidebarActive;

	if ((Game::RecordingFlag & 2) && Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
		return(old);
	}

	switch (control) {
		case -1:
			IsSidebarActive = IsSidebarActive == false;
			break;

		case 1:
			IsSidebarActive = true;
			break;

		default:
		case 0:
			IsSidebarActive = false;
			break;
	}

	bool changed = IsSidebarActive != old;
	if (changed) {
		Set_Buttons(IsSidebarActive);
		if (IsSidebarActive) {
			Reposition_Sidebar();
			IsToRedraw = true;
			Power.Zap();
			Add_A_Button(Power);
			Repair.Zap();
			Add_A_Button(Repair);
			for (int index = 0; index < STRIPS; index++) {
				TabButtons[index].Zap();
				TabButtons[index].IsPressed = false;
				Add_A_Button(TabButtons[index]);
			}
			Upgrade.Zap();
			Map.Add_A_Button(Upgrade);
			Waypoint.Zap();
			Map.Add_A_Button(Waypoint);
			Background.Zap();
			Add_A_Button(Background);
			RadarButton.Zap();
			Add_A_Button(RadarButton);
			for (int strip = 0; strip < STRIPS; strip++) {
				Column[strip].TopIndex = 0;
				if (Column[strip].BuildableCount > 0) {
					TabButtons[strip].Enable();
				}
			}
			TabButtons[ActiveTabIndex].Turn_On();
			StripClass & current = Column[ActiveTabIndex];
			current.AllowedToDraw = true;
			int count = Max_Visible_Cameos() * 2;
			for (int added = 0; added < count; added++) {
				StripClass::SelectButton[current.ID * 60 + added].Zap();
				Map.Add_A_Button(StripClass::SelectButton[current.ID * 60 + added]);
			}
			DAT_00884B8F = true;
			UpdateScrollButtons();
		} else {
			FUN_005BF450();
			Remove_A_Button(Power);
			Remove_A_Button(Repair);
			Remove_A_Button(Background);
			for (int index = 0; index < STRIPS; index++) {
				Remove_A_Button(TabButtons[index]);
			}
			Remove_A_Button(Upgrade);
			Remove_A_Button(Waypoint);
			StripClass & current = Column[ActiveTabIndex];
			for (int slot = 0; slot < 60; slot++) {
				Map.Remove_A_Button(StripClass::SelectButton[current.ID * 60 + slot]);
			}
			Remove_A_Button(RadarButton);
		}
		Flag_To_Redraw(GS_REDRAW_ALL);
	}
	return(changed);
}

// FUNCTION: GAMEMD 0x006A8080
int SidebarClass::FUN_006A8080(int count)
{
	return DAT_00B0B4FC * count;
}

// FUNCTION: GAMEMD 0x006A8090
int SidebarClass::FUN_006A8090(int count)
{
	return DAT_00B0B500 * count + 1;
}

// FUNCTION: GAMEMD 0x006A80A0
SidebarClass::StripClass::StripClass(InitClass const &) :
	X(0),
	Y(0),
	ObjectRect(DAT_00B07DC8),
	ID(0),
	IsToRedraw(true),
	IsBuilding(false),
	IsScrollingDown(false),
	IsScrolling(false),
	Flasher(-1),
	TopIndex(0),
	Scroller(0),
	Slid(0),
	LastSlid(0),
	BuildableCount(0)
{
	for (int index = 0; index < MAX_BUILDABLES; index++) {
		Buildables[index].BuildableID = 0;
		Buildables[index].BuildableType = RTTI_NONE;
		Buildables[index].IsAlt = 0;
		Buildables[index].Factory = NULL;
	}
}

// FUNCTION: GAMEMD 0x006A8160
void SidebarClass::StripClass::One_Time(int)
{
	FileSystem::DARKEN_SHP = FileSystem::LoadFile("DARKEN.SHP", false);
}

// FUNCTION: GAMEMD 0x006A8180
ShapeSet const * SidebarClass::StripClass::Get_Special_Cameo(SuperWeaponType type)
{
	if ((unsigned)type < (unsigned)SuperWeaponTypes.Count()) {
		return SuperWeaponTypes[type]->CameoData;
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x006A81B0
void SidebarClass::StripClass::Init_Clear(void)
{
	IsScrollingDown = false;
	IsScrolling = false;
	IsBuilding = false;
	Flasher = -1;
	TopIndex = 0;
	Slid = 0;
	BuildableCount = 0;

	for (int index = 0; index < MAX_BUILDABLES; index++) {
		Buildables[index].BuildableID = 0;
		Buildables[index].BuildableType = RTTI_NONE;
		Buildables[index].IsAlt = 0;
		Buildables[index].Factory = NULL;
		Buildables[index].unknown_10 = 0;
		Buildables[index].Progress.Set_Rate(0);
		Buildables[index].Progress.Set_Stage(0);
		Buildables[index].FlashEndFrame = 0;
	}
}

static inline int Strip_Rows(void)
{
	int side = Scen->PlayerSideIndex;
	int reserved;
	if (side == 0) {
		reserved = 0x1a;
	} else if (side == 1) {
		reserved = 0x12;
	} else {
		reserved = 0x12;
	}
	return (DAT_00886F94 - DAT_00B0B4F8 - reserved + DAT_00886F9C - 7) / 50;
}

// FUNCTION: GAMEMD 0x006A8220
void SidebarClass::StripClass::Init_IO(int id)
{
	ID = id;

	int rows = Strip_Rows();
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < 2; col++) {
			int slot = row * 2 + col;
			SelectClass & g = SelectButton[ID * 60 + slot];
			g.ID = 202;
			g.X = Map.Column[ID].X + DAT_00B0B4FC * col;
			g.Y = Map.Column[ID].Y + FUN_006A8090(row);
			g.Width = 60;
			g.Height = 48;
			g.Set_Owner(*this, slot);
		}
	}
}

// FUNCTION: GAMEMD 0x006A8330
void SidebarClass::StripClass::Activate(void)
{
	int count = Strip_Rows() * 2;
	for (int index = 0; index < count; index++) {
		SelectButton[ID * 60 + index].Zap();
		Map.Add_A_Button(SelectButton[ID * 60 + index]);
	}
	DAT_00884B8F = true;
}

// FUNCTION: GAMEMD 0x006A83E0
void SidebarClass::StripClass::Deactivate(void)
{
	for (int index = 0; index < 60; index++) {
		Map.Remove_A_Button(SelectButton[ID * 60 + index]);
	}
}

TechnoTypeClass const * __fastcall Fetch_Techno_Type(RTTIType type, int id);

// FUNCTION: GAMEMD 0x006A8420
bool SidebarClass::StripClass::SortsBefore(RTTIType type, int id, RTTIType otherType, int otherId)
{
	TechnoTypeClass const * left = Fetch_Techno_Type(type, id);
	TechnoTypeClass const * right = Fetch_Techno_Type(otherType, otherId);

	if (otherType == RTTI_NONE) {
		return true;
	}

	bool leftSpecial = (type == RTTI_SPECIAL || type == RTTI_SUPERWEAPON || type == RTTI_SUPERWEAPONTYPE);
	bool rightSpecial = (otherType == RTTI_SPECIAL || otherType == RTTI_SUPERWEAPON || otherType == RTTI_SUPERWEAPONTYPE);
	bool leftTech, leftAir, leftNaval, leftNormal, rightTech, rightAir, rightNaval, rightNormal;
	int leftCost, rightCost;
	wchar_t const * leftName;
	wchar_t const * rightName;

	if (leftSpecial && rightSpecial) {
		SuperWeaponTypeClass * a = SuperWeaponTypes[id];
		SuperWeaponTypeClass * b = SuperWeaponTypes[otherId];
		if (a->RechargeTime < b->RechargeTime) {
			return true;
		}
		if (a->RechargeTime > b->RechargeTime) {
			return false;
		}
		leftName = a->UIName;
		rightName = b->UIName;
		goto compare_names;
	}

	if (!leftSpecial && !rightSpecial) {
		int side = PlayerPtr->Class->Side;
		bool leftOwn = (side == left->AIBasePlanningSide);
		bool rightOwn = (side == right->AIBasePlanningSide);
		if (leftOwn && !rightOwn) {
			return true;
		}
		if (!leftOwn && rightOwn) {
			return false;
		}
	}

	leftTech = (type == RTTI_UNITTYPE || type == RTTI_AIRCRAFTTYPE);
	leftAir = leftTech && left->ConsideredAircraft;
	leftNaval = leftTech && left->Naval;
	leftNormal = !leftAir && !leftNaval;

	rightTech = (otherType == RTTI_UNITTYPE || otherType == RTTI_AIRCRAFTTYPE);
	rightAir = rightTech && right->ConsideredAircraft;
	rightNaval = rightTech && right->Naval;
	rightNormal = !rightAir && !rightNaval;

	if (leftSpecial) {
		if (rightAir || rightNaval || rightNormal) {
			return true;
		}
	} else if (leftNormal) {
		if (rightSpecial) {
			return false;
		}
		if (rightAir || rightNaval) {
			return true;
		}
	} else if (leftAir) {
		if (rightSpecial) {
			return false;
		}
		if (rightNormal) {
			return false;
		}
		if (rightNaval) {
			return true;
		}
	} else if (leftNaval) {
		if (rightSpecial) {
			return false;
		}
		if (rightNormal || rightAir) {
			return false;
		}
	}

	if (left->Level < right->Level) {
		return true;
	}
	if (left->Level > right->Level) {
		return false;
	}
	leftCost = left->Cost_Of(PlayerPtr);
	rightCost = right->Cost_Of(PlayerPtr);
	if (leftCost < rightCost) {
		return true;
	}
	if (leftCost > rightCost) {
		return false;
	}
	leftName = left->UIName;
	rightName = right->UIName;
compare_names:
	if (wcscmp(leftName, rightName) > 0) {
		return false;
	}
	return true;
}

// FUNCTION: GAMEMD 0x006A87F0
bool SidebarClass::StripClass::Add(RTTIType type, int id)
{
	if (BuildableCount <= MAX_BUILDABLES) {
		for (int index = 0; index < BuildableCount; index++) {
			if (Buildables[index].BuildableType == type && Buildables[index].BuildableID == id) {
				return false;
			}
		}
		if (!ScenarioInit && type != RTTI_SPECIAL) {
			VoxClass::Play("EVA_NewConstructionOptions");
		}
		FUN_006A8710(type, id);
		IsToRedraw = true;
		DAT_00884B8F = true;
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x006A8860
bool SidebarClass::StripClass::Scroll(bool up, int amount)
{
	if (up) {
		if (!TopIndex) {
			return false;
		}
		Scroller -= amount;
	} else {
		int rows = Strip_Rows();
		if (PlayerPtr == DAT_00AC1198) {
			if (TopIndex + rows / 2 >= DAT_00884CF8) {
				return false;
			}
		} else if ((TopIndex + rows) * 2 >= BuildableCount) {
			return false;
		}
		Scroller += amount;
	}
	return true;
}

// FUNCTION: GAMEMD 0x006A8920
bool SidebarClass::StripClass::Scroll(bool up)
{
	return Scroll(up, 1);
}

// FUNCTION: GAMEMD 0x006A89E0
bool SidebarClass::StripClass::Page(bool up)
{
	int amount = Strip_Rows();
	if (PlayerPtr == DAT_00AC1198) {
		amount = 1;
	}
	return Scroll(up, amount);
}

// FUNCTION: GAMEMD 0x006A8B10
void SidebarClass::StripClass::Flag_To_Redraw(void)
{
	IsToRedraw = true;
	Map.Flag_To_Redraw();
}

extern bool DAT_00884B8C;
extern unsigned short DAT_00B07BC4[];

// FUNCTION: GAMEMD 0x006A92E0
unsigned short const * SidebarClass::StripClass::Help_Text(int id)
{
	int i = id + TopIndex * 2;
	if (Game::IsActive) {
		if (i < BuildableCount && BuildableCount < MAX_BUILDABLES) {
			if (Buildables[i].BuildableType == RTTI_SPECIAL) {
				return SuperWeaponTypes[Buildables[i].BuildableID]->UIName;
			}
			TechnoTypeClass const * choice = Fetch_Techno_Type(Buildables[i].BuildableType, Buildables[i].BuildableID);
			if (choice) {
				if (DAT_00884B8C) {
					swprintf(DAT_00B07BC4, StringTable::LoadString("TXT_MONEY_FORMAT_1", NULL, "D:\\ra2mdpost\\Sidebar.CPP", 3180), choice->Cost_Of(PlayerPtr));
				} else {
					swprintf(DAT_00B07BC4, StringTable::LoadString("TXT_MONEY_FORMAT_2", NULL, "D:\\ra2mdpost\\Sidebar.CPP", 3182), choice->UIName, choice->Cost_Of(PlayerPtr));
				}
				int n = wcslen(DAT_00B07BC4);
				if (n > 0) {
					wchar_t * text = DAT_00B07BC4;
					do {
						if (*text == L' ') {
							*text = L'\n';
						}
						text++;
					} while (--n);
				}
				return DAT_00B07BC4;
			}
		}
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x006A93F0
void SidebarClass::StripClass::AddButtons(void)
{
	AllowedToDraw = true;
	int count = Strip_Rows() * 2;
	for (int index = 0; index < count; index++) {
		SelectButton[ID * 60 + index].Zap();
		Map.Add_A_Button(SelectButton[ID * 60 + index]);
	}
	DAT_00884B8F = true;
}

// FUNCTION: GAMEMD 0x006A94B0
void SidebarClass::StripClass::RemoveButtons(void)
{
	AllowedToDraw = false;
	for (int index = 0; index < 60; index++) {
		Map.Remove_A_Button(SelectButton[ID * 60 + index]);
	}
}

// FUNCTION: GAMEMD 0x006A94F0
int SidebarClass::StripClass::FUN_006A94F0(int index)
{
	int progress = 0;
	if (Buildables[index].Factory) {
		progress = Buildables[index].Factory->Completion();
	}
	if (Buildables[index].unknown_10 == 1) {
		int stage = Buildables[index].Progress.Fetch_Stage();
		if (stage > progress) {
			progress = (progress + stage) / 2;
		}
	}
	return progress;
}

// FUNCTION: GAMEMD 0x006AACE0
void SidebarClass::StripClass::SelectClass::Set_Owner(StripClass & strip, int index)
{
	Strip = &strip;
	Index = index;
}

extern bool UserInputLocked;

// FUNCTION: GAMEMD 0x006AB990
void SidebarClass::StripClass::SelectClass::OnMouseEnter(void)
{
	ToolTips->Save_Timer_Delay();
	ToolTips->Set_Timer_Delay(0);
	unknown_34 = true;
	Strip->IsToRedraw = true;
	Map.Flag_To_Redraw();
	DAT_00884B8F = true;
	DAT_00B0B518 = true;
}

// FUNCTION: GAMEMD 0x006AB9E0
void SidebarClass::StripClass::SelectClass::OnMouseLeave(void)
{
	ToolTips->Restore_Timer_Delay();
	unknown_34 = false;
	Strip->IsToRedraw = true;
	Map.Flag_To_Redraw();
	DAT_00884B8F = true;
	DAT_00B0B518 = true;
}

// FUNCTION: GAMEMD 0x006ABA20
bool SidebarClass::StripClass::SelectClass::FUN_006ABA20(void)
{
	return unknown_34 && !UserInputLocked;
}

// FUNCTION: GAMEMD 0x006ABA40
int SidebarClass::SBGadgetClass::Action(unsigned flags, KeyNumType & key, int modifier)
{
	Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
	return true;
}

// FUNCTION: GAMEMD 0x006ABA60
bool SidebarClass::StripClass::Factory_Link(FactoryClass * factory, RTTIType type, int id)
{
	for (int index = 0; index < BuildableCount; index++) {
		if (Buildables[index].BuildableType == type && Buildables[index].BuildableID == id) {
			Buildables[index].Factory = factory;
			IsBuilding = true;
			IsToRedraw = true;
			Map.Flag_To_Redraw();
			return true;
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x006ABAC0
bool SidebarClass::StripClass::FUN_006ABAC0(void)
{
	return BuildableCount > 0;
}

// FUNCTION: GAMEMD 0x006ABAD0
bool SidebarClass::Abandon_Production(RTTIType type, int id, FactoryClass * factory)
{
	return Column[GetObjectTabIdx(type, id, false)].Abandon_Production(factory);
}

static inline TechnoClass * Building_Cast(TechnoClass * object)
{
	if (object == NULL) return NULL;
	return object->Fetch_RTTI() == RTTI_BUILDING ? object : (TechnoClass *)NULL;
}
static inline bool Is_Building(TechnoClass * object)
{
	return object && object->Fetch_RTTI() == RTTI_BUILDING;
}

// FUNCTION: GAMEMD 0x006ABB60
bool SidebarClass::StripClass::Abandon_Production(FactoryClass const * factory)
{
	if (factory && factory->Get_Object() && Is_Building(factory->Get_Object())) {
		Building_Cast(factory->Get_Object())->ClearSidebarTabObject();
	}
	bool abandoned = false;
	bool idle = true;
	for (int index = 0; index < BuildableCount; index++) {
		if (Buildables[index].Factory == factory) {
			((FactoryClass *)factory)->Abandon();
			Buildables[index].Factory = NULL;
			abandoned = true;
			Buildables[index].unknown_10 = 0;
		} else if (Buildables[index].Factory != NULL) {
			idle = false;
		}
	}
	if (abandoned) {
		IsToRedraw = true;
		Map.Flag_To_Redraw();
		DAT_00884B8F = true;
	}
	if (idle) {
		IsBuilding = false;
	}
	return abandoned;
}

namespace Game { extern int SpecialDialog; }

// FUNCTION: GAMEMD 0x006ABC40
void FUN_006ABC40(void)
{
	if (Session.Type != GAME_NORMAL) {
		Game::SpecialDialog = 8;
	}
}

// FUNCTION: GAMEMD 0x006ABC60
int SidebarClass::GetObjectTabIdx(RTTIType type, int id, bool naval)
{
	if (type == RTTI_INFANTRY || type == RTTI_INFANTRYTYPE) {
		return 2;
	}
	if (type == RTTI_UNIT || type == RTTI_UNITTYPE || type == RTTI_AIRCRAFT || type == RTTI_AIRCRAFTTYPE) {
		return 3;
	}
	if (type == RTTI_BUILDING || type == RTTI_BUILDINGTYPE) {
		return FUN_005004E0(type, id) == 5;
	}
	if (type == RTTI_SUPERWEAPON || type == RTTI_SUPERWEAPONTYPE || type == RTTI_SPECIAL) {
		return 1;
	}
	return -1;
}

// FUNCTION: GAMEMD 0x006ABCD0
int SidebarClass::GetObjectTabIdxByCat(RTTIType type, int buildCat, bool naval)
{
	if (type == RTTI_INFANTRYTYPE) {
		return 2;
	}
	if (type == RTTI_UNITTYPE || type == RTTI_AIRCRAFTTYPE) {
		return 3;
	}
	if (type == RTTI_BUILDINGTYPE) {
		return buildCat == 5;
	}
	return -1;
}

// FUNCTION: GAMEMD 0x006ABD10
int SidebarClass::FUN_006ABD10(void)
{
	for (int index = 0; index < STRIPS; index++) {
		if (Column[index].BuildableCount > 0) {
			return index;
		}
	}
	return -1;
}

extern int DAT_00886F9C;

// FUNCTION: GAMEMD 0x006AC3E0
int SidebarClass::Max_Visible(void)
{
	int side = Scen->PlayerSideIndex;
	int reserved;
	if (side == 0) {
		reserved = 0x1a;
	} else if (side == 1) {
		reserved = 0x12;
	} else {
		reserved = 0x12;
	}
	return (DAT_00886F94 - DAT_00B0B4F8 - reserved + DAT_00886F9C - 7) / 50;
}

// FUNCTION: GAMEMD 0x006AC5D0
HRESULT SidebarClass::Load(IStream * stream)
{
	return BASECLASS::Load(stream);
}

// FUNCTION: GAMEMD 0x006AC5E0
HRESULT SidebarClass::Save(IStream * stream)
{
	return BASECLASS::Save(stream);
}

// FUNCTION: GAMEMD 0x006AC5F0
void SidebarClass::FUN_006AC5F0(TechnoTypeClass const * type, int frames)
{
	int id;
	for (id = 0; id < InfantryTypes.Count(); id++) {
		if ((void const *)InfantryTypes[id] == (void const *)type) {
			goto found;
		}
	}
	for (id = 0; id < UnitTypes.Count(); id++) {
		if ((void const *)type == (void const *)UnitTypes[id]) {
			goto found;
		}
	}
	for (id = 0; id < AircraftTypes.Count(); id++) {
		if ((void const *)type == (void const *)AircraftTypes[id]) {
			goto found;
		}
	}
	for (id = 0; id < BuildingTypes.Count(); id++) {
		if ((void const *)type == (void const *)BuildingTypes[id]) {
			goto found;
		}
	}
	return;
found:
	RTTIType rtti = type->Fetch_RTTI();
	int tab;
	if (rtti == RTTI_INFANTRY || rtti == RTTI_INFANTRYTYPE) {
		tab = 2;
	} else if (rtti == RTTI_UNIT || rtti == RTTI_UNITTYPE || rtti == RTTI_AIRCRAFT || rtti == RTTI_AIRCRAFTTYPE) {
		tab = 3;
	} else if (rtti == RTTI_BUILDING || rtti == RTTI_BUILDINGTYPE) {
		tab = FUN_005004E0(rtti, id) == 5;
	} else if (rtti == RTTI_SUPERWEAPON || rtti == RTTI_SUPERWEAPONTYPE || rtti == RTTI_SPECIAL) {
		tab = 1;
	} else {
		return;
	}
	Column[tab].FUN_006AC740(id, frames);
	unknown_53A0 = Unsorted::CurrentFrame + frames + 1;
}

// FUNCTION: GAMEMD 0x006AC740
void SidebarClass::StripClass::FUN_006AC740(int id, int frames)
{
	for (int index = 0; index < MAX_BUILDABLES; index++) {
		if (Buildables[index].BuildableID == id) {
			Buildables[index].FlashEndFrame = Unsorted::CurrentFrame + frames;
			return;
		}
	}
}

// SYNTHETIC: GAMEMD 0x006AC780
// ??_GSelectClass@StripClass@SidebarClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x006AC7A0
// ??_GSBGadgetClass@SidebarClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x006AC7C0
// SidebarClass::StripClass::BuildType::BuildType

// SYNTHETIC: GAMEMD 0x006AC7F0
// ??_GSidebarClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x006AC840
// DisplayClass::DisplayClass

// TEMPLATE: GAMEMD 0x006AC8D0
// ??0?$DynamicVectorClass@VPoint2D@@@@QAE@IPBVPoint2D@@@Z
template class DynamicVectorClass<Point2D>;
