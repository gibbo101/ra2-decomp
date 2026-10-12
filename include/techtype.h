// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_weapon.h"
#include "objtype.h"
#include "typelist.h"

#include "category.hh"
#include "mission.hh"
#include "mph.hh"
#include "mzone.hh"
#include "pip.hh"
#include "speed.hh"

class VoxelAnimTypeClass;
class UnitTypeClass;
class ParticleSystemTypeClass;
class AnimTypeClass;
class WeaponTypeClass;
class AircraftTypeClass;
class InfantryTypeClass;
class ColorSchemeClass;
template<class T> class DynamicVectorClass;

struct YRWeaponDataStruct : public WeaponDataStruct
{
	bool TurretLocked;
};

struct TurretControl
{
	TurretControl(void) {}
	TurretControl(int travel, int compress, int recover, int hold) : Travel(travel), CompressFrames(compress), RecoverFrames(recover), HoldFrames(hold) {}
	int Travel;
	int CompressFrames;
	int RecoverFrames;
	int HoldFrames;
};

class TechnoTypeClass : public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:

		enum {
			MAX_WEAPONS = 18
		};

		int WalkRate;
		int IdleRate;

		AbilityFlagsType VeteranAbilities;
		AbilityFlagsType EliteAbilities;

		double SpecialThreatValue;

		double MyEffectivenessCoefficient;
		double TargetEffectivenessCoefficient;
		double TargetSpecialThreatCoefficient;
		double TargetStrengthCoefficient;
		double TargetDistanceCoefficient;

		double ThreatAvoidanceCoefficient;

		int SlowdownDistance;

		double DeaccelerationFactor;
		double AccelerationFactor;

		int CloakingSpeed;

		TypeList<VoxelAnimTypeClass const *> DebrisTypes;
		TypeList<int> DebrisMaximums;

		CLSID Locomotor;

		double VoxelCenterY;
		double VoxelCenterX;

		double Weight;

		double PhysicalSize;
		double Size;
		double SizeLimit;

		bool HoverAttack;
		int VHPScan;

		MissionType InitialMission;

		double RollAngle;
		double PitchSpeed;
		double PitchAngle;

		int BuildLimit;

		CategoryType Category;

		int Unused2;

		double DeployTime;

		int FireAngle;

		PipScaleType PipScale;
		bool PipsDrawForAll;
		int LeptonMindControlOffset;
		int PixelSelectionBracketDelta;
		int PipWrap;

		TypeList<BuildingTypeClass const *> Dock;

		BuildingTypeClass *DeploysInto;
		UnitTypeClass *UndeploysInto;
		UnitTypeClass *PowersUnit;
		bool PoweredUnit;

		TypeList<int> VoiceSelect;
		TypeList<int> VoiceSelectEnslaved;
		TypeList<int> VoiceSelectDeactivated;
		TypeList<int> VoiceMove;
		TypeList<int> VoiceAttack;
		TypeList<int> VoiceSpecialAttack;
		TypeList<int> VoiceDie;
		TypeList<int> VoiceFeedback;
		TypeList<int> MoveSound;
		TypeList<int> DieSound;

		VocType AuxSound1;
		VocType AuxSound2;
		int CreateSound;
		int DamageSound;
		int ImpactWaterSound;
		int ImpactLandSound;
		int CrashingSound;
		int SinkingSound;
		int VoiceFalling;
		int VoiceCrashing;
		int VoiceSinking;
		int VoiceEnter;
		int VoiceCapture;
		int TurretRotateSound;
		int EnterTransportSound;
		int LeaveTransportSound;
		int DeploySound;
		int UndeploySound;
		int ChronoInSound;
		int ChronoOutSound;
		int VoiceHarvest;
		int VoicePrimaryWeaponAttack;
		int VoicePrimaryEliteWeaponAttack;
		int VoiceSecondaryWeaponAttack;
		int VoiceSecondaryEliteWeaponAttack;
		int VoiceDeploy;
		int VoiceUndeploy;
		int EnterGrinderSound;
		int LeaveGrinderSound;
		int EnterBioReactorSound;
		int LeaveBioReactorSound;
		int ActivateSound;
		int DeactivateSound;
		int MindClearedSound;

		MZoneType MZone;

		LEPTON ThreatRange;

		int MaxDebris;
		int MinDebris;
		TypeList<AnimTypeClass const *> DebrisAnims;

		int MaxPassengers;
		bool OpenTopped;

		int SightRange;

		bool ResourceGatherer;
		bool ResourceDestination;
		bool RevealToAll;
		bool Drainable;
		int SensorsSight;
		int DetectDisguiseRange;
		int BombSight;
		int LeadershipRating;
		int NavalTargeting;
		int LandTargeting;
		float BuildTimeMultiplier;
		int MindControlRingOffset;

		int Cost;
		int Soylent;

		int FlightLevel;
		int AirstrikeTeam;
		int EliteAirstrikeTeam;
		AircraftTypeClass *AirstrikeTeamType;
		AircraftTypeClass *EliteAirstrikeTeamType;
		int AirstrikeRechargeTime;
		int EliteAirstrikeRechargeTime;

		int Level;

		TypeList<int> Prerequisite;
		TypeList<int> PrerequisiteOverride;

		int Risk;
		int Reward;

		MPHType MaxSpeed;

		SpeedType Speed;

		int InitialAmmo;
		int MaxAmmo;
		int IFVMode;
		int AirRangeBonus;
		bool BerserkFriendly;
		bool SprayAttack;
		bool Pushy;
		bool Natural;
		bool Unnatural;
		bool CloseRange;
		int Reload;
		int EmptyReload;
		int ReloadIncrement;
		int RadialFireSegments;
		int DeployFireWeapon;
		bool DeployFire;
		bool DeployToLand;
		bool MobileFire;
		bool OpportunityFire;
		bool DistributedFire;
		bool DamageReducesReadiness;
		int ReadinessReductionMultiplier;
		UnitTypeClass *UnloadingClass;
		AnimTypeClass *DeployingAnim;
		bool AttackFriendlies;
		bool AttackCursorOnFriendlies;
		int UndeployDelay;
		bool PreventAttackMove;

		int Ownable;
		int AIBasePlanningSide;
		bool StupidHunt;

		bool IsAllowedToStartInMultiplayer;

		TStringID<24> CameoFilename;
		const void * CameoData;
		bool CameoAllocated;
		TStringID<24> AltCameoFilename;
		const void * AltCameoData;
		bool AltCameoAllocated;

		int Rotation;

		int ROT;

		LEPTON TurretOffset;
		bool CanBeHidden;

		int Points;

		TypeList<AnimTypeClass const *> Explosion;
		TypeList<AnimTypeClass const *> DestroyAnim;

		ParticleSystemTypeClass const * NaturalParticleSystem;
		TPoint3D<int> NaturalParticleLocation;
		ParticleSystemTypeClass const * RefinerySmokeParticleSystem;

		TypeList<ParticleSystemTypeClass const *> DamageParticleSystems;
		TypeList<ParticleSystemTypeClass const *> DestroyParticleSystems;
		TPoint3D<int> DamageSmokeOffset;
		bool DamSmkOffScrnRel;
		TPoint3D<int> DestroySmokeOffset;
		TPoint3D<int> RefinerySmokeOffsetOne;
		TPoint3D<int> RefinerySmokeOffsetTwo;
		TPoint3D<int> RefinerySmokeOffsetThree;
		TPoint3D<int> RefinerySmokeOffsetFour;

		int ShadowIndex;

		int Capacity;

		bool TurretNotExportedOnGround;
		bool Gunner;
		bool HasTurretTooltips;
		int TurretCount;
		int WeaponCount;
		bool IsChargeTurret;
		int TurretWeapon[MAX_WEAPONS];
		TPoint3D<int> AlternativeFLH[5];

		YRWeaponDataStruct Weapons[MAX_WEAPONS];
		bool ClearAllWeapons;
		YRWeaponDataStruct EliteWeapons[MAX_WEAPONS];

		bool IsTypeImmune;

		bool IsMoveToShroud;

		bool IsTrainable;

		bool IsDamageSparks;

		bool IsTargetLaser;

		bool IsImmuneToVeins;

		bool IsTiberiumHeal;

		bool IsCloakStop;

		bool IsTrain;

		bool IsDropship;

		bool IsToProtect;

		bool IsDisableable;

		bool IsUnbuildable;

		bool IsDoubleOwned;

		bool IsInvisible;

		bool IsRadarVisible;

		bool IsLeader;

		bool IsScanner;

		bool IsNominal;
		bool DontScore;
		bool DamageSelf;

		bool IsTurretEquipped;
		bool TurretRecoil;
		TurretControl TurretAnimData;
		bool UnknownBoolCB4;
		TurretControl BarrelAnimData;
		bool UnknownBoolCC8;
		unsigned char : 8;
		unsigned char : 8;
		unsigned char : 8;

		bool IsRepairable;

		bool IsCrew;
		bool Naval;

		bool IsRemappable;

		bool IsCloakable;
		bool GapGenerator;
		char GapRadiusInCells;
		char SuperGapRadiusInCells;
		bool Teleporter;
		bool IsGattling;
		int WeaponStages;
		int WeaponStage[6];
		int EliteStage[6];
		int RateUp;
		int RateDown;

		bool IsSelfHealing;

		bool IsExploding;
		WeaponTypeClass *DeathWeapon;
		float DeathWeaponDamageModifier;

		bool IsNoAutoFire;
		bool TurretSpins;
		bool TiltCrashJumpjet;
		bool Normalized;

		bool IsManualReload;

		bool IsVisibleLoad;

		bool IsLightningRod;

		bool IsHunterSeeker;

		bool IsCrusher;
		bool OmniCrusher;
		bool OmniCrushResistant;

		bool IsTiltsWhenCrushes;

		bool IsSubterranean;

		bool IsAutoCrush;
		bool Bunkerable;
		bool CanDisguise;
		bool PermaDisguise;
		bool DetectDisguise;
		bool DisguiseWhenStill;
		bool CanApproachTarget;
		bool CanRecalcApproachTarget;
		bool ImmuneToPsionics;
		bool ImmuneToPsionicWeapons;
		bool ImmuneToRadiation;
		bool Parasiteable;
		bool DefaultToGuardArea;
		bool Warpable;
		bool ImmuneToPoison;
		bool ReselectIfLimboed;
		bool RejoinTeamIfLimboed;
		bool Slaved;
		InfantryTypeClass *Enslaves;
		int SlavesNumber;
		int SlaveRegenRate;
		int SlaveReloadRate;
		int OpenTransportWeapon;
		bool Spawned;
		AircraftTypeClass *Spawns;
		int SpawnsNumber;
		int SpawnRegenRate;
		int SpawnReloadRate;
		bool MissileSpawn;
		bool Underwater;
		bool BalloonHover;
		int SuppressionThreshold;
		int JumpjetTurnRate;
		int JumpjetSpeed;
		float JumpjetClimb;
		float JumpjetCrash;
		int JumpjetHeight;
		float JumpjetAccel;
		float JumpjetWobbles;
		bool JumpjetNoWobbles;
		int JumpjetDeviation;
		bool JumpJet;
		bool Crashable;
		bool ConsideredAircraft;
		bool Organic;
		bool NoShadow;
		bool CanPassiveAquire;
		bool CanRetaliate;
		bool RequiresStolenThirdTech;
		bool RequiresStolenSovietTech;
		bool RequiresStolenAlliedTech;
		DWORD RequiredHouses;
		DWORD ForbiddenHouses;
		DWORD SecretHouses;
		bool UseBuffer;
		TPoint3D<int> SecondSpawnOffset;
		bool IsSelectableCombatant;

		bool IsAccelerates;
		bool DisableVoxelCache;
		bool DisableShadowCache;

		int ZFudgeCliff;
		int ZFudgeColumn;
		int ZFudgeTunnel;
		int ZFudgeBridge;

		char PaletteFile[32];
		DynamicVectorClass<ColorSchemeClass *> * Palette;
		unsigned int : 32;

		TechnoTypeClass(NoInitClass const & x);
		TechnoTypeClass(char const * ininame, SpeedType speed);
		virtual ~TechnoTypeClass() override;

		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;
		virtual HRESULT STDMETHODCALLTYPE GetSizeMax(ULARGE_INTEGER *pcbSize) override;

		virtual bool CanUseWaypoint(void) const;
		virtual bool CanAttackMove(void) const;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		bool Is_Two_Shooter(void) const;
		virtual bool Legal_Placement(Cell const & pos, HouseClass * house) const;
		virtual int Raw_Cost(void) const;
		int Max_Passengers(void) const {return(MaxPassengers);}
		virtual int Repair_Cost(void) const;
		virtual int Repair_Step(void) const;
		virtual int GetRefund(HouseClass * house, bool unknown) const;
		virtual int GetFlightLevel(void) const;
		virtual void const * Get_Cameo_Data(void) const override;
		virtual int Cost_Of(HouseClass * house = NULL) const override;
		virtual int Time_To_Build(void) const override;
		virtual int Get_Ownable(void) const override;
		virtual int Max_Pips(void) const override;
		bool In_Range(Coord const & coord, AbstractClass * target, WeaponTypeClass * weapon) const;
		bool HasMultipleTurrets(void) const;
		void FUN_00717890(int index, int weapon);
		void FUN_00717820(void);
		static void FUN_00717840(void);
		virtual bool Read_INI(CCINIClass & ini) override;

		WeaponDataStruct const * Get_Weapon(int which) const;
		YRWeaponDataStruct * GetWeapon(int index);
		YRWeaponDataStruct * GetEliteWeapon(int index);
		void Set_Weapon(WeaponDataStruct const & weapon, int which);

		static void const * WakeShapes;
		static DynamicVectorClass<TechnoTypeClass *> Array;
};
