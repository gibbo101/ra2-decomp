// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_house.h"
#include "base.h"
#include "coord.h"
#include "counter.h"
#include "credits.h"
#include "dropship.h"
#include "ftimer.h"
#include "iaihouse.h"
#include "ihouse.h"
#include "ipublichouse.h"
#include "object.h"
#include "region.h"
#include "rgb.h"
#include "smartptr.h"
#include "storage.h"
#include "teamtype.h"
#include "timer.h"
#include "typelist.h"
#include "utracker.h"

#include "aircraft.hh"
#include "diff.hh"
#include "house.hh"
#include "infantry.hh"
#include "prodfail.hh"
#include "quarry.hh"
#include "rtti.hh"
#include "source.hh"
#include "state.hh"
#include "struct.hh"
#include "super.hh"
#include "tiberium.hh"
#include "unit.hh"
#include "urgency.hh"
#include "zone.hh"

template<class T> class DynamicVectorClass;
class TriggerClass;
class FootClass;
class FactoryClass;
class HouseTypeClass;
class ObjectClass;
class AnimClass;
class BulletClass;
class TechnoClass;
class TagClass;
class BuildingClass;
class WaypointPathClass;
struct StartingTechnoStruct;
class WaypointClass;
class SuperClass;
class AbstractClass;
class UnitClass;
class CCINIClass;
class ObjectTypeClass;
template<class T> class DynamicVectorClass;

#define HOUSE_NAME_MAX	20

class HouseStaticClass {
	public:
		HouseStaticClass(void);
		HouseStaticClass(NoInitClass const & ) {};

		int IQ;

		int TechLevel;

		int Allies;

		int InitialCredits;

		SourceType Edge;
};

struct StartingTechnoStruct {
	TechnoTypeClass const * Type;
	Cell CellID;
};

class HouseClass : public AbstractClass, public IHouse, public IPublicHouse, public IConnectionPointContainer
{
		typedef AbstractClass BASECLASS;

	public:

		HousesType HeapID;

		HouseTypeClass * Class;

		DynamicVectorClass<TagClass *> HouseTags;

		DynamicVectorClass<BuildingClass *> ConYards;
		DynamicVectorClass<BuildingClass *> Buildings;
		DynamicVectorClass<BuildingClass *> UnitRepairStations;
		DynamicVectorClass<BuildingClass *> Grinders;
		DynamicVectorClass<BuildingClass *> Absorbers;
		DynamicVectorClass<BuildingClass *> Bunkers;
		DynamicVectorClass<BuildingClass *> Occupiables;
		DynamicVectorClass<BuildingClass *> CloningVats;
		DynamicVectorClass<BuildingClass *> SecretLabs;
		DynamicVectorClass<BuildingClass *> PsychicDetectionBuildings;
		DynamicVectorClass<BuildingClass *> FactoryPlants;

		int CountResourceGatherers;
		int CountResourceDestinations;
		int CountWarfactories;

		int InfantrySelfHeal;
		int UnitsSelfHeal;

		DynamicVectorClass<StartingTechnoStruct *> StartingUnits;

		DiffType Difficulty;

		double FirepowerBias;
		double GroundspeedBias;
		double AirspeedBias;
		double ArmorBias;
		double ROFBias;
		double CostBias;
		double BuildSpeedBias;
		double RepairDelay;
		double BuildDelay;

		HouseStaticClass Control;

		enum {
			EVERYTHING,
			BUILDINGS,
			UNITS,
			NOTHING
		} ProductionMode;

		HousesType ActLike;

		bool IsHuman;
		bool IsPlayerControl;
		bool IsStarted;
		bool IsAlerted;
		bool NodeLogic_1F0;
		bool ShipYardConst_1F1;
		bool IsAITriggersOn;
		bool IsBaseBuilding;
		bool IsDiscovered;
		bool IsDefeated;
		bool IsToDie;
		bool IsToWin;
		bool IsToLose;
		bool IsCivEvacuated;
		bool FirestormDefenseActivated;
		bool IsThreatRatingNodeActive;
		bool IsRecalcNeeded;

