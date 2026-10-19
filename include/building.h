// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "builtype.h"
#include "rect.h"
#include "techno.h"

#include "banim.hh"
#include "bstate.hh"
#include "check.hh"

#define	MAX_DOOR_STAGE			18
#define	DOOR_OPEN_STAGE			9
#define	MAX_REPAIR_ANIM_STAGE	5
#define BUILDING_UPGRADE_MAX	3
#define MAX_FIRESTORM_WALL_FRAMES 15

class BuildingLightClass;
class LightSourceClass;
class ConvertClass;
template<class T>
class DynamicVectorClass;
class FoggedObjectClass;

class BuildingClass : public TechnoClass
{
		typedef TechnoClass BASECLASS;

	public:

		BuildingTypeClass * Class;

		FactoryClass * Factory;

		CDTimerClass<FrameTimerClass> CountDown;

		BStateType BState;
		BStateType QueueBState;

		HousesType WhoLastHurtMe;

		AbstractClass * WhomToRepay;

		int LastStrength;

		AbstractClass * AnimToTrack;
		AnimClass * PsiWarnAnim;

		CDTimerClass<FrameTimerClass> PlacementDelay;

		AnimClass * Anims[BANIM_COUNT];
		bool AnimStates[BANIM_COUNT];

		AnimClass * DamageFireAnims[8];
		bool RequiresDamageFires;

		void FUN_0043C0D0(void);
		void FUN_0043C2A0(void);

		BuildingTypeClass const * Upgrades[BUILDING_UPGRADE_MAX];

		int LastSuperWeaponIndex;

		int TurretIndex;

		BuildingLightClass *BuildingLight;

		ProgressTimerClass<FrameTimerClass> GateTimer;

		LightSourceClass *LightSource;

		int LaserFenceFrame;

		int FirestormWallFrame;

		StageClass BuildingStage;

		Rect LastRenderRect;
		Coord LastRenderCoord;
		Point2D LastRenderOffset;

		bool IsOn;
		bool IsOverpowered;
		bool RegisteredAsPoweredUnitSource;

		int SupportingPrisms;
		bool HasExtraPowerBonus;
		bool HasExtraPowerDrain;
		DynamicVectorClass<InfantryClass *> Overpowerers;
		DynamicVectorClass<InfantryClass *> Occupants;
		int FiringOccupantIndex;
		AudioController Audio7;
		AudioController Audio8;

		bool WasOnline;
		bool IsNominal;
		bool IsToRebuild;
		bool IsToRepair;
		bool HasEngineer;
		CDTimerClass<FrameTimerClass> CashProductionTimer;
		bool IsAllowedToSell;
		bool IsReadyToCommence;
		bool IsWrenchVisible;
		bool IsGoingToBlow;
		bool IsSurvivorless;
		bool IsCharging;
		bool IsCharged;
		bool IsCaptured;
		bool ActuallyPlacedOnMap;
		bool HasOpened;
		bool IsDamagedAnims;
		bool IsFogged;
		bool IsRepairing;
		bool HasBuildupData;
		bool IsPoweredOn;
		char CloakGeneratorState;
		char CurrentCloakRadius;
		char TranslucencyLevel;

		int StorageFilledSlots;
		TechnoTypeClass * SecretProduction;
		unsigned char ColorAdd;
		unsigned char ColorAddG : 8;
		unsigned char ColorAddB : 8;
		int unknown_int_6FC;
		unsigned short Brightness;
		char UpgradeLevel;
		char GateFrame;
		int PrismStage;
		CoordStruct PrismTargetCoords;
		int DelayBeforeFiring;
		unsigned TankBunkerState;

