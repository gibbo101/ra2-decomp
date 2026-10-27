// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "data.h"
#include "language\language.h"
#include "wdtnet.h"
#include "rect.h"
#include "geometry.h"
#include <math.h>

using namespace WorldDominationTour;


// SYNTHETIC: GAMEMD 0x00765D50 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x00765D70 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x00765DA0 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x00765DC0 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x00765DE0 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x00765E00 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x00765E20 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x00765E40 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x00765E80 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x00765EB0 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x00765EE0 SYMBOL
// _$E32


// SYNTHETIC: GAMEMD 0x00765F10 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x00765F20 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x00765F40 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x00765F60
rc_ptr<Conflict> __fastcall FUN_00765F60(rc_ptr<WDTTerritory> const & territory)
{
	return(rc_ptr<Conflict>(new Conflict(territory)));
}

// FUNCTION: GAMEMD 0x00765FD0
// WorldDominationTour::Conflict::Conflict
Conflict::Conflict(rc_ptr<WDTTerritory> const & territory) :
	Territory(territory)
{
}

// FUNCTION: GAMEMD 0x00766010
// WorldDominationTour::Conflict::operator==
bool Conflict::operator==(Conflict const & that) const
{
	return(Territory == that.Territory);
}

// FUNCTION: GAMEMD 0x00766030
// WorldDominationTour::Conflict::~Conflict
Conflict::~Conflict(void)
{
}

// FUNCTION: GAMEMD 0x00766050
// WorldDominationTour::Conflict::Is_Territory_Set
bool Conflict::Is_Territory_Set(void) const
{
	return(Territory.Get() != NULL);
}

// FUNCTION: GAMEMD 0x00766060
// WorldDominationTour::Conflict::FUN_00766060
int Conflict::FUN_00766060(void)
{
	if (Territory.Get() != NULL) {
		return(Territory->Index);
	}
	return(0);
}

WDT_GAME_OPTION_LIST WorldDominationTour::WDTGameOptionList;

// SYNTHETIC: GAMEMD 0x00766070 SYMBOL
// _$E46

// SYNTHETIC: GAMEMD 0x007660B0 SYMBOL
// _$E44

rc_ptr<FlagGameOption> __fastcall FUN_00767460(unsigned int bitmask, Wstring off_string, int off_value, Wstring on_string, int on_value);
rc_ptr<MapSizeGameOption> __fastcall FUN_007676F0(unsigned int bitmask);

// FUNCTION: GAMEMD 0x007660F0
// WorldDominationTour::Conflict::Init_Game_Options
void Conflict::Init_Game_Options(void)
{
	WDTGameOptionList.Clear();

	WDTGameOptionList.Add(rc_ptr<GameOption>(FUN_00766880<unsigned char>(0x10000, &WDTTerritory::FUN_007667F0, Wstring("WDT:TechLevel"), 0)));
	WDTGameOptionList.Add(rc_ptr<GameOption>(FUN_00767460(0x10, Wstring("WDT:NoBases"), 0, Wstring("WDT:Bases"), 1)));
	WDTGameOptionList.Add(rc_ptr<GameOption>(FUN_00767460(0x400, Wstring("WDT:NotShortGame"), 1, Wstring("WDT:ShortGame"), 0)));
	WDTGameOptionList.Add(rc_ptr<GameOption>(FUN_00766880<unsigned char>(0x8000, &WDTTerritory::FUN_007667E0, Wstring("WDT:UnitCount"), 0)));
	WDTGameOptionList.Add(rc_ptr<GameOption>(FUN_00767460(0x20, Wstring("WDT:NoRedeployMCV"), 1, Wstring("WDT:RedeployableMCV"), 0)));
	WDTGameOptionList.Add(rc_ptr<GameOption>(FUN_00767460(0x100, Wstring("WDT:NoCrates"), 1, Wstring("WDT:Crates"), 1)));
	WDTGameOptionList.Add(FUN_007676F0(0xc000000));
}

// FUNCTION: GAMEMD 0x00766590
// WorldDominationTour::Conflict::Deinit_Game_Options
void Conflict::Deinit_Game_Options(void)
{
	WDTGameOptionList.Clear();
}

template class DynamicVectorClass<rc_ptr<GameOption> >;

// FUNCTION: GAMEMD 0x007667E0
// WDTTerritory::FUN_007667E0

// FUNCTION: GAMEMD 0x007667F0
// WDTTerritory::FUN_007667F0

// TEMPLATE: GAMEMD 0x00766880
// ?FUN_00766880@WorldDominationTour@@YI?AV?$rc_ptr@V?$ValueGameOption@E@WorldDominationTour@@@@IP8WDTTerritory@@AEHXZVWstring@@H@Z

// TEMPLATE: GAMEMD 0x00766A10
// ??0?$ValueGameOption@E@WorldDominationTour@@QAE@IP8WDTTerritory@@AEHXZVWstring@@H@Z

// TEMPLATE: GAMEMD 0x00766A80
// ?Get_Display_Priority@?$ValueGameOption@E@WorldDominationTour@@UAEHABV?$rc_ptr@VWDTTerritory@@@@@Z

// TEMPLATE: GAMEMD 0x00766850
// ?GetItem@?$VectorClass@V?$rc_ptr@VGameOption@WorldDominationTour@@@@@@UBE?AV?$rc_ptr@VGameOption@WorldDominationTour@@@@H@Z

// TEMPLATE: GAMEMD 0x007668F0
// ??1?$rc_ptr@VGameOption@WorldDominationTour@@@@UAE@XZ

// TEMPLATE: GAMEMD 0x00766900
// ??1?$rc_ptr@VFlagGameOption@WorldDominationTour@@@@UAE@XZ

// TEMPLATE: GAMEMD 0x00766910
// ??1?$rc_ptr@VMapSizeGameOption@WorldDominationTour@@@@UAE@XZ

// SYNTHETIC: GAMEMD 0x00766A60
// ??_GGameOption@WorldDominationTour@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00766B90
// ??_G?$ValueGameOption@E@WorldDominationTour@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00766D30
// ?Clear@?$VectorClass@V?$rc_ptr@VGameOption@WorldDominationTour@@@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00766EE0
// ?Add@?$DynamicVectorClass@V?$rc_ptr@VGameOption@WorldDominationTour@@@@@@QAE_NABV?$rc_ptr@VGameOption@WorldDominationTour@@@@@Z

// TEMPLATE: GAMEMD 0x00766F70
// ??0?$rc_ptr@VGameOption@WorldDominationTour@@@@QAE@ABV?$rc_ptr@VMapSizeGameOption@WorldDominationTour@@@@@Z