		int IPAddress;
		int SquadID;
		bool LostConnection;

		PathType SelectedPath;
		WaypointPathClass * Paths[PATH_COUNT];

		bool IsVisionary;
		bool MapIsClear;
		bool IsTiberiumShort;
		bool IsSpied;
		bool IsThieved;
		bool DidRepair;
		bool IsBuiltSomething;
		bool IsResigner;
		bool IsGiverUpper;
		bool IsAllToHunt;
		bool IsParanoid;
		bool IsToLook;

		int IQ;

		StateType State;

		DynamicVectorClass<SuperClass *> SuperWeapon;

		StructType JustBuiltStructure;
		InfantryType JustBuiltInfantry;
		UnitType JustBuiltUnit;
		AircraftType JustBuiltAircraft;

		int Blockage;

		CDTimerClass<FrameTimerClass> RepairTimer;
		CDTimerClass<FrameTimerClass> AlertTime;
		CDTimerClass<FrameTimerClass> BorrowedTime;
		CDTimerClass<FrameTimerClass> PowerBlackoutTimer;
		CDTimerClass<FrameTimerClass> RadarBlackoutTimer;

		bool Side2TechInfiltrated;
		bool Side1TechInfiltrated;
		bool Side0TechInfiltrated;
		bool BarracksInfiltrated;
		bool WarFactoryInfiltrated;

		unsigned InfantryAltOwner;
		unsigned UnitAltOwner;
		unsigned AircraftAltOwner;
		unsigned BuildingAltOwner;

		int AirportDocks;
		int PoweredUnitCenters;

		unsigned CreditsSpent;
		unsigned HarvestedCredits;
		int StolenBuildingsCredits;

		int CurUnits;
		int OwnedNavy;
		int CurBuildings;
		int CurInfantry;
		int CurAircraft;

		StorageClass Tiberium;
		int Credits;
		int Capacity;
		StorageClass Weed;

		int field_1BC;

		UnitTrackerClass AircraftTotals;
		UnitTrackerClass InfantryTotals;
		UnitTrackerClass UnitTotals;
		UnitTrackerClass BuildingTotals;
		UnitTrackerClass DestroyedAircraft;
		UnitTrackerClass DestroyedInfantry;
		UnitTrackerClass DestroyedUnits;
		UnitTrackerClass DestroyedBuildings;
		UnitTrackerClass CapturedBuildings;
		UnitTrackerClass TotalCrates;

		int AircraftFactories;
		int InfantryFactories;
		int UnitFactories;
		int BuildingFactories;
		int NumShipyards;
		int NumOrePurifiers;

		float CostInfantryMult;
		float CostUnitsMult;
		float CostAircraftMult;
		float CostBuildingsMult;
		float CostDefensesMult;

		int Power;
		int Drain;

		FactoryClass * AircraftFactory;
		FactoryClass * InfantryFactory;
		FactoryClass * UnitFactory;
		FactoryClass * Primary_ForShips;
		FactoryClass * BuildingFactory;
		FactoryClass * Primary_Unused1;
		FactoryClass * Primary_Unused2;
		FactoryClass * Primary_Unused3;
		FactoryClass * Primary_ForDefenses;

		unsigned char AircraftType_53D0;
		unsigned char InfantryType_53D1;
		unsigned char VehicleType_53D2;
		unsigned char ShipType_53D3;
		unsigned char BuildingType_53D4;
		unsigned char unknown_53D5;
		unsigned char unknown_53D6;
		unsigned char unknown_53D7;
		unsigned char DefenseType_53D8;
		unsigned char unknown_53D9;
		unsigned char unknown_53DA;
		unsigned char unknown_53DB;

		AbstractClass * FlagLocation;
		Cell FlagHome;

		int UnitsKilled[20];
		int UnitsLost;
		int BuildingsKilled[20];
		int BuildingsLost;

		HousesType WhoLastHurtMe;

		Cell BaseSpawnCell;
		Cell BaseCenter;

		int Radius;

		struct {
			int AirDefense;
			int ArmorDefense;
			int InfantryDefense;
		} ZoneInfo[ZONE_COUNT];