		BuildingClass(BuildingTypeClass const * type = NULL, HouseClass * house = NULL);
		BuildingClass(NoInitClass const & x);
		virtual ~BuildingClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		BuildingTypeClass::AnimControlType const * Fetch_Anim_Control(void) {return(&Class->Anims[BState]);};

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual void const * Get_Image_Data(void) const override;
		virtual int How_Many_Survivors(void) const override;
		virtual void Scatter_Incoming_Infantry(void) const override;
		virtual int DistanceToDockingCoord(TechnoClass * docker) const;
		virtual int Get_Z_Adjust(void) const override;
		virtual bool Is_Move_Override(void) const override;
		virtual DirType Turret_Facing(void) const override;
		virtual bool Is_Ready_To_Cloak(void) const override;
		virtual bool Should_Uncloak(void) const override;
		virtual void Grand_Opening(bool captured = false);
		virtual Coord Turret_Coord(int which=0) const override;
		virtual InfantryTypeClass const * Crew_Type(void) const override;
		virtual int Pip_Count(void) const override;
		virtual bool Can_Player_Move(void) const override;
		virtual ActionType What_Action(ObjectClass const * target, bool disallow_force = false) const override;
		virtual ActionType What_Action(Cell const & cell, bool check_fog = false, bool disallow_force = false) const override;
		virtual bool Can_Repair(void) const override;
		virtual bool Can_Demolish(void) const override;
		bool Can_Upgrade(BuildingTypeClass const * upgrade, HouseClass const * upgrader) const;
		bool Add_Upgrade(void);
		bool Remove_Upgrade(void);
		void Sell_All_Upgrades(void);
		bool CanBeOccupiedBy(InfantryClass * infantry) const;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual wchar_t const * Full_Name(void) const override;
		virtual DirType Fire_Direction(void) const override;
		virtual bool IsStrange(void) const override;
		virtual bool CanBeSelectedNow(void) const override;
		virtual int IronCurtain(int duration, HouseClass * source, bool force_shield) override;
		virtual bool Ready_To_Next_Mission(void) const override;
		virtual int Do_MISSION_CAPTURE(void) override;
		virtual bool Is_Weapon_Equipped(void) const override;
		virtual unsigned vt_entry_2C8(unsigned, unsigned) override;
		virtual int Anti_Air(void) const override;
		virtual int Anti_Armor(void) const override;
		virtual int Anti_Infantry(void) const override;
		virtual void GotHijacked(void) override;
		virtual bool IsPowerOnline(void) const override;
		virtual int VoiceDeploy(void) override;
		virtual bool CanOccupyFire(void) const override;
		virtual int GetOccupyRangeBonus(void) const override;
		virtual int GetOccupantCount(void) const override;
		void KillOccupants(TechnoClass * killer);
		virtual void UpdateRefinerySmokeSystems(void) override;
		virtual bool Is_Radar_Visible(DetectedType & detected) const override;
		int Set_Turret_Frame(void);
		void Set_Turret_Index(int index);
		int Shape_Number(void) const;
		void DrawInfoTipAndSpiedSelection(Point2D * location, Rect * rect) const;
		int Power_Output(void) const;
		int Power_Drain(void) const;
		Cell Check_Point(CheckPointType cp) const;
		bool Is_Powered_On(void) const;
		int Tinted_Brightness(int cellbrightness);
		SuperWeaponType Fetch_Super_Weapon(void) const;
		SuperWeaponType Fetch_Super_Weapon2(void) const;
		bool Should_Fog(void) const;
		Matrix3D Get_Barrel_Matrix(void) const;

		virtual Coord Target_Coord(void) const override;
		virtual Coord Docking_Coord(TechnoClass * docker) const override;
		virtual Coord Render_Coord(void) const override;
		virtual Coord Fire_Coord(int which, Coord base) const override;
		virtual Coord Center_Coord(void) const override;
		virtual Coord Destination_Coord(TechnoClass * docker = NULL) const override;
		virtual int Sort_Y(void) const override;
		virtual Coord Exit_Coord(int index) const override;

		Coord Voxel_Fire_Coord(int which, bool just_fired) const;

		virtual void Init(void) override;
		virtual void Detach(AbstractClass const * target, bool all) override;
		virtual void Detach_All(bool all=true) override;
		virtual MoveType Can_Enter_Cell(CellClass const * cell, FacingType dir = FACING_NONE, int cell_height = -1, CellClass const * = 0, bool = true) const override;
		virtual bool Unlimbo(Coord const & , Dir256 dir = DIR_N) override;
		virtual bool Limbo(void) override;
		void Reserve_Base_Area(bool=false);
		void Release_Base_Area(void);

