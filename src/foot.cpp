// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "foot.h"
#include "_astar.h"
#include "_bench.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rand.h"
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
#include "capmgr.h"
#include "ccrand.h"
#include "cell.h"
#include "convert.h"
#include "ddist.h"
#include "dsurface.h"
#include "event.h"
#include "goptions.h"
#include "house.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipiggy.h"
#include "mono.h"
#include "notify.h"
#include "parasite.h"
#include "partsys.h"
#include "queue.h"
#include "revent.h"
#include "rules.h"
#include "session.h"
#include "slavemgr.h"
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
#include "warhead.h"
#include "waypoint.h"
#include "weapon.h"
#include "xsurface.h"
#include "bench.hh"
#include "color.hh"
#include "tube.hh"

// SYNTHETIC: GAMEMD 0x004D2F50 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004D2F80 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004D2FA0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004D2FC0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004D2FE0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004D3000 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004D3020 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004D3060 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004D3090 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004D30C0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004D30F0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004D3100 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004D3120 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004D3140 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x004D3160 SYMBOL
// _$E194
// SYNTHETIC: GAMEMD 0x004D31A0 SYMBOL
// _$E192
DynamicVectorClass<FootClass *> Feet;

// FUNCTION: GAMEMD 0x004D31E0
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

// FUNCTION: GAMEMD 0x004D3540
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

// FUNCTION: GAMEMD 0x004D3590
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

// FUNCTION: GAMEMD 0x004D3710
void FootClass::Set_Speed(double speed)
{
	if (speed >= 1.0) {
		speed = 1.0;
	} else if (speed <= 0.0) {
		speed = 0.0;
	}

	Speed = speed;
}

// FUNCTION: GAMEMD 0x004D3780
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

// FUNCTION: GAMEMD 0x004D3810
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

