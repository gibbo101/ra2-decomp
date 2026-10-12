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

// FUNCTION: 0x004C4F90
char const * EnlistedMemberClass::Build_INI_Entry(void) const
{
	static char _buf[32];

	sprintf(_buf, "%d,%s", Quantity, (char const *)Class->IniName);
	return(_buf);
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

// FUNCTION: 0x004C5000 $E2
// FUNCTION: 0x004C5030 $E5
// FUNCTION: 0x004C5050 $E8
// FUNCTION: 0x004C5070 $E11
// FUNCTION: 0x004C5090 $E14
// FUNCTION: 0x004C50B0 $E17
// FUNCTION: 0x004C50D0 $E20
// FUNCTION: 0x004C5110 $E23
// FUNCTION: 0x004C5140 $E26
// FUNCTION: 0x004C5170 $E29
// FUNCTION: 0x004C51A0 $E32
// FUNCTION: 0x004C51B0 $E35
// FUNCTION: 0x004C51D0 $E38
// FUNCTION: 0x004C51F0 $E41