		virtual VisualType Visual_Character(bool raw = false, HouseClass const * = NULL) const override;
		virtual void Set_Occupy_Bit(Coord const & coord) override;
		virtual void Clear_Occupy_Bit(Coord const & coord) override;
		virtual int Exit_Object(TechnoClass * base, Cell cell) override;
		virtual bool Render(Rect &, bool forced, bool extras_only) const override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual void Editor_Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual bool Mark(MarkType mark=MARK_CHANGE) override;
		virtual Rect Get_Render_Rect(void) override;
		virtual void Draw_Radial_Indicator(DWORD) override;
		void Draw_Overlays(Point2D const & point, Rect const & cliprect) const;
		void Begin_Mode(BStateType bstate);
		virtual void Update_Buildables(void);
		virtual void Draw_Extras(Point2D &point, Rect &rect);
		virtual DirType Aim_Direction(AbstractClass * target) const;
		void AfterDestruction(void);
		void Make_Fogged(DynamicVectorClass<FoggedObjectClass *> * fogged_objects = NULL, CellClass * cellptr = NULL, bool fade = false);
		void Charge_Turret(void);
		void Discharge_Turret(void);

		virtual bool Active_Click_With(ActionType action, ObjectClass * object, bool) override;
		virtual bool Active_Click_With(ActionType action, Cell const & cell, Cell const & cell2, bool) override;
		virtual void Clicked_As_Target(int = 7) override;

