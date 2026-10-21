// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "carry.h"

#include "building.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "unit.h"

// FUNCTION: GAMEMD 0x00473750
CarryoverClass::CarryoverClass(TechnoClass * techno) :
	RTTI(RTTI_NONE),
	CellID(0,0),
	Strength(0),
	House(HOUSE_NONE)
{
	if (techno) {
		RTTI = techno->RTTI;

		switch (RTTI) {
			case RTTI_UNIT:
				Type.Unit = (UnitType)((UnitClass *)techno)->TClass->Fetch_Heap_ID();
				break;

			case RTTI_BUILDING:
				Type.Building = (StructType)((BuildingClass *)techno)->TClass->Fetch_Heap_ID();
				break;

			case RTTI_INFANTRY:
				Type.Infantry = (InfantryType)((InfantryClass *)techno)->TClass->Fetch_Heap_ID();
				break;

			default:
				break;
		}

		House = techno->House->Class->House;
		Strength = techno->Strength;
		CellID = techno->PositionCell;
	}
}

// FUNCTION: GAMEMD 0x004737F0
bool CarryoverClass::Create(void) const
{
	TechnoClass * techno = 0;

	HouseClass * house = HouseClass::FindByCountryIndex(House);

	switch (RTTI) {
		case RTTI_UNIT:
			techno = new UnitClass(UnitTypes[Type.Unit], house);
			break;

		case RTTI_INFANTRY:
			techno = new InfantryClass(InfantryTypes[Type.Infantry], house);
			break;

		case RTTI_BUILDING:
			techno = new BuildingClass(BuildingTypes[Type.Building], house);
			break;
	}

	if (techno) {
		bool oldscen = ScenarioInit ? true : false;
		techno->Strength = Strength;
		if (RTTI == RTTI_INFANTRY) {
			ScenarioInit = 0;
		}
		techno->Unlimbo(CellID);
		if (RTTI == RTTI_INFANTRY) {
			ScenarioInit = oldscen;
		}
	}

	return(false);
}

// SYNTHETIC: GAMEMD 0x00473920
// ??_GCarryoverClass@@UAEPAXI@Z
