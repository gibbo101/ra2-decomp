// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include <math.h>

#include "techtype.h"
#include "voc.h"

#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_tactica.h"
#include "airctype.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "cell.h"
#include "combat.h"
#include "fastmath.h"
#include "findmake.h"
#include "globals.h"
#include "ilocos.h"
#include "infatype.h"
#include "mixfile.h"
#include "notify.h"
#include "psystype.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "swizzle.h"
#include "tactical.h"
#include "tracker.h"
#include "unittype.h"
#include "vanimtype.h"
#include "weapon.h"

// SYNTHETIC: GAMEMD 0x007108E0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00710910 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00710930 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00710950 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00710970 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00710990 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x007109B0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x007109F0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00710A20 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00710A50 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00710A80 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00710A90 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00710AB0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00710AD0 SYMBOL
// _$E41


extern char DAT_00889F64[];

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

DynamicVectorClass<TechnoTypeClass *> TechnoTypeClass::Array;

// FUNCTION: GAMEMD 0x00711840
TechnoTypeClass::TechnoTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	VeteranAbilities(x),
	EliteAbilities(x),
	DebrisTypes(x),
	DebrisMaximums(x),
	Unused2(0),
	Dock(x),
	VoiceSelect(x),
	VoiceSelectEnslaved(x),
	VoiceSelectDeactivated(x),
	VoiceMove(x),
	VoiceAttack(x),
	VoiceSpecialAttack(x),
	VoiceDie(x),
	VoiceFeedback(x),
	MoveSound(x),
	DieSound(x),
	DebrisAnims(x),
	Prerequisite(x),
	PrerequisiteOverride(x),
	CameoFilename(NULL),
	AltCameoFilename(NULL),
	Explosion(x),
	DestroyAnim(x),
	DamageParticleSystems(),
	DestroyParticleSystems(),
	TurretAnimData(2, 1, 1, 1),
	BarrelAnimData(2, 1, 1, 1)
{
	bool found = false;
	for (int i = 0; i < Array.Count(); i++) {
		if (Array[i] == this) {
			found = true;
			break;
		}
	}
	if (!found) {
		Array.Add(this);
	}
}

// FUNCTION: GAMEMD 0x00711AE0
TechnoTypeClass::~TechnoTypeClass(void)
{
	if (CameoAllocated && CameoData != NULL) {
		delete (char *)CameoData;
		CameoData = NULL;
		CameoAllocated = false;
	}
	if (AltCameoAllocated && AltCameoData != NULL) {
		delete (char *)AltCameoData;
		AltCameoData = NULL;
		AltCameoAllocated = false;
	}
	PointerExpiredNotification::NotifyInvalidType.Delete(this);
	Array.Delete(this);
}

// FUNCTION: GAMEMD 0x00711E80
bool TechnoTypeClass::CanUseWaypoint(void) const
{
	return(true);
}