		int LATime;
		HousesType LAEnemy;
		AbstractClass * ToCapture;
		int RadarSpied;
		int PointTotal;
		QuarryType PreferredTarget;

		Cell PreferredTargetCell;
		Cell PreferredDefensiveCell;
		Cell PreferredDefensiveCell2;
		int PreferredDefensiveCellStartTime;

		CounterClass BQuantity;
		CounterClass UQuantity;
		CounterClass IQuantity;
		CounterClass AQuantity;

		CounterClass ABQuantity;
		CounterClass AUQuantity;
		CounterClass AIQuantity;
		CounterClass AAQuantity;

		CounterClass PBQuantity;
		CounterClass PUQuantity;
		CounterClass PIQuantity;
		CounterClass PAQuantity;

		struct AttackStruct {
			CDTimerClass<FrameTimerClass> Timer;

			unsigned InitialAttack;
			AttackStruct(void) { InitialAttack = 0; }
			AttackStruct(NoInitClass const & x) : Timer(x) { }
		} Attack;

	public:

		HousesType Enemy;

		DynamicVectorClass<AngerStruct> AngerNodes;

		DynamicVectorClass<ScoutStruct> ScoutNodes;

		CDTimerClass<FrameTimerClass> AITimer;
		CDTimerClass<FrameTimerClass> PickEnemyTimer;

		StructType BuildStructure;
		UnitType BuildUnit;
		InfantryType BuildInfantry;
		AircraftType BuildAircraft;

		int RatioAITriggerTeam;

		int RatioTeamAircraft;
		int RatioTeamInfantry;
		int RatioTeamUnits;

		int BaseDefenseTeamCount;

		HouseClass(HouseTypeClass const * type = NULL);
		HouseClass(NoInitClass const & x);
		virtual ~HouseClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
		virtual ULONG STDMETHODCALLTYPE Release(void) override;

		virtual LONG STDMETHODCALLTYPE ID_Number(void) override;
		virtual BSTR STDMETHODCALLTYPE Name(void) override;
		virtual IApplication *STDMETHODCALLTYPE Get_Application(void) override;
		virtual LONG STDMETHODCALLTYPE Available_Money(void) override;
		virtual LONG STDMETHODCALLTYPE Available_Storage(void) override;
		virtual LONG STDMETHODCALLTYPE Power_Output(void) override;
		virtual LONG STDMETHODCALLTYPE Power_Drain(void) override;
		virtual LONG STDMETHODCALLTYPE Category_Quantity(CategoryType category) override;
		virtual LONG STDMETHODCALLTYPE Category_Power(CategoryType category) override;
		virtual CellStruct STDMETHODCALLTYPE Base_Center(void) override;
		virtual HRESULT STDMETHODCALLTYPE Fire_Sale(void) override;
		virtual HRESULT STDMETHODCALLTYPE All_To_Hunt(void) override;

		virtual LONG STDMETHODCALLTYPE Apparent_Category_Quantity(CategoryType category) override;
		virtual LONG STDMETHODCALLTYPE Apparent_Category_Power(CategoryType category) override;
		virtual CellStruct STDMETHODCALLTYPE Apparent_Base_Center() override;
		Cell Get_Base_Center(void) const;
		Coord Get_Base_Coord(void);
		bool IonSensitivesShouldBeOffline(void) const;
		virtual bool STDMETHODCALLTYPE Is_Powered() override;

		virtual HRESULT STDMETHODCALLTYPE EnumConnectionPoints(IEnumConnectionPoints **ppEnum) override;
		virtual HRESULT STDMETHODCALLTYPE FindConnectionPoint(REFIID riid, IConnectionPoint **ppCP) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual int Fetch_Heap_ID(void) const override;

