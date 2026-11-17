// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"

#include <math.h>
#include "geometry.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "always.h"
#include "stringtable.h"
#include "wdtnet.h"

// SYNTHETIC: GAMEMD 0x0076BC80 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x0076BCA0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0076BCD0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0076BCF0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0076BD10 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0076BD30 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0076BD50 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0076BD70 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0076BDB0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0076BDE0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0076BE10 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0076BE40 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0076BE50 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0076BE70 SYMBOL
// _$E38

using namespace WorldDominationTour;
extern char DAT_00889F64[];
extern int DAT_00B72A18, DAT_00B72A1C, DAT_00B72A20, DAT_00B72A24;

// FUNCTION: GAMEMD 0x0076BE90
rc_ptr<WorldDominationTour::CampaignProperties> __fastcall FUN_0076BE90(rc_ptr<WDTState> const & state)
{
	return rc_ptr<WorldDominationTour::CampaignProperties>(new WorldDominationTour::CampaignProperties(state));
}

// FUNCTION: GAMEMD 0x0076BED0
WorldDominationTour::CampaignProperties::~CampaignProperties(void)
{
}

// FUNCTION: GAMEMD 0x0076BF00
WorldDominationTour::CampaignProperties::CampaignProperties(rc_ptr<WDTState> const & state) :
	State(state),
	ShortDesc(state.Get() ? StringTable::LoadString(state->ShortDesc, NULL, "D:\\ra2mdpost\\WDTProps.cpp", 0x14) : StringTable::LoadString("\"WDT:DefaultShortDescription\"", NULL, "D:\\ra2mdpost\\WDTProps.cpp", 0x14)),
	LongDesc(state.Get() ? StringTable::LoadString(state->LongDesc, NULL, "D:\\ra2mdpost\\WDTProps.cpp", 0x15) : StringTable::LoadString("\"WDT:DefaultLongDescription\"", NULL, "D:\\ra2mdpost\\WDTProps.cpp", 0x15))
{
}

// FUNCTION: GAMEMD 0x0076BFA0
int WorldDominationTour::CampaignProperties::Get_Cycle_ID(void)
{
	if (State.Get()) {
		return State->CycleID;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0076BFB0
int WorldDominationTour::CampaignProperties::Get_Layout(void)
{
	return 1;
}

// FUNCTION: GAMEMD 0x0076BFC0
int WorldDominationTour::CampaignProperties::Get_Map_ID(void)
{
	if (State.Get()) {
		return State->MapID;
	}
	return 1;
}

// FUNCTION: GAMEMD 0x0076BFE0
int WorldDominationTour::CampaignProperties::Get_Current_Tick(void)
{
	if (State.Get()) {
		return State->NumTicks - 1;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0076BFF0
Wstring WorldDominationTour::CampaignProperties::Get_Web_URL(void)
{
	char const * url;
	if (State.Get()) {
		url = State->WebURL;
	} else {
		url = DAT_00889F64;
	}
	return Wstring(url);
}

// SYNTHETIC: GAMEMD 0x0076C030
// ??_GCampaignProperties@WorldDominationTour@@UAEPAXI@Z
