// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"

#include "carry.h"

#include "building.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "unit.h"

// SYNTHETIC: GAMEMD 0x00473540 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00473570 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00473590 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004735B0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004735D0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004735F0 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x00473610 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x00473650 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x00473680 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x004736B0 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x004736E0 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x004736F0 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x00473710 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x00473730 SYMBOL
// _$E41

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