		Cell Random_Cell_In_Zone(ZoneType zone) const;
		static void Computer_Paranoid(void);
		static HouseClass * FindByCountryIndex(int index);
		static int FindIndexByName(char const * name);
		bool FUN_0050C340(int node);
		bool Is_Allowed_To_Ally(HouseClass * house) const;
		void Super_Weapon_Handler(void);
		int * Factory_Counter(RTTIType rtti, unsigned char naval);
		int Factory_Count(RTTIType rtti) const;
		void CalculateCostMultipliers(void);
		double FUN_0050C050(TechnoTypeClass const * type) const;
		double FUN_0050C0A0(TechnoTypeClass const * type) const;
		bool FUN_0050B370(TechnoTypeClass const * type);
		void FUN_0050E010(UnitTypeClass * type);
		void LostPoweredCenter(UnitTypeClass * type);
		bool GainedPoweredCenter(TechnoTypeClass const * type);
		int FUN_00500910(RTTIType rtti, bool naval) const;
		DiffType Assign_Handicap(DiffType handicap);
		AbstractClass * Find_Juicy_Target(Coord const & coord) const;
		Cell Where_To_Go(FootClass const * object) const;
		Cell Zone_Cell(ZoneType zone) const;
		ZoneType Which_Zone(Coord const & coord) const;
		ZoneType Which_Zone(ObjectClass const * object) const;
		ZoneType Which_Zone(Cell const & cell) const;
		Cell Find_Cell_In_Zone(TechnoClass const * techno, ZoneType zone) const;
		ProdFailType Begin_Production(RTTIType type, int id, bool naval, bool resume);
		void FUN_005007A0(RTTIType type, bool naval, int buildcat, bool flag);
		ProdFailType Suspend_Production(RTTIType type, int id, bool naval);
		ProdFailType Abandon_Production(RTTIType type, int id, bool naval, bool all);
		bool Place_Object(RTTIType type, int id, bool naval, Cell const * cell);
		bool Manual_Place(BuildingClass * builder, BuildingClass * object);
		void Just_Built(TechnoClass * product);
		void Special_Weapon_AI(SuperWeaponType id);
		void FUN_004FAF00(SuperClass * super, Cell const * cell);
		void FUN_0050D6D0(Cell cell);
		bool Place_Special_Blast(SuperWeaponType id, Cell const & cell);
		bool Flag_Attach(Cell const & cell, bool set_home = false);
		bool Flag_Attach(UnitClass * object, bool set_home = false);
		bool Flag_Remove(AbstractClass * target, bool set_home = false);
		void Init_Data(int color, HousesType house, int credits);
		Coord Find_Build_Location(BuildingClass * building) const;
		BuildingClass * Find_Building(StructType type, ZoneType zone=ZONE_NONE) const;
		int Expert_AI(void);
		void Production_Begun(TechnoClass const * rtti);
		void Sell_Wall(Cell const & cell, bool quiet = true);
		bool Flag_To_Die(void);
		bool Flag_To_Win(bool silent = false);
		bool Flag_To_Lose(bool silent = false);
		void Flag_To_End(void);
		void Destroy_All(void);
		void Destroy_All_Buildings(void);
		void Destroy_All_Non_Buildings_Non_Naval(void);
		void Destroy_All_Non_Buildings_Naval(void);
		void Cheer(void);
		void FUN_0050C920(void);
		bool FUN_0050CAD0(BaseNodeClass * node);
		void FUN_0050D250(Cell offset);
		void FUN_0050D290(HouseClass * other);
		void FUN_0050D2D0(HouseClass * other);
		void RespawnStartingForces(void);
		void FUN_0050D560(TechnoTypeClass const * type, Cell cell);
		void FUN_0050D5D0(void);
		void FUN_0050D610(void);
		bool FUN_0050D9C0(void) const;
		bool FUN_0050D9D0(void) const;
		int GetInfSelfHealStep(void) const;
		int GetUnitSelfHealStep(void) const;
		void RespawnStartingBuildings(void);
		void Make_Ally(HouseClass * house, bool announce);
		void Make_Ally(HousesType house, bool announce);
		void Make_Ally(ObjectClass * object, bool announce);
		void Make_Enemy(HouseClass * house, bool announce);
		void Make_Enemy(HousesType house, bool announce);
		void Make_Enemy(ObjectClass * object, bool announce);
		bool Is_Ally(HousesType house) const;
		bool Is_Ally(HouseClass const * house) const;
		bool Is_Ally(ObjectClass const * object) const;
		bool Is_Ally(AbstractClass const * target) const;
		bool CanOverpower(AbstractClass const * target) const;
#ifdef _DEBUG
		void Debug_Dump(MonoClass *mono) const;
		void Print_Zone_Stats(int x, int y, ZoneType zone, MonoClass * mono) const;
#endif
		virtual void AI(void) override;

