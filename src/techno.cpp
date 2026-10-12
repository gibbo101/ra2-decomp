// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "techno.h"
#include "_bench.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "airctype.h"
#include "airstrike.h"
#include "anim.h"
#include "bench.h"
#include "blit.h"
#include "bound.h"
#include "bsurface.h"
#include "building.h"
#include "capmgr.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "data.h"
#include "dbgprint.h"
#include "dialog.h"
#include "draw.h"
#include "dsurface.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "isotile.h"
#include "isotype.h"
#include "language\language.h"
#include "laser.h"
#include "lightcon.h"
#include "mono.h"
#include "overtype.h"
#include "parasite.h"
#include "partsys.h"
#include "psystype.h"
#include "queue.h"
#include "revent.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "slavemgr.h"
#include "spawnmgr.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "team.h"
#include "techtype.h"
#include "temporal.h"
#include "tracker.h"
#include "unit.h"
#include "unittype.h"
#include "vanim.h"
#include "vein.h"
#include "voc.h"
#include "vox.h"
#include "voxdrsys.h"
#include "warhead.h"
#include "wave.h"
#include "weapon.h"
#include "bench.hh"
#include "tube.hh"

CDTimerClass<FrameTimerClass> TechnoClass::ActionLineTimer;

// FUNCTION: 0x006F2AF0
// FUNCTION: 0x006F2B20
CDTimerClass<SystemTimerClass> TechnoClass::TalkBubbleTimer;

// FUNCTION: 0x006F3270
TechnoTypeClass const * TechnoClass::Techno_Type_Class(void) const
{
	return((TechnoTypeClass const *)Class_Of());
}

