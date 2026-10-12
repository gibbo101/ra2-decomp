// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_voxel.h"
#include "audioctl.h"
#include "cargo.h"
#include "comtypes.h"
#include "door.h"
#include "facing.h"
#include "flasher.h"
#include "ftimer.h"
#include "globals.h"
#include "house.h"
#include "matrix3d.h"
#include "point.h"
#include "radio.h"
#include "stage.h"
#include "stbuffer.h"
#include "storage.h"
#include "techtype.h"
#include "timer.h"
#include "vector.h"
#include "veteran.h"

#include "cloak.hh"
#include "detected.hh"
#include "draw.hh"
#include "fire.hh"
#include "infantry.hh"
#include "particle.hh"
#include "talk.hh"
#include "threat.hh"
#include "zgrad.hh"

class ParticleSystemClass;
class WaveClass;
class WeaponTypeClass;
class ShapeSet;
struct WeaponDataStruct;
class AnimClass;
class TemporalClass;
class AirstrikeClass;
class FootClass;
class CaptureManagerClass;
class SpawnManagerClass;
class SlaveManagerClass;
class TeamClass;
class RadBeam;
class PlanningTokenClass
{
	public:
		void FUN_00636310(void);
};
class ObjectTypeClass;
class Surface;

struct RecoilData
{
	TurretControl Turret;
	float TravelPerFrame;
	float TravelSoFar;
	int State;
	int TravelFramesLeft;

	RecoilData(void)
	{
		Turret.Travel = 2;
		Turret.CompressFrames = 1;
		Turret.RecoverFrames = 1;
		Turret.HoldFrames = 1;
		TravelSoFar = 0;
		State = 0;
	}
};

struct YRVectorData
{
	void * VTable;
	void ** Items;
	int Capacity;
	bool IsInitialized;
	bool IsAllocated;
	int Count;
	int CapacityIncrement;
};

class TechnoClass : public RadioClass
{
		typedef RadioClass BASECLASS;

	public:
		enum {
			RAILGUN_STEPS = 50,
		};

		FlasherClass Flashing;
		StageClass Animation;

		CargoClass Cargo;
		TechnoClass * Transporter;

		int LastFireBulletFrame;
		int CurrentTurretNumber;
		int unknown_int_128;
		AnimClass * BehindAnim;
		AnimClass * DeployAnim;
		bool InAir;
		int CurrentWeaponNumber;
		int CurrentRanking;
		int CurrentGattlingStage;
		int GattlingValue;
		int TurretAnimFrame;
		HouseClass * InitialOwner;

		VeterancyClass Veterancy;

		double ArmorBias;
		double FirepowerBias;

		CDTimerClass<FrameTimerClass> IdleTimer;
		CDTimerClass<FrameTimerClass> RadarFlashTimer;
		CDTimerClass<FrameTimerClass> TargetingTimer;
		CDTimerClass<FrameTimerClass> IronCurtainTimer;
		CDTimerClass<FrameTimerClass> IronTintTimer;
		int IronTintStage;
		CDTimerClass<FrameTimerClass> AirstrikeTimer;
		CDTimerClass<FrameTimerClass> AirstrikeTintTimer;
		unsigned AirstrikeTintStage;
		int ForceShielded;
		bool Deactivated;
		TechnoClass * DrainTarget;
		TechnoClass * DrainingMe;
		AnimClass * DrainAnim;
		bool Disguised;
		unsigned DisguiseCreationFrame;
		CDTimerClass<FrameTimerClass> InfantryBlinkTimer;
		CDTimerClass<FrameTimerClass> DisguiseBlinkTimer;
		bool UnlimboingInfantry;
		CDTimerClass<FrameTimerClass> ReloadTimer;

		Point2D RadarPos;

		unsigned SpiedBy;

		int Group;

		AbstractClass * ArchivedTarget;

		HouseClass * House;

		CloakType Cloak;
		StageClass CloakingDevice;
		CDTimerClass<FrameTimerClass> CloakDelay;

