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
#include "super.h"
#include "suprtype.h"
#include "surface.h"
#include "voc.h"
#include "vox.h"
#include "bench.hh"
#include "color.hh"
#include <cmath>

// FUNCTION: 0x006A4A00 $E2
// FUNCTION: 0x006A4A20 $E5
// FUNCTION: 0x006A4A50 $E8
// FUNCTION: 0x006A4A70 $E11
// FUNCTION: 0x006A4A90 $E14
// FUNCTION: 0x006A4AB0 $E17
// FUNCTION: 0x006A4AD0 $E20
// FUNCTION: 0x006A4AF0 $E23
// FUNCTION: 0x006A4B30 $E26
// FUNCTION: 0x006A4B60 $E29
// FUNCTION: 0x006A4B90 $E32
// FUNCTION: 0x006A4BC0 $E35
// FUNCTION: 0x006A4BD0 $E38
// FUNCTION: 0x006A4BF0 $E41

SidebarClass::StripClass::SelectClass::SelectClass(void) :
	ControlClass(0, 0, 0, 60, 48, LEFTPRESS|RIGHTPRESS|LEFTUP),
	Strip(0),
	Index(0)
{
}

// FUNCTION: 0x006A4C10 $E175
// FUNCTION: 0x006A4C40 $E172
SidebarClass::SBGadgetClass SidebarClass::Background;

// FUNCTION: 0x006A4E40
SidebarClass::SBGadgetClass::SBGadgetClass(void) :
	GadgetClass(0, 0, 1, 1, LEFTUP)
{
}

// FUNCTION: 0x006A4C60 $E181
// FUNCTION: 0x006A4C80 $E178
ShapeButtonClass SidebarClass::Power;
// FUNCTION: 0x006A4CA0 $E186
// FUNCTION: 0x006A4CC0 $E184
ShapeButtonClass SidebarClass::Repair;
// FUNCTION: 0x006A4CE0 $E191
// FUNCTION: 0x006A4D10 $E189
ShapeButtonClass SidebarClass::TabButtons[4];
// FUNCTION: 0x006A4D40 $E196
// FUNCTION: 0x006A4D60 $E194
ShapeButtonClass SidebarClass::Upgrade;
// FUNCTION: 0x006A4D80 $E201
// FUNCTION: 0x006A4DA0 $E199
ShapeButtonClass SidebarClass::Waypoint;

// FUNCTION: 0x006A4DC0 $E206
// FUNCTION: 0x006A4E10 $E204
SidebarClass::StripClass::SelectClass SidebarClass::StripClass::SelectButton[SidebarClass::StripClass::SELECT_BUTTONS];

// FUNCTION: 0x006A4E60
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

// FUNCTION: 0x006A4F20
SidebarClass::SidebarClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x006A4FF0
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

void SidebarClass::StripClass::One_Time(int)
{
	FileSystem::DARKEN_SHP = FileSystem::LoadFile("DARKEN.SHP", false);
}

// FUNCTION: 0x006A5000
void SidebarClass::One_Time(void)
{
	BASECLASS::One_Time();

	for (int index = 0; index < STRIPS; index++) {
		Column[index].One_Time(index);
	}
}

// FUNCTION: 0x006A5030
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

// FUNCTION: 0x006A5090
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

// FUNCTION: 0x006A5310
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

// FUNCTION: 0x006A5840
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

// FUNCTION: 0x006A5BF0
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

// FUNCTION: 0x006A5ED0
void SidebarClass::FUN_006A5ED0(void)
{
	IsToRedraw = true;
	Column[ActiveTabIndex].IsToRedraw = true;
	Map.Flag_To_Redraw();
	if (unknown_53A0 <= Unsorted::CurrentFrame) {
		unknown_53A0 = Unsorted::CurrentFrame + 2;
	}
}

// FUNCTION: 0x006A5F20
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

// FUNCTION: 0x006A60A0
void SidebarClass::RepaintSidebar(int tab)
{
	if (tab == ActiveTabIndex) {
		IsToRedraw = true;
		Column[tab].IsToRedraw = true;
		Map.Flag_To_Redraw();
	}
}

// FUNCTION: 0x006A60E0
void SidebarClass::FUN_006A60E0(int tab)
{
	if (tab == ActiveTabIndex) {
		FUN_006A5ED0();
	}
}

int __fastcall FUN_005004E0(RTTIType type, int id);

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

// FUNCTION: 0x006A6140
bool SidebarClass::Factory_Link(FactoryClass * factory, RTTIType type, int id)
{
	return Column[GetObjectTabIdx(type, id, false)].Factory_Link(factory, type, id);
}

// FUNCTION: 0x006A6220
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

// FUNCTION: 0x006A6290
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

// FUNCTION: 0x006A6300
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

// FUNCTION: 0x006A6610
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

// FUNCTION: 0x006A67A0
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

// FUNCTION: 0x006A6820
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

// FUNCTION: 0x006ABC60
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

// FUNCTION: 0x006AACE0
void SidebarClass::StripClass::SelectClass::Set_Owner(StripClass & strip, int index)
{
	Strip = &strip;
	Index = index;
}