// FUNCTION: GAMEMD 0x004D4200
int FootClass::Do_MISSION_MOVE(void)
{
	if (NavCom == NULL && !Locomotion->Is_Moving() && MissionQueue == MISSION_NONE) {
		Enter_Idle_Mode();
		return(1);
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

static inline ObjectClass * To_Object(AbstractClass * a)
{
	return(a == NULL ? NULL : ((a->AbstractFlags & 2) ? (ObjectClass *)a : NULL));
}
// FUNCTION: GAMEMD 0x004D4280
int FootClass::Do_MISSION_PATROL(void)
{
	int trange = Threat_Range(2);
	unknown_bool_430 = true;
	int dist = (trange / CELL_LEPTON_W) + 6;

	enum {
		PATROL_FIND_TARGET,
		PATROL_ENGAGE_TARGET,
		PATROL_RETURN,
		PATROL_RESET
	};

	switch (Status) {

		case PATROL_RESET:
			if (NavCom != NULL) {
				Status = PATROL_FIND_TARGET;
				return(1);
			}
			break;

		case PATROL_FIND_TARGET: {
			AbstractClass * target = Greatest_Threat(THREAT_AREA, Get_Coord(), false);
			ObjectClass * optr = To_Object(target);
			int primary = What_Weapon_Should_I_Use(optr);

			if (optr != NULL && (In_Range(optr, primary) || Search.Test_Cell_Walk(Cell(Destination_Coord()), Cell(optr->Destination_Coord()), this, Is_Moving_Onto_Bridge(), optr->Is_Moving_Onto_Bridge(), MZONE_NONE) < dist)) {
				PatrolCell = Get_Target_Cell_Ptr();
				if (PatrolCell->IsUnderBridge && !Is_Moving_Onto_Bridge()) {
					Cell cell = Map.Nearby_Location(PatrolCell->CellID, Techno_Type_Class()->Speed, Map.Get_Cell_Zone(PatrolCell->CellID, Techno_Type_Class()->MZone, false), Techno_Type_Class()->MZone, false, Point2D(1,1), false, true, false, false);
					if (cell != CELL_NONE) {
						PatrolCell = &Map[cell];
					} else {
						PatrolCell = NULL;
						return(60);
					}
				}

				if (ArchiveTarget == NULL) {
					if (RouteQueue.Count() > 0) {
						ArchiveTarget = RouteQueue[RouteQueue.Count()-1];
					} else if (NavCom != NULL) {
						ArchiveTarget = NavCom;
					} else {
						ArchiveTarget = PatrolCell;
					}
				}

				Assign_Destination(NULL);
				Assign_Target(target);
				Status = PATROL_ENGAGE_TARGET;

			} else if (NavCom == NULL || !Locomotion->Is_Moving()) {
				Enter_Idle_Mode();
				Status = PATROL_RESET;
				return(1);
			}
			break;
		}

		case PATROL_ENGAGE_TARGET: {
			ObjectClass * optr = To_Object(TarCom);
			int primary = What_Weapon_Should_I_Use(optr);

			if (Is_Renovator() && optr != NULL && optr->RTTI == RTTI_BUILDING && House->Is_Ally(((TechnoClass *)optr)->House) && optr->HealthRatio > Rule->ConditionRed) {
				Assign_Target(NULL);
			} else if (optr != NULL && (In_Range(optr, primary) || Search.Test_Cell_Walk(PatrolCell->CellID, Cell(optr->Destination_Coord()), this, PatrolCell->IsUnderBridge, optr->Is_Moving_Onto_Bridge(), MZONE_NONE) < dist)) {
				Approach_Target();
				if (TarCom != NULL) {
					break;
				}
				Assign_Destination(ArchiveTarget);
				ArchiveTarget = NULL;
				PatrolCell = NULL;
				Status = PATROL_FIND_TARGET;
				return(45);
			}

			if (TarCom == NULL) {
				AbstractClass * target = Greatest_Threat(THREAT_AREA, PatrolCell->Center_Coord(), false);
				ObjectClass * scanptr = To_Object(target);
				int scanweapon = What_Weapon_Should_I_Use(scanptr);
				if (scanptr != NULL && (In_Range(scanptr, scanweapon) || Search.Test_Cell_Walk(PatrolCell->CellID, Cell(scanptr->Destination_Coord()), this, PatrolCell->IsUnderBridge, scanptr->Is_Moving_Onto_Bridge(), MZONE_NONE) < dist)) {
					Assign_Target(target);
					Assign_Destination(NULL);
					return(1);
				}
			}

			Status = PATROL_RETURN;
			Assign_Target(NULL);
			if (ArchiveTarget != NULL) {
				CellClass * tcell = Get_Target_Cell_Ptr();
				bool onbridge = Is_Moving_Onto_Bridge();
				CellClass * acell = &Map[ArchiveTarget->Center_Coord()];
				int patrolwalk = Search.Test_Cell_Walk(tcell->CellID, PatrolCell->CellID, this, onbridge, PatrolCell->IsUnderBridge, MZONE_NONE);
				if (patrolwalk < Search.Test_Cell_Walk(tcell->CellID, acell->CellID, this, onbridge, acell->IsUnderBridge, MZONE_NONE)) {
					Assign_Destination(PatrolCell);
				} else {
					Assign_Destination(ArchiveTarget);
				}
				return(1);
			}
			Status = PATROL_FIND_TARGET;
			Enter_Idle_Mode();
			Status = PATROL_RESET;
			return(1);
		}

		case PATROL_RETURN: {
			AbstractClass * target = Greatest_Threat(THREAT_AREA, PatrolCell->Center_Coord(), false);
			ObjectClass * optr = To_Object(target);
			int primary = What_Weapon_Should_I_Use(optr);

			if (optr != NULL && (In_Range(optr, primary) || Search.Test_Cell_Walk(PatrolCell->CellID, Cell(optr->Destination_Coord()), this, PatrolCell->IsUnderBridge, optr->Is_Moving_Onto_Bridge(), MZONE_NONE) < dist)) {
				Assign_Target(target);
				Assign_Destination(NULL);
				Status = PATROL_ENGAGE_TARGET;
				break;
			}

			if (NavCom == NULL) {
				Assign_Target(NULL);
				if (ArchiveTarget != NULL) {
					Assign_Destination(ArchiveTarget);
					ArchiveTarget = NULL;
					Status = PATROL_FIND_TARGET;
					PatrolCell = NULL;
				} else {
					Status = PATROL_FIND_TARGET;
					Enter_Idle_Mode();
					Status = PATROL_RESET;
					return(1);
				}
			}
			break;
		}

		default:
			break;
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: GAMEMD 0x004D4B20
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

// FUNCTION: GAMEMD 0x004D4CB0
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

// FUNCTION: GAMEMD 0x004D4DC0
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

// FUNCTION: GAMEMD 0x004D4FC0
BuildingClass * FootClass::Adjacent_Overpowerable_Building(void)
{
	for (FacingType face = FACING_FIRST; face < FACING_COUNT; face++) {
		BuildingClass * building = Map[Adjacent_Cell(Get_Cell(), face)].Cell_Building();
		if (building != NULL && building->Class->Overpowerable && building->Owner_HouseClass() == Owner_HouseClass()) {
			return(building);
		}
	}
	return(NULL);
}

static inline BuildingClass * Adjacent_Overpowerable(FootClass * foot)
{
	for (int face = FACING_FIRST; face < FACING_COUNT; face++) {
		Cell cell = Adjacent_Cell(foot->Get_Cell(), (FacingType)face);
		BuildingClass * building = Map[cell].Cell_Building();
		if (building != NULL && building->Class->Overpowerable && building->Owner_HouseClass() == foot->Owner_HouseClass()) {
			return(building);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x004D5070
int FootClass::Do_MISSION_GUARD(void)
{
	if (ShouldEnterAbsorber) {
		EnterBioReactor();
	} else if (ShouldEnterOccupiable) {
		EnterBattleBunker();
	} else if (ShouldGarrisonStructure) {
		GarrisonStructure();
	} else {
		if (TarCom == NULL) {
			if (Get_Secondary_Weapon() != NULL && Get_Secondary_Weapon()->WarheadPtr->ElectricAssault) {
				BuildingClass * building = Adjacent_Overpowerable(this);
				if (building != NULL) {
					Assign_Target(building);
					unknown_bool_68E = true;
					QueueMission(MISSION_ATTACK, false);
				} else {
					Random_Animate();
				}
			} else {
				Random_Animate();
			}
		} else {
			if (Techno_Type_Class()->HoverAttack && HeightAGL == 0) {
				Assign_Destination(&Map[Nearby_Location(this)]);
			}
		}

		int dtime = Current_Mission_Control().Normal_Delay();

		if (this != NULL && RTTI == RTTI_INFANTRY && !House->Is_Human_Player() && (((InfantryClass *)this)->Class->IsBomber || Has_Ability(ABILITY_C4)) && Get_Mission() != MISSION_SABOTAGE && TarCom != NULL && TarCom->RTTI == RTTI_BUILDING) {
			QueueMission(MISSION_SABOTAGE, false);
		}

		int delay = 0;
		if (Arm != 0) {
			delay = (int)Arm;
		} else if (!(Techno_Type_Class()->DistributedFire && CurrentTargets.Count() > 0)) {
			delay = dtime + Random_Pick(0, 2);
		}
		return(delay);
	}
	return(Current_Mission_Control().Normal_Delay());
}

// FUNCTION: GAMEMD 0x004D5350
int FootClass::Do_MISSION_HUNT(void)
{
	if (!Techno_Type_Class()->StupidHunt && Target_Something_Nearby(PositionCoord, THREAT_NORMAL)) {
		InfantryClass * infantry = RTTI == RTTI_INFANTRY ? (InfantryClass *)this : NULL;
		if (infantry != NULL && infantry->Class->IsEngineer && !infantry->Class->IsBomber && !infantry->Has_Ability(ABILITY_C4)) {
			Assign_Destination(TarCom);
			QueueMission(MISSION_CAPTURE, false);
			if (Ready_To_Next_Mission()) {
				Commence();
			}
		} else {
			if (infantry != NULL && (infantry->Class->IsBomber || infantry->Has_Ability(ABILITY_C4)) && TarCom != NULL && TarCom->RTTI == RTTI_BUILDING) {
				Assign_Destination(TarCom);
				QueueMission(MISSION_SABOTAGE, false);
				if (Ready_To_Next_Mission()) {
					Commence();
				}
			} else if (infantry != NULL && infantry->Class->IsVehicleThief) {
				Assign_Destination(TarCom);
				QueueMission(MISSION_CAPTURE, false);
				if (Ready_To_Next_Mission()) {
					Commence();
				}
			} else if (infantry != NULL) {
				Assign_Destination(NULL);
				Approach_Target(false);
			} else {
				Approach_Target(false);
			}
		}
	} else {
		if (!House->Is_Human_Player() && Session.Type == GAME_NORMAL) {
			Cell cell = CELL_NONE;
			cell = PlayerPtr->Get_Base_Center();
			if (cell != CELL_NONE) {
				if (SlaveManager != NULL) {
					SlaveManager->AllGuard();
				}
				Assign_Destination(&Map[cell], true);
				QueueMission(MISSION_MOVE, true);
			}
		} else {
			Random_Animate();
		}
	}
	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: GAMEMD 0x004D55C0
bool FootClass::Stop_Driver(void)
{
	Locomotion->Stop_Moving();
	return(false);
}

// FUNCTION: GAMEMD 0x004D55F0
bool FootClass::Start_Driver(Coord & headto)
{
	Locomotion->Move_To(headto);
	return(Locomotion->Is_Moving() != 0);
}

// FUNCTION: GAMEMD 0x004D5660
void FootClass::Stun(void)
{
	Assign_Destination(NULL);
	Path[0] = FACING_NONE;
	Stop_Driver();
	BASECLASS::Stun();
}

inline Cell Adjacent_Cell_By_Value(Cell cell, FacingType dir)
{
	return(cell + AdjacentCell[(int)dir & 0x07]);
}

static inline bool Is_Foot_Object(AbstractClass const * object)
{
	if (object == NULL) {
		return(false);
	}
	unsigned char flag = (object->AbstractFlags >> 2) & 1;
	return(flag != 0);
}

static inline int Point_Distance(Point2D const & a, Point2D const & b)
{
	Point2D diff = a - b;
	return(diff.Length());
}

static inline void Nav_Pop(FootClass * foot)
{
	AbstractClass * target = foot->NavQueue[0];
	if (target != NULL) {
		foot->Assign_Destination(target);
		foot->NavQueue.Delete_Index(0);
		if (foot->IsNavQueueLoop) {
			foot->NavQueue.Add(target);
		}
	}
}

// FUNCTION: GAMEMD 0x004D6AA0
int FootClass::Do_MISSION_GUARD_AREA(void)
{
	if (BunkerLinkedItem != NULL) {
		QueueMission(MISSION_GUARD, true);
		return(1);
	}

	if ((unsigned char&)ShouldEnterAbsorber != 0) {
		EnterBioReactor();
	} else if (ShouldEnterOccupiable != false) {
		EnterBattleBunker();
	} else if (ShouldGarrisonStructure != false) {
		GarrisonStructure();
	} else {
		if (!House->Is_Human_Player()) {
			if (NavQueue.Count() > 0 && NavCom == NULL && NavQueue[0] == ArchiveTarget) {
				Nav_Pop(this);
			}
		}

		if (Is_Target_Cell(ArchiveTarget)) {
			BuildingClass * building = Map[ArchiveTarget->Center_Coord()].Cell_Building();
			if (building != NULL && House->Is_Ally(building->House) && !House->Is_Human_Player() && (RTTI != RTTI_UNIT || !static_cast<UnitClass *>(this)->Class->Harvester)) {
				Cell nearby = Map.Nearby_Location((Cell)ArchiveTarget->Center_Coord(), SPEED_TRACK, Map.Get_Cell_Zone((Cell)PositionCoord));
				if (nearby != CELL_NONE) {
					ArchiveTarget = &Map[nearby];
				}
			}
		}

		if (RTTI == RTTI_UNIT && static_cast<UnitClass *>(this)->Class->Harvester) {
			QueueMission(MISSION_HARVEST, false);
			Commence();
			return(1 + Random_Pick(1, 10));
		}
		if (SlaveManager != NULL) {
			SlaveManager->FUN_006B0CC0();
		}

		if (ArchiveTarget == NULL && MissionQueue == MISSION_NONE) {
			ArchiveTarget = &Map[(Coord const &)PositionCoord];
		}

		InfantryClass * infantry = (this == NULL) ? NULL : (RTTI == RTTI_INFANTRY) ? (InfantryClass *)this : NULL;
		if (!House->Is_Human_Player() && infantry != NULL && (infantry->Class->IsBomber || infantry->Has_Ability(ABILITY_C4)) && Get_Mission() != MISSION_SABOTAGE && TarCom != NULL && TarCom->RTTI == RTTI_BUILDING) {
			QueueMission(MISSION_SABOTAGE, false);
			return(1);
		}

		int maxrange = (int)(Threat_Range(1) * 1.1);

		if (Is_Foot_Object(ArchiveTarget)) {
			maxrange = Rule->GuardModeStray;
		}
		if (ArchiveTarget != NULL) {
			if (!IsFiring && NavCom == NULL && Distance(ArchiveTarget) > maxrange) {
				Assign_Target(NULL);
				Assign_Destination(ArchiveTarget);
			}

			if (TarCom == NULL) {
				if (CanPassiveAcquireTargets() && TargetingTimerFinished()) {
					Target_Something_Nearby(ArchiveTarget->Center_Coord(), THREAT_AREA);
				}
				if (TarCom != NULL) {
					return(1);
				}
				Random_Animate();
			} else {
				Approach_Target();
			}
		}

		if (TarCom == NULL) {
			if (Get_Secondary_Weapon() != NULL && Get_Secondary_Weapon()->WarheadPtr->ElectricAssault) {
				BuildingClass * building = Adjacent_Overpowerable(this);
				if (building != NULL) {
					Assign_Target(building);
					unknown_bool_68E = true;
					QueueMission(MISSION_ATTACK, false);
				}
			}
		}

		int dtime = Current_Mission_Control().Normal_Delay();
		if (RTTI == RTTI_AIRCRAFT) {
			dtime *= 2;
		}
		dtime += Random_Pick(1, 5);

		if (TarCom != NULL) {
			if ((this != NULL && RTTI == RTTI_INFANTRY && (RTTI == RTTI_INFANTRY ? (InfantryClass *)this : NULL)->Class->CloseRange) || (PrimaryWeapon != NULL && PrimaryWeapon->Range <= 0x200)) {
				int dist = Point_Distance((Point2D)Center_Coord(), (Point2D)TarCom->Center_Coord());
				if (dist <= 0x300) {
					if ((double)dist >= 281.6) {
						dtime /= 6;
					}
				}
			}
		}
		return(dtime);
	}
	return(Current_Mission_Control().Normal_Delay());
}


// FUNCTION: GAMEMD 0x004D7170
bool FootClass::Unlimbo(Coord const & coord, Dir256 dir)
{
	if (BASECLASS::Unlimbo(coord, dir)) {

		Locomotion->Unlimbo();

		bool off = false;
		if (House->IonSensitivesShouldBeOffline()) {
			if (Locomotion->Is_Ion_Sensitive()) {
				off = true;
			}
		}

		if (off) {
			Locomotion->Power_Off();
		} else {
			Locomotion->Power_On();
		}

		Revealed(House);

		Path[0] = FACING_NONE;

		Cell cell = Get_Cell();
		for (int face = FACING_FIRST; face < FACING_COUNT; face++) {
			Cell c = Adjacent_Cell_By_Value(cell, FacingType(face));
			CellClass * cptr = &Map[c];
			cptr->AdjacentObjectCount++;
		}

		if (!In_Air()) {
			LastAdjacencyCell = cell;
		} else if (Techno_Type_Class()->ConsideredAircraft) {
			AircraftTracker.Add(this);
		}

		double avoidance = Techno_Type_Class()->ThreatAvoidanceCoefficient;
		ThreatAvoidanceCoefficient = avoidance;

		if (Techno_Type_Class()->SensorsSight != 0) {
			AddSensorsAt(CELL_NONE);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004D7330
ResultType FootClass::Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source, bool forced, bool no_crew, HouseClass * house)
{
	if (warhead != NULL && warhead->Sonic && ParasiteEatingMe != NULL) {
		ParasiteEatingMe->ParasiteImUsing->ExitUnit();
		if (source != NULL) {
			source->Assign_Target(NULL);
		}
	}

	if (ParasiteEatingMe != NULL && source != ParasiteEatingMe) {
		if (damage > ParasiteEatingMe->Techno_Type_Class()->SuppressionThreshold) {
			ParasiteEatingMe->ParasiteImUsing->SuppressionTimer = damage * 2 - ParasiteEatingMe->Techno_Type_Class()->SuppressionThreshold;
		}
	}

	if (ParasiteEatingMe != NULL && damage < 0) {
		ParasiteEatingMe->ParasiteImUsing->SuppressionTimer = 50;
		ParasiteEatingMe->ParasiteImUsing->ExitUnit();
	}

	ResultType result = BASECLASS::Take_Damage(damage, distance, warhead, source, forced, no_crew, house);

	if (result == RESULT_ALREADY_DESTROYED) {
		return(result);
	}

	if (result != RESULT_NONE && Team != NULL) {
		Team->Took_Damage(this, result, source);
		return(result);
	}

	if (result != RESULT_DESTROYED && result != RESULT_NONE) {
		if (Team != NULL && Team->Class->IsWhiner && !House->Is_Human_Player()) {
			if (source == NULL) {
				return(result);
			}
			Base_Is_Attacked(source);
		}

		if (source != NULL && Current_Mission_Control().IsNoThreat && !Current_Mission_Control().IsZombie) {
			Enter_Idle_Mode();
		}
	}
	return(result);
}

extern bool MoveFeedback;

// FUNCTION: GAMEMD 0x004D7D50
bool FootClass::Active_Click_With(ActionType action, Cell const & cell, Cell const & cell2, bool is_waypoint)
{
	if (TechnoClass::Berzerk) {
		return(false);
	}
	if (DirectRockerLinkedUnit != NULL && !Techno_Type_Class()->Pushy) {
		return(false);
	}

	action = What_Action(cell, false, is_waypoint);

	if (!Map.In_Local_Radar(cell, true) && action != ACTION_NOMOVE) {
		return(false);
	}

	switch ((int)action) {
		case 51:
			if (Can_Player_Move()) {
				Cell moveto = Move_Order(cell, false);
				Player_Assign_Mission(MISSION_PATROL, NULL, &Map[moveto], NULL);
				break;
			}
			return(false);

		case ACTION_GUARD_AREA:
			if ((Can_Player_Fire() || Is_Renovator()) && Can_Player_Move()) {
				Player_Assign_Mission(MISSION_GUARD_AREA, &Map[cell], NULL, NULL);
				break;
			}
			return(false);

		case ACTION_HARVEST:
			Player_Assign_Mission(MISSION_HARVEST, NULL, &Map[cell], NULL);
			break;

		case ACTION_MOVE:
		case 62:
			if (MoveFeedback) {
				Coord coord = cell;
				coord.Z = Map.Get_Height_GL(coord);
				if (Map[coord].IsUnderBridge) {
					coord.Z += BRIDGE_LEPTON_HEIGHT;
				}
				int scenid = Scen->UniqueID;
				if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
					Scen->UniqueID = -3;
					((AnimTypeClass *)Rule->MoveFlash)->YSortAdjust = -5000;
				}
				AnimClass * moveflash = new AnimClass(Rule->MoveFlash, coord, 0, 1, ShapeFlags_Type(0x600), 0, false);
				if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
					Scen->UniqueID = scenid;
					Anims.Delete(moveflash);
					MoveFlashes.Add(moveflash);
				}
			}

		case ACTION_NOMOVE: {
			Cell moveto = Move_Order(cell, false);
			if (moveto != CELL_NONE) {
				Player_Assign_Mission(MISSION_MOVE, NULL, &Map[moveto], &Map[cell2]);
				break;
			}
		}
		return(false);

		case ACTION_ATTACK:
		case 63: {
			VeinholeMonsterClass * veinhole = VeinholeMonsterClass::Get_Monster_At(cell);
			if (veinhole != NULL && !veinhole->IsDead && veinhole->Strength > 0) {
				Player_Assign_Mission(MISSION_ATTACK, veinhole, NULL, NULL);
				break;
			} else {
				Player_Assign_Mission(MISSION_ATTACK, &Map[cell], NULL, NULL);
				break;
			}
		}
		return(false);

		case ACTION_CAPTURE:
			if (Can_Player_Move()) {
				Player_Assign_Mission(MISSION_CAPTURE, NULL, &Map[cell], NULL);
				break;
			}
			return(false);

		case ACTION_ENTER:
			if (Can_Player_Move()) {
				Player_Assign_Mission(MISSION_ENTER, NULL, &Map[cell], NULL);
				break;
			}
			return(false);

		case ACTION_SABOTAGE:
			Player_Assign_Mission(MISSION_SABOTAGE, NULL, &Map[cell], NULL);
			break;

		case ACTION_ENTER_TUNNEL: {
			TubeClass * tunnel = Map[cell].Get_Tunnel();
			TubeClass * exittunnel = Map[(Cell)tunnel->Exit].Get_Tunnel();
			Cell exit = Adjacent_Cell((Cell)tunnel->Exit, Facing_Sub(exittunnel->EnterDir, FACING_180));
			Player_Assign_Mission(MISSION_MOVE, NULL, &Map[exit], NULL);
			break;
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004D82B0
bool FootClass::Enter_Idle_Mode(bool initial, bool resume_waypoint)
{
	if (!IsIdle) {
		IsIdle = true;
		BASECLASS::Enter_Idle_Mode(initial, resume_waypoint);

		if (IsToScatter) {
			IsToScatter = false;
			Scatter(COORD_NONE, true);
		}

		bool was_piggybacking = false;
		IPiggybackPtr piggy(Locomotion);
		if (piggy != NULL && piggy->Is_Ok_To_End()) {
			piggy->End_Piggyback(&Locomotion);
			was_piggybacking = true;
		}

		if (RouteQueue.Count() > 0) {
			Assign_Destination(RouteQueue[0], false);
			RouteQueue.Delete_Index(0);
			return(true);
		}

		if (CurrentMission != MISSION_PATROL || Status == 0) {
			if (CurrentPath != PATH_NONE && resume_waypoint) {
				WaypointClass * wp = PlayerPtr->Paths[CurrentPath]->Get_Waypoint(NextWaypoint);
				WaypointClass * next_wp = PlayerPtr->Paths[CurrentPath]->Get_Next_Waypoint(wp);
				Execute_Waypoint_Path(next_wp);
			}
		}

		if (was_piggybacking) {
			return(true);
		}

		if (RTTI == RTTI_INFANTRY && ArchiveTarget != NULL && SlaveOwner == NULL) {
			if (ArchiveTarget->Center_Coord().As_Cell() != Center_Coord().As_Cell()) {
				AbstractClass * target = ArchiveTarget;
				if (CurrentMission != MISSION_GUARD_AREA) {
					QueueMission(MISSION_MOVE, false);
					ArchiveTarget = NULL;
				}
				Assign_Destination(target, true);
			}
		}

		Set_Speed(0);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004D8560
Coord FootClass::GetAttackCoordinates(void) const
{
	if (Get_Mission() != MISSION_ATTACK) {
		if (unknown_5A0 != NULL) {
			return(unknown_5A0->As_Coord());
		}
		if (NavCom != NULL) {
			return(NavCom->As_Coord());
		}
	}
	return(BASECLASS::GetAttackCoordinates());
}

// FUNCTION: GAMEMD 0x004D8F40
void FootClass::Override_Mission(MissionType mission, AbstractClass * tarcom, AbstractClass * navcom)
{
	SuspendedNavCom = NavCom;
	BASECLASS::Override_Mission(mission, tarcom, navcom);

	Assign_Destination(navcom);
}

// FUNCTION: GAMEMD 0x004D8F80
bool FootClass::Restore_Mission(void)
{
	if (BASECLASS::Restore_Mission()) {
		Assign_Destination(SuspendedNavCom);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004D8FB0
RadioMessageType FootClass::Receive_Message(RadioClass * from, RadioMessageType message, int & param)
{
	BuildingClass const * building = NULL;
	ObjectClass * object = NULL;

	switch (message) {

		case RADIO_ON_DEPOT:
			building = Map[Center_Coord()].Cell_Building();
			if (building == (RadioClass const *)from) {
				return(RADIO_ROGER);
			}
			return(RADIO_NEGATIVE);

		case RADIO_REPAIR:
			if (NavCom != NULL) return(RADIO_NEGATIVE);
			break;

		case RADIO_RUN_AWAY:
			if (HasAnyLink()) {
				if (NavCom == FUN_0065AD30(0)) {
					Assign_Destination(NULL);
				}
			}
			if (Get_Mission() == MISSION_SLEEP) {
				QueueMission(MISSION_GUARD, false);
				if (Ready_To_Next_Mission()) {
					Commence();
				}
			}
			if (Get_Mission() == MISSION_ENTER) {
				QueueMission(MISSION_GUARD, false);
			}
			if (!IsRotating && NavCom == NULL) {
				Scatter(COORD_NONE, true, true);
			}
			break;

		case RADIO_NEED_TO_MOVE:
			param = (int)NavCom;
			if (NavCom == NULL || !Locomotion->Is_Moving()) {
				return(RADIO_ROGER);
			}
			return(RADIO_NEGATIVE);

		case RADIO_MOVE_HERE:
			object = (ObjectClass *)param;
			{
				if (object != NULL && PositionCell == Cell(object->Center_Coord())) {
					return(RADIO_YEA_NOW_WHAT);
				} else {
					if (Mission == MISSION_GUARD && MissionQueue == MISSION_NONE) {
						QueueMission(MISSION_MOVE, false);
					}
					if (MissionQueue == MISSION_ENTER && Ready_To_Next_Mission()) {
						Commence();
					}
					Assign_Destination((ObjectClass *)param);
					Shorten_Mission_Timer();
				}
			}
			return(RADIO_ROGER);

		case RADIO_TRYING_TO_LOAD:
			if (Mission == MISSION_ENTER || MissionQueue == MISSION_ENTER) {
				BASECLASS::Receive_Message(from, message, param);
				return(RADIO_ROGER);
			}
			break;
	}
	return(BASECLASS::Receive_Message(from, message, param));
}

// FUNCTION: GAMEMD 0x004D9290
int FootClass::Do_MISSION_ENTER(void)
{
	TechnoClass * contact = FUN_0065AD30(0);
	if (contact == NULL) {
		contact = Dynamic_Cast<TechnoClass *>(ArchiveTarget);
	}

	if (contact != NULL) {
		if (Transmit_Message(RADIO_DOCKING, contact) != RADIO_ROGER && !IsTethered) {
			Transmit_Message(RADIO_OVER_OUT);
			Enter_Idle_Mode();
		} else {
			if (NavCom == NULL && RouteQueue.Count() > 0) {
				IPiggybackPtr piggy(Locomotion);
				if (piggy != NULL && piggy->Is_Ok_To_End()) {
					piggy->End_Piggyback(&Locomotion);
				}
				if (RouteQueue.Count() > 0) {
					Assign_Destination(RouteQueue[0], false);
					RouteQueue.Delete_Index(0);
				}
			} else if (Techno_Type_Class()->Teleporter) {
				AbstractClass * target = NavCom;
				unknown_5A0 = NULL;
				NavCom = NULL;
				Assign_Destination(target, true);
			}
		}
	} else {
		if (!Move_To_Object_Nearby()) {
			if (NavCom == NULL || NavCom->RTTI != RTTI_UNIT && NavCom->RTTI != RTTI_AIRCRAFT) {
				Enter_Idle_Mode();
			}
		}
		Commence();
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: GAMEMD 0x004D94A0
void FootClass::vt_entry_47C(unsigned value)
{
	unknown_5A0 = (AbstractClass *)value;
}

namespace LocomotionClass { namespace CLSIDs { extern const CLSID Hover; } }

// FUNCTION: GAMEMD 0x004D94B0
void FootClass::Assign_Destination(AbstractClass * target, bool)
{
	unknown_5A0 = NULL;

	if (IsAttackedByLocomotor && target != NULL) {
		return;
	}
	if (InOpenToppedTransport && target != NULL) {
		return;
	}
	if (BunkerLinkedItem != NULL && target != NULL) {
		return;
	}
	if (LocomotorTarget != NULL && target != NULL) {
		ReleaseLocomotor(true);
	}

	NavCom = target;

	if (target == NULL && IsAttackedByLocomotor && LocomotorSource != NULL) {
		LocomotorSource->LocomotorTarget = NULL;
		LocomotorSource = NULL;
		IsLetGoByLocomotor = true;
	}

	if (NavCom != NULL) {
		if (ParticleSystems[ATTACHED_PARTICLE_FIRE] != NULL) {
			ParticleSystems[ATTACHED_PARTICLE_FIRE]->Delete_Me();
			ParticleSystems[ATTACHED_PARTICLE_FIRE] = NULL;
		}

		CLSID locoid;
		IPersistPtr persist(Locomotion);
		persist->GetClassID(&locoid);

		if (locoid == LocomotionClass::CLSIDs::Hover && PathDelay == 0) {
			PathDelay = 1;
		}

		if (unknown_bool_6AC) {
			unknown_bool_6AC = false;
		} else {
			Locomotion->Move_To(NavCom->Destination_Coord(this));
		}
	} else if (RTTI != RTTI_AIRCRAFT || (CurrentMission != MISSION_ATTACK && MissionQueue != MISSION_ATTACK) || TarCom == NULL) {
		Locomotion->Stop_Moving();
		NavCom = target;
	}

	IsToPathAroundBlockage = false;
	BlockagePathDelay = Rule->BlockagePathDelay;
	PathDelay = 0;
}

// FUNCTION: GAMEMD 0x004D9720
void FootClass::Detach_All(bool all)
{
	if (all) {
		if (Team && !ScenarioInit) {
			Team->Remove(this);
		}
	}

	if (!all) {
		TechnoClass * contact = FUN_0065AD30(0);
		if (contact != NULL && !House->Is_Ally(contact)) {
			Transmit_Message(RADIO_OVER_OUT);
		}
	} else {
		Transmit_Message(RADIO_OVER_OUT);
	}

	BASECLASS::Detach_All(all);
}

// FUNCTION: GAMEMD 0x004D97A0
int FootClass::Rescue_Mission(AbstractClass * tarcom)
{
	if (tarcom == NULL) return(0);

	if (TarCom == tarcom) {
		return(-Risk());
	}

	if (TarCom != NULL && (TarCom->AbstractFlags & 1) && ((TechnoClass *)TarCom)->Is_Weapon_Equipped()) {
		return(0);
	}

	bool basedefense = Team != NULL && Team->Class->IsBaseDefense;
	if ((Team != NULL && !basedefense) || Get_Mission() == MISSION_HARVEST || !Risk()) {
		return(0);
	}

	int dist = Distance(tarcom) - Weapon_Range(0);
	int threat = Risk() * 1024;
	int speed = -1;
	if (dist > 0) {
		speed = MAX((unsigned)Get_Max_Speed(), (unsigned)1);

		int ratio = (speed > 0) ? MAX(dist / speed, 1) : 1;

		threat = MAX(threat/ratio, 1);
	}
	return(threat);
}

// FUNCTION: GAMEMD 0x004D98C0
void FootClass::Death_Announcement(TechnoClass const * ) const
{
	if (House->Is_Player_Control() && !Techno_Type_Class()->Spawned) {
		if (RadarEventClass::Create(RADAREVENT_UNIT_LOST, PositionCell)) {
			VoxClass::Play("EVA_UnitLost", -1, -1);
		}
	}
}

// FUNCTION: GAMEMD 0x004D9920
AbstractClass * FootClass::Greatest_Threat(ThreatType method, Coord const & coord, bool onlyenemy) const
{
	if (IsScanLimited) {
		method = ThreatType(method & ~THREAT_AREA);
		method = ThreatType(method | THREAT_RANGE);
	}

	AbstractClass * target = BASECLASS::Greatest_Threat(method, coord, onlyenemy);

	if (IsScanLimited && target == NULL) {
		const_cast<FootClass *>(this)->IsScanLimited = false;
	}

	return(target);
}

extern bool ScenarioStarted;

// FUNCTION: GAMEMD 0x004D9960
void FootClass::Detach(AbstractClass const * target, bool all)
{
	int index;

	BASECLASS::Detach(target, all);

	if (Team == target) {
		Team = NULL;
	}

	if (ParasiteEatingMe == target) {
		if (ParasiteEatingMe->Strength == 0 || !ScenarioStarted) {
			ParasiteEatingMe = NULL;
		}
	}

	if (ParasiteEatingMe != NULL && ParasiteEatingMe->Strength > 0) {
		ParasiteEatingMe->ParasiteImUsing->Detach(target, true);
	}

	if (target == this) {
		ParasiteEatingMe = NULL;
	}

	if (Member == target && all) {
		if (target != NULL) {
			Member = Member->Member;
		}
	}

	if (ArchiveTarget == target) {
		ArchiveTarget = NULL;
	}

	if (SuspendedNavCom == target) {
		SuspendedNavCom = NULL;
	}

	if (NavCom == target) {
		bool clear = true;
		CellClass * cptr;
		if (!all && target != NULL && target->Is_Techno() && (cptr = &Map[target->Center_Coord()], cptr->Is_Sensed(House->HeapID))) {
			clear = false;
		}
		if (Get_Mission() == MISSION_CAPTURE && RTTI == RTTI_INFANTRY && ((InfantryClass *)this)->Class->Occupier && NavCom != NULL && (NavCom->AbstractFlags & 2) && ((ObjectClass *)NavCom)->IsActive && ((ObjectClass *)NavCom)->Strength > 0 && ((ObjectClass *)NavCom)->Get_Mission() != MISSION_DECONSTRUCTION) {
		} else if (clear) {
			unknown_5A0 = NULL;
			NavCom = NULL;
		}
	}

	if (MegaTarget == target) {
		if (MegaTarget != NULL) {
			MegaDestination = &Map[MegaTarget->Center_Coord().As_Cell()];
		} else {
			MegaDestination = NULL;
		}
		MegaTarget = NULL;
	}

	if (MegaDestination == target) {
		MegaDestination = NULL;
	}

	for (index = 0; index < NavQueue.Count(); index++) {
		if (NavQueue[index] == target) {
			NavQueue.Delete_Index(index);
			index--;
		}
	}

	for (index = 0; index < RouteQueue.Count(); index++) {
		if (RouteQueue[index] == target) {
			RouteQueue.Delete_Index(index);
			index--;
		}
	}
}

// FUNCTION: GAMEMD 0x004D9C00
int FootClass::Offload_Tiberium_Bail(void)
{
	return(0);
}

// FUNCTION: GAMEMD 0x004D9C10
MoveType FootClass::Can_Enter_Cell(CellClass const * cell, FacingType, int cell_height, CellClass const *, bool use_locomotor_enter_check) const
{
	if (Locomotion != NULL && use_locomotor_enter_check) {
		CellStruct cellnum = (CellStruct &)cell->CellID;
		return(Locomotion->Can_Enter_Cell(cellnum));
	}
	return(MOVE_OK);
}

// FUNCTION: GAMEMD 0x004D9C60
MoveType FootClass::Can_Reach(CellClass const * current_cell, FacingType facing, int & cell_height, bool & onto_bridge, CellClass const * adjacent_cell) const
{
	if (adjacent_cell == NULL) {
		adjacent_cell = &Map[Adjacent_Cell(current_cell->CellID, Facing_Sub(facing, FACING_180))];
	}

	if (facing != FACING_NONE) {
		if (adjacent_cell == NULL || current_cell == NULL) {
			return(MOVE_OK);
		}

		if (cell_height == -1 && adjacent_cell->IsUnderBridge) {
			cell_height = adjacent_cell->Height + BRIDGE_CELL_HEIGHT;
			if (!current_cell->IsBridgeTraversable) {
				return(MOVE_NO);
			}
		}

		int height_difference = adjacent_cell->IsUnderBridge ? adjacent_cell->Height - current_cell->Height : cell_height - current_cell->Height;
		int current_height = current_cell->Height;

		switch (abs(height_difference)) {
			case 0:
				if ((!current_cell->IsUnderBridge || !current_cell->IsBridgeTraversable || !adjacent_cell->IsUnderBridge) && cell_height != -1 && cell_height != current_height) {
					return(MOVE_NO);
				}
				break;

			case 1:
				if (height_difference > 0) {
					if (current_cell->Ramp == 0) {
						return(MOVE_NO);
					}
				} else if (adjacent_cell->Ramp == 0) {
					return(MOVE_NO);
				}
				break;

			case BRIDGE_CELL_HEIGHT:
				if (adjacent_cell->Height == current_height - BRIDGE_CELL_HEIGHT) {
					if (cell_height != current_height) return(MOVE_NO);
					if (!adjacent_cell->IsUnderBridge) return(MOVE_NO);
				}
				if (current_height != adjacent_cell->Height - BRIDGE_CELL_HEIGHT) return(MOVE_OK);
				if (!current_cell->IsUnderBridge) return(MOVE_NO);
				if (!current_cell->IsBridgeTraversable) return(MOVE_NO);
				onto_bridge = true;
				break;

			default:
				return(MOVE_NO);
		}
	} else {
		if (cell_height == -1 && current_cell->IsUnderBridge) {
			cell_height = current_cell->Height + BRIDGE_CELL_HEIGHT;
		}
	}
	return(MOVE_OK);
}

static inline int Coord_Distance(Coord const & a, Coord const & b)
{
	Coord diff = a - b;
	return(diff.Length());
}

// FUNCTION: GAMEMD 0x004D9E70
bool FootClass::Can_Demolish(void) const
{
	switch ((RTTIType)RTTI) {
		case RTTI_UNIT:
		case RTTI_AIRCRAFT:
			if (HasAnyLink() &&
				FUN_0065AD40(0)->RTTI == RTTI_BUILDING &&
				((BuildingClass *)FUN_0065AD40(0))->Class->IsCanUnitRepair &&
				Coord_Distance(Center_Coord(), FUN_0065AD40(0)->Center_Coord()) < CELL_LEPTON / 2) {

				return(true);
			}
			break;

		default:
			break;
	}
	return(BASECLASS::Can_Demolish());
}

// FUNCTION: GAMEMD 0x004D9F70
void FootClass::Sell_Back(int control)
{
	if (control != 0) {
		if (House->Is_Player_Control()) {
			VoxClass::Play("EVA_UnitSold", -1, -1);
			VocClass::PlayGlobal(Rule->SellSound, 1.0f, 0x2000);
		}
		House->Refund_Money(Refund_Amount());
		Stun();
		Limbo();
		Delete_Me();
	}
}

// FUNCTION: GAMEMD 0x004D9FF0
Coord FootClass::Likely_Coord(void) const
{
	return(Target_Coord());
}

// FUNCTION: GAMEMD 0x004DA020
Cell FootClass::Adjust_Dest(Cell const & cell) const
{
	return(cell);
}

// FUNCTION: GAMEMD 0x004DA030
void FootClass::Handle_Navigation_List(void)
{
	if (NavCom == NULL && NavQueue.Count()) {
		AbstractClass * target = NavQueue[0];

		if (target != NULL) {
			Assign_Destination(target);
			NavQueue.Delete_Index(0);

			if (IsNavQueueLoop) {
				NavQueue.Add(target);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004DA0E0
void FootClass::Queue_Navigation_List(AbstractClass * target)
{
	if (target != NULL) {
		int count = NavQueue.Count();
		if (target == this && count > 0) {
			IsNavQueueLoop = true;
		} else {
			if (count == 0) {
				IsNavQueueLoop = false;
			}
			NavQueue.Add(target);
		}

		if (NavCom == NULL && Mission == MISSION_GUARD) {
			TechnoClass *tptr = FUN_0065AD30(0);
			if (tptr == NULL || tptr->RTTI != RTTI_BUILDING || !((BuildingClass*)tptr)->Class->IsWeaponsFactory) {
				Enter_Idle_Mode();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004DA1C0
void FootClass::Clear_Navigation_List(void)
{
	NavQueue.Clear();
}

// FUNCTION: GAMEMD 0x004DA1D0
bool FootClass::Is_Allowed_To_Leave_Map(void) const
{
	if (!IsInPlayfield) return(false);

	if (!TClass->IsTrain && !IsALoaner && Mission != MISSION_RETREAT && (Team == NULL || !Team->Is_Leaving_Map())) return(false);

	return(true);
}

// FUNCTION: GAMEMD 0x004DA230
bool FootClass::Is_Recruitable(HouseClass const * house) const
{
	if (house == NULL) {
		return(false);
	}

	if (house != House) {
		return(false);
	}

	if (IsInLimbo) {
		return(false);
	}

	if (Team != NULL) {
		return(false);
	}

	if (!Is_Recruitable_Mission(Mission)) {
		return(false);
	}

	if (!IsTeamRecruitable) {
		return(false);
	}

	return(true);
}

// FUNCTION: GAMEMD 0x004DA2A0
bool FootClass::Is_On_Priority_Mission(void) const
{
	if (Mission == MISSION_ENTER) return(true);
	return(false);
}

// FUNCTION: GAMEMD 0x004DA2B0
int FootClass::Do_MISSION_UNLOAD(void)
{
	return(BASECLASS::Do_MISSION_UNLOAD());
}

// FUNCTION: GAMEMD 0x004DA2C0
int FootClass::Do_MISSION_RETREAT(void)
{
	enum {
		FIND_EDGE,
		TRAVELLING
	};

	switch (Status) {
		case FIND_EDGE:
			if (NavCom != NULL) {
				Status = TRAVELLING;
			} else {
				Cell cell(0,0);

				if (Team != NULL && Team->Class->Get_Origin() != CELL_NONE) {
					cell = Map.Calculated_Cell(House->FUN_0050DAA0(), Team->Class->Get_Origin(), Center_Coord().As_Cell(), TClass->Speed);
				}

				if (cell == CELL_NONE) {
					cell = Map.Calculated_Cell(House->FUN_0050DAA0(), CELL_NONE, Center_Coord().As_Cell(), TClass->Speed);
				}

				Assign_Destination(&Map[cell]);
				Status = TRAVELLING;
			}
			break;

		case TRAVELLING:
			if (NavCom == NULL) {
				Status = FIND_EDGE;
			}
			break;
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: GAMEMD 0x004DA4E0
VisualType FootClass::Visual_Character(bool raw, HouseClass const * house) const
{
	VisualType visual = VISUAL_NORMAL;

	if (Locomotion != NULL) {
		visual = Locomotion->Visual_Character(raw);
	}
	if (visual == VISUAL_NORMAL) {
		visual = BASECLASS::Visual_Character(raw, house);
	}

	return(visual);
}

AudioEvent * __fastcall FUN_00750D40(Coord const & coord, AudioController * controller);
// FUNCTION: GAMEMD 0x004DA530
void FootClass::AI(void)
{
	BASECLASS::AI();

	if (IsActive) {
		IsIdle = false;

		if (Frame % Rule->RadApplicationDelay == 0 && !Techno_Type_Class()->ImmuneToRadiation && !In_Air() && !IsInLimbo) {
			Cell cell = Center_Coord();
			int rad = Map[cell].GetRadLevel();
			if (rad > 0) {
				int damage = (int)(rad * Rule->RadLevelFactor);
				Take_Damage(damage, 0, Rule->RadSiteWarhead, NULL, false, true, NULL);
				if (!IsActive) {
					return;
				}
			}
		}

		if (!IsInPlayfield && Techno_Type_Class()->BalloonHover) {
			if (Map.In_Local_Radar(Get_Cell_Ptr(), true)) {
				IsInPlayfield = true;
			}
		}

		if (Locomotion->Is_Moving_Now() && In_Air() && House->Is_Ally(PlayerPtr)) {
			if (SightTimer == 0) {
				vt_entry_48C(0, 0, 0, 0);
				UpdateSight(0, 0, 0, 0, 0);
				if (LastSightHeight != Get_Height_AGL()) {
					Map.RevealArea3(PositionCoord, 0, LastSightRange + 3, 0);
				} else {
					Map.RevealArea3(PositionCoord, LastSightRange - 3, LastSightRange + 3, 0);
				}
				LastSightHeight = Get_Height_AGL();
				SightTimer = 15;
			}
		}

		if (Frame % 16 == 0 && In_Air() && Get_Cell_Ptr()->Tag != NULL) {
			Get_Cell_Ptr()->Tag->Spring((TEventType)0x3b, this, Get_Cell());
		}

		int oldwalked = TotalFramesWalked;
		if (Locomotion != NULL && !IsSinking && !IsFalling && (DirectRockerLinkedUnit == NULL || Techno_Type_Class()->Pushy) && !IsInLimbo) {
			Locomotion->Process();
			if (!IsActive) {
				return;
			}
			UnitClass * unit = (this == NULL) ? NULL : (RTTI == RTTI_UNIT) ? (UnitClass *)this : NULL;
			if ((Locomotion->Is_Moving_Now() && !IsWarpingIn()) || (TarCom != NULL && Techno_Type_Class()->HoverAttack && unit != NULL && !unit->FUN_00746D80())) {
				if (Frame % Techno_Type_Class()->WalkRate == 0 && !IsBeingWarpedOut() && !IsWarpingIn() && !IsAttackedByLocomotor) {
					TotalFramesWalked++;
					goto walked;
				}
			}
			if (Techno_Type_Class()->IdleRate != 0 && !Locomotion->Is_Moving_Now() && Frame % Techno_Type_Class()->IdleRate == 0 && !IsBeingWarpedOut() && !IsWarpingIn() && !IsAttackedByLocomotor) {
				TotalFramesWalked++;
				goto walked;
			}
			if (Techno_Type_Class()->DeployToLand && Get_Height_AGL() > 0 && !IsBeingWarpedOut() && !IsWarpingIn() && !IsAttackedByLocomotor) {
				TotalFramesWalked++;
			}
		}
walked:

		if (oldwalked == TotalFramesWalked && !Locomotion->Is_Moving_Now() || IsFalling || IsCrashing || (DirectRockerLinkedUnit != NULL && Techno_Type_Class()->Pushy)) {
			if (IsMoveSoundPlaying) {
				if (MoveSoundDelay != 0 && !IsFalling && !IsCrashing) {
					MoveSoundDelay--;
				} else {
					MoveSoundAudioController.EndLooping();
					IsMoveSoundPlaying = false;
					MoveSoundDelay = 0;
				}
			}
		} else {
			if (!IsMoveSoundPlaying) {
				if (Techno_Type_Class()->MoveSound.Count() > 0) {
					MoveSoundAudioController.Stop();
					Coord coord = PositionCoord;
					TypeList<int> const & sounds = Techno_Type_Class()->MoveSound;
					unsigned int rnd = NonCriticalRandomNumber();
					VocClass::PlayAt(sounds[rnd % (unsigned int)sounds.Count()], coord, &MoveSoundAudioController);
					IsMoveSoundPlaying = true;
				}
			}
			MoveSoundDelay = 3;
		}

		if (IsFalling != IsABomb) {
			if (IsFalling) {
				if (Techno_Type_Class()->VoiceFalling != -1 && IsToExplode) {
					VocClass::PlayAt(Techno_Type_Class()->VoiceFalling, PositionCoord, NULL);
				}
			}
			IsABomb = IsFalling;
		}

		if (IsSinking != WasSinkingAlready) {
			if (IsSinking) {
				if (Techno_Type_Class()->VoiceSinking != -1) {
					VocClass::PlayAt(Techno_Type_Class()->VoiceSinking, PositionCoord, NULL);
				}
				if (Techno_Type_Class()->SinkingSound != -1) {
					VocClass::PlayAt(Techno_Type_Class()->SinkingSound, PositionCoord, &MoveSoundAudioController);
				} else if (Rule->SinkingSound != -1) {
					VocClass::PlayAt(Rule->SinkingSound, PositionCoord, &MoveSoundAudioController);
				}
			} else if (!IsMoveSoundPlaying) {
				MoveSoundAudioController.EndLooping();
			}
			WasSinkingAlready = IsSinking;
		}

		if (IsCrashing != WasCrashingAlready) {
			if (IsCrashing) {
				MoveSoundAudioController.EndLooping();
				if (Techno_Type_Class()->VoiceCrashing != -1 && House->Is_Player_Control()) {
					VocClass::PlayAt(Techno_Type_Class()->VoiceCrashing, PositionCoord, NULL);
				}
				if (Techno_Type_Class()->CrashingSound != -1) {
					VocClass::PlayAt(Techno_Type_Class()->CrashingSound, PositionCoord, &MoveSoundAudioController);
				}
			} else if (!IsMoveSoundPlaying) {
				MoveSoundAudioController.EndLooping();
			}
			WasCrashingAlready = IsCrashing;
		}

		FUN_00750D40(PositionCoord, &MoveSoundAudioController);

		if ((Frame & 63) == 63 && NavCom == NULL && !IsOnBridge && Get_Cell_Ptr()->Ramp == 0 && Is_On_Elevation() && Get_Height_AGL() == 0) {
			Scatter(Coord(0, 0, 0), true);
		}

		IPiggybackPtr piggy(Locomotion);
		if (piggy != NULL) {
			if (piggy->Is_Ok_To_End()) {
				piggy->End_Piggyback(&Locomotion);
			}
		}

		IonBlastYDrawOffset = 0;

		if (!IsWarpingIn()) {
			Enter_Object_Nearby();
		}

		if (ParasiteEatingMe != NULL) {
			ParasiteEatingMe->ParasiteImUsing->AI();
		}
	}
}

// FUNCTION: GAMEMD 0x004DAF10
void FootClass::Draw_A_VXL(VoxelDataStruct const & voxeldata, int frame, int key, VoxelIndexClass * cache, Rect const & cliprect, Point2D const & point, Matrix3D const & matrix, int brightness, ShapeFlags_Type flags, int unknown) const
{
	Matrix3D mtx = matrix;
	Point2D pt = point;

	if (Locomotion != NULL) {
		pt = Point2D(Locomotion->Draw_Point()) + point;
	}

	TechnoClass::Draw_Voxel(voxeldata, frame, key, cache, cliprect, pt, mtx, brightness, unknown, flags);
}

// FUNCTION: GAMEMD 0x004DAFC0
int FootClass::Get_Z_Adjust(void) const
{
	int adjust = 0;
	if (Locomotion != NULL) {
		adjust = Locomotion->Z_Adjust();
	}

	int column = TClass->ZFudgeColumn * Get_Z_Fudge_Column();
	int tunnel = TClass->ZFudgeTunnel * Get_Z_Fudge_Tunnel();
	int cliff = TClass->ZFudgeCliff * Get_Z_Fudge_Cliff();
	int bridge = 0;
	if (Is_Z_Fudge_Bridge()) {
		bridge = TClass->ZFudgeBridge;
	}

	int max_fudge = MAX(column, tunnel);
	max_fudge = MAX(max_fudge, cliff);
	max_fudge = MAX(max_fudge, bridge);

	adjust += BASECLASS::Get_Z_Adjust();
	adjust += max_fudge;
	return(adjust);
}

// FUNCTION: GAMEMD 0x004DB0A0
ZGradientType FootClass::Get_Z_Gradient(void) const
{
	if (Locomotion != NULL) {
		return(Locomotion->Z_Gradient());
	}
	return(ZGRAD_90DEG);
}

// FUNCTION: GAMEMD 0x004DB0D0
void FootClass::Draw_Voxel_Shadow(VoxelDataStruct const & voxeldata, int layer_index, int key, VoxelIndexClass * cache, Rect const & cliprect, Point2D const & point, Matrix3D const & matrix, bool force_cache, Surface * surface, int shadow_x, int shadow_y) const
{
	if (Locomotion != NULL && Locomotion->Is_To_Have_Shadow() == (boolean)true) {
		Point2D drawpoint = point;
		if (Locomotion != NULL) {
			drawpoint = Point2D(Locomotion->Shadow_Point()) + point;
		}
		Techno_Draw_Voxel_Shadow(voxeldata, layer_index, key, cache, cliprect, drawpoint, matrix, force_cache, surface, shadow_x, shadow_y);
	}
}

// FUNCTION: GAMEMD 0x004DB1A0
int FootClass::Current_Speed(void)
{
	int speed = Get_Max_Speed() * House->FUN_0050C050(TClass) * SpeedBias;
	if (Has_Ability(ABILITY_FASTER)) {
		speed *= Rule->VeteranSpeed;
	}
	speed *= Speed;
	if (RTTI == RTTI_UNIT && ((UnitClass *)this)->Flagged != HOUSE_NONE) {
		speed /= 2;
	}
	return(speed);
}

// FUNCTION: GAMEMD 0x004DB250
void FootClass::Draw_It(Point2D const &, Rect const &) const
{
}

// FUNCTION: GAMEMD 0x004DB260
bool FootClass::Limbo(void)
{
	if (!IsInLimbo && Techno_Type_Class() != NULL) {
		Cell cell = LastAdjacencyCell;
		for (int face = FACING_FIRST; face < FACING_COUNT; face++) {
			Cell c = Adjacent_Cell_By_Value(cell, FacingType(face));
			CellClass * cptr = &Map[c];
			cptr->AdjacentObjectCount--;
		}
		if (!ScenarioInit) {
			Stop_Driver();
		}
		if (Locomotion != NULL) {
			Locomotion->Mark_All_Occupation_Bits(0);
			Locomotion->Limbo();
		}
		MoveSoundAudioController.End();
		if (Techno_Type_Class()->SensorsSight != 0) {
			RemoveSensorsAt(CELL_NONE);
		}
		if (GetLastFlightMapCoords() != CELL_NONE) {
			AircraftTracker.Remove(this);
		}
	}
	return(BASECLASS::Limbo());
}

void __fastcall FUN_004E02C0(void * loco);

// FUNCTION: GAMEMD 0x004DB3C0
HRESULT STDMETHODCALLTYPE FootClass::Load(IStream * stream)
{
	if (Locomotion) {
		((ILocomotion *)Locomotion)->Release();
	}
	Locomotion.Detach();

	HRESULT result = TechnoClass::Load(stream);
	if (SUCCEEDED(result)) {
		int index;
		int count;

		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		new (&RouteQueue) DynamicVectorClass<AbstractClass *>;

		for (index = 0; index < count; index++) {
			AbstractClass * obj = NULL;
			result = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(result)) {
				return(result);
			}
			RouteQueue.Add(obj);
		}

		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		new (&NavQueue) DynamicVectorClass<AbstractClass *>;

		for (index = 0; index < count; index++) {
			AbstractClass * obj = NULL;
			result = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(result)) {
				return(result);
			}
			NavQueue.Add(obj);
		}

		Locomotion.Detach();
		FUN_004E02C0(&reinterpret_cast<char &>(Locomotion));
		Locomotion.Detach();
		OleLoadFromStream(stream, IID_ILocomotion, (LPVOID *)&reinterpret_cast<char &>(Locomotion));
		unknown_5A0 = NULL;
		Swizzle_Pointer(&Team);
		Swizzle_Pointer(&Member);
		Swizzle_Pointer(&NavCom);
		Swizzle_Pointer(&SuspendedNavCom);
		Swizzle_Pointer(&MegaDestination);
		Swizzle_Pointer(&MegaTarget);
		Swizzle_Pointer(&PatrolCell);
		Swizzle_Pointer(&ParasiteEatingMe);
		Swizzle_Pointer(&ParasiteImUsing);
		MoveSoundAudioController.FUN_00405BE0();
		IsMoveSoundPlaying = false;
		MoveSoundDelay = 0;
		for (index = 0; index < NavQueue.Count(); index++) {
			Swizzle_Pointer(&NavQueue[index]);
		}
		for (index = 0; index < RouteQueue.Count(); index++) {
			Swizzle_Pointer(&RouteQueue[index]);
		}
		return(S_OK);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004DB690
HRESULT STDMETHODCALLTYPE FootClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = TechnoClass::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int index;
		int count = RouteQueue.Count();

		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (index = 0; index < count; index++) {
			result = stream->Write(&RouteQueue[index], sizeof(RouteQueue[index]), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		count = NavQueue.Count();

		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (index = 0; index < count; index++) {
			result = stream->Write(&NavQueue[index], sizeof(NavQueue[index]), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}

		IPersistStreamPtr persist(Locomotion);
		OleSaveToStream(persist, stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004DB7E0
LayerType FootClass::In_Which_Layer(void) const
{
	return(Locomotion->In_Which_Layer());
}

// FUNCTION: GAMEMD 0x004DB810
void FootClass::Set_Coord(Coord const & coord)
{
	Coord old = Position;
	bool moved;
	if (coord != old) {
		moved = true;
	} else {
		moved = false;
	}
	if (IsDown) {
		Mark(MARK_UP);
		BASECLASS::Set_Coord(coord);
		Mark(MARK_DOWN);
	} else {
		BASECLASS::Set_Coord(coord);
	}
	if (moved && Techno_Type_Class()->OpenTopped) {
		UpdatePassengerCoords();
	}
}

namespace LocomotionClass { namespace CLSIDs { extern const CLSID Droppod; } }

// FUNCTION: GAMEMD 0x004DB8A0
void FootClass::Link_DropPod(void)
{
	ILocomotionPtr locomotion = Locomotion;
	ILocomotionPtr ballistic(LocomotionClass::CLSIDs::Droppod);
	ballistic->Link_To_Object(this);
	IPiggybackPtr piggy(ballistic);
	piggy->Begin_Piggyback(locomotion);
	Locomotion = ballistic;
}

// FUNCTION: GAMEMD 0x004DB9B0
bool FootClass::Is_Docked_For_Repair(void)
{
	if (NavCom == NULL && TarCom == NULL) {
		TechnoClass * whom = FUN_0065AD30(0);
		BuildingClass * bptr = NULL;
		if (whom != NULL && whom->RTTI == RTTI_BUILDING) {
			bptr = (BuildingClass *)whom;
		}
		bool going_to_repair = (CurrentMission == MISSION_ENTER && bptr != NULL && bptr->Class->IsCanUnitRepair);
		return(Enter_Idle_Mode(false, going_to_repair == false));
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004DBA30
void FootClass::Stop_Movement_Animation(void)
{
}

// FUNCTION: GAMEMD 0x004DBA40
void FootClass::vt_entry_54C(void)
{
}

// FUNCTION: GAMEMD 0x004DBA50
bool FootClass::Is_In_Same_Zone_As(ObjectClass const * object) const
{
	return(Is_In_Same_Zone(object->Destination_Coord((TechnoClass *)this).As_Cell()));
}

// FUNCTION: GAMEMD 0x004DBAD0
void FootClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(ThreatAvoidanceCoefficient);
	crc(LastPathingCell.Length());
	crc(LastTubeCoord.Length());
	crc(Speed);
	crc(SpeedBias);
	if (NavCom != NULL) crc(NavCom->Fetch_ID());
	if (SuspendedNavCom != NULL) crc(SuspendedNavCom->Fetch_ID());
	if (Team != NULL) crc(Team->Fetch_ID());
	if (Member != NULL) crc(Member->Fetch_ID());
	crc((int)PathDelay);
	crc(TryTryAgain);
	crc((int)BaseAttackTimer);
	crc(HeadToCoord.Length());
	crc(CurrentTube);
	crc(CurrentTubeDir);
	crc(IsScanLimited);
	crc(IsInitiated);
	crc(IsNewNavCom);
	crc(IsPlanningToLook);
	crc(IsDeploying);
	crc(IsFiring);
	crc(IsRotating);
	crc(IsUnloading);
	crc(IsNavQueueLoop);
	crc(IsScattering);
	crc(IsIdle);
	crc(IonBlastYDrawOffset);
}

// FUNCTION: GAMEMD 0x004DBDA0
bool FootClass::Is_Allowed_To_Recloak(void) const
{
	if (BASECLASS::Is_Allowed_To_Recloak()) {
		if (!Techno_Type_Class()->IsCloakStop || !Locomotion->Is_Moving_Now()) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004DBDF0
Coord FootClass::Destination_Coord(TechnoClass * docker) const
{
	if (CurrentTube >= TUBE_FIRST) {
		Cell exit = Tubes[CurrentTube]->Exit;
		return(exit.As_Coord());
	} else {
		CoordStruct tmp = Locomotion->Head_To_Coord();
		Coord head_to = tmp;
		if (head_to != COORD_NONE) {
			return(head_to);
		}
		return(Center_Coord());
	}
}

// FUNCTION: GAMEMD 0x004DBED0
bool FootClass::Captured(HouseClass * newowner, bool announce)
{
	if (Techno_Type_Class()->SensorsSight != 0) {
		RemoveSensorsAt(PositionCell);
	}
	if (BASECLASS::Captured(newowner, true)) {
		if (House->IsHuman && Team != NULL) {
			Team->Remove(this, -1, false);
		}
		if (!In_Air()) {
			LastAdjacencyCell = PositionCell;
		}
		if (Techno_Type_Class()->SensorsSight != 0) {
			AddSensorsAt(PositionCell);
		}
		return(true);
	}
	if (Techno_Type_Class()->SensorsSight != 0) {
		AddSensorsAt(PositionCell);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004DBFD0
bool FootClass::Is_Considered_Slow(void)
{
	if (Get_Class_Weapon_Data(0) == NULL) {
		return(true);
	}
	if (Techno_Type_Class()->Level == -1) {
		return(true);
	}
	if (Techno_Type_Class()->DeploysInto != NULL) {
		return(true);
	}
	if (Techno_Type_Class()->MaxSpeed > 13) {
		return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004DC030
void FootClass::On_Movement_Blocked(void)
{
	if (Get_Mission() == MISSION_HUNT) {
		Assign_Target(NULL);
		Assign_Destination(NULL);
	}
}

// FUNCTION: GAMEMD 0x004DC060
void FootClass::Draw_Action_Line(bool force, int unknown) const
{
	if (!TarCom && !NavCom) {
		return;
	}

	if (force || ActionLineTimer > 0) {
		Coord start_coord;
		Coord end_coord;

		if (TarCom != NULL) {
			start_coord = Turret_Coord();
			end_coord = Predict_Target_Coord();
			RGBClass color = DSurface::Deconstruct_Hicolor_Pixel(NormalDrawer->Convert_Pixel(RED));
			FUN_007049C0(start_coord, end_coord, color, 0, 0);
		} else {
			start_coord = PositionCoord;
			if (RouteQueue.Count() == 0) {
				end_coord = NavCom->Center_Coord();
			} else {
				end_coord = RouteQueue[RouteQueue.Count() - 1]->Center_Coord();
			}
			if (Map.In_Radar(end_coord.As_Cell()) && Map[end_coord].IsUnderBridge) {
				end_coord.Z = BRIDGE_LEPTON_HEIGHT + Map.Get_Height_GL(end_coord);
			}
			RGBClass color = DSurface::Deconstruct_Hicolor_Pixel(NormalDrawer->Convert_Pixel(GREEN));
			FUN_007049C0(start_coord, end_coord, color, unknown, 0);
		}
	}
}

extern bool DAT_00822540[];

static inline Rect Action_Line_Rect(Point2D const & point)
{
	return(Rect(point - Point2D(2, 2), 3, 3));
}

static inline void Fill_Action_Rect(Point2D const & point, int pixel)
{
	DSurface::Composite->Fill_Rect(Intersect(DSurface::ViewBounds, Action_Line_Rect(point)), pixel);
}

// FUNCTION: GAMEMD 0x004DC340
void FootClass::FUN_004DC340(void)
{
	RGBClass white(255, 255, 255);
	RGBClass color;

	if (!TarCom && !NavCom) {
		return;
	}

	Coord start_coord;
	Coord end_coord;

	if (TarCom != NULL) {
		start_coord = Turret_Coord();
		end_coord = Predict_Target_Coord();
	} else {
		start_coord = PositionCoord;
		if (RouteQueue.Count() == 0) {
			end_coord = NavCom->Center_Coord();
		} else {
			end_coord = RouteQueue[RouteQueue.Count() - 1]->Center_Coord();
		}
		if (Map.In_Radar(end_coord.As_Cell()) && Map[end_coord].IsUnderBridge) {
			end_coord.Z = BRIDGE_LEPTON_HEIGHT + Map.Get_Height_GL(end_coord);
		}
	}

	Point2D start_point;
	Point2D end_point;

	TacticalMap->Coord_To_Pixel(start_coord, start_point);
	TacticalMap->Coord_To_Pixel(end_coord, end_point);

	start_point.Y += DSurface::ViewBounds.Y;
	end_point.Y += DSurface::ViewBounds.Y;

	color = House->RemapColorRGB;
	int pixel = DSurface::Build_Hicolor_Pixel(color);

	Fill_Action_Rect(start_point, pixel);
	Fill_Action_Rect(end_point, pixel);

	Rect bounds = DSurface::ViewBounds;
	if (Clip_Line_To_Rect(start_point, end_point, bounds)) {
		unsigned int now = timeGetTime();
		int phase = now & 0x3ff;
		if (!(phase & 0x200)) {
			if (phase & 0x100) {
				phase ^= 0xff;
			}
			color.Adjust(phase & 0xff, white);
		}
		int offset = (-(int)now >> 5) & 0xf;
		DSurface::Composite->Draw_Masked_Dashed_Line(start_point, end_point, DSurface::Build_Hicolor_Pixel(color), DAT_00822540, offset, false);
	}
}

// FUNCTION: GAMEMD 0x004DC760
double FootClass::Threat_Avoidance_Value(void) const
{
	if (Team != NULL && Team->Class->AvoidThreats) {
		return(1.0);
	}
	return(ThreatAvoidanceCoefficient);
}

// FUNCTION: GAMEMD 0x004DC790
bool FootClass::Should_Delete_Off_Map(void)
{
	if (!IsInPlayfield) {
		return(false);
	}
	if (Team != NULL && !Team->Is_Leaving_Map()) {
		return(false);
	}
	if (TClass->IsTrain && NavCom != NULL && Map.In_Local_Radar(NavCom->Center_Coord())) {
		return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004DC810
void FootClass::Set_Waypoint_Path(PathType path, char index)
{
	if (path != PATH_NONE) {
		CurrentPath = path;
		NextWaypoint = index;
		WaypointClass * wpc = PlayerPtr->Paths[path]->Get_Waypoint(index);
		WaypointTargetCell = wpc->Location.As_Cell();
	} else {
		CurrentPath = PATH_NONE;
		NextWaypoint = 0;
		WaypointTargetCell = Cell(0, 0);
	}
	WaypointOffsetCell = Cell(0, 0);
}

// FUNCTION: GAMEMD 0x004DC8C0
bool FootClass::Execute_Waypoint_Path(WaypointClass * waypoint)
{
	bool action_succeeded = false;
	ActionType occupier_action;
	Cell waypoint_offset;
	int waypoint_zone;
	int adjusted_zone;

	if (unknown_bool_430) {
		unknown_bool_43A = true;
	}

	if (waypoint != NULL) {
		PlayerPtr->Fetch_Waypoint_Data(waypoint, CurrentPath, NextWaypoint);

		Coord waypoint_coord = waypoint->Location;
		ObjectClass * occupying_object = Map[waypoint_coord].Cell_Occupier();
		TechnoClass * occupying_techno = Dynamic_Cast<TechnoClass *>(occupying_object);

		WaypointTargetCell = waypoint_coord;

		bool was_voice_allowed = AllowVoice;
		AllowVoice = false;

		Cell waypoint_cell = waypoint_coord;

		bool is_direct_waypoint = true;
		bool should_click_occupier = false;

		if (occupying_techno != NULL) {
			BuildingClass * building;
			bool needs_repair;
			bool needs_reload;
			bool is_allied_building;

			occupier_action = What_Action(occupying_object, true);

			if (PlayerPtr->Is_Ally(occupying_techno->House)) {
				if (occupying_techno->RTTI == RTTI_BUILDING) {
					building = (BuildingClass *)occupying_techno;

					needs_repair = (building->Class->IsCanUnitRepair &&
						Strength < (int)TClass->MaxStrength &&
						RTTI != RTTI_INFANTRY);

					needs_reload = (building->Class->IsCanUnitReload &&
						Ammo < TClass->MaxAmmo &&
						Ammo != -1);

					is_allied_building = building->House->Is_Ally(PlayerPtr);

					if ((needs_repair || needs_reload) &&
						is_allied_building &&
						occupier_action != ACTION_SELECT &&
						occupier_action != ACTION_TOGGLE_SELECT) {
						should_click_occupier = true;
					}
				}
			} else if (occupier_action != ACTION_TOGGLE_SELECT &&
					   occupier_action != ACTION_SELECT &&
					   occupier_action != ACTION_MOVE &&
					   occupier_action != ACTION_NOMOVE) {
				should_click_occupier = true;
			}

			if (!should_click_occupier) {
				Cell nearby_cell = Map.Nearby_Location(
					waypoint_cell,
					TClass->Speed,
					Map.Get_Cell_Zone(waypoint_cell, TClass->MZone, Map[waypoint_cell].IsUnderBridge),
					TClass->MZone,
					false,
					Point2D(1, 1),
					false,
					true,
					false,
					true,
					Cell(0, 0));

				if (nearby_cell != CELL_NONE) {
					WaypointOffsetCell = nearby_cell - waypoint_cell;
					is_direct_waypoint = false;
				}
			}
		}

		if (should_click_occupier) {
			occupier_action = What_Action(occupying_object, true);
			action_succeeded = Active_Click_With(occupier_action, occupying_object, true);
			WaypointOffsetCell = Cell(0, 0);
		} else {

			occupier_action = What_Action(waypoint_cell, false, true);
			waypoint_offset = WaypointOffsetCell;
			Cell adjusted_waypoint = waypoint_cell + waypoint_offset;
			CellClass * adjusted_cell = &Map[adjusted_waypoint];
			CellClass * waypoint_map_cell = &Map[waypoint_cell];
			ActionType adjusted_action = What_Action(adjusted_waypoint, false, true);

			waypoint_zone = Map.Get_Cell_Zone(waypoint_cell, TClass->MZone, waypoint_map_cell->IsUnderBridge);
			adjusted_zone = Map.Get_Cell_Zone(adjusted_waypoint, TClass->MZone, adjusted_cell->IsUnderBridge);
			int destination_zone = Map.Get_Cell_Zone(Destination_Coord().As_Cell(), TClass->MZone, Is_Moving_Onto_Bridge());
			MoveType entry_result = Can_Enter_Cell(adjusted_cell, FACING_NONE,
				adjusted_cell->Height + (BRIDGE_CELL_HEIGHT * adjusted_cell->IsUnderBridge), 0, true);

			int walk_distance_limit = MAX(abs((int)waypoint_offset.X), abs((int)waypoint_offset.Y));
			walk_distance_limit = MIN(5, walk_distance_limit + 3);

			if (occupier_action != adjusted_action && is_direct_waypoint ||
				entry_result != MOVE_OK ||
				waypoint_zone != adjusted_zone ||
				destination_zone != waypoint_zone ||
				is_direct_waypoint && Search.Test_Cell_Walk(adjusted_waypoint, waypoint_cell, this,
					adjusted_cell->IsUnderBridge, waypoint_map_cell->IsUnderBridge, MZONE_NONE) > walk_distance_limit) {
				action_succeeded = Active_Click_With(occupier_action, waypoint_cell, Cell(-1, -1), true);
				WaypointOffsetCell = Cell(0, 0);
			} else {
				action_succeeded = Active_Click_With(occupier_action, adjusted_waypoint, Cell(-1, -1), true);
				WaypointTargetCell = adjusted_waypoint;
			}
		}

		if (was_voice_allowed) {
			AllowVoice = true;
		}
	}

	if (!action_succeeded) {
		CurrentPath = PATH_NONE;
		NextWaypoint = 0;
		WaypointOffsetCell = Cell(0, 0);
		WaypointTargetCell = Cell(0, 0);
	}

	unknown_bool_43A = false;
	return(action_succeeded);
}

// FUNCTION: GAMEMD 0x004DCE80
bool FootClass::Tiberium_Check(Cell & center)
{
	if (!Map.In_Local_Radar(center)) return(false);

	if ((Session.Type != GAME_NORMAL || (!IsOwnedByPlayer || !Map.Is_Shrouded(center.As_Coord(Map.Get_Height_GL(center)))))) {
		if (!Map.Is_Same_Cell_Zone(Destination_Coord().As_Cell(), center, TClass->MZone, Is_Moving_Onto_Bridge(), false, false)) return(false);
		CellClass * cptr = &Map[center];
		if (cptr->Land_Type() == LAND_TIBERIUM && !Can_Enter_Cell(cptr)) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004DCFE0
bool FootClass::Goto_Tiberium(int rad, bool allow_weighted)
{
	if (NavCom == NULL) {
		Cell newcell = ScanForTiberium(rad, allow_weighted);
		if (newcell != CELL_NONE) {
			if (newcell == Center_Coord().As_Cell()) {
				return(true);
			}
			Assign_Destination(&Map[newcell]);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004DD0A0
Cell FootClass::ScanForTiberium(int rad, bool allow_weighted)
{
	Cell bestcell = CELL_NONE;

	Cell center = Center_Coord().As_Cell();
	if (Map[center].Land_Type() == LAND_TIBERIUM) {
		return(center);
	} else {
		Cell cell(0, 0);
		int bestvalue = -1;
		for (int radius = 1; radius < rad; radius++) {
			for (int x = -radius; x <= radius; x++) {
				cell = center + Cell(x, -radius);
				if (Tiberium_Check(cell)) {
					int value = Map[cell].Tiberium_Value();
					if (value > bestvalue) {
						bestvalue = value;
						bestcell = cell;
					}
				}

				cell = center + Cell(x, +radius);
				if (Tiberium_Check(cell)) {
					int value = Map[cell].Tiberium_Value();
					if (value > bestvalue) {
						bestvalue = value;
						bestcell = cell;
					}
				}

				cell = center + Cell(-radius, x);
				if (Tiberium_Check(cell)) {
					int value = Map[cell].Tiberium_Value();
					if (value > bestvalue) {
						bestvalue = value;
						bestcell = cell;
					}
				}

				cell = center + Cell(+radius, x);
				if (Tiberium_Check(cell)) {
					int value = Map[cell].Tiberium_Value();
					if (value > bestvalue) {
						bestvalue = value;
						bestcell = cell;
					}
				}
			}
			if (bestvalue != -1) break;
		}
	}
	return(bestcell);
}

// FUNCTION: GAMEMD 0x004DD890
Cell FootClass::Search_For_Weed(int rad)
{
	Cell bestcell = CELL_NONE;
	Cell center = Center_Coord().As_Cell();
	if (Weed_Check(center, 0, 0)) {
		return(center);
	} else {
		Cell cell(0, 0);
		for (int radius = 1; radius < rad; radius++) {
			for (int x = -radius; x <= radius; x++) {
				cell = center;
				if (Weed_Check(cell, x, -radius)) {
					bestcell = cell;
				}

				cell = center;
				if (Weed_Check(cell, x, +radius)) {
					bestcell = cell;
				}

				cell = center;
				if (Weed_Check(cell, -radius, x)) {
					bestcell = cell;
				}

				cell = center;
				if (Weed_Check(cell, +radius, x)) {
					bestcell = cell;
				}
			}
			if (bestcell != CELL_NONE) break;
		}
	}
	return(bestcell);
}

// FUNCTION: GAMEMD 0x004DD9F0
bool FootClass::Weed_Check(Cell & center, int x, int y)
{
	Cell cell = center + Cell(x, y);

	if (!Map.In_Local_Radar(cell)) return(false);

	center = cell;

	if ((Session.Type != GAME_NORMAL || (!IsOwnedByPlayer || !Map.Is_Shrouded(center.As_Coord(Map.Get_Height_GL(center)))))) {
		if (!Map.Is_Same_Cell_Zone(Destination_Coord().As_Cell(), center, TClass->MZone, Is_Moving_Onto_Bridge(), false, false)) return(false);
		CellClass * cptr = &Map[center];
		if (!Can_Enter_Cell(cptr) && cptr->Land_Type() == LAND_WEEDS && cptr->OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004DDB90
bool FootClass::Goto_Weed(int rad)
{
	if (NavCom == NULL) {
		Cell newcell = Search_For_Weed(rad);
		if (newcell != CELL_NONE) {
			if (newcell == Center_Coord().As_Cell()) {
				return(true);
			}
			Assign_Destination(&Map[newcell]);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004DDC40
bool FootClass::Is_Moving_Onto_Bridge(TechnoClass * docker) const
{
	if (CurrentTube >= 0) {
		return(false);
	}
	return(BASECLASS::Is_Moving_Onto_Bridge(docker));
}

// FUNCTION: GAMEMD 0x004DDC60
bool FootClass::Is_LZ_Clear(AbstractClass * target) const
{
	if (target == NULL) return(false);
	Cell cell = target->Center_Coord().As_Cell();
	if (!Map.In_Local_Radar(cell)) return(false);

	TechnoClass * object = Map[cell].Cell_Techno();
	if (object) {
		if ((TechnoClass const *)this == object) return(true);

		if (FUN_0065AD40(0) == object) {
			return(true);
		}
		if (Techno_Type_Class()->Spawned) {
			if (object->SpawnManager != NULL) return(true);
			if (object->Techno_Type_Class()->Spawned) return(true);
		}
		return(false);
	}

	if (!Map[cell].Is_Clear_To_Move(SPEED_TRACK, false, false)) return(false);

	for (int i = 0; i < Aircraft.Count(); i++) {
		AircraftClass * aptr = Aircraft[i];
		if (aptr->IsActive && !aptr->IsInLimbo && aptr->NavCom == target && aptr != this) {
			return(false);
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x004DDDE0
ActionType FootClass::What_Action(Cell const & cell, bool check_fog, bool disallow_force) const
{
	ActionType action = BASECLASS::What_Action(cell, check_fog, disallow_force);

	Coord coord = cell;
	coord.Z = Map.Get_Height_GL(coord);
	if (Map[coord].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	if (Map.Is_Shrouded(coord) && action != ACTION_NONE) {
		if (Techno_Type_Class()->IsMoveToShroud && Map.In_Local_Radar(cell)) {
			if (action != ACTION_PATROL_WAYPOINT) {
				action = ACTION_MOVE;
			}
		} else {
			action = ACTION_NOMOVE;
		}
	}

	return(action);
}

// FUNCTION: GAMEMD 0x004DDED0
ActionType FootClass::What_Action(ObjectClass const * target, bool disallow_force) const
{
	ActionType action = BASECLASS::What_Action(target, disallow_force);

	Coord coord = target->Center_Coord();
	coord.Z = Map.Get_Height_GL(coord);
	if (Map[coord].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	if (Map.Is_Shrouded(coord) && action != ACTION_NONE && Session.Type == GAME_NORMAL) {
		if (Techno_Type_Class()->IsMoveToShroud) {
			action = ACTION_MOVE;
		} else {
			action = ACTION_NOMOVE;
		}
	}

	return(action);
}

// FUNCTION: GAMEMD 0x004DDF90
int FootClass::Do_MISSION_RESCUE(void)
{
	enum {
		ENGAGE_THREATS,
		TRAVELLING
	};

	switch (Status) {

		case ENGAGE_THREATS:
			if (TarCom != NULL) {
				Approach_Target();
			} else {
				if (ArchiveTarget == NULL) {
					ArchiveTarget = &Map[(Coord const &)PositionCoord];
				}

				IsScanLimited = false;

				ObjectClass * threat = (ObjectClass *)Greatest_Threat(THREAT_NORMAL, ArchiveTarget->Center_Coord(), false);
				if (threat != NULL) {
					if (threat->Distance(ArchiveTarget->Center_Coord()) < Threat_Range(1) * 1.5) {
						Assign_Target(threat);
					}
				}

				if (TarCom != NULL) {
					return(1);
				} else {
					Status = TRAVELLING;
					Cell where = House->Where_To_Go(this);
					if (where != CELL_NONE) {
						Assign_Destination(&Map[where]);
					} else {
						Assign_Destination(NULL);
					}
					ArchiveTarget = NULL;
				}
			}
			break;

		case TRAVELLING:
			if (NavCom == NULL) {
				ArchiveTarget = NULL;
				QueueMission(MISSION_GUARD_AREA, false);
				Commence();
			}
			break;
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: GAMEMD 0x004DE1D0
Cell FootClass::Move_Order(Cell const & where, bool consider_fog)
{
	ActionType action = What_Action(where, consider_fog, false);
	bool inradar = Map.In_Local_Radar(where);

	Coord coord = where;
	coord.Z = Map.Get_Height_GL(coord);
	if (Map[coord].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	if (Techno_Type_Class()->IsMoveToShroud || !Map.Is_Shrouded(coord)) {

		Cell cell = where;
		CellClass * cptr = &Map[Destination_Coord(NULL)];

		MZoneType mzone = Techno_Type_Class()->MZone;
		if (mzone == MZONE_SUBTERANNEAN) {
			mzone = MZONE_NORMAL;
		} else if (mzone == MZONE_FLYER && House->IonSensitivesShouldBeOffline()) {
			mzone = MZONE_INFANTRY;
		}

		bool moveanywhere = (In_Air() || Techno_Type_Class()->IsSubterranean || (RTTI == RTTI_INFANTRY && ((InfantryClass *)this)->Class->JumpJet && !House->IonSensitivesShouldBeOffline()) || RTTI == RTTI_AIRCRAFT) ? true : false;
		bool ontobridge = Is_Moving_Onto_Bridge(NULL);

		Coord coord2 = where.As_Coord();
		coord2.Z = Map.Get_Height_GL(coord2);
		if (Map[coord2].IsUnderBridge) {
			coord2.Z += BRIDGE_LEPTON_HEIGHT;
		}

		bool shroud = Map.Is_Shrouded(coord2);

		if (moveanywhere && action == ACTION_NOMOVE) {
			cell = Map.Nearby_Location(where, Techno_Type_Class()->Speed, -1, mzone, Map[where].IsUnderBridge, Point2D(1, 1), false, true, Techno_Type_Class()->IsSubterranean && HeightAGL < 0);
		} else if (!inradar || shroud || (!moveanywhere && (!Techno_Type_Class()->Teleporter || action == ACTION_NOMOVE) && !Map.Is_Same_Cell_Zone(cptr->CellID, cell, mzone, ontobridge, Map[where].IsUnderBridge, false))) {
			cell = Map.Nearby_Location(where, Techno_Type_Class()->Speed, Map.Get_Cell_Zone(cptr->CellID, mzone, ontobridge), mzone, Map[where].IsUnderBridge, Point2D(1, 1), false, true, false, true);
		}

		if (cell != CELL_NONE) {
			return(cell);
		}
	}

	return(CELL_NONE);
}

// FUNCTION: GAMEMD 0x004DE580
void FootClass::Advance_Waypoint_Path(void)
{
	if (CurrentPath != PATH_NONE) {
		WaypointClass * wp = PlayerPtr->Paths[CurrentPath]->Get_Waypoint(NextWaypoint);
		WaypointClass * next_wp = PlayerPtr->Paths[CurrentPath]->Get_Next_Waypoint(wp);
		Execute_Waypoint_Path(next_wp);
	}
}

// FUNCTION: GAMEMD 0x004DE5D0
void FootClass::Delete_Me(void)
{
	if (CaptureManager != NULL) {
		CaptureManager->FreeAll();
	}
	if (LocomotorTarget != NULL) {
		ReleaseLocomotor(true);
	}
	if (Team != NULL) {
		Team->Remove(this, -1, false);
	}
	BASECLASS::Delete_Me();
}

// FUNCTION: GAMEMD 0x004DE620
bool FootClass::In_Air(void) const
{
	return(BASECLASS::In_Air());
}

// FUNCTION: GAMEMD 0x004DE630
void FootClass::AddPassenger(FootClass * passenger)
{
	BASECLASS::AddPassenger(passenger);
	if (Techno_Type_Class()->Gunner && Cargo.How_Many() == 1) {
		ReceiveGunner(passenger);
	}
}

// FUNCTION: GAMEMD 0x004DE670
void FootClass::RemovePassenger(FootClass * passenger)
{
	Cargo.Detach(passenger);
	if (passenger != NULL && Techno_Type_Class()->LeaveTransportSound != -1) {
		VocClass::PlayIndexAtPos(Techno_Type_Class()->LeaveTransportSound, PositionCoord, 0);
	}
	if (Techno_Type_Class()->Gunner && !Cargo.How_Many()) {
		RemoveGunner(passenger);
	}
}

// FUNCTION: GAMEMD 0x004DE710
FootClass * FootClass::RemoveFirstPassenger(void)
{
	FootClass * passenger = Cargo.Detach_Object();
	if (Techno_Type_Class()->Gunner && !Cargo.How_Many()) {
		RemoveGunner(passenger);
	}
	return(passenger);
}

// FUNCTION: GAMEMD 0x004DE750
void FootClass::ReceiveGunner(FootClass * gunner)
{
}

// FUNCTION: GAMEMD 0x004DE760
void FootClass::RemoveGunner(FootClass * gunner)
{
}

// FUNCTION: GAMEMD 0x004DE770
bool FootClass::IsParalyzed(void) const
{
	return(ParalysisTimer.Value() != 0);
}

// FUNCTION: GAMEMD 0x004DE7B0
void FootClass::AddSensorsAt(Cell cell)
{
	int radius = Techno_Type_Class()->SensorsSight;
	int radius_sq = radius * radius;
	HousesType house = House->HeapID;

	if (cell == CELL_NONE) {
		cell = Center_Coord().As_Cell();
	}

	for (int y = -radius; y < radius; y++) {
		for (int x = -radius; x < radius; x++) {
			Cell here = Cell(cell.X + x, cell.Y + y);
			if (x * x + y * y < radius_sq) {
				CellClass * cptr = &Map[here];
				cptr->Sensed_By(house);
				for (ObjectClass * obj = cptr->Cell_Occupier(); obj; obj = obj->Next) {
					RTTIType rtti = obj->RTTI;
					if (rtti == RTTI_UNIT || rtti == RTTI_INFANTRY || rtti == RTTI_AIRCRAFT) {
						((TechnoClass *)obj)->Sensed();
					}
				}
				BuildingClass * building = cptr->Cell_Building();
				if (building && building->House != PlayerPtr && building->Visual_Character(false, NULL)) {
					building->IsToDisplay = true;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004DE940
void FootClass::RemoveSensorsAt(Cell cell)
{
	int radius = Techno_Type_Class()->SensorsSight;
	int radius_sq = radius * radius;
	HousesType house = House->HeapID;

	if (cell == CELL_NONE) {
		cell = Center_Coord().As_Cell();
	}

	for (int y = -radius; y < radius; y++) {
		for (int x = -radius; x < radius; x++) {
			Cell here = Cell(cell.X + x, cell.Y + y);
			if (x * x + y * y < radius_sq) {
				CellClass * cptr = &Map[here];
				if (!cptr->Is_Sensed(house)) {
					continue;
				}
				{
					cptr->Unsensed_By(house);
					for (ObjectClass * obj = cptr->Cell_Occupier(); obj; obj = obj->Next) {
						RTTIType rtti = obj->RTTI;
						if (rtti == RTTI_UNIT || rtti == RTTI_INFANTRY || rtti == RTTI_AIRCRAFT) {
							((TechnoClass *)obj)->Sensed();
						}
					}
					BuildingClass * building = cptr->Cell_Building();
					if (building && building->House != PlayerPtr && building->Visual_Character(false, NULL)) {
						building->IsToDisplay = true;
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004DEAE0
int FootClass::IronCurtain(int duration, HouseClass * source, bool force_shield)
{
	if (Techno_Type_Class()->Organic) {
		int damage = Techno_Type_Class()->MaxStrength;
		return(Take_Damage(damage, 0, Rule->C4Warhead, NULL, false, false, NULL));
	}
	if (ParasiteEatingMe != NULL) {
		ParasiteEatingMe->ParasiteImUsing->SuppressionTimer = 50;
		ParasiteEatingMe->ParasiteImUsing->ExitUnit();
	}
	ParalysisTimer = 0;
	return(BASECLASS::IronCurtain(duration, source, force_shield));
}

extern const double DAT_007E3570;
extern const double DAT_007E9280;
extern const double DAT_007E3860;

// FUNCTION: GAMEMD 0x004DEBB0
bool FootClass::Crash(ObjectClass * killer)
{
	if (Get_Height_AGL() > 0) {
		if (Strength > 0) {
			if (killer && Tag != NULL) Tag->Spring(TEVENT_FIRST_DAMAGED, this);
			if (Tag != NULL && IsActive) Tag->Spring(TEVENT_FIRST_DAMAGED_ANY, this);

			if (Tag != NULL && IsActive && killer) {
				Tag->Spring(TEVENT_FIRST_DAMAGED_ANY, this, CELL_NONE, false, (TechnoClass *)killer);
			}

			if (!IsActive) {
				return(true);
			}
			RegisterDestruction((TechnoClass *)killer);
			if (!IsActive) {
				return(true);
			}
			if (!IsAttackedByLocomotor) {
				Strength = 0;
			}
		}

		IsCrashing = true;
		Transmit_Message(RADIO_OVER_OUT);
		Stun();
		Kill_Cargo((TechnoClass *)killer);

		if (RTTI != RTTI_INFANTRY && !ScenarioInit) {
			RockingSidewaysPerFrame = Scen->RandomNumber(0, 0x7FFFFFFE) * DAT_007E9280 * DAT_007E3570 + DAT_007E3860;
			if (Scen->RandomNumber(0, 1) == 0) {
				RockingSidewaysPerFrame = -RockingSidewaysPerFrame;
			}
			RockingForwardsPerFrame = DAT_007E3860 * (Scen->RandomNumber(0, 0x7FFFFFFE) * DAT_007E3570);
		}

		Detach_This_From_All(this, false);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004DED70
void const * FootClass::Get_Image_Data(void) const
{
	Cell cell = Center_Coord().As_Cell();
	CellClass * cptr = &Map[cell];

	if (!House->Is_Ally(PlayerPtr) && IsDisguised() && !cptr->Is_Disguise_Sensed(PlayerPtr->HeapID)) {
		return(GetDisguise(false)->ImageData);
	}
	if (!IsClearlyVisibleTo(PlayerPtr)) {
		return(GetDisguise(true)->ImageData);
	}
	return(BASECLASS::Get_Image_Data());
}

// FUNCTION: GAMEMD 0x004DEE50
BuildingClass * FootClass::FindNearestDockBuilding(BuildingTypeClass * type, unsigned unk2, unsigned unk3) const
{
	int distance = -1;
	return(FindCloserDockBuilding(type, unk2, unk3, &distance));
}

// FUNCTION: GAMEMD 0x004DEE80
BuildingClass * FootClass::FindCloserDockBuilding(BuildingTypeClass * type, unsigned unk2, unsigned unk3, int * distance) const
{
	BuildingClass * result = NULL;

	if (House->BQuantity.Value(type->HeapID)) {
		int count = House->Buildings.Count();
		for (int index = 0; index < count; index++) {
			BuildingClass * building = House->Buildings[index];
			if (building && !building->IsInLimbo && building->Class == type) {
				if (unk3 == 1 || building->HasFreeLink(this)) {
					if (RTTI != RTTI_AIRCRAFT) {
						Cell cell1 = building->Center_Coord().As_Cell();
						Cell cell2 = Destination_Coord().As_Cell();
						if (!Map.Is_Same_Cell_Zone(cell2, cell1, Techno_Type_Class()->MZone, ((ObjectClass *)this)->Is_Moving_Onto_Bridge(), false, false)) {
							continue;
						}
					}
					if (((FootClass *)this)->Transmit_Message(RADIO_CAN_LOAD, building) == RADIO_ROGER) {
						int dist = Relative_Distance(building);
						if (*distance == -1 || dist < *distance || building->IsLeader) {
							result = building;
							*distance = dist;
						}
					}
				}
			}
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004DF040
BuildingClass * FootClass::TryNearestDockBuilding(TypeList<BuildingTypeClass *> * list, unsigned unk2, unsigned unk3) const
{
	BuildingClass * result = NULL;
	int best = -1;
	for (int index = 0; index < list->Count(); index++) {
		int distance = -1;
		BuildingClass * building = FindCloserDockBuilding((*list)[index], unk2, unk3, &distance);
		if (building) {
			if (!result || distance < best || best == -1 || building->IsLeader) {
				result = building;
				best = distance;
			}
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004DF0D0
void FootClass::AbortMotion(void)
{
	unknown_5A0 = NULL;
	NavCom = NULL;
}

// FUNCTION: GAMEMD 0x004DF1A0
void FootClass::ClearMegaMissionData(void)
{
	MegaMission = MISSION_NONE;
	MegaDestination = NULL;
	MegaTarget = NULL;
	HasAttackMoveTarget = false;
}

// FUNCTION: GAMEMD 0x004DF1C0
bool FootClass::HaveMegaMission(void) const
{
	return(MegaMission != MISSION_NONE);
}

// FUNCTION: GAMEMD 0x004DF1D0
bool FootClass::HaveAttackMoveTarget(void) const
{
	return(HasAttackMoveTarget);
}

// FUNCTION: GAMEMD 0x004DF1E0
int FootClass::GetMegaMission(void) const
{
	return(MegaMission);
}

// FUNCTION: GAMEMD 0x004DF1F0
Coord FootClass::GetAttackMoveCoords(void)
{
	if (MegaDestination) {
		return(MegaDestination->Center_Coord());
	}
	if (MegaTarget) {
		return(MegaTarget->Center_Coord());
	}
	return(Coord(-1, -1, -1));
}

// FUNCTION: GAMEMD 0x004DF280
void FootClass::ResolveMegaTarget(void)
{
	if (MegaTarget) {
		Cell cell = Cell(MegaTarget->Center_Coord());
		MegaDestination = &Map[cell];
		MegaTarget = NULL;
	} else {
		MegaDestination = NULL;
		MegaTarget = NULL;
	}
}

// FUNCTION: GAMEMD 0x004DF310
bool FootClass::MegaMissionIsAttackMove(void) const
{
	return(MegaMission == MISSION_ATTACK_MOVE);
}

// FUNCTION: GAMEMD 0x004DF320
bool FootClass::ContinueMegaMission(void)
{
	bool result = true;
	HasAttackMoveTarget = false;
	if (MegaDestination) {
		QueueMission(MISSION_MOVE, true);
		Assign_Destination(MegaDestination, true);
	} else if (MegaTarget) {
		QueueMission(MISSION_ATTACK, true);
		Assign_Target(MegaTarget);
	} else {
		ClearMegaMissionData();
		result = false;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004DF3A0
void FootClass::UpdateAttackMove(void)
{
	if (MegaDestination) {
		if (HasAttackMoveTarget) {
			if (InAuxiliarySearchRange(TarCom)) {
				return;
			}
			TarCom = NULL;
		}
		if (Target_Something_Nearby(Coord(Position), THREAT_RANGE)) {
			QueueMission(MISSION_ATTACK, true);
			HasAttackMoveTarget = true;
		}
	} else {
		if (MegaTarget && InAuxiliarySearchRange(MegaTarget)) {
			return;
		}
		if (HasAttackMoveTarget && InAuxiliarySearchRange(TarCom)) {
			return;
		}
		TarCom = NULL;
		if (Target_Something_Nearby(Coord(Position), THREAT_RANGE)) {
			QueueMission(MISSION_ATTACK, true);
			HasAttackMoveTarget = true;
		} else {
			TarCom = MegaTarget;
		}
	}
}

// FUNCTION: GAMEMD 0x004DF4B0
bool FootClass::RefreshMegaMission(void)
{
	bool result = false;
	if (MegaMission == MISSION_ATTACK_MOVE) {
		if (HasAttackMoveTarget) {
			HasAttackMoveTarget = false;
			result = ContinueMegaMission();
		} else if (Get_Mission() == MISSION_GUARD || Get_Mission() == MISSION_GUARD) {
			result = ContinueMegaMission();
		} else {
			ClearMegaMissionData();
			result = false;
		}
	}
	return(result);
}

namespace LocomotionClass { namespace CLSIDs { extern const CLSID Teleport; } }
extern int DAT_008B3DF4;

// FUNCTION: GAMEMD 0x004DF7F0
bool FootClass::ChronoWarpTo(Coord dest)
{
	if (In_Air() || IsIronCurtained() || IsImmobilized || IsBeingWarpedOut() || IsWarpingIn()) {
		return(false);
	}
	ILocomotionPtr teleport(LocomotionClass::CLSIDs::Teleport);
	IPiggybackPtr piggy(teleport);
	if (Map[dest].IsUnderBridge) {
		dest.Z += DAT_008B3DF4;
	}
	if (RTTI == RTTI_UNIT) {
		Locomotion->Mark_All_Occupation_Bits(0);
		Locomotion->Force_Track(-1, dest);
		Clear_Occupy_Bit(dest);
		IsOccupyingCell = true;
	}
	Assign_Destination(&Map[dest], true);
	Assign_Destination(NULL, true);
	teleport->Link_To_Object(this);
	piggy->Begin_Piggyback(Locomotion);
	Locomotion = teleport;
	IsImmobilized = true;
	(Coord &)ChronoDestCoords = dest;
	Assign_Destination(&Map[dest], true);
	return(true);
}

// FUNCTION: GAMEMD 0x004DFA50
bool FootClass::CanBeSelected(void) const
{
	if (IsAttackedByLocomotor) {
		return(false);
	}
	return(BASECLASS::CanBeSelected());
}

// FUNCTION: GAMEMD 0x004DFA70
bool FootClass::EnterGrinder(void)
{
	BuildingClass * best = NULL;
	int bestdist = 0x7FFFFFFF;
	for (int index = House->Grinders.Count() - 1; index >= 0; index--) {
		BuildingClass * building = House->Grinders[index];
		int dist = (Center_Coord() - building->Center_Coord()).Length();
		if (dist < bestdist) {
			bestdist = dist;
			best = building;
		}
	}
	if (best == NULL) {
		return(false);
	}
	Assign_Destination(best, true);
	QueueMission(MISSION_EATEN, false);
	return(true);
}

// FUNCTION: GAMEMD 0x004DFB70
bool FootClass::EnterBioReactor(void)
{
	BuildingClass * best = NULL;
	int bestdist = 0x7FFFFFFF;
	for (int index = House->Absorbers.Count() - 1; index >= 0; index--) {
		BuildingClass * building = House->Absorbers[index];
		if (Transmit_Message(RADIO_CAN_LOAD, building) == RADIO_ROGER) {
			int dist = (Center_Coord() - building->Center_Coord()).Length();
			if (dist < bestdist) {
				bestdist = dist;
				best = building;
			}
		}
	}
	if (best == NULL) {
		ShouldEnterAbsorber = false;
		return(false);
	}
	ShouldEnterAbsorber = true;
	if (FUN_0065AD30(0) != best && QueueUpToEnter != best) {
		QueueMission(MISSION_ENTER, false);
		Assign_Destination(best, true);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004DFCB0
bool FootClass::EnterBattleBunker(void)
{
	BuildingClass * best = NULL;
	int bestdist = 0x7FFFFFFF;
	for (int index = House->Occupiables.Count() - 1; index >= 0; index--) {
		BuildingClass * building = House->Occupiables[index];
		Coord diff = Center_Coord() - building->Center_Coord();
		int dist = int(fastmath::sqrt((double)diff.X*(double)diff.X + (double)diff.Y*(double)diff.Y + (double)diff.Z*(double)diff.Z));
		if (dist < bestdist && building->CanBeOccupiedBy((this == NULL) ? NULL : (RTTI == RTTI_INFANTRY) ? (InfantryClass *)this : NULL)) {
			bestdist = dist;
			best = building;
		}
	}
	if (best == NULL) {
		ShouldEnterOccupiable = false;
		return(false);
	}
	ShouldEnterOccupiable = true;
	if (NavCom != best || CurrentMission != MISSION_CAPTURE) {
		Assign_Destination(best, true);
		QueueMission(MISSION_CAPTURE, true);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004DFE00
bool FootClass::GarrisonStructure(void)
{
	BuildingClass * best = NULL;
	int bestdist = 0x7FFFFFFF;
	for (int index = ::Buildings.Count() - 1; index >= 0; index--) {
		BuildingClass * building = ::Buildings[index];
		if (building->CanBeOccupiedBy((this == NULL) ? NULL : (RTTI == RTTI_INFANTRY) ? (InfantryClass *)this : NULL)) {
			int dist = (Center_Coord() - building->Center_Coord()).Length();
			if (dist < bestdist) {
				bestdist = dist;
				best = building;
			}
		}
	}
	if (best == NULL) {
		ShouldGarrisonStructure = false;
		return(false);
	}
	ShouldGarrisonStructure = true;
	if (NavCom != best || CurrentMission != MISSION_CAPTURE) {
		Assign_Destination(best, true);
		QueueMission(MISSION_CAPTURE, true);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004DFF40
bool FootClass::EnterTankBunker(void)
{
	if (!FUN_0070FB50()) {
		return(false);
	}
	BuildingClass * best = NULL;
	int bestdist = 0x7FFFFFFF;
	for (int index = House->Bunkers.Count() - 1; index >= 0; index--) {
		BuildingClass * building = House->Bunkers[index];
		int dist = (Center_Coord() - building->Center_Coord()).Length();
		if (dist < bestdist && !building->FUN_0065AD30(0) && !building->BunkerLinkedItem) {
			bestdist = dist;
			best = building;
		}
	}
	if (best == NULL) {
		return(false);
	}
	Assign_Destination(best, true);
	Transmit_Message(RADIO_HELLO, best);
	QueueMission(MISSION_ENTER, true);
	return(true);
}

// FUNCTION: GAMEMD 0x004E0080
bool FootClass::Is_Entering_Structure(void) const
{
	return(ShouldEnterAbsorber || ShouldEnterOccupiable || ShouldGarrisonStructure);
}

// FUNCTION: GAMEMD 0x004E00B0
void FootClass::Jumpjet_OccupyCell(Cell cell)
{
	if (CurrentJumpjetMapCoords != CELL_NONE && cell != CELL_NONE) {
		if (Map[CurrentJumpjetMapCoords].Jumpjet == this) {
			Map[CurrentJumpjetMapCoords].Jumpjet = NULL;
		}
	}
	CurrentJumpjetMapCoords = cell;
}

// FUNCTION: GAMEMD 0x004E0130
// ObjectClass::Class_Of

// FUNCTION: GAMEMD 0x004E0140
bool MissionClass::Ready_To_Next_Mission(void) const
{
	return(true);
}

// FUNCTION: GAMEMD 0x004E0150
// TechnoClass::Turret_Facing

// SYNTHETIC: GAMEMD 0x004E0170
// ??_GFootClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004E0190
// ?Clear@?$DynamicVectorClass@PAVAbstractClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E01C0
// ?ID@?$DynamicVectorClass@PAVAbstractClass@@@@UAEHPBQAVAbstractClass@@@Z
// TEMPLATE: GAMEMD 0x004E01E0
// ?GetItem@?$VectorClass@PAVAbstractClass@@@@UBEPAVAbstractClass@@H@Z
// TEMPLATE: GAMEMD 0x004E04A0
// ?Resize@?$DynamicVectorClass@PAVAbstractClass@@@@UAE_NHPBQAVAbstractClass@@@Z
// TEMPLATE: GAMEMD 0x004E0550
// ?ID@?$DynamicVectorClass@PAVAbstractClass@@@@UAEHABQAVAbstractClass@@@Z
template class DynamicVectorClass<AbstractClass *>;

// TEMPLATE: GAMEMD 0x004E01F0
// ?Clear@?$DynamicVectorClass@PAVFootClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E0220
// ?ID@?$DynamicVectorClass@PAVFootClass@@@@UAEHPBQAVFootClass@@@Z
// TEMPLATE: GAMEMD 0x004E0240
// ?GetItem@?$VectorClass@PAVFootClass@@@@UBEPAVFootClass@@H@Z
// TEMPLATE: GAMEMD 0x004E0700
// ?Resize@?$DynamicVectorClass@PAVFootClass@@@@UAE_NHPBQAVFootClass@@@Z
// TEMPLATE: GAMEMD 0x004E07B0
// ?ID@?$DynamicVectorClass@PAVFootClass@@@@UAEHABQAVFootClass@@@Z
template class DynamicVectorClass<FootClass *>;

typedef DiscreteDistributionClass<CellClass>::DIST_OBJ CELL_DIST_OBJ;

// TEMPLATE: GAMEMD 0x004E0250
// ?GetItem@?$VectorClass@V?$DistributionObject@PAVCellClass@@@?$DiscreteDistributionClass@VCellClass@@@@@@UBE?AV?$DistributionObject@PAVCellClass@@@?$DiscreteDistributionClass@VCellClass@@@@H@Z
template class DynamicVectorClass<CELL_DIST_OBJ>;

// SYNTHETIC: GAMEMD 0x004E0270
// ??_G?$DynamicVectorClass@PAVAbstractClass@@@@UAEPAXI@Z


// SYNTHETIC: GAMEMD 0x004E02D0
// ??_G?$VectorClass@PAVAbstractClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004E0320
// ??8?$VectorClass@PAVAbstractClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004E0360
// ?Resize@?$VectorClass@PAVAbstractClass@@@@UAE_NHPBQAVAbstractClass@@@Z
// TEMPLATE: GAMEMD 0x004E0410
// ?Clear@?$VectorClass@PAVAbstractClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E0440
// ?ID@?$VectorClass@PAVAbstractClass@@@@UAEHABQAVAbstractClass@@@Z
// TEMPLATE: GAMEMD 0x004E0480
// ?ID@?$VectorClass@PAVAbstractClass@@@@UAEHPBQAVAbstractClass@@@Z
template class VectorClass<AbstractClass *>;

// TEMPLATE: GAMEMD 0x004E0580
// ??8?$VectorClass@PAVFootClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x004E05C0
// ?Resize@?$VectorClass@PAVFootClass@@@@UAE_NHPBQAVFootClass@@@Z
// TEMPLATE: GAMEMD 0x004E0670
// ?Clear@?$VectorClass@PAVFootClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x004E06A0
// ?ID@?$VectorClass@PAVFootClass@@@@UAEHABQAVFootClass@@@Z
// TEMPLATE: GAMEMD 0x004E06E0
// ?ID@?$VectorClass@PAVFootClass@@@@UAEHPBQAVFootClass@@@Z
template class VectorClass<FootClass *>;
