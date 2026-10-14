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
#include "bomblist.h"
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
#include "ebolt.h"
#include "event.h"
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
#include "notify.h"
#include "parasite.h"
#include "partsys.h"
#include "psystype.h"
#include "queue.h"
#include "radbeam.h"
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
#include "stbuffer.h"
#undef LoadString
#include "stringtable.h"
#include "_voxel.h"
#include "voxlib.h"
#include "warhead.h"
#include "wave.h"
#include "weapon.h"
#include "bench.hh"
#include "tube.hh"

namespace Unsorted { extern int CurrentFrame; }



// FUNCTION: 0x006F2AD0 $E225
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
Coord TechnoClass::Fire_Coord(int which, Coord base) const
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

// FUNCTION: 0x006F4300
TechnoClass::TechnoClass(NoInitClass const & x) :
	BASECLASS(x),
	Flashing(x),
	Animation(x),
	Cargo(x),
	Veterancy(x),
	IdleTimer(x),
	RadarFlashTimer(x),
	TargetingTimer(x),
	IronCurtainTimer(x),
	IronTintTimer(x),
	InfantryBlinkTimer(x),
	DisguiseBlinkTimer(x),
	ReloadTimer(x),
	CloakDelay(x),
	Arm(x),
	Storage(x),
	Door(x),
	BarrelPitch(x),
	PrimaryFacing(x),
	SecondaryFacing(x),
	TargetingLaserTimer(x),
	CurrentTargetThreatValues(0, 0),
	CurrentTargets(0, 0),
	AttackedTargets(0, 0)
{
	bool found = false;
	for (int i = 0; i < Array.Count(); i++) {
		if (Array[i] == this) {
			found = true;
			break;
		}
	}
	TurretRotateSoundController.FUN_00405BE0();
	GattlingSoundController.FUN_00405BE0();
	UnusedGattlingSoundController.FUN_00405BE0();
	QueuedVoiceSoundController.FUN_00405BE0();
	QueuedVoiceIndex = -1;
	unknown_4F4 = -1;
	if (!found) {
		Array.Add(this);
	}
}


// FUNCTION: 0x006F4500
TechnoClass::~TechnoClass(void)
{
	if (PlanningToken != NULL) {
		PlanningToken->FUN_00636310();
		PlanningToken = NULL;
	}

	if (CaptureManager != NULL) {
		delete CaptureManager;
		CaptureManager = NULL;
	}

	if (SpawnManager != NULL) {
		delete SpawnManager;
		SpawnManager = NULL;
	}

	if (SlaveManager != NULL) {
		SlaveManager->Killed(NULL, 0);
		delete SlaveManager;
		SlaveManager = NULL;
	}

	if (TemporalImUsing != NULL) {
		delete TemporalImUsing;
		TemporalImUsing = NULL;
	}

	if (Airstrike != NULL && Airstrike->Owner == this) {
		delete Airstrike;
		Airstrike = NULL;
	}

	TurretRotateSoundController.End();
	TurretRotateSoundController.FUN_00405C00();
	GattlingSoundController.End();
	GattlingSoundController.FUN_00405C00();
	UnusedGattlingSoundController.End();
	UnusedGattlingSoundController.FUN_00405C00();
	QueuedVoiceSoundController.FUN_00405C00();

	House = NULL;
	IsOwnedByPlayer = false;
	Array.Delete(this);
	PointerExpiredNotification::NotifyInvalidHouse.Delete(this);

	if (BehindAnim != NULL) {
		BehindAnim->Delete_Me();
		BehindAnim = NULL;
	}

	if (DeployAnim != NULL) {
		DeployAnim->Delete_Me();
		DeployAnim = NULL;
	}
}

// FUNCTION: 0x006F47A0
int TechnoClass::Time_To_Build(void) const
{
	int val = Class_Of()->Time_To_Build();
	val = val * House->FUN_0050C0A0(Techno_Type_Class());
	val = val * Techno_Type_Class()->BuildTimeMultiplier;

	float power = House->Power_Fraction();
	float modifier = 1.0f - (1.0f - power) * Rule->LowPowerPenaltyModifier;
	if ((double)Rule->MinLowPowerProductionSpeed >= (double)modifier) {
		modifier = Rule->MinLowPowerProductionSpeed;
	}
	if (power < 1.0f && (double)Rule->MaxLowPowerProductionSpeed <= (double)modifier) {
		modifier = Rule->MaxLowPowerProductionSpeed;
	}
	if (modifier == 0.0f) {
		modifier = 0.01f;
	}
	val = val / modifier;

	bool naval = false;
	if (Fetch_RTTI() == RTTI_UNIT) {
		TechnoTypeClass const * type = Techno_Type_Class();
		if (type != NULL) {
			naval = type->Naval;
		}
	}
	int count = House->FUN_00500910(Fetch_RTTI(), naval);
	float multiple = Rule->MultipleFactory;
	if (multiple > 0.0f && count - 1 > 0) {
		for (int i = count - 1; i > 0; i--) {
			val = val * multiple;
		}
	}

	if (Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass const *)this)->Class->IsWall) {
		return(val * Rule->WallBuildSpeedCoefficient);
	}
	return(val);
}

// FUNCTION: 0x006F4960
bool TechnoClass::Revealed(HouseClass * house)
{
	if (house == PlayerPtr) {
		if (IsDiscoveredByPlayer) {
			return(false);
		}
	} else {
		if (IsDiscoveredByComputer) {
			return(false);
		}
		IsDiscoveredByComputer = true;
	}

	if (BASECLASS::Revealed(house)) {
		if (!house->Is_Human_Player() && Get_Mission() == MISSION_AMBUSH) {
			QueueMission(MISSION_HUNT, false);
		}

		if (house == PlayerPtr) {
			IsDiscoveredByPlayer = true;
			House->RecalcPower = true;
			House->RecalcRadar = true;

			if (!IsOwnedByPlayer) {
				if (!ScenarioInit && Tag != NULL) {
					Tag->Spring(TEVENT_DISCOVERED, this);
				}

				House->IsDiscovered = true;
			}
		} else {
			IsDiscoveredByComputer = true;
		}
		return(true);
	}
	return(false);
}


// FUNCTION: 0x006F4A40
void TechnoClass::Hidden(void)
{
	if (IsDiscoveredByPlayer) {
		if (!House->Is_Human_Player()) {
			IsDiscoveredByPlayer = false;
		}
	}
}


// FUNCTION: 0x006F4A70
bool TechnoClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		if (IsTethered) {
			Transmit_Message(RADIO_REDRAW);
		}
		return(true);
	}
	return(false);
}

static inline AircraftClass * Aircraft_Of(AbstractClass * object)
{
	if (object == NULL) {
		return(NULL);
	}
	return((object->Fetch_RTTI() == RTTI_AIRCRAFT) ? (AircraftClass *)object : NULL);
}

static inline bool Is_Foot_Object(AbstractClass const * object)
{
	if (object == NULL) {
		return(false);
	}
	unsigned char flag = (object->AbstractFlags >> 2) & 1;
	return(flag != 0);
}

// FUNCTION: 0x006F4AB0
RadioMessageType TechnoClass::Receive_Message(RadioClass * from, RadioMessageType message, int & param)
{
	WeaponDataStruct const * weapon;
	int cost;
	int step;
	switch (message) {
		case RADIO_ATTACK_THIS:
			weapon = GetTurretWeapon();
			if (weapon != NULL && weapon->Weapon != NULL) {
				Assign_Target((AbstractClass *)param);
				QueueMission(MISSION_ATTACK, false);
				return(RADIO_ROGER);
			}
			break;

		case RADIO_TETHER:
			if (RTTI == RTTI_AIRCRAFT && Aircraft_Of(this)->Class->IsAirportBound) {
				break;
			}
			if (!IsTethered) {
				IsTethered = true;
				Transmit_Message(RADIO_TETHER, from);
				return(RADIO_ROGER);
			}
			break;

		case RADIO_UNTETHER:
			if (IsTethered) {
				IsTethered = false;
				Transmit_Message(RADIO_UNTETHER, from);
				return(RADIO_ROGER);
			}
			break;

		case RADIO_ALT_TETHER:
			if (!IsAlternativeTether) {
				IsAlternativeTether = true;
				Transmit_Message(RADIO_ALT_TETHER, from);
				return(RADIO_ROGER);
			}
			break;

		case RADIO_ALT_UNTETHER:
			if (IsAlternativeTether) {
				IsAlternativeTether = false;
				Transmit_Message(RADIO_ALT_UNTETHER, from);
				return(RADIO_ROGER);
			}
			break;

		case RADIO_UNLOADED:
			Transmit_Message(RADIO_UNTETHER, from);
			return(Transmit_Message(RADIO_OVER_OUT, from));

		case RADIO_OVER_OUT:
			if (IsTethered && ((TechnoClass *)from)->IsTethered) {
				Transmit_Message(RADIO_UNTETHER, from);
			}
			BASECLASS::Receive_Message(from, message, param);
			return(RADIO_ROGER);

		case RADIO_UNLOAD:
		case RADIO_BACKUP_NOW:
		case RADIO_HOLD_STILL:
			Transmit_Message(RADIO_TETHER, from);
			BASECLASS::Receive_Message(from, message, param);
			return(RADIO_ROGER);

		case RADIO_RELOAD:
			if (Ammo == Techno_Type_Class()->MaxAmmo) {
				return(RADIO_NEGATIVE);
			}
			Ammo++;
			return(RADIO_ROGER);

		case RADIO_REPAIR:
			if (HealthRatio < Rule->ConditionGreen) {
				cost = Techno_Type_Class()->Repair_Cost();
				step = Techno_Type_Class()->Repair_Step();
				if (step <= 1) {
					step = 1;
				}

				if (House->Available_Money() >= cost) {
					if (cost != 0) {
						House->Spend_Money(cost);
					}
					Strength += step;
					EstimatedHealth += step;

					if (Is_Foot_Object(this) && ((FootClass *)this)->ParasiteEatingMe != NULL) {
						((FootClass *)this)->ParasiteEatingMe->ParasiteImUsing->SuppressionTimer = 50;
						((FootClass *)this)->ParasiteEatingMe->ParasiteImUsing->ExitUnit();
					}

					if (HealthRatio > Rule->ConditionYellow || HeightAGL < -10) {
						if (ParticleSystems[ATTACHED_PARTICLE_DAMAGE] != NULL) {
							ParticleSystems[ATTACHED_PARTICLE_DAMAGE]->Delete_Me();
						}
					}

					if (HealthRatio < Rule->ConditionGreen) {
						return(RADIO_ROGER);
					}
					EstimatedHealth = Strength = Techno_Type_Class()->MaxStrength;
					return(RADIO_ALL_DONE);
				}
				return(RADIO_CANT);
			}
			return(RADIO_NEGATIVE);

		default:
			break;
	}
	return(BASECLASS::Receive_Message(from, message, param));
}

// FUNCTION: 0x006F4EB0
void TechnoClass::Sensed(void)
{
	int i;

	CellClass & cell = Map[Center_Coord().As_Cell()];
	if (Cloak == CLOAKED && PlayerPtr != NULL && House != PlayerPtr && !cell.Is_Sensed(PlayerPtr->HeapID)) {
		Unselect();
	}

	if (cell.Is_Cloaked(House->HeapID) && Is_Ready_To_Cloak()) {
		DynamicVectorClass<TechnoClass *> targeting_me;
		for (i = Array.Count() - 1; i >= 0; i--) {
			TechnoClass * tech = Array[i];
			if (tech->TarCom == this) {
				if (Map[Center_Coord()].Is_Sensed(tech->House->HeapID) || tech->House == House) {
					targeting_me.Add(tech);
				}
			}
		}
		Do_Cloak();
		for (i = targeting_me.Count() - 1; i >= 0; i--) {
			targeting_me[i]->Assign_Target(this);
		}
	}
}


// FUNCTION: 0x006F5090
void TechnoClass::Per_Cell_Process(PCPType why)
{
	if (why == PCP_END) {
		if (TemporalImUsing != NULL && TemporalImUsing->Target != NULL) {
			TemporalImUsing->LetGo();
		}

		Cell cell = Center_Coord().As_Cell();

		Sensed();

		if (Tag != NULL) {
			Tag->Spring(TEVENT_NEAR_WAYPOINT, this);
		}

		if (!IsInPlayfield && Map.In_Local_Radar(cell, true)) {
			IsInPlayfield = true;
		}

		if (!IsDiscoveredByPlayer && Map[cell].IsVisible) {
			Revealed(PlayerPtr);
		}

		Map[cell].Trigger_Veins();
	}
}

// FUNCTION: 0x006F5EF0
void TechnoClass::Draw_Double_Selection_Bracket(Coord const & coord1, Coord const & coord2, int color)
{
	Coord c1 = (coord1 + coord1 + coord1 + coord2) / 4;
	Coord c2 = (coord2 + coord2 + coord2 + coord1) / 4;

	if (coord1.Z > c1.Z) {
		TacticalMap->Draw_3D_Line(coord1, c1, color, 0);
	} else {
		TacticalMap->Draw_3D_Line(c1, coord1, color, 0);
	}

	if (coord2.Z > c2.Z) {
		TacticalMap->Draw_3D_Line(coord2, c2, color, 0);
	} else {
		TacticalMap->Draw_3D_Line(c2, coord2, color, 0);
	}
}

// FUNCTION: 0x006F6030
void TechnoClass::Draw_Single_Selection_Bracket(Coord const & coord1, Coord const & coord2, int color)
{
	Coord c1 = (coord1 + coord1 + coord1 + coord2) / 4;

	if (coord1.Z > c1.Z) {
		TacticalMap->Draw_3D_Line(coord1, c1, color, 0);
	} else {
		TacticalMap->Draw_3D_Line(c1, coord1, color, 0);
	}
}

// FUNCTION: 0x006F60C0
void TechnoClass::vt_entry_448(unsigned, unsigned)
{
}

