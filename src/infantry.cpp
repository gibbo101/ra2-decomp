// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "infantry.h"
#include "_bench.h"
#include "_convert.h"
#include "_mixfile.h"
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
#include "combat.h"
#include "data.h"
#include "draw.h"
#include "fly.h"
#include "fog.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "ilocos.h"
#include "incdec.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "isotype.h"
#include "language\language.h"
#include "lightcon.h"
#include "mixfile.h"
#include "mono.h"
#include "overlay.h"
#include "overtype.h"
#include "radsite.h"
#include "rules.h"
#include "slavemgr.h"
#include "scheme.h"
#include "session.h"
#include "stringtable.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "temporal.h"
#include "team.h"
#include "tiberium.h"
#include "tracker.h"
#include "tube.h"
#include "tube.hh"
#include "unit.h"
#include "vox.h"
#include "warhead.h"
#include "weapon.h"
#include "bench.hh"

extern int __fastcall FUN_0050C170(char const * name);

// FUNCTION: 0x00517A50
InfantryClass::InfantryClass(InfantryTypeClass const * type, HouseClass * house) :
	BASECLASS(house),
	Class((InfantryTypeClass *)type),
	Doing(DO_NOTHING),
	Comment(0),
	IsTechnician(false),
	IsStoked(false),
	IsProne(false),
	IsBerzerk(false),
	IsZoneCheat(false),
	WasSelected(false),
	Fear(FEAR_NONE),
	unknown_6E0(0),
	ShouldDeploy(false),
	unknown_int_6E8(2)
{
	Create_ID();
	Infantry.Add(this);

	Init();

	if (Class != NULL) {
		Locomotion.CreateInstance(Class->Locomotor, NULL, CLSCTX_ALL);
		Locomotion->Link_To_Object(this);
	}

	PrimaryFacing.Set_ROT(127);
	TargetTracker.Add_Index(Fetch_ID(), this);
}

// FUNCTION: 0x00517CC0
void InfantryClass::Init(void)
{
	BASECLASS::Init();

	if (House != NULL) {
		House->Tracking_Add(this);
	}

	if (Class != NULL) {
		if (House != NULL) {
			TypeList<InfantryTypeClass *> & list = House->Class->VeteranInfantry;
			for (int index = 0; index < list.Count(); index++) {
				if (list[index] == Class) {
					Veterancy.Set_Veteran(true);
					break;
				}
			}
		}

		if (House != NULL && House->BarracksInfiltrated && Class->IsTrainable) {
			Veterancy.Set_Veteran(true);
		}

		Strength = Class->MaxStrength;
		EstimatedHealth = Class->MaxStrength;
		if (Class->InitialAmmo == -1) {
			Ammo = Class->MaxAmmo;
		} else {
			Ammo = Class->InitialAmmo;
		}
		IsCloakable = Class->IsCloakable;
	}
}

// FUNCTION: 0x00517D90
InfantryClass::~InfantryClass(void)
{
	if (GameActive && Class != NULL) {
		if (House->Can_Build(Class, false, false) == -1) {
			House->IsRecalcNeeded = true;
		}

		if (Team != NULL) {
			Team->Remove(this);
			Team = NULL;
		}

		if (SlaveOwner != NULL && SlaveOwner->SlaveManager != NULL) {
			SlaveOwner->SlaveManager->LostSlave(this);
		}

		House->Tracking_Remove(this);
		Locomotion->Stop_Movement_Animation();
		unknown_int_6E8 = 2;
		IsProne = false;
		Doing = DO_STAND_READY;
		FootClass::Limbo();
	}
	Detach_This_From_All(this, true);
	Infantry.Delete(this);
	TargetTracker.Remove_Index(Fetch_ID());
	IsActive = false;
}

// FUNCTION: 0x00522100
bool InfantryClass::Is_JumpJet(void) const
{
	if (!Class->JumpJet) {
		return(false);
	}

	IPersistPtr persist(Locomotion);
	CLSID clsid;
	persist->GetClassID(&clsid);
	return((clsid == CLSID_JumpjetLocomotion) ? true : false);
}

// FUNCTION: 0x00518D80
int InfantryClass::Shape_Number(void) const
{
	DoType doit = Doing;
	if (doit == DO_NOTHING) {
		if (Map[Get_Cell()].Land_Type() == LAND_WATER && !IsOnBridge) {
			doit = (DoType)0x10;
		} else {
			doit = DO_STAND_READY;
		}
	}

	InfantryTypeClass const * type = Class;
	if (!IsClearlyVisibleTo(PlayerPtr)) {
		type = (InfantryTypeClass const *)GetDisguise(true);
	}

	int shapenum = Animation.Fetch_Stage() % MAX(type->DoControls[doit].Count, 1);

	if (Is_JumpJet() && TarCom != NULL && !Class->JumpJetTurn) {
		if (type->DoControls[doit].Jump > 0) {
			shapenum += HumanShape[GetTargetDirection(TarCom).As_Dir32()] * type->DoControls[doit].Jump;
		}
	} else {
		if (type->DoControls[doit].Jump > 0) {
			shapenum += HumanShape[PrimaryFacing.Current().As_Dir32()] * type->DoControls[doit].Jump;
		}
	}

	shapenum += type->DoControls[doit].Frame;
	return(shapenum);
}

// FUNCTION: 0x0051AA10
void InfantryClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);
	if (Class == target) {
		Class = NULL;
	}
}

