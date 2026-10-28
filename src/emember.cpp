// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "emember.h"
#include "airctype.h"
#include "crc.h"
#include "globals.h"
#include "infatype.h"
#include "techtype.h"
#include "unittype.h"
#include <cstdio>
#include <cmath>

// SYNTHETIC: GAMEMD 0x004C4D00 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004C4D30 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004C4D50 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004C4D70 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004C4D90 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004C4DB0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004C4DD0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004C4E10 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004C4E40 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004C4E70 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004C4EA0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004C4EB0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004C4ED0 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x004C4EF0
EnlistedMemberClass::EnlistedMemberClass(char const * entry)
{
	char name[64];
	int quantity;

	Quantity = 0;
	Class = NULL;

	sscanf(entry, "%d,%s", &quantity, name);

	Quantity = quantity;

	int type = InfantryTypeClass::From_Name(name);
	if (type != INFANTRY_NONE) {
		Class = InfantryTypes[type];
		return;
	}

	type = UnitTypeClass::From_Name(name);
	if (type != UNIT_NONE) {
		Class = UnitTypes[type];
		return;
	}

	type = AircraftTypeClass::From_Name(name);
	if (type != AIRCRAFT_NONE) {
		Class = AircraftTypes[type];
	}
}

// FUNCTION: GAMEMD 0x004C4F90
char const * EnlistedMemberClass::Build_INI_Entry(void) const
{
	static char _buf[32];

	sprintf(_buf, "%d,%s", Quantity, (char const *)Class->IniName);
	return(_buf);
}

// FUNCTION: GAMEMD 0x004C4FC0
void EnlistedMemberClass::Compute_CRC(CRCEngine & crc) const
{
	crc(Quantity);
	if (Class != NULL) {
		crc((RTTIType)Class->RTTI);
		crc(Class->Fetch_ID());
	}
}