// FUNCTION: 0x006F6AC0
bool TechnoClass::Limbo(void)
{
	if (!IsInLimbo && TClass != NULL) {
		if (TClass->RevealToAll) {
			vt_entry_48C(0, 0, 1, PlayerPtr);
		} else {
			vt_entry_48C(0, 0, 0, 0);
		}
		Map.RevealArea3(Coord(Position), LastSightRange - 3, LastSightRange + 3, false);
		if (TClass->GapGenerator) {
			DestroyGap();
		}
		if (TClass->BombSight != 0) {
			BombListClass::Instance.RemoveDetector(this);
		}
		if (TClass->OpenTopped) {
			TechnoClass * passenger = Cargo.Attached_Object();
			if (passenger != NULL) {
				do {
					passenger->Assign_Target(NULL);
					passenger = (TechnoClass *)passenger->Next;
				} while (passenger != NULL && (passenger->AbstractFlags & 4));
			}
		}
		House->Tracking_Active_Remove(this, false);
		if (Risk() > 0) {
			if (this != NULL && (AbstractFlags & 4)) {
				CellClass * cell = &Map[((FootClass *)this)->LastAdjacencyCell];
				if (cell != NULL) {
					cell->Adjust_Threat(Owner(), -ThreatPosed);
					ThreatPosed = 0;
				}
			} else {
				CellClass * cell = Get_Cell_Ptr();
				if (cell != NULL) {
					cell->Adjust_Threat(Owner(), -ThreatPosed);
					ThreatPosed = 0;
				}
			}
		}
		if (IsRadarTracked) {
			Radar_Untrack();
		}
		TurretRotateSoundController.EndLooping();
		IsTurretRotateSoundPlaying = true;
		TurretIsRotating = 0;
		GattlingSoundController.EndLooping();
		IsGattlingSoundPlaying = false;
		unknown_4BC = 0;
		UnusedGattlingSoundController.EndLooping();
		IsUnusedGattlingSoundPlaying = false;
		unknown_4D8 = 0;
	}
	return(BASECLASS::Limbo());
}

static inline int Sight_Bonus_For(Coord const & coord)
{
	return(10 * (coord.Z / Rule->LeptonsPerSightIncrease));
}