// FUNCTION: 0x006F3280
bool TechnoClass::Can_Scatter(void) const
{
	if (Mission != MISSION_SLEEP && Mission != MISSION_STICKY && Mission != MISSION_UNLOAD && !TClass->IsTrain) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x006F32D0
bool TechnoClass::Is_Players_Army(void) const
{
	if (Strength <= 0) {
		return(false);
	}
	if (IsInLimbo || !IsInPlayfield) {
		return(false);
	}
	if (RTTI == RTTI_BUILDING) {
		return(false);
	}
	if (!IsDiscoveredByPlayer) {
		return(false);
	}
	if (!TClass->IsSelectable) {
		return(false);
	}
	if (!House->Is_Player_Control()) {
		return(false);
	}
	return(true);
}

static inline BuildingClass * Building_If_Building(AbstractClass * t)
{
	return(t->RTTI == RTTI_BUILDING ? (BuildingClass *)t : (BuildingClass *)0);
}

// FUNCTION: 0x006F3330
int TechnoClass::What_Weapon_Should_I_Use(AbstractClass * target) const
{
	bool inwater;
	if (TClass->HasMultipleTurrets() && !TClass->IsGattling) {
		return(CurrentWeaponNumber == -1 ? 0 : CurrentWeaponNumber);
	}

	if (CanOccupyFire()) {
		return(0);
	}

	WeaponTypeClass const * secondary = SecondaryWeapon;
	if (secondary == NULL) {
		return(0);
	}
	WeaponTypeClass const * primary = PrimaryWeapon;
	if (primary == NULL) {
		return(0);
	}
	if (secondary->NeverUse) {
		return(0);
	}
	if (target == NULL) {
		return(0);
	}

	if (InOpenToppedTransport && TClass->OpenTransportWeapon != -1) {
		return(TClass->OpenTransportWeapon);
	}

	TechnoClass * techno = (target->AbstractFlags & 1) ? (TechnoClass *)target : NULL;
	if (TClass->IsGattling) {
		int stage = CurrentGattlingStage;
		if (secondary->Bullet->IsAntiAircraft && techno != NULL && techno->In_Air()) {
			return(stage * 2 + 1);
		}
		return(stage * 2);
	}

	if (secondary->WarheadPtr->Airstrike) {
		if (target->RTTI == RTTI_BUILDING) {
			if (Building_If_Building(target)->Class->CanC4) {
				if (!Building_If_Building(target)->TClass->ResourceDestination) {
					goto success;
				}
				if (!Building_If_Building(target)->TClass->ResourceGatherer) {
					return(1);
				}
			}
		}
		return(0);
	}

	if (primary->WarheadPtr->IsLocomotor && techno != NULL && techno->RTTI == RTTI_BUILDING) {
		return(1);
	}

	if (secondary->DrainWeapon && techno != NULL && techno->TClass->Drainable && DrainTarget == NULL && !House->Is_Ally(techno)) {
		return(1);
	}

	if (secondary->AreaFire && Mission == MISSION_UNLOAD) {
		return(1);
	}

	if (RTTI == RTTI_BUILDING && ((BuildingClass *)this)->IsOverpowered) {
		return(1);
	}

	if (target->RTTI == RTTI_BUILDING && House->CanOverpower(target) && secondary->WarheadPtr->ElectricAssault && ((BuildingClass *)target)->Class->Overpowerable) {
		return(1);
	}

	if (RTTI == RTTI_AIRCRAFT && ((AircraftClass *)this)->IsKamikaze) {
		return(1);
	}

	if (target->RTTI == RTTI_CELL) {
		CellClass * cell = (CellClass *)target;
		bool proceed = true;
		if (cell->Land_Type() == LAND_WATER || !cell->On_Ground()) {
			proceed = cell->IsUnderBridge && TClass->Naval;
		}
		if (proceed && !cell->In_Air() && TClass->LandTargeting == 2) {
			return(1);
		}
	}

	if (techno != NULL) {
		if (secondary->WarheadPtr->Modifier[techno->TClass->Armor] == 0.0) {
			return(0);
		}
		if (primary->WarheadPtr->Modifier[techno->TClass->Armor] == 0.0) {
			return(1);
		}
		inwater = techno->Get_Cell_Ptr()->Land_Type() == LAND_WATER || techno->Get_Cell_Ptr()->Land_Type() == LAND_BEACH;
		if (techno->In_Air()) {
			inwater = false;
		}
		if (!techno->IsOnBridge && inwater) {
			int result = SelectNavalTargeting(target);
			if (result == -1) {
				result = 0;
			}
			return(result);
		}
		if (!techno->In_Air() && TClass->LandTargeting == 2) {
			return(1);
		}
		if (secondary->Bullet->IsAntiAircraft) {
			if (techno->In_Air()) {
success:
				return(1);
			}
		}
		return(0);
	}
	return(0);
}

// FUNCTION: 0x006F3820
int TechnoClass::SelectNavalTargeting(AbstractClass * target) const
{
	if (target == NULL) goto none;
	if (!(target->AbstractFlags & 1)) {
none:
		return(-1);
	}
	TechnoClass * techno = (TechnoClass *)target;
	unsigned char underwater = techno->TClass->Underwater;
	bool organic = techno->TClass->Organic;
	bool hover = techno->TClass->Speed == SPEED_HOVER;
	bool unnatural = techno->TClass->Unnatural;

	switch (TClass->NavalTargeting) {
		case 0:
			if (underwater && techno->Cloak != 0) {
				return(-1);
			}
			break;

		case 1:
			return(underwater != 0);

		case 2:
			return(underwater ? 0 : -1);

		case 3:
			if (organic || unnatural) {
				return(1);
			}
			break;

		case 4:
			if (!hover && !organic) {
				return(1);
			}
			break;

		case 6:
			return(-1);
		case 7:
			return(0);
	}
	return(0);
}

// FUNCTION: 0x006F3950
int TechnoClass::How_Many_Survivors(void) const
{
	if (TClass->IsCrew) {
		return(1);
	}
	return(0);
}

// FUNCTION: 0x006F3AD0
Coord TechnoClass::Get_FLH(int which, Coord base) const
{
	TechnoTypeClass const * tclass = TClass;
	int flhx = 0;
	int flhy = 0;
	int flhz = 0;

	if (which < 0) {
		which = -which;
		if (which <= 5) {
			TPoint3D<int> flh = TClass->AlternativeFLH[which - 1];
			flhx = flh.X;
			flhy = flh.Y;
			flhz = flh.Z;
		}
	} else {
		WeaponDataStruct const * weapon = Get_Class_Weapon_Data(which);
		flhx = weapon->FireFLH.X;
		flhy = weapon->FireFLH.Y;
		flhz = weapon->FireFLH.Z;
	}
	flhx += base.X;
	flhy += base.Y;
	flhz += base.Z;

	Matrix3D matrix;
	double angle;
	FootClass const * foot = static_cast<FootClass const *>(this);

	if (foot != NULL && (AbstractFlags & 4) && foot->Locomotion != NULL) {
		matrix = foot->Locomotion->Draw_Matrix(NULL);
		angle = Turret_Facing().As_Radian32() - PrimaryFacing.Current().As_Radian32();
	} else {
		matrix.Make_Identity();
		angle = Turret_Facing().As_Radian32();
	}

	matrix.Translate(tclass->TurretOffset, 0, 0);
	matrix.Rotate_Z(angle);
	matrix.Translate(flhx, flhy * (BurstIndex % 2 != 0 ? -1 : 1), flhz);

	Vector3 firevec = matrix * Vector3(0, 0, 0);
	Coord coord(firevec.X, -firevec.Y, firevec.Z);

	return(Render_Coord() + coord);
}

// FUNCTION: 0x006F3D60
Coord TechnoClass::Turret_Coord(int which) const
{
	TechnoTypeClass const * tclass = TClass;
	Matrix3D matrix;
	double angle;
	FootClass const * foot = static_cast<FootClass const *>(this);

	if (foot != NULL && (AbstractFlags & 4) && foot->Locomotion != NULL) {
		matrix = foot->Locomotion->Draw_Matrix(NULL);
		angle = Turret_Facing().As_Radian32() - PrimaryFacing.Current().As_Radian32();
	} else {
		matrix.Make_Identity();
		angle = Turret_Facing().As_Radian32();
	}

	matrix.Translate(tclass->TurretOffset, 0, 0);
	matrix.Rotate_Z(angle);

	Vector3 firevec = matrix * Vector3(0, 0, 0);
	Coord coord(firevec.X, -firevec.Y, firevec.Z);

	return(Render_Coord() + coord);
}

static inline void Set_Recoil(RecoilData & recoil, TurretControl const & control)
{
	recoil.Turret.Travel = control.Travel;
	recoil.Turret.CompressFrames = control.CompressFrames;
	recoil.Turret.RecoverFrames = control.RecoverFrames;
	recoil.Turret.HoldFrames = control.HoldFrames;
}

// FUNCTION: 0x006F3F40
void TechnoClass::Init(void)
{
	if (House != NULL) {
		if (PlayerPtr == House) {
			IsOwnedByPlayer = true;
		} else {
			IsOwnedByPlayer = false;
		}
	}

	if (TClass != NULL) {
		if (PlanningToken == NULL) {
			Create_Planning_Token();
		}

		if (TClass->Spawns != NULL) {
			SpawnManager = new SpawnManagerClass(this, TClass->Spawns, TClass->SpawnsNumber, TClass->SpawnRegenRate, TClass->SpawnReloadRate);
		}

		if (TClass->Enslaves != NULL) {
			SlaveManager = new SlaveManagerClass(this, TClass->Enslaves, TClass->SlavesNumber, TClass->SlaveRegenRate, TClass->SlaveReloadRate);
		}

		if (Get_Class_Weapon_Data(0)->Weapon != NULL && Get_Class_Weapon_Data(0)->Weapon->WarheadPtr->MindControl) {
			CaptureManager = new CaptureManagerClass(this, Get_Class_Weapon_Data(0)->Weapon->Attack, Get_Class_Weapon_Data(0)->Weapon->InfiniteMindControl);
		}

		if (RTTI != RTTI_BUILDING && Get_Class_Weapon_Data(0)->Weapon != NULL && Get_Class_Weapon_Data(0)->Weapon->WarheadPtr->Parasite) {
			static_cast<FootClass *>(this)->ParasiteImUsing = new ParasiteClass(static_cast<FootClass *>(this));
		}

		if (Get_Class_Weapon_Data(0)->Weapon != NULL && Get_Class_Weapon_Data(0)->Weapon->WarheadPtr->Temporal) {
			TemporalImUsing = new TemporalClass(this);
			TemporalImUsing->WarpPerStep = Get_Class_Weapon_Data(0)->Weapon->Attack;
		}

		if (TClass->AirstrikeTeam > 0) {
			Airstrike = new AirstrikeClass(this);
		}

		if (TClass->CanDisguise && TClass->PermaDisguise) {
			Disguised = true;
			DisguisedAsHouse = House;
			if (House->ActLike == 0) {
				Disguise = (ObjectTypeClass *)Rule->AlliedDisguise;
			} else if (House->ActLike == 1) {
				Disguise = (ObjectTypeClass *)Rule->SovietDisguise;
			} else {
				Disguise = (ObjectTypeClass *)Rule->ThirdDisguise;
			}
		}

		Set_Recoil(TurretRecoil, TClass->TurretAnimData);
		Set_Recoil(BarrelRecoil, TClass->BarrelAnimData);
		IsTurretRotateSoundPlaying = 1;
		TurretIsRotating = 0;
	}
}

// FUNCTION: 0x00708C10
FacingType TechnoClass::Desired_Load_Dir(ObjectClass * , Cell & moveto) const
{
	moveto = CELL_NONE;
	return(FACING_N);
}

// FUNCTION: 0x0070D150
void TechnoClass::Reset_Action_Line_Timer(void)
{
	ActionLineTimer = 25;
}