		float WarpFactor;
		bool unknown_bool_250;
		int LastSightCoords[3];
		int LastSightRange;
		int LastSightHeight;
		bool GapSuperCharged;
		bool GeneratingGap;
		int GapRadius;
		bool BeingWarpedOut;
		bool WarpingOut;
		bool unknown_bool_272;
		unsigned char unused_273;
		TemporalClass * TemporalImUsing;
		TemporalClass * TemporalTargetingMe;
		bool IsImmobilized;
		unsigned unknown_280;
		int ChronoLockRemaining;
		int ChronoDestCoords[3];
		AirstrikeClass * Airstrike;
		bool Berzerk;
		int BerzerkDurationLeft;
		unsigned SprayOffsetIndex;
		bool Uncrushable;
		FootClass * DirectRockerLinkedUnit;
		FootClass * LocomotorTarget;
		FootClass * LocomotorSource;

		AbstractClass * TarCom;
		AbstractClass * SuspendedTarCom;

		CaptureManagerClass * CaptureManager;
		TechnoClass * MindControlledBy;
		bool MindControlledByAUnit;
		AnimClass * MindControlRingAnim;
		HouseClass * MindControlledByHouse;
		SpawnManagerClass * SpawnManager;
		TechnoClass * SpawnOwner;
		SlaveManagerClass * SlaveManager;
		TechnoClass * SlaveOwner;
		HouseClass * OriginallyOwnedByHouse;
		TechnoClass * BunkerLinkedItem;

		float PitchAngle;

		CDTimerClass<FrameTimerClass> Arm;

		int ChargeTurretDelay;

		int Ammo;

		int PurchasePrice;

		ParticleSystemClass * ParticleSystems[ATTACHED_PARTICLE_COUNT];
		ParticleSystemClass * unk1ParticleSystem;
		ParticleSystemClass * unk2ParticleSystem;
		ParticleSystemClass * FiringParticleSystem;

		WaveClass * Wave;

		float AngleRotatedSideways;
		float AngleRotatedForwards;

		float RockingSidewaysPerFrame;
		float RockingForwardsPerFrame;

		InfantryType EnteredByInfType;

		StorageClass Storage;
		unsigned unknown_34C;

		DoorClass Door;

		FacingClass BarrelPitch;

		FacingClass PrimaryFacing;

		FacingClass SecondaryFacing;

		int BurstIndex;

		CDTimerClass<FrameTimerClass> TargetingLaserTimer;

		unsigned short SoundRandomSeed;
		unsigned short unknown_3CA;

		bool CountedAsOwned;

		bool IsSinking;
		bool WasSinkingAlready;
		bool unknown_bool_3CF;

		bool IsUseless;

		bool IsTickedOff;

		bool IsCloakable;

		bool IsLeader;

		bool IsALoaner;

		bool IsInPlayfield;

		RecoilData TurretRecoil;
		RecoilData BarrelRecoil;

		bool IsTethered;
		bool IsAlternativeTether;

		bool IsOwnedByPlayer;

		bool IsDiscoveredByPlayer;

		bool IsDiscoveredByComputer;
		bool unknown_bool_41D;
		bool unknown_bool_41E;
		bool unknown_bool_41F;

		char SightIncrease;

		bool IsTeamRecruitable;
		bool IsAutocreateRecruitable;

		bool IsRadarTracked;

		bool IsInTransport;
		bool IsCrashing;
		bool WasCrashingAlready;
		bool IsBeingManipulated;
		TechnoClass * BeingManipulatedBy;
		HouseClass * ChronoWarpedByHouse;
		bool unknown_bool_430;
		bool IsMouseHovering;
		bool ShouldBeReselectOnUnlimbo;
		TeamClass * OldTeam;
		bool CountedAsOwnedSpecial;
		bool Absorbed;
		bool unknown_bool_43A;
		unsigned unknown_43C;

		DynamicVectorClass<int> CurrentTargetThreatValues;
		DynamicVectorClass<AbstractClass *> CurrentTargets;
		DynamicVectorClass<AbstractClass *> AttackedTargets;