		virtual void Death_Announcement(TechnoClass const * source=0) const override;
		virtual FireErrorType Can_Fire(AbstractClass *, int which, bool ignoreRange) const override;
		virtual AbstractClass * Greatest_Threat(ThreatType threat, Coord const & coord, bool) const override;
		virtual ResultType Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source=0, bool forced=false, bool=false, HouseClass * = 0) override;
		virtual bool Captured(HouseClass * newowner, bool announce = true) override;
		virtual WeaponDataStruct const * Get_Class_Weapon_Data(int which=0) const override;
		virtual bool Is_Turret_Equipped() const override;
		bool IsBeingDrained(void) const;
		int GetRadialIndicatorRange(void) const;
		void FireLaser(Coord coords);
		bool FUN_00448070(bool demote);
		void FUN_0044EB60(FactoryClass * factory);
		void FUN_004503F0(void);
		void FUN_004504F0(int stage, CoordStruct const & coords);
		void FUN_00450590(void);
		void FUN_00457DE0(bool, bool);
		void FUN_00458200(void);
		void FUN_00458330(void);
		void FUN_00458750(void);
		bool FUN_004587D0(ObjectTypeClass const * type);
		int FUN_004589C0(void) const;
		bool FUN_00458A00(CellClass const * cell);
		bool FUN_00458A80(Coord const & coord, Dir256 dir);
		void UpdateBunker(void);
		void UnloadBunker(void);
		void ClearBunker(void);
		void EmptyBunker(void);
		TechnoTypeClass const * FUN_00459840(void) const;
		int IsAbsorbAllowed(void) const;
		bool FUN_004598D0(void) const;
		bool FUN_00459CA0(void) const;
		void Update_Radar_Spied(void);
		void Spied_By(HouseClass * house);
		void Infiltrate(HouseClass * house);

		void Update_FS_Wall_State(void);
		bool Crossing_Firestorm(ObjectClass * object, bool do_damage);

		int Anti_Air_Defense_Value(void);
		int Anti_Armor_Defense_Value(void);
		int Anti_Infantry_Defense_Value(void);

		void Charging_AI(void);
		void Rotation_AI(void);
		void Factory_AI(void);
		void Repair_AI(void);
		void Animation_AI(void);
		virtual bool Revealed(HouseClass * house) override;
		virtual void Repair(int control) override;
		virtual void Sell_Back(int control) override;
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, int & param) override;
		virtual void AI(void) override;
		virtual void Cloaking_AI(bool fast) override;
		virtual void Assign_Target(AbstractClass * target) override;
		virtual void Do_Destruction(TechnoClass * last_contact, TechnoClass * source, bool forced, Cell const * offset);
		virtual bool Toggle_Primary(void);
		virtual void FUN_00455820(Cell cell = Cell());
		virtual void SensorArrayDeactivate(Cell cell = Cell());
		virtual void SensorArrayActivate(Cell cell = Cell());
		virtual void DisguiseDetectorDeactivate(Cell cell = Cell());
		virtual unsigned vt_entry_504(void);
		void Turn_On(void);
		void Turn_Off(void);
		void Power_On(void);
		void Power_Off(void);
		bool Open_Gate(void);
		bool Is_Gate_Open(void) const;
		bool Is_Blocked_By_Occupier(void);
		int Flush_For_Placement(TechnoClass * techno, Cell const & cell);
		void Enable_Cloak_Generator(void);
		void Disable_Cloak_Generator(void);
		void Disable_Sensor_Array(void);
		void EnableStuff(void);
		void FUN_00452820(InfantryClass * infantry);
		bool FUN_004528A0(AbstractClass const * target);
		bool FUN_00452940(CellClass const * cell);
		void FUN_004545D0(void);
		void FUN_00454730(void);
		void FUN_004547C0(void);
		void FUN_00454930(void);
		void FUN_004549B0(void);
		void DisableStuff(void);
		void Enable_Sensor_Array(void);

		virtual bool ForceCreate(Coord & coord, unsigned unk = 0) override;
		virtual int Do_MISSION_UNLOAD(void) override;
		virtual int Do_MISSION_REPAIR(void) override;
		virtual int Do_MISSION_ATTACK(void) override;
		virtual int Do_MISSION_HARVEST(void) override;
		virtual int Do_MISSION_GUARD(void) override;
		virtual int Do_MISSION_GUARD_AREA(void) override;
		virtual int Do_MISSION_CONSTRUCTION(void) override;
		virtual int Do_MISSION_DECONSTRUCTION(void) override;
		virtual int Do_MISSION_MISSILE(void) override;
		virtual int Do_MISSION_OPEN(void) override;
		bool Can_Be_Undeployed(void);
		virtual int Apparent_Brightness(int brightness = 1000) const override;
		virtual void Assign_Destination(AbstractClass * target, bool = true) override;
		void Assign_Rally_Point(Cell const & cell, bool play_eva);
		virtual bool Enter_Idle_Mode(bool initial=false,  bool = true) override;
		virtual void Radar_Track(void) override;
		virtual void Radar_Untrack(void) override;
		virtual void Plot_On_Radar(void) const override;

		bool Clear_Weapons_Factory_Bib(void);
		int FirstActiveSWIdx(void) const;
		int SecondActiveSWIdx(void) const;

		void Init_Laser_Fence(void);
		void Update_Laser_Fence_Connections(int explode);
		BuildingClass * Find_Laser_Fence_Post(FacingType dir, bool connected, int range);
		static void Init_All_Laser_Fences(void);
		void Connect_Laser_Fence(FacingType dir);
		void Disconnect_Laser_Fence(FacingType dir, bool explode);
		static void Unlimbo_Laser_Fence_Helper(Cell const & cell);
		static void __stdcall FUN_00453D60(Coord const & coord);
		void Init_Laser_Fence_Frame(void);
		void Toggle_Laser_Fence_Post(bool force);

		int Get_Firestorm_Wall_Frame(void);

		void Begin_Anim(BAnimType anim, bool damaged, int delay = 0);
		void Set_Anim_Coords(void);
		void PlayAnim(char const * name, BAnimType anim, bool damaged, bool garrisoned, int delay);
		void PlayNthAnim(BAnimType anim, bool damaged, bool garrisoned, int delay = 0);
		void Detach_Anim(AnimClass * anim);
		void DestroyNthAnim(BAnimType anim);
		void HideNthAnim(BAnimType anim);
		void DisableTemporal(void);
		void EnableTemporal(void);
		void Set_Anim_Damage_State(bool damaged);
		void Set_Anim_Drawer(ConvertClass * drawer, int brightness);
		void Update_Anim_Appearance(void);
		void Set_Anim_Translucency(int translucency);
		bool Anim_Active(BAnimType anim) { return(Anims[anim] != NULL); }

#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const override;
#endif

		static void Read_INI(CCINIClass const & ini);
		static void Write_All(CCINIClass & ini);
		void Write_INI(CCINIClass & ini);

	private:
		void Drop_Debris(AbstractClass * source = NULL);

		static char const * const INI_NAME;
};

void __fastcall DrawRadialIndicator(bool draw_line, bool adjust_color, Coord center, RGBClass color, float radius, bool in_pixels, bool whole);
void Adjust_House_Power(HouseClass * house);
BuildingClass * Find_Unit_Repair_Facility(HouseClass * house, TechnoClass * obj);

inline BuildingClass * AbstractClass::As_BuildingClass(void)
{
	return(dynamic_cast<BuildingClass *>(this));
}

inline BuildingClass const * AbstractClass::As_BuildingClass(void) const
{
	return(dynamic_cast<BuildingClass const *>(this));
}
