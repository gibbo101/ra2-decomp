// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "foot.h"
#include "_astar.h"
#include "_bench.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "anim.h"
#include "astar.h"
#include "bench.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "convert.h"
#include "ddist.h"
#include "goptions.h"
#include "house.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipiggy.h"
#include "loco.h"
#include "mono.h"
#include "notify.h"
#include "parasite.h"
#include "partsys.h"
#include "revent.h"
#include "rules.h"
#include "session.h"
#include "swizzle.h"
#include "tactical.h"
#include "team.h"
#include "tracker.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vein.h"
#include "voc.h"
#include "vox.h"
#include "waypoint.h"
#include "weapon.h"
#include "xsurface.h"
#include "bench.hh"
#include "color.hh"
#include "tube.hh"

// FUNCTION: 0x004D3160 $E1
// FUNCTION: 0x004D31A0 $E1
DynamicVectorClass<FootClass *> Feet;

// FUNCTION: 0x004D31E0
FootClass::FootClass(HouseClass * house) :
	BASECLASS(house),
	IsToScatter(false),
	IsScanLimited(false),
	IsInitiated(false),
	IsNewNavCom(false),
	IsPlanningToLook(false),
	IsDeploying(false),
	IsFiring(false),
	unknown_bool_68E(false),
	ShouldEnterAbsorber(false),
	ShouldEnterOccupiable(false),
	ShouldGarrisonStructure(false),
	IsRotating(false),
	IsUnloading(false),
	IsNavQueueLoop(false),
	IsScattering(false),
	IsIdle(false),
	IonBlastYDrawOffset(0),
	IsCrushing(false),
	IsOccupyingCell(true),
	IsToPathAroundBlockage(false),
	IsDroppedFromTeam(false),
	CurrentPath(PATH_NONE),
	WaypointOffsetCell(0,0),
	WaypointTargetCell(0,0),
	ThreatAvoidanceCoefficient(0),
	TotalFramesWalked(0),
	IsMoveSoundPlaying(false),
	MoveSoundDelay(0),
	LastPathingCell(0,0),
	LastAdjacencyCell(0,0),
	LastFlightMapCoords(0,0),
	CurrentJumpjetMapCoords(0,0),
	LastTubeCoord(0,0,0),
	Speed(0),
	SpeedBias(1),
	unknown_5A0(NULL),
	NavCom(NULL),
	SuspendedNavCom(NULL),
	MegaMission(MISSION_NONE),
	MegaDestination(NULL),
	MegaTarget(NULL),
	HasAttackMoveTarget(false),
	Team(0),
	Member(0),
	PatrolCell(0),
	PathDelay(0),
	TryTryAgain(PATH_RETRY),
	BaseAttackTimer(0),
	SightTimer(0),
	BlockagePathDelay(0),
	HeadToCoord(COORD_NONE),
	CurrentTube(-1),
	CurrentTubeDir(0),
	NextWaypoint(0),
	ParasiteEatingMe(NULL),
	LastBeParasitedStartFrame(0),
	ParasiteImUsing(NULL),
	ParalysisTimer(0),
	unknown_bool_6AC(false),
	IsAttackedByLocomotor(false),
	IsLetGoByLocomotor(false)
{
	Path[0] = FACING_NONE;
	MoveSoundAudioController.FUN_00405BE0();
	Feet.Add(this);
	AbstractFlags |= 4;
	PointerExpiredNotification::NotifyInvalidTeam.Add(this);
}

// FUNCTION: 0x004D3540
FootClass::FootClass(NoInitClass const & x) :
	BASECLASS(x),
	RouteQueue(x),
	NavQueue(x),
	PathDelay(x),
	BaseAttackTimer(x),
	SightTimer(x),
	BlockagePathDelay(x),
	ParalysisTimer(x)
{
}

// FUNCTION: 0x004D3590
FootClass::~FootClass(void)
{
	if (Team != NULL && GameActive) {
		Team->Remove(this);
	}

	Feet.Delete(this);
	PointerExpiredNotification::NotifyInvalidTeam.Delete(this);

	if (ParasiteImUsing != NULL) {
		delete ParasiteImUsing;
		ParasiteImUsing = NULL;
	}

	if (CurrentJumpjetMapCoords != CELL_NONE) {
		CellClass & cell = Map[CurrentJumpjetMapCoords];
		if (cell.Jumpjet == this) {
			cell.Jumpjet = NULL;
		}
	}

	MoveSoundAudioController.EndLooping();
	MoveSoundAudioController.FUN_00405C00();
}

// FUNCTION: 0x004D3710
void FootClass::Set_Speed(double speed)
{
	if (speed >= 1.0) {
		speed = 1.0;
	} else if (speed <= 0.0) {
		speed = 0.0;
	}

	Speed = speed;
}

