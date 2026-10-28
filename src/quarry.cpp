// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "quarry.h"
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
