// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "unit.h"
#include "_bench.h"
#include "_convert.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "astar.h"
#include "bench.h"
#include "blit.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "conquer.h"
#include "draw.h"
#include "fog.h"
#include "house.h"
#include "houstype.h"
#include "ilocos.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipiggy.h"
#include "isotype.h"
#include "lightcon.h"
#include "mixfile.h"
#include "mono.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "revent.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "ebolt.h"
#include "shapeset.h"
#include "slavemgr.h"
#include "spawnmgr.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "team.h"
#include "tiberium.h"
#include "tracker.h"
#include "tube.h"
#include "unittype.h"
#include "voc.h"
#include "vox.h"
#include "warhead.h"
#include "weapon.h"
#include "bench.hh"
#include "color.hh"
#include "tube.hh"

extern int DAT_00B1CFA8, DAT_00B1CFAC, DAT_00B1CFB0, DAT_00B1CFB4;
extern int DAT_00B1CFC0, DAT_00B1CFC4, DAT_00B1CFC8, DAT_00B1CFCC;
extern int __fastcall FUN_0050C170(char const * name);

// FUNCTION: 0x00735390
void FUN_00735390(void)
{
	DAT_00B1CFC0 = DAT_00B1CFA8;
	DAT_00B1CFC4 = DAT_00B1CFAC;
	DAT_00B1CFC8 = DAT_00B1CFB0;
	DAT_00B1CFCC = DAT_00B1CFB4;
}

// FUNCTION: 0x007353C0
UnitClass::UnitClass(UnitTypeClass const * type, HouseClass * house) :
	BASECLASS(house),
	FiringSyncDelay(-1),
	Class((UnitTypeClass *)type),
	FollowingMe(NULL),
	Flagged(HOUSE_NONE),
	IsFollowing(false),
	IsDumping(false),
	IsHarvesting(false),
	TerrainPalette(false),
	unknown_int_6D4(-1),
	DeathCounter(-1),
	ElectricBolt(NULL),
	Deployed(false),
	Deploying(false),
	Undeploying(false),
	NonPassengerCount(0)
{
	Create_ID();
	Units.Add(this);
	SecondaryFacing.Set(PrimaryFacing.Current());

	if (Class != NULL) {
		Locomotion = ILocomotionPtr(Class->Locomotor, NULL, CLSCTX_ALL);
		Locomotion->Link_To_Object(this);
	}

	if (Class != NULL) {
		PrimaryFacing.Set_ROT(Class->ROT);
		SecondaryFacing.Set_ROT(Class->ROT);
		if (Class->InitialAmmo == -1) {
			Ammo = Class->MaxAmmo;
		} else {
			Ammo = Class->InitialAmmo;
		}
		IsCloakable = Class->IsCloakable;
		Strength = Class->MaxStrength;
		EstimatedHealth = Strength;
	}

	ReloadTimer = 0;

	if (House != NULL) {
		House->Tracking_Add(this);
	}

	TechnoClass::Init();

	if (Class != NULL) {
		if (House != NULL) {
			TypeList<UnitTypeClass *> & list = House->Class->VeteranUnits;
			for (int i = 0; i < list.Count(); i++) {
				if (list[i] == Class) {
					Veterancy.Set_Veteran(true);
					break;
				}
			}
		}
		if (House != NULL && House->WarFactoryInfiltrated && !Class->Naval && Class->IsTrainable) {
			Veterancy.Set_Veteran(true);
		}
		if (Class->Gunner) {
			FUN_0070DC70(0);
		}
	}

	TargetTracker.Add_Index(Fetch_ID(), this);
}

// FUNCTION: 0x00735780 UnitClass::~UnitClass
UnitClass::~UnitClass(void)
{
	if (ElectricBolt != NULL) {
		ElectricBolt->~EBolt();
		ElectricBolt = NULL;
	}

	if (GameActive && Class != NULL) {
		if (House->Can_Build(Class, false, false) == -1) {
			House->IsRecalcNeeded = true;
		}

		if (Team != NULL) {
			Team->Remove(this, -1, false);
			Team = NULL;
		}

		House->Tracking_Remove(this);

		if (!ScenarioInit) {
			while (Cargo.Is_Something_Attached()) {
				delete Cargo.Detach_Object();
			}
		}

		if (FootClass::Limbo() && Flagged != HOUSE_NONE) {
			TechnoClass * self = this;
			Houses[Flagged]->Flag_Attach(self->Get_Cell(), false);
			Flagged = HOUSE_NONE;
		}
	}

	Detach_This_From_All(this, true);
	Units.Delete(this);
	TargetTracker.Remove_Index(Fetch_ID());
	IsActive = false;
}

// FUNCTION: 0x00736990
void UnitClass::Rotation_AI(void)
{
	if (TarCom != NULL && !IsRotating) {
		DirType dir = GetTargetDirection(TarCom);

		if (Class->IsTurretEquipped) {
			WeaponDataStruct const * weapon = GetTurretWeapon();
			if (FUN_0070E240(weapon) && !weapon->Weapon->OmniFire) {
				if (((YRWeaponDataStruct const *)weapon)->TurretLocked) {
					SecondaryFacing.Set_Desired(PrimaryFacing.Current());
				} else {
					SecondaryFacing.Set_Desired(dir);
				}
			}
		} else {
			if (Class->Speed == SPEED_TRACK && NavCom == NULL && !Locomotion->Is_Moving() && PrimaryFacing.Current() == dir) {
				PrimaryFacing.Set_Desired(dir);
			}
		}
	}

	if (Class->TurretSpins) {
		SecondaryFacing.Set_Desired((Dir256)(SecondaryFacing.Current().As_Dir256() + DIR_STEP_32));
	} else {
		IsRotating = false;
		if (Class->IsTurretEquipped) {
			if (SecondaryFacing.Is_Rotating()) {
				if (!Class->TurretSpins) {
					IsRotating = SecondaryFacing.Is_Rotating();
				}
			} else if (TarCom == NULL) {
				bool ready = true;
				if (Frame - LastFireBulletFrame < Rule->GuardAreaTargetingDelay + 5) {
					ready = false;
				}
				if (BunkerLinkedItem != NULL) {
					ready = false;
				}
				if (!(Class->IsSimpleDeployer && Deployed) && ready) {
					WeaponDataStruct const * weapon = GetTurretWeapon();
					bool locked = FUN_0070E240(weapon) && ((YRWeaponDataStruct const *)weapon)->TurretLocked;
					if (NavCom != NULL && !locked) {
						if (!IsAttackedByLocomotor) {
							SecondaryFacing.Set_Desired(GetTargetDirection(NavCom));
						}
					} else {
						SecondaryFacing.Set_Desired(PrimaryFacing.Current());
					}
				}
			}
		}
	}

	if (Class->IsTurretEquipped) {
		TurretIsRotating = SecondaryFacing.Is_Rotating();
	}
}

