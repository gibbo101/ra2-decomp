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
extern const double DAT_007F7280;
extern const double DAT_007F7288;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x0076BCA0 SYMBOL
// _$E2
double DAT_00B729B0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0076BCD0 SYMBOL
// _$E5
double DAT_00B72A00 = DAT_00B729B0 / DAT_007F7280;

// SYNTHETIC: GAMEMD 0x0076BCF0 SYMBOL
// _$E8
double DAT_00B729E8 = DAT_007E1718 / DAT_00B72A00;

// SYNTHETIC: GAMEMD 0x0076BD10 SYMBOL
// _$E11
double DAT_00B729E0 = DAT_007E1720 * DAT_007F7288;

// SYNTHETIC: GAMEMD 0x0076BD30 SYMBOL
// _$E14
double DAT_00B729B8 = DAT_007E1728 * DAT_007F7288;

// SYNTHETIC: GAMEMD 0x0076BD50 SYMBOL
// _$E17
double DAT_00B72990 = DAT_007E1730 * DAT_007F7288;

// SYNTHETIC: GAMEMD 0x0076BD70 SYMBOL
// _$E20
int DAT_00B72A08 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B72990 - DAT_00B729B8) * DAT_00B729B0));

// SYNTHETIC: GAMEMD 0x0076BDB0 SYMBOL
// _$E23
double DAT_00B729C0 = FUN_004CADE0(DAT_00B72A08 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0076BDE0 SYMBOL
// _$E26
double DAT_00B729D8 = FUN_004CADE0((DAT_00B72A08 * 2) / DAT_00B729B0);

// SYNTHETIC: GAMEMD 0x0076BE10 SYMBOL
// _$E29
int DAT_00B729FC = (int)(DAT_00B72A08 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0076BE40 SYMBOL
// _$E32
ShortPair DAT_00B729A8;

// SYNTHETIC: GAMEMD 0x0076BE50 SYMBOL
// _$E35
IntTriple DAT_00B729C8;

#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "always.h"
#include "stringtable.h"
#include "wdtnet.h"

using namespace WorldDominationTour;
extern char DAT_00889F64[];
extern int DAT_00B729F0, DAT_00B729F4, DAT_00B729F8, DAT_00B72A18, DAT_00B72A1C, DAT_00B72A20, DAT_00B72A24;

// FUNCTION: GAMEMD 0x0076BE70
void FUN_0076BE70(void)
{
	DAT_00B729F8 = 0;
	DAT_00B729F0 = 0x80;
	DAT_00B729F4 = 0x80;
}

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
int WorldDominationTour::CampaignProperties::FUN_0076BFA0(void)
{
	if (State.Get()) {
		return State->CycleID;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0076BFB0
int WorldDominationTour::CampaignProperties::FUN_0076BFB0(void)
{
	return 1;
}

// FUNCTION: GAMEMD 0x0076BFC0
int WorldDominationTour::CampaignProperties::FUN_0076BFC0(void)
{
	if (State.Get()) {
		return State->MapID;
	}
	return 1;
}

// FUNCTION: GAMEMD 0x0076BFE0
int WorldDominationTour::CampaignProperties::FUN_0076BFE0(void)
{
	if (State.Get()) {
		return State->NumTicks - 1;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x0076BFF0
Wstring WorldDominationTour::CampaignProperties::FUN_0076BFF0(void)
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

// FUNCTION: GAMEMD 0x0076C080
void FUN_0076C080(void)
{
	DAT_00B72A18 = 0;
	DAT_00B72A1C = 0;
	DAT_00B72A20 = 0;
	DAT_00B72A24 = 0;
}
