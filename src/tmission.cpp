// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "tmission.h"
#include "mission.h"
#include "quarry.h"
#include "teamtype.h"
#include "need.hh"
#include "target.hh"
#include "tmission.hh"
#include "unload.hh"
#include <cstdio>

// FUNCTION: GAMEMD 0x00723CA0
void TeamMissionClass::Fill_In(char const * entry)
{
	if (entry != NULL) {
		TeamMissionType tmission;
		int value;

		sscanf(entry, "%d,%d", &tmission, &value);

		Mission = tmission;
		Data.Value = value;
	}
}

// FUNCTION: GAMEMD 0x00723CE0
int TeamMissionClass::Build_INI_Entry(char * ptr) const
{
	if (ptr != NULL) {
		sprintf(ptr, "%d,%d", Mission, Data.Value);
		return(strlen(ptr));
	}

	return(0);
}