		AudioController TurretRotateSoundController;
		int IsTurretRotateSoundPlaying;
		int TurretIsRotating;
		AudioController GattlingSoundController;
		bool IsGattlingSoundPlaying;
		unsigned unknown_4BC;
		AudioController UnusedGattlingSoundController;
		bool IsUnusedGattlingSoundPlaying;
		unsigned unknown_4D8;
		AudioController QueuedVoiceSoundController;
		unsigned QueuedVoiceIndex;
		unsigned unknown_4F4;
		bool unknown_bool_4F8;
		unsigned unknown_4FC;
		TechnoClass * QueueUpToEnter;
		unsigned EMPLockRemaining;
		unsigned ThreatPosed;
		unsigned ShouldLoseTargetNow;
		RadBeam * FiringRadBeam;
		PlanningTokenClass * PlanningToken;
		ObjectTypeClass * Disguise;
		HouseClass * DisguisedAsHouse;

		TechnoClass(HouseClass * house=0);
		TechnoClass(NoInitClass const & x);
		virtual ~TechnoClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;

		virtual void Compute_CRC(CRCEngine &) const override;
		virtual TechnoTypeClass const * Techno_Type_Class(void) const override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;
		virtual void AnimPointerExpired(AnimClass * anim) override;
		virtual bool IsDisguised(void) const override;
		virtual bool IsDisguisedAs(HouseClass * house) const override;
		virtual void RegisterDestruction(TechnoClass * destroyer) override;
		virtual bool CanBeSelectedNow(void) const override;
		virtual int IronCurtain(int duration, HouseClass * source, bool force_shield) override;
		virtual void StopAirstrikeTimer(void) override;
		virtual void StartAirstrikeTimer(int duration) override;
		virtual bool IsIronCurtained(void) const override;
		virtual void Destroy(void) override;
		virtual bool IsBeingWarpedOut(void) const override;
		virtual bool IsWarpingIn(void) const override;
		virtual bool IsWarpingSomethingOut(void) const override;
		virtual bool IsNotWarping(void) const override;
		virtual void * GetRemapColour(void) const override;

