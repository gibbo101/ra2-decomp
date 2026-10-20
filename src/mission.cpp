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

namespace Unsorted { extern int CurrentFrame; }

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

// FUNCTION: 0x005B2F80
int MissionClass::Do_MISSION_PARADROP_APPROACH(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2F90
int MissionClass::Do_MISSION_PARADROP_OVERFLY(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2FA0
int MissionClass::Do_MISSION_WAIT(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2FB0
int MissionClass::Do_MISSION_SPYPLANE_APPROACH(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2FC0
int MissionClass::Do_MISSION_SPYPLANE_OVERFLY(void)
{
	return(TICKS_PER_SECOND*30);
}

// FUNCTION: 0x005B2FD0
void MissionClass::Set_Mission(MissionType mission)
{
	if (CurrentMission == MISSION_WAIT && mission == MISSION_GUARD) {
		return;
	}
	CurrentMission = mission;
	MissionQueue = MISSION_NONE;
	IsMissionUnloadStandby = false;
	Status = 0;
	CurrentMissionStartTime = Unsorted::CurrentFrame;
	MissionAccumulateTime = 0;
	Timer = 0;
}

// FUNCTION: 0x005B3040
MissionType MissionClass::Get_Mission(void) const
{
	return(CurrentMission == MISSION_NONE ? MissionQueue : CurrentMission);
}

// FUNCTION: 0x005B3060
void MissionClass::AI(void)
{
	BASECLASS::AI();

	if (!IsActive) {
		return;
	}

#define INVOKE(e) case MISSION_ ## e: Timer = Do_MISSION_ ## e (); break;

	if (Timer == 0 && Strength > 0) {
		switch (CurrentMission) {
			default:
				Timer = Do_MISSION_SLEEP();
				break;

			INVOKE(SLEEP);
			INVOKE(HARMLESS);
			INVOKE(ENTER);
			INVOKE(CONSTRUCTION);
			INVOKE(DECONSTRUCTION);
			INVOKE(ATTACK);
			INVOKE(RETREAT);
			INVOKE(HARVEST);
			INVOKE(GUARD_AREA);
			INVOKE(RETURN);
			INVOKE(STOP);
			INVOKE(AMBUSH);
			INVOKE(UNLOAD);
			INVOKE(REPAIR);
			INVOKE(OPEN);
			INVOKE(MISSILE);
			INVOKE(GUARD);
			INVOKE(CAPTURE);
			INVOKE(EATEN);
			INVOKE(MOVE);
			INVOKE(HUNT);
			INVOKE(RESCUE);
			INVOKE(PATROL);
			INVOKE(PARADROP_APPROACH);
			INVOKE(PARADROP_OVERFLY);
			INVOKE(SPYPLANE_APPROACH);
			INVOKE(SPYPLANE_OVERFLY);
			INVOKE(WAIT);

			case MISSION_STICKY:
				Timer = Do_MISSION_GUARD();
				break;

			case MISSION_SABOTAGE:
				Timer = Do_MISSION_CAPTURE();
				break;
		}
	}
}

// FUNCTION: 0x005B3570
bool MissionClass::Commence(void)
{
	if (MissionQueue != MISSION_NONE) {
		CurrentMission = MissionQueue;
		MissionQueue = MISSION_NONE;
		Timer = 0;
		Status = 0;
		CurrentMissionStartTime = Unsorted::CurrentFrame;
		MissionAccumulateTime = 0;
		IsMissionUnloadStandby = false;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005B35E0
void MissionClass::QueueMission(MissionType mission, bool start)
{
	if (CurrentMission == MISSION_WAIT && mission == MISSION_GUARD) {
		return;
	}
	if (CurrentMission == MISSION_DECONSTRUCTION) {
		return;
	}
	if (mission != MISSION_NONE && (CurrentMission != mission || MissionQueue != mission && MissionQueue != MISSION_NONE)) {
		MissionQueue = mission;
		IsMissionUnloadStandby = false;
	}
	if (start && Ready_To_Next_Mission()) {
		Commence();
	}
}

// FUNCTION: 0x005B3650
void MissionClass::Override_Mission(MissionType mission, AbstractClass *, AbstractClass *)
{
	if (CurrentMission == MISSION_WAIT && mission == MISSION_GUARD) {
		return;
	}
	if (CurrentMission == MISSION_DECONSTRUCTION) {
		return;
	}
	if (MissionQueue != MISSION_NONE) {
		SuspendedMission = MissionQueue;
	} else {
		SuspendedMission = CurrentMission;
	}
	CurrentMission = mission;
	IsMissionUnloadStandby = false;
}

// FUNCTION: 0x005B36B0
bool MissionClass::Restore_Mission(void)
{
	if (SuspendedMission != MISSION_NONE) {
		CurrentMission = SuspendedMission;
		SuspendedMission = MISSION_NONE;
		IsMissionUnloadStandby = false;
		return(true);
	}
	return(false);
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
