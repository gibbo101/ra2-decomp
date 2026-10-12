// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "assert.h"

enum MissionType {
	MISSION_NONE=-1,
	MISSION_SLEEP,
	MISSION_ATTACK,
	MISSION_MOVE,
	MISSION_QMOVE,
	MISSION_RETREAT,
	MISSION_GUARD,
	MISSION_STICKY,
	MISSION_ENTER,
	MISSION_CAPTURE,
	MISSION_EATEN,
	MISSION_HARVEST,
	MISSION_GUARD_AREA,
	MISSION_RETURN,
	MISSION_STOP,
	MISSION_AMBUSH,
	MISSION_HUNT,
	MISSION_UNLOAD,
	MISSION_SABOTAGE,
	MISSION_CONSTRUCTION,
	MISSION_DECONSTRUCTION,
	MISSION_REPAIR,
	MISSION_RESCUE,
	MISSION_MISSILE,
	MISSION_HARMLESS,
	MISSION_OPEN,
	MISSION_PATROL,
	MISSION_PARADROP_APPROACH,
	MISSION_PARADROP_OVERFLY,
	MISSION_WAIT,
	MISSION_ATTACK_MOVE,
	MISSION_SPYPLANE_APPROACH,
	MISSION_SPYPLANE_OVERFLY,

	MISSION_COUNT,
	MISSION_FIRST=0
};

inline MissionType operator++(MissionType & mission)
{
	mission = MissionType(int(mission) + 1);
	assert(mission >= MISSION_FIRST && mission <= MISSION_COUNT);
	return(mission);
}

MissionType MissionType_From_Name(char const * name);
char const * MissionType_To_Name(MissionType mission);
