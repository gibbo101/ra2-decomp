// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

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

Wstring & __fastcall WDT_Country_Name(int index);
extern int DAT_00B726D0, DAT_00B726D4, DAT_00B726D8;

// FUNCTION: GAMEMD 0x00764A00
void FUN_00764A00(void)
{
	DAT_00B726D8 = 0;
	DAT_00B726D0 = 0x80;
	DAT_00B726D4 = 0x80;
}

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

// FUNCTION: GAMEMD 0x00764F30
WorldDominationTour::Campaign::~Campaign(void)
{
}

// FUNCTION: GAMEMD 0x00764FA0
bool WorldDominationTour::Campaign::Is_Different_Cycle(void)
{
	return(CycleID != Properties->FUN_0076BFA0());
}

// FUNCTION: GAMEMD 0x00764FC0
bool WorldDominationTour::Campaign::Is_Different(int index)
{
	if (Is_Different_Cycle()) {
		return(true);
	}
	if (PreviousTicks[index] != Properties->FUN_0076BFE0()) {
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
	CycleID = Properties->FUN_0076BFA0();
	PreviousTicks[index] = Get_Current_Tick();
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