// FUNCTION: GAMEMD 0x00711E90
bool TechnoTypeClass::CanAttackMove(void) const
{
	if (Weapons[0].Weapon != NULL) {
		if (!PreventAttackMove) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00711EB0
int TechnoTypeClass::Raw_Cost(void) const
{
	return(Cost);
}

// FUNCTION: GAMEMD 0x00711EC0
int TechnoTypeClass::Get_Ownable(void) const
{
	if (IsDoubleOwned && Session.Type != GAME_NORMAL) {
		return(0x7FFFFFFF);
	}
	return(Ownable);
}

// FUNCTION: GAMEMD 0x00711EE0
int TechnoTypeClass::Time_To_Build(void) const
{
	return(Cost * Rule->BuildSpeedBias * (TICKS_PER_MINUTE / 1000.));
}

// FUNCTION: GAMEMD 0x00711F00
int TechnoTypeClass::Cost_Of(HouseClass * house) const
{
	if (house != NULL) {
		float a = house->FUN_0050BDF0(this);
		float b = house->FUN_0050BEB0(this);
		return(Raw_Cost() * b * a);
	}
	return(Raw_Cost());
}

// FUNCTION: GAMEMD 0x00711F60
int TechnoTypeClass::GetRefund(HouseClass * house, bool unknown) const
{
	float refund = Rule->RefundPercent;
	if (unknown) {
		refund = 1.0f;
	}
	if (house != NULL) {
		float a = house->FUN_0050BDF0(this);
		float b = house->FUN_0050BEB0(this);
		if (Soylent != 0) {
			return(Soylent * a);
		}
		int c = Raw_Cost() * b * a;
		if (house->IsControlledByHuman()) {
			c = c * refund;
		}
		return(c);
	}
	return(Raw_Cost() * refund);
}

// FUNCTION: GAMEMD 0x00712040
void const * TechnoTypeClass::Get_Cameo_Data(void) const
{
	if (PlayerPtr != NULL && PlayerPtr->BarracksInfiltrated) {
		if (Fetch_RTTI() == RTTI_INFANTRYTYPE && AltCameoData != NULL && IsTrainable) {
			return(AltCameoData);
		}
	}
	if (PlayerPtr != NULL && PlayerPtr->WarFactoryInfiltrated) {
		if (Fetch_RTTI() == RTTI_UNITTYPE || (Fetch_RTTI() == RTTI_BUILDINGTYPE && UndeploysInto != NULL)) {
			if (AltCameoData != NULL && IsTrainable) {
				return(AltCameoData);
			}
		}
	}
	return(CameoData);
}

// FUNCTION: GAMEMD 0x007120D0
int TechnoTypeClass::Repair_Cost(void) const
{
	int cost = (Raw_Cost()/(MaxStrength/Rule->RepairStep)) * Rule->RepairPercent;
	return(MAX(cost, 1));
}

// FUNCTION: GAMEMD 0x00712120
int TechnoTypeClass::Repair_Step(void) const
{
	return(Rule->RepairStep);
}

// FUNCTION: GAMEMD 0x00712130
bool TechnoTypeClass::Is_Two_Shooter(void) const
{
	WeaponTypeClass *pri = Weapons[0].Weapon;

	if (pri != NULL) {
		WeaponTypeClass *sec = Weapons[1].Weapon;

		if (pri == sec || pri->Burst > 1) {
			return(true);
		}

		if (sec != NULL && sec->Burst > 1) {
			return(true);
		}
	}
	return(false);
}

int __fastcall FUN_00626DA0(void);
void __cdecl FUN_004068E0(char const * format, ...);
void Exit_Process(int);
bool FUN_00717AE0(VoxelLibrary * library);

static inline int Read_Sound(CCINIClass const & ini, char const * section, char const * key, int fallback)
{
	char sound[128];
	if (ini.Get_String(section, key, "", sound, sizeof(sound)) != 0) {
		int index = VocClass::FindIndex(sound);
		if (index != -1)
			return index;
	}
	return fallback;
}

static inline int _Scale_To_256(int val)
{
	val = MIN(val, 100);
	val = MAX(val, 0);
	val = ((val * (MPH_LIGHT_SPEED + 1)) / 100);
	val = MIN(val, MPH_LIGHT_SPEED);
	return (val);
}

// FUNCTION: GAMEMD 0x00712170
bool TechnoTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		LandTargeting = ini.Get_Int(IniName, "LandTargeting", LandTargeting);
		NavalTargeting = ini.Get_Int(IniName, "NavalTargeting", NavalTargeting);
		Speed = ini.Get_SpeedType(IniName, "SpeedType", Speed);
		CanBeHidden = ArtINI.Get_Bool(IniName, "CanBeHidden", CanBeHidden);
		IsTypeImmune = ini.Get_Bool(Name(), "TypeImmune", IsTypeImmune);
		WalkRate = ini.Get_Int(Name(), "WalkRate", WalkRate);
		IdleRate = ini.Get_Int(IniName, "IdleRate", IdleRate);
		IsMoveToShroud = ini.Get_Bool(Name(), "MoveToShroud", IsMoveToShroud);
		IsTrain = ini.Get_Bool(IniName, "IsTrain", IsTrain);
		IsDoubleOwned = ini.Get_Bool(IniName, "DoubleOwned", IsDoubleOwned);
		ThreatRange = ini.Get_Lepton(IniName, "GuardRange", ThreatRange);
		IsExploding = ini.Get_Bool(IniName, "Explodes", IsExploding);
		DeathWeapon = TGet_Class(ini, IniName, "DeathWeapon", DeathWeapon);
		DeathWeaponDamageModifier = ini.Get_Float(IniName, "DeathWeaponDamageModifier", DeathWeaponDamageModifier);
		FlightLevel = ini.Get_Int(IniName, "FlightLevel", FlightLevel);
		IsDropship = ini.Get_Bool(IniName, "IsDropship", IsDropship);
		{
			const double pitch = ini.Get_Float(IniName, "PitchAngle", -1.0);
			if (pitch != -1.0)
				PitchAngle = pitch * (3.14159265358979323846 / 180.0);
			const double roll = ini.Get_Float(IniName, "RollAngle", -1.0);
			if (roll != -1.0)
				RollAngle = roll * (3.14159265358979323846 / 180.0);
		}
		PitchSpeed = ini.Get_Float(IniName, "PitchSpeed", PitchSpeed);
		Locomotor = ini.Get_CLSID(IniName, "Locomotor", Locomotor);
		CloakingSpeed = ini.Get_Int(IniName, "CloakingSpeed", CloakingSpeed);
		ThreatAvoidanceCoefficient = ini.Get_Float(IniName, "ThreatAvoidanceCoefficient", ThreatAvoidanceCoefficient);
		SlowdownDistance = ini.Get_Int(IniName, "SlowdownDistance", SlowdownDistance);
		DeaccelerationFactor = ini.Get_Float(IniName, "DeaccelerationFactor", DeaccelerationFactor);
		AccelerationFactor = ini.Get_Float(IniName, "AccelerationFactor", AccelerationFactor);
		Weight = ini.Get_Float(IniName, "Weight", Weight);
		PhysicalSize = ini.Get_Float(IniName, "PhysicalSize", PhysicalSize);
		Size = ini.Get_Float(IniName, "Size", Size);
		SizeLimit = ini.Get_Float(IniName, "SizeLimit", SizeLimit);
		HoverAttack = ini.Get_Bool(IniName, "HoverAttack", HoverAttack);
		VHPScan = ini.Get_VHPScan(IniName, "VHPScan", VHPScan);
		MaxDebris = ini.Get_Int(IniName, "MaxDebris", MaxDebris);
		MinDebris = ini.Get_Int(IniName, "MinDebris", MinDebris);
		if (MinDebris < 0)
			MinDebris = 0;
		if (MaxDebris < MinDebris)
			MaxDebris = MinDebris;

		DebrisTypes = TGet_TypeList<VoxelAnimTypeClass>(ini, IniName, "DebrisTypes", DebrisTypes);
		DebrisMaximums = ini.Get_IntList(IniName, "DebrisMaximums", DebrisMaximums);
		{
			TypeList<AnimTypeClass const *> const & parsed = TGet_TypeList<AnimTypeClass>(ini, IniName, "DebrisAnims", DebrisAnims);
			DebrisAnims = parsed;
		}

		HasTurretTooltips = ini.Get_Bool(IniName, "HasTurretTooltips", HasTurretTooltips);
		TurretCount = ini.Get_Int(IniName, "TurretCount", TurretCount);
		WeaponCount = ini.Get_Int(IniName, "WeaponCount", WeaponCount);
		IsChargeTurret = ini.Get_Bool(IniName, "IsChargeTurret", IsChargeTurret);
		ClearAllWeapons = ini.Get_Bool(IniName, "ClearAllWeapons", ClearAllWeapons);
		if (TurretCount > 0) {
			for (int index = 0; index < WeaponCount; index++) {
				char key[12], elite[16];
				sprintf(key, "Weapon%d", index + 1);
				sprintf(elite, "EliteWeapon%d", index + 1);
				Weapons[index].Weapon = TGet_Class(ini, IniName, key, Weapons[index].Weapon);
				EliteWeapons[index].Weapon = TGet_Class(ini, IniName, elite, EliteWeapons[index].Weapon);
			}
		} else if (!ClearAllWeapons) {
			Weapons[0].Weapon = TGet_Class(ini, IniName, "Primary", Weapons[0].Weapon);
			Weapons[1].Weapon = TGet_Class(ini, IniName, "Secondary", Weapons[1].Weapon);
			EliteWeapons[0].Weapon = TGet_Class(ini, IniName, "ElitePrimary", EliteWeapons[0].Weapon);
			EliteWeapons[1].Weapon = TGet_Class(ini, IniName, "EliteSecondary", EliteWeapons[1].Weapon);
		}
		if (ClearAllWeapons) {
			Weapons[0].Weapon = 0;
			Weapons[1].Weapon = 0;
			EliteWeapons[0].Weapon = 0;
			EliteWeapons[1].Weapon = 0;
		}

		VoiceMove = ini.Get_VocType_List(ini, IniName, "VoiceMove", VoiceMove);
		VoiceSelect = ini.Get_VocType_List(ini, IniName, "VoiceSelect", VoiceSelect);
		VoiceSelectEnslaved = ini.Get_VocType_List(ini, IniName, "VoiceSelectEnslaved", VoiceSelectEnslaved);
		VoiceSelectDeactivated = ini.Get_VocType_List(ini, IniName, "VoiceSelectDeactivated", VoiceSelectDeactivated);
		VoiceAttack = ini.Get_VocType_List(ini, IniName, "VoiceAttack", VoiceAttack);
		VoiceSpecialAttack = ini.Get_VocType_List(ini, IniName, "VoiceSpecialAttack", VoiceSpecialAttack);
		VoiceDie = ini.Get_VocType_List(ini, IniName, "VoiceDie", VoiceDie);
		VoiceFeedback = ini.Get_VocType_List(ini, IniName, "VoiceFeedback", VoiceFeedback);

		AuxSound1 = (VocType)Read_Sound(ini, IniName, "AuxSound1", AuxSound1);
		AuxSound2 = (VocType)Read_Sound(ini, IniName, "AuxSound2", AuxSound2);
		char const * sound_section = IniName;
		CreateSound = Read_Sound(ini, sound_section, "CreateSound", CreateSound);
		DamageSound = Read_Sound(ini, sound_section, "DamageSound", DamageSound);
		ImpactWaterSound = Read_Sound(ini, sound_section, "ImpactWaterSound", ImpactWaterSound);
		ImpactLandSound = Read_Sound(ini, sound_section, "ImpactLandSound", ImpactLandSound);
		CrashingSound = Read_Sound(ini, sound_section, "CrashingSound", CrashingSound);
		SinkingSound = Read_Sound(ini, sound_section, "SinkingSound", SinkingSound);
		VoiceFalling = Read_Sound(ini, IniName, "VoiceFalling", VoiceFalling);
		VoiceCrashing = Read_Sound(ini, IniName, "VoiceCrashing", VoiceCrashing);
		VoiceSinking = Read_Sound(ini, IniName, "VoiceSinking", VoiceSinking);
		VoiceEnter = Read_Sound(ini, IniName, "VoiceEnter", VoiceEnter);
		VoiceCapture = Read_Sound(ini, IniName, "VoiceCapture", VoiceCapture);
		IsCloakStop = ini.Get_Bool(IniName, "CloakStop", IsCloakStop);
		Capacity = ini.Get_Int(IniName, "Storage", Capacity);
		BuildLimit = ini.Get_Int(IniName, "BuildLimit", BuildLimit);
		Category = ini.Get_CategoryType(IniName, "Category", Category);

		Dock = TGet_TypeList<BuildingTypeClass>(ini, IniName, "Dock", Dock);
		DeploysInto = TGet_Class(ini, IniName, "DeploysInto", DeploysInto);
		UndeploysInto = TGet_Class(ini, IniName, "UndeploysInto", UndeploysInto);
		PowersUnit = TGet_Class(ini, IniName, "PowersUnit", PowersUnit);
		PoweredUnit = ini.Get_Bool(IniName, "PoweredUnit", PoweredUnit);
		IsLightningRod = ini.Get_Bool(IniName, "LightningRod", IsLightningRod);
		IsManualReload = ini.Get_Bool(IniName, "ManualReload", IsManualReload);
		TurretSpins = ini.Get_Bool(IniName, "TurretSpins", TurretSpins);
		TiltCrashJumpjet = ini.Get_Bool(IniName, "TiltCrashJumpjet", TiltCrashJumpjet);
		IsTurretEquipped = ini.Get_Bool(IniName, "Turret", IsTurretEquipped);
		TurretRotateSound = Read_Sound(ini, IniName, "TurretRotateSound", TurretRotateSound);
		EnterTransportSound = Read_Sound(ini, IniName, "EnterTransportSound", EnterTransportSound);
		LeaveTransportSound = Read_Sound(ini, IniName, "LeaveTransportSound", LeaveTransportSound);
		MoveSound = ini.Get_VocType_List(ini, IniName, "MoveSound", MoveSound);
		DieSound = ini.Get_VocType_List(ini, IniName, "DieSound", DieSound);
		DeploySound = Read_Sound(ini, IniName, "DeploySound", DeploySound);
		UndeploySound = Read_Sound(ini, IniName, "UndeploySound", UndeploySound);
		ChronoInSound = Read_Sound(ini, IniName, "ChronoInSound", ChronoInSound);
		ChronoOutSound = Read_Sound(ini, IniName, "ChronoOutSound", ChronoOutSound);
		VoiceHarvest = Read_Sound(ini, IniName, "VoiceHarvest", VoiceHarvest);
		VoicePrimaryWeaponAttack = Read_Sound(ini, IniName, "VoicePrimaryWeaponAttack", VoicePrimaryWeaponAttack);
		VoicePrimaryEliteWeaponAttack = Read_Sound(ini, IniName, "VoicePrimaryEliteWeaponAttack", VoicePrimaryEliteWeaponAttack);
		VoiceSecondaryWeaponAttack = Read_Sound(ini, IniName, "VoiceSecondaryWeaponAttack", VoiceSecondaryWeaponAttack);
		VoiceSecondaryEliteWeaponAttack = Read_Sound(ini, IniName, "VoiceSecondaryEliteWeaponAttack", VoiceSecondaryEliteWeaponAttack);
		VoiceDeploy = Read_Sound(ini, IniName, "VoiceDeploy", VoiceDeploy);
		VoiceUndeploy = Read_Sound(ini, IniName, "VoiceUndeploy", VoiceUndeploy);
		EnterGrinderSound = Read_Sound(ini, IniName, "EnterGrinderSound", EnterGrinderSound);
		LeaveGrinderSound = Read_Sound(ini, IniName, "LeaveGrinderSound", LeaveGrinderSound);
		EnterBioReactorSound = Read_Sound(ini, IniName, "EnterBioReactorSound", EnterBioReactorSound);
		LeaveBioReactorSound = Read_Sound(ini, IniName, "LeaveBioReactorSound", LeaveBioReactorSound);
		ActivateSound = Read_Sound(ini, IniName, "ActivateSound", ActivateSound);
		DeactivateSound = Read_Sound(ini, IniName, "DeactivateSound", DeactivateSound);
		MindClearedSound = Read_Sound(ini, IniName, "MindClearedSound", MindClearedSound);

		Explosion = TGet_TypeList<AnimTypeClass>(ini, IniName, "Explosion", Explosion);
		DestroyAnim = TGet_TypeList<AnimTypeClass>(ini, IniName, "DestroyAnim", DestroyAnim);
		NaturalParticleSystem = TGet_Class(ini, IniName, "NaturalParticleSystem", NaturalParticleSystem);
		RefinerySmokeParticleSystem = TGet_Class(ini, IniName, "RefinerySmokeParticleSystem", RefinerySmokeParticleSystem);
		NaturalParticleLocation = ini.Get_Offset(IniName, "NaturalParticleLocation", NaturalParticleLocation);
		DamageParticleSystems = TGet_TypeList<ParticleSystemTypeClass>(ini, IniName, "DamageParticleSystems", DamageParticleSystems);
		DestroyParticleSystems = TGet_TypeList<ParticleSystemTypeClass>(ini, IniName, "DestroyParticleSystems", DestroyParticleSystems);
		DamageSmokeOffset = ini.Get_Offset(IniName, "DamageSmokeOffset", DamageSmokeOffset);
		DamSmkOffScrnRel = ini.Get_Bool(IniName, "DamSmkOffScrnRel", DamSmkOffScrnRel);
		DestroySmokeOffset = ini.Get_Offset(IniName, "DestroySmokeOffset", DestroySmokeOffset);
		RefinerySmokeOffsetOne = ini.Get_Offset(IniName, "RefinerySmokeOffsetOne", RefinerySmokeOffsetOne);
		RefinerySmokeOffsetTwo = ini.Get_Offset(IniName, "RefinerySmokeOffsetTwo", RefinerySmokeOffsetTwo);
		RefinerySmokeOffsetThree = ini.Get_Offset(IniName, "RefinerySmokeOffsetThree", RefinerySmokeOffsetThree);
		RefinerySmokeOffsetFour = ini.Get_Offset(IniName, "RefinerySmokeOffsetFour", RefinerySmokeOffsetFour);
		IsNominal = ini.Get_Bool(IniName, "Nominal", IsNominal);
		DontScore = ini.Get_Bool(IniName, "DontScore", DontScore);
		DamageSelf = ini.Get_Bool(IniName, "DamageSelf", DamageSelf);
		IsCloakable = ini.Get_Bool(IniName, "Cloakable", IsCloakable);
		GapGenerator = ini.Get_Bool(IniName, "GapGenerator", GapGenerator);
		GapRadiusInCells = static_cast<char>(ini.Get_Int(IniName, "GapRadiusInCells", GapRadiusInCells));
		SuperGapRadiusInCells = static_cast<char>(ini.Get_Int(IniName, "SuperGapRadiusInCells", SuperGapRadiusInCells));
		Teleporter = ini.Get_Bool(IniName, "Teleporter", Teleporter);
		IsScanner = ini.Get_Bool(IniName, "Sensors", IsScanner);
		IsGattling = ini.Get_Bool(IniName, "IsGattling", IsGattling);
		WeaponStages = ini.Get_Int(IniName, "WeaponStages", WeaponStages);
		RateUp = ini.Get_Int(IniName, "RateUp", RateUp);
		RateDown = ini.Get_Int(IniName, "RateDown", RateDown);
		if (IsGattling && WeaponStages > 1) {
			for (int index = 0; index < WeaponStages; ++index) {
				char key[8], elite_key[12];
				sprintf(key, "Stage%d", index + 1);
				sprintf(elite_key, "EliteStage%d", index + 1);
				WeaponStage[index] = ini.Get_Int(IniName, key, WeaponStage[index]);
				EliteStage[index] = ini.Get_Int(IniName, elite_key, EliteStage[index]);
			}
		}
		PipScale = ini.Get_PipScaleType(IniName, "PipScale", PipScale);
		PipsDrawForAll = ini.Get_Bool(IniName, "PipsDrawForAll", PipsDrawForAll);
		LeptonMindControlOffset = ini.Get_Int(IniName, "LeptonMindControlOffset", LeptonMindControlOffset);
		PixelSelectionBracketDelta = ini.Get_Int(IniName, "PixelSelectionBracketDelta", PixelSelectionBracketDelta);
		PipWrap = ini.Get_Int(IniName, "PipWrap", PipWrap);
		Prerequisite = ini.Get_BuildingType_List(ini, IniName, "Prerequisite", Prerequisite);
		PrerequisiteOverride = ini.Get_BuildingType_List(ini, IniName, "PrerequisiteOverride", PrerequisiteOverride);

		SightRange = ini.Get_Int(IniName, "Sight", SightRange);
		ReselectIfLimboed = ini.Get_Bool(IniName, "ReselectIfLimboed", ReselectIfLimboed);
		RejoinTeamIfLimboed = ini.Get_Bool(IniName, "RejoinTeamIfLimboed", RejoinTeamIfLimboed);
		SensorsSight = ini.Get_Int(IniName, "SensorsSight", SensorsSight);
		DetectDisguiseRange = ini.Get_Int(IniName, "DetectDisguiseRange", DetectDisguiseRange);
		BombSight = ini.Get_Int(IniName, "BombSight", BombSight);
		LeadershipRating = ini.Get_Int(IniName, "LeadershipRating", LeadershipRating);
		MindControlRingOffset = ini.Get_Int(IniName, "MindControlRingOffset", MindControlRingOffset);
		BuildTimeMultiplier = ini.Get_Float(IniName, "BuildTimeMultiplier", BuildTimeMultiplier);
		RevealToAll = ini.Get_Bool(IniName, "RevealToAll", RevealToAll);
		Drainable = ini.Get_Bool(IniName, "Drainable", Drainable);
		OpenTopped = ini.Get_Bool(IniName, "OpenTopped", OpenTopped);
		ResourceGatherer = ini.Get_Bool(IniName, "ResourceGatherer", ResourceGatherer);
		ResourceDestination = ini.Get_Bool(IniName, "ResourceDestination", ResourceDestination);
		CanDisguise = ini.Get_Bool(IniName, "CanDisguise", CanDisguise);
		PermaDisguise = ini.Get_Bool(IniName, "PermaDisguise", PermaDisguise);
		DetectDisguise = ini.Get_Bool(IniName, "DetectDisguise", DetectDisguise);
		DisguiseWhenStill = ini.Get_Bool(IniName, "DisguiseWhenStill", DisguiseWhenStill);
		CanPassiveAquire = ini.Get_Bool(IniName, "CanPassiveAquire", CanPassiveAquire);
		CanRetaliate = ini.Get_Bool(IniName, "CanRetaliate", CanRetaliate);
		CanApproachTarget = ini.Get_Bool(IniName, "CanApproachTarget", CanApproachTarget);
		CanRecalcApproachTarget = ini.Get_Bool(IniName, "CanRecalcApproachTarget", CanRecalcApproachTarget);
		RequiresStolenThirdTech = ini.Get_Bool(IniName, "RequiresStolenThirdTech", RequiresStolenThirdTech);
		RequiresStolenSovietTech = ini.Get_Bool(IniName, "RequiresStolenSovietTech", RequiresStolenSovietTech);
		RequiresStolenAlliedTech = ini.Get_Bool(IniName, "RequiresStolenAlliedTech", RequiresStolenAlliedTech);
		RequiredHouses = ini.Get_Owners(IniName, "RequiredHouses", RequiredHouses);
		SecretHouses = ini.Get_Owners(IniName, "SecretHouses", SecretHouses);
		ForbiddenHouses = ini.Get_Owners(IniName, "ForbiddenHouses", ForbiddenHouses);
		Level = ini.Get_Int(IniName, "TechLevel", Level);
		AirstrikeTeam = ini.Get_Int(IniName, "AirstrikeTeam", AirstrikeTeam);
		EliteAirstrikeTeam = ini.Get_Int(IniName, "EliteAirstrikeTeam", AirstrikeTeam);
		AirstrikeTeamType = TGet_Class(ini, IniName, "AirstrikeTeamType", AirstrikeTeamType);
		EliteAirstrikeTeamType = TGet_Class(ini, IniName, "EliteAirstrikeTeamType", AirstrikeTeamType);
		AirstrikeRechargeTime = ini.Get_Int(IniName, "AirstrikeRechargeTime", AirstrikeRechargeTime);
		EliteAirstrikeRechargeTime = ini.Get_Int(IniName, "EliteAirstrikeRechargeTime", AirstrikeRechargeTime);
		int maxspeed = ini.Get_Int(IniName, "Speed", -1);
		if (maxspeed != -1) {
			MaxSpeed = MPHType(_Scale_To_256(maxspeed));
		}
		Cost = ini.Get_Int(IniName, "Cost", Cost);
		Soylent = ini.Get_Int(IniName, "Soylent", Soylent);
		UnloadingClass = TGet_Class(ini, IniName, "UnloadingClass", UnloadingClass);
		DeployingAnim = TGet_Class(ini, IniName, "DeployingAnim", DeployingAnim);
		InitialAmmo = ini.Get_Int(IniName, "InitialAmmo", InitialAmmo);
		MaxAmmo = ini.Get_Int(IniName, "Ammo", MaxAmmo);
		IFVMode = ini.Get_Int(IniName, "IFVMode", IFVMode);
		AirRangeBonus = ini.Get_Lepton(IniName, "AirRangeBonus", AirRangeBonus);
		RadialFireSegments = ini.Get_Int(IniName, "RadialFireSegments", RadialFireSegments);
		DeployFireWeapon = ini.Get_Int(IniName, "DeployFireWeapon", DeployFireWeapon);
		DeployFire = ini.Get_Bool(IniName, "DeployFire", DeployFire);
		DeployToLand = ini.Get_Bool(IniName, "DeployToLand", DeployToLand);
		MobileFire = ini.Get_Bool(IniName, "MobileFire", MobileFire);
		OpportunityFire = ini.Get_Bool(IniName, "OpportunityFire", OpportunityFire);
		DistributedFire = ini.Get_Bool(IniName, "DistributedFire", DistributedFire);
		Reload = ini.Get_Int(IniName, "Reload", Reload);
		EmptyReload = ini.Get_Int(IniName, "EmptyReload", EmptyReload);
		ReloadIncrement = ini.Get_Int(IniName, "ReloadIncrement", ReloadIncrement);
		DamageReducesReadiness = ini.Get_Bool(IniName, "DamageReducesReadiness", DamageReducesReadiness);
		ReadinessReductionMultiplier = ini.Get_Float(IniName, "ReadinessReductionMultiplier", ReadinessReductionMultiplier);
		BerserkFriendly = ini.Get_Bool(IniName, "BerserkFriendly", BerserkFriendly);
		SprayAttack = ini.Get_Bool(IniName, "SprayAttack", SprayAttack);
		Pushy = ini.Get_Bool(IniName, "Pushy", Pushy);
		Natural = ini.Get_Bool(IniName, "Natural", Natural);
		Unnatural = ini.Get_Bool(IniName, "Unnatural", Unnatural);
		CloseRange = ini.Get_Bool(IniName, "CloseRange", CloseRange);
		PreventAttackMove = ini.Get_Bool(IniName, "PreventAttackMove", PreventAttackMove);
		Reward = Points = ini.Get_Int(IniName, "Points", Points);
		Risk = ini.Get_Int(IniName, "ThreatPosed", Risk);
		Ownable = ini.Get_Owners(IniName, "Owner", Ownable);
		AIBasePlanningSide = ini.Get_Int(IniName, "AIBasePlanningSide", AIBasePlanningSide);
		IsTrainable = ini.Get_Bool(IniName, "Trainable", IsTrainable);
		IsCrew = ini.Get_Bool(IniName, "Crewed", IsCrew);
		Gunner = ini.Get_Bool(IniName, "Gunner", Gunner);
		Naval = ini.Get_Bool(IniName, "Naval", Naval);
		IsRepairable = ini.Get_Bool(IniName, "Repairable", IsRepairable);
		IsInvisible = ini.Get_Bool(IniName, "Invisible", IsInvisible);
		IsRadarVisible = ini.Get_Bool(IniName, "RadarVisible", IsRadarVisible);
		IsSelfHealing = ini.Get_Bool(IniName, "SelfHealing", IsSelfHealing);
		IsNoAutoFire = ini.Get_Bool(IniName, "NoAutoFire", IsNoAutoFire);
		ROT = ini.Get_Int(IniName, "ROT", ROT);
		MaxPassengers = ini.Get_Int(IniName, "Passengers", MaxPassengers);
		FireAngle = ini.Get_Int(IniName, "FireAngle", FireAngle);
		DeployTime = ini.Get_Float(IniName, "DeployTime", DeployTime);
		UndeployDelay = ini.Get_Int(IniName, "UndeployDelay", UndeployDelay);
		IsDisableable = ini.Get_Bool(IniName, "Disableable", IsDisableable);
		IsToProtect = ini.Get_Bool(IniName, "ToProtect", IsToProtect);
		IsTiberiumHeal = ini.Get_Bool(IniName, "TiberiumHeal", IsTiberiumHeal);
		IsImmuneToVeins = ini.Get_Bool(IniName, "ImmuneToVeins", IsImmuneToVeins);
		IsAllowedToStartInMultiplayer = ini.Get_Bool(IniName, "AllowedToStartInMultiplayer", IsAllowedToStartInMultiplayer);
		StupidHunt = ini.Get_Bool(IniName, "StupidHunt", StupidHunt);
		IsTargetLaser = ini.Get_Bool(IniName, "TargetLaser", IsTargetLaser);
		IsHunterSeeker = ini.Get_Bool(IniName, "HunterSeeker", IsHunterSeeker);
		IsCrusher = ini.Get_Bool(IniName, "Crusher", IsCrusher);
		OmniCrusher = ini.Get_Bool(IniName, "OmniCrusher", OmniCrusher);
		OmniCrushResistant = ini.Get_Bool(IniName, "OmniCrushResistant", OmniCrushResistant);
		IsAutoCrush = ini.Get_Bool(IniName, "AutoCrush", IsAutoCrush);
		ImmuneToRadiation = ini.Get_Bool(IniName, "ImmuneToRadiation", ImmuneToRadiation);
		Underwater = ini.Get_Bool(IniName, "Underwater", Underwater);
		BalloonHover = ini.Get_Bool(IniName, "BalloonHover", BalloonHover);
		Slaved = ini.Get_Bool(IniName, "Slaved", Slaved);
		Enslaves = TGet_Class(ini, IniName, "Enslaves", Enslaves);
		SlaveRegenRate = ini.Get_Int(IniName, "SlaveRegenRate", SlaveRegenRate);
		SlavesNumber = ini.Get_Int(IniName, "SlavesNumber", SlavesNumber);
		SlaveReloadRate = ini.Get_Int(IniName, "SlaveReloadRate", SlaveReloadRate);
		OpenTransportWeapon = ini.Get_Int(IniName, "OpenTransportWeapon", OpenTransportWeapon);
		Spawned = ini.Get_Bool(IniName, "Spawned", Spawned);
		Spawns = TGet_Class(ini, IniName, "Spawns", Spawns);
		SpawnRegenRate = ini.Get_Int(IniName, "SpawnRegenRate", SpawnRegenRate);
		SpawnsNumber = ini.Get_Int(IniName, "SpawnsNumber", SpawnsNumber);
		SpawnReloadRate = ini.Get_Int(IniName, "SpawnReloadRate", SpawnReloadRate);
		MissileSpawn = ini.Get_Bool(IniName, "MissileSpawn", MissileSpawn);
		DefaultToGuardArea = ini.Get_Bool(IniName, "DefaultToGuardArea", DefaultToGuardArea);
		Warpable = ini.Get_Bool(IniName, "Warpable", Warpable);
		Parasiteable = ini.Get_Bool(IniName, "Parasiteable", Parasiteable);
		ImmuneToPsionics = ini.Get_Bool(IniName, "ImmuneToPsionics", ImmuneToPsionics);
		ImmuneToPsionicWeapons = ini.Get_Bool(IniName, "ImmuneToPsionicWeapons", ImmuneToPsionicWeapons);
		ConsideredAircraft = ini.Get_Bool(IniName, "ConsideredAircraft", ConsideredAircraft);
		Bunkerable = ini.Get_Bool(IniName, "Bunkerable", Bunkerable);
		Organic = ini.Get_Bool(IniName, "Organic", Organic);
		ImmuneToPoison = ini.Get_Bool(IniName, "ImmuneToPoison", ImmuneToPoison);
		SuppressionThreshold = ini.Get_Int(IniName, "SuppressionThreshold", SuppressionThreshold);
		NoShadow = ini.Get_Bool(IniName, "NoShadow", NoShadow);
		JumpjetTurnRate = ini.Get_Int(IniName, "JumpjetTurnRate", JumpjetTurnRate);
		JumpjetSpeed = ini.Get_Int(IniName, "JumpjetSpeed", JumpjetSpeed);
		JumpjetClimb = ini.Get_Float(IniName, "JumpjetClimb", JumpjetClimb);
		JumpjetCrash = ini.Get_Float(IniName, "JumpjetCrash", JumpjetCrash);
		JumpjetHeight = ini.Get_Int(IniName, "JumpjetHeight", JumpjetHeight);
		JumpjetAccel = ini.Get_Float(IniName, "JumpjetAccel", JumpjetAccel);
		JumpjetWobbles = ini.Get_Float(IniName, "JumpjetWobbles", JumpjetWobbles);
		JumpjetNoWobbles = ini.Get_Bool(IniName, "JumpjetNoWobbles", JumpjetNoWobbles);
		JumpjetDeviation = ini.Get_Int(IniName, "JumpjetDeviation", JumpjetDeviation);
		JumpJet = ini.Get_Bool(IniName, "JumpJet", JumpJet);
		Crashable = ini.Get_Bool(IniName, "Crashable", Crashable);
		AttackFriendlies = ini.Get_Bool(IniName, "AttackFriendlies", AttackFriendlies);
		AttackCursorOnFriendlies = ini.Get_Bool(IniName, "AttackCursorOnFriendlies", AttackCursorOnFriendlies);
		TurretRecoil = ini.Get_Bool(IniName, "TurretRecoil", TurretRecoil);
		TurretAnimData.FUN_00717A30(ini.Get_Int(IniName, "TurretTravel", TurretAnimData.FUN_00717A40()));
		TurretAnimData.FUN_00717A50(ini.Get_Int(IniName, "TurretCompressFrames", TurretAnimData.FUN_00717A70()));
		TurretAnimData.FUN_00717AB0(ini.Get_Int(IniName, "TurretHoldFrames", TurretAnimData.FUN_00717AD0()));
		TurretAnimData.FUN_00717A80(ini.Get_Int(IniName, "TurretRecoverFrames", TurretAnimData.FUN_00717AA0()));
		BarrelAnimData = TurretAnimData;
		BarrelAnimData.FUN_00717A30(ini.Get_Int(IniName, "BarrelTravel", TurretAnimData.FUN_00717A40()));
		BarrelAnimData.FUN_00717A50(ini.Get_Int(IniName, "BarrelCompressFrames", TurretAnimData.FUN_00717A70()));
		BarrelAnimData.FUN_00717AB0(ini.Get_Int(IniName, "BarrelHoldFrames", TurretAnimData.FUN_00717AD0()));
		BarrelAnimData.FUN_00717A80(ini.Get_Int(IniName, "BarrelRecoverFrames", TurretAnimData.FUN_00717AA0()));
		IsTiltsWhenCrushes = ini.Get_Bool(IniName, "TiltsWhenCrushes", IsTiltsWhenCrushes);
		IsAccelerates = ini.Get_Bool(IniName, "Accelerates", IsAccelerates);
		ZFudgeCliff = ini.Get_Int(IniName, "ZFudgeCliff", ZFudgeCliff);
		ZFudgeColumn = ini.Get_Int(IniName, "ZFudgeColumn", ZFudgeColumn);
		ZFudgeTunnel = ini.Get_Int(IniName, "ZFudgeTunnel", ZFudgeTunnel);
		ZFudgeBridge = ini.Get_Int(IniName, "ZFudgeBridge", ZFudgeBridge);
		VeteranAbilities = ini.Get_Abilities(IniName, "VeteranAbilities", VeteranAbilities);
		EliteAbilities = ini.Get_Abilities(IniName, "EliteAbilities", EliteAbilities);
		MyEffectivenessCoefficient = ini.Get_Float(IniName, "MyEffectivenessCoefficient", MyEffectivenessCoefficient == 0 ? Rule->MyEffectivenessCoefficientDefault : MyEffectivenessCoefficient);
		TargetEffectivenessCoefficient = ini.Get_Float(IniName, "TargetEffectivenessCoefficient", TargetEffectivenessCoefficient == 0 ? Rule->TargetEffectivenessCoefficientDefault : TargetEffectivenessCoefficient);
		TargetSpecialThreatCoefficient = ini.Get_Float(IniName, "TargetSpecialThreatCoefficient", TargetSpecialThreatCoefficient == 0 ? Rule->TargetSpecialThreatCoefficientDefault : TargetSpecialThreatCoefficient);
		TargetStrengthCoefficient = ini.Get_Float(IniName, "TargetStrengthCoefficient", TargetStrengthCoefficient == 0 ? Rule->TargetStrengthCoefficientDefault : TargetStrengthCoefficient);
		TargetDistanceCoefficient = ini.Get_Float(IniName, "TargetDistanceCoefficient", TargetDistanceCoefficient == 0 ? Rule->TargetDistanceCoefficientDefault : TargetDistanceCoefficient);
		SpecialThreatValue = ini.Get_Float(IniName, "SpecialThreatValue", SpecialThreatValue);
		IsSelectableCombatant = ini.Get_Bool(IniName, "IsSelectableCombatant", IsSelectableCombatant);
		UseBuffer = ArtINI.Get_Bool(IniName, "UseBuffer", UseBuffer);
		IsLeader = false;
		if (Weapons[0].Weapon != NULL && Weapons[0].Weapon->Attack > 0) {
			IsLeader = true;
		}
		char filename[512];
		_makepath(filename, NULL, NULL, GraphicName, ".SHP");
		if (ImageAllocated && ImageData) {
			delete (char *)ImageData;
			ImageData = NULL;
			ImageAllocated = false;
		}
		if (Fetch_RTTI() != RTTI_BUILDINGTYPE) {
			ImageData = FileSystem::LoadFile(filename, false);
		}
		ArtINI.Get_String(GraphicName, "Palette", "", PaletteFile, sizeof(PaletteFile));
		FUN_00717820();
		if (FUN_00626DA0() > 1) {
			FUN_004068E0("You have violated the limit of having only one extra building palette.\n");
			Exit_Process(-1);
		}
		TurretOffset = ArtINI.Get_Int(GraphicName, "TurretOffset", TurretOffset);
		Rotation = ArtINI.Get_Int(GraphicName, "RotCount", Rotation);
		IsRemappable = ArtINI.Get_Bool(GraphicName, "Remapable", IsRemappable);
		Normalized = ArtINI.Get_Bool(GraphicName, "Normalized", Normalized);
		IsVisibleLoad = ArtINI.Get_Bool(GraphicName, "VisibleLoad", IsVisibleLoad);
		ShadowIndex = ArtINI.Get_Int(GraphicName, "ShadowIndex", ShadowIndex);
		DisableVoxelCache = ArtINI.Get_Bool(GraphicName, "DisableVoxelCache", DisableVoxelCache);
		DisableShadowCache = ArtINI.Get_Bool(GraphicName, "DisableShadowCache", DisableShadowCache);
		char cameo[32];
		ArtINI.Get_String(GraphicName, "Cameo", "", cameo, sizeof(GraphicName));
		if (strlen(cameo)) {
			strcpy(CameoFilename, cameo);
			_makepath(filename, NULL, NULL, CameoFilename, ".SHP");
			if (!CameoData)
				CameoData = FileSystem::LoadFile(filename, false);
		}
		if (!CameoData)
			CameoData = FileSystem::LoadFile("XXICON.SHP", false);
		ArtINI.Get_String(GraphicName, "AltCameo", "", cameo, sizeof(GraphicName));
		if (strlen(cameo)) {
			strcpy(AltCameoFilename, cameo);
			_makepath(filename, NULL, NULL, AltCameoFilename, ".SHP");
			if (!AltCameoData)
				AltCameoData = FileSystem::LoadFile(filename, false);
		}
		if (HasMultipleTurrets()) {
			for (int index = 0; index < WeaponCount; ++index) {
				char prefix[32], key[128];
				sprintf(prefix, "Weapon%d", index + 1);

				sprintf(key, "%sFLH", prefix);
				Weapons[index].FireFLH = ArtINI.Get_Point(GraphicName, key, Weapons[index].FireFLH);
				sprintf(key, "%sBarrelLength", prefix);
				Weapons[index].BarrelLength = ArtINI.Get_Int(GraphicName, key, Weapons[index].BarrelLength);
				sprintf(key, "%sBarrelThickness", prefix);
				Weapons[index].BarrelThickness = ArtINI.Get_Int(GraphicName, key, Weapons[index].BarrelThickness);
				sprintf(key, "%sTurretLocked", prefix);
				Weapons[index].TurretLocked = ArtINI.Get_Bool(GraphicName, key, Weapons[index].TurretLocked);
				sprintf(prefix, "EliteWeapon%d", index + 1);

				sprintf(key, "%sFLH", prefix);
				EliteWeapons[index].FireFLH = ArtINI.Get_Point(GraphicName, key, Weapons[index].FireFLH);
				sprintf(key, "%sBarrelLength", prefix);
				EliteWeapons[index].BarrelLength = ArtINI.Get_Int(GraphicName, key, Weapons[index].BarrelLength);
				sprintf(key, "%sBarrelThickness", prefix);
				EliteWeapons[index].BarrelThickness = ArtINI.Get_Int(GraphicName, key, Weapons[index].BarrelThickness);
				sprintf(key, "%sTurretLocked", prefix);
				EliteWeapons[index].TurretLocked = ArtINI.Get_Bool(GraphicName, key, Weapons[index].TurretLocked);
			}
		} else {
			Weapons[0].FireFLH = ArtINI.Get_Point(GraphicName, "PrimaryFireFLH", Weapons[0].FireFLH);
			Weapons[0].BarrelLength = ArtINI.Get_Int(GraphicName, "PBarrelLength", Weapons[0].BarrelLength);
			Weapons[0].BarrelThickness = ArtINI.Get_Int(GraphicName, "PBarrelThickness", Weapons[0].BarrelThickness);
			Weapons[1].FireFLH = ArtINI.Get_Point(GraphicName, "SecondaryFireFLH", Weapons[1].FireFLH);
			Weapons[1].BarrelLength = ArtINI.Get_Int(GraphicName, "SBarrelLength", Weapons[1].BarrelLength);
			Weapons[1].BarrelThickness = ArtINI.Get_Int(GraphicName, "SBarrelThickness", Weapons[1].BarrelThickness);
			EliteWeapons[0].FireFLH = ArtINI.Get_Point(GraphicName, "ElitePrimaryFireFLH", Weapons[0].FireFLH);
			EliteWeapons[0].BarrelLength = ArtINI.Get_Int(GraphicName, "ElitePBarrelLength", Weapons[0].BarrelLength);
			EliteWeapons[0].BarrelThickness = ArtINI.Get_Int(GraphicName, "ElitePBarrelThickness", Weapons[0].BarrelThickness);
			EliteWeapons[1].FireFLH = ArtINI.Get_Point(GraphicName, "EliteSecondaryFireFLH", Weapons[1].FireFLH);
			EliteWeapons[1].BarrelLength = ArtINI.Get_Int(GraphicName, "EliteSBarrelLength", Weapons[1].BarrelLength);
			EliteWeapons[1].BarrelThickness = ArtINI.Get_Int(GraphicName, "EliteSBarrelThickness", Weapons[1].BarrelThickness);
		}
		for (int index = 0; index < 5; ++index) {
			char key[0x20];
			sprintf(key, "AlternateFLH%d", index);
			AlternativeFLH[index] = ArtINI.Get_Point(GraphicName, key, Weapons[0].FireFLH);
		}
		TurretNotExportedOnGround = ArtINI.Get_Bool(GraphicName, "TurretNotExportedOnGround", TurretNotExportedOnGround);
		SecondSpawnOffset = ArtINI.Get_Point(GraphicName, "SecondSpawnOffset", SecondSpawnOffset);
		MZone = ini.Get_MZoneType(IniName, "MovementZone", MZone);
		IsSubterranean = MZone == MZONE_SUBTERANNEAN;

		if (IsVoxel) {
			Fetch_Voxel_Image();
		} else if (IsTurretEquipped) {
			Fetch_Aux_Voxel_Image();
		}

		if (Voxel.VoxLib != NULL && FUN_00717AE0(Voxel.VoxLib)) {
			VoxelCenterY = Voxel.VoxLib->Get_Layer_Info(0, 0).YSize * 0.5f + 0.5;
			VoxelCenterX = Voxel.VoxLib->Get_Layer_Info(0, 0).XSize * 0.5f;
		}

		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00716150
bool TechnoTypeClass::Legal_Placement(Cell const & pos, HouseClass * house) const
{
	if (pos == CELL_NONE) return(0);

	Cell const * offset = Occupy_List(true);
	bool build = (Fetch_RTTI() == RTTI_BUILDINGTYPE);
	bool any_blocked = false;
	bool any_clear = false;

	BuildingTypeClass * bptr = (BuildingTypeClass *)this;

	while (offset != NULL && *offset != REFRESH_EOL) {
		Cell cell = pos + *offset++;

		if (build) {
			if (!Map[cell].Is_Clear_To_Build(bptr->Speed, bptr, house)) {
				any_blocked = true;
			} else {
				any_clear = true;
			}
		} else {
			if (!Map[cell].Is_Clear_To_Move(Speed, false, false)) {
				any_blocked = true;
			} else {
				any_clear = true;
			}
		}
	}

	if (build && bptr->ToTile != NULL) {
		return(any_clear);
	}
	return(any_blocked == 0);
}

// FUNCTION: GAMEMD 0x00716290
int TechnoTypeClass::Max_Pips(void) const
{
	switch (PipScale) {
		case PIPSCALE_POWER:
			return(10);

		case PIPSCALE_AMMO:
			return(MaxAmmo);

		case PIPSCALE_CHARGE:
			return(Weapons[0].Weapon->Attack);

		case PIPSCALE_TIBERIUM:
			return(Fetch_RTTI() == RTTI_INFANTRYTYPE ? 3 : 5);

		case PIPSCALE_PASSENGERS:
			return(MaxPassengers);

	}
	return(0);
}

// FUNCTION: GAMEMD 0x007162F0
HRESULT STDMETHODCALLTYPE TechnoTypeClass::Load(IStream * stream)
{
	int i;

	Dock.Clear();
	DebrisTypes.Clear();
	DebrisAnims.Clear();
	VoiceSelect.Clear();
	VoiceSelectEnslaved.Clear();
	VoiceSelectDeactivated.Clear();
	VoiceMove.Clear();
	VoiceAttack.Clear();
	VoiceSpecialAttack.Clear();
	VoiceDie.Clear();
	VoiceFeedback.Clear();
	MoveSound.Clear();
	DieSound.Clear();
	Explosion.Clear();
	DestroyAnim.Clear();
	Prerequisite.Clear();
	PrerequisiteOverride.Clear();
	DebrisMaximums.Clear();

	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		Swizzle_Pointer(&DeploysInto);
		Swizzle_Pointer(&UndeploysInto);
		Swizzle_Pointer(&PowersUnit);
		Swizzle_Pointer(&NaturalParticleSystem);
		Swizzle_Pointer(&RefinerySmokeParticleSystem);
		Swizzle_Pointer(&Spawns);
		Swizzle_Pointer(&Enslaves);
		Swizzle_Pointer(&DeathWeapon);
		Swizzle_Pointer(&AirstrikeTeamType);
		Swizzle_Pointer(&EliteAirstrikeTeamType);
		Swizzle_Pointer(&UnloadingClass);
		Swizzle_Pointer(&DeployingAnim);

		Dock.Load(stream);
		DebrisTypes.Load(stream);
		DebrisAnims.Load(stream);
		VoiceSelect.Load(stream);
		VoiceSelectEnslaved.Load(stream);
		VoiceSelectDeactivated.Load(stream);
		VoiceMove.Load(stream);
		VoiceAttack.Load(stream);
		VoiceSpecialAttack.Load(stream);
		VoiceDie.Load(stream);
		VoiceFeedback.Load(stream);
		MoveSound.Load(stream);
		DieSound.Load(stream);
		Explosion.Load(stream);
		DestroyAnim.Load(stream);
		DebrisMaximums.Load(stream);
		Prerequisite.Load(stream);
		PrerequisiteOverride.Load(stream);
		DamageParticleSystems.Load(stream);
		DestroyParticleSystems.Load(stream);

		for (i = 0; i < Dock.Count(); i++) {
			Swizzle_Pointer(&Dock[i]);
		}
		for (i = 0; i < Explosion.Count(); i++) {
			Swizzle_Pointer(&Explosion[i]);
		}
		for (i = 0; i < DestroyAnim.Count(); i++) {
			Swizzle_Pointer(&DestroyAnim[i]);
		}
		for (i = 0; i < DebrisTypes.Count(); i++) {
			Swizzle_Pointer(&DebrisTypes[i]);
		}
		for (i = 0; i < DebrisAnims.Count(); i++) {
			Swizzle_Pointer(&DebrisAnims[i]);
		}
		for (i = 0; i < MAX_WEAPONS; i++) {
			Swizzle_Pointer(&Weapons[i].Weapon);
		}
		for (i = 0; i < MAX_WEAPONS; i++) {
			Swizzle_Pointer(&EliteWeapons[i].Weapon);
		}
		for (i = 0; i < DamageParticleSystems.Count(); i++) {
			Swizzle_Pointer(&DamageParticleSystems[i]);
		}
		for (i = 0; i < DestroyParticleSystems.Count(); i++) {
			Swizzle_Pointer(&DestroyParticleSystems[i]);
		}

		char buffer[256];
		char fname[_MAX_PATH];

		_makepath(fname, NULL, NULL, (const char *)GraphicName, ".SHP");
		ImageData = FileSystem::LoadFile(fname, false);

		ArtINI.Get_String((const char *)GraphicName, "Cameo", "XXICON", buffer, sizeof(buffer));
		if (stricmp(buffer, "XXICON") == 0) {
			ArtINI.Get_String((const char *)IniName, "Cameo", "XXICON", buffer, sizeof(buffer));
		}
		_makepath(fname, NULL, NULL, buffer, ".SHP");
		CameoData = NULL;
		CameoData = FileSystem::LoadFile(fname, false);
		if (CameoData == NULL) {
			CameoData = FileSystem::LoadFile("XXICON.SHP", false);
		}

		ArtINI.Get_String((const char *)GraphicName, "AltCameo", DAT_00889F64, buffer, sizeof(buffer));
		_makepath(fname, NULL, NULL, buffer, ".SHP");
		AltCameoData = NULL;
		AltCameoData = FileSystem::LoadFile(fname, false);

		ArtINI.Get_String((const char *)GraphicName, "Palette", DAT_00889F64, PaletteFile, sizeof(PaletteFile));
		if (PaletteFile[0] != '\0') {
			Palette = ColorScheme::GeneratePalette(PaletteFile);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00716DC0
HRESULT STDMETHODCALLTYPE TechnoTypeClass::Save(IStream *stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		Dock.Save(stream);
		DebrisTypes.Save(stream);
		DebrisAnims.Save(stream);
		VoiceSelect.Save(stream);
		VoiceSelectEnslaved.Save(stream);
		VoiceSelectDeactivated.Save(stream);
		VoiceMove.Save(stream);
		VoiceAttack.Save(stream);
		VoiceSpecialAttack.Save(stream);
		VoiceDie.Save(stream);
		VoiceFeedback.Save(stream);
		MoveSound.Save(stream);
		DieSound.Save(stream);
		Explosion.Save(stream);
		DestroyAnim.Save(stream);
		DebrisMaximums.Save(stream);
		Prerequisite.Save(stream);
		PrerequisiteOverride.Save(stream);
		DamageParticleSystems.Save(stream);
		DestroyParticleSystems.Save(stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x007170A0
HRESULT STDMETHODCALLTYPE TechnoTypeClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	HRESULT res = BASECLASS::GetSizeMax(pcbSize);
	if (SUCCEEDED(res)) {
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceSelect[0]) * VoiceSelect.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceSelectEnslaved[0]) * VoiceSelectEnslaved.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceSelectDeactivated[0]) * VoiceSelectDeactivated.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceMove[0]) * VoiceMove.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceAttack[0]) * VoiceAttack.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceSpecialAttack[0]) * VoiceSpecialAttack.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceDie[0]) * VoiceDie.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(VoiceFeedback[0]) * VoiceFeedback.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(MoveSound[0]) * MoveSound.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DieSound[0]) * DieSound.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(Explosion[0]) * Explosion.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DestroyAnim[0]) * DestroyAnim.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DebrisTypes[0]) * DebrisTypes.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DebrisAnims[0]) * DebrisAnims.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(Dock[0]) * Dock.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DebrisMaximums[0]) * DebrisMaximums.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DamageParticleSystems[0]) * DamageParticleSystems.Count());
		pcbSize->LowPart += sizeof(int) + (sizeof(DestroyParticleSystems[0]) * DestroyParticleSystems.Count());
		res = S_OK;
	}
	return(res);
}

