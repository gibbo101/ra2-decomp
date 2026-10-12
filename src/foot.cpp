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

// FUNCTION: 0x004D4FC0
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

// FUNCTION: 0x004D5070
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

// FUNCTION: 0x004D5350
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

// FUNCTION: 0x004D55C0
bool FootClass::Stop_Driver(void)
{
	Locomotion->Stop_Moving();
	return(false);
}

// FUNCTION: 0x004D55F0
bool FootClass::Start_Driver(Coord & headto)
{
	Locomotion->Move_To(headto);
	return(Locomotion->Is_Moving() != 0);
}

// FUNCTION: 0x004D5660
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

// FUNCTION: 0x004D7170
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

// FUNCTION: 0x004D7330
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

// FUNCTION: 0x004D7D50
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

// FUNCTION: 0x004D82B0
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

// FUNCTION: 0x004D8560
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

// FUNCTION: 0x004D8F40
void FootClass::Override_Mission(MissionType mission, AbstractClass * tarcom, AbstractClass * navcom)
{
	SuspendedNavCom = NavCom;
	BASECLASS::Override_Mission(mission, tarcom, navcom);

	Assign_Destination(navcom);
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