		virtual bool Is_Move_Override(void) const;
		virtual bool Is_Allowed_To_Recloak(void) const;
		virtual bool Can_Scatter(void) const;
		virtual bool Is_In_Team(void) const;
		virtual bool Should_Self_Heal_Now(void) const;
		virtual bool Is_Voxel_Loaded(void) const;
		virtual bool Is_Ready_To_Move(void) const;
		virtual bool Is_Ready_To_Cloak(void) const;
		virtual bool Should_Uncloak(void) const;
		virtual DirType Turret_Facing(void) const;
		virtual bool Is_Weapon_Equipped(void) const;
		virtual bool Is_On_Elevation(void) const;
		virtual double Tiberium_Load(void) const;
		virtual int Pip_Count(void) const;
		virtual int Refund_Amount(void) const;
		virtual int Risk(void) const;
		virtual bool Is_In_Same_Zone_As(ObjectClass const * object) const;
		virtual unsigned vt_entry_2C8(unsigned, unsigned);
		virtual bool Is_In_Same_Zone(Coord const & coord) const;
		virtual int How_Many_Survivors(void) const;
		virtual int Anti_Air(void) const;
		virtual int Anti_Armor(void) const;
		virtual int Anti_Infantry(void) const;
		virtual void GotHijacked(void);
		virtual int What_Weapon_Should_I_Use(AbstractClass * target) const;
		virtual int SelectNavalTargeting(AbstractClass * target) const;
		virtual int Get_Z_Adjust(void) const;
		virtual ZGradientType Get_Z_Gradient(void) const {return(ZGRAD_90DEG);}
		virtual Cell GetLastFlightMapCoords(void) const;
		virtual void SetLastFlightMapCoords(Cell coord);
		virtual Cell Find_Exit_Cell(TechnoClass const * techno) const;
		virtual Coord Turret_Coord(int which=0) const;
		void Create_Planning_Token(void);
		void FUN_0070DC70(int which);
		int FUN_0070DDF0(void);
		void GattlingRateUp(int rate);
		void GattlingRateDown(int rate);
		bool FUN_0070E240(WeaponDataStruct const * weapon);
		virtual FacingType Desired_Load_Dir(ObjectClass * , Cell & moveto) const;
		virtual DirType Fire_Direction(void) const;
		virtual InfantryTypeClass const * Crew_Type(void) const;
		virtual bool Can_Attack_Now(void) const;
		virtual bool Can_Deploy_Now(void) const;
		virtual int Rearm_Delay(int which=0) const;
		virtual int Threat_Range(int control) const;
		virtual bool Is_Allowed_To_Leave_Map(void) const;
		virtual bool Is_Radar_Visible(DetectedType & detected) const;
		virtual bool Is_Sensed_By_Player(void) const;
		virtual bool Is_Sensed_By_House(HouseClass const * house) const;
		virtual bool Is_Renovator(void) const;
		virtual void Advance_Waypoint_Path(void);
		virtual Cell ScanForTiberium(Cell * buffer, int range, unsigned unk) const;
		virtual bool EnterGrinder(void);
		virtual bool EnterBioReactor(void);
		virtual bool EnterTankBunker(void);
		virtual bool EnterBattleBunker(void);
		virtual bool GarrisonStructure(void);
		virtual bool IsPowerOnline(void) const;
		virtual void QueueVoice(int voice);
		virtual int VoiceEnter(void);
		virtual int VoiceHarvest(void);
		virtual void Response_Select(void);
		virtual int VoiceCapture(void);
		virtual void Response_Move(void);
		virtual int VoiceDeploy(void);
		virtual void Response_Attack(void);
		virtual bool ClickedEvent(int event);
		virtual void Player_Assign_Mission(MissionType order, AbstractClass * target=NULL, AbstractClass * destination=NULL, AbstractClass * nearby=NULL);
		virtual bool IsUnderEMP(void) const;
		virtual bool IsParalyzed(void) const;
		virtual bool CanCheer(void) const;
		virtual void Cheer(bool force);
		virtual int Get_Max_Speed(void) const;
		virtual void Reduce_Ammunition(void);
		virtual void AddPassenger(FootClass * passenger);
		virtual bool CanDisguiseAs(AbstractClass * target) const;
		virtual bool Target_Something_Nearby(Coord const & coord, ThreatType threat=THREAT_NORMAL);
		virtual void Stun(void);
		virtual bool TriggersCellInset(AbstractClass * target);
		virtual bool In_Range(AbstractClass * target, int which=0) const;
		virtual bool IsCloseEnoughToAttack(AbstractClass * target) const;
		virtual bool IsCloseEnoughToAttackCoords(Coord const & coord) const;
		virtual bool InAuxiliarySearchRange(AbstractClass * target) const;
		virtual void Death_Announcement(TechnoClass const * source=0) const = 0;
		virtual int GetFireErrorWithoutRange(AbstractClass * target, int which) const;
		virtual FireErrorType Can_Fire(AbstractClass * target, int which=0, bool ignoreRange=false) const;
		virtual AbstractClass * Greatest_Threat(ThreatType threat, Coord const & coord, bool) const;
		virtual void Assign_Target(AbstractClass * target);
		virtual BulletClass * Fire_At(AbstractClass * target, int which=0);
		virtual void Guard(void);
		virtual bool Captured(HouseClass * newowner);
		virtual void vt_entry_3D8(unsigned, unsigned, unsigned);
		virtual bool Crash(ObjectClass * killer);
		virtual bool IsAreaFire(void) const;
		virtual int IsNotSprayAttack(void) const;
		virtual int GetSecondaryWeaponIndex(void) const;
		virtual int IsNotSprayAttack2(void) const;
		virtual WeaponDataStruct const * GetDeployWeapon(void) const;
		virtual WeaponDataStruct const * GetTurretWeapon(void) const;
		virtual WeaponDataStruct const * Get_Class_Weapon_Data(int which=0) const;
		virtual bool Is_Turret_Equipped(void) const;
		virtual bool CanOccupyFire(void) const;
		virtual int GetOccupyRangeBonus(void) const;
		virtual int GetOccupantCount(void) const;
		virtual void Renovate(void);
		virtual void Cloaking_AI(bool=false);
		virtual void CreateGap(void);
		virtual void DestroyGap(void);
		virtual void Try_To_Cloak(void);
		virtual void Sensed(void);
		virtual void Reload(void);
		virtual void Rocking_AI(void);
		virtual Coord GetAttackCoordinates(void) const;
		virtual bool IsNotWarpingIn(void) const;
		virtual bool vt_entry_434(unsigned) const;
		virtual void Draw_Action_Line(void) const;
		virtual unsigned GetDisguiseFlags(unsigned flags) const;
		virtual bool IsClearlyVisibleTo(HouseClass * house) const;
		virtual void Draw_Voxel(VoxelDataStruct const & voxeldata, int frame, int key, VoxelIndexClass * cache, Rect const & cliprect, Point2D const & point, Matrix3D const & matrix, int brightness, ShapeFlags_Type negflags) const;
		virtual void vt_entry_448(unsigned, unsigned);
		virtual void Draw_Health_Bar(Point2D const & point, Rect const & rect, bool sensed_underground) const;
		virtual void Draw_Pips(Point2D const & bottomleft, Point2D const & bottomright, Rect const & rect) const;
		virtual void DrawVeterancyPips(Point2D const & point, Rect const & rect) const;
		virtual void Draw_Text_Overlay(Point2D const & point1, Point2D const & point2, Rect const & rect) const;
		virtual void Do_Uncloak(bool silent=false);
		virtual void Do_Cloak(bool silent=false);
		virtual int Apparent_Brightness(int brightness = 1000) const;
		virtual void UpdateRefinerySmokeSystems(void);
		virtual unsigned DisguiseAs(AbstractClass * target);
		virtual void ClearDisguise(void);
		virtual bool Is_Ready_To_Random_Animate(void) const;
		virtual bool Random_Animate(void) {return(false);}
		virtual void vt_entry_47C(unsigned);
		virtual void Assign_Destination(AbstractClass * target, bool = true);
		virtual bool Enter_Idle_Mode(bool initial=false, bool = true);
		bool Is_Draining(void) const;
		virtual void UpdateSight(unsigned, unsigned, unsigned, unsigned, unsigned);
		virtual void vt_entry_48C(unsigned, unsigned, unsigned, unsigned);
		virtual bool ForceCreate(Coord & coord, unsigned unk = 0);
		virtual void Radar_Track(void);
		virtual void Radar_Untrack(void);
		virtual void Plot_On_Radar(void) const;
		virtual void Update_Radar_Position(bool force_update=false);
		virtual int RespondMegaEventMission(void * event);
		virtual void ClearMegaMissionData(void);
		virtual bool HaveMegaMission(void) const;
		virtual bool HaveAttackMoveTarget(void) const;
		virtual int GetMegaMission(void) const;
		virtual Coord GetAttackMoveCoords(Coord * buffer);
		virtual bool CanUseWaypoint(void) const;
		virtual bool CanAttackOnTheMove(void) const;
		virtual bool MegaMissionIsAttackMove(void) const;
		virtual bool ContinueMegaMission(void);
		virtual void UpdateAttackMove(void);
		virtual bool RefreshMegaMission(void);