// FUNCTION: GAMEMD 0x007171A0
void TechnoTypeClass::Compute_CRC(class CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(ThreatAvoidanceCoefficient);
	crc(SlowdownDistance);
	crc(DeaccelerationFactor);
	crc(AccelerationFactor);
	crc(CloakingSpeed);
	crc(DebrisTypes.Count());
	crc(DebrisAnims.Count());
	crc(Dock.Count());
	crc(DebrisMaximums.Count());
	crc((char*)&Locomotor, sizeof(Locomotor));
	crc(VoxelCenterY);
	crc(VoxelCenterX);
	crc(Weight);
	crc(PhysicalSize);
	crc(Size);
	crc(SizeLimit);
	crc(HoverAttack);
	crc(InitialMission);
	crc(IsImmuneToVeins);
	crc(IsTiberiumHeal);
	crc(IsTargetLaser);
	crc(RollAngle);
	crc(PitchAngle);
	crc(PitchSpeed);
	crc(FlightLevel);
	crc(BuildLimit);
	crc(Category);
	crc(DeployTime);
	crc(FireAngle);
	crc(PipScale);
	crc(VoiceSelect.Count());
	crc(VoiceSelectEnslaved.Count());
	crc(VoiceMove.Count());
	crc(VoiceAttack.Count());
	crc(VoiceSpecialAttack.Count());
	crc(VoiceDie.Count());
	crc(VoiceFeedback.Count());
	crc(MoveSound.Count());
	crc(DieSound.Count());
	crc(DamageParticleSystems.Count());
	crc(DestroyParticleSystems.Count());
	crc(MZone);
	crc(ThreatRange);
	crc(MaxDebris);
	crc(MinDebris);
	crc(MaxPassengers);
	crc(SightRange);
	crc(Cost);
	crc(Soylent);
	crc(Level);
	crc(Prerequisite.Count());
	crc(PrerequisiteOverride.Count());
	crc(Risk);
	crc(Reward);
	crc(MaxSpeed);
	crc(Speed);
	crc(MaxAmmo);
	crc(AirRangeBonus);
	crc((int)Ownable);
	crc(AIBasePlanningSide);
	crc(Rotation);
	crc(ROT);
	crc(TurretOffset);
	crc(Points);
	crc(Explosion.Count());
	crc(DestroyAnim.Count());
	crc(ShadowIndex);
	crc(Capacity);
	crc(IsTrain);
	crc(IsDropship);
	crc(IsToProtect);
	crc(IsDisableable);
	crc(IsUnbuildable);
	crc(IsDoubleOwned);
	crc(IsInvisible);
	crc(IsRadarVisible);
	crc(IsLeader);
	crc(IsScanner);
	crc(IsNominal);
	crc(DontScore);
	crc(DamageSelf);
	crc(IsTurretEquipped);
	crc(IsRepairable);
	crc(IsCrew);
	crc(IsRemappable);
	crc(IsCloakable);
	crc(GapGenerator);
	crc(GapRadiusInCells);
	crc(SuperGapRadiusInCells);
	crc(Teleporter);
	crc(IsSelfHealing);
	crc(IsExploding);
	crc(IsNoAutoFire);
	crc(TurretSpins);
	crc(Normalized);
	crc(IsManualReload);
	crc(IsVisibleLoad);
	crc(IsLightningRod);
	crc(DisableVoxelCache);
	crc(DisableShadowCache);
	crc(MobileFire);
	crc(OpportunityFire);
	crc(DistributedFire);
}

