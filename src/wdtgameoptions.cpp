// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include <math.h>

#include "wdtnet.h"
#include "geometry.h"
#undef LoadString
#include "stringtable.h"

using namespace WorldDominationTour;


WideWstring __fastcall FUN_007B8170(const WideWstring & a, const WideWstring & b);
Wstring __fastcall operator+(Wstring const & a, Wstring const & b);
// SYNTHETIC: GAMEMD 0x007670F0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00767110 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00767140 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00767160 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00767180 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x007671A0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x007671C0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x007671E0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00767220 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00767250 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00767280 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x007672B0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x007672C0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x007672E0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x00767300
// WorldDominationTour::GameOption::~GameOption
GameOption::~GameOption(void)
{
}

// TEMPLATE: GAMEMD 0x00767310
// ?Is_User_Modifiable@GameOption@WorldDominationTour@@QAE_NABV?$rc_ptr@VWDTTerritory@@@@@Z
bool GameOption::Is_User_Modifiable(const rc_ptr<WDTTerritory> & territory)
{
	return(false);
}

// TEMPLATE: GAMEMD 0x00767320
// ?Get_String@RangedGameOption@WorldDominationTour@@UAE?AVWideWstring@@ABV?$rc_ptr@VWDTTerritory@@@@ABV3@@Z
WideWstring RangedGameOption::Get_String(const rc_ptr<WDTTerritory> & territory, const WideWstring & prefix)
{
	int range = Get_In_Range_Of(territory);
	Wstring key = operator+(StringID, Wstring_FromInt(range));
	if (prefix.length() != 0) {
		WideWstring comma(L", ");
		return(FUN_007B8170(prefix, comma) + StringTable::LoadString(key.get(), 0, "D:\\ra2mdpost\\WDTGameOptions.cpp", 22));
	}
	return(WideWstring(StringTable::LoadString(key.get(), 0, "D:\\ra2mdpost\\WDTGameOptions.cpp", 25)));
}

// FUNCTION: GAMEMD 0x00767400
// WorldDominationTour::RangedGameOption::In_Range_Of
int RangedGameOption::In_Range_Of(unsigned int value)
{
	if (value <= Range0) {
		return(WDT_GAME_OPT_RANGE_VERY_LOW);
	}
	if (value <= Range1) {
		return(WDT_GAME_OPT_RANGE_LOW);
	}
	if (value >= Range3) {
		return(WDT_GAME_OPT_RANGE_VERY_HIGH);
	}
	if (value >= Range2) {
		return(WDT_GAME_OPT_RANGE_HIGH);
	}
	return(WDT_GAME_OPT_RANGE_MIDDLE);
}

// TEMPLATE: GAMEMD 0x00767440
// ?Get_Display_Priority@RangedGameOption@WorldDominationTour@@UAEHABV?$rc_ptr@VWDTTerritory@@@@@Z
int RangedGameOption::Get_Display_Priority(const rc_ptr<WDTTerritory> & territory)
{
	if (Get_In_Range_Of(territory) == WDT_GAME_OPT_RANGE_MIDDLE) {
		return(WDT_GAME_OPT_USER_MOD);
	}
	return(WDT_GAME_OPT_PRIORITY);
}

// FUNCTION: GAMEMD 0x00767460
rc_ptr<FlagGameOption> __fastcall FUN_00767460(unsigned int bitmask, Wstring off_string, int off_value, Wstring on_string, int on_value)
{
	return(rc_ptr<FlagGameOption>(new FlagGameOption(bitmask, off_string, off_value, on_string, on_value)));
}

// TEMPLATE: GAMEMD 0x007674E0
// ?Get_Display_Priority@FlagGameOption@WorldDominationTour@@UAEHABV?$rc_ptr@VWDTTerritory@@@@@Z
int FlagGameOption::Get_Display_Priority(const rc_ptr<WDTTerritory> & territory)
{
	if (territory->Booleans & Bitmask) {
		return(OnValue);
	}
	return(OffValue);
}

// TEMPLATE: GAMEMD 0x00767500
// ?Get_String@FlagGameOption@WorldDominationTour@@UAE?AVWideWstring@@ABV?$rc_ptr@VWDTTerritory@@@@ABV3@@Z
WideWstring FlagGameOption::Get_String(const rc_ptr<WDTTerritory> & territory, const WideWstring & prefix)
{
	Wstring key = (territory->Booleans & Bitmask) ? OnString : OffString;
	if (key.length() == 0) {
		return(prefix);
	}
	if (prefix.length() > 0) {
		WideWstring comma(L", ");
		return(FUN_007B8170(prefix, comma) + StringTable::LoadString(key.get(), 0, "D:\\ra2mdpost\\WDTGameOptions.cpp", 78));
	}
	return(WideWstring(StringTable::LoadString(key.get(), 0, "D:\\ra2mdpost\\WDTGameOptions.cpp", 81)));
}

// TEMPLATE: GAMEMD 0x00767600
// ?Get_Display_Priority@NumberOfPlayersGameOption@WorldDominationTour@@UAEHABV?$rc_ptr@VWDTTerritory@@@@@Z
int NumberOfPlayersGameOption::Get_Display_Priority(const rc_ptr<WDTTerritory> & territory)
{
	return(WDT_GAME_OPT_PRIORITY);
}

// TEMPLATE: GAMEMD 0x00767610
// ?Get_String@NumberOfPlayersGameOption@WorldDominationTour@@UAE?AVWideWstring@@ABV?$rc_ptr@VWDTTerritory@@@@ABV3@@Z
WideWstring NumberOfPlayersGameOption::Get_String(const rc_ptr<WDTTerritory> & territory, const WideWstring & prefix)
{
	Wstring key = (territory->NumPlayers == 4) ? "WDT:FourPlayerOnly" : "WDT:TwoPlayerOnly";
	if (prefix.length() > 0) {
		WideWstring comma(L", ");
		return(FUN_007B8170(prefix, comma) + StringTable::LoadString(key.get(), 0, "D:\\ra2mdpost\\WDTGameOptions.cpp", 104));
	}
	return(WideWstring(StringTable::LoadString(key.get(), 0, "D:\\ra2mdpost\\WDTGameOptions.cpp", 107)));
}

// FUNCTION: GAMEMD 0x007676F0
rc_ptr<MapSizeGameOption> __fastcall FUN_007676F0(unsigned int bitmask)
{
	return(rc_ptr<MapSizeGameOption>(new MapSizeGameOption(bitmask)));
}

// TEMPLATE: GAMEMD 0x00767800
// ?Get_Display_Priority@MapSizeGameOption@WorldDominationTour@@UAEHABV?$rc_ptr@VWDTTerritory@@@@@Z
int MapSizeGameOption::Get_Display_Priority(const rc_ptr<WDTTerritory> & territory)
{
	static int _table[4][4] = {
		{ 0, 0, 0, 0 },
		{ 0, 1, 2, 0 },
		{ 0, 2, 1, 0 },
		{ 0, 0, 0, 0 }
	};

	return(_table[territory->Width][territory->Height]);
}

// FUNCTION: GAMEMD 0x00767820
// WorldDominationTour::FlagGameOption::FlagGameOption

// SYNTHETIC: GAMEMD 0x00767880
// ??_GFlagGameOption@WorldDominationTour@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x007678F0 SYMBOL
// _$E44
const Rect RECT_NONE_2(0, 0, 0, 0);

// SYNTHETIC: GAMEMD 0x007678C0
// ??_GMapSizeGameOption@WorldDominationTour@@UAEPAXI@Z

