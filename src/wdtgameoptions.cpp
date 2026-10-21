// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include <math.h>

#include "wdtnet.h"
#undef LoadString
#include "stringtable.h"

using namespace WorldDominationTour;

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
extern const double DAT_007F7078;
extern const double DAT_007F7080;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00767110 $E5
double DAT_00B727B0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// FUNCTION: 0x00767140 $E8
double DAT_00B72800 = DAT_00B727B0 / DAT_007F7078;

// FUNCTION: 0x00767160 $E11
double DAT_00B727E8 = DAT_007E1718 / DAT_00B72800;

// FUNCTION: 0x00767180 $E14
double DAT_00B727E0 = DAT_007E1720 * DAT_007F7080;

// FUNCTION: 0x007671A0 $E17
double DAT_00B727B8 = DAT_007E1728 * DAT_007F7080;

// FUNCTION: 0x007671C0 $E20
double DAT_00B72790 = DAT_007E1730 * DAT_007F7080;

// FUNCTION: 0x007671E0 $E23
int DAT_00B72808 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B72790 - DAT_00B727B8) * DAT_00B727B0));

// FUNCTION: 0x00767220 $E26
double DAT_00B727C0 = FUN_004CADE0(DAT_00B72808 * DAT_007E1740);

// FUNCTION: 0x00767250 $E29
double DAT_00B727D8 = FUN_004CADE0((DAT_00B72808 * 2) / DAT_00B727B0);

// FUNCTION: 0x00767280 $E32
int DAT_00B727FC = (int)(DAT_00B72808 * 4 + DAT_007E1738);

// FUNCTION: 0x007672B0 $E35
ShortPair DAT_00B727A8;

// FUNCTION: 0x007672C0 $E38
IntTriple DAT_00B727C8;

WideWstring __fastcall FUN_007B8170(const WideWstring & a, const WideWstring & b);

Wstring __fastcall operator+(Wstring const & a, Wstring const & b);

struct IntTripleB {
	int A, B, C;
	IntTripleB() { C = 0; A = 128; B = 128; }
};

// FUNCTION: 0x007672E0 $E41
IntTripleB DAT_00B727F0;

// FUNCTION: 0x00767300 WorldDominationTour::GameOption::~GameOption
GameOption::~GameOption(void)
{
}

// FUNCTION: 0x00767310 WorldDominationTour::GameOption::Is_User_Modifiable
bool GameOption::Is_User_Modifiable(const rc_ptr<WDTTerritory> & territory)
{
	return(false);
}

// FUNCTION: 0x00767320 WorldDominationTour::RangedGameOption::Get_String
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

// FUNCTION: 0x00767400 WorldDominationTour::RangedGameOption::In_Range_Of
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

// FUNCTION: 0x00767440 WorldDominationTour::RangedGameOption::Get_Display_Priority
int RangedGameOption::Get_Display_Priority(const rc_ptr<WDTTerritory> & territory)
{
	if (Get_In_Range_Of(territory) == WDT_GAME_OPT_RANGE_MIDDLE) {
		return(WDT_GAME_OPT_USER_MOD);
	}
	return(WDT_GAME_OPT_PRIORITY);
}

// FUNCTION: 0x00767460
rc_ptr<FlagGameOption> __fastcall FUN_00767460(unsigned int bitmask, Wstring off_string, int off_value, Wstring on_string, int on_value)
{
	return(rc_ptr<FlagGameOption>(new FlagGameOption(bitmask, off_string, off_value, on_string, on_value)));
}

// FUNCTION: 0x007674E0 WorldDominationTour::FlagGameOption::Get_Display_Priority
int FlagGameOption::Get_Display_Priority(const rc_ptr<WDTTerritory> & territory)
{
	if (territory->Booleans & Bitmask) {
		return(OnValue);
	}
	return(OffValue);
}

// FUNCTION: 0x00767500 WorldDominationTour::FlagGameOption::Get_String
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

// FUNCTION: 0x00767600 WorldDominationTour::NumberOfPlayersGameOption::Get_Display_Priority
int NumberOfPlayersGameOption::Get_Display_Priority(const rc_ptr<WDTTerritory> & territory)
{
	return(WDT_GAME_OPT_PRIORITY);
}

// FUNCTION: 0x00767610 WorldDominationTour::NumberOfPlayersGameOption::Get_String
WideWstring NumberOfPlayersGameOption::Get_String(const rc_ptr<WDTTerritory> & territory, const WideWstring & prefix)
{
	Wstring key = (territory->NumPlayers == 4) ? "WDT:FourPlayerOnly" : "WDT:TwoPlayerOnly";
	if (prefix.length() > 0) {
		WideWstring comma(L", ");
		return(FUN_007B8170(prefix, comma) + StringTable::LoadString(key.get(), 0, "D:\\ra2mdpost\\WDTGameOptions.cpp", 104));
	}
	return(WideWstring(StringTable::LoadString(key.get(), 0, "D:\\ra2mdpost\\WDTGameOptions.cpp", 107)));
}

// FUNCTION: 0x007676F0
rc_ptr<MapSizeGameOption> __fastcall FUN_007676F0(unsigned int bitmask)
{
	return(rc_ptr<MapSizeGameOption>(new MapSizeGameOption(bitmask)));
}

// FUNCTION: 0x00767800 WorldDominationTour::MapSizeGameOption::Get_Display_Priority
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

// FUNCTION: 0x00767820 WorldDominationTour::FlagGameOption::FlagGameOption

// FUNCTION: 0x00767880 WorldDominationTour::FlagGameOption::~FlagGameOption

// FUNCTION: 0x007678C0 WorldDominationTour::MapSizeGameOption::~MapSizeGameOption

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// FUNCTION: 0x007678F0 $E44
IntQuad DAT_00B72818;