// FUNCTION: GAMEMD 0x007177C0
YRWeaponDataStruct * TechnoTypeClass::GetWeapon(int index)
{
	return(&Weapons[index]);
}

// FUNCTION: GAMEMD 0x007177E0
YRWeaponDataStruct * TechnoTypeClass::GetEliteWeapon(int index)
{
	return(&EliteWeapons[index]);
}

// FUNCTION: GAMEMD 0x00717800
int TechnoTypeClass::GetFlightLevel(void) const
{
	if (FlightLevel == -1) {
		return(Rule->FlightLevel);
	}
	return(FlightLevel);
}

// FUNCTION: GAMEMD 0x00717820
void TechnoTypeClass::FUN_00717820(void)
{
	if (PaletteFile[0]) {
		Palette = ColorScheme::GeneratePalette(PaletteFile);
	}
}

// FUNCTION: GAMEMD 0x00717840
void TechnoTypeClass::FUN_00717840(void)
{
	for (int index = 0; index < Array.Count(); index++) {
		Array[index]->FUN_00717820();
	}
}

// FUNCTION: GAMEMD 0x00717880
bool TechnoTypeClass::HasMultipleTurrets(void) const
{
	return(TurretCount > 0);
}

// FUNCTION: GAMEMD 0x00717890
void TechnoTypeClass::FUN_00717890(int index, int weapon)
{
	TurretWeapon[weapon] = index;
}