		bool Is_Immobilized(void) const;
		void Scatter_Incoming_Infantry(void) const;
		int Get_Collateral_Damage(void) const;
		DirType Barrel_Pitch(AbstractClass * target) const;
		void Laser_Zap(AbstractClass * target, int which, WeaponTypeClass const * weapon, Coord const & source_coord);
		void Rock(Coord const & coord, float force);
		void Draw_Health_Bar_Old(Point2D const & point, Rect const & rect) const;


		bool Is_Z_Fudge_Bridge(void) const;
		bool Was_Z_Fudge_Bridge(void) const;
		int Get_Z_Fudge_Column(void) const;
		int Get_Z_Fudge_Tunnel(void) const;
		int Get_Z_Fudge_Cliff(void) const;




		bool Is_Allowed_To_Retaliate(TechnoClass const * source, WarheadTypeClass const * warhead) const;
		virtual bool Is_Players_Army(void) const override;
		int Combat_Damage(int which=-1) const;

		Cell Nearby_Location(TechnoClass const * from=NULL) const;

		int Time_To_Build(void) const;
		virtual ActionType What_Action(ObjectClass const *, bool disallow_force = false) const override;
		virtual ActionType What_Action(Cell const &, bool check_fog = false, bool disallow_force = false) const override;
		virtual Coord Fire_Coord(int which, Coord base) const override;

