// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "wdtnet.h"
#include "geometry.h"
#include "rect.h"

#include "packet.h"
#include "session.h"

#include <cstdio>
#include <new>

// SYNTHETIC: GAMEMD 0x0076A8A0 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x0076A8D0 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x0076A8F0 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x0076A910 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x0076A930 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x0076A950 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x0076A970 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x0076A9B0 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x0076A9E0 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x0076AA10 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x0076AA40 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x0076AA50 SYMBOL
// _$E38

#define PEEK_AT(data, pos) (*((unsigned char*)(data) + pos))

// FUNCTION: GAMEMD 0x0076AAB0
rc_ptr<WDTTerritory> FUN_0076AAB0(void)
{
	return rc_ptr<WDTTerritory>(new WDTTerritory);
}

// FUNCTION: GAMEMD 0x0076AAF0
WDTTerritory::WDTTerritory(void)
{
	Index = 0;
	UnitCount = 10;
	TechLevel = 10;
	StartingCredits = 10000;
	Seed = 0;
	Width = 1;
	Height = 1;
	NumPlayers = 2;
	Biome = 1;
	Time = 0;
	Field1E = 0;
	CliffsMin = 0;
	CliffsMax = 100;
	CliffsDefault = 50;
	AccessibilityMin = 0;
	AccessibilityMax = 100;
	AccessibilityDefault = 50;
	HillsMin = 0;
	HillsMax = 100;
	HillsDefault = 50;
	WaterMin = 0;
	WaterMax = 100;
	WaterDefault = 50;
	TiberiumAmountMin = 1;
	TiberiumAmountMax = 100;
	TiberiumAmountDefault = 50;
	TiberiumFieldsMin = 0;
	TiberiumFieldsMax = 100;
	TiberiumFieldsDefault = 50;
	VegetationMin = 0;
	VegetationMax = 100;
	VegetationDefault = 50;
	CitiesMin = 0;
	CitiesMax = 100;
	CitiesDefault = 50;
	Booleans = -1;
	UserModBooleans = -1;
}

// FUNCTION: GAMEMD 0x0076AB90
WDTTerritory::~WDTTerritory(void)
{
}

// FUNCTION: GAMEMD 0x0076ABA0
rc_ptr<WDTState> FUN_0076ABA0(void *data)
{
	return rc_ptr<WDTState>(new WDTState(data));
}

// FUNCTION: GAMEMD 0x0076ABE0
WDTState::WDTState(void)
{
	MapID = 0;
	CycleID = 0;
	NumTerritories = 0;
	NumTicks = 0;
	TickTime = 1000000;
	Territories = DynamicVectorClass<rc_ptr<WDTTerritory> >();
	OwnerHistory = NULL;
}

// FUNCTION: GAMEMD 0x0076AC90
WDTState::WDTState(void *data)
{
	PacketClass *packet = new PacketClass((char *)data);

	int size_mpvc = 0;
	int size_hsvc = 0;
	int size_sdsc = 0;
	int size_ldsc = 0;
	int size_wurl = 0;
	int size_temp = 0;

	ShortDesc = NULL;
	LongDesc = NULL;
	WebURL = NULL;
	MapID = 0;
	CycleID = 0;
	CycleLength = 0;
	NumTerritories = 0;
	NumTicks = 0;
	TickTime = 0;
	OwnerHistory = NULL;

	if (packet->Get_Field("NACK", NULL, size_temp)) {
		delete packet;
	} else {
		if (packet->Get_Field("TOTK", CycleLength)) {
			IsPreviousCycle = true;
			CycleLength++;
			NumTicks = CycleLength;
		} else {
			IsPreviousCycle = false;
			packet->Get_Field("CRTK", NumTicks);
			NumTicks++;
			packet->Get_Field("CYLN", CycleLength);
			CycleLength++;
		}

		packet->Get_Field("WURL", NULL, size_wurl);
		packet->Get_Field("SDSC", NULL, size_sdsc);
		packet->Get_Field("LDSC", NULL, size_ldsc);

		if (!IsPreviousCycle) {
			packet->Get_Field("MPVC", NULL, size_mpvc);
		}
		packet->Get_Field("HSVC", NULL, size_hsvc);

		ShortDesc = new char[size_sdsc];
		LongDesc = new char[size_ldsc];
		WebURL = new char[size_wurl];

		unsigned char *hsvc_ptr = new unsigned char[size_hsvc];
		unsigned char *mpvc_ptr = NULL;

		if (!IsPreviousCycle) {
			mpvc_ptr = new unsigned char[size_mpvc];
			memset(mpvc_ptr, 0, size_mpvc);
		}

		memset(ShortDesc, 0, size_sdsc);
		memset(LongDesc, 0, size_ldsc);
		memset(WebURL, 0, size_wurl);
		memset(hsvc_ptr, 0, size_hsvc);

		packet->Get_Field("CMID", MapID);
		packet->Get_Field("CYID", CycleID);

		if (IsPreviousCycle) {
			TickTime = 0;
		} else {
			packet->Get_Field("TICC", TickTime);
		}

		packet->Get_Field("SDSC", ShortDesc, size_sdsc);
		packet->Get_Field("LDSC", LongDesc, size_ldsc);
		packet->Get_Field("WURL", WebURL, size_wurl);

		if (!IsPreviousCycle) {
			packet->Get_Field("MPVC", mpvc_ptr, size_mpvc);
		}

		packet->Get_Field("HSVC", hsvc_ptr, size_hsvc);

		if (!IsPreviousCycle) {
			Create_Territories(mpvc_ptr, size_mpvc);
		} else {
			NumTerritories = 30;
		}

		Create_History(hsvc_ptr, size_hsvc);

		delete [] hsvc_ptr;

		if (!IsPreviousCycle) {
			delete [] mpvc_ptr;
		}

		delete packet;
	}
}