// FUNCTION: GAMEMD 0x007178B0
int TechnoTypeClass::FUN_007178B0(int weapon) const
{
	return(TurretWeapon[weapon]);
}

// SYNTHETIC: GAMEMD 0x007179A0
// ??_GTechnoTypeClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x007179C0
// ??1AbilityFlagsType@@QAE@XZ

// FUNCTION: GAMEMD 0x007179D0
AbilityFlagsType AbilityFlagsType::operator=(AbilityFlagsType const & that)
{
	if (this != &that) {
		memcpy(this, &that, sizeof(*this));
	}
	return(*this);
}

// FUNCTION: GAMEMD 0x00717A30
void TurretControl::FUN_00717A30(int travel)
{
	Travel = travel;
}

// FUNCTION: GAMEMD 0x00717A40
int TurretControl::FUN_00717A40(void) const
{
	return(Travel);
}

// FUNCTION: GAMEMD 0x00717A50
void TurretControl::FUN_00717A50(int frames)
{
	CompressFrames = (frames > 0) ? frames : 1;
}

// FUNCTION: GAMEMD 0x00717A70
int TurretControl::FUN_00717A70(void) const
{
	return(CompressFrames);
}

// FUNCTION: GAMEMD 0x00717A80
void TurretControl::FUN_00717A80(int frames)
{
	RecoverFrames = (frames > 0) ? frames : 1;
}

