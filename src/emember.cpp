// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "emember.h"
#include "airctype.h"
#include "crc.h"
#include "globals.h"
#include "infatype.h"
#include "techtype.h"
#include "unittype.h"
#include <cstdio>

// FUNCTION: 0x004C4EF0
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

// FUNCTION: 0x004C4FC0
void EnlistedMemberClass::Compute_CRC(CRCEngine & crc) const
{
	crc(Quantity);
	if (Class != NULL) {
		crc((RTTIType)Class->RTTI);
		crc(Class->Fetch_ID());
	}
}