// FUNCTION: 0x006F6CA0
bool TechnoClass::Unlimbo(Coord const & coord, Dir256 dir)
{
	if (BASECLASS::Unlimbo(coord, dir)) {
		IsInPlayfield = Map.In_Local_Radar(coord.As_Cell());
		if (!IsActive) {
			return(true);
		}
		UpdateSight(0, 0, 0, 0, 0);
		Map.RevealArea3(Coord(Position), 0, LastSightRange + 3, false);
		if (TClass->BombSight != 0) {
			BombListClass::Instance.AddDetector(this);
		}
		House->Tracking_Active_Add(this, false);
		PrimaryFacing.Set(dir);
		BarrelPitch.Set(DIR_E << 8);
		BarrelPitch.Set_Desired(DirType(DIR_E << 8) - (TClass->FireAngle << 8));
		IsTurretRotateSoundPlaying = true;
		TurretIsRotating = 0;
		if (RTTI == RTTI_INFANTRY) {
			UnlimboingInfantry = true;
		}
		Enter_Idle_Mode(true);
		UnlimboingInfantry = false;
		if (Ready_To_Next_Mission()) {
			Commence();
		}
		SightIncrease = Sight_Bonus_For(coord);
		if (!Map.In_Local_Radar(coord.As_Cell())) {
			IsDiscoveredByPlayer = false;
		}
		CellClass * cell = Get_Cell_Ptr();
		if (cell != NULL) {
			cell->Adjust_Threat(Owner(), ThreatPosed = Risk());
		}
		RadarPos = Map.Coord_To_Radar_Pixel(PositionCoord, true);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006F6F40
int TechnoClass::Get_Sight_Bonus(Coord const & coord)
{
	return(10 * (coord.Z / Rule->LeptonsPerSightIncrease));
}

// FUNCTION: 0x006F6F60
int TechnoClass::Elevation_Distance(AbstractClass const * target) const
{
	if (!On_Ground() || !target->On_Ground()) {
		return(0);
	}
	Cell mycell = Center_Coord().As_Cell();
	Cell theircell = target->Center_Coord().As_Cell();
	CellClass * mine = &Map[mycell];
	CellClass * theirs = &Map[theircell];
	int diff = (mine->GetLevel() - theirs->GetLevel() < 0) ? 0 : mine->GetLevel() - theirs->GetLevel();
	double bonus = MIN((diff / Rule->ElevationIncrement) * Rule->ElevationIncrementBonus, Rule->ElevationBonusCap);
	int height = LEVEL_LEPTON_H * diff;
	int range = (int)bonus << 8;
	return((int)fastmath::sqrt((double)(range * range + height * height)));
}

// FUNCTION: 0x006F70E0
int TechnoClass::Elevation_Range_Bonus(AbstractClass const * target) const
{
	if (!On_Ground() || !target->On_Ground()) {
		return(0);
	}
	Cell mycell = Center_Coord().As_Cell();
	Cell theircell = target->Center_Coord().As_Cell();
	CellClass * mine = &Map[mycell];
	CellClass * theirs = &Map[theircell];
	int diff = (mine->GetLevel() - theirs->GetLevel() < 0) ? 0 : mine->GetLevel() - theirs->GetLevel();
	double bonus = MIN((diff / Rule->ElevationIncrement) * Rule->ElevationIncrementBonus, Rule->ElevationBonusCap);
	return((int)bonus << 8);
}

// FUNCTION: 0x006F7660
bool TechnoClass::TriggersCellInset(AbstractClass * target)
{
	if (IsAreaFire()) {
		WarheadTypeClass const * warhead = GetDeployWeapon()->Weapon->WarheadPtr;
		if (warhead != NULL) {
			float inset = warhead->SpreadFactor;
			inset -= warhead->CellInset;
			Cell mine = Center_Coord().As_Cell();
			Cell theirs = target->Center_Coord().As_Cell();
			if (mine.Distance_To(theirs) <= inset) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: 0x006F7780
bool TechnoClass::IsCloseEnoughToAttack(AbstractClass * target) const
{
	return(In_Range(target, What_Weapon_Should_I_Use(target)));
}

// FUNCTION: 0x006F77B0
bool TechnoClass::In_Range(AbstractClass * target, int which) const
{
	if (target == NULL) {
		return(true);
	}
	Coord coord = Center_Coord();
	WeaponTypeClass * weapon = Get_Class_Weapon_Data(which)->Weapon;
	if (weapon != NULL && weapon->CellRangefinding) {
		coord = Map[Center_Coord().As_Cell()].Center_Coord();
		if (IsOnBridge) {
			coord.Z += BRIDGE_LEPTON_HEIGHT;
		}
	}
	if (In_Air()) {
		coord.Z = target->Center_Coord().Z;
	}
	return(In_Weapon_Range(coord, target, weapon));
}

// FUNCTION: 0x006F78D0
bool TechnoClass::InAuxiliarySearchRange(AbstractClass * target) const
{
	int which = GetSecondaryWeaponIndex();
	WeaponDataStruct const * data = Get_Class_Weapon_Data(which);
	if (data != NULL && data->Weapon != NULL && data->Weapon->NeverUse) {
		return(In_Range(target, which));
	}
	return(IsCloseEnoughToAttack(target));
}

// FUNCTION: 0x006F7930
bool TechnoClass::IsCloseEnoughToAttackCoords(Coord const & coord) const
{
	AbstractClass * target = &Map[coord];
	return(In_Range(target, What_Weapon_Should_I_Use(target)));
}

// FUNCTION: 0x006F7970
bool TechnoClass::In_Range(Coord const & coord, int which) const
{
	return(In_Range(&Map[coord], which));
}

// FUNCTION: 0x006F9DB0
HousesType TechnoClass::Owner(void) const
{
	return(House->HeapID);
}

// FUNCTION: 0x006F9DC0
HouseClass * TechnoClass::Owner_HouseClass(void) const
{
	return(House);
}

// FUNCTION: 0x006F9DD0
void TechnoClass::Clicked_As_Target(int count)
{
	if (!IsDisguisedAs(PlayerPtr) || GetDisguiseHouse(true)) {
		Flashing.FlashCount = count;
	}
}

// FUNCTION: 0x006F9E10
bool TechnoClass::Is_Voxel_Loaded(void) const
{
	if (TClass->Voxel.VoxLib == NULL) {
		return(false);
	}
	if (TClass->Voxel.VoxLib->Load_Failed()) {
		return(false);
	}
	return(true);
}

// FUNCTION: 0x006FB010
void TechnoClass::Reload(void)
{
	int max = TClass->MaxAmmo;
	if (max != -1 && Ammo < max) {
		if (ReloadTimer == 0) {
			Ammo++;
			Mark(MARK_CHANGE);
			StartReloading();
		}
	}
}

// FUNCTION: 0x006FB080
void TechnoClass::StartReloading(void)
{
	int max = TClass->MaxAmmo;
	if (Ammo < max) {
		if (Ammo == 0 && TClass->EmptyReload != -1) {
			ReloadTimer = TClass->EmptyReload;
			return;
		}
		int steps = (TClass->PipWrap == 0) ? 1 : Ammo / TClass->PipWrap;
		int delay = TClass->ReloadIncrement * steps * steps;
		ReloadTimer = TClass->Reload + delay;
	}
}

// FUNCTION: 0x006FB170
void TechnoClass::CreateGap(void)
{
	if (PlayerPtr != NULL && !GeneratingGap && IsPowerOnline()) {
		if (GapRadius == 0) {
			GapRadius = TClass->GapRadiusInCells;
		}
		GeneratingGap = true;

		Coord coord = Position;
		coord.X += (Tactical::Z_Lepton_To_Pixel(coord.Z) / -30) << 8;
		coord.Y += (Tactical::Z_Lepton_To_Pixel(coord.Z) / -30) << 8;
		Cell center = coord.As_Cell();

		int radius = GapRadius;
		for (int dx = -2 - radius; dx < radius + 2; dx++) {
			for (int dy = -2 - radius; dy < radius + 2; dy++) {
				if (dx * dx + dy * dy < (radius + 1) * (radius + 1)) {
					Cell cell = center + Cell(dx, dy);
					if (!Owner_HouseClass()->Is_Player_Control() && !House->Is_Ally(PlayerPtr) && !PlayerPtr->IsDefeated) {
						if (Map[cell].ShroudCounter != 1) {
							if (Map[cell].ShroudCounter >= 0) {
								++Map[cell].ShroudCounter;
							}
						}
						++Map[cell].GapsCoveringThisCell;
						if (Map[cell].ShroudCounter >= 1) {
							Map[cell].IsVisible = false;
							Map[cell].IsMapped = false;
						}
					}
					if (Owner_HouseClass()->Is_Player_Control() || House->Is_Ally(PlayerPtr) || PlayerPtr->IsDefeated) {
						++Map[cell].unknown_13C;
					}
				}
			}
		}

		if (PlayerPtr != NULL) {
			PlayerPtr->IsVisionary = false;
		}
		Map.sub_657CE0();
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
	}
}

// FUNCTION: 0x006FB470
void TechnoClass::DestroyGap(void)
{
	if (PlayerPtr != NULL && GeneratingGap) {
		if (GapRadius == 0) {
			GapRadius = TClass->GapRadiusInCells;
		}
		GeneratingGap = false;

		Coord coord = Position;
		coord.X += (Tactical::Z_Lepton_To_Pixel(coord.Z) / -30) << 8;
		coord.Y += (Tactical::Z_Lepton_To_Pixel(coord.Z) / -30) << 8;
		Cell center = coord.As_Cell();

		int radius = GapRadius;
		for (int dx = -2 - radius; dx < radius + 2; dx++) {
			for (int dy = -2 - radius; dy < radius + 2; dy++) {
				if (dx * dx + dy * dy < (radius + 1) * (radius + 1)) {
					Cell cell = center + Cell(dx, dy);
					if (!Owner_HouseClass()->Is_Player_Control() && !House->Is_Ally(PlayerPtr) && !PlayerPtr->IsDefeated) {
						--Map[cell].GapsCoveringThisCell;
						if (PlayerPtr->SpySatActive && Map[cell].GapsCoveringThisCell <= 0) {
							--Map[cell].ShroudCounter;
							if (Map[cell].ShroudCounter <= 0) {
								Map[cell].IsMapped = true;
								Map[cell].IsVisible = true;
							}
						}
					}
					if (Owner_HouseClass()->Is_Player_Control() || House->Is_Ally(PlayerPtr) || PlayerPtr->IsDefeated) {
						--Map[cell].unknown_13C;
					}
				}
			}
		}

		if (PlayerPtr != NULL) {
			PlayerPtr->IsVisionary = false;
		}
		Map.sub_657CE0();
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
	}
}

inline bool TechnoClass::Has_Ability(AbilityType ability) const
{
	if (Veterancy.Is_Veteran() || Veterancy.Is_Elite()) {
		TechnoTypeClass const * ttype = TClass;
		if (Veterancy.Is_Veteran() && ttype->VeteranAbilities[ability]) {
			return(true);
		}
		if (Veterancy.Is_Elite() && (ttype->VeteranAbilities[ability] || ttype->EliteAbilities[ability])) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x006FB740
void TechnoClass::Cloaking_AI(bool)
{
	if (Cloak == UNCLOAKED) {
		if ((Is_Allowed_To_Recloak() && !Is_Immobilized()) || Has_Ability(ABILITY_CLOAK)) {
			RadioClass * radio = FUN_0065AD30(0);
			if (radio == NULL || !(radio->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)radio)->Class->IsWeaponsFactory)) {
				CloakingDevice.Graphic_Logic();
				if (Is_Ready_To_Cloak()) {
					if (HealthRatio > Rule->ConditionRed) {
						Do_Cloak();
					} else {
						if (Scen->RandomNumber(0, 99) < 4) {
							Do_Cloak();
						}
					}
				}
			}
		}
	} else {
		CloakingDevice.Graphic_Logic();
		if (CloakingDevice.Fetch_Stage() < 0) {
			CloakingDevice.Set_Stage(0);
		}
		switch (Cloak) {
			case UNCLOAKING:
			{
				Mark(MARK_CHANGE);
				switch (Visual_Character(true)) {
					case VISUAL_NORMAL:
						CloakingDevice.Set_Rate(0);
						CloakingDevice.Set_Stage(0);
						Cloak = UNCLOAKED;
						CloakDelay = Rule->CloakDelay * TICKS_PER_MINUTE;
						Mark(MARK_CHANGE);
						break;

					case VISUAL_INDISTINCT:
						if (Is_Ready_To_Cloak()) {
							Do_Cloak(true);
						}
				}
				break;
			}
			case CLOAKING:
				Mark(MARK_CHANGE);
				if (!CloakingDevice.Fetch_Rate()) {
					CloakingDevice.Set_Rate(1);
				}
				switch (Visual_Character(true)) {
					case VISUAL_DARKEN:
						if (HealthRatio <= Rule->ConditionRed && Scen->RandomNumber(0, 99) < 10) {
							Do_Uncloak(true);
						}
						break;
					case VISUAL_SHADOWY:
					case VISUAL_HIDDEN:
						Cloak = CLOAKED;
						CloakingDevice.Set_Rate(0);
						CloakingDevice.Set_Stage(0);
						Mark(MARK_CHANGE);
						if (RTTI == RTTI_UNIT && ((UnitClass *)this)->Flagged != HOUSE_NONE) {
							Do_Shimmer();
						} else {
							int i;
							DynamicVectorClass<TechnoClass *> targeting_me;
							for (i = Array.Count() - 1; i >= 0; i--) {
								TechnoClass * tech = Array[i];
								if (tech->TarCom == this) {
									if (Map[Center_Coord()].Is_Sensed(tech->House->HeapID) || tech->House == House) {
										targeting_me.Add(tech);
									}
								}
							}
							Detach_All(false);
							for (i = targeting_me.Count() - 1; i >= 0; i--) {
								targeting_me[i]->Assign_Target(this);
							}
						}
						if (RTTI == RTTI_UNIT && !House->FUN_0050B730()) {
							Scatter(COORD_NONE, true);
						}
						break;
				}
				break;
			case CLOAKED:
				if (Should_Uncloak()) {
					Do_Uncloak();
				}
				break;
		}
	}
}

// FUNCTION: 0x006FBC90
bool TechnoClass::Should_Uncloak(void) const
{
	if (!(Is_Allowed_To_Recloak() || IsCloakable) || Is_Immobilized()) {
		if (Has_Ability(ABILITY_CLOAK)) {
			return(false);
		}
		if (!Map[Center_Coord().As_Cell()].Is_Cloaked(House->HeapID)) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x006FBDC0
bool TechnoClass::Is_Ready_To_Cloak(void) const
{
	if (!Is_Allowed_To_Recloak() && !Has_Ability(ABILITY_CLOAK)) {
		if (!Map[Center_Coord().As_Cell()].Is_Cloaked(House->HeapID) && !IsCloakable) {
			return(false);
		}
	}

	if (Cloak == CLOAKED) {
		return(false);
	}

	if (Arm != 0) {
		return(false);
	}

	if (TarCom != NULL && IsCloseEnoughToAttack(TarCom)) {
		return(false);
	}

	if (RTTI != RTTI_BUILDING && CloakingDevice.Fetch_Stage() != 0) {
		return(false);
	}

	if (CloakDelay != 0) {
		return(false);
	}

	FootClass * foot = (FootClass *)this;
	if (LocomotorSource != NULL && foot != NULL && (AbstractFlags & 4) && foot->IsAttackedByLocomotor) {
		return(false);
	}

	bool grounded = Get_Height_AGL() <= 0;
	return(grounded);
}

extern bool MoveFeedback;
void FUN_00637840(TechnoClass * techno);

// FUNCTION: 0x006FBFA0
bool TechnoClass::Select(void)
{
	if (!IsDiscoveredByPlayer && !House->Is_Player_Control() && MainWindow) {
		return(false);
	}

	if (BASECLASS::Select()) {
		if (Tag != NULL) {
			Tag->Spring(TEVENT_SELECTED, this);
		}
		if (House->Is_Player_Control() || House->Class->IsMultiplayPassive) {
			if (MoveFeedback) {
				Response_Select();
			}
		}
		FUN_00637840(this);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006FC030
bool TechnoClass::CanBeSelectedNow(void) const
{
	if (SlaveOwner != NULL) {
		return(false);
	}
	if (Deactivated) {
		return(false);
	}
	if (BunkerLinkedItem != NULL) {
		return(false);
	}
	if (IsTethered) {
		if (Get_Cell_Ptr()->Cell_Building() != NULL) {
			return(false);
		}
	}
	return(FUN_005F6C70());
}

// FUNCTION: 0x006FC090
FireErrorType TechnoClass::FUN_006FC090(AbstractClass * target, int which) const
{
	return(Can_Fire(target, which, false));
}

// FUNCTION: 0x006FCD40
void TechnoClass::Stun(void)
{
	Assign_Target(NULL);
	Assign_Destination(NULL);
	SendToEachLink(RADIO_OVER_OUT);

	FootClass * foot = (FootClass *)this;
	if (foot == NULL || !(AbstractFlags & 4) || !foot->IsAttackedByLocomotor) {
		if (SpawnManager != NULL) {
			SpawnManager->KillNodes();
			SpawnManager->ResetTarget();
		}
		Detach_All(true);
	}
	Unselect();
}

// FUNCTION: 0x006FCDB0
void TechnoClass::Assign_Target(AbstractClass * target)
{
	ShouldLoseTargetNow = false;
	if (target == TarCom) {
		return;
	}

	if (Airstrike != NULL && Airstrike->Owner == this) {
		Airstrike->ClearTarget();
	}

	if (Fetch_RTTI() == RTTI_AIRCRAFT) {
		if (target == NULL) {
			target = NULL;
		} else if (Techno_Type_Class()->Spawned != false && Techno_Type_Class()->MissileSpawn == false && Ammo != 0 && CurrentMission == MISSION_ATTACK && Status > 4 && Status < 10) {
			Ammo = 0;
			target = NULL;
		}
	}

	if (target == NULL) {
		target = NULL;
	} else {
		if (target->AbstractFlags & 1) {
			TechnoClass * techno = (TechnoClass *)target;
			if (techno->BunkerLinkedItem != NULL && techno->Fetch_RTTI() != RTTI_BUILDING) {
				if (TemporalImUsing != NULL) {
					target = techno->BunkerLinkedItem;
				}
				WeaponDataStruct const * data = Get_Class_Weapon_Data(0);
				WeaponTypeClass * weapon = data->Weapon;
				if (weapon != NULL && weapon->WarheadPtr != NULL && weapon->WarheadPtr->IsLocomotor != false) {
					target = techno->BunkerLinkedItem;
				}
				if (Airstrike != NULL && Airstrike->Owner == this) {
					target = techno->BunkerLinkedItem;
				}
			}
		}

		if (target == this) {
			target = &Map[(Coord const &)PositionCoord];
		} else if (target != NULL && (target->AbstractFlags & 2)) {
			ObjectClass * object = (ObjectClass *)target;
			if (object->IsActive == false || object->Strength == 0) {
				target = NULL;
			} else if (object->Is_Foot()) {
				TechnoClass * foot = (TechnoClass *)target;
				if (foot->IsSinking != false) {
					target = NULL;
				}
				if (foot->Fetch_RTTI() == RTTI_INFANTRY && ((InfantryClass *)foot)->FUN_00522CB0()) {
					target = NULL;
				}
			}
		}
	}

	TarCom = target;

	if (SpawnManager != NULL && target == NULL) {
		SpawnManager->SetTarget(NULL);
	}

	if (TarCom == NULL) {
		BurstIndex = 0;
	}

	if (ParticleSystems[0] != NULL) {
		if (!IsCloseEnoughToAttack(target)) {
			ParticleSystems[0]->Delete_Me();
			ParticleSystems[0] = NULL;
		}
	}
}

// FUNCTION: 0x006FCFA0
int TechnoClass::Rearm_Delay(int which) const
{
	if (RTTI == RTTI_BUILDING && Ammo > 1) {
		return(1);
	}

	WeaponTypeClass const * weapon = Get_Class_Weapon_Data(which)->Weapon;
	if (weapon == NULL) {
		return(1);
	}

	if (weapon->IsSonic
		|| (weapon->UseSparkParticles && ParticleSystems[1])
		|| (weapon->UseFireParticles && ParticleSystems[0])
		|| (weapon->IsRailgun && ParticleSystems[4])) {
		return(weapon->ROF);
	}

	UnitClass const * unit = (this == NULL) ? NULL : ((RTTI == RTTI_UNIT) ? (UnitClass const *)this : NULL);
	if (BurstIndex < weapon->Burst) {
		int delay = -1;
		if (BurstIndex > 0 && BurstIndex <= 4 && unit != NULL) {
			delay = unit->Class->BurstDelay[BurstIndex - 1];
		}
		return((delay != -1) ? delay : Random_Pick(3, 5));
	}

	int delay = weapon->ROF * House->ROFBias + Random_Pick(0, 2);

	if (Has_Ability(ABILITY_ROF)) {
		delay = delay * Rule->VeteranROF;
	}

	if (CanOccupyFire()) {
		if (GetOccupantCount() > 0) {
			delay = delay / GetOccupantCount();
		}
		if (Rule->OccupyROFMultiplier > 0.0f) {
			delay = delay / Rule->OccupyROFMultiplier;
		}
	}

	if (BunkerLinkedItem != NULL && RTTI != RTTI_BUILDING) {
		if (Rule->BunkerROFMultiplier != 0.0f) {
			delay = delay / Rule->BunkerROFMultiplier;
		}
	}

	return(delay);
}

// FUNCTION: 0x006FD210
LaserDrawClass * TechnoClass::CreateLaser(ObjectClass * target, int which, WeaponTypeClass * weapon, Coord const & source_coord)
{
	RGBClass color;
	int duration = weapon->LaserDuration;
	RGBClass const * spread = &weapon->LaserOuterSpread;
	RGBClass const * inner;
	RGBClass const * outer;
	if (weapon->IsHouseColor) {
		inner = &House->LaserColor;
		color.Set_Red(inner->Get_Red() >> 1);
		color.Set_Green(inner->Get_Green() >> 1);
		color.Set_Blue(inner->Get_Blue() >> 1);
		outer = &color;
	} else {
		inner = &weapon->LaserInnerColor;
		outer = &weapon->LaserOuterColor;
	}

	Coord source;
	Coord dest;
	int zadjust = 0;

	if (source_coord != COORD_NONE) {
		source = source_coord;
	} else {
		source = Fire_Coord(which, Coord(0, 0, 0));
		if (RTTI == RTTI_BUILDING) {
			Point2D p1 = TacticalMap->Coord_To_Pixel_Absolute(source);
			Point2D p2 = TacticalMap->Coord_To_Pixel_Absolute(Render_Coord());
			zadjust = p1.Y - p2.Y;
			if (zadjust > 0) {
				zadjust = 0;
			}
		}
	}

	if (target != NULL && (target->AbstractFlags & 2)) {
		dest = target->Target_Coord();
	} else {
		dest = target->As_Coord();
	}

	LaserDrawClass * laser = new LaserDrawClass(source, dest, zadjust, true, *inner, *outer, *spread, duration, false, true, 1.0f, 0.0f);
	if (laser != NULL && weapon->IsHouseColor) {
		laser->IsHouseColor = true;
	}
	return(laser);
}

// FUNCTION: 0x006FD460
EBolt * TechnoClass::CreateEBolt(ObjectClass * target, WeaponTypeClass * weapon, Coord const & source_coord)
{
	int zadjust = 0;
	Coord dest;
	EBolt * ebolt = new EBolt();
	if (ebolt == NULL) {
		return(NULL);
	}

	if (RTTI == RTTI_BUILDING) {
		Point2D p1 = TacticalMap->Coord_To_Pixel_Absolute(source_coord);
		Point2D p2 = TacticalMap->Coord_To_Pixel_Absolute(Render_Coord());
		zadjust = p1.Y - p2.Y;
		if (zadjust > 0) {
			zadjust = 0;
		}
	}

	if (target != NULL && (target->AbstractFlags & 2)) {
		dest = target->Target_Coord();
	} else {
		dest = target->As_Coord();
	}

	if (weapon != NULL) {
		ebolt->AlternateColor = weapon->IsAlternateColor;
	}

	ebolt->Fire(source_coord, dest, zadjust);
	return(ebolt);
}

// FUNCTION: 0x006FD570
EBolt * TechnoClass::FireEBolt(ObjectClass * target)
{
	int which = What_Weapon_Should_I_Use(target);
	WeaponTypeClass * weapon = Get_Class_Weapon_Data(which)->Weapon;
	Coord source = Fire_Coord(which, Coord(0, 0, 0));
	EBolt * ebolt = CreateEBolt(target, weapon, source);
	if (ebolt != NULL && Fetch_RTTI() == RTTI_UNIT && IsActive && ((UnitClass *)this)->ElectricBolt == NULL) {
		((UnitClass *)this)->ElectricBolt = ebolt;
		ebolt->SetOwner(this, which);
	}
	return(ebolt);
}

static inline TechnoClass * As_Techno(AbstractClass * object)
{
	if (object != NULL && (object->AbstractFlags & 1)) {
		return((TechnoClass *)object);
	}
	return(NULL);
}

// FUNCTION: 0x006FD620
RadBeam * TechnoClass::CreateRadBeam(ObjectClass * target, int type)
{
	RadBeam * beam = RadBeam::Allocate(1);
	int zadjust = 0;
	Coord source = Fire_Coord(0, Coord(0, 0, 0));
	Coord render = Render_Coord();
	if (source.Y != render.Y) {
		Point2D p1 = TacticalMap->Coord_To_Pixel_Absolute(source);
		Point2D p2 = TacticalMap->Coord_To_Pixel_Absolute(render);
		zadjust = p1.Y - p2.Y;
	}

	Coord dest;
	if (target == NULL) goto none;
	if (!(target->AbstractFlags & 2)) {
none:
		dest = target->As_Coord();
	} else {
		dest = target->Target_Coord();
	}

	RGBClass color;
	switch (type) {
		case 1:
			color = Rule->ChronoBeamColor;
			break;

		case 2:
			color = Rule->MagnaBeamColor;
			break;

		default:
			color = Rule->RadColor;
			break;
	}

	beam->FUN_006594A0(source);
	beam->FUN_006594C0(dest);
	beam->FUN_006594E0(15);
	beam->FUN_006594F0(40.0);
	beam->FUN_00659470(color);
	beam->FUN_00659490(zadjust);
	if (type == 2) {
		beam->FUN_00659540(1);
		beam->FUN_00659550(true);
		if (target != NULL && (target->AbstractFlags & 1)) {
			beam->Owner = As_Techno(target);
			As_Techno(target)->FiringRadBeam = beam;
		}
	}
	return(beam);
}

// FUNCTION: 0x006FDA00
DirType TechnoClass::Barrel_Pitch(AbstractClass * target) const
{
	DirType pitch(Dir256(TClass->FireAngle));
	if (target != NULL) {
		Coord predicted = Predict_Target_Coord();
		int which = TClass->HasMultipleTurrets() ? CurrentWeaponNumber : 0;
		Coord coord = predicted - Turret_Coord(which);
		WeaponTypeClass const * weapon = Get_Class_Weapon_Data(which)->Weapon;
		double gravity = Rule->Gravity;
		BulletTypeClass const * bullet = weapon->Bullet;
		if (bullet != NULL && bullet->IsFloater) {
			gravity = Get_Floater_Gravity();
		}
		int distance = fastmath::sqrt(pow(coord.X, 2) + pow(coord.Y, 2));
		int speed = weapon->Get_Speed(distance);
		if (!Calculate_Projectile_Pitch(Should_Use_High_Arc(which), speed, distance, coord.Z, gravity, pitch)) {
			pitch = DIR_NW;
		}
	}
	return(pitch);
}

// FUNCTION: 0x006FDB80
int TechnoClass::Estimate_Damage(TechnoClass * target, WeaponTypeClass const * weapon) const
{
	if (target == NULL) {
		return(0);
	}
	int damage = weapon->Attack;
	if (weapon->IsSonic || weapon->UseFireParticles) {
		return(0);
	}

	if (damage > 0) {
		damage = (int)(House->FirepowerBias * FirepowerBias * damage);
		if (Has_Ability(ABILITY_FIREPOWER)) {
			damage = (int)(damage * Rule->VeteranCombat);
		}
		damage = (int)((double)damage / (target->House->GetArmorMultiplier(Techno_Type_Class()) * ArmorBias));
		if (target->Has_Ability(ABILITY_STRONGER)) {
			damage = (int)(damage / Rule->VeteranArmor);
		}
		if (damage < 1) {
			damage = 1;
		}
		damage = Modify_Damage(damage, weapon->WarheadPtr, target->Class_Of()->Armor, 0);
	}
	return(damage);
}

// FUNCTION: 0x006FF950
void TechnoClass::FUN_006FF950(void)
{
}

bool FUN_00731BF0(void);
bool FUN_00637AA0(void);
void FUN_00637DD0(EventClass event);

// FUNCTION: 0x006FFBE0
bool TechnoClass::Player_Assign_Mission(MissionType order, AbstractClass * target, AbstractClass * destination, AbstractClass * nearby)
{
	bool attack_move = FUN_00731BF0() && CanAttackOnTheMove();
	if ((order == MISSION_MOVE || order == MISSION_ATTACK) && attack_move) {
		order = MISSION_ATTACK_MOVE;
	}

	if (FUN_00637AA0()) {
		EventClass event(PlayerPtr->HeapID, TargetClass(this), order, target, destination, nearby);
		FUN_00637DD0(event);
		return(false);
	}

	if (MoveFeedback) {
		if (order == MISSION_HARVEST) {
			VoiceHarvest();
		} else if (order == MISSION_ATTACK) {
			Response_Attack(target);
		} else if (order == MISSION_MOVE || order == MISSION_ATTACK_MOVE) {
			Response_Move();
		} else if (order == MISSION_ENTER) {
			VoiceEnter();
		} else if (order == MISSION_CAPTURE) {
			VoiceCapture();
		} else if (order == MISSION_UNLOAD) {
			VoiceDeploy();
		} else if (Techno_Type_Class()->VoiceSpecialAttack.Count() > 0) {
			QueueVoice(Techno_Type_Class()->VoiceSpecialAttack.Pick(NonCriticalRandomNumber()));
		}
	}

	{
		TargetClass dest(destination);
		TargetClass targ(target);
		Queue_Mission(TargetClass(this), order, targ, dest);
	}
	return(true);
}

void __fastcall FUN_00639FD0(EventClass * event);

// FUNCTION: 0x006FFE00
bool TechnoClass::ClickedEvent(int event)
{
	EventClass ev(PlayerPtr->HeapID, (unsigned char)event, TargetClass(this));
	if (FUN_00637AA0()) {
		FUN_00639FD0(&ev);
		return(false);
	}
	OutList.Add(ev);
	return(true);
}

// FUNCTION: 0x00700C40
bool TechnoClass::Can_Player_Move(void) const
{
	if (!House->Is_Player_Control()) {
		return(false);
	}
	if (IsUnderEMP()) {
		return(false);
	}
	if (BunkerLinkedItem != NULL) {
		return(false);
	}
	if (Techno_Type_Class()->Spawned) {
		return(false);
	}
	if (IsParalyzed()) {
		return(false);
	}
	if (!IsNotWarping()) {
		return(false);
	}
	if (SlaveOwner != NULL) {
		return(false);
	}
	if (Deactivated) {
		return(false);
	}
	if (SpawnManager != NULL && SpawnManager->CountLaunchingSpawns() > 0) {
		if (SpawnManager->CountLaunchingSpawns() < Techno_Type_Class()->SpawnsNumber) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: 0x00700D10
bool TechnoClass::Can_Attack_Now(void) const
{
	if (this != NULL && Fetch_RTTI() == RTTI_UNIT) {
		FootClass const * foot = (FootClass const *)this;
		if ((foot->CurrentTube >= 0 && foot->Deploy_To_Fire()) || IsUnderEMP()) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: 0x007010D0
bool TechnoClass::Can_Player_Fire(void) const
{
	if (!Deactivated && House->Is_Player_Control()) {
		WeaponDataStruct const * weapon = GetTurretWeapon();
		if (weapon != NULL && weapon->Weapon != NULL && !IsUnderEMP()) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x00701120
bool TechnoClass::Is_Weapon_Equipped(void) const
{
	WeaponDataStruct const * weapon = GetTurretWeapon();
	return(weapon != NULL && weapon->Weapon != NULL);
}

// FUNCTION: 0x00701140
bool TechnoClass::Can_Repair(void) const
{
	if (Fetch_RTTI() != RTTI_BUILDING) {
		return(false);
	}
	return(Techno_Type_Class()->IsRepairable && Strength != Class_Of()->MaxStrength);
}

// FUNCTION: 0x00701190
bool TechnoClass::IsPowerOnline(void) const
{
	return(false);
}

// FUNCTION: 0x007011A0
int TechnoClass::FUN_007011A0(void)
{
	if (In_Air()) {
		return(0);
	}
	int increment = Rule->ElevationIncrement;
	if (increment == 0) {
		return(0);
	}
	double bonus = Rule->ElevationIncrementBonus;
	Cell where = Center_Coord().As_Cell();
	CellClass * cell = &Map[where];
	int lowest = Map.FUN_00588990();
	int diff = (cell->Height - lowest) / increment;
	double scaled = diff * bonus;
	double cap = Rule->ElevationBonusCap;
	scaled = (scaled < cap) ? scaled : Rule->ElevationBonusCap;
	int height = cell->Height * LEVEL_LEPTON_H;
	int lift = (int)scaled << 8;
	return((int)fastmath::sqrt(lift * lift + height * height));
}

static inline int Passenger_Range(TechnoClass const * transport)
{
	int range = 0x7FFFFFFF;
	TechnoClass * passenger = transport->Cargo.Attached_Object();
	if (passenger != NULL) {
		do {
			WeaponTypeClass * weapon = passenger->GetTurretWeapon()->Weapon;
			if (weapon != NULL && weapon->Range < range) {
				range = weapon->Range;
			}
			passenger = (TechnoClass *)passenger->Next;
		} while (passenger != NULL && (passenger->AbstractFlags & 4));
	}
	return(range);
}

// FUNCTION: 0x007012C0
int TechnoClass::Weapon_Range(int which) const
{
	WeaponTypeClass * weapon = Get_Class_Weapon_Data(which)->Weapon;
	if (weapon != NULL) {
		int range = weapon->Range;
		if (Techno_Type_Class()->OpenTopped) {
			return((range < Passenger_Range(this)) ? range : Passenger_Range(this));
		}
		return(range);
	}
	return(0);
}

// FUNCTION: 0x007013A0
void TechnoClass::Override_Mission(MissionType mission, AbstractClass * tarcom, AbstractClass * navcom)
{
	SuspendedTarCom = TarCom;
	MissionClass::Override_Mission(mission, tarcom, navcom);
	Assign_Target(tarcom);
}

// FUNCTION: 0x007013E0
bool TechnoClass::Restore_Mission(void)
{
	if (MissionClass::Restore_Mission()) {
		Assign_Target(SuspendedTarCom);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00701410
void TechnoClass::Renovate(void)
{
	Mark(MARK_CHANGE);
	int max_strength = Techno_Type_Class()->MaxStrength;
	Strength = max_strength;
	EstimatedHealth = max_strength;
	if (Fetch_RTTI() == RTTI_BUILDING) {
		Repair(0);
		((BuildingClass *)this)->Set_Anim_Damage_State(HealthRatio <= Rule->ConditionYellow);
		VocClass::PlayAt(Rule->BuildingRepairedSound, Center_Coord(), NULL);
	}
}

extern bool __fastcall FUN_00746DB0(UnitClass * unit);
extern void __fastcall FUN_00638840(TechnoClass * techno);

static inline UnitClass * Cast_To_Unit(TechnoClass * techno)
{
	return((techno->Fetch_RTTI() == RTTI_UNIT) ? (UnitClass *)techno : NULL);
}

// FUNCTION: 0x007014A0
bool TechnoClass::Captured(HouseClass * newowner, bool announce)
{
	if (newowner != House) {
		if (House == PlayerPtr && IsSelected) {
			Unselect();
		}
		Assign_Target(NULL);
		Assign_Destination(NULL, true);
		if (!(this != NULL && Fetch_RTTI() == RTTI_UNIT && FUN_00746DB0(Cast_To_Unit(this)))) {
			ArchivedTarget = NULL;
		}
		if (CurrentMission != MISSION_DECONSTRUCTION) {
			if (!(this != NULL && Fetch_RTTI() == RTTI_UNIT && Cast_To_Unit(this)->Class->IsSimpleDeployer) || CurrentMission != MISSION_UNLOAD) {
				QueueMission(MISSION_GUARD, true);
			}
		}
		if (SpawnManager != NULL) {
			SpawnManager->KillNodes();
		}
		FUN_00638840(this);
		if (!IsInLimbo) {
			House->RegisterLoss(this, false);
		}
		RegisterDestruction(NULL);
		newowner->PointTotal += Techno_Type_Class()->Cost_Of(House);
		House->Tracking_Remove(this);
		newowner->Tracking_Add(this);
		switch (Fetch_RTTI()) {
			case 6:
				if (!Techno_Type_Class()->DontScore) {
					newowner->BuildingsKilled[Owner()]++;
				}
				break;
			case 1:
			case 2:
			case 15:
				if (!Techno_Type_Class()->DontScore) {
					newowner->UnitsKilled[Owner()]++;
				}
				break;
			default:
				break;
		}
		House->WhoLastHurtMe = newowner->Class->House;
		if (!IsInLimbo) {
			Mark(MARK_UP);
		}
		BecomeUntargetable();
		if (!IsInLimbo) {
			Mark(MARK_DOWN_FORCED);
		}
		CellClass * cell = NULL;
		if (!IsInLimbo) {
			if (!Is_Foot()) {
				cell = &Map[Coord(Position)];
			} else {
				cell = &Map[((FootClass *)this)->LastAdjacencyCell];
			}
			if (cell != NULL) {
				cell->Adjust_Threat(Owner(), -ThreatPosed);
				ThreatPosed = 0;
			}
			vt_entry_48C(0, 0, 0, 0);
		}
		House = newowner;
		IsOwnedByPlayer = (House == PlayerPtr && PlayerPtr != NULL);
		if (!IsInLimbo) {
			if (cell != NULL) {
				cell->Adjust_Threat(Owner(), ThreatPosed = Risk());
			}
			House->RegisterGain(this, true);
		}
		OriginallyOwnedByHouse = NULL;
		if (CurrentMission == MISSION_RESCUE || MissionQueue == MISSION_RESCUE) {
			Set_Mission(MISSION_GUARD);
		}
		if (!IsInLimbo) {
			if (!(Fetch_RTTI() == RTTI_BUILDING && CurrentMission == MISSION_UNLOAD && ((BuildingClass *)this)->Class->IsWeaponsFactory)) {
				TechnoClass * radio;
				if (FUN_0065AD30(0) == NULL || (radio = FUN_0065AD30(0))->Fetch_RTTI() != RTTI_BUILDING || !((BuildingClass *)radio)->Class->IsWeaponsFactory) {
					Assign_Destination(NULL, true);
					Assign_Target(NULL);
					Enter_Idle_Mode(false, true);
				}
			}
			if (IsRadarTracked) {
				Radar_Untrack();
				Radar_Track();
			}
			UpdateSight(0, 0, 0, 0, 0);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x007018C0
void TechnoClass::Set_Owner(HouseClass * newowner)
{
	House = newowner;
	IsOwnedByPlayer = (newowner == PlayerPtr && PlayerPtr != NULL);
}

namespace Unsorted { extern bool ArmageddonMode; }

// FUNCTION: 0x00702D40
void TechnoClass::RegisterDestruction(TechnoClass * destroyer)
{
	int total_recorded = 0;
	int points = Techno_Type_Class()->Cost_Of(House);
	if (IsActive && Tag && destroyer) Tag->Spring(TEVENT_ATTACKED, this);
	if (IsActive && Tag && destroyer) Tag->Spring(TEVENT_DISCOVERED, this);
	if (IsActive && Fetch_RTTI() != RTTI_UNIT) {
		if (IsActive && Tag && destroyer) Tag->Spring(TEVENT_DESTROYED, this);
		if (IsActive && Tag) Tag->Spring(TEVENT_DESTROYED_ANY, this);
		if (IsActive && Tag) Tag->Spring(TEVENT_DESTROYED_ANY_X, this);
	}
	if (!Techno_Type_Class()->DontScore) {
		if (destroyer != NULL) {
			if (destroyer->House->Is_Ally(this)) {
				points = 0;
			} else if (Veterancy.Is_Veteran()) {
				points += points;
			} else if (Veterancy.Is_Elite()) {
				points *= 3;
			}
			if (destroyer->InOpenToppedTransport && destroyer->Transporter != NULL && destroyer->Transporter->Techno_Type_Class()->IsTrainable) {
				destroyer->Transporter->Veterancy.Made_A_Kill(destroyer->Transporter->Techno_Type_Class()->Cost_Of(House), points);
			} else if (destroyer->Techno_Type_Class()->IsTrainable) {
				destroyer->Veterancy.Made_A_Kill(destroyer->Techno_Type_Class()->Cost_Of(House), points);
			} else if (destroyer->Techno_Type_Class()->MissileSpawn) {
				if (destroyer->SpawnOwner != NULL && destroyer->SpawnOwner->Techno_Type_Class()->IsTrainable) {
					destroyer->SpawnOwner->Veterancy.Made_A_Kill(destroyer->SpawnOwner->Techno_Type_Class()->Cost_Of(House), points);
				}
			} else if (destroyer->CanOccupyFire() && destroyer->Fetch_RTTI() == RTTI_BUILDING) {
				BuildingClass * building = (BuildingClass *)((destroyer->Fetch_RTTI() == RTTI_BUILDING) ? destroyer : NULL);
				InfantryClass * occupant = building->Occupants[building->FiringOccupantIndex];
				occupant->Veterancy.Made_A_Kill(occupant->Class->Cost_Of(House), points);
			}
			House->WhoLastHurtMe = destroyer->Owner();
			destroyer->House->PointTotal += points;
		}
		switch (Fetch_RTTI()) {
			case RTTI_BUILDING:
				if (!Techno_Type_Class()->IsInsignificant) {
					if (((BuildingClass *)this)->WhoLastHurtMe != HOUSE_NONE) {
						House->BuildingsLost++;
					}
					if (destroyer != NULL && !Techno_Type_Class()->DontScore) {
						destroyer->House->DestroyedBuildings.Increment_Unit_Total(((BuildingClass *)this)->Class->HeapID);
						destroyer->House->BuildingsKilled[Owner()]++;
					}
					if (Map.Is_Player_Names()) {
						Map.Redraw_Radar(false);
					}
				}
				break;
			case RTTI_AIRCRAFT:
				if (destroyer != NULL && !Techno_Type_Class()->DontScore) {
					destroyer->House->DestroyedAircraft.Increment_Unit_Total(((AircraftClass *)this)->Class->HeapID);
					total_recorded++;
				}
			case RTTI_INFANTRY:
				if (destroyer != NULL && !total_recorded && !Techno_Type_Class()->DontScore) {
					destroyer->House->DestroyedInfantry.Increment_Unit_Total(((InfantryClass *)this)->Class->HeapID);
					total_recorded++;
				}
			case RTTI_UNIT:
				if (destroyer != NULL && !total_recorded && !Techno_Type_Class()->DontScore) {
					destroyer->House->DestroyedUnits.Increment_Unit_Total(((UnitClass *)this)->Class->HeapID);
					total_recorded++;
				}
				House->UnitsLost++;
				if (destroyer != NULL && !Techno_Type_Class()->DontScore) {
					destroyer->House->UnitsKilled[Owner()]++;
				}
				if (Map.Is_Player_Names()) {
					Map.Redraw_Radar(false);
				}
				break;
			default:
				break;
		}
	}
}

static inline TechnoClass * Cast_To_Techno(ObjectClass const * object)
{
	if (object == NULL) {
		return(NULL);
	}
	if (object->AbstractFlags & 1) {
		return((TechnoClass *)object);
	}
	return(NULL);
}

// FUNCTION: 0x006F8960
bool TechnoClass::TryAutoTargetObject(ThreatType method, int mask, Cell const & cell, int range, TechnoClass const ** object, int & value, int zone) const
{
	*object = NULL;
	value = 0;

	CellClass * cellptr = &Map[cell];

	if (zone != -1 && Map.Get_Cell_Zone(cell, TClass->MZone, true) != zone) {
		return(false);
	}

	ObjectClass const * tentative = cellptr->Cell_Occupier(true);
	TechnoClass * tech = NULL;
	if (tentative == NULL) {
		tentative = cellptr->Cell_Occupier(false);
	}

	bool controlled = false;
	HouseClass * controller = NULL;
	if (Transporter != NULL && Transporter->TClass->OpenTopped) {
		if (Transporter->MindControlledBy != NULL) {
			controller = Transporter->MindControlledBy->House;
			controlled = true;
		}
	}

	while (tentative != NULL) {
		if (tentative != this) {
			tech = Cast_To_Techno(tentative);
			if (tech) {
				if (Combat_Damage() < 0) {
					if (tech->HealthRatio < Rule->ConditionGreen && House->Is_Ally(tech)) break;
				} else {
					if ((!House->Is_Ally(tech) && !controlled)
						|| TClass->AttackFriendlies
						|| Berzerk
						|| (controlled && controller != NULL && controller->Is_Ally(tech))
						|| (RTTI == RTTI_INFANTRY
							&& (((InfantryClass *)this)->IsBerzerk
							|| (((InfantryClass *)this)->Class->IsEngineer
								&& CurrentMission == MISSION_GUARD_AREA
								&& tech->HealthRatio <= Rule->ConditionRed
								&& tech->RTTI == RTTI_BUILDING
								&& ((BuildingClass *)tech)->Class->Cost_Of(tech->House) > 0)))) {
						break;
					}
				}
			}
		}
		tentative = tentative->Next;
	}

	if (tech == NULL) {
		return(false);
	}
	*object = tech;

	bool result = false;
	if (!House->Is_Ally(tech) || Combat_Damage() < 0
		|| (!House->Is_Human_Player() && this != NULL && RTTI == RTTI_INFANTRY
			&& ((InfantryClass *)((RTTI == RTTI_INFANTRY) ? this : NULL))->Class->IsEngineer)
		|| TClass->AttackFriendlies || Berzerk || controlled) {
		result = CanAutoTargetObject(method, mask, range, tech, value, -1, COORD_NONE);
	}
	return(result);
}

// FUNCTION: 0x006F8C10
int TechnoClass::Evaluate_Just_Cell(Cell const & cell) const
{
	if (House->Is_Human_Player()) {
		return(0);
	}

	if (!Rule->Diff[House->Difficulty].IsWallDestroyer) {
		return(0);
	}

	CellClass const * cellptr = &Map[cell];
	if (cellptr->Overlay == OVERLAY_NONE || !OverlayTypes[cellptr->Overlay]->IsWall) {
		return(0);
	}

	int primary = What_Weapon_Should_I_Use(&Map[cell]);
	if (!In_Range(cell, primary)) {
		return(0);
	}

	WeaponDataStruct const * data = GetTurretWeapon();
	if (data == NULL || data->Weapon == NULL || data->Weapon->WarheadPtr == NULL) {
		return(0);
	}

	if (data->Weapon->Bullet != NULL && !data->Weapon->Bullet->IsAntiGround) {
		return(0);
	}

	if (!data->Weapon->WarheadPtr->IsWallDestroyer) {
		return(0);
	}

	if (cellptr->Owner == HOUSE_NONE || House->Is_Ally(Houses[cellptr->Owner])) {
		return(0);
	}

	return(Weapon_Range(0) - Distance(cell));
}

// FUNCTION: 0x00703230
void TechnoClass::Record_The_Kill(HouseClass * source)
{
	int total_recorded = 0;
	int points = Techno_Type_Class()->Cost_Of(House);
	if (IsActive && Tag && source) Tag->Spring(TEVENT_ATTACKED, this);
	if (IsActive && Tag && source) Tag->Spring(TEVENT_DISCOVERED, this);
	if (IsActive && Fetch_RTTI() != RTTI_UNIT) {
		if (IsActive && Tag && source) Tag->Spring(TEVENT_DESTROYED, this);
		if (IsActive && Tag) Tag->Spring(TEVENT_DESTROYED_ANY, this);
		if (IsActive && Tag) Tag->Spring(TEVENT_DESTROYED_ANY_X, this);
	}
	if (source != NULL) {
		if (source->Is_Ally(this)) {
			points = 0;
		} else if (Veterancy.Is_Veteran()) {
			points += points;
		} else if (Veterancy.Is_Elite()) {
			points *= 3;
		}
		House->WhoLastHurtMe = source->HeapID;
		source->PointTotal += points;
	}
	switch (Fetch_RTTI()) {
		case RTTI_BUILDING:
			if (!Techno_Type_Class()->IsInsignificant) {
				if (((BuildingClass *)this)->WhoLastHurtMe != HOUSE_NONE) {
					House->BuildingsLost++;
				}
				if (source != NULL) {
					if (!Techno_Type_Class()->DontScore) {
						source->DestroyedBuildings.Increment_Unit_Total(((BuildingClass *)this)->Class->HeapID);
						source->BuildingsKilled[Owner()]++;
					}
				}
				if (Map.Is_Player_Names()) {
					Map.Redraw_Radar(false);
				}
			}
			break;
		case RTTI_AIRCRAFT:
			if (source != NULL && !Techno_Type_Class()->DontScore) {
				source->DestroyedAircraft.Increment_Unit_Total(((AircraftClass *)this)->Class->HeapID);
				total_recorded++;
			}
		case RTTI_INFANTRY:
			if (source != NULL && !total_recorded && !Techno_Type_Class()->DontScore) {
				source->DestroyedInfantry.Increment_Unit_Total(((InfantryClass *)this)->Class->HeapID);
				total_recorded++;
			}
		case RTTI_UNIT:
			if (source != NULL && !total_recorded && !Techno_Type_Class()->DontScore) {
				source->DestroyedUnits.Increment_Unit_Total(((UnitClass *)this)->Class->HeapID);
				total_recorded++;
			}
			House->UnitsLost++;
			if (source != NULL && !Techno_Type_Class()->DontScore) {
				source->UnitsKilled[Owner()]++;
			}
			if (Map.Is_Player_Names()) {
				Map.Redraw_Radar(false);
			}
			break;
		default:
			break;
	}
}

// FUNCTION: 0x00703590
Cell TechnoClass::Nearby_Location(TechnoClass const * techno) const
{
	SpeedType speed = Techno_Type_Class()->Speed;
	if (speed == SPEED_WINGED) {
		speed = SPEED_TRACK;
	}
	Cell cell;
	if (techno != NULL) {
		cell = techno->Center_Coord().As_Cell();
	} else {
		cell = Center_Coord().As_Cell();
	}
	MZoneType mzone = Techno_Type_Class()->MZone;
	int zone = mzone != MZONE_NONE ? Map.Get_Cell_Zone(cell, mzone, IsOnBridge) : -1;
	return(Map.Nearby_Location(cell, speed, zone, mzone, IsOnBridge));
}

// FUNCTION: 0x007036C0
void TechnoClass::Do_Uncloak(bool silent)
{
	if (Cloak == CLOAKED || Cloak == CLOAKING) {
		Cloak = UNCLOAKING;
		CloakingDevice.Set_Stage(Rule->CloakingStages - 1);
		CloakingDevice.Set_Rate(Techno_Type_Class()->CloakingSpeed);
		CloakingDevice.Set_Step(-1);
		if (!silent) {
			VocClass::PlayAt(Rule->CloakSound, PositionCoord);
		}
	}
}

// FUNCTION: 0x00703770
void TechnoClass::Do_Cloak(bool silent)
{
	if (Cloak == UNCLOAKED || Cloak == UNCLOAKING) {
		Detach_All(false);
		Cloak = CLOAKING;
		CloakingDevice.Set_Stage(0);
		CloakingDevice.Set_Rate(Techno_Type_Class()->CloakingSpeed);
		CloakingDevice.Set_Step(1);
		if (!silent) {
			VocClass::PlayAt(Rule->CloakSound, PositionCoord);
		}
		if (House != PlayerPtr && IsSelected) {
			Unselect();
		}
	}
}

// FUNCTION: 0x00703850
void TechnoClass::Do_Shimmer(void)
{
	Do_Uncloak();
}

// FUNCTION: 0x00703860
VisualType TechnoClass::Visual_Character(bool raw, HouseClass const * house) const
{
	if (Techno_Type_Class()->IsInvisible && IsOwnedByPlayer) return(VISUAL_NORMAL);
	if (Techno_Type_Class()->IsInvisible && !IsOwnedByPlayer && !Unsorted::ArmageddonMode) return(VISUAL_HIDDEN);
	if (Cloak == UNCLOAKED || Unsorted::ArmageddonMode || Fetch_RTTI() == RTTI_BUILDING) return(VISUAL_NORMAL);
	if (Cloak == CLOAKED) {
		if (raw && house != NULL && Map[PositionCoord.As_Cell()].Is_Sensed(house->HeapID)) return(VISUAL_SHADOWY);
		if (!raw && !MainWindow) return(VISUAL_SHADOWY);
		if (!raw && IsOwnedByPlayer) return(VISUAL_SHADOWY);
		if (!raw && Map[PositionCoord.As_Cell()].Is_Sensed(PlayerPtr->HeapID)) return(VISUAL_SHADOWY);
		if (!raw && (Session.Type != GAME_NORMAL && House != NULL && PlayerPtr != NULL && PlayerPtr->Is_Ally(House) && House->Is_Ally(PlayerPtr))) return(VISUAL_SHADOWY);
		return(VISUAL_HIDDEN);
	}
	int stage = CloakingDevice.Fetch_Stage();
	if (stage <= 0) {
		return(VISUAL_NORMAL);
	}
	stage = double(stage) / Rule->CloakingStages * 256;
	if (stage < 0x0040) return(VISUAL_INDISTINCT);
	if (stage < 0x0080) return(VISUAL_DARKEN);
	if (stage < 0x00C0) return(VISUAL_SHADOWY);
	if (!raw && IsOwnedByPlayer) return(VISUAL_SHADOWY);
	if (stage < 0x00FF) return(VISUAL_RIPPLE);
	return(VISUAL_HIDDEN);
}

// FUNCTION: 0x00703B10
bool TechnoClass::Is_Z_Fudge_Bridge(void) const
{
	Cell cell = Get_Cell();
	CellClass * cptr = &Map[cell];

	if (cptr != NULL && !IsOnBridge) {
		CellClass * south = &Map[Adjacent_Cell(cell, FACING_S)];
		CellClass * north = &Map[Adjacent_Cell(cell, FACING_N)];
		CellClass * east = &Map[Adjacent_Cell(cell, FACING_E)];
		CellClass * west = &Map[Adjacent_Cell(cell, FACING_W)];

		if (cptr->IsUnderBridge ||
			(south != NULL && south->IsUnderBridge && south->IsBridgeEastWest) ||
			(west != NULL && west->IsUnderBridge && !west->IsBridgeEastWest) ||
			(east != NULL && east->IsUnderBridge && !east->IsBridgeEastWest) ||
			(north != NULL && north->IsUnderBridge && north->IsBridgeEastWest)) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x00703CC0
bool TechnoClass::Was_Z_Fudge_Bridge(void) const
{
	Cell cell = Get_Cell();
	CellClass * cptr = &Map[cell];

	if (cptr != NULL && !IsOnBridge) {
		CellClass * south = &Map[Adjacent_Cell(cell, FACING_S)];
		CellClass * north = &Map[Adjacent_Cell(cell, FACING_N)];
		CellClass * east = &Map[Adjacent_Cell(cell, FACING_E)];
		CellClass * west = &Map[Adjacent_Cell(cell, FACING_W)];

		if (cptr->WasUnderBridge ||
			(south != NULL && south->WasUnderBridge && south->IsBridgeEastWest) ||
			(west != NULL && west->WasUnderBridge && !west->IsBridgeEastWest) ||
			(east != NULL && east->WasUnderBridge && !east->IsBridgeEastWest) ||
			(north != NULL && north->WasUnderBridge && north->IsBridgeEastWest)) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x00703E70
int TechnoClass::Get_Z_Fudge_Column(void) const
{
	Cell cell = Get_Cell();
	int fudge = 0;
	CellClass * cptr = &Map[cell];

	if (cptr != NULL && (Is_Z_Fudge_Bridge() || Was_Z_Fudge_Bridge())) {
		CellClass * south = &Map[Adjacent_Cell(cell, FACING_S)];
		CellClass * east = &Map[Adjacent_Cell(cell, FACING_E)];
		CellClass * south_east = &Map[Adjacent_Cell(cell, FACING_SE)];

		if (south != NULL && east != NULL && south_east != NULL) {
			if (south->ITType != (IsometricTileType)0xFFFF && south->ITType != (IsometricTileType)0xFF) {
				int tile = south->ITType - IsometricTileTypeClass::BridgeSet + 1;
				if (tile >= 7 && tile <= 16) {
					fudge = 1;
				}
			}
			if (east->ITType != (IsometricTileType)0xFFFF && east->ITType != (IsometricTileType)0xFF) {
				int tile = east->ITType - IsometricTileTypeClass::BridgeSet + 1;
				if (tile >= 7 && tile <= 16) {
					fudge = 1;
				}
			}
			if (south_east->ITType != (IsometricTileType)0xFFFF && south_east->ITType != (IsometricTileType)0xFF) {
				int tile = south_east->ITType - IsometricTileTypeClass::BridgeSet + 1;
				if (tile >= 7 && tile <= 16) {
					fudge++;
				}
			}
		}
	}
	return(fudge);
}

// FUNCTION: 0x00704000
int TechnoClass::Get_Z_Fudge_Tunnel(void) const
{
	int fudge = 0;
	Cell cell = PositionCell;

	if (!IsOnBridge) {
		CellClass * north = &Map[Adjacent_Cell(cell, DIR_N)];
		CellClass * west = &Map[Adjacent_Cell(cell, DIR_W)];
		Cell tuncell = CELL_NONE;
		CellClass * own = &Map[cell];

		if (north != NULL && west != NULL) {
			if (own->Has_Tunnel()) {
				tuncell = own->Fetch_CellID();
			} else if (north->Has_Tunnel()) {
				tuncell = north->Fetch_CellID();
			} else if (west->Has_Tunnel()) {
				tuncell = west->Fetch_CellID();
			}

			if (tuncell != CELL_NONE) {
				north = &Map[Adjacent_Cell(tuncell, DIR_N)];
				west = &Map[Adjacent_Cell(tuncell, DIR_W)];

				if (north != NULL && west != NULL) {
					north = &Map[Adjacent_Cell(north->Fetch_CellID(), DIR_N)];
					west = &Map[Adjacent_Cell(west->Fetch_CellID(), DIR_W)];

					if (north != NULL && west != NULL) {
						if (west->Has_Tunnel() || north->Has_Tunnel()) {
							fudge = 1;
						}
					}
				}
			}
		}
	}
	return(fudge);
}

// FUNCTION: 0x00704240
int TechnoClass::Get_Z_Fudge_Cliff(void) const
{
	int fudge = 0;
	Cell cell = Get_Cell();
	CellClass * cptr = &Map[cell];

	if (!IsOnBridge) {
		CellClass * south_east = &Map[Adjacent_Cell(Get_Cell(), FACING_SE)];
		if (south_east != NULL) {
			if (south_east->Height - cptr->Height >= 4) {
				fudge = 2;
			}
			south_east = &Map[Adjacent_Cell(south_east->Fetch_CellID(), FACING_SE)];
			if (south_east != NULL) {
				if (south_east->Height - cptr->Height >= 4) {
					fudge = 1;
				}
			}
		}
	}
	return(fudge);
}

// FUNCTION: 0x00705CA0
Coord TechnoClass::GetAttackCoordinates(void) const
{
	if (Get_Mission() == MISSION_ATTACK && TarCom != NULL) {
		return(TarCom->As_Coord());
	}
	return(Position);
}

// FUNCTION: 0x00705D10
void TechnoClass::FUN_00705D10(PlanningTokenClass * token)
{
	PlanningToken = token;
}

// FUNCTION: 0x00705D20
PlanningTokenClass * TechnoClass::FUN_00705D20(void)
{
	return(PlanningToken);
}

// FUNCTION: 0x00705D30
bool TechnoClass::FUN_00705D30(void)
{
	if (PlanningToken == NULL) {
		return(false);
	}
	return(PlanningToken->FUN_00636DC0() > 0 ? true : false);
}

// FUNCTION: 0x00705D50
bool TechnoClass::IsNotWarpingIn(void) const
{
	return(IsWarpingIn() ? false : true);
}

// FUNCTION: 0x00705D60
void TechnoClass::FUN_00705D60(void)
{
	TryNextPlanningTokenNode();
}

// FUNCTION: 0x00705D70
void * TechnoClass::GetRemapColour(void) const
{
	TechnoTypeClass const * type = Techno_Type_Class();
	int index = House->Scheme;
	DynamicVectorClass<ColorScheme *> * palette = type->Palette;
	if (palette != NULL) {
		return((*palette)[index]->Converter);
	}
	if (!IsClearlyVisibleTo(PlayerPtr) && GetDisguiseHouse(true) != NULL) {
		return(ColorSchemes[GetDisguiseHouse(true)->Scheme]->Converter);
	}
	return(ColorSchemes[index]->Converter);
}

// FUNCTION: 0x00706BD0
void TechnoClass::Techno_Draw_Voxel_Shadow(VoxelDataStruct const & voxeldata, int layer_index, int key, VoxelIndexClass * cache, Rect const & cliprect, Point2D const & point, Matrix3D const & matrix, bool force_cache, Surface * surface, int shadow_x, int shadow_y) const
{
	if (Cloak != UNCLOAKED || Techno_Type_Class()->NoShadow || voxeldata.VoxLib->Load_Failed()) {
		return;
	}

	if (key != -1) {
		if (cache == NULL || Techno_Type_Class()->DisableShadowCache) {
			key = -1;
		} else {
			StaticBufferClass::Entry * entry = (*cache)[key];
			if (entry != NULL) {
				Techno_Blit_Voxel(*entry, point, cliprect, ShapeFlags_Type(SHAPE_DARKEN|SHAPE_ZGRAD), NORMAL_LIGHT, 0);
				return;
			}
			if (!force_cache) {
				return;
			}
		}
	}

	SurfaceRegion region = Techno_Render_Voxel_Shadow(voxeldata, matrix, point, cliprect, layer_index, ShapeFlags_Type(SHAPE_DARKEN|SHAPE_ZGRAD), key != -1, surface, shadow_x, shadow_y);
	if (key != -1) {
		StaticBufferClass::Entry * data = VoxelStaticBuffer.Add(*VoxelDrawSystem::Get_Surface(), region);
		if (data == NULL || !cache->Add_Index(key, data)) {
			Class_Of();
			ObjectTypeClass::Clear_Voxel_Indexes();
			if (data == NULL) {
				data = VoxelStaticBuffer.Add(*VoxelDrawSystem::Get_Surface(), region);
			}
			cache->Add_Index(key, data);
		}
		Techno_Draw_Voxel_Shadow(voxeldata, layer_index, key, cache, cliprect, point, matrix, false, NULL, 0, 0);
	}
}

extern int DAT_00B43180;

// FUNCTION: 0x00707280
SurfaceRegion TechnoClass::Techno_Render_Voxel_Shadow(VoxelDataStruct const & voxeldata, Matrix3D const & matrix, Point2D const & point, Rect const & cliprect, int layer_index, ShapeFlags_Type flags, bool cached, Surface * surface, int shadow_x, int shadow_y) const
{
	Matrix3D mtx3;
	Matrix3D mtx2 = matrix * Matrix3D(voxeldata.MotLib->Get_Layer_Matrix(layer_index, 0));

	if (Techno_Type_Class()->UseBuffer) {
		DAT_00B43180 = 1;
	}

	mtx3.Make_Identity();
	if (Techno_Type_Class()->ConsideredAircraft) {
		mtx3.Scale(0.5f);
	}

	VoxelDrawSystem::Reset();
	VoxelDrawSystem::Prep_For_Shadow(voxeldata.VoxLib, layer_index, 0, VoxelCameraMatrix * mtx3, mtx2, VoxelShadowLightVector, surface, shadow_x, shadow_y);
	SurfaceRegion region = VoxelDrawSystem::Render();

	if (!cached) {
		Rect r = VoxelDrawSystem::Get_Surface()->Get_Rect();
		Blit_Block(*LogicalSurface, *NormalDrawer, *VoxelDrawSystem::Get_Surface(), region.Bounds, region.Point + point, cliprect, NULL, NormalDrawer->Blitter_From_Flags(flags), Get_Z_Adjust(), Get_Z_Gradient());
	}

	if (Techno_Type_Class()->UseBuffer) {
		DAT_00B43180 = 1;
	}

	return(region);
}

// FUNCTION: 0x00707750
void TechnoClass::Calculate_Sinking_Offset(short height, int y)
{
	SinkingYOffset = TacticalMap->TacPixelY + height + y - 10;
	FacingType dir = PrimaryFacing.Current().As_Dir8();
	if (dir == FACING_N || dir == FACING_W) {
		SinkingYOffset -= 8;
	} else if (dir == FACING_NW) {
		SinkingYOffset -= 12;
	}
}

static inline void Stop_Drain(TechnoClass * drainer)
{
	if (drainer->DrainAnim != NULL) {
		drainer->DrainAnim->Delete_Me();
		drainer->DrainAnim = NULL;
	}
	if (drainer->DrainTarget != NULL) {
		drainer->DrainTarget->DrainingMe = NULL;
		if (drainer->DrainTarget->House != NULL) {
			drainer->DrainTarget->House->RecalcPower = true;
		}
		drainer->DrainTarget = NULL;
	}
}

// FUNCTION: 0x007077C0
void TechnoClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);

	if (all) {
		Cargo.Detach((FootClass *)target);
	}

	bool clear_target = true;

	if (target == SpawnOwner) {
		SpawnOwner = NULL;
	}
	if (target == OriginallyOwnedByHouse) {
		OriginallyOwnedByHouse = NULL;
	}
	if (target == MindControlledByHouse) {
		MindControlledByHouse = NULL;
	}
	if (target == TemporalTargetingMe) {
		TemporalTargetingMe = NULL;
	}
	if (target == Airstrike) {
		Airstrike = NULL;
	}
	if (target == BeingManipulatedBy) {
		BeingManipulatedBy = NULL;
	}
	if (target == BunkerLinkedItem && ScenarioInit) {
		BunkerLinkedItem = NULL;
	}

	if (target == DrainTarget) {
		Stop_Drain(this);
		DrainTarget = NULL;
	}

	if (target == Transporter && Transporter != NULL) {
		if (!Transporter->IsActive || !Transporter->Strength || ScenarioInit) {
			Transporter = NULL;
		}
	}

	TechnoClass * drainer = DrainingMe;
	if (target == drainer) {
		Stop_Drain(drainer);
		DrainingMe = NULL;
	}

	if (target == DrainAnim) {
		DrainAnim = NULL;
	}
	if (target == MindControlRingAnim) {
		MindControlRingAnim = NULL;
	}
	if (target == DeployAnim) {
		DeployAnim = NULL;
	}

	if (!all && target != NULL && target->Is_Techno()) {
		CellClass * cptr = &Map[target->Center_Coord()];
		if (cptr->Is_Sensed(House->HeapID)) {
			clear_target = false;
		}
	}

	if (TarCom == target && clear_target) {
		if (all || target->Owner_HouseClass() != House) {
			if (TargetingTimer > 10 && !ScenarioInit) {
				TargetingTimer = Scen->RandomNumber(4, 8);
			}
			Assign_Target(NULL);
			if (Has_Suspended_Mission()) {
				Restore_Mission();
				if (RTTI == RTTI_AIRCRAFT && CurrentMission == MISSION_PATROL) {
					Status = 0;
					((AircraftClass *)this)->IsLockedStraight = false;
				}
			}
		}
	}

	if (SuspendedTarCom == target && clear_target) {
		SuspendedTarCom = NULL;
	}

	if (House == target) {
		House = NULL;
		IsOwnedByPlayer = false;
	}

	if (IsDisguised() && GetDisguiseHouse(true) == target) {
		ClearDisguise();
	}

	for (int i = 0; i < 8; i++) {
		if (ParticleSystems[i] == target) {
			ParticleSystems[i] = NULL;
		}
	}

	if (Wave == target) {
		Wave = NULL;
	}

	if (all) {
		if (QueueUpToEnter == target) {
			QueueUpToEnter = NULL;
		}
		if (ArchivedTarget == target) {
			ArchivedTarget = NULL;
		}
		if (CaptureManager != NULL) {
			CaptureManager->FUN_00471F90((AbstractClass *)target);
		}
	}

	if (SpawnManager != NULL) {
		SpawnManager->UnlinkPointer((AbstractClass *)target);
	}

	if (TemporalImUsing != NULL) {
		TemporalImUsing->FUN_0071AB60((AbstractClass *)target);
	}

	if (Airstrike != NULL && Airstrike->Owner == this) {
		Airstrike->InvalidatePointer((AbstractClass *)target);
	}

	if (target == DirectRockerLinkedUnit) {
		DirectRockerLinkedUnit->DirectRockerLinkedUnit = NULL;
		DirectRockerLinkedUnit = NULL;
	}

	if (target == LocomotorTarget) {
		ReleaseLocomotor(true);
		if (LocomotorTarget != NULL) {
			LocomotorTarget->LocomotorSource = NULL;
			LocomotorTarget = NULL;
		}
	}

	if (target == LocomotorSource) {
		LocomotorSource->ReleaseLocomotor(true);
		if (LocomotorSource != NULL) {
			LocomotorSource->LocomotorTarget = NULL;
			LocomotorSource = NULL;
		}
	}

	if (target == OldTeam) {
		OldTeam = NULL;
	}

	if (target == BehindAnim) {
		BehindAnim = NULL;
	}

	int index = -1;
	for (int slot = 0; slot < CurrentTargets.Count(); slot++) {
		if (target == CurrentTargets[slot]) {
			index = slot;
			break;
		}
	}
	if (index != -1) {
		CurrentTargetThreatValues.Delete_Index(index);
		CurrentTargets.Delete((AbstractClass *)target);
	}
	AttackedTargets.Delete((AbstractClass *)target);
}

// FUNCTION: 0x00707CB0
void TechnoClass::Kill_Cargo(TechnoClass * source)
{
	while (Cargo.Is_Something_Attached()) {
		Cargo.Attached_Object()->Remove_From_Team();
		FootClass * foot = Cargo.Detach_Object();
		if (foot != NULL) {
			foot->Kill_Cargo(foot);
			foot->RegisterDestruction(source);
			foot->Delete_Me();
		}
	}
}

// FUNCTION: 0x00707D20
InfantryTypeClass const * TechnoClass::Crew_Type(void) const
{
	if (!TClass->IsCrew) {
		return(NULL);
	}

	InfantryTypeClass const * infantry;
	if (House->ActLike == 0) {
		infantry = Rule->AlliedCrew;
	} else if (House->ActLike == 1) {
		infantry = Rule->SovietCrew;
	} else if (House->ActLike == 2) {
		infantry = Rule->ThirdCrew;
	} else {
		infantry = Rule->Technician;
	}
	if (House->Class->Side != -1) {
		if (Is_Weapon_Equipped() && Percent_Chance(15)) {
			infantry = Rule->Technician;
		}
	} else {
		infantry = Rule->Technician;
	}
	return(infantry);
}

// FUNCTION: 0x00707DD0
int TechnoClass::Value(void) const
{
	int value = 0;

	if (Rule->Diff[House->Difficulty].IsContentScan || House->IQ >= Rule->IQContentScan) {
		if (Cargo.Is_Something_Attached()) {
			FootClass * object = Cargo.Attached_Object();

			while (object != NULL) {
				value += object->Value();
				object = (FootClass *)(ObjectClass *)object->Next;
			}
		}
	}

	return(Risk() + TClass->Reward + value);
}

// FUNCTION: 0x00707E60
int TechnoClass::Threat_Range(int control) const
{
	if (control == -1) return(-1);

	if (control == 0) {
		bool is_renovator = Is_Renovator();
		if (TClass->ThreatRange != 0 && !is_renovator) {
			return(TClass->ThreatRange);
		}
		return(0);
	}

	int range = TClass->ThreatRange;
	if (range == 0) {
		range = MAX(Weapon_Range(0), Weapon_Range(1));
	}

	range *= 2;
	if (control == 2) {
		range = Bound(range, 7 * CELL_LEPTON, 16 * CELL_LEPTON);
	} else {
		range = Bound(range, 0, 16 * CELL_LEPTON);
	}

	return(range);
}

// FUNCTION: 0x00707F60
bool TechnoClass::Is_In_Same_Zone(Coord const & coord) const
{
	MZoneType zone = TClass->MZone;

	if (zone != MZONE_NONE) {
		if (coord != COORD_NONE) {
			Cell cell1 = coord.As_Cell();
			Cell cell2 = Destination_Coord(NULL).As_Cell();

			return(Map.Is_Same_Cell_Zone(cell2, cell1, zone, ((ObjectClass *)this)->Is_Moving_Onto_Bridge(NULL), Map[cell1].IsUnderBridge, Is_Allowed_To_Leave_Map()));
		}
	}
	return(true);
}

static inline bool Is_Object(AbstractClass const * a)
{
	if (a == NULL) return(false);
	unsigned char flag = (a->AbstractFlags >> 1) & 1;
	return(flag != 0);
}

// FUNCTION: 0x007087C0
bool TechnoClass::CanRetaliateToAttacker(ObjectClass * attacker, WarheadTypeClass * warhead)
{
	if (attacker == NULL) return(false);

	if (!Techno_Type_Class()->CanRetaliate) return(false);

	if (SlaveOwner != NULL || SlaveManager != NULL) return(false);

	if (DrainTarget != NULL && !House->IsHuman) return(false);

	if (CaptureManager != NULL && CaptureManager->CannotControlAnyMore()) return(false);

	if (SpawnManager != NULL) return(false);

	if (House->Is_Human_Player() && TarCom != NULL) return(false);

	if (!Current_Mission_Control().IsRetaliate) return(false);

	if (House->Is_Ally(attacker)) return(false);

	if (attacker->IsDisguisedAs(House)) return(false);

	if (Combat_Damage() <= 0 || !Is_Weapon_Equipped()) return(false);

	TechnoTypeClass const * ttype = Techno_Type_Class();
	int which = What_Weapon_Should_I_Use(attacker);
	FireErrorType error = (FireErrorType)GetFireErrorWithoutRange(attacker, which);
	if (error == FIRE_ILLEGAL || error == FIRE_CANT) return(false);

	if (House->Is_Human_Player() && attacker->RTTI == RTTI_BUILDING &&
			(RTTI == RTTI_INFANTRY && ((InfantryTypeClass const *)ttype)->IsBomber || Has_Ability(ABILITY_C4))) {
		return(false);
	}

	if (House->Is_Human_Player() && RTTI == RTTI_UNIT) {
		BuildingTypeClass const * deploys_into = ((UnitClass const *)this)->Class->DeploysInto;
		if (deploys_into != NULL && deploys_into->IsArtillary) {
			return(false);
		}
	}

	if (House->Is_Human_Player() && !Rule->IsSmartDefense && RTTI != RTTI_BUILDING) {
		if (CurrentMission != MISSION_GUARD_AREA && CurrentMission != MISSION_GUARD && CurrentMission != MISSION_PATROL) {
			return(false);
		}
	}

	if (Is_Foot() && ((FootClass *)this)->Team != NULL && ((FootClass *)this)->Team->Class->IsSuicide) {
		return(false);
	}

	if (!House->Is_Human_Player() && Is_Object(TarCom)
			&& Target_Threat((TechnoClass *)TarCom) > Target_Threat((TechnoClass *)attacker)) {
		return(false);
	}

	if (Is_Foot() && attacker == ((FootClass *)this)->ParasiteEatingMe) {
		return(false);
	}

	WeaponDataStruct const * wdata = Get_Class_Weapon_Data(which);
	WeaponTypeClass const * weapon = wdata->Weapon;
	if (weapon != NULL && weapon->WarheadPtr != NULL) {
		if ((float)weapon->WarheadPtr->Modifier[attacker->Techno_Type_Class()->Armor] > 0.01f) {
			return(true);
		}
		return(false);
	}

	return(true);
}

// FUNCTION: 0x00708B30
int TechnoClass::Get_Ownable(void) const
{
	return(TClass->Get_Ownable());
}

// FUNCTION: 0x00708B40
int TechnoClass::Risk(void) const
{
	TechnoTypeClass const * ttptr = Techno_Type_Class();
	if (ttptr != NULL) {
		if (Fetch_RTTI() == RTTI_BUILDING) {
			if (GetOccupantCount() > 0) {
				int threat = Rule->ThreatPerOccupant;
				return(threat * GetOccupantCount());
			}
			if (BunkerLinkedItem != NULL) {
				return(BunkerLinkedItem->Techno_Type_Class()->Risk);
			}
		}
		return(ttptr->Risk);
	}

	return(0);
}

// FUNCTION: 0x00708BC0
double TechnoClass::Tiberium_Load(void) const
{
	if (Techno_Type_Class()->Capacity != 0) {
		double amount = Storage.Get_Total_Amount();
		return(amount / Techno_Type_Class()->Capacity);
	}
	return(0.0);
}

// FUNCTION: 0x00708C10
FacingType TechnoClass::Desired_Load_Dir(ObjectClass * , Cell & moveto) const
{
	moveto = CELL_NONE;
	return(FACING_N);
}

// FUNCTION: 0x00708C30
int TechnoClass::Pip_Count(void) const
{
	double maximum, current;
	switch (Techno_Type_Class()->PipScale) {

		case PIPSCALE_AMMO:
			current = Ammo;
			maximum = Techno_Type_Class()->Max_Pips();
			break;

		case PIPSCALE_CHARGE:
			if (CaptureManager == NULL) {
				return(0);
			}
			current = CaptureManager->GetControlledCount();
			maximum = Techno_Type_Class()->Max_Pips();
			break;

		case PIPSCALE_PASSENGERS:
			current = Cargo.How_Many();
			maximum = Techno_Type_Class()->Max_Pips();
			break;

		case PIPSCALE_TIBERIUM:
			current = Storage.Get_Total_Amount();
			maximum = Techno_Type_Class()->Capacity;
			break;

		default:
			return(0);
	}

	int retval = Techno_Type_Class()->Max_Pips() * (current / maximum) + 0.5;
	if (!retval && current > 0) retval = 1;
	return(retval);
}

// FUNCTION: 0x00708D70
DirType TechnoClass::Fire_Direction(void) const
{
	return(Turret_Facing());
}

// FUNCTION: 0x00708D90
void TechnoClass::QueueVoice(int voice)
{
	if (MoveFeedback && voice != -1 && House->Is_Player_Control()) {
		QueuedVoiceIndex = voice;
	}
}

#pragma warning(push)
#pragma warning(disable : 4716)
// FUNCTION: 0x00708DC0
int TechnoClass::VoiceCapture(void)
{
	if (Techno_Type_Class()->VoiceCapture != -1) {
		QueueVoice(Techno_Type_Class()->VoiceCapture);
	} else {
		VoiceEnter();
	}
}
#pragma warning(pop)

static inline InfantryClass * As_Infantry_Techno(TechnoClass * techno)
{
	return(techno->Fetch_RTTI() == RTTI_INFANTRY ? (InfantryClass *)techno : NULL);
}

#pragma warning(push)
#pragma warning(disable : 4716)
// FUNCTION: 0x00708E00
int TechnoClass::VoiceDeploy(void)
{
	if (!unknown_bool_4F8) {
		if (this != NULL && Is_Infantry() && As_Infantry_Techno(this)->FUN_00522510()) {
			if (Techno_Type_Class()->VoiceUndeploy != -1) {
				QueueVoice(Techno_Type_Class()->VoiceUndeploy);
			}
		} else if (Techno_Type_Class()->VoiceDeploy != -1) {
			QueueVoice(Techno_Type_Class()->VoiceDeploy);
		}
	}
	unknown_bool_4F8 = false;
}
#pragma warning(pop)

// FUNCTION: 0x00708EB0
void TechnoClass::Response_Select(void)
{
	if (SlaveOwner != NULL) {
		if (Techno_Type_Class()->VoiceSelectEnslaved.Count()) {
			QueueVoice(Techno_Type_Class()->VoiceSelectEnslaved.Pick(NonCriticalRandomNumber()));
		}
	} else if (Deactivated) {
		if (Techno_Type_Class()->VoiceSelectDeactivated.Count()) {
			QueueVoice(Techno_Type_Class()->VoiceSelectDeactivated.Pick(NonCriticalRandomNumber()));
		}
	} else if (Techno_Type_Class()->VoiceSelect.Count()) {
		QueueVoice(Techno_Type_Class()->VoiceSelect.Pick(NonCriticalRandomNumber()));
	}
}

// FUNCTION: 0x00708FC0
void TechnoClass::Response_Move(void)
{
	if (Techno_Type_Class()->VoiceMove.Count()) {
		QueueVoice(Techno_Type_Class()->VoiceMove.Pick(NonCriticalRandomNumber()));
	}
}

#pragma warning(push)
#pragma warning(disable : 4716)
// FUNCTION: 0x00709020
int TechnoClass::VoiceEnter(void)
{
	if (Techno_Type_Class()->VoiceEnter != -1) {
		QueueVoice(Techno_Type_Class()->VoiceEnter);
	} else {
		Response_Move();
	}
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4716)
// FUNCTION: 0x00709060
int TechnoClass::VoiceHarvest(void)
{
	if (Techno_Type_Class()->VoiceHarvest != -1) {
		QueueVoice(Techno_Type_Class()->VoiceHarvest);
	} else {
		Response_Move();
	}
}
#pragma warning(pop)

// FUNCTION: 0x007091D0
bool TechnoClass::CanPassiveAcquireTargets(void) const
{
	if (IsWarpingSomethingOut() || SlaveOwner != NULL) {
		return(false);
	}
	if (!Techno_Type_Class()->CanPassiveAquire) {
		return(false);
	}
	if (Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass const *)this)->Class->CanBeOccupied && GetOccupantCount() == 0) {
		return(false);
	}
	bool cannotControl = (CaptureManager != NULL && CaptureManager->CannotControlAnyMore());
	if (Is_Renovator() && House->FUN_0050B730()) {
		return(false);
	}
	if (!cannotControl) {
		if (Is_Weapon_Equipped()) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x00709290
bool TechnoClass::CanOpportunityFire(void) const
{
	if (TarCom == NULL && !House->FUN_0050B730() && Is_Foot() && ((FootClass const *)this)->Team != NULL
		&& !((FootClass const *)this)->Team->Class->IsSuicide && ((FootClass const *)this)->Team->Class->IsAggressive
		&& CurrentMission == MISSION_MOVE) {
		return(true);
	}
	if (!CanPassiveAcquireTargets()) {
		return(false);
	}
	if (CurrentMission == MISSION_MOVE) {
		if (Techno_Type_Class()->BalloonHover) {
			if (!Is_Foot()) {
				return(false);
			}
			if (((FootClass const *)this)->NavCom == Get_Cell_Ptr()) {
				if (PlanningToken == NULL || PlanningToken->FUN_00636DC0() <= 0) {
					return(true);
				}
			}
		}
	}
	if (CurrentMission == MISSION_MOVE && this != NULL && Fetch_RTTI() == RTTI_UNIT) {
		UnitClass const * unit = (Fetch_RTTI() == RTTI_UNIT) ? (UnitClass const *)this : NULL;
		if (unit->Class->IsSimpleDeployer) {
			FootClass const * foot = (AbstractFlags & 4) ? (FootClass const *)this : NULL;
			if (foot->NavCom == Get_Cell_Ptr()) {
				if (PlanningToken == NULL || PlanningToken->FUN_00636DC0() <= 0) {
					return(true);
				}
			}
		}
	}
	if (Techno_Type_Class()->OpportunityFire) {
		return(true);
	}
	if (CurrentMission != MISSION_GUARD) {
		return(false);
	}
	int weapon = IsNotSprayAttack();
	WeaponDataStruct const * data = Get_Class_Weapon_Data(weapon);
	bool areaFire = (data != NULL && data->Weapon != NULL && data->Weapon->AreaFire);
	bool current = (What_Weapon_Should_I_Use(TarCom) == weapon);
	if (areaFire && current) {
		return(false);
	}
	return(true);
}

// FUNCTION: 0x00709460
int TechnoClass::FUN_00709460(FireErrorType error) const
{
	if (error == FIRE_ILLEGAL || error == FIRE_CANT || error == FIRE_RANGE) {
		return(false);
	}
	return(true);
}

// FUNCTION: 0x00709480
bool TechnoClass::FUN_00709480(void)
{
	bool result = false;
	if (CanOpportunityFire()) {
		AbstractClass * old = TarCom;
		unknown_4FC = Unsorted::CurrentFrame;
		if ((result = Target_Something_Nearby(Center_Coord(), THREAT_RANGE)) != false) {
			if (TarCom != old) {
				ShouldLoseTargetNow = true;
			}
		}
	}
	return(result);
}

// FUNCTION: 0x007094E0
bool TechnoClass::FUN_007094E0(void) const
{
	return(CurrentMission == MISSION_MOVE || CurrentMission == MISSION_HARVEST || CurrentMission == MISSION_GUARD);
}

// FUNCTION: 0x00709500
bool TechnoClass::FUN_00709500(void) const
{
	return(CurrentMission == MISSION_SLEEP || CurrentMission == MISSION_ENTER || CurrentMission == MISSION_STOP
		|| CurrentMission == MISSION_AMBUSH || CurrentMission == MISSION_UNLOAD || CurrentMission == MISSION_CONSTRUCTION
		|| CurrentMission == MISSION_DECONSTRUCTION || CurrentMission == MISSION_REPAIR || CurrentMission == MISSION_MISSILE
		|| CurrentMission == MISSION_HARMLESS || CurrentMission == MISSION_WAIT || CurrentMission == MISSION_OPEN);
}

// FUNCTION: 0x00709800
int TechnoClass::FUN_00709800(FireErrorType error) const
{
	if (error == FIRE_ILLEGAL || error == FIRE_CANT) {
		return(false);
	}
	return(true);
}

// FUNCTION: 0x00709820
bool TechnoClass::Target_Something_Nearby(Coord const & coord, ThreatType threat)
{
	unknown_4FC = Unsorted::CurrentFrame;
	if (CurrentMission == MISSION_GUARD_AREA) {
		int r = Scen->RandomNumber(0, 2);
		TargetingTimer = Rule->GuardAreaTargetingDelay + r;
	} else {
		int r = Scen->RandomNumber(0, 2);
		TargetingTimer = Rule->NormalTargetingDelay + r;
	}
	threat = ThreatType(threat & (THREAT_RANGE|THREAT_AREA));

	if (TarCom != NULL && ShouldLoseTargetNow) {
		int primary = What_Weapon_Should_I_Use(TarCom);
		FireErrorType error = Can_Fire(TarCom, primary, true);
		if (error == FIRE_CANT && SpawnManager != NULL) {
			SpawnManager->ResetTarget();
		} else if (error == FIRE_CANT || error == FIRE_ILLEGAL || error == FIRE_RANGE) {
			Assign_Target(NULL);
		}
	}

	if (TarCom == NULL) {
		AbstractClass * found = Greatest_Threat(threat, coord, false);
		if (Techno_Type_Class()->DistributedFire) {
			FUN_00709550();
		} else {
			if (found != NULL) {
				Assign_Target(found);
			}
			int weapon = What_Weapon_Should_I_Use(TarCom);
			TechnoClass * target = (found == NULL) ? NULL : ((found->AbstractFlags & 1) ? (TechnoClass *)found : NULL);
			WeaponTypeClass const * wp = Get_Class_Weapon_Data(weapon)->Weapon;
			if (target != NULL && wp != NULL && !wp->Bullet->IsInaccurate) {
				target->EstimatedHealth -= Estimate_Damage(target, wp);
			}
		}
	}
	return(TarCom != NULL);
}

// FUNCTION: 0x007099D0
int TechnoClass::Exit_Object(TechnoClass *, Cell)
{
	return(0);
}

// FUNCTION: 0x007099E0
bool TechnoClass::Is_Ready_To_Random_Animate(void) const
{
	return(IdleTimer == 0);
}

// FUNCTION: 0x00709A20
void TechnoClass::vt_entry_47C(unsigned)
{
}

// FUNCTION: 0x00709A30
void TechnoClass::Assign_Destination(AbstractClass *, bool)
{
}

// FUNCTION: 0x00709A40
bool TechnoClass::Enter_Idle_Mode(bool, bool)
{
	if (TemporalImUsing != NULL && TemporalImUsing->Target != NULL) {
		TemporalImUsing->LetGo();
	}
	if (!RefreshMegaMission() && IsNotWarpingIn()) {
		TryNextPlanningTokenNode();
	}
	return(false);
}

namespace FileSystem { extern ShapeSet * PIPS_SHP; extern ConvertClass * PALETTE_PAL; }

// FUNCTION: 0x0070A990
void TechnoClass::DrawVeterancyPips(Point2D const & point, Rect const & rect) const
{
	Class_Of();
	ShapeSet * shapes = FileSystem::PIPS_SHP;
	int frame = -1;
	if (Veterancy.Is_Veteran()) {
		frame = 14;
	}
	if (Veterancy.Is_Elite()) {
		frame = 15;
	}
	if (Veterancy.Is_Dumbass()) {
		frame = 19;
	} else if (frame == -1) {
		return;
	}

	Point2D xy = point + Point2D(5, 2);
	if (RTTI != (RTTIType)15) {
		xy.X += 5;
		xy.Y += 4;
	}
	Draw_Shape(*DSurface::Temp, *FileSystem::PALETTE_PAL, shapes, frame, xy, rect, ShapeFlags_Type(0xE00), NULL, -2, ZGRAD_GROUND, 1000);
}

Point2D Simple_Text_Print_Wide(wchar_t const * text, Surface * surface, Rect const * bounds, Point2D const * pt, unsigned fore, unsigned back, TextPrintType flags, int unused);
Rect Drawing_GetTextDimensions(wchar_t const * text, Point2D loc, unsigned short flags, int marginX, int marginY);

// FUNCTION: 0x0070AA60
void TechnoClass::Draw_Text_Overlay(Point2D const & point1, Point2D const & point2, Rect const & cliprect) const
{
	wchar_t buffer[128];
	wchar_t const * text;

	if (RTTI == RTTI_BUILDING && ((BuildingClass const *)this)->Class->Power > 0 && !IsLeader) {
		swprintf(buffer, StringTable::LoadString("TXT_POWER_DRAIN2", NULL, "D:\\ra2mdpost\\Techno.CPP", 15483), House->Power_Output(), House->Power_Drain());
		Rect box = Drawing_GetTextDimensions(buffer, point2, 0x149, 4, 2);
		box = Intersect(box, cliprect);
		int color = DSurface::Build_Hicolor_Pixel(House->RemapColorRGB);
		DSurface::Temp->Fill_Rect(box, 0);
		DSurface::Temp->Draw_Rect(box, color);
		Simple_Text_Print_Wide(buffer, DSurface::Temp, &cliprect, &point2, color, 0, (TextPrintType)0x149, 1);
	}

	if (IsLeader) {
		text = StringTable::LoadString("TXT_PRIMARY", NULL, "D:\\ra2mdpost\\Techno.CPP", 15502);
		if (RTTI == RTTI_BUILDING && ((BuildingClass const *)this)->Class->Width() == 1) {
			text = StringTable::LoadString("TXT_PRI", NULL, "D:\\ra2mdpost\\Techno.CPP", 15504);
		}
		Rect box = Drawing_GetTextDimensions(text, point2, 0x149, 4, 2);
		box = Intersect(box, cliprect);
		int color = DSurface::Build_Hicolor_Pixel(House->RemapColorRGB);
		DSurface::Temp->Fill_Rect(box, 0);
		DSurface::Temp->Draw_Rect(box, color);
		Simple_Text_Print_Wide(text, DSurface::Temp, &cliprect, &point2, color, 0, (TextPrintType)0x149, 1);
	}
}

// FUNCTION: 0x0070AD50
Cell TechnoClass::Find_Exit_Cell(TechnoClass const * techno, unsigned) const
{
	return(Docking_Coord((TechnoClass *)techno).As_Cell());
}

// FUNCTION: 0x0070ADA0
int TechnoClass::Refund_Amount(void) const
{
	return(Techno_Type_Class()->GetRefund(House, false));
}

// FUNCTION: 0x0070ADC0
void TechnoClass::Look(bool incremental, bool dontmap)
{
	if (IsInPlayfield && (!House->Class->IsMultiplayPassive || Session.Type == GAME_NORMAL)) {
		Coord position = PositionCoord;
		int sight_increase = 10 * (position.Z / Rule->LeptonsPerSightIncrease);
		if (sight_increase > SightIncrease) {
			incremental = false;
		}
		SightIncrease = sight_increase;

		int sight_range = Techno_Type_Class()->SightRange * (SightIncrease * 0.01 + 1.0);
		if (Has_Ability(ABILITY_SIGHT) && Rule->VeteranSight != 0.0) {
			sight_range *= Rule->VeteranSight;
		}

		if (sight_range) {
			Coord reveal = PositionCoord;
			Map.RevealArea1(&reveal, sight_range, House, incremental, dontmap, false, true, true);
		}
	}
}

// FUNCTION: 0x0070AF50
void TechnoClass::UpdateSight(bool incremental, bool flag, bool use_house, HouseClass * house, int override_range)
{
	if (IsInPlayfield && !House->Class->IsMultiplayPassive) {
		if (Techno_Type_Class()->SightRange != 0 || override_range != 0) {
			Coord position = PositionCoord;
			int sight_increase = 10 * (position.Z / Rule->LeptonsPerSightIncrease);
			if (sight_increase > SightIncrease) {
				incremental = false;
			}
			SightIncrease = sight_increase;

			int sight_range = Techno_Type_Class()->SightRange * (SightIncrease * 0.01 + 1.0);
			if (Has_Ability(ABILITY_SIGHT) && Rule->VeteranSight != 0.0) {
				sight_range *= Rule->VeteranSight;
			}
			if (override_range != 0) {
				sight_range = override_range;
			}

			if (!unknown_bool_250) {
				unknown_bool_250 = true;
				*(Coord *)LastSightCoords = PositionCoord;
				LastSightRange = sight_range;
				if (sight_range != 0) {
					if (use_house && house != NULL) {
						Coord reveal = PositionCoord;
						Map.RevealArea2(&reveal, sight_range, house, incremental, flag, false, true, false);
					} else {
						Coord reveal = PositionCoord;
						Map.RevealArea2(&reveal, sight_range, House, incremental, flag, false, true, false);
					}
				}
			} else if (use_house && sight_range != 0 && house != NULL) {
				Coord reveal = PositionCoord;
				Map.RevealArea2(&reveal, sight_range, house, incremental, flag, false, true, false);
			}
		}
	}
}

// FUNCTION: 0x0070B1D0
void TechnoClass::vt_entry_48C(bool incremental, bool flag, bool use_house, HouseClass * house)
{
	if (IsInPlayfield && !House->Class->IsMultiplayPassive) {
		if (unknown_bool_250) {
			int range = LastSightRange;
			unknown_bool_250 = false;
			if (range != 0) {
				if (use_house && house != NULL) {
					Map.RevealArea2((Coord *)LastSightCoords, range, house, incremental, flag, false, true, true);
					return;
				}
				if (LastSightRange != 0) {
					Map.RevealArea2((Coord *)LastSightCoords, range, House, incremental, flag, false, true, true);
				}
			}
		} else if (use_house && LastSightRange != 0 && house != NULL) {
			Map.RevealArea2((Coord *)LastSightCoords, LastSightRange, house, incremental, flag, false, true, true);
		}
	}
}

// FUNCTION: 0x0070BE50
int TechnoClass::Get_Predator_Offset(void) const
{
	return((Fetch_ID() + (int)WarpFactor) % 400);
}

// FUNCTION: 0x0070D150
void TechnoClass::Reset_Action_Line_Timer(void)
{
	ActionLineTimer = 25;
}

// FUNCTION: 0x0070E140
WeaponDataStruct const * TechnoClass::Get_Class_Weapon_Data(int which) const
{
	if (which == -1) return(NULL);

	if (Veterancy.Is_Elite()) {
		WeaponDataStruct const * weapon = ((TechnoTypeClass *)Techno_Type_Class())->GetEliteWeapon(which);
		if (weapon != NULL && weapon->Weapon != NULL) return(weapon);
	}
	return(((TechnoTypeClass *)Techno_Type_Class())->GetWeapon(which));
}