// FUNCTION: GAMEMD 0x00717AA0
int TurretControl::FUN_00717AA0(void) const
{
	return(RecoverFrames);
}

// FUNCTION: GAMEMD 0x00717AB0
void TurretControl::FUN_00717AB0(int frames)
{
	HoldFrames = (frames > 0) ? frames : 1;
}

// FUNCTION: GAMEMD 0x00717AD0
int TurretControl::FUN_00717AD0(void) const
{
	return(HoldFrames);
}

// FUNCTION: GAMEMD 0x00717AE0
bool FUN_00717AE0(VoxelLibrary * library)
{
	return(!library->Load_Failed());
}

// TEMPLATE: GAMEMD 0x00717AF0
// ??0?$TypeList@PBVParticleSystemTypeClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x00717B20
// ?Save@?$TypeList@PBVParticleSystemTypeClass@@@@QBEXPAUIStream@@@Z
// SYNTHETIC: GAMEMD 0x00717B70
// ??_G?$TypeList@PBVParticleSystemTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x00717BC0
// ??0?$DynamicVectorClass@PBVVoxelAnimTypeClass@@@@QAE@ABVNoInitClass@@@Z
// TEMPLATE: GAMEMD 0x00717BD0
// ??0?$DynamicVectorClass@H@@QAE@ABVNoInitClass@@@Z
// TEMPLATE: GAMEMD 0x00717BE0
// ??0?$DynamicVectorClass@PBVBuildingTypeClass@@@@QAE@ABVNoInitClass@@@Z
// TEMPLATE: GAMEMD 0x00717BF0
// ??0?$DynamicVectorClass@PBVAnimTypeClass@@@@QAE@ABVNoInitClass@@@Z
// TEMPLATE: GAMEMD 0x00717C00
// ??4?$VectorClass@PBVParticleSystemTypeClass@@@@QAEAAV0@ABV0@@Z
// TEMPLATE: GAMEMD 0x00717C70
// ??0?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@QAE@IPBQBVParticleSystemTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00717CD0
// ?Add@?$DynamicVectorClass@PBVParticleSystemTypeClass@@@@QAE_NABQBVParticleSystemTypeClass@@@Z
template class TypeList<ParticleSystemTypeClass const *>;
template class DynamicVectorClass<VoxelAnimTypeClass const *>;
template class DynamicVectorClass<int>;
template class DynamicVectorClass<BuildingTypeClass const *>;
template class DynamicVectorClass<AnimTypeClass const *>;
template class VectorClass<ParticleSystemTypeClass const *>;
template class DynamicVectorClass<ParticleSystemTypeClass const *>;
// TEMPLATE: GAMEMD 0x00717D20
// ??0?$TypeList@PBVParticleSystemTypeClass@@@@QAE@ABV0@@Z
// TEMPLATE: GAMEMD 0x00717DB0
// ?Delete_Index@?$DynamicVectorClass@PAVTechnoTypeClass@@@@QAE_NH@Z
template class DynamicVectorClass<TechnoTypeClass *>;
