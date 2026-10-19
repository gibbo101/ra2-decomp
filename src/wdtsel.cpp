// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "language\language.h"

#include "_keyboar.h"
#include "_surface.h"
#include "ccfile.h"
#include "convert.h"
#include "data.h"
#include "globals.h"
#include "ini.h"
#include "keyboard.h"
#include "mschoice.h"
#include "msfont.h"
#include "newmenu.h"
#include "netshare.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "session.h"
#include "theme.h"
#include "vector.h"
#include "wdtnet.h"
#include "worlddom.h"

#define IID_DEFINED
#include "wolapi\wolapi.h"

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F72A0;
extern const double DAT_007F72A8;

// FUNCTION: 0x0076C0A0 $E31
double DAT_00B72A30 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0076C0D0 $E34
double DAT_00B72A80 = DAT_00B72A30 / DAT_007F72A0;

// FUNCTION: 0x0076C0F0 $E37
double DAT_00B72A68 = DAT_007E1718 / DAT_00B72A80;

// FUNCTION: 0x0076C110 $E40
double DAT_00B72A60 = DAT_007E1720 * DAT_007F72A8;

// FUNCTION: 0x0076C130 $E43
double DAT_00B72A38 = DAT_007E1728 * DAT_007F72A8;

// FUNCTION: 0x0076C150 $E46
double DAT_00B72A10 = DAT_007E1730 * DAT_007F72A8;

// FUNCTION: 0x0076C170 $E49
int DAT_00B72A88 = (int)(fastmath::tan(DAT_00B72A10 - DAT_00B72A38) * DAT_007E1738 * DAT_00B72A30);

// FUNCTION: 0x0076C1B0 $E52
double DAT_00B72A40 = fastmath::atan(DAT_00B72A88 * DAT_007E1740);

// FUNCTION: 0x0076C1E0 $E55
double DAT_00B72A58 = fastmath::atan((DAT_00B72A88 * 2) / DAT_00B72A30);

// FUNCTION: 0x0076C210 $E58
int DAT_00B72A7C = (int)(DAT_00B72A88 * 4 + DAT_007E1738);