// FUNCTION: GAMEMD 0x0076AFC0
WDTState::~WDTState(void)
{
	Territories.Clear();

	if (LongDesc != NULL) {
		delete [] LongDesc;
	}
	if (ShortDesc != NULL) {
		delete [] ShortDesc;
	}
	if (WebURL != NULL) {
		delete [] WebURL;
	}

	if (OwnerHistory != NULL) {
		for (int i = 0; i < (int)NumTicks; i++) {
			if (OwnerHistory[i] != NULL) {
				delete [] OwnerHistory[i];
			}
		}
		delete [] OwnerHistory;
	}
}

// FUNCTION: GAMEMD 0x0076B080
bool WDTState::Has_Owner_History(void)
{
	if (OwnerHistory != NULL) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0076B090
bool WDTState::Is_Previous_Cycle(void)
{
	return(IsPreviousCycle);
}

// FUNCTION: GAMEMD 0x0076B0A0
bool WDTState::Create_History(void *data, int count)
{
	unsigned int i;

	if (4 * count < NumTicks * NumTerritories) {
		return(false);
	}

	OwnerHistory = new unsigned char *[NumTicks];

	for (i = 0; i < NumTicks; i++) {
		OwnerHistory[i] = new unsigned char[NumTerritories];
	}

	int pos = 0;
	int index = 0;

	for (unsigned int tick = 0; tick < NumTicks; tick++) {
		for (unsigned int territory = 0; territory < NumTerritories; territory++) {
			switch (index) {
				case CONTESTED:
					OwnerHistory[tick][territory] = (PEEK_AT(data, pos)) & 3;
					break;

				case GDI:
					OwnerHistory[tick][territory] = (PEEK_AT(data, pos) >> 2) & 3;
					break;

				case NOD:
					OwnerHistory[tick][territory] = (PEEK_AT(data, pos) >> 4) & 3;
					break;

				default:
					OwnerHistory[tick][territory] = (PEEK_AT(data, pos) >> 6) & 3;
					break;
			}

			index++;
			if (index == 4) {
				index = 0;
				pos++;
			}
		}

		if (index) {
			index = 0;
			pos++;
		}
	}
	return(true);
}

// SYNTHETIC: GAMEMD 0x0076B6C0
// ??_GWDTTerritory@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0076B6F0
// ??_GWDTState@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0076B710
// ?Clear@?$DynamicVectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0076B740
// ?ID@?$DynamicVectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAEHPBV?$rc_ptr@VWDTTerritory@@@@@Z
// TEMPLATE: GAMEMD 0x0076B760
// ?GetItem@?$VectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UBE?AV?$rc_ptr@VWDTTerritory@@@@H@Z
// TEMPLATE: GAMEMD 0x0076B790
// ??0?$rc_ptr@VWDTTerritory@@@@QAE@PAVWDTTerritory@@@Z
// TEMPLATE: GAMEMD 0x0076B7B0
// ??4?$VectorClass@V?$rc_ptr@VWDTTerritory@@@@@@QAEAAV0@ABV0@@Z
// TEMPLATE: GAMEMD 0x0076B870
// ??8?$VectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0076B8C0
// ?Resize@?$VectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAE_NHPBV?$rc_ptr@VWDTTerritory@@@@@Z
// TEMPLATE: GAMEMD 0x0076B9E0
// ?Clear@?$VectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0076BA10
// ?ID@?$VectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAEHABV?$rc_ptr@VWDTTerritory@@@@@Z
// TEMPLATE: GAMEMD 0x0076BA50
// ?ID@?$VectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAEHPBV?$rc_ptr@VWDTTerritory@@@@@Z
// TEMPLATE: GAMEMD 0x0076BA70
// ?Resize@?$DynamicVectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAE_NHPBV?$rc_ptr@VWDTTerritory@@@@@Z

// TEMPLATE: GAMEMD 0x0076BB90
// ?ID@?$DynamicVectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAEHABV?$rc_ptr@VWDTTerritory@@@@@Z

// SYNTHETIC: GAMEMD 0x0076BBC0
// ??_G?$VectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0076BC10
// ??_G?$DynamicVectorClass@V?$rc_ptr@VWDTTerritory@@@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0076BC60
// ??0?$rc_ptr@VWDTTerritory@@@@QAE@XZ

template class DynamicVectorClass<rc_ptr<WDTTerritory> >;
template class rc_ptr<WDTTerritory>;