		virtual HousesType Owner(void) const override;
		virtual HouseClass * Owner_HouseClass(void) const override;
		virtual bool Can_Player_Fire(void) const override;
		virtual bool Can_Player_Move(void) const override;
		virtual bool Can_Repair(void) const override;
		virtual int Value(void) const override;
		virtual int Get_Ownable(void) const override;
		virtual VisualType Visual_Character(bool raw = false, HouseClass const * = NULL) const override;

		virtual void Clicked_As_Target(int count = 7) override;
		virtual bool Select(void) override;
		Coord Predict_Target_Coord(void) const;

		double Area_Modify(Cell const & cell) const;
		void Base_Is_Attacked(TechnoClass const * enemy);
		void Kill_Cargo(TechnoClass * source);
		virtual void Record_The_Kill(TechnoClass * source) override;
		virtual bool In_Range(Coord const & coord, int which=0) const override;
		virtual void Override_Mission(MissionType mission, AbstractClass * tarcom, AbstractClass * navcom) override;
		virtual bool Restore_Mission(void) override;
		virtual int Weapon_Range(int which) const override;
		void Set_Owner(HouseClass * newowner);
		int Get_Sight_Bonus(Coord const & coord);
		static void Remove_Target(AbstractClass * target);
		bool Has_Ability(AbilityType ability) const;
		bool Should_Use_High_Arc(int which) const;
		double Target_Threat(TechnoClass * target, Coord const & firing_coord = COORD_NONE) const;

		virtual ResultType Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source=0, bool forced=false, bool=false, HouseClass * = 0) override;
		bool Evaluate_Cell(ThreatType method, int mask, Cell const & cell, int range, TechnoClass const ** object, int & value, int zone=0) const;
		bool Evaluate_Object(ThreatType method, int mask, int range, TechnoClass const * object, int & value, int zone=-1, Coord const & coord=COORD_NONE) const;
		int Evaluate_Just_Cell(Cell const & cell) const;

		void Assign_Archive_Target(AbstractClass * target);
		AbstractClass * Fetch_Archive_Target(void) const { return(ArchivedTarget); }
		__declspec( property( get=Fetch_Archive_Target, put=Assign_Archive_Target) ) AbstractClass * ArchiveTarget;

		Coord Railgun_Beam_Damage(Coord & coord, AbstractClass *abstract, WeaponTypeClass *weapon);

		inline WeaponTypeClass * Get_Primary_Weapon(void) const { const WeaponDataStruct * wdata = Get_Class_Weapon_Data(0); return(wdata->Weapon); }
		inline WeaponTypeClass * Get_Secondary_Weapon(void) const { const WeaponDataStruct * wdata = Get_Class_Weapon_Data(1); return(wdata->Weapon); }
		__declspec( property( get=Get_Primary_Weapon) ) WeaponTypeClass * PrimaryWeapon;
		__declspec( property( get=Get_Secondary_Weapon) ) WeaponTypeClass * SecondaryWeapon;

		virtual void AI(void) override;
		virtual bool Revealed(HouseClass * house) override;
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, int & param) override;

