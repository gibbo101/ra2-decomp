// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mission.h"
#include "_bench.h"
#include "_mission.h"
#include "bench.h"
#include "ccini.h"
#include "globals.h"
#include "incdec.h"
#include "mono.h"
#include "bench.hh"

// FUNCTION: 0x005B2DA0
MissionClass::MissionClass(void) :
	BASECLASS(),
	CurrentMission(MISSION_NONE),
	SuspendedMission(MISSION_NONE),
	MissionQueue(MISSION_NONE),
	IsMissionUnloadStandby(false),
	Status(0),
	CurrentMissionStartTime(0),
	MissionAccumulateTime(0),
	Timer(0)
{
}

// FUNCTION: 0x005B2E10
int MissionClass::Do_MISSION_SLEEP(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2E20
int MissionClass::Do_MISSION_HARMLESS(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2E30
int MissionClass::Do_MISSION_AMBUSH(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2E40
int MissionClass::Do_MISSION_ATTACK(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2E50
int MissionClass::Do_MISSION_CAPTURE(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2E60
int MissionClass::Do_MISSION_EATEN(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2E70
int MissionClass::Do_MISSION_GUARD(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2E80
int MissionClass::Do_MISSION_GUARD_AREA(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2E90
int MissionClass::Do_MISSION_HARVEST(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2EA0
int MissionClass::Do_MISSION_HUNT(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2EB0
int MissionClass::Do_MISSION_MOVE(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2EC0
int MissionClass::Do_MISSION_RETREAT(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2ED0
int MissionClass::Do_MISSION_RETURN(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2EE0
int MissionClass::Do_MISSION_STOP(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2EF0
int MissionClass::Do_MISSION_UNLOAD(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2F00
int MissionClass::Do_MISSION_ENTER(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2F10
int MissionClass::Do_MISSION_CONSTRUCTION(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2F20
int MissionClass::Do_MISSION_DECONSTRUCTION(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2F30
int MissionClass::Do_MISSION_REPAIR(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2F40
int MissionClass::Do_MISSION_MISSILE(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2F50
int MissionClass::Do_MISSION_OPEN(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2F60
int MissionClass::Do_MISSION_RESCUE(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2F70
int MissionClass::Do_MISSION_PATROL(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B36E0
bool MissionClass::Is_Recruitable_Mission(MissionType mission)
{
	if (mission == MISSION_NONE) {
		return(true);
	}
	return(MissionControl[mission].IsRecruitable);
}

// FUNCTION: 0x005B3700
MissionControlClass::MissionControlClass(void) :
	Mission(MISSION_NONE),
	IsNoThreat(false),
	IsZombie(false),
	IsRecruitable(true),
	IsParalyzed(false),
	IsRetaliate(true),
	IsScatter(true),
	Rate(.016),
	AARate(.016)
{
}

// FUNCTION: 0x005B3740
char const * MissionControlClass::Name(void) const
{
	if (Mission == MISSION_NONE) {
		return("<none>");
	}
	return(Missions[Mission]);
}

// FUNCTION: 0x005B3910
MissionType MissionClass::Mission_From_Name(char const * name)
{
	MissionType	order;

	if (name) {
		for (order = MISSION_FIRST; order < MISSION_COUNT; order++) {
			if (stricmp(Missions[order], name) == 0) {
				return(order);
			}
		}
	}
	return(MISSION_NONE);
}

// FUNCTION: 0x005B3950
char const * MissionClass::Mission_Name(MissionType mission)
{
	return(mission != MISSION_NONE ? Missions[mission] : "<none>");
}
