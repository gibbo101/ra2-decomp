// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "face.h"
#include "techtype.h"

#include "banim.hh"
#include "bsize.hh"
#include "bstate.hh"
#include "struct.hh"
#include "super.hh"

class OverlayTypeClass;
class IsometricTileTypeClass;
class InfantryTypeClass;
class BSurface;
class ShapeSet;

class BuildingTypeClass : public TechnoTypeClass
{
		typedef TechnoTypeClass BASECLASS;
	public:

		StructType HeapID;

		Cell const * OccupyList;

		void const * BuildupData;

		bool BuildupLoaded;

		int BuildCat;

		Point3D HalfDamageSmokeLocation1;
		Point3D HalfDamageSmokeLocation2;

		double GateCloseDelay;

		int LightVisibility;

		int LightIntensity;

		int LightRedTint;
		int LightGreenTint;
		int LightBlueTint;

		Point2D PrimaryFirePixelOffset;
		Point2D SecondaryFirePixelOffset;

		OverlayTypeClass const * ToOverlay;

		IsometricTileTypeClass const * ToTile;

		TStringID<15> BuildupFilename;

		int BuildupSound;
		int PackupSound;
		int CreateUnitSound;
		int UnitEnterSound;
		int UnitExitSound;
		int WorkingSound;
		int NotWorkingSound;

		TStringID<23> PowersUpBuilding;

		UnitTypeClass const * FreeUnit;

		InfantryTypeClass const * SecretInfantry;
		UnitTypeClass const * SecretUnit;
		BuildingTypeClass const * SecretBuilding;

		FacingType FoundationFace;

		int Adjacent;

		RTTIType ToBuild;

		Point3D TargetCoordOffset;

		Coord ExitCoordinate;

		Cell const * ExitList;

		Dir256 StartFace;

		int DeployFacing;

		int Power;
		int Drain;
		int ExtraPower;
		int ExtraDrain;

		BSizeType Size;

		int ZHeight;

		int OccupyHeight;

		int MidPoint;

		int DoorStages;

		struct AnimControlType {
			int	Start;
			int	Count;
			int	Rate;
		};
		AnimControlType Anims[BSTATE_COUNT];

		struct AnimDataType {

			char Anim[16];

			char AnimDamaged[16];

			char AnimGarrisoned[16];

			Point2D Location;

			int ZAdjust;

			int YSort;

			bool Powered;

			bool PoweredLight;

			bool PoweredEffect;

			bool PoweredSpecial;
		};

		AnimDataType AnimData[BANIM_COUNT];

		int Upgrades;

		ShapeSet const * DeployingAnim;
		bool DeployingAnimLoaded;

		ShapeSet const * UnderDoorAnim;
		bool UnderDoorAnimLoaded;

		ShapeSet const * Rubble;
		bool RubbleLoaded;

		ShapeSet const * RoofDeployingAnim;
		bool RoofDeployingAnimLoaded;

		ShapeSet const * UnderRoofDoorAnim;
		bool UnderRoofDoorAnimLoaded;

		ShapeSet const * DoorAnim;

		ShapeSet const * SpecialZOverlay;

		int SpecialZOverlayZAdjust;

		ShapeSet const * BibShape;
		bool BibShapeLoaded;

		int NormalZAdjust;

		int AntiAirValue;
		int AntiArmorValue;
		int AntiInfantryValue;

		Point2D ZShapePointMove;

		Rect DrawRect;

		short ExtraLight;

		bool IsCanTogglePower;

		bool HasSpotlight;

		bool IsTemple;

		bool IsPlug;

		bool IsHoverPad;

		bool IsBase;

		bool EligibleForAllyBuilding;
		bool EligibleForDelayKill;
		bool NeedsEngineer;

		int CaptureEvaEvent;
		int ProduceCashStartup;
		int ProduceCashAmount;
		int ProduceCashDelay;
		int InfantryGainSelfHeal;
		int UnitsGainSelfHeal;
		int RefinerySmokeFrames;

		bool IsBibbed;

		bool IsWall;

		bool IsCaptureable;

		bool IsPowered;

		bool PoweredSpecial;

		bool Overpowerable;

		bool Spyable;

		bool CanC4;

		bool WantsExtraSpace;

		bool IsUnsellable;

		bool ClickRepairable;

		bool CanBeOccupied;

		bool CanOccupyFire;

		int MaxNumberOccupants;

		bool ShowOccupantPips;

		Point2D MuzzleFlash[10];

		Point2D DamageFireOffset[8];

		Point2D QueueingCell;

		int NumberImpassableRows;

		Point2D RemoveOccupy[8];

		Point2D AddOccupy[8];

		bool IsRadar;

		bool SpySat;

		bool IsHasChargeAnim;

		bool IsAnimDelayedFire;

		bool IsSiloDamage;

		bool IsCanUnitRepair;

		bool IsCanUnitReload;

		bool Bunker;

		bool Cloning;

		bool Grinding;

		bool UnitAbsorb;

		bool InfantryAbsorb;

		bool SecretLab;

		bool DoubleThick;

		bool IsFlat;

		bool IsDockUnload;

		bool IsRecoilless;

		bool IsHasStupidGuardMode;

		bool IsBridgeRepairHut;

		bool IsGate;

		bool IsSAM;

		bool IsConstructionYard;

		bool IsNukeSilo;

		bool IsRefinery;

		bool IsWeeder;