// FUNCTION: 0x004D3780
bool FootClass::Mark(MarkType mark)
{
	if (mark == MARK_CHANGE) {
		return(true);
	}

	if (BASECLASS::Mark(mark)) {
		if (In_Which_Layer() == LAYER_GROUND) {
			Cell cell = Get_Cell();

			switch (mark) {
				case MARK_UP:
					Map.Pick_Up(cell, this);
					break;

				case MARK_DOWN:
				case MARK_DOWN_FORCED:
					Map.Place_Down(cell, this);
					break;
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004D3810
bool FootClass::Is_In_Same_Zone(Coord const & coord) const
{
	MZoneType zone = Techno_Type_Class()->MZone;
	if (zone == MZONE_NONE) {
		return(true);
	}

	Cell cell1 = coord.As_Cell();

	if (cell1 == CELL_NONE) {
		return(false);
	}

	Cell cell2 = Destination_Coord().As_Cell();

	return(Map.Is_Same_Cell_Zone(cell2, cell1, zone, ((ObjectClass *)this)->Is_Moving_Onto_Bridge(), Map[cell1].IsUnderBridge, Is_Allowed_To_Leave_Map()));
}

// FUNCTION: 0x004D4200
int FootClass::Do_MISSION_MOVE(void)
{
	if (NavCom == NULL && !Locomotion->Is_Moving() && MissionQueue == MISSION_NONE) {
		Enter_Idle_Mode();
		return(1);
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: 0x004D4B20
int FootClass::Do_MISSION_CAPTURE(void)
{
	InfantryClass * inf = (this == NULL) ? NULL : (RTTI == RTTI_INFANTRY) ? (InfantryClass *)this : NULL;

	if (TarCom != NULL && TarCom->RTTI == RTTI_BUILDING && NavCom == NULL && inf != NULL && (inf->Class->IsBomber || inf->Has_Ability(ABILITY_C4) || inf->Class->IsEngineer || inf->Class->Occupier || inf->Class->Assaulter || inf->Class->IsCapture)) {
		Assign_Destination(TarCom);
	}

	if (NavCom == NULL) {
		Enter_Idle_Mode();
		if (Map[Center_Coord()].Cell_Building()) {
			Scatter(COORD_NONE, true);
		}
	}

	if (TarCom == NULL && !House->Is_Human_Player()) {
		if (RTTI != RTTI_INFANTRY || !((InfantryClass *)this)->Class->Occupier) {
			Assign_Target(NULL);
			Assign_Destination(NULL);
			QueueMission(MISSION_HUNT, false);
		}
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: 0x004D4CB0
int FootClass::Do_MISSION_EATEN(void)
{
	InfantryClass * inf = (this == NULL) ? NULL : (RTTI == RTTI_INFANTRY) ? (InfantryClass *)this : NULL;

	if (TarCom != NULL && TarCom->RTTI == RTTI_BUILDING && NavCom == NULL && inf != NULL && (inf->Class->IsBomber || inf->Has_Ability(ABILITY_C4) || inf->Class->IsEngineer)) {
		Assign_Destination(TarCom);
	}

	if (NavCom == NULL) {
		Enter_Idle_Mode();
		if (Map[Center_Coord()].Cell_Building()) {
			Scatter(COORD_NONE, true);
		}
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: 0x004D4DC0
int FootClass::Do_MISSION_ATTACK(void)
{
	if (Techno_Type_Class()->HoverAttack && HeightAGL == 0) {
		Assign_Destination(&Map[Nearby_Location(this)]);
	}

	if (unknown_bool_68E) {
		Coord coord = Position;
		AbstractClass * target = Greatest_Threat(THREAT_RANGE, coord, false);
		if (target != NULL) {
			Assign_Target(target);
			unknown_bool_68E = false;
		}
	}

	if (TarCom != NULL) {
		Approach_Target(false);
	} else {
		Enter_Idle_Mode();
	}

	int delay = Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2);

	if (TarCom != NULL) {
		if ((this != NULL && RTTI == RTTI_INFANTRY && (RTTI == RTTI_INFANTRY ? (InfantryClass *)this : NULL)->Class->CloseRange) || (PrimaryWeapon != NULL && PrimaryWeapon->Range <= 0x200)) {
			int dist = ((Point2D)Center_Coord()).Distance_To((Point2D)TarCom->Center_Coord());
			if (dist <= 0x300) {
				if ((double)dist >= 281.6) {
					delay /= 2;
				}
			}
		}
	}
	return(delay);
}

// FUNCTION: 0x004D9FF0
Coord FootClass::Likely_Coord(void) const
{
	return(Target_Coord());
}

// FUNCTION: 0x004DA020
Cell FootClass::Adjust_Dest(Cell const & cell) const
{
	return(cell);
}
