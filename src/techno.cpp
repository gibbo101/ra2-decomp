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
#include "ebolt.h"
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
							Map[cell].IsAltUnknown = false;
							Map[cell].IsFogVisible = false;
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
								Map[cell].IsFogVisible = true;
								Map[cell].IsAltUnknown = true;
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