		void Add_Anger(int level, HouseClass const * house);
		void Mark_Scouted(HouseClass const * house);

		void Begin_Construction(void);
		BuildingTypeClass const * Get_First_Ownable(DynamicVectorClass<BuildingTypeClass const *> const & owned) const;
		bool AI_Has_Prerequisites(TechnoTypeClass const * type, DynamicVectorClass<BuildingTypeClass const *> & owned, int ownedcount) const;
		void Make_Base_Nodes(void);
		static int Base_Cell_Weight_By_Distance(HouseClass const & house, Cell const & cell, int tie_breaker, int context);
		static int Base_Cell_Weight_By_Defense_Coverage(HouseClass const & house, Cell const & cell, int tie_breaker, int context);
		Cell Where_To_Place_Upgrade(BuildingTypeClass const * upgrade) const;
		Cell Where_To_Place_Building(BuildingTypeClass *buildingtype, int (*func)(HouseClass const &, Cell const &, int, int), int context);
		void Calculate_Defense_Values(BuildingClass const * building, int value, int * values, Rect const & area) const;
		DynamicVectorClass<BuildingTypeClass *> Get_Anti_Air_Defense_Buildings(DynamicVectorClass<BuildingTypeClass const *> & owned) const;
		DynamicVectorClass<BuildingTypeClass *> Get_Anti_Armor_Defense_Buildings(DynamicVectorClass<BuildingTypeClass const *> & owned) const;
		DynamicVectorClass<BuildingTypeClass *> Get_Anti_Ground_Defense_Buildings(DynamicVectorClass<BuildingTypeClass const *> & owned) const;

		void Recalc_Power_Drain(void);
		void Recalc_Radar_Availability(void);
		void Recalc_Spy_Satellite(void);
		void Create_Radar_Outage(int duration);
		void CreatePowerOutage(int duration);
		void ReshroudMap(void);

		void Update_Present_Super_Weapons(void);
		void AISupers(void);
		void Enable_Available_Super_Weapons(void);

		void Calculate_Enemy_Predictions(void);
		bool AI_Build_Defense(int nodeid, DynamicVectorClass<Cell> * cells);
		void AI_Build_Wall(void);

		void Clear_Anger(HouseClass const * house);
		void Recalc_Threat_Regions(void);
		void Activate_Threat_Node(void);

		int Owned_Team_Count(TeamTypeClass const * teamtype) const;
		bool Can_Create_Team(TeamTypeClass const * teamtype);

		WaypointPathClass * Ensure_Path(PathType path);
		void Place_Waypoint(Coord const & coord);
		bool Select_Waypoint(Coord const & coord);
		bool Can_Place_Waypoint(Cell const & cell);
		bool Fetch_Waypoint_Data(WaypointClass * waypt, PathType & path, char & waypt_id);
		WaypointClass * Waypoint_At(Cell const & cell);
		PathType New_Waypoint_Path(void) const;
		bool Can_Add_Waypoint_To_Path(void) const;

		FactoryClass * Fetch_Factory(RTTIType rtti, bool naval, int buildcat) const;
		void Set_Factory(FactoryClass * factory, RTTIType rtti, bool naval, int buildcat);
		void FUN_005005B0(RTTIType rtti, bool naval, int buildcat);
		void FUN_00500660(RTTIType rtti, bool naval, int buildcat);
		unsigned char FUN_00500700(RTTIType rtti, bool naval, int buildcat) const;
		void Update_Production_Mode(RTTIType type);
		void Production_Status_Changed(void) {IsRecalcNeeded = true;}

		int Can_Build(ObjectTypeClass const * type, bool illegal, bool nofactory) const;
		FactoryClass * Factory_Producing_This(ObjectTypeClass const * object) const;