struct WDTShortPair { short a; short b; WDTShortPair(void) { a = 0; b = 0; } };
struct WDTIntTriple { int a; int b; int c; WDTIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x0076C240 $E61
WDTShortPair DAT_00B72A28;

// FUNCTION: 0x0076C250 $E64
WDTIntTriple DAT_00B72A48;

struct WDTAnchor { int a; int b; int c; WDTAnchor(void) { c = 0; a = 0x80; b = 0x80; } };

// FUNCTION: 0x0076C270 $E67
WDTAnchor DAT_00B72A70;

using namespace WorldDominationTour;

// FUNCTION: 0x0076C970 WorldDominationTour::Selection::Init_Voices
void Selection::Init_Voices(INIClass const & ini, Wstring const & section)
{
	VoicePlayer.FUN_00770550(ini, section);
	{ Wstring key("WinningThreshold"); WinningThreshold = ini.Get_Int(section, key, WinningThreshold); }
	{ Wstring key("MegaWinningThreshold"); MegaWinningThreshold = ini.Get_Int(section, key, MegaWinningThreshold); }
	{ Wstring key("LosingThreshold"); LosingThreshold = ini.Get_Int(section, key, LosingThreshold); }
	{ Wstring key("MegaLosingThreshold"); MegaLosingThreshold = ini.Get_Int(section, key, MegaLosingThreshold); }
}

// FUNCTION: 0x0076CA60 WorldDominationTour::Selection::Init_Sounds
void Selection::Init_Sounds(INIClass const & ini, Wstring const & section)
{
	Wstring buffer;

	int count = ini.Entry_Count(section);
	for (int i = 0; i < count; i++) {
		Wstring entry = ini.Get_Entry(section, i);
		if (entry != (char const *)NULL) {
			bool found;
			{
				Wstring none((char const *)NULL);
				found = ini.Get_String(section, entry, none, buffer) > 0;
			}
			if (found) {
				if (entry == "Theme") {
					ThemeName = buffer;
					break;
				}
				MSSfxEntry * sfx = new MSSfxEntry(entry, buffer);
				if (sfx->Get_Name() == (char const *)NULL) {
					delete sfx;
				} else {
					Add_Sfx_Entry(sfx);
				}
			}
		}
	}
}

// FUNCTION: 0x0076CBF0 WorldDominationTour::Selection::Init_Target
void Selection::Init_Target(INIClass const & ini, Wstring const & section)
{
	Wstring buffer;
	int faction = TourCampaign.Get()->PlayerFaction;
	Wstring faction_name = *FUN_007AF2A0(faction);

	bool found;
	{
		Wstring none((char const *)NULL);
		found = ini.Get_String(section, faction_name + "ZoomingTarget", none, buffer) > 0;
	}
	if (found) {
		ZoomingTarget = new MSAnimEntry(buffer);
	}

	{
		Wstring none((char const *)NULL);
		ini.Get_String(section, faction_name + "TargetPalette", none, buffer);
	}
	TargetDrawer = Create_Drawer(buffer);

	{
		Wstring none((char const *)NULL);
		ini.Get_String(section, faction_name + "Target", none, buffer);
	}
	ThrobbingTarget = new MSAnimEntry(buffer);

	ThrobbingTargetDividingFrame = ini.Get_Int(section, faction_name + "TargetDividingFrame", 32);
}

// FUNCTION: 0x0076D470 WorldDominationTour::Selection::Update_Hovered_Territory
void Selection::Update_Hovered_Territory(Point2D const & point)
{
	int id = WorldMap.Get()->FUN_00769330(point);
	if (id != HoveredTerritoryID) {
		HoveredTerritoryID = id;
		Select_Territory(WorldMap.Get()->FUN_0076F000(id));
	}
}

class InputManagerClass {
public:
	static InputManagerClass * Instance;
	short FUN_0054F000(void);
	short FUN_0054F050(void);
};

typedef VectorCursor<MSSfxEntry *, DynamicVectorClass<MSSfxEntry *> > WDT_SFX_ITER;

static inline void Play_Sfx_Named(Selection * sel, char const * text)
{
	Wstring name(text);
	if (name != (char const *)NULL) {
		WDT_SFX_ITER it(sel->SfxEntries, 0);
		for (; it; it++) {
			if (!(name == it->Get_Name())) {
				break;
			}
		}
		if (it) {
			it->Play();
		}
	}
}

// FUNCTION: 0x0076DFD0 WorldDominationTour::Selection::On_Map_Enter
void Selection::On_Map_Enter(void)
{
	Play_Sfx_Named(this, "MapEnter");
}

// FUNCTION: 0x0076E090 WorldDominationTour::Selection::On_Map_Leave
void Selection::On_Map_Leave(void)
{
	Play_Sfx_Named(this, "MapLeave");
	HoveredTerritoryID = -1;
	YRTerritoryHandle none;
	Select_Territory(none);
}

// FUNCTION: 0x0076E9D0 WorldDominationTour::Selection::Check_For_Breakout
void Selection::Check_For_Breakout(void)
{
	Wait_For_Focus();
	while (InputManagerClass::Instance->FUN_0054F000()) {
		if (InputManagerClass::Instance->FUN_0054F050() == VK_ESCAPE) {
			Brokeout = true;
		}
	}
}

// FUNCTION: 0x0076F000 WorldDominationTour::YRMapImpl::FUN_0076F000
YRTerritoryHandle YRMapImpl::FUN_0076F000(int id)
{
	return(FUN_0076FF40(&Territories, id));
}

// FUNCTION: 0x0076F030 WorldDominationTour::YRTerritoryHandle::YRTerritoryHandle
YRTerritoryHandle::YRTerritoryHandle(YRTerritoryHandle const & that)
{
	FUN_0065D480(that.Ptr);
}

// FUNCTION: 0x0076F060 WorldDominationTour::YRTerritoryHandle::~YRTerritoryHandle
YRTerritoryHandle::~YRTerritoryHandle(void)
{
}