// FUNCTION: 0x00736C10
bool UnitClass::Edge_Of_World_AI(void)
{
	if (Get_Mission() == MISSION_GUARD && !Map.In_Radar(Get_Coord()) && IsInPlayfield) {
		if (Team != NULL) {
			Team->IsLeaveMap = true;
		}
		Stun();
		Delete_Me();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00736CA0
void UnitClass::Reload(void)
{
	if (!Class->MobileFire && Locomotion->Is_Moving()) {
		if (ReloadTimer > 0) {
			ReloadTimer = ReloadTimer + 1;
		}
	}
	TechnoClass::Reload();
}

// FUNCTION: 0x00736D50
bool UnitClass::Deploy_To_Fire(void) const
{
	if (Class->DeployToFire) {
		return(true);
	}

	if (Class->DeploysInto != NULL && Class->DeploysInto->IsTickTank) {
		if (!House->FUN_0050B730() && TarCom != NULL) {
			switch (TarCom->What_Am_I()) {
				case RTTI_INFANTRY:
					return(false);

				case RTTI_UNIT:
					if (Map[Get_Coord()].Can_Build_Here()) {
						return(true);
					}
					return(false);

				case RTTI_BUILDING:
					return(false);
			}
		}
	}
	return(false);
}

inline Cell Adjacent_Cell_By_Value(Cell cell, FacingType dir)
{
	return(cell + AdjacentCell[(int)dir & 0x07]);
}

// FUNCTION: 0x00736DF0
void UnitClass::Firing_AI(void)
{
	if (TarCom != NULL && Get_Class_Weapon_Data(0)->Weapon != NULL) {
		int primary = What_Weapon_Should_I_Use(TarCom);
		FireErrorType ok = Can_Fire(TarCom, primary, true);

		if ((ok == FIRE_OK || ok == FIRE_FACING) && Deploy_To_Fire()) {
			QueueMission(MISSION_UNLOAD, false);
			return;
		}

		switch (ok) {
			case FIRE_ILLEGAL:
				if (Combat_Damage(primary) < 0) {
					AbstractClass * obj = TarCom;
					if (obj != NULL && (obj->AbstractFlags & 2) && obj->Fetch_RTTI() == RTTI_UNIT) {
						if (((ObjectClass *)obj)->HealthRatio >= Rule->ConditionGreen) {
							Assign_Target(NULL);
						}
					} else {
						Assign_Target(NULL);
					}
				}
				break;

			case FIRE_OK:
				if (!Class->unknown_E10) {
					IsFiring = false;
				}

				if (Class->LargeVisceroid || Class->SmallVisceroid) {
					Animation.Set_Stage(Visceroid_Fire_List[GetTargetDirection(TarCom).As_Dir8()]);
					Animation.Set_Rate(5);
				}

				Fire_At(TarCom, primary);
				break;

			case FIRE_FACING:
				if (!Class->HasTurret && Class->IsTurretEquipped) {
					SecondaryFacing.Set_Desired(GetTargetDirection(TarCom));
				} else {
					if (NavCom == NULL && !Locomotion->Is_Moving()) {
							PrimaryFacing.Set_Desired(GetTargetDirection(TarCom));
						SecondaryFacing.Set_Desired(PrimaryFacing.Desired());
					}
				}
				break;

			case FIRE_CLOAKED:
				IsFiring = false;
				if (TechnoClass::In_Range(TarCom, primary)) {
					Do_Uncloak(false);
				}
				break;

			case FIRE_RANGE:
			case FIRE_MUST_DEPLOY:
				IsFiring = false;
				break;

			case FIRE_CANT:
				if (SpawnManager != NULL) {
					SpawnManager->ResetTarget();
				}
				break;
		}

		if (Class->IsGattling && (ok == FIRE_OK || ok == FIRE_REARM || ok == FIRE_FACING || ok == FIRE_ROTATING)) {
			GattlingRateUp(1);
		} else if (Techno_Type_Class()->IsGattling) {
			GattlingRateDown(1);
		}

		if (Techno_Type_Class()->IsGattling) {
			if (FUN_0070DDF0() > 0) {
				TurretAnimFrame++;
			}
		} else if (ok == FIRE_OK || ok == FIRE_REARM) {
			TurretAnimFrame++;
		}
		return;
	}

	if (Techno_Type_Class()->IsGattling) {
		GattlingRateDown(1);
	}
	if (Techno_Type_Class()->IsGattling && FUN_0070DDF0() > 0) {
		TurretAnimFrame++;
	}
}

// FUNCTION: 0x00737180
void UnitClass::Visceroid_AI(void)
{
	Cell cell = PositionCell;
	int stage = Animation.Fetch_Stage();

	if (stage >= 90) {
		if (stage % 5 == 4) {
			Animation.Set_Stage(TICKS_PER_SECOND * 2 * Random_Pick(0, 2));
			Animation.Set_Rate(1);
		}
		return;
	}

	if (stage % 30 == 29) {
		Animation.Set_Stage(TICKS_PER_SECOND * 2 * Random_Pick(0, 2));
	}
	Animation.Set_Rate(1);

	if (!Locomotion->Is_Moving()) {
		if (Class->SmallVisceroid) {
			FacingType facing = FACING_NW;
			for (int i = 0; i < FACING_COUNT; i++) {
				facing = FacingType(unsigned(facing + FACING_45) % FACING_COUNT);
				FacingType f2 = facing;
				Cell adjacent = Adjacent_Cell_By_Value(cell, f2);
				CellClass * cellptr = &Map[adjacent];
				ObjectClass * occupier = cellptr->Cell_Occupier();
				if (occupier != NULL && occupier->Fetch_RTTI() == RTTI_UNIT && ((UnitClass *)occupier)->Class->SmallVisceroid) {
					UnitClass * visceroid = (UnitClass *)occupier;
					if (visceroid->NavCom == NULL && visceroid->TarCom == NULL) {
						visceroid->Assign_Destination(this);
						unknown_int_6D4 = -1;
					}
					return;
				}
			}
		}

		if (NavCom == NULL && TarCom == NULL) {
			FacingType dir;
			if (unknown_int_6D4 != -1 && Random_Pick(0, 2) != 0) {
				dir = (FacingType)unknown_int_6D4;
			} else {
				dir = Random_Pick(FACING_FIRST, FacingType(FACING_COUNT - 1));
				unknown_int_6D4 = dir;
			}
			Cell adjacent = Adjacent_Cell_By_Value(cell, dir);
			CellClass * cellptr = &Map[adjacent];
			if (Can_Enter_Cell(cellptr, dir, Get_Cell_Height(), &Map[cell]) == MOVE_OK) {
				Assign_Destination(cellptr);
			} else {
				unknown_int_6D4 = -1;
			}
		}
	}
}

// FUNCTION: 0x00737BA0
bool UnitClass::Unlimbo(Coord const & coord, Dir256 dir)
{
	if (BASECLASS::Unlimbo(coord, dir)) {
		SecondaryFacing.Set(dir);

		if (IsCloakable && !IsInPlayfield) {
			Cloak = CLOAKED;
		}

		if (Class->SmallVisceroid || Class->LargeVisceroid) {
			Animation.Set_Stage(Random_Pick(0, 29));
			Animation.Set_Rate(1);
		} else {
			Animation.Set_Stage(0);
			Animation.Set_Rate(0);
		}

		return(true);
	}
	return(false);
}

// FUNCTION: 0x00738890
bool UnitClass::Active_Click_With(ActionType action, ObjectClass * object, bool is_waypoint)
{
	ActionType what = What_Action(object, is_waypoint);
	if (action != what) {
		action = what;
		switch (action) {
			case ACTION_SABOTAGE:
			case ACTION_CAPTURE:
				action = ACTION_ATTACK;
				break;

			case ACTION_ENTER:
				action = ACTION_MOVE;
				break;

			default:
				break;
		}
	}

	if (action == ACTION_HEAL || action == ACTION_GREPAIR) {
		action = ACTION_ATTACK;
	}

	if (object == this && (action == ACTION_NOMOVE || action == ACTION_ATTACK)) {
		return(false);
	}

	if (TechnoClass::Berzerk) {
		return(false);
	}

	return(BASECLASS::Active_Click_With(action, object, is_waypoint));
}

// FUNCTION: 0x00738910
bool UnitClass::Active_Click_With(ActionType action, Cell const & cell, Cell const & cell2, bool is_waypoint)
{
	if (Deployed && What_Action(cell, false, is_waypoint) == ACTION_NOMOVE) {
		return(false);
	}
	if (TechnoClass::Berzerk) {
		return(false);
	}
	return(BASECLASS::Active_Click_With(action, cell, cell2, is_waypoint));
}

// FUNCTION: 0x00738970
bool UnitClass::Enter_Idle_Mode(bool initial, bool resume_waypoint)
{
	MissionType order;
	bool res = BASECLASS::Enter_Idle_Mode(initial, resume_waypoint);

	if (HaveMegaMission() || Get_Mission() == MISSION_WAIT) {
		return(res);
	}

	if (LocomotorTarget != NULL) {
		ReleaseLocomotor(true);
	}

	if (Get_Mission() == MISSION_MOVE && NavCom == NULL && PositionCoord == PositionCell.As_Coord()) {
		Transmit_Message(RADIO_OVER_OUT);
	}

	Handle_Navigation_List();

	if (NavCom != NULL) {
		order = MISSION_MOVE;
	} else if (Class->Harvester || Class->Weeder) {
		if (HasAnyLink() || Get_Mission() == MISSION_HARVEST || MissionQueue == MISSION_HARVEST) {
			return(res);
		}
		order = MISSION_HARVEST;
		if (!initial && House->FUN_0050B730()) {
			if (Map[Get_Coord()].Land_Type() != (Class->Harvester ? LAND_TIBERIUM : LAND_WEEDS)) {
				order = MISSION_GUARD;
			}
		}
		Assign_Target(NULL);
		Assign_Destination(NULL);
	} else if (!Is_Weapon_Equipped()) {
		if (IsALoaner && Class->Max_Passengers() > 0 && Cargo.Is_Something_Attached() && Team == NULL) {
			order = MISSION_UNLOAD;
		} else {
			if (IsDeploying) {
				return(res);
			}
			if (CurrentMission == MISSION_UNLOAD && Class->DeploysInto != NULL) {
				return(res);
			}
			if (CurrentMission == MISSION_GUARD_AREA) {
				return(res);
			}
			order = MISSION_GUARD;
			Assign_Target(NULL);
			Assign_Destination(NULL);
		}
	} else {
		if (Get_Mission() == MISSION_GUARD || Get_Mission() == MISSION_GUARD_AREA) {
			return(res);
		}
		if (Get_Mission() != MISSION_NONE && (Current_Mission_Control().IsParalyzed || Current_Mission_Control().IsZombie)) {
			return(res);
		}
		if (House->IQ < Rule->IQGuardArea && !Has_Ability(ABILITY_GUARD_AREA) && !Class->DefaultToGuardArea || Team != NULL || SlaveOwner != NULL || SlaveManager != NULL) {
			order = MISSION_GUARD;
		} else {
			order = MISSION_GUARD_AREA;
		}
	}

	if (order == MISSION_GUARD_AREA && BunkerLinkedItem != NULL) {
		order = MISSION_GUARD;
	}

	TechnoClass * contact = FUN_0065AD30(0);
	if (CurrentMission == MISSION_ENTER && contact != NULL && contact->Fetch_RTTI() == RTTI_BUILDING) {
		BuildingClass * building = (contact->Fetch_RTTI() == RTTI_BUILDING) ? (BuildingClass *)contact : NULL;
		if (building->Class->Bunker && Get_Cell_Ptr()->Cell_Building() != contact) {
			return(res);
		}
	}

	if (CurrentMission != MISSION_PATROL && CurrentMission != MISSION_GUARD_AREA && CurrentMission != MISSION_UNLOAD && CurrentMission != MISSION_EATEN) {
		QueueMission(order, false);
	}
	return(res);
}

// FUNCTION: 0x007393B0
void FUN_007393B0(void)
{
}

// FUNCTION: 0x00739AC0
void UnitClass::Deploy(void)
{
	if (Class->IsSimpleDeployer) {
		if (Get_Height_AGL() > 0 && !Techno_Type_Class()->DeployToLand) {
			return;
		}
		if (Deployed) {
			return;
		}
		if (!InAir && Get_Height_AGL() > 0) {
			InAir = true;
		}
		if (Deploying && DeployAnim != NULL) {
			if (Animation.Fetch_Stage() >= DeployAnim->Type->End + DeployAnim->Type->Start - 1) {
				Deployed = true;
				Deploying = false;
			}
		} else if (!InAir) {
			if (Class->DeployingAnim != NULL) {
				if (DeployAnim == NULL) {
					DeployAnim = new AnimClass(Class->DeployingAnim, PositionCoord, 0, 1, ShapeFlags_Type(0x600), 0, false);
					DeployAnim->SetOwnerObject(this);
					DeployAnim->LightConvert = (LightConvertClass *)GetRemapColour();
				}
				Animation.Set_Stage(DeployAnim->Type->Start);
				Animation.Set_Rate(DeployAnim->Type->Rate);
				Deploying = true;
			} else {
				Deployed = true;
			}
		}
		if (Techno_Type_Class()->DeploySound != -1) {
			VocClass::PlayAt(Techno_Type_Class()->DeploySound, PositionCoord, NULL);
		}
	}
}

// FUNCTION: 0x00739CD0
void UnitClass::Undeploy(void)
{
	if (Deployed) {
		if (Undeploying && DeployAnim != NULL) {
			if (Animation.Fetch_Stage() >= DeployAnim->Type->End + DeployAnim->Type->Start - 2) {
				Deployed = false;
				Undeploying = false;
				if (Techno_Type_Class()->DeployToLand) {
					Assign_Destination(&Map[Nearby_Location(this)], true);
				}
			}
		} else if (Class->DeployingAnim != NULL) {
			if (DeployAnim == NULL) {
				DeployAnim = new AnimClass(Class->DeployingAnim, PositionCoord, 0, 1, ShapeFlags_Type(0x600), 0, true);
				DeployAnim->SetOwnerObject(this);
				DeployAnim->LightConvert = (LightConvertClass *)GetRemapColour();
			}
			Animation.Set_Stage(DeployAnim->Type->Start);
			Animation.Set_Rate(DeployAnim->Type->Rate);
			Undeploying = true;
		} else {
			Deployed = false;
		}

		if (IsDisguised()) {
			ClearDisguise();
		}
		if (Techno_Type_Class()->UndeploySound != -1) {
			VocClass::PlayAt(Techno_Type_Class()->UndeploySound, PositionCoord, NULL);
		}
	}
}

// FUNCTION: 0x0073B0B0
bool UnitClass::Render(Rect & rect, bool forced, bool extras_only) const
{
	if (IsTethered) {
		TechnoClass * link = FUN_0065AD40(0);
		if (link->Fetch_RTTI() == RTTI_BUILDING && (link->Get_Mission() == MISSION_UNLOAD || link->MissionQueue == MISSION_UNLOAD)) {
			if (link->Door.Is_Door_Opening() || link->Door.Is_Door_Closing() || link->Door.Is_Door_Open() || link->Door.Is_Door_Closed()) {
				return(false);
			}
		}
	}
	return(BASECLASS::Render(rect, forced, false));
}

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
	static ConvertClass * ANIM_PAL;
};

struct PsyDom {
	static bool Active(void);
};

struct NukeFlash {
	static bool IsFadingIn(void);
};

inline void Coord_Pixel(Coord coord, Point2D & pixel)
{
	TacticalMap->Coord_To_Pixel(coord, pixel);
}

// FUNCTION: 0x0073CEC0
void UnitClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
	static ShapeSet const * harvesting_shape = (ShapeSet const *)FileSystem::LoadFile("OREGATH.SHP", false);

	Point2D adjusted_point = point;
	adjusted_point.Y -= IonBlastYDrawOffset;
	Point2D raw_point = adjusted_point;
	if (IsInTransport) {
		adjusted_point.Y += 14;
	}

	Cell cell = PositionCell;

	if (Visual_Character(false, NULL) != VISUAL_HIDDEN && !Deploying && !Undeploying) {

		TacticalMap->Add_To_Selectables((ObjectClass *)this, adjusted_point);
		int height = Get_Height_AGL();
		CellClass * cptr = &Map[Get_Coord()];

		int brightness = 0;
		if (((!IsOnBridge && height >= BRIDGE_LEPTON_HEIGHT) || (IsOnBridge && height >= 0)) &&
			(cptr->IsUnderBridge &&
				(cptr->IsBridgeEastWest && cptr->Adjacent_Cell(FACING_N).IsUnderBridge ||
				!cptr->IsBridgeEastWest && cptr->Adjacent_Cell(FACING_W).IsUnderBridge))) {
			int light;
			if (IonStormClass::Is_Ion_Storm_Active()) {
				light = Scen->IonLighting.LevelLight;
			} else if (PsyDom::Active()) {
				light = Scen->DominatorLighting.LevelLight;
			} else if (NukeFlash::IsFadingIn()) {
				light = Scen->NukeLighting.LevelLight;
			} else {
				light = Scen->NormalLighting.LevelLight;
			}
			brightness = Map[cell].Brightness + 4 * light;
		} else {
			int offset = 0;
			if (Map[cell].IsOvershadowed) {
				if (!In_Air()) {
					offset = -500;
				}
			}
			brightness = Map[cell].Brightness + offset;
		}
		brightness += Rule->ExtraUnitLight;

		if (Class->Harvester && IsHarvesting && !Locomotion->Is_Moving_Now()) {
			Point2D pixel;
			FacingType face = (FacingType)PrimaryFacing.Current().As_Dir8();
			Coord_Pixel(Move_Coord(Render_Coord(), face, 30), pixel);
			FacingType face2 = (FacingType)PrimaryFacing.Current().As_Dir8();
			int row = 7 - face2;
			if (TemporalTargetingMe == NULL && !IsBeingWarpedOut() && !IsWarpingIn()) {
				Draw_Shape(*DSurface::Temp, *FileSystem::ANIM_PAL, harvesting_shape, row * 15 + (Frame + TotalFramesWalked) % 15, pixel, cliprect, ShapeFlags_Type(0x2a00), NULL, Get_Z_Adjust() - 2, ZGRAD_GROUND, brightness);
			}
		}

		UnitTypeClass * oldclass = Class;
		if (Class->Harvester && IsDumping) {
			if (Class->UnloadingClass != NULL) {
				((UnitClass *)this)->Class = Class->UnloadingClass;
			}
		}

		UnitTypeClass * drawclass = Class;
		if (!IsClearlyVisibleTo(PlayerPtr)) {
			drawclass = (UnitTypeClass *)GetDisguise(true);
		}
		if (Deployed && Class->UnloadingClass != NULL) {
			drawclass = Class->UnloadingClass;
		}

		if (CurrentTube == TUBE_NONE) {
			if (drawclass->IsVoxel) {
				if (drawclass->Voxel.VoxLib != NULL) {
					((UnitClass *)this)->Unit_Draw_Voxel(adjusted_point, cliprect, brightness, 0);
				}
			} else {
				((UnitClass *)this)->Unit_Draw_Shape(adjusted_point, cliprect, brightness, 0);
			}
		}

		((UnitClass *)this)->Class = oldclass;

		if (Flagged != HOUSE_NONE) {
			Draw_Shape(*DSurface::Temp, *ColorSchemes[Houses[Flagged]->Scheme]->Converter, (ShapeSet const *)FileSystem::LoadFile("FLAGFLY.SHP", false), Frame % 14, raw_point, cliprect, ShapeFlags_Type(0xa00), NULL, 0, ZGRAD_GROUND, brightness);
		}

		BASECLASS::Draw_It(raw_point, cliprect);

		if (TargetingLaserTimer != 0) {
			Draw_Target_Laser();
		}
	}
}