		TechnoTypeClass const * Suggest_New_Object(RTTIType objectype, bool kennel=false) const;
		BuildingTypeClass const * Suggest_New_Building(void) const;
		void Recalc_Center(void);
		void Set_Base_Spawn_Cell(Cell cell);
		void FUN_0050C210(void);
		bool Does_Enemy_Building_Exist(StructType) const;
		void Harvested(int tiberium, TiberiumType slot);
		float Give_Tiberium(float amount, int slot);
		BuildingClass * Emptiest_Silo(void) const;
		void Harvested_Weed(int weed, int slot);
		void Spend_Money(int money);
		void Refund_Money(int money);
		void Attacked(BuildingClass * source);
		void Adjust_Power(int adjust);
		void Adjust_Drain(int adjust);
		void Update_Spied_Radar(HouseClass * house);
		void Update_Spied_Power_Plants(void);
		void Update_Factories(RTTIType rtti, bool naval, int buildcat);
		double Power_Fraction(void) const;
		double Tiberium_Fraction(void) const;
		double Weed_Fraction(void) const;
		void Begin_Production(void) {IsStarted = true;};
		SUGGESTED_TEAM_LIST Suggested_New_Team(bool alertcheck = false);
		void Adjust_Threat(int region, int threat);
		void Tracking_Remove(TechnoClass const * techno);
		void Tracking_Add(TechnoClass const * techno);
		void RegisterLoss(TechnoClass * techno, bool keeptiberium);
		void RegisterGain(TechnoClass * techno, bool ownerchange);
		void Tracking_Active_Remove(TechnoClass * techno, bool bycapture);
		void Tracking_Active_Add(TechnoClass * techno, bool bycapture);
		void Active_Remove(TechnoClass const * techno);
		void Active_Add(TechnoClass const * techno);

		UrgencyType Check_Fire_Sale(void);
		UrgencyType Check_Raise_Money(void);

		bool AI_Fire_Sale(UrgencyType urgency);
		bool AI_Raise_Money(UrgencyType urgency);

		void AI_TryFireSW(void);
		void AI_Hunter_Seeker(SuperClass * super);
		void AI_Multi_Missile(SuperClass * super);
		void AI_Chem_Missile(SuperClass * super);
		void AI_Drop_Pods(SuperClass * super);
		void Fire_ParaDrop(SuperClass * super);
		void Fire_LightningStorm(SuperClass * super);
		void Fire_PsyDom(SuperClass * super);
		void Fire_GenMutator(SuperClass * super);
		void FUN_00509EC0(SuperClass * super);
		void FUN_0050A350(SuperClass * super);
		Cell PickIonCannonTarget(void);
		Cell PickTargetByType(QuarryType quarry);

		void AI_Takeover(void);

		void Activate_Firestorm(void);
		void Deactivate_Firestorm(void);
		void Lost_Firestorm_Generator(void);

		bool Can_Make_Money(void);
		void MindControlBuildings(HouseClass * controller);
		void RelinquishBuildings(HouseClass * controller);
		TechnoTypeClass const * FirstBuildableFromArray(DynamicVectorClass<BuildingTypeClass const *> const & list) const;

		static void One_Time(void);
		static AnimClass * PsiWarn(HouseClass * house, ObjectClass * target, BulletClass * bullet, char const * name);

		void Invalidate_Base_Node_Position(BuildingClass * building);
		SourceType FUN_0050DB00(BuildingClass * building);
		bool Is_Build_Limited(TechnoTypeClass const * type) const;

		bool Is_Player_Control(void) const;
		SourceType FUN_0050DAA0(void) const;
		void FUN_0050DA00(Cell cell);
		void FUN_0050DA10(void);
		void FUN_0050DA20(Cell cell);
		void FUN_0050DA40(Cell cell);
		void FUN_0050DA50(void);
		SourceType FUN_0050DA80(void) const;
		SourceType FUN_0050DAC0(void) const;
		Coord FUN_0050DF30(void) const;
		void FUN_0050DFE0(Cell cell);
		void FUN_0050DFF0(void);
		bool Is_Human_Player(void) const;

