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
#include "notify.h"
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



// FUNCTION: 0x006F2AD0 $E224
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

		if (!IsDiscoveredByPlayer && Map[cell].IsAltUnknown) {
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
			vt_entry_48C(0, 0, 1, (unsigned)PlayerPtr);
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
