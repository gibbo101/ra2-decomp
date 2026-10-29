// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "mpsiege.h"
#include "session.h"
#include "scenario.h"
#include "house.h"
#include "houstype.h"
#include "building.h"
#include "rules.h"
#include "_rules.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include <string.h>
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x005CA420 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005CA450 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005CA470 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005CA490 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005CA4B0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005CA4D0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005CA4F0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005CA530 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005CA560 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005CA590 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005CA5C0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005CA5D0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005CA5F0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005CA610 SYMBOL
// _$E41

void __cdecl FUN_004068E0(char const * format, ...);
DynamicVectorClass<Cell> __fastcall FUN_00688380(char arg);
int __fastcall FUN_00688ED0(ObjectClass * object, Cell const & cell, int flag);

// FUNCTION: GAMEMD 0x005CA630
MPSiegeClass::MPSiegeClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MultiplayerGameMode(title, tooltip, inifile, mapfilter, aiallowed, modeindex)
{
}

// FUNCTION: GAMEMD 0x005CA670
MPSiegeClass::~MPSiegeClass(void)
{
}

// FUNCTION: GAMEMD 0x005CA680
bool MPSiegeClass::vt_entry_10(void)
{
	if (!MultiplayerGameMode::vt_entry_10()) {
		return(false);
	}
	if (Session.Players.Count() > 0) {
		Session.Players[0]->Slot.SpectatorFlag = 1;
		for (int i = 1; i < Session.Players.Count(); i++) {
			Session.Players[1]->Slot.SpectatorFlag = 2;
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005CA6D0
bool MPSiegeClass::vt_entry_14(WideWstring * message)
{
	if (!MultiplayerGameMode::vt_entry_14(message)) {
		return(false);
	}
	bool defender = false;
	int attackers = 0;
	for (int i = 0; i < Session.Players.Count(); i++) {
		switch (Session.Players[i]->Slot.SpectatorFlag) {
			case 0:
				break;
			case 1:
				if (defender) {
					*message = StringTable::LoadString("MP:OnlyOneBeseiged", NULL, "D:\\ra2mdpost\\MPSiege.cpp", 0x33);
					return(false);
				}
				defender = true;
				break;
			case 2:
				attackers++;
				break;
			default:
				*message = StringTable::LoadString("MP:IllegalTeam", NULL, "D:\\ra2mdpost\\MPSiege.cpp", 0x3c);
				return(false);
		}
	}
	if (!defender) {
		*message = StringTable::LoadString("MP:NoDefender", NULL, "D:\\ra2mdpost\\MPSiege.cpp", 0x41);
		return(false);
	}
	if (attackers < 1) {
		*message = StringTable::LoadString("MP:NoAttackers", NULL, "D:\\ra2mdpost\\MPSiege.cpp", 0x46);
		return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005CA7D0
bool MPSiegeClass::vt_entry_18(void)
{
	return(MultiplayerGameMode::vt_entry_18());
}

// FUNCTION: GAMEMD 0x005CA7E0
bool MPSiegeClass::IsAIAllowed(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x005CA7F0
bool MPSiegeClass::UnfixAlliances(void)
{
	return(MultiplayerGameMode::UnfixAlliances());
}

// FUNCTION: GAMEMD 0x005CA800
bool MPSiegeClass::StartingPositionsToHouseBaseCells2(bool arg)
{
	DynamicVectorClass<Cell> cells = FUN_00688380(arg);
	unsigned char used[16];
	memset(used, 0, sizeof(used));
	int defender = 0;
	while (defender < Session.Players.Count()) {
		if (Session.Players[defender]->Slot.SpectatorFlag == 1) {
			break;
		}
		defender++;
	}
	if (defender >= Session.Players.Count()) {
		return(false);
	}
	int index = Session.Players[defender]->Slot.HouseIndex;
	if (index < 0 || index >= Houses.Count()) {
		return(false);
	}
	HouseClass * base = Houses[index];
	if (base->Class->IsMultiplayPassive) {
		return(false);
	}
	FUN_004068E0("Assigning Starting Points for Defending house %d (%s)\n", index, base->Class->Name());
	Cell first = cells[0];
	used[0] = 1;
	base->Set_Base_Spawn_Cell(first);
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * house = Houses[i];
		if (!house->Class->IsMultiplayPassive && house != base) {
			FUN_004068E0("Assigning Starting Points for Attacking house %d (%s)\n", i, house->Class->Name());
			Cell cell;
			Cell chosen = *AssignStartingPositionsToHouse(&cell, i, &cells, used);
			house->Set_Base_Spawn_Cell(chosen);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005CA9B0
void MPSiegeClass::CreateMPTeams(DynamicVectorClass<MultiplayerTeam *> * teams)
{
	teams->Add(new MPObserverTeam);
	teams->Add(new MPSiegeDefenderTeam);
	teams->Add(new MPSiegeAttackerTeam);
}

// FUNCTION: GAMEMD 0x005CAAC0
bool MPSiegeClass::SpawnBaseUnits(HouseClass * house, unsigned unused)
{
	int i = 0;
	while (i < Session.Players.Count()) {
		if (Session.Players[i]->Slot.HouseIndex == house->HeapID) {
			break;
		}
		i++;
	}
	if (i >= Session.Players.Count()) {
		MultiplayerGameMode::SpawnBaseUnits(house, unused);
		return(false);
	}
	if (Session.Players[i]->Slot.SpectatorFlag != 1) {
		return(MultiplayerGameMode::SpawnBaseUnits(house, unused));
	}
	BuildingTypeClass const * type = (BuildingTypeClass const *)house->FirstBuildableFromArray(Rule->BuildConst);
	if (type != NULL) {
		BuildingClass * building = new BuildingClass(type, house);
		if (building != NULL) {
			if (building->Unlimbo(house->FUN_0050DF30()) || FUN_00688ED0(building, house->Get_Base_Center(), 1)) {
				return(true);
			}
			delete building;
		}
	}
	return(false);
}

// SYNTHETIC: GAMEMD 0x005CABD0
// ??_GMPSiegeClass@@UAEPAXI@Z