		bool Can_Build_Here(BuildingTypeClass *building,  Cell const & cell);
		double GetArmorMultiplier(TechnoTypeClass const * type) const;
		float FUN_0050BDF0(TechnoTypeClass const * type) const;
		float FUN_0050BEB0(TechnoTypeClass const * type) const;
		bool FUN_0050B730(void) const;
		bool FUN_0050B760(BuildingTypeClass const * type, Cell const & cell);
		float FUN_0050C160(void) const;

		static void Read_All(CCINIClass const & ini);
		void Read_INI(CCINIClass const & ini);
		static void Write_All(CCINIClass & ini);
		void Write_INI(CCINIClass & ini);

		void Detach(AbstractClass const * target, bool all);

		DropshipLoadoutClass DropshipLoadouts[3];

		int CurrentDropship;

		bool HasCloakGenerator;

		RGBClass RemapColorRGB;
		RGBClass LaserColor;
		void Initialize_Radar_Color(void);
		void FUN_0050B840(void);
		void FUN_0050BA00(void);

		BaseClass Base;

		bool RecalcPower;
		bool RecalcRadar;
		bool SpySatActive;
		bool IsBeingDrained;

		SourceType Edge;

		Cell EMPDest;

		Cell NukeDest;

		void Clobber_All(void);

		void Blowup_All(void);

		void MPlayer_Defeated(void);

		friend class MapEditClass;

	private:
		void Silo_Redraw_Check(int oldtib, int oldcap);
		int AI_Building(void);
		int AI_Unit(void);
		int AI_Infantry(void);
		int AI_Aircraft(void);

		unsigned Allies;

		CDTimerClass<FrameTimerClass> DamageTime;

		CDTimerClass<FrameTimerClass> TeamTime;

		CDTimerClass<FrameTimerClass> TriggerTime;

		CDTimerClass<FrameTimerClass> SpeakAttackDelay;
		CDTimerClass<FrameTimerClass> SpeakPowerDelay;
		CDTimerClass<FrameTimerClass> SpeakMoneyDelay;
		CDTimerClass<FrameTimerClass> SpeakMaxedDelay;

		struct BuildChoiceClass {
			UrgencyType	Urgency;
			StructType	Structure;

			BuildChoiceClass(UrgencyType urgency=URGENCY_NONE, StructType structure=STRUCT_NONE) : Urgency(urgency), Structure(structure) {};
			BuildChoiceClass(NoInitClass const & ) {};
			bool operator==(BuildChoiceClass const & ) const {return(false);}
			bool operator!=(BuildChoiceClass const & ) const {return(true);}
			HRESULT Save(IStream *) const {return(S_OK);};
			HRESULT Load(IStream *) {return(S_OK);};
		};

		static DynamicVectorClass<BuildChoiceClass *> BuildChoice;

		IAIHouse *AIGeneral;

	public:

		RegionClass Regions[MAP_TOTAL_REGIONS];

	public:

		TStringID<HOUSE_NAME_MAX> IniName;
		char UINameString[33];
		wchar_t UIName[21];

		int Scheme;

		int QuantityB(int index) {return(BQuantity.Value(index));}
		int QuantityU(int index) {return(UQuantity.Value(index));}
		int QuantityI(int index) {return(IQuantity.Value(index));}
		int QuantityA(int index) {return(AQuantity.Value(index));}

		int StartingPoint;
		unsigned StartingAllies;

		int *BaseAreaMap;

		DynamicVectorClass<IConnectionPoint *> PowerEventConnectionPoints;

		IConnectionPoint *PowerEvents;

		int field_10E20;
		unsigned unknown_16084;

		double field_10E28;

		double DefenseCostMultiplier;

		unsigned int : 32;

		float EnemyArmorForcePrediction;
		float EnemyAirForcePrediction;
		float EnemyInfantryForcePrediction;

		int TotalOwnedInfantryCost;
		int TotalOwnedVehicleCost;
		int TotalOwnedAircraftCost;

		int PowerSurplus;
};

HouseClass * House_From_HousesType(HousesType house);