static inline bool Edge_Of_World(InfantryClass * infantry)
{
	if (infantry->Team != NULL && infantry->IsInPlayfield) {
		infantry->Team->IsLeaveMap = true;
	}

	if (infantry->Team == NULL && infantry->Get_Mission() == MISSION_GUARD && !Map.In_Radar(infantry->PositionCoord)) {
		infantry->Stun();
		infantry->Delete_Me();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0051B1F0
void InfantryClass::Assign_Target(AbstractClass * target)
{
	if (target != TarCom) {
		if (Strength > 0) {
			IsFiring = false;
			if (Doing == 0x1b) goto deploy_fire;
			if (Doing == 0x1c) goto deploy_fire;
			if (Doing == 0x1d) goto deploy_fire;
			if (Doing == 0x1e) {
deploy_fire:
				Do_Action((DoType)0x1c);
			} else if (IsProne) {
				Do_Action(DO_PRONE);
			} else {
				Do_Action(DO_STAND_READY);
			}
		}

		if (target != TarCom) {
			if (DirectRockerLinkedUnit != NULL) {
				DirectRockerLinkedUnit->DirectRockerLinkedUnit = NULL;
				DirectRockerLinkedUnit = NULL;
			}
		}
	}

	if ((Doing == 0x1b || Doing == 0x1c || Doing == 0x1d || Doing == 0x1e) && !Class->DeployFire) {
		return;
	}

	Path[0] = FACING_NONE;
	BASECLASS::Assign_Target(target);

	if (NavCom == NULL && Class->IsCapture) {
		BuildingClass * building = (target != NULL && target->RTTI == RTTI_BUILDING) ? (BuildingClass *)target : NULL;

		if (!Class->IsAgent && building != NULL && building->Class->IsCaptureable) {
			Assign_Destination(target, true);
			unknown_bool_68E = false;
			return;
		}
		if (Class->IsAgent && building != NULL && building->Class->Spyable) {
			Assign_Destination(target, true);
		}
	}

	unknown_bool_68E = false;
}

// FUNCTION: 0x0051BAB0
void InfantryClass::AI(void)
{
	if (CurrentTube >= 0) {
		Tunnel_AI();
		Update_Radar_Position(false);
		return;
	}

	if (IsBeingWarpedOut() || IsWarpingIn()) {
		if (Frame % 24 == 0 && Rule->ChronoSparkle1 != NULL) {
			new AnimClass(Rule->ChronoSparkle1, PositionCoord + Coord(0x78, 0x78, 0), 0, 1, (ShapeFlags_Type)0x600, 0, false);
		}
	}

	if (TemporalTargetingMe != NULL) {
		TemporalTargetingMe->AI();
	}

	if (IsWarpingIn() || (IsBeingWarpedOut() && IsImmobilized)) {
		Locomotion->Process();
		if (!IsActive) {
			return;
		}
	}

	if (IsBeingWarpedOut()) {
		if (TarCom != NULL) {
			Assign_Target(NULL);
		}
		if (NavCom != NULL) {
			Assign_Destination(NULL, true);
		}
		return;
	}

	if (Ready_To_Next_Mission()) {
		if (Get_Mission() == MISSION_NONE && MissionQueue == MISSION_NONE) {
			Enter_Idle_Mode();
		}
		Commence();
	}

	if (Strength <= 0) {
		if (Doing != 0xb && Doing != 0xc && Doing != 0xd && Doing != 0xe && Doing != 0xf && Doing != 0x22 && Doing != 0x23 && Doing != 0x24 && Doing != 0x14 && Doing != 0x15) {
			Strength = 1;
		}
	}

	BASECLASS::AI();

	if (!IsActive) {
		return;
	}

	if (!IsInLimbo && (Get_Mission() == MISSION_GUARD || Get_Mission() == MISSION_GUARD_AREA)) {
		BuildingClass * building = Map[(Coord const &)PositionCoord].Cell_Building();
		if (building != NULL) {
			if (building->Class->IsInvisibleInGame) {
				building = NULL;
			} else if (building->Class->IsLaserFence) {
				if (building->LaserFenceFrame == 12 || building->LaserFenceFrame == 8) {
					building = NULL;
				}
			} else if (building->Class->IsFirestormWall) {
				if (!building->House->FirestormDefenseActivated) {
					building = NULL;
				}
			} else if (building->Class->IsGate) {
				if (building->Is_Gate_Open()) {
					building = NULL;
				}
			}

			if (SlaveOwner != NULL && SlaveOwner->SlaveManager != NULL && (TechnoClass *)building == SlaveOwner) {
				if (SlaveOwner->SlaveManager->FUN_006B0880(this, Get_Cell_Ptr())) {
					goto no_scatter;
				}
			}

			if (building != NULL) {
				Scatter(COORD_NONE, true, true);
			}
		}
	}
no_scatter:

	if (In_Which_Layer() != LAYER_GROUND) {
		Mark(MARK_CHANGE);
	}

	if (IsFiring && Animation.Fetch_Rate() == 0) {
		IsFiring = false;
		if (Doing == 0x1b) goto deploy_fire;
		if (Doing == 0x1c) goto deploy_fire;
		if (Doing == 0x1d) goto deploy_fire;
		if (Doing == 0x1e) {
deploy_fire:
			Do_Action((DoType)0x1c);
		} else {
			Do_Action(DO_STAND_READY);
		}
	}

	if (Edge_Of_World(this)) {
		return;
	}

	if (Theft_AI()) {
		return;
	}

	if (Ready_To_Next_Mission()) {
		if (Get_Mission() == MISSION_NONE && MissionQueue == MISSION_NONE) {
			Enter_Idle_Mode();
		}
		Commence();
	}

	Fear_AI();

	if (NavCom == NULL && !IsProne && IsStoked && Comment == 0) {
		IsStoked = false;
	}

	Firing_AI();

	if (!IsActive) {
		return;
	}

	Doing_AI();

	if (!IsActive) {
		return;
	}

	Movement_AI();
}

// FUNCTION: 0x0051CBA0
bool InfantryClass::Enter_Idle_Mode(bool initial, bool resume_waypoint)
{
	bool result = BASECLASS::Enter_Idle_Mode(initial, resume_waypoint);

	if (HaveMegaMission()) {
		return(result);
	}

	if (Get_Mission() == MISSION_WAIT) {
		return(result);
	}

	MissionType order;

	if (TarCom != NULL) {
		order = MISSION_ATTACK;
		if (Get_Mission() == MISSION_SABOTAGE) {
			order = MISSION_SABOTAGE;
		}
		if (Get_Mission() == MISSION_CAPTURE) {
			order = MISSION_CAPTURE;
		}
	} else {
		Handle_Navigation_List();

		if (NavCom != NULL) {
			order = MISSION_MOVE;
			if (Get_Mission() == MISSION_CAPTURE) {
				order = MISSION_CAPTURE;
			}
			if (Get_Mission() == MISSION_SABOTAGE) {
				order = MISSION_SABOTAGE;
			}
		} else {
			if (Get_Mission() == MISSION_GUARD || Get_Mission() == MISSION_GUARD_AREA || Get_Mission() != MISSION_NONE && (Current_Mission_Control().IsZombie || Current_Mission_Control().IsParalyzed)) {
				return(false);
			}

			if (House->Is_Human_Player() || Team != NULL) {
				if (CurrentMission == MISSION_GUARD_AREA || ((Has_Ability((AbilityType)0x10) || Class->DefaultToGuardArea) && Team == NULL)) {
					order = MISSION_GUARD_AREA;
				} else {
					order = MISSION_GUARD;
				}
			} else {
				if (House->IQ < Rule->IQGuardArea && CurrentMission != MISSION_GUARD_AREA) {
					order = MISSION_GUARD;
				} else if (SlaveOwner != NULL) {
					order = MISSION_GUARD;
				} else if (Is_Weapon_Equipped()) {
					order = MISSION_GUARD_AREA;
				} else if (Class->IsEngineer || Class->IsVehicleThief) {
					order = MISSION_GUARD_AREA;
				} else {
					order = MISSION_GUARD;
				}
			}
		}
	}

	if (CurrentMission != MISSION_PATROL && CurrentMission != MISSION_GUARD_AREA && order != MISSION_NONE) {
		QueueMission(order, false);
	}
	return(result);
}

// FUNCTION: 0x0051CDB0
bool InfantryClass::Random_Animate(void)
{
	if (Is_Ready_To_Random_Animate()) {
		IdleTimer = (int)Random_Double(Rule->RandomAnimateTime * (TICKS_PER_MINUTE/2), Rule->RandomAnimateTime * (TICKS_PER_MINUTE*2));

		if (Class->IsFraidyCat && !House->Is_Human_Player() && Fear > 0x32) {
			Scatter(COORD_NONE, true);
			return(true);
		}

		int pick = Random_Pick(0, 10);
		bool cow = Is_Type_Named("COW");
		if (cow && Random_Pick(0, 10) < 5) {
			pick = 8;
		}

		switch (pick) {
			case 0:
				break;

			case 3:
			case 4:
			case 5:
				Do_Action(DO_IDLE1);
				break;

			case 6:
				PrimaryFacing.Set(DirType(DirType(Facing_Dir(Random_Pick(FACING_N, FACING_NW))).As_Dir256()));
				break;

			case 1:
			case 2:
			case 7:
				Do_Action(DO_IDLE2);
				if (!IsSelected && House->Is_Player_Control() && Class->VoiceComment.Count() > 0 && Sim_Random_Pick(0, 2) == 0) {
					VocClass::PlayAt(Class->VoiceComment[0], PositionCoord);
				}
				break;

			case 8:
				PrimaryFacing.Set(DirType(DirType(Facing_Dir(Random_Pick(FACING_N, FACING_NW))).As_Dir256()));
				if (cow || (!House->Is_Human_Player() && Class->IsFraidyCat)) {
					Scatter(COORD_NONE, true);
				}
				break;

			case 9:
			case 10:
				PrimaryFacing.Set(DirType(DirType(Facing_Dir(Random_Pick(FACING_N, FACING_NW))).As_Dir256()));
				break;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0051D6F0
bool InfantryClass::Do_Action(DoType todo, bool force, bool randomize)
{
	if (todo == DO_NOTHING || Class->DoControls[todo].Count == 0) {
		return(false);
	}

	if (Doing == 0x21 && IsFalling) {
		return(false);
	}

	if (SlaveOwner != NULL && TClass->Capacity != 0 && Tiberium_Load() == 1.0 && todo == 3) {
		todo = (DoType)0x27;
	} else if (todo == 5) {
		if (!Class->IsCrawling) {
			return(false);
		}
	}

	if (Class->MZone == 3) {
		LandType land = Map[Get_Cell()].Land_Type();
		int surface = 1;
		if ((land == LAND_BEACH || land == LAND_WATER) && !IsOnBridge) {
			surface = 0;
			if (todo == 3 || todo == 6) {
				todo = (DoType)0x11;
			} else if (todo == 2 || todo == 0) {
				todo = (DoType)0x10;
			} else if (todo == 9) {
				todo = (DoType)0x12;
			} else if (todo == 10) {
				todo = (DoType)0x13;
			} else if (todo == 11) {
				todo = (DoType)0x14;
			} else if (todo == 12) {
				todo = (DoType)0x15;
			} else if (todo == 4 || todo == 8) {
				todo = (DoType)0x16;
			}
		}

		if (unknown_int_6E8 == 0 && surface == 1) {
			VocClass::PlayAt(Class->LeaveWaterSound, PositionCoord, NULL);
		} else if (unknown_int_6E8 == 1 && surface == 0) {
			VocClass::PlayAt(Class->EnterWaterSound, PositionCoord, NULL);
		}
		unknown_int_6E8 = surface;
	}

	if (In_Air() && !IsOnBridge && Class->DoControls[0x17].Frame > 0 && todo == 0) {
		todo = (DoType)0x17;
	} else if (todo == 3 && Fear >= 200) {
		todo = (DoType)0x25;
	}

	if (todo == Doing) {
		return(false);
	}

	if (Doing != DO_NOTHING && !force && !MasterDoControls[Doing].Interrupt) {
		return(false);
	}

	if (todo == 0x1b) {
		if (TClass->DeploySound != -1) {
			VocClass::PlayAt(TClass->DeploySound, PositionCoord, NULL);
		}
	} else if (todo == 0x1f) {
		if (TClass->UndeploySound != -1) {
			VocClass::PlayAt(TClass->UndeploySound, PositionCoord, NULL);
		}
	}

	Doing = todo;
	if (todo == 9) goto idle_rate;
	if (todo == 10) goto idle_rate;
	if (todo == 0x12) goto idle_rate;
	if (todo == 0x13) goto idle_rate;
	if (todo == 0x17) goto idle_rate;
	if (todo == 0x20) {
idle_rate:
		Animation.Set_Rate(Options.Normalize_Delay(MasterDoControls[todo].Rate));
	} else {
		Animation.Set_Rate(MasterDoControls[todo].Rate);
	}

	if (randomize) {
		Animation.Set_Stage(Random_Pick(0, MAX((int)Class->DoControls[todo].Count, 1) - 1));
	} else {
		Animation.Set_Stage(0);
	}

	if (Strength == 0) {
		Stop_Driver();
	}

	switch (todo) {
		case 5:
			IsProne = true;
			break;

		case 7:
		case 0x1b:
			IsProne = false;
			break;

		default:
			break;
	}

	return(true);
}

// FUNCTION: 0x0051DAF0
bool InfantryClass::Stop_Driver(void)
{
	if (Doing == 0x1b) goto deploy_fire;
	if (Doing == 0x1c) goto deploy_fire;
	if (Doing == 0x1d) goto deploy_fire;
	if (Doing == 0x1e) {
deploy_fire:
		Do_Action((DoType)0x1c);
	} else if (IsProne) {
		Do_Action(DO_PRONE);
	} else {
		Do_Action(DO_STAND_READY);
	}

	if (Can_Enter_Cell(&Map[(Coord const &)PositionCoord], PrimaryFacing.Current().As_Dir8(), Get_Cell_Height()) == MOVE_OK) {
		IsZoneCheat = false;
	} else {
		IsZoneCheat = true;
	}

	return(BASECLASS::Stop_Driver());
}

// FUNCTION: 0x0051DBD0
bool InfantryClass::Start_Driver(Coord & headto)
{
	Coord old = headto;

	headto = Map[headto].Closest_Free_Spot(Move_Coord(headto, DirType().Direction(Center_Coord(), headto) + ((DIR_S << 8) - 1), CELL_LEPTON / 2 - 4), false, Map[headto].IsUnderBridge, false);
	if (headto == COORD_NONE && Can_Enter_Cell(&Map[old]) == MOVE_OK) {
		headto = Map[old].Closest_Free_Spot(Move_Coord(old, DirType().Direction(Center_Coord(), headto) + ((DIR_S << 8) - 1), CELL_LEPTON / 2), true, Map[old].IsUnderBridge, false);
	}

	if (headto != COORD_NONE && BASECLASS::Start_Driver(headto)) {
		if (!IsActive) return(false);
		Clear_Occupy_Bit(Coord(Position));
		Set_Occupy_Bit(headto);
		return(true);
	}

	return(false);
}

// FUNCTION: 0x0051DF10
bool InfantryClass::Limbo(void)
{
	Locomotion->Stop_Movement_Animation();
	unknown_int_6E8 = 2;
	IsProne = false;
	Doing = DO_STAND_READY;
	return(FootClass::Limbo());
}

// FUNCTION: 0x0051DF60
BulletClass * InfantryClass::Fire_At(AbstractClass * target, int which)
{
	IsFiring = false;

	BulletClass * bullet = BASECLASS::Fire_At(target, which);
	if (bullet != NULL && !IsInLimbo) {
		if (Class->IsFraidyCat && !Ammo) {
			Fear = (FearType)0x12c;
			if (Get_Mission() == MISSION_ATTACK || Get_Mission() == MISSION_HUNT) {
				QueueMission(MISSION_GUARD, false);
			}
		}
	}
	return(bullet);
}

// FUNCTION: 0x0051DFF0
bool InfantryClass::Unlimbo(Coord const & xcoord, Dir256 facing)
{
	int height = Map.Get_Height_GL(xcoord);
	Coord coord = xcoord;

	if (coord.Z == height) {
		coord = Map[xcoord].Closest_Free_Spot(xcoord, ScenarioInit || !Map.In_Local_Radar(xcoord.As_Cell()), false, false);
		if (coord == COORD_NONE) {
			return(false);
		}
		coord.Z = height;
	}

	if (BASECLASS::Unlimbo(coord, facing)) {
		if (Class->SightRange == 0) {
			IsDiscoveredByPlayer = false;
		}

		if (coord.Z <= height + BRIDGE_LEPTON_HEIGHT) {
			Set_Occupy_Bit(coord);
		}

		unknown_int_6E8 = 2;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0051E140
AbstractClass * InfantryClass::Greatest_Threat(ThreatType threat, Coord const & coord, bool onlyenemy) const
{
	if (!House->Is_Human_Player() && Class->IsEngineer) {
		if (House->ToCapture != NULL && Distance_To(House->ToCapture) < 15 * CELL_LEPTON) {
			return(House->ToCapture);
		}
		threat = ThreatType(threat | THREAT_CAPTURE);
	}

	if (Class->IsVehicleThief) {
		if (NavCom != NULL && NavCom->Is_Techno() && ((TechnoClass *)NavCom)->IsStrange() && !((TechnoClass *)NavCom)->Techno_Type_Class()->IsTrain && Distance(NavCom) < 15 * CELL_LEPTON) {
			return(NavCom);
		}
	}

	if (!Is_Weapon_Equipped()) {
		if (!Class->IsCapture && !Class->IsVehicleThief) {
			return(NULL);
		}
		if (Class->IsVehicleThief) {
			threat = ThreatType((threat & ~(THREAT_BUILDINGS|THREAT_VEHICLES|THREAT_BOATS|THREAT_AIR)) | THREAT_VEHICLES);
		}
	}

	if ((threat & (THREAT_INFANTRY|THREAT_VEHICLES|THREAT_BUILDINGS|THREAT_TIBERIUM|THREAT_CIVILIANS|THREAT_POWER|THREAT_FACTORIES|THREAT_BASE_DEFENSE|0x8000|0x10000)) == 0) {
		if (Get_Class_Weapon_Data(0)->Weapon != NULL) {
			threat = ThreatType(threat | Get_Class_Weapon_Data(0)->Weapon->Allowed_Threats());
		}
		if (Get_Class_Weapon_Data(1)->Weapon != NULL) {
			threat = ThreatType(threat | Get_Class_Weapon_Data(1)->Weapon->Allowed_Threats());
		}
	}

	if (Is_Weapon_Equipped() && Get_Class_Weapon_Data(0)->Weapon->WarheadPtr->IsOrganic) {
		threat = ThreatType(threat & ~(THREAT_BUILDINGS|THREAT_VEHICLES|THREAT_BOATS|THREAT_AIR));
	}

	if ((Class->IsBomber || Has_Ability(ABILITY_C4)) && !House->Is_Human_Player()) {
		threat = ThreatType(threat | THREAT_BUILDINGS);
	}

	if (Class->IsThief) {
		threat = ThreatType(threat | THREAT_CAPTURE | THREAT_TIBERIUM);
	}
	return(BASECLASS::Greatest_Threat(threat, coord, onlyenemy));
}

// FUNCTION: 0x0051F190
bool InfantryClass::Active_Click_With(ActionType action, ObjectClass * object, bool is_waypoint)
{
	action = What_Action(object, is_waypoint);

	switch (action) {
		case ACTION_GREPAIR:
		case ACTION_DAMAGE:
		case ACTION_CAPTURE:
			action = ACTION_CAPTURE;
			break;

		case ACTION_HEAL:
		case 57:
			action = ACTION_ATTACK;
			break;

		case ACTION_SABOTAGE:
		case ACTION_ATTACK:
		case ACTION_GUARD_AREA:
		case ACTION_MOVE:
			action = action;
			break;

		default:
			break;
	}

	if (object == this && action == ACTION_ATTACK) {
		return(false);
	}
	if (TechnoClass::Berzerk) {
		return(false);
	}
	return(BASECLASS::Active_Click_With(action, object, is_waypoint));
}

// FUNCTION: 0x0051F250
bool InfantryClass::Active_Click_With(ActionType action, Cell const & cell, Cell const & cell2, bool is_waypoint)
{
	if (Doing == 0x1b) goto deploy_fire;
	if (Doing == 0x1c) goto deploy_fire;
	if (Doing == 0x1d) goto deploy_fire;
	if (Doing == 0x1e) {
deploy_fire:
		if (What_Action(cell, false, is_waypoint) == ACTION_NOMOVE) {
			return(false);
		}
	}
	if (TechnoClass::Berzerk) {
		return(false);
	}
	return(BASECLASS::Active_Click_With(action, cell, cell2, is_waypoint));
}

// FUNCTION: 0x0051F2C0
wchar_t const * InfantryClass::Full_Name(void) const
{
	if (IsTechnician) {
#line 4636 "D:\\ra2mdpost\\Infantry.CPP"
		return(StringTable::LoadString("TXT_TECHNICIAN", NULL, __FILE__, __LINE__));
	}

	if (IsDisguised() && !House->Is_Ally(PlayerPtr)) {
		return(GetDisguise(true)->UIName);
	}

	return(Class->UIName);
}

// FUNCTION: 0x0051F330
void InfantryClass::Rocking_AI(void)
{
	int weapon = What_Weapon_Should_I_Use(NULL);
	if (TarCom != NULL && In_Range(TarCom, weapon)) {
		return;
	}

	AbstractClass * target = Greatest_Threat(THREAT_RANGE, PositionCoord, false);
	if (TarCom != NULL || target != NULL) {
		Assign_Target(target);
	}
	if (target != NULL) {
		return;
	}
	if (CurrentMission != MISSION_GUARD && !Techno_Type_Class()->JumpJet) {
		Enter_Idle_Mode(false, true);
	}
}

// FUNCTION: 0x0051F3E0
int InfantryClass::Do_MISSION_ATTACK(void)
{
	if ((Class->IsBomber || Has_Ability(ABILITY_C4)) && TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)TarCom)->Class->CanC4 && !((BuildingClass *)TarCom)->Class->IsInvisibleInGame) {
		Assign_Destination(TarCom, true);
		QueueMission(MISSION_SABOTAGE, false);
		return(1);
	}

	if (!House->Is_Human_Player() && TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_BUILDING && (Class->IsCapture || ((Class->Occupier || Class->Assaulter) && ((BuildingClass *)TarCom)->CanBeOccupiedBy(this)))) {
		Assign_Destination(TarCom, true);
		Set_Mission(MISSION_CAPTURE);
		return(1);
	}

	if (House->Is_Human_Player()) {
		if (Doing == 0x1b) goto deploy_fire;
		if (Doing == 0x1c) goto deploy_fire;
		if (Doing == 0x1d) goto deploy_fire;
		if (Doing == 0x1e) {
deploy_fire:
			Rocking_AI();
			return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
		}
	}

	return(BASECLASS::Do_MISSION_ATTACK());
}

// FUNCTION: 0x0051F540
int InfantryClass::Do_MISSION_HUNT(void)
{
	if (!House->Is_Human_Player() && TarCom != NULL && TarCom->Fetch_RTTI() == RTTI_BUILDING && (Class->IsCapture || ((Class->Occupier || Class->Assaulter) && ((BuildingClass *)TarCom)->CanBeOccupiedBy(this)))) {
		Assign_Destination(TarCom, true);
		Set_Mission(MISSION_CAPTURE);
		return(1);
	}

	if (!House->Is_Human_Player()) {
		if (Doing == 0x1b) goto deploy_fire;
		if (Doing == 0x1c) goto deploy_fire;
		if (Doing == 0x1d) goto deploy_fire;
		if (Doing == 0x1e) {
deploy_fire:
			if (TarCom == NULL) {
				Do_Action((DoType)0x1f, false, false);
				return(1);
			}
		}
	}

	return(BASECLASS::Do_MISSION_HUNT());
}

// FUNCTION: 0x0051F620
int InfantryClass::Do_MISSION_GUARD(void)
{
	int delay = FUN_00521320();
	if (delay == -1) {
		delay = BASECLASS::Do_MISSION_GUARD();
	}
	return(delay);
}

// FUNCTION: 0x0051F640
int InfantryClass::Do_MISSION_GUARD_AREA(void)
{
	int delay = FUN_00521320();
	if (delay == -1) {
		delay = BASECLASS::Do_MISSION_GUARD_AREA();
	}
	return(delay);
}

// FUNCTION: 0x0051F660
int InfantryClass::Do_MISSION_MOVE(void)
{
	if (Doing == 0x1b || Doing == 0x1c || Doing == 0x1d || Doing == 0x1e) {
		if (Owner_HouseClass()->FUN_0050B730()) {
			Assign_Destination(NULL, true);
			return(1);
		}
		if (Class->UndeployDelay <= -1) {
			Do_Action((DoType)0x1f, false, false);
			return(Class->DoControls[0x1f].Count);
		}
	}
	return(BASECLASS::Do_MISSION_MOVE());
}

// FUNCTION: 0x0051F6E0
int InfantryClass::Do_MISSION_UNLOAD(void)
{
	if (Class->Deployer) {
		int delay = -1;
		if (Doing == 0x1b) goto undeploy;
		if (Doing == 0x1c) goto undeploy;
		if (Doing == 0x1d) goto undeploy;
		if (Doing == 0x1e) {
undeploy:
			if (Class->UndeployDelay <= -1) {
				Do_Action((DoType)0x1f, true, false);
			}
		} else {
			Do_Action((DoType)0x1b, true, false);
			WeaponDataStruct const * weapon = GetDeployWeapon();
			if (FUN_0070E240(weapon) && weapon->Weapon->AreaFire) {
				if (Is_Type_Named("DESO")) {
					delay = Class->DoControls[0x1b].Count + 1;
				} else {
					Assign_Target(&Map[Get_Cell()]);
				}
			}
			if (Class->UndeployDelay > -1) {
				delay = Class->UndeployDelay;
			}
		}
		Set_Mission(MISSION_GUARD);
		Assign_Destination(NULL, true);
		if (delay > -1) {
			return(delay);
		}
	}
	return(BASECLASS::Do_MISSION_UNLOAD());
}

// FUNCTION: 0x0051F800
ActionType InfantryClass::What_Action(Cell const & cell, bool check_fog, bool disallow_force) const
{
	if (!House->Is_Player_Control()) {
		return(ACTION_NONE);
	}

	ActionType action = BASECLASS::What_Action(cell, check_fog, disallow_force);

	if (Combat_Damage() < 0 && House->Is_Player_Control() && action == ACTION_ATTACK) {
		action = ACTION_GUARD_AREA;
	}

	if (Class->JumpJet && (action == ACTION_MOVE || action == ACTION_NOMOVE)) {
		if (Map[cell].Is_Near_Tunnel_NW()) {
			action = ACTION_NONE;
		} else if (Map[cell].Is_Near_Tunnel_ES()) {
			action = ACTION_NONE;
		}
	}

	if (House->Is_Player_Control() && Class->Deployer && (Doing == 0x1b || Doing == 0x1c || Doing == 0x1d || Doing == 0x1e) && (action == ACTION_MOVE || action == ACTION_ATTACK)) {
		if (action == ACTION_MOVE) {
			action = ACTION_NOMOVE;
		} else {
			bool inrange = false;
			if (Class->DeployFire) {
				int weapon = What_Weapon_Should_I_Use(NULL);
				Coord coord = cell;
				inrange = In_Range(coord, weapon);
			}
			if (!inrange) {
				action = ACTION_NOMOVE;
			}
		}
	} else if (action == ACTION_MOVE && Map[cell].Has_Tunnel()) {
		return(Map[cell].Can_Enter_Tunnel(this) ? ACTION_ENTER_TUNNEL : ACTION_NO_ENTER_TUNNEL);
	}

	CellClass * cellptr = &Map[cell];
	BuildingTypeClass * fogged_building = NULL;
	FoggedObjectClass * fogged_object = NULL;

	if (Scen->Special.IsFogOfWar && check_fog && cellptr->FoggedObjects != NULL) {
		for (int i = 0; i < cellptr->FoggedObjects->Count(); i++) {
			fogged_object = (*cellptr->FoggedObjects)[i];
			if (fogged_object->CanDraw && fogged_object->RTTI == RTTI_BUILDING) {
				if (fogged_object->RTTI == RTTI_BUILDING) {
					fogged_building = (BuildingTypeClass *)fogged_object->Records[0].TypeClass;
				} else {
					fogged_building = NULL;
				}
				break;
			}
		}
	}

	if (Class->IsEngineer && fogged_building != NULL && House->Is_Player_Control()) {
		if (fogged_building->IsBridgeRepairHut) {
			action = Map.Can_Repair_Bridge(fogged_object->Center_Coord()) ? ACTION_GREPAIR : ACTION_NO_GREPAIR;
			goto done;
		}
		if (fogged_building->IsRepairable) {
			action = ACTION_CAPTURE;
			goto done;
		}
	}

	if (action == ACTION_ATTACK && !Is_Weapon_Equipped()) {
		action = ACTION_ATTACK_SUPPORT;
	}
done:
	if (House->Is_Player_Control() && action == ACTION_NONE && !Class->JumpJet && Can_Player_Move()) {
		action = ACTION_NOMOVE;
	}

	return(action);
}

// FUNCTION: 0x0051FAF0
ObjectTypeClass const * InfantryClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: 0x0051FB00
void InfantryClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();

	int len = ini.Entry_Count(INI_Name);
	for (int index = 0; index < len; index++) {
		char const * entry = ini.Get_Entry(INI_Name, index);
		char buf[128];
		ini.Get_String(INI_Name, entry, NULL, buf, sizeof(buf));

		int inhouse = FUN_0050C170(strtok(buf, ","));
		if (inhouse != -1) {
			HouseClass * inhousep = Houses[inhouse];
			InfantryType classid = InfantryTypeClass::From_Name(strtok(NULL, ","));

			if (classid != INFANTRY_NONE) {
				InfantryClass * infantry = new InfantryClass(InfantryTypes[classid], inhousep);
				if (infantry != NULL) {
					int strength = atoi(strtok(NULL, ","));

					Coord coord;
					if (NewINIFormat >= 4) {
						int x = atoi(strtok(NULL, ","));
						int y = atoi(strtok(NULL, ","));
						coord = Cell(x, y).As_Coord();
					} else {
						coord = Cell(atoi(strtok(NULL, ","))).As_Coord();
					}

					int sub = atoi(strtok(NULL, ","));
					coord = StoppingCoordAbs[sub] + Coord_Whole(coord);

					MissionType mission = MissionClass::Mission_From_Name(strtok(NULL, ","));
					char * validation = strtok(NULL, ",");
					Dir256 dir;
					if (validation) {
						dir = (Dir256)atoi(validation);
					} else {
						dir = DIR_N;
					}

					validation = strtok(NULL, ",");
					if (validation) {
						TagTypeClass * tp = TagTypeClass::From_Name(validation);
						if (tp != NULL) {
							TagClass * tt = Find_Or_Make(tp);
							if (tt != NULL) {
								infantry->Attach_Tag(tt);
							}
						}
					}

					validation = strtok(NULL, ",");
					if (validation) {
						infantry->Veterancy.From_Integer(atoi(validation));
					}

					validation = strtok(NULL, ",");
					if (validation) {
						infantry->Group = atoi(validation);
					}

					validation = strtok(NULL, ",");
					if (validation) {
						infantry->IsOnBridge = atoi(validation) != 0;
						if (infantry->IsOnBridge) {
							coord.Z = Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT;
						} else {
							coord.Z = Map.Get_Height_GL(coord);
						}
					}

					validation = strtok(NULL, ",");
					if (validation) {
						infantry->IsTeamRecruitable = atoi(validation) != 0;
					}

					validation = strtok(NULL, ",");
					if (validation) {
						infantry->IsAutocreateRecruitable = atoi(validation) != 0;
					}

					if (&Map[coord] != &InvalidCell && infantry->Unlimbo(coord, dir)) {
						infantry->Strength = infantry->Class->MaxStrength * (strength / 256.0);
						if (infantry->Strength > infantry->Class->MaxStrength - 3) infantry->Strength = infantry->Class->MaxStrength;
						if (infantry->Strength < 1) infantry->Strength = 1;
						infantry->EstimatedHealth = infantry->Strength;
						infantry->QueueMission(mission, false);
						infantry->Commence();
					} else {
						delete infantry;
					}
				}
			}
		}
	}
}

// FUNCTION: 0x0051FEF0
void InfantryClass::Write_INI(CCINIClass & ini)
{
	ini.Clear(INI_Name);

	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * infantry = Infantry[index];
		if (!infantry->IsInLimbo) {
			char uname[10];
			char buf[128];

			sprintf(uname, "%d", index);
			sprintf(buf, "%s,%s,%d,%d,%d,%d,%s,%d,%s,%d,%d,%d,%d,%d",
					(char const *)infantry->House->IniName,
					(char const *)infantry->Class->IniName,
					int(infantry->HealthRatio * 256),
					infantry->PositionCell.X,
					infantry->PositionCell.Y,
					CellClass::Spot_Index(infantry->PositionCoord),
					MissionClass::Mission_Name((infantry->Get_Mission() == MISSION_NONE) ?
						infantry->MissionQueue : infantry->Get_Mission()),
					infantry->PrimaryFacing.Current().As_Dir256(),
					infantry->Tag != NULL ? (char const *)infantry->Tag->Class->IniName : "None",
					infantry->Veterancy.To_Integer(),
					infantry->Group,
					infantry->IsOnBridge,
					infantry->IsTeamRecruitable,
					infantry->IsAutocreateRecruitable
				);
			ini.Put_String(INI_Name, uname, buf);
		}
	}
}

static inline void Reduce_Fear(int & fear)
{
	fear--;
}

// FUNCTION: 0x005200B0
void InfantryClass::Fear_AI(void)
{
	if (Fear > 0) {
		if (!Class->IsFearless) {
			Reduce_Fear(Fear);
		}

		if (Fear == 0 && Ammo == 0 && Is_Weapon_Equipped()) {
			Ammo = Class->MaxAmmo;
		}

		if (IsProne) {
			if (Fear < 0x32 && Doing != 0x1b && Doing != 0x1c && Doing != 0x1d && Doing != 0x1e) {
				Do_Action((DoType)7, false, false);
			}
		} else {
			if (Fear >= 0x32 && Doing != 0x1b && Doing != 0x1c && Doing != 0x1d && Doing != 0x1e) {
				if (!House->FUN_0050B730() || (NavCom == NULL && !Locomotion->Is_Moving())) {
					if (!Class->IsFraidyCat) {
						Do_Action((DoType)5, false, false);
					}
				}
			}
		}
	}

	if (Class->IsFraidyCat && Fear > 0x32 && Doing != 0x1b && Doing != 0x1c && Doing != 0x1d && Doing != 0x1e && !IsFalling && !Locomotion->Is_Moving() && NavCom == NULL) {
		Scatter(COORD_NONE, true);
	}
}

// FUNCTION: 0x00520260
bool InfantryClass::Edge_Of_World_AI(void)
{
	return(Edge_Of_World(this));
}

// FUNCTION: 0x005202F0
bool InfantryClass::Theft_AI(void)
{
	if (!Class->IsThief) return(false);
	if (NavCom == NULL) return(false);
	if (NavCom->Fetch_RTTI() != RTTI_UNIT) return(false);

	UnitClass * unit = (UnitClass *)NavCom;
	if (House->Is_Ally(unit)) return(false);

	Coord coord = unit->Destination_Coord(this);
	int z = coord.Z;
	coord = Center_Coord();

	if (abs(z - coord.Z) < LEVEL_LEPTON_H) {
		if (Center_Coord().Distance_To(unit->Center_Coord()) < CELL_LEPTON / 2) {
			if (unit->Tag != NULL) {
				unit->Tag->Spring(TEVENT_PLAYER_ENTERED, this);
			}
			unit->Transmit_Message(RADIO_OVER_OUT);
			unit->Detach_All(false);
			if (Tag != NULL && Tag->Is_To_Inherit()) {
				unit->Attach_Tag(Tag);
			}
			unit->Captured(House, true);
			unit->EnteredByInfType = Class->HeapID;
			Delete_Me();
			return(true);
		}
	}

	if (Center_Coord().Distance_To(unit->Center_Coord()) < 2 * CELL_LEPTON) {
		if (Coord(Locomotion->Destination()) != COORD_NONE) {
			if (unit->Distance(Coord(Locomotion->Destination())) <= CELL_LEPTON / 2) {
				return(false);
			}
		}
		Assign_Destination(NavCom);
	} else {
		if (Coord(Locomotion->Destination()) != COORD_NONE) {
			if (unit->Destination_Coord(this).As_Cell() == Coord(Locomotion->Destination()).As_Cell()) {
				return(false);
			}
		}
	}
	Assign_Destination(NavCom);
	return(false);
}

// FUNCTION: 0x005206B0
void InfantryClass::Firing_AI(void)
{
	if (TarCom != NULL) {
		int primary = What_Weapon_Should_I_Use(TarCom);

		if (!IsFiring) {
			switch (Can_Fire(TarCom, primary, true)) {
				case FIRE_ILLEGAL:
					if (Combat_Damage(primary) < 0) {
						AbstractClass * obj = TarCom;
						if (obj != NULL && (obj->AbstractFlags & 2) && obj->Fetch_RTTI() == RTTI_INFANTRY) {
							if (((ObjectClass *)obj)->HealthRatio >= Rule->ConditionGreen) {
								Assign_Target(NULL);
							}
						} else {
							Assign_Target(NULL);
						}
					}
					break;

				case FIRE_CLOAKED:
					Do_Uncloak(false);
					break;

				case FIRE_OK:
					if (Is_JumpJet()) {
						Do_Action((DoType)0x1a);
					} else if (Doing == 0x1b) {
						goto deployed;
					} else if (Doing == 0x1c) {
						goto deployed;
					} else if (Doing == 0x1d) {
						goto deployed;
					} else if (Doing == 0x1e) {
deployed:
						Do_Action((DoType)0x1d);
					} else if (Airstrike != NULL && (Doing == 0x28 || Doing == 0x29)) {
						Animation.Set_Stage(0);
					} else if (primary != 0 && IsProne && Class->DoControls[0x29].Count != 0) {
						Do_Action((DoType)0x29);
					} else if (primary != 0 && Class->DoControls[0x28].Count != 0) {
						Do_Action((DoType)0x28);
					} else if (IsProne) {
						Do_Action((DoType)8);
					} else {
						Do_Action((DoType)4);
					}

					IsFiring = true;
					PrimaryFacing.Set(GetTargetDirection(TarCom));

					if (TarCom == NavCom) {
						AbortMotion();
						Stop_Driver();
					}
					break;
			}
		}

		int firestage = Class->FireLaunch;
		if (IsProne) firestage = Class->ProneLaunch;
		if (primary != 0) {
			if (IsProne && Class->DoControls[0x29].Count != 0) firestage = Class->SecondaryProne;
			if (!IsProne && Class->DoControls[0x28].Count != 0) firestage = Class->SecondaryFire;
		}

		if (IsFiring && Animation.Fetch_Stage() == firestage) {
			primary = What_Weapon_Should_I_Use(TarCom);
			if (Can_Fire(TarCom, primary, true) == FIRE_OK) {
				Fire_At(TarCom, primary);
			} else {
				IsFiring = false;
				if (IsProne) {
					Do_Action((DoType)2);
				} else if (Doing == 0x1b) {
					goto deployed_idle;
				} else if (Doing == 0x1c) {
					goto deployed_idle;
				} else if (Doing == 0x1d) {
					goto deployed_idle;
				} else if (Doing == 0x1e) {
deployed_idle:
					Do_Action((DoType)0x1c);
				} else {
					Do_Action((DoType)0);
				}
			}

			if (TarCom != NULL) {
				WeaponDataStruct const * wdata = Get_Class_Weapon_Data(0);
				if (wdata->Weapon->MaxSpeed < Rule->Incoming) {
					Map[TarCom->Center_Coord()].Incoming(PositionCoord, true);
				}
			}
		}
	} else {
		IsFiring = false;
	}
}

// FUNCTION: 0x00520AE0
void InfantryClass::Doing_AI(void)
{
	if (Doing == -1 || Animation.Fetch_Stage() >= Class->DoControls[Doing].Count) {
		switch (Doing) {
			case 0x1b:
				Do_Action((DoType)0x1c, true, false);
				if (!Class->DeployedCrushable) {
					Uncrushable = true;
				}
				FUN_0070F770();
				break;

			case 0x26:
				if (Get_Mission() == (MissionType)0xa) {
					Do_Action((DoType)0x26, true, false);
				} else {
					Do_Action((DoType)0, true, false);
				}
				break;

			case 0x1f:
				Do_Action((DoType)0, true, false);
				if (!Class->DeployedCrushable) {
					Uncrushable = false;
				}
				break;

			case 0x22:
				Do_Action((DoType)0x23, true, false);
				break;

			case 0xb:
			case 0xc:
			case 0xd:
			case 0xe:
			case 0xf:
				if (Animation.Fetch_Stage() >= Class->DoControls[Doing].Count) {
					if (Class->DeadBodies.Count() > 0) {
						new AnimClass(Class->DeadBodies.Pick(Scen->RandomNumber), Center_Coord());
					} else if (!Class->NotHuman) {
						new AnimClass(Rule->DeadBodies.Pick(Scen->RandomNumber), Center_Coord());
					}
					Delete_Me();
				}
				break;

			case 0x14:
			case 0x15:
			case 0x24:
				if (Animation.Fetch_Stage() >= Class->DoControls[Doing].Count) {
					Delete_Me();
				}
				break;

			case 0x21:
				break;

			default:
				if (Doing != -1) {
					if (Class->DoControls[Doing].Facing != -1) {
						PrimaryFacing.Set((DirType)(Class->DoControls[Doing].Facing << 13));
					}
				}

				if (Locomotion->Is_Moving() && Speed > 0.1) {
					if (IsProne) {
						Do_Action((DoType)6, true, false);
					} else {
						Do_Action((DoType)3, true, false);
					}
				} else if ((Doing == 0x28 || Doing == 0x29) && Airstrike != NULL && TarCom != NULL && !Locomotion->Is_Moving()) {
					if (IsProne) {
						Do_Action((DoType)0x29, true, false);
					} else {
						Do_Action((DoType)0x28, true, false);
					}
					IsFiring = false;
				} else if (Doing == 0x1b) {
					goto settle;
				} else if (Doing == 0x1c) {
					goto settle;
				} else if (Doing == 0x1d) {
					goto settle;
				} else if (Doing == 0x1e) {
settle:
					Do_Action((DoType)0x1c, true, false);
				} else if (IsProne) {
					Do_Action((DoType)2, true, false);
				} else {
					Do_Action((DoType)0, true, false);
				}
				break;
		}
	}

	if (Doing == 0x21 && !IsFalling) {
		Do_Action((DoType)0, true, false);
	}

	DoInfoStruct const * info = &Class->DoControls[Doing];
	for (int i = 0; i < info->SoundCount; i++) {
		if (Animation.Has_Changed()) {
			if (Animation.Fetch_Stage() % MAX(info->Count, 1) == (&info->Sound1StartFrame)[i * 2]) {
				VocClass::PlayAt((&info->Sound1StartFrame)[i * 2 + 1], PositionCoord);
			}
		}
	}
}

// FUNCTION: 0x00520F40
void InfantryClass::Movement_AI(void)
{
	if (Get_Mission() == MISSION_MOVE && !Locomotion->Is_Moving()) {
		if (NavCom == NULL) {
			Enter_Idle_Mode();
		} else {
			Assign_Destination(NavCom);
			Set_Speed(1.0);
		}
	}

	if (NavCom != NULL) {
		Cell cell = NavCom->Destination_Coord(this).As_Cell();

		if ((!IsZoneCheat || Can_Enter_Cell(Get_Target_Cell_Ptr()) != MOVE_NO) && !Locomotion->Is_Moving() && !IsTethered && NavCom != NULL && IsInPlayfield && !Map.Is_Same_Cell_Zone(Destination_Coord(NULL).As_Cell(), cell, Class->MZone, Is_Moving_Onto_Bridge(NULL), Map[cell].IsUnderBridge, Is_Allowed_To_Leave_Map())) {
			if (!Class->IsCapture && Get_Mission() != MISSION_ENTER) {
				Assign_Destination(NULL);
			}
		}
	}

	if (Locomotion->Is_Really_Moving_Now()) {
		if (Is_JumpJet()) {
			if (!IsFiring) {
				if (Speed > 0.8) {
					Do_Action((DoType)0x18);
				} else {
					Do_Action((DoType)0x17);
				}
			}
		} else {
			if (IsProne) {
				Do_Action((DoType)6);
			} else {
				Do_Action((DoType)3);
			}
		}
	} else {
		if (Doing == 3 || Doing == 0x18 || Doing == 0x17) {
			Do_Action((DoType)0);
		} else if (Doing == 6) {
			Do_Action((DoType)2);
		} else if (Doing == 0x11) {
			Do_Action((DoType)0x10);
		}
	}
}

// FUNCTION: 0x00521320
int InfantryClass::FUN_00521320(void)
{
	if (Doing == 0x1b) goto deployed;
	if (Doing == 0x1c) goto deployed;
	if (Doing == 0x1d) goto deployed;
	if (Doing == 0x1e) {
deployed:
		if (Class->UndeployDelay > -1) {
			Do_Action((DoType)0x1f, false, false);
			return(Class->DoControls[0x1f].Count);
		}
		if (Class->DeployFire) {
			if (Class->ImmuneToRadiation) {
				RadSiteClass * site = Map[Get_Cell()].FUN_00487C80();
				WeaponTypeClass * weapon = Get_Class_Weapon_Data(1)->Weapon;
				int level = weapon->RadLevel / 3;
				if (site == NULL || site->GetRadLevel() < level) {
					Assign_Target(&Map[Get_Cell()]);
					if (Can_Fire(TarCom, 1, true) == 0) {
						Fire_At(TarCom, 1);
						Assign_Target(NULL);
						Do_Action((DoType)0x1d, false, false);
						return(Class->DoControls[0x1d].Count);
					}
					Assign_Target(NULL);
				}
				return(Current_Mission_Control().Normal_Delay() + Random_Pick(10, 20));
			}
			Rocking_AI();
			return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
		}
		return(-1);
	}

	if (!Owner_HouseClass()->FUN_0050B730() && Class->Deployer && Class->DeployFire && Class->UndeployDelay <= -1 && NavCom == NULL && CurrentMissionStartTime + Rule->AIAutoDeployFrameDelay[House->Difficulty] < Frame) {
		if (ArchiveTarget != NULL) {
			if (ArchiveTarget->Center_Coord().As_Cell() != Center_Coord().As_Cell()) {
				return(-1);
			}
		}
		if (!Class->ImmuneToRadiation) {
			if (!Locomotion->Is_Moving()) {
				Do_Action((DoType)0x1b, false, false);
				return(Class->DoControls[0x1b].Count);
			}
			Locomotion->Stop_Moving();
			ShouldDeploy = true;
			return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
		}
	}
	return(-1);
}

// FUNCTION: 0x005216C0
void const * InfantryClass::Get_Image_Data(void) const
{
	return(BASECLASS::Get_Image_Data());
}

// FUNCTION: 0x005216D0
bool InfantryClass::Is_Ready_To_Random_Animate(void) const
{
	if (!BASECLASS::Is_Ready_To_Random_Animate()) {
		return(false);
	}
	if (Locomotion->Is_Moving()) {
		return(false);
	}
	if (IsProne) {
		return(false);
	}
	if (IsFiring) {
		return(false);
	}
	if (Doing == 0x1b) goto not_ready;
	if (Doing == 0x1c) goto not_ready;
	if (Doing == 0x1d) goto not_ready;
	if (Doing == 0x1e) {
not_ready:
		return(false);
	}
	if (Doing == 1 || Doing == 0 || Doing == 0x10) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00521760
bool InfantryClass::Paradrop(Coord const & coord)
{
	if (BASECLASS::Paradrop(coord)) {
		if (House->FUN_0050B730()) {
			QueueMission(MISSION_GUARD, false);
		} else {
			QueueMission(MISSION_HUNT, false);
		}
		Do_Action((DoType)0x21, true, false);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005217C0
void InfantryClass::Set_Occupy_Bit(Coord const & coord)
{
	int spot_index = CellClass::Spot_Index(coord);
	CellClass & cell = Map[coord];
	if (Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT <= coord.Z && cell.IsUnderBridge) {
		cell.BridgeFlag.Composite |= (1 << spot_index);
		cell.BridgeInfType = Owner();
	} else {
		cell.Flag.Composite |= (1 << spot_index);
		cell.InfType = Owner();
	}
}

// FUNCTION: 0x00521850
void InfantryClass::Clear_Occupy_Bit(Coord const & coord)
{
	int spot_index = CellClass::Spot_Index(coord);
	CellClass & cell = Map[coord];
	int height = Map.Get_Height_GL(coord);

	if (height + BRIDGE_LEPTON_HEIGHT <= coord.Z) {
		cell.BridgeFlag.Composite &= ~(1 << spot_index);
		if (!(cell.BridgeFlag.Composite & 0x1C)) {
			cell.BridgeInfType = HOUSE_NONE;
		}
	} else {
		cell.Flag.Composite &= ~(1 << spot_index);
		if (!(cell.Flag.Composite & 0x1C)) {
			cell.InfType = HOUSE_NONE;
		}
	}
}

// FUNCTION: 0x005218E0
int InfantryClass::What_Weapon_Should_I_Use(AbstractClass * target) const
{
	if (Class->DeployFire) {
		if (Doing == 0x1b) goto deploy_weapon;
		if (Doing == 0x1c) goto deploy_weapon;
		if (Doing == 0x1d) goto deploy_weapon;
		if (Doing == 0x1e) {
deploy_weapon:
			return(Class->DeployFireWeapon);
		}
		if (InOpenToppedTransport && Techno_Type_Class()->OpenTransportWeapon != -1) {
			return(Techno_Type_Class()->OpenTransportWeapon);
		}
		return(0);
	}
	return(BASECLASS::What_Weapon_Should_I_Use(target));
}

// FUNCTION: 0x00521960
HRESULT STDMETHODCALLTYPE InfantryClass::Load(IStream * stream)
{
	TargetTracker.Remove_Index(Fetch_ID());
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		ILocomotion * locomotion = Locomotion;

		new (this) InfantryClass(NoInitClass());

		Locomotion = locomotion;
		locomotion->Release();

		TargetTracker.Add_Index(Fetch_ID(), this);
		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&Disguise);
		Swizzle_Pointer(&DisguisedAsHouse);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00521B00
HRESULT STDMETHODCALLTYPE InfantryClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x00521B20
void InfantryClass::Stop_Movement_Animation(void)
{
	if (Doing == 6 || Doing == 3 || Doing == 0x11) {
		Doing = (DoType)-1;
	}
}

// FUNCTION: 0x00521B40
void InfantryClass::vt_entry_54C(void)
{
	if (ShouldDeploy) {
		ShouldDeploy = false;
		Do_Action((DoType)0x1b);
	}
}

// FUNCTION: 0x00521B60
bool InfantryClass::Ready_To_Next_Mission(void) const
{
	if (CurrentMission != MISSION_STICKY && CurrentMission != MISSION_RESCUE) {
		if (!IsFiring && !IsFalling) {
			if (Locomotion->Is_Moving_Now() && Mission != MISSION_GUARD && Mission != MISSION_HUNT && (Mission != MISSION_ATTACK || TarCom)) {
				return(false);
			}
			if (Doing == DO_NOTHING || MasterDoControls[Doing].Interrupt) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: 0x00521C10
void InfantryClass::Start_Fear(void)
{
	if (!Class->IsFearless && !Has_Ability(ABILITY_FEARLESS)) {
		Fear = (FearType)0x12c;
	}
}

// FUNCTION: 0x00521C40
void InfantryClass::Stop_Fear(void)
{
	if (!Class->IsFearless) {
		Fear = FEAR_NONE;
	}
}

// FUNCTION: 0x00521C60
void InfantryClass::Do_Idle(int which)
{
	switch (which) {
		case 0:
		default:
			Do_Action(DO_IDLE1);
			break;
		case 1:
			Do_Action(DO_IDLE2);
			break;
	}
}

// FUNCTION: 0x00521C90
void InfantryClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Doing);
	crc((int)Comment);
	crc(Fear);
	crc(IsTechnician);
	crc(IsStoked);
	crc(IsProne);
	crc(IsZoneCheat);
	crc(WasSelected);
}

// FUNCTION: 0x00521D30
Coord InfantryClass::Turret_Coord(int which) const
{
	return(Fire_Coord(which, Coord(0, 0, 0)));
}

// FUNCTION: 0x00521D80
int InfantryClass::Current_Speed(void)
{
	int speed = BASECLASS::Current_Speed();
	if (IsProne) {
		if (Class->IsCrawling) {
			speed -= speed / 3;
		} else {
			speed += speed / 2;
		}
	}
	return(speed);
}

// FUNCTION: 0x00521DD0
void InfantryClass::On_Movement_Blocked(void)
{
	BASECLASS::On_Movement_Blocked();
	if (NavCom != NULL && Distance(NavCom) < CELL_LEPTON_W * 4 && Is_JumpJet()) {
	}
}

// FUNCTION: 0x00521EB0
bool InfantryClass::JumpJet_To_Walk(void)
{
	int path_length = 0;
	if (NavCom != NULL) {
		for (int i = 0; i < CONQUER_PATH_MAX; i++) {
			if (Path[i] == FACING_NONE) break;
			if (Path[i] == FACING_COUNT) break;
			path_length++;
		}
	}
	if (path_length >= 4) return(false);

	if (Is_JumpJet()) {
		IPiggybackPtr piggy(Locomotion);
		if (piggy != NULL && !piggy->Is_Piggybacking()) {
			ILocomotionPtr walk(CLSID_WalkLocomotion);
			walk->Link_To_Object(this);
			piggy = IPiggybackPtr(walk);
			if (piggy != NULL) {
				Path[0] = FACING_NONE;
				piggy->Begin_Piggyback(Locomotion);
				Locomotion = walk;
				Locomotion->Move_To(NavCom->Center_Coord());
				return(true);
			}
		}
	}

	return(false);
}

// FUNCTION: 0x005220F0
void InfantryClass::Berzerk(void)
{
	IsBerzerk = true;
}

// FUNCTION: 0x005221D0
bool InfantryClass::Should_JumpJet_Fly(Cell const & from, Cell const & to)
{
	int height = HeightAGL;
	height = height;

	if (House->IonSensitivesShouldBeOffline()) {
		return(false);
	}

	if (HeightAGL > 0) {
		return(true);
	}

	if (House->FUN_0050B730()) {
		CellClass * startptr = &Map[from];
		if (startptr != NULL && startptr->FUN_00484AE0()) {
			return(false);
		}
	}

	if (Map.Is_Same_Cell_Zone(from, to, MZONE_INFANTRY, IsOnBridge, Map[to].IsUnderBridge, false)) {
		if (from == to) {
			return(false);
		}
		int dist = MAX(abs(to.X - from.X), abs(to.Y - from.Y));
		if (dist == 1) {
			return(false);
		}
		if (dist >= 4 || !Map.In_Local_Radar(from) || !Map.In_Local_Radar(to)) {
			return(true);
		}
		return(Search.Test_Cell_Walk(from, to, this, IsOnBridge, Map[to].IsUnderBridge, MZONE_INFANTRY) > 7);
	}

	return(true);
}

static inline bool Is_Deploy_Doing(int doing)
{
	return(doing == 0x1b || doing == 0x1c || doing == 0x1d || doing == 0x1e);
}

// FUNCTION: 0x00522340
AbstractClass * InfantryClass::Approach_Target(bool force)
{
	if (TarCom != NULL) {
		if (!Is_Deploy_Doing(Doing)) {
			return(BASECLASS::Approach_Target(false));
		}
		if (!Owner_HouseClass()->FUN_0050B730() && Class->Deployer && Class->DeployFire) {
			int weapon = Class->DeployFireWeapon;
			bool inrange = In_Range(TarCom, weapon);
			WeaponTypeClass const * wt = Get_Class_Weapon_Data(weapon)->Weapon;
			if (wt != NULL && !wt->AreaFire) {
				if (Is_Deploy_Doing(Doing)) {
					if (!inrange && !force) {
						Do_Action((DoType)0x1f);
					}
				} else if (inrange && !force) {
					if (!Locomotion->Is_Moving()) {
						Do_Action((DoType)0x1b);
						return(NULL);
					}
					Locomotion->Stop_Moving();
					ShouldDeploy = true;
					Current_Mission_Control();
					Random_Pick(0, 2);
					return(NULL);
				}
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x005224D0
bool InfantryClass::Is_Renovator(void) const
{
	return(Class->IsEngineer);
}

// FUNCTION: 0x005224E0
bool InfantryClass::FUN_005224E0(void) const
{
	return(Class->Deployer);
}

// FUNCTION: 0x005224F0
bool InfantryClass::FUN_005224F0(void) const
{
	return(Class->UndeployDelay > -1);
}

// FUNCTION: 0x00522510
bool InfantryClass::FUN_00522510(void) const
{
	return(Is_Deploy_Doing(Doing));
}

// FUNCTION: 0x00522540
bool InfantryClass::FUN_00522540(void) const
{
	return(Class->DeployFire);
}

// FUNCTION: 0x00522550
int InfantryClass::FUN_00522550(void)
{
	if (!Locomotion->Is_Moving()) {
		Do_Action((DoType)0x1b, false, false);
		return(Class->DoControls[0x1b].Count);
	}
	Locomotion->Stop_Moving();
	ShouldDeploy = true;
	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: 0x00522600
int InfantryClass::IronCurtain(int duration, HouseClass * source, bool force_shield)
{
	int damage = Class->MaxStrength;
	return(Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, false, source));
}

// FUNCTION: 0x00522640
ObjectTypeClass * InfantryClass::GetDisguise(bool against_allies) const
{
	if (House->Is_Ally(PlayerPtr) && !against_allies) {
		return(Class);
	}
	if (Disguise == NULL) {
		if (PlayerPtr != NULL) {
			if (PlayerPtr->ActLike == 0) {
				return((ObjectTypeClass *)Rule->AlliedDisguise);
			}
			if (PlayerPtr->ActLike == 1) {
				return((ObjectTypeClass *)Rule->SovietDisguise);
			}
		}
		return((ObjectTypeClass *)Rule->ThirdDisguise);
	}
	return(Disguise);
}

// FUNCTION: 0x005226C0
HouseClass * InfantryClass::GetDisguiseHouse(bool against_allies) const
{
	if (House->Is_Ally(PlayerPtr) && !against_allies) {
		return(House);
	}
	if (DisguisedAsHouse == NULL) {
		return(PlayerPtr);
	}
	return(DisguisedAsHouse);
}

#pragma warning(push)
#pragma warning(disable: 4716)
// FUNCTION: 0x00522700
unsigned InfantryClass::DisguiseAs(AbstractClass * target)
{
	if (target != NULL && target->Fetch_RTTI() == RTTI_INFANTRY) {
		BASECLASS::DisguiseAs(target);
		ObjectClass * object = (ObjectClass *)target;
		if (object->IsDisguised()) {
			Disguise = object->GetDisguise(true);
			DisguisedAsHouse = object->GetDisguiseHouse(true);
		} else {
			Disguise = (ObjectTypeClass *)object->Class_Of();
			DisguisedAsHouse = object->Owner_HouseClass();
		}
	}
}

#pragma warning(pop)

// FUNCTION: 0x00522780
void InfantryClass::ClearDisguise(void)
{
	if (Class->PermaDisguise) {
		Disguised = true;
		DisguisedAsHouse = House;
		if (House->ActLike == 0) {
			Disguise = (ObjectTypeClass *)Rule->AlliedDisguise;
		} else if (House->ActLike == 1) {
			Disguise = (ObjectTypeClass *)Rule->SovietDisguise;
		} else {
			Disguise = (ObjectTypeClass *)Rule->ThirdDisguise;
		}
	} else {
		Disguised = false;
	}
}

// FUNCTION: 0x005227F0
bool InfantryClass::IsDisguisedAs(HouseClass * house) const
{
	if (IsDisguised()) {
		CellClass * cell = &Map[Center_Coord().As_Cell()];
		if (!House->Is_Ally(house) && !cell->Is_Disguise_Sensed(house->HeapID)) {
			HouseClass * disguiser = DisguisedAsHouse;
			if (disguiser == house || disguiser == NULL || house->Is_Ally(disguiser)) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: 0x00523300
HRESULT STDMETHODCALLTYPE InfantryClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_InfantryClass;
	return(S_OK);
}
