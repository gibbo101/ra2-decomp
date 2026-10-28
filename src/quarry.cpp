// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "quarry.h"
#include "threat.hh"
#include "win.h"

char const * const QuarryName[QUARRY_COUNT] = {
	"N/A",
	"Anything",
	"Buildings - any",
	"Harvesters",
	"Infantry",
	"Vehicles - any",
	"Factories",
	"Base Defenses",
	"Base Threats",
	"Power Facilities",
	"Occupiable Buildings Only",
	"Tech Buildings Only"
};

// FUNCTION: GAMEMD 0x00645B60
QuarryType Quarry_From_Name(char const * name)
{
	for (int quarry = QUARRY_FIRST; quarry < QUARRY_COUNT; quarry++) {
		if (stricmp(name, QuarryName[quarry]) == 0) {
			return(QuarryType(quarry));
		}
	}
	return(QUARRY_NONE);
}

// FUNCTION: GAMEMD 0x00645BA0
char const * Name_From_Quarry(QuarryType quarry)
{
	return(QuarryName[quarry]);
}

// FUNCTION: GAMEMD 0x00645BB0
ThreatType __fastcall FUN_00645BB0(QuarryType quarry)
{
	switch (quarry) {
		case QUARRY_ANYTHING:
		case QUARRY_THREAT:
			goto none;
		case QUARRY_BUILDINGS:
			return(THREAT_BUILDINGS);
		case QUARRY_HARVESTERS:
			return(THREAT_TIBERIUM);
		case QUARRY_INFANTRY:
			return(THREAT_INFANTRY);
		case QUARRY_VEHICLES:
			return(THREAT_VEHICLES);
		case QUARRY_FACTORIES:
			return(THREAT_FACTORIES);
		case QUARRY_DEFENSE:
			return(THREAT_BASE_DEFENSE);
		case QUARRY_POWER:
			return(THREAT_POWER);
		case QUARRY_OCCUPIABLE_BUILDINGS:
			return(ThreatType(0x8000));
		case QUARRY_TECH_BUILDINGS:
			return(ThreatType(0x10000));
	}
none:
	return(THREAT_NORMAL);
}