		bool IsWeaponsFactory;

		bool IsLaserFencePost;

		bool IsLaserFence;

		bool IsFirestormWall;

		bool IsHospital;

		bool IsArmory;

		bool IsEMPulseCannon;

		bool IsTickTank;

		bool IsTurretAnimAVoxel;

		bool IsBarrelAnimAVoxel;

		bool IsCloakGenerator;

		bool IsSensorArray;

		bool IsICBMLauncher;

		bool IsArtillary;

		bool IsHelipad;

		bool OrePurifier;

		bool FactoryPlant;

		float InfantryCostBonus;
		float UnitsCostBonus;
		float AircraftCostBonus;
		float BuildingsCostBonus;
		float DefensesCostBonus;

		bool IsGDIBarracks;

		bool IsNODBarracks;

		bool IsYuriBarracks;

		float ChargedAnimTime;

		int DelayedFireDelay;

		SuperWeaponType SuperWeapon;

		SuperWeaponType SuperWeapon2;

		int GateStages;

		int PowersUpToLevel;

		bool IsDamagedDoor;

		bool IsInvisibleInGame;

		bool IsTerrainPalette;

		bool IsCanPlaceAnywhere;

		bool IsExtraDamageStage;

		bool CanAIBuildThis;

		bool IsBaseDefense;

		char CloakRadiusInCells;

		bool ConcentricRadialIndicator;

		int PsychicDetectionRadius;

		Dir256 StartPitch;

		char VoxelBarrelFile[19 + 1];

		double VoxelBarrelScale;

		Point3D VoxelBarrelOffsetToPitchPivotPoint;

		Point3D VoxelBarrelOffsetToRotatePivotPoint;

		Point3D VoxelBarrelOffsetToBuildingPivotPoint;

		Point3D VoxelBarrelOffsetToBarrelEnd;

		bool IsDemandLoad;

		bool IsDemandLoadBuildup;

		bool IsFreeBuildup;

		bool IsThreatRatingNode;

		bool PrimaryFireDualOffset;

		bool ProtectWithWall;

		bool CanHideThings;

		bool CrateBeneath;

		bool LeaveRubble;

		bool CrateBeneathIsMoney;

		char TheaterImageFile[18 + 1];

		int NumberOfDocks;

		VectorClass<Point3D> DockingOffsets;

		BuildingTypeClass(NoInitClass const & x);
		BuildingTypeClass(char const * ininame = NULL);
		virtual ~BuildingTypeClass() override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static StructType From_Name(char const * name);
		static StructType From_Given_Name(char const * name);
		static void Init(TheaterType theater);
		static void Fetch_Z_Data(void);
		static void Post_Load_Game(void);
		static void One_Time(void);
		static BuildingTypeClass * Find_Or_Make(char const * name);

		int Width(void) const;
		int Height(bool bib=false) const;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual bool CanUseWaypoint(void) const override;
		virtual bool CanAttackMove(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual int Fetch_Heap_ID(void) const override;

		virtual bool Read_INI(CCINIClass & ini) override;
		int Flush_For_Placement(Cell const & cell, HouseClass * house) const;
		virtual int Cost_Of(HouseClass * house = NULL) const override;
		virtual Coord const Coord_Fixup(Coord const & coord) const override;
		virtual int Max_Pips(void) const override;
		virtual Point3D Pixel_Dimensions(void) const override;
		virtual Point3D Lepton_Dimensions(void) const override;
		virtual bool Create_And_Place(Cell const & cell, HouseClass * house = NULL) const override;
		virtual ObjectClass * Create_One_Of(HouseClass * house) const override;
		virtual Cell const * Occupy_List(bool placement=false) const override;
		virtual void const * Get_Image_Data(void) const override;
		void Fetch_Building_Normal_Image(TheaterType theater);
		void Fetch_Building_Voxel_Image(void);
		virtual void const * Get_Buildup_Data(void) const;
		void Free_Buildup_Data(void);
		virtual bool Legal_Placement(Cell const & pos, HouseClass * house = NULL) const override;

		bool Is_Factory(void) const {return(ToBuild != RTTI_NONE);}
		virtual int Raw_Cost(void) const override;

		Cell const * Exit_List(void) const;
		Rect Get_Draw_Rect(void);

		static int Get_Max_Drain(DynamicVectorClass<BuildingTypeClass *> const & list);
		static void Post_Read_Tile_Fixup(void);
		bool Can_Always_Undeploy(void) const;
		bool Is_Mobile_Deployer(void) const;
		Dir256 Deploy_Facing(void) const;
		void Calculate_Base_Defense_Values(void);

	public:

		static void const * BuildingZShape;

		static void const * PowerOffShapes;

		static void const * WrenchShapes;

		static int SizeWidth[BSIZE_COUNT];
		static int SizeHeight[BSIZE_COUNT];

	private:

		static Cell const OccupyLists[BSIZE_COUNT][30];

		static Cell const ExitLists[BSIZE_COUNT][30];

		void Init_Anim(BStateType state, int start, int count, int rate) const;
};

inline BuildingTypeClass * AbstractClass::As_BuildingTypeClass(void) { return(dynamic_cast<BuildingTypeClass *>(this)); }
inline BuildingTypeClass const * AbstractClass::As_BuildingTypeClass(void) const { return(dynamic_cast<BuildingTypeClass const *>(this)); }

extern BSurface * CloakingSurface;