// FUNCTION: 0x0073D450
bool UnitClass::Harvesting(void)
{
	CellClass * ptr = &Map[Get_Coord()];

	if (NavCom != NULL) {
		return(true);
	}

	if (Class->Harvester && Tiberium_Load() < 1 && ptr->Land_Type() == LAND_TIBERIUM) {
		if (Class->Weeder) {
			ptr->Reduce_Weed();
			Storage.Increase_Amount(1, 0);
			Animation.Set_Stage(0);
			Animation.Set_Rate(Rule->HarvesterLoadRate * 3);
		} else {
			TiberiumType tib = ptr->Tiberium_Type_Here();
			int reducer = (int)MIN(1.0f, Class->Capacity - Storage.Get_Total_Amount());
			reducer = ptr->Reduce_Tiberium(reducer);
			if (reducer <= 0) {
				return(false);
			}
			Storage.Increase_Amount(reducer, tib);
			Animation.Set_Stage(0);
			Animation.Set_Rate(Rule->HarvesterLoadRate);
		}
		return(true);
	}

	Animation.Set_Stage(0);
	Animation.Set_Rate(0);
	return(false);
}

// FUNCTION: 0x0073EFC0
int UnitClass::Do_MISSION_HUNT(void)
{
	if (Class->DeploysInto != NULL && (Rule->BuildConst.Is_In_List(Class->DeploysInto) || TarCom != NULL || House->FUN_0050B730())) {
		switch (Status) {
			case 0:
				if (Goto_Clear_Spot()) {
					if (Try_To_Deploy()) {
						Status = 1;
					}
				}
				break;

			case 1:
				if (!IsDeploying) {
					Status = 0;
				}
				break;
		}
	} else {
		return(BASECLASS::Do_MISSION_HUNT());
	}
	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: 0x007404B0
ActionType UnitClass::What_Action(Cell const & cell, bool check_fog, bool disallow_force) const
{
	if (!House->Is_Player_Control()) {
		return(ACTION_NONE);
	}

	CellClass * cellptr = &Map[cell];

	ActionType action = BASECLASS::What_Action(cell, check_fog, disallow_force);

	if (Combat_Damage() < 0 && House->Is_Player_Control() && action == ACTION_ATTACK) {
		action = ACTION_ATTACK_SUPPORT;
	}

	OverlayType overlay = OVERLAY_NONE;
	if (check_fog && Scen->Special.IsFogOfWar && cellptr->FoggedObjects != NULL) {
		for (int i = 0; i < cellptr->FoggedObjects->Count(); i++) {
			FoggedObjectClass * fogged = (*cellptr->FoggedObjects)[i];
			if (fogged->CanDraw && fogged->RTTI == RTTI_OVERLAY) {
				overlay = fogged->Overlay;
			}
		}
	}

	Coord coord = cell;
	coord.Z = Map.Get_Height_GL(coord);
	if (Map[coord].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	if (!Map.Is_Shrouded(coord)) {
		if (action == ACTION_MOVE) {
			LandType land = cellptr->Land_Type();
			if ((land == LAND_TIBERIUM || overlay != OVERLAY_NONE && OverlayTypes[overlay]->IsTiberium && check_fog) && Class->Harvester) {
				return(ACTION_HARVEST);
			}
			if ((land == LAND_WEEDS || overlay == OVERLAY_VEINS && cellptr->OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN && check_fog) && Class->Weeder) {
				return(ACTION_HARVEST);
			}
			if ((land == LAND_TIBERIUM || overlay != OVERLAY_NONE && OverlayTypes[overlay]->IsTiberium && check_fog) && Class->ResourceDestination && Class->ResourceGatherer) {
				return(ACTION_HARVEST);
			}
		}
	}

	if (action == ACTION_MOVE) {
		if (Map[cell].Has_Tunnel()) {
			if (Map[cell].Can_Enter_Tunnel((FootClass *)this)) {
				return(ACTION_ENTER_TUNNEL);
			}
			return(ACTION_NO_ENTER_TUNNEL);
		}
		if (Deployed || Deploying || Undeploying) {
			action = ACTION_NONE;
		}
	}

	if (House->Is_Player_Control() && Class->IsSimpleDeployer && Deployed) {
		if (action == ACTION_MOVE) {
			action = ACTION_NOMOVE;
		} else if (action == ACTION_ATTACK) {
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
	}

	if (action == ACTION_ATTACK && !Can_Attack_Now()) {
		action = ACTION_NONE;
	}

	return(action);
}

// FUNCTION: 0x00740810
int UnitClass::Do_MISSION_GUARD(void)
{
	if (SlaveManager != NULL && Rule->SlaveMinerKickFrameDelay + CurrentMissionStartTime < Frame && SlaveManager->ShouldWakeUpNow()) {
		SlaveManager->FUN_006B0CC0();
		return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
	}

	if (Class->Harvester || Class->Weeder) {
		if (!House->FUN_0050B730()) {
			for (int i = 0; i < Class->Dock.Count(); i++) {
				if (House->BQuantity.Value(Class->Dock[i]->HeapID) > 0) {
					if (Class->Harvester && House->IsTiberiumShort) {
						break;
					}
					QueueMission(MISSION_HARVEST, false);
					return(1);
				}
			}
		} else if (Class->Teleporter) {
			CellClass * cell = Get_Cell_Ptr();
			for (int i = 0; i < FACING_COUNT; i++) {
				BuildingClass * building = cell->Adjacent_Cell((FacingType)i).Cell_Building();
				if (building != NULL && building->Class->IsRefinery && building->House == House) {
					QueueMission(MISSION_HARVEST, false);
					return(1);
				}
			}
			if (Tiberium_Load() == 1 && Locomotion->Is_Moving()) {
				QueueMission(MISSION_HARVEST, false);
				return(1);
			}
		}
	}

	if (Class->DeploysInto != NULL && Rule->BuildConst.Is_In_List(Class->DeploysInto) && House->IsBaseBuilding && !House->FUN_0050B730()) {
		QueueMission(MISSION_UNLOAD, false);
		return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
	}

	if (Class->Weeder && IsDroppedFromTeam) {
		IsDroppedFromTeam = false;
		QueueMission(MISSION_HARVEST, false);
	}
	return(BASECLASS::Do_MISSION_GUARD());
}

// FUNCTION: 0x00740A90
int UnitClass::Do_MISSION_MOVE(void)
{
	IsHarvesting = false;

	if (Deployed || Deploying || Undeploying) {
		QueueMission(MISSION_GUARD, false);
		return(1);
	}

	if (!Door.Is_Door_Closed()) {
		APC_Close_Door();
	}

	return(BASECLASS::Do_MISSION_MOVE());
}

// FUNCTION: 0x00740B10
int UnitClass::Do_MISSION_PATROL(void)
{
	IsHarvesting = false;

	if (!Door.Is_Door_Closed()) {
		APC_Close_Door();
	}

	return(BASECLASS::Do_MISSION_PATROL());
}

// FUNCTION: 0x00740DF0
bool UnitClass::Flag_Attach(HousesType house)
{
	if (house != HOUSE_NONE && Flagged == HOUSE_NONE) {
		Flagged = house;
		Mark(MARK_CHANGE);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00740E20
bool UnitClass::Flag_Remove(void)
{
	if (Flagged != HOUSE_NONE) {
		Flagged = HOUSE_NONE;
		Mark(MARK_CHANGE);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00740E50
int UnitClass::Pip_Count(void) const
{
	return(BASECLASS::Pip_Count());
}

// FUNCTION: 0x00740E60
void UnitClass::APC_Close_Door(void)
{
	Door.Close_Door(Class->DeployTime);
}

// FUNCTION: 0x00740E80
void UnitClass::APC_Open_Door(void)
{
	if (!Locomotion->Is_Moving() && !IsRotating) {
		Door.Open_Door(Class->DeployTime);
	}
}

// FUNCTION: 0x00740EE0
InfantryTypeClass const * UnitClass::Crew_Type(void) const
{
	return(BASECLASS::Crew_Type());
}

// FUNCTION: 0x00740EF0
int UnitClass::Do_MISSION_REPAIR(void)
{
	BuildingClass * nearest = TryNearestDockBuilding((TypeList<BuildingTypeClass *> *)&Rule->RepairBay, 0, 0);

	IsHarvesting = false;

	if (nearest == NULL) {
		if (TryNearestDockBuilding((TypeList<BuildingTypeClass *> *)&Rule->RepairBay, 0, 1)) {
			Enter_Idle_Mode();
		}
	} else {
		if (Transmit_Message(RADIO_HELLO, nearest) == RADIO_ROGER) {
			QueueMission(MISSION_ENTER, false);
			return(1);
		}
	}

	return(Current_Mission_Control().Normal_Delay());
}

// FUNCTION: 0x00740F80
DirType UnitClass::Fire_Direction(void) const
{
	if (Class->IsTurretEquipped) {
		return(SecondaryFacing.Current());
	}

	return(BASECLASS::Fire_Direction());
}

// FUNCTION: 0x00740FD0
FireErrorType UnitClass::Can_Fire(AbstractClass * target, int which, bool ignoreRange) const
{
	if (DeathCounter >= 0) {
		return(FIRE_CANT);
	}

	if (SpawnManager != NULL && SpawnManager->CountLaunchingSpawns()) {
		return(FIRE_BUSY);
	}

	if (DirectRockerLinkedUnit != NULL) {
		return(FIRE_ILLEGAL);
	}

	FireErrorType fire = BASECLASS::Can_Fire(target, which, ignoreRange);

	if (fire == FIRE_OK || fire == FIRE_FACING) {
		bool buildable = Map[(Coord const &)PositionCoord].Can_Build_Here();
		if (Class->DeployToFire && Deploy_To_Fire() && !buildable) {
			return(FIRE_MUST_DEPLOY);
		}
	}

	if (fire == FIRE_OK) {
		if (IsTethered && FUN_0065AD40(0)->Fetch_RTTI() == RTTI_BUILDING) {
			return(FIRE_CANT);
		}

		WeaponTypeClass const * weapon = Get_Class_Weapon_Data(which)->Weapon;

		if (Combat_Damage() < 0) {
			TechnoClass const * techno = Dynamic_Cast<TechnoClass const *>((AbstractClass const *)target);
			if (techno == NULL || !techno->IsStrange() || techno->HealthRatio >= Rule->ConditionGreen) {
				return(FIRE_ILLEGAL);
			}
		}

		if (!Class->MobileFire && NavCom != NULL) {
			return(FIRE_MOVING);
		}

		if (!weapon->FireWhileMoving) {
			if (!Class->BalloonHover && NavCom != NULL) {
				return(FIRE_MOVING);
			}
			if (Class->BalloonHover && Locomotion->Is_Moving_Now()) {
				return(FIRE_MOVING);
			}
		}

		if ((weapon->UseSparkParticles || weapon->UseFireParticles) && NavCom != NULL) {
			return(FIRE_MOVING);
		}

		if (TemporalImUsing != NULL && NavCom != NULL) {
			return(FIRE_MOVING);
		}

		if (!IsFiring && IsRotating && weapon->Bullet->ROT == 0) {
			return(FIRE_ROTATING);
		}

		if (!weapon->OmniFire && !Class->LargeVisceroid && !Class->SmallVisceroid) {
			DirType turret;
			if (Class->IsTurretEquipped) {
				turret = SecondaryFacing.Current();
			} else {
				turret = PrimaryFacing.Current();
			}
			DirType rot = (weapon->Bullet->ROT ? DIR_STEP_16 : DIR_STEP_32);
			if (rot < (turret - Direction(target))) {
				return(FIRE_FACING);
			}
		}

		FireErrorType locomotion_fire = Locomotion->Can_Fire();
		if (locomotion_fire != FIRE_OK) {
			return(locomotion_fire);
		}
	}
	return(fire);
}

// FUNCTION: 0x00741340
BulletClass * UnitClass::Fire_At(AbstractClass * target, int which)
{
	BulletClass * bullet = NULL;
	WeaponTypeClass const * weap = Get_Class_Weapon_Data(which)->Weapon;
	if (weap == NULL) return(NULL);

	bool sync = false;
	if (which == 0) {
		int burst = BurstIndex % weap->Burst;
		int frame = burst < 2 ? Class->FiringSyncFrame[burst] : -1;
		sync = FiringSyncDelay != -1 && frame != -1;
	}

	if (Can_Fire(target, which, true) == FIRE_OK) {
		bullet = BASECLASS::Fire_At(target, which);

		if (bullet != NULL) {
			if (Class->MaxAmmo > 0 && Ammo < Class->MaxAmmo && !Class->IsManualReload) {
				StartReloading();
			}

			if (!sync && Class->FiringFrames > 0) {
				FiringSyncDelay = 2 * Class->FiringFrames - 1;
			}

			if (bullet->Class->Scalable) {
				bullet->InitScalable();
			}

			if (Class->DisguiseWhenStill) {
				DisguiseBlinkTimer = weap->DisguiseFakeBlinkTime;
			}
		}
	}

	return(bullet);
}

// FUNCTION: 0x00741490
ObjectTypeClass const * UnitClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: 0x007414A0
double UnitClass::Tiberium_Load(void) const
{
	if (Class->Harvester || Class->Weeder) {
		return(Storage.Get_Total_Amount() / (double)Class->Capacity);
	}
	return(0.0);
}

static inline TechnoClass * Techno_Of(AbstractClass * a)
{
	if (a == NULL) return(NULL);
	return((a->AbstractFlags & 1) ? (TechnoClass *)a : NULL);
}

static inline bool Is_Object(AbstractClass * a)
{
	return(a != NULL && (a->AbstractFlags & 2));
}

static inline ObjectClass * Object_Of(AbstractClass * a)
{
	if (a == NULL) return(NULL);
	return((a->AbstractFlags & 2) ? (ObjectClass *)a : NULL);
}

// FUNCTION: 0x007414E0
AbstractClass * UnitClass::Approach_Target(bool force)
{
	if (!House->FUN_0050B730() && TarCom != NULL && NavCom == NULL) {
		TechnoClass * target = Techno_Of(TarCom);
		if ((Class->IsCrusher || Has_Ability(ABILITY_CRUSHER)) && Distance(TarCom) < Rule->CrushDistance && target != NULL && ((ObjectClass *)target)->IsCrushable(this) && (Class->IsAutoCrush || !House->FUN_0050B730())) {
			if (!force) {
				Assign_Destination(TarCom, true);
			}
			return(TarCom);
		}
	}

	if (Class->BalloonHover) {
		WeaponTypeClass const * weap = Get_Class_Weapon_Data(What_Weapon_Should_I_Use(TarCom))->Weapon;
		if (weap != NULL && weap->Bullet != NULL && weap->Bullet->Vertical) {
			if (!force) {
				Assign_Destination(TarCom, true);
			}
			return(TarCom);
		}
	}

	if (Class->OmniCrusher && TarCom != NULL && (!House->IsHuman || !ShouldLoseTargetNow) && Is_Object(TarCom) && Object_Of(TarCom)->IsCrushable(this)) {
		if (!force) {
			Assign_Destination(TarCom, true);
		}
		return(TarCom);
	}

	return(BASECLASS::Approach_Target(false));
}

// FUNCTION: 0x007416A0
void UnitClass::Overrun_Square(Cell const & cell, bool threaten)
{
	CellClass * cellptr = &Map[cell];
	bool isbridge = false;

	if (cellptr->IsUnderBridge) {
		isbridge = IsOnBridge || (Map[Center_Coord().As_Cell()].Height == (cellptr->Height + BRIDGE_CELL_HEIGHT));
	}

	if (Class->IsCrusher || Has_Ability(ABILITY_CRUSHER)) {
		if (threaten) {
			if (isbridge) {
				if (cellptr->BridgeFlag.Composite & 0x1F) {
					cellptr->Incoming(COORD_NONE, true, false, true);
				}
			} else {
				if (cellptr->Flag.Composite & 0x1F) {
					cellptr->Incoming(COORD_NONE, true, false, false);
				}
			}
		} else {
			ObjectClass * object = cellptr->Cell_Occupier(isbridge);
			bool crushed = false;
			bool crushed_unit = false;
			while (object != NULL) {
				if (object->IsCrushable(this) && (!House->Is_Ally(object) || Class->IsTrain) && Relative_Distance(object->Center_Coord()) < CELL_LEPTON * 64 && !object->IsFalling) {
					if (object->Fetch_RTTI() == RTTI_INFANTRY && ((InfantryClass *)object)->Class->IsVehicleThief && ((InfantryClass *)object)->NavCom == this && !Class->IsTrain) {
						ObjectClass * next = object->Next;
						IsOwnedByPlayer = ((InfantryClass *)object)->IsOwnedByPlayer;
						Detach_All(false);
						Captured(((InfantryClass *)object)->House, true);
						object->Delete_Me();
						object = next;
					} else {
						ObjectClass * next = object->Next;
						crushed = true;
						if (object->Fetch_RTTI() == RTTI_UNIT) {
							crushed_unit = true;
						}
						VocClass::PlayAt(object->Class_Of()->CrushSound, PositionCoord, NULL);
						object->Destroy();
						object->RegisterDestruction(this);
						object->Mark(MARK_UP);
						object->Limbo();
						object->Delete_Me();
						object = next;
					}
				} else {
					object = object->Next;
				}
			}
			if (crushed) {
				Do_Uncloak(false);
				if (crushed_unit && RockingForwardsPerFrame == 0.0f) {
					RockingForwardsPerFrame = -0.05f;
				}
			}
		}
	}
}

// FUNCTION: 0x00743190
AbstractClass * UnitClass::Greatest_Threat(ThreatType threat, Coord const & coord, bool onlyenemy) const
{
	if (House->FUN_0050B730() && Class->DeployToFire) {
		return(NULL);
	}
	if (!(threat & (THREAT_INFANTRY|THREAT_VEHICLES|THREAT_BUILDINGS|THREAT_TIBERIUM|THREAT_CIVILIANS|THREAT_POWER|THREAT_FACTORIES|THREAT_BASE_DEFENSE|0x8000|0x10000))) {
		if (FUN_0070DC60() && !Class->IsGattling) {
			threat = ThreatType(threat | Get_Class_Weapon_Data(FUN_0070DD20())->Weapon->Allowed_Threats());
		} else {
			if (Get_Class_Weapon_Data(0)->Weapon != NULL) {
				threat = ThreatType(threat | Get_Class_Weapon_Data(0)->Weapon->Allowed_Threats());
			}
			if (Get_Class_Weapon_Data(1)->Weapon != NULL) {
				threat = ThreatType(threat | Get_Class_Weapon_Data(1)->Weapon->Allowed_Threats());
			}
		}
	}

	return(BASECLASS::Greatest_Threat(threat, coord, onlyenemy));
}

// FUNCTION: 0x00743270
void UnitClass::Read_INI(CCINIClass & ini)
{
	UnitClass * unit;
	int inhouse;
	UnitType classid;
	char buf[128];

	ini.Reset();

	DynamicVectorClass<int> followers;
	followers.Clear();

	int len = ini.Entry_Count(INI_NAME);

	for (int index = 0; index < len; index++) {
		char const * entry = ini.Get_Entry(INI_NAME, index);

		ini.Get_String(INI_NAME, entry, NULL, buf, sizeof(buf));

		inhouse = FUN_0050C170(strtok(buf, ","));
		if (inhouse != -1) {
			classid = UnitTypeClass::From_Name(strtok(NULL, ","));
			HouseClass * inhousep = Houses[inhouse];

			if (classid != UNIT_NONE) {
				unit = new UnitClass(UnitTypes[classid], inhousep);
				if (unit != NULL) {
					int strength = atoi(strtok(NULL, ","));

					Cell cell;
					if (NewINIFormat >= 4) {
						unsigned short x = atoi(strtok(NULL, ","));
						unsigned short y = atoi(strtok(NULL, ","));
						cell = Cell(x, y);
					} else {
						int c = atoi(strtok(NULL, ","));
						cell = Cell(c);
					}

					Coord coord = cell.As_Coord();

					Dir256 dir = (Dir256)atoi(strtok(NULL, ","));
					char * name = strtok(NULL, ",");
					MissionType mission = MissionClass::Mission_From_Name(name);

					TagTypeClass * tp = TagTypeClass::From_Name(strtok(NULL, ","));
					if (tp != NULL) {
						TagClass * tt = Find_Or_Make(tp);
						if (tt != NULL) {
							unit->Attach_Tag(tt);
						}
					}

					char * token = strtok(NULL, ",");
					if (token != NULL) {
						unit->Veterancy.From_Integer(atoi(token));
					}

					token = strtok(NULL, ",");
					if (token != NULL) {
						unit->Group = atoi(token);
					}

					token = strtok(NULL, ",");
					if (token != NULL) {
						unit->IsOnBridge = atoi(token) != 0;
						if (unit->IsOnBridge) {
							coord.Z = Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT;
						}
					}

					token = strtok(NULL, ",");
					if (token != NULL) {
						followers.Add(atoi(token));
					}

					token = strtok(NULL, ",");
					if (token != NULL) {
						unit->IsTeamRecruitable = atoi(token) != 0;
					}

					token = strtok(NULL, ",");
					if (token != NULL) {
						unit->IsAutocreateRecruitable = atoi(token) != 0;
					}

					if (unit->Unlimbo(coord, dir)) {
						double fs;
						unit->Strength = (fs = strength) * unit->Class->MaxStrength / 256.0;
						if (unit->Strength > unit->Class->MaxStrength - 3) unit->Strength = unit->Class->MaxStrength;
						if (unit->Strength == 0) unit->Strength = 1;
						unit->EstimatedHealth = unit->Strength;
						unit->QueueMission(mission, false);
						if (unit->Ready_To_Next_Mission()) {
							unit->Commence();
						}
					} else {
						delete unit;
					}
				}
			}
		}
	}

	for (int i = 0; i < Units.Count(); i++) {
		UnitClass * unit = Units[i];
		int followerid = followers[i];
		if ((UnitType)followerid != UNIT_NONE && (unsigned)followerid < (unsigned)Units.Count()) {
			UnitClass * follower = Units[followerid];
			unit->FollowingMe = follower;
			follower->IsFollowing = true;
		} else {
			unit->FollowingMe = NULL;
		}
	}
}

// FUNCTION: 0x007436E0
void UnitClass::Write_INI(CCINIClass & ini)
{
	ini.Clear(INI_NAME);

	for (int index = 0; index < Units.Count(); index++) {
		UnitClass * unit = Units[index];
		if (unit != NULL && !unit->IsInLimbo && unit->IsActive) {
			char uname[10];
			char buf[128];

			sprintf(uname, "%d", index);
			sprintf(buf, "%s,%s,%d,%d,%d,%d,%s,%s,%d,%d,%d,%d,%d,%d",
				(char const *)unit->House->IniName,
				(char const *)unit->Class->IniName,
				(int)(unit->HealthRatio * 256),
				unit->PositionCell.X,
				unit->PositionCell.Y,
				unit->PrimaryFacing.Current().As_Dir256(),
				MissionClass::Mission_Name(unit->Get_Mission()),
				(unit->Tag != NULL && unit->Tag->Class != NULL) ? (char const *)unit->Tag->Class->IniName : "None",
				unit->Veterancy.To_Integer(),
				unit->Group,
				unit->IsOnBridge,
				(unit->FollowingMe != NULL) ? Units.ID(unit->FollowingMe) : -1,
				unit->IsTeamRecruitable,
				unit->IsAutocreateRecruitable
				);
			ini.Put_String(INI_NAME, uname, buf);
		}
	}
}

// FUNCTION: 0x007438B0
int UnitClass::Credit_Load(void) const
{
	float total = Storage.Get_Total_Value();
	return((int)(House->FUN_0050C160() * total));
}

// FUNCTION: 0x007438F0
bool UnitClass::Should_Crush_It(TechnoClass const * it) const
{
	if (!(Class->IsCrusher || Has_Ability(ABILITY_CRUSHER)) || it == NULL || !((ObjectClass const *)it)->IsCrushable((TechnoClass *)this)) return(false);

	if ((Center_Coord() - it->Center_Coord()).Length() > Rule->CrushDistance) return(false);

	if ((House->FUN_0050B730() && !Rule->IsAutoCrush && !Class->IsAutoCrush) || House->Difficulty == DIFF_HARD) return(false);

	if (!House->FUN_0050B730() && House->IQ < Rule->IQCrush) return(false);

	if (it->IsDisguisedAs(House)) return(false);

	return(true);
}

// FUNCTION: 0x007440B0
bool UnitClass::Limbo(void)
{
	if (BASECLASS::Limbo()) {
		if (Flagged != HOUSE_NONE) {
			Houses[Flagged]->Flag_Attach(Get_Cell(), false);
			Flagged = HOUSE_NONE;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00744100
int UnitClass::Do_MISSION_GUARD_AREA(void)
{
	if (SlaveManager != NULL && Rule->SlaveMinerKickFrameDelay + CurrentMissionStartTime < Frame && SlaveManager->ShouldWakeUpNow()) {
		SlaveManager->FUN_006B0CC0();
		return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
	}
	return(BASECLASS::Do_MISSION_GUARD_AREA());
}

// FUNCTION: 0x00746DE0
HRESULT STDMETHODCALLTYPE UnitClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_UnitClass;
	return(S_OK);
}
