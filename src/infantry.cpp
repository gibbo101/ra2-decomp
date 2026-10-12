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
#include "rules.h"
#include "slavemgr.h"
#include "scheme.h"
#include "session.h"
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

inline bool InfantryClass::Is_JumpJet(void) const
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

inline bool InfantryClass::Edge_Of_World_AI(void)
{
	if (Team != NULL && IsInPlayfield) {
		Team->IsLeaveMap = true;
	}

	if (Team == NULL && Get_Mission() == MISSION_GUARD && !Map.In_Radar(PositionCoord)) {
		Stun();
		Delete_Me();
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

	if (Edge_Of_World_AI()) {
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

// FUNCTION: 0x00521B00
HRESULT STDMETHODCALLTYPE InfantryClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x00523300
HRESULT STDMETHODCALLTYPE InfantryClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_InfantryClass;
	return(S_OK);
}
