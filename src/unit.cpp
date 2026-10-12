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

// FUNCTION: 0x00746DE0
HRESULT STDMETHODCALLTYPE UnitClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_UnitClass;
	return(S_OK);
}
