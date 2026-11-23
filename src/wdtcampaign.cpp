// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <math.h>
#include "ccfile.h"
#include "ini.h"
#include "wdtnet.h"

using namespace WorldDominationTour;

// SYNTHETIC: GAMEMD 0x00764810 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x00764830 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00764860 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00764880 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x007648A0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x007648C0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x007648E0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00764900 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00764940 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00764970 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x007649A0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x007649D0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x007649E0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00764A00 SYMBOL
// _$E38

Wstring & __fastcall WDT_Country_Name(int index);
// FUNCTION: GAMEMD 0x00764A20
int WDT_Get_Campaign_ID(Wstring const & section)
{
	CDFileClass file("WDTHist.INI");
	if (file.Is_Available()) {
		INIClass ini;
		ini.Load(file);
		file.Close();
		return(ini.Get_Int(section, "CampaignID", -1));
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x00764B30
rc_ptr<Campaign> __fastcall FUN_00764B30(Wstring const & name, rc_ptr<CampaignProperties> const & props)
{
	if (props.Get() == NULL) {
		return rc_ptr<Campaign>();
	}
	return rc_ptr<Campaign>(new Campaign(name, props));
}

static inline int Nonnegative(int value)
{
	return value < 0 ? 0 : value;
}

// FUNCTION: GAMEMD 0x00764B90
// WorldDominationTour::Campaign::Campaign
Campaign::Campaign(Wstring const &name, rc_ptr<CampaignProperties> const &properties)
	: Properties(properties), CycleHistory(FUN_00767B00(properties->State)),
	  Conflicts(4), PlayerFaction(0), Name(name)
{
	memset(PreviousTicks, 0, sizeof(PreviousTicks));
	if (properties->State.Get() != 0) {
		rc_ptr<State> state = CycleHistory->Get_State(properties->Get_Current_Tick());
		unsigned index = properties->State->NumTerritories;
		while (index--) {
			if (state->Get_Territory_State(index) == 1) {
				DynamicVectorClass<rc_ptr<WDTTerritory> > &territories = properties->State->Territories;
				rc_ptr<WDTTerritory> territory = index < (unsigned)territories.Count()
					? territories[index] : rc_ptr<WDTTerritory>();
				if (territory.Get() != 0) {
					rc_ptr<Conflict> conflict = FUN_00765F60(territory);
					Conflicts.Add(conflict);
				}
			}
		}
	}
	CDFileClass file("WDTHist.INI");
	if (file.Is_Available()) {
		INIClass ini;
		ini.Load(file, false);
		file.Close();
		int saved_id;
		{
			Wstring key("CampaignID");
			saved_id = ini.Get_Int(Name, key, -1);
		}
		CycleID = Nonnegative(saved_id);
		if (CycleID == properties->Get_Cycle_ID()) {
			for (int faction = 0; faction < 4; faction++) {
				Wstring key = Wstring("LastDay") + WDT_Country_Name(faction);
				int day = ini.Get_Int(Name, key, 0);
				PreviousTicks[faction] = Nonnegative(day);
			}
			{
				Wstring key("PlayerFaction");
				PlayerFaction = ini.Get_Int(Name, key, PlayerFaction);
			}
		} else {
			memset(PreviousTicks, 0, sizeof(PreviousTicks));
		}
	}
}


// FUNCTION: GAMEMD 0x00764F30
WorldDominationTour::Campaign::~Campaign(void)
{
}

// FUNCTION: GAMEMD 0x00764FA0
bool WorldDominationTour::Campaign::Is_Different_Cycle(void)
{
	return(CycleID != Properties->Get_Cycle_ID());
}

// FUNCTION: GAMEMD 0x00764FC0
bool WorldDominationTour::Campaign::Is_Different(int index)
{
	if (Is_Different_Cycle()) {
		return(true);
	}
	if (PreviousTicks[index] != Properties->Get_Current_Tick()) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00764FF0
int WorldDominationTour::Campaign::Get_Previous_Tick(int index)
{
	return(PreviousTicks[index]);
}

static inline void Load_History(INIClass & ini)
{
	CDFileClass file;
	file.Open("WDTHist.ini", 1);
	if (!file.Is_Available()) {
		file.Create();
		file.Close();
		file.Open("WDTHist.ini", 1);
	}
	ini.Load(file);
	file.Close();
}

// FUNCTION: GAMEMD 0x00765000
void WorldDominationTour::Campaign::Write_INI(int index)
{
	CycleID = Properties->Get_Cycle_ID();
	PreviousTicks[index] = Properties->Get_Current_Tick();
	INIClass ini;
	Load_History(ini);
	Wstring key = Wstring("LastDay") + WDT_Country_Name(index);
	ini.Put_Int(Name, "CampaignID", CycleID);
	ini.Put_Int(Name, key, PreviousTicks[index]);
	CDFileClass file;
	file.Open("WDTHist.ini", 2);
	ini.Save(file);
}

// FUNCTION: GAMEMD 0x007651D0
void WorldDominationTour::Campaign::Reset_INI(void)
{
	INIClass ini;
	Load_History(ini);
	ini.Put_Int(Name, "CampaignID", 0);
	for (int i = 0; i < 4; i++) {
		Wstring key = Wstring("LastDay") + WDT_Country_Name(i);
		ini.Put_Int(Name, key, 0);
	}
	CDFileClass file;
	file.Open("WDTHist.ini", 2);
	ini.Save(file);
}

// FUNCTION: GAMEMD 0x00765380
CONFLICT_ITER WorldDominationTour::Campaign::Get_Conflict_Iterator(int index)
{
	CONFLICT_ITER it(Conflicts);
	for (; it; it++) {
		if (it->Get_Territory_Index() == index) {
			break;
		}
	}
	return(it);
}

// FUNCTION: GAMEMD 0x00765410
void WorldDominationTour::Campaign::Set_Faction(int faction)
{
	if (PlayerFaction != faction) {
		PlayerFaction = faction;
		INIClass ini;
		Load_History(ini);
		ini.Put_Int(Name, "PlayerFaction", PlayerFaction);
		CDFileClass file;
		file.Open("WDTHist.ini", 2);
		ini.Save(file);
	}
}

// FUNCTION: GAMEMD 0x00765580
void __stdcall Load_History_Out(INIClass & ini)
{
	CDFileClass file;
	file.Open("WDTHist.ini", 1);
	if (!file.Is_Available()) {
		file.Create();
		file.Close();
		file.Open("WDTHist.ini", 1);
	}
	ini.Load(file);
	file.Close();
}

// SYNTHETIC: GAMEMD 0x00765600
// ??_GCampaign@WorldDominationTour@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00765680
// ?Clear@?$DynamicVectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x007656B0
// ?ID@?$DynamicVectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAEHPBV?$rc_ptr@VConflict@WorldDominationTour@@@@@Z
// TEMPLATE: GAMEMD 0x007656D0
// ?GetItem@?$VectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UBE?AV?$rc_ptr@VConflict@WorldDominationTour@@@@H@Z
template class DynamicVectorClass<rc_ptr<WorldDominationTour::Conflict> >;

// TEMPLATE: GAMEMD 0x00765700
// ??B?$VectorCursor@V?$rc_ptr@VConflict@WorldDominationTour@@@@V?$DynamicVectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@@@UBE_NXZ
// SYNTHETIC: GAMEMD 0x00765710
// ??_G?$rc_ptr@VCampaignProperties@WorldDominationTour@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00765740
// ??_G?$rc_ptr@VHistory@WorldDominationTour@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00765770
// ??_G?$VectorCursor@V?$rc_ptr@VConflict@WorldDominationTour@@@@V?$DynamicVectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@@@UAEPAXI@Z
template class VectorCursor<rc_ptr<WorldDominationTour::Conflict>, DynamicVectorClass<rc_ptr<WorldDominationTour::Conflict> > >;

// SYNTHETIC: GAMEMD 0x00765790
// ??_G?$rc_ptr@VCampaign@WorldDominationTour@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x007657C0
// ??_G?$rc_ptr@VState@WorldDominationTour@@@@UAEPAXI@Z
template class rc_ptr<WorldDominationTour::State>;

// TEMPLATE: GAMEMD 0x007657F0
// ??8?$VectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x00765840
// ?Resize@?$VectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAE_NHPBV?$rc_ptr@VConflict@WorldDominationTour@@@@@Z
// TEMPLATE: GAMEMD 0x00765960
// ?Clear@?$VectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00765990
// ?ID@?$VectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAEHABV?$rc_ptr@VConflict@WorldDominationTour@@@@@Z
// TEMPLATE: GAMEMD 0x007659D0
// ?ID@?$VectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAEHPBV?$rc_ptr@VConflict@WorldDominationTour@@@@@Z
// TEMPLATE: GAMEMD 0x007659F0
// ?Resize@?$DynamicVectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAE_NHPBV?$rc_ptr@VConflict@WorldDominationTour@@@@@Z
// TEMPLATE: GAMEMD 0x00765B10
// ?ID@?$DynamicVectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAEHABV?$rc_ptr@VConflict@WorldDominationTour@@@@@Z
// TEMPLATE: GAMEMD 0x00765B40
// ??E?$VectorCursor@V?$rc_ptr@VConflict@WorldDominationTour@@@@V?$DynamicVectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x00765B50
// ??F?$VectorCursor@V?$rc_ptr@VConflict@WorldDominationTour@@@@V?$DynamicVectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@@@UAEAAV0@XZ
// SYNTHETIC: GAMEMD 0x00765B60
// ??_G?$VectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00765BB0
// ??_E?$rc_ptr@VConflict@WorldDominationTour@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00765C20
// ??_G?$DynamicVectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x00765C70
// ??0?$rc_ptr@VConflict@WorldDominationTour@@@@QAE@XZ


// TEMPLATE: GAMEMD 0x00765C90
// ??0?$VectorClass@V?$rc_ptr@VConflict@WorldDominationTour@@@@@@QAE@HPBV?$rc_ptr@VConflict@WorldDominationTour@@@@@Z
template class VectorClass<rc_ptr<WorldDominationTour::Conflict> >;