#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const override;
#endif

		void Techno_Draw_Object(ShapeSet const * shapefile, int shapenum, Point2D const & drawpoint, Rect const & rect, Dir256 rotation=DIR_N, int scale=0x0100, int zadjust=0, ZGradientType zgrad=ZGRAD_GROUND, bool=false, int brightness=0, int tint=0, ShapeSet const * zshapefile=0, int zshapenum=0, Point2D zoff = Point2D(0,0), ShapeFlags_Type negflags=SHAPE_NORMAL) const;

		virtual void Draw_Pre_Render(Point2D const & point, Rect const & cliprect) const override;
		virtual void Draw_Post_Render(Point2D const & point, Rect const & cliprect) const override;


		void Techno_Blit_Voxel(StaticBufferClass::Entry const & cache, Point2D const & point, Rect const & cliprect, ShapeFlags_Type flags, int brightness) const;
		void Techno_Draw_Voxel_Shadow(VoxelDataStruct const & voxeldata, int layer_index, int key, VoxelIndexClass * cache, Rect const & cliprect, Point2D const & point, Matrix3D const & matrix, bool force_cache, Surface * surface, int shadow_x, int shadow_y) const;
		SurfaceRegion Techno_Render_Voxel_Object(VoxelDataStruct const & voxeldata, Matrix3D const & matrix, Point2D const & point, Rect const & cliprect, int frame, ShapeFlags_Type flags, int brightness) const;
		SurfaceRegion Techno_Render_Voxel_Shadow(VoxelDataStruct const & voxeldata, Matrix3D const & matrix, Point2D const & point, Rect const & cliprect, int layer_index, ShapeFlags_Type flags, bool cached) const;

		virtual void Hidden(void) override;
		virtual bool Mark(MarkType mark=MARK_CHANGE) override;
		virtual int Exit_Object(TechnoClass * techno, Cell cell) override;
		virtual void Do_Shimmer(void) override;
		int Get_Predator_Offset(void) const;
		void Calculate_Sinking_Offset(short height, int y);
		void Draw_Target_Laser(void) const;
		static void Draw_Double_Selection_Bracket(Coord const & coord1, Coord const & coord2, int color);
		static void Draw_Single_Selection_Bracket(Coord const & coord1, Coord const & coord2, int color);

		virtual void Per_Cell_Process(PCPType why) override;
		virtual void Look(bool incremental=false, bool dontmap=false) override;
		static void Set_Talker(TechnoClass * techno, TalkType bubble=TALK_NONE);
		static void Set_Action_Lines(bool on);
		void Remove_Damage_Particle(void);
		bool Enter_Object_Nearby(void);
		bool Move_To_Object_Nearby(void);

		virtual bool Limbo(void) override;
		virtual bool Unlimbo(Coord const & , Dir256 facing=DIR_N) override;
		virtual void Init(void) override;
		virtual void Detach(AbstractClass const * target, bool all) override;

		static DynamicVectorClass<TechnoClass *> Array;

		static CDTimerClass<FrameTimerClass> ActionLineTimer;

		static bool ActionLines;

		static void Reset_Action_Line_Timer(void);

		static TalkType TalkBubbleType;
		static TechnoClass * TalkBubbleOwner;
		static CDTimerClass<SystemTimerClass> TalkBubbleTimer;

		static int const BodyShape[FACING_COUNT * 4];
};

inline bool TechnoClass::Is_Allowed_To_Leave_Map(void) const { return(false); }
inline bool TechnoClass::Is_Renovator(void) const { return(false); }
inline bool TechnoClass::Is_Turret_Equipped(void) const { return(TClass->IsTurretEquipped); }
inline bool TechnoClass::Is_Move_Override(void) const { return(false); }
inline bool TechnoClass::Is_In_Team(void) const { return(false); }
inline bool TechnoClass::Is_Ready_To_Move(void) const { return(true); }

inline DirType TechnoClass::Turret_Facing(void) const
{
	return(PrimaryFacing.Current());
}

inline WeaponDataStruct const * TechnoClass::Get_Class_Weapon_Data(int which) const
{
	if (Veterancy.Is_Elite() && which == 0) {
		return(TClass->Get_Weapon(2));
	}

	return(TClass->Get_Weapon(which));
}

inline void TechnoClass::Advance_Waypoint_Path(void)
{

}

inline void TechnoClass::Draw_Action_Line(void) const
{

}

inline bool TechnoClass::Is_In_Same_Zone_As(ObjectClass const * object) const
{
	return(true);
}

inline TechnoClass * AbstractClass::As_TechnoClass(void)
{
	return(dynamic_cast<TechnoClass *>(this));
}

inline TechnoClass const * AbstractClass::As_TechnoClass(void) const
{
	return(dynamic_cast<TechnoClass const *>(this));
}
