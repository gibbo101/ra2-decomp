// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ftimer.h"
#include "iloco.h"
#include "team.h"
#include "techno.h"

class UnitClass;
class BuildingClass;
class WaypointClass;
struct PathStruct;

class AircraftTrackerClass
{
	public:
		void Add(FootClass * foot);
		void Remove(FootClass * foot);
		void Update(FootClass * foot, Cell oldcell, Cell newcell);
		bool IsJumpjet(FootClass * foot);
		void Clear(void);
};

class ParasiteClass;

class FootClass : public TechnoClass
{
		typedef TechnoClass BASECLASS;
	public:

		PathType CurrentPath;

		Cell WaypointOffsetCell;
		Cell WaypointTargetCell;

		double ThreatAvoidanceCoefficient;

		int TotalFramesWalked;

		bool IsMoveSoundPlaying;
		int MoveSoundDelay;
		AudioController MoveSoundAudioController;

		Cell LastPathingCell;
		Cell LastAdjacencyCell;
		Cell LastFlightMapCoords;
		Cell CurrentJumpjetMapCoords;

		Coord LastTubeCoord;

		double Speed;
		double SpeedBias;

		DynamicVectorClass<AbstractClass *> RouteQueue;

		AbstractClass * unknown_5A0;

		AbstractClass * NavCom;
		AbstractClass * SuspendedNavCom;

		DynamicVectorClass<AbstractClass *> NavQueue;

		MissionType MegaMission;
		AbstractClass * MegaDestination;
		AbstractClass * MegaTarget;
		unsigned char unknown_5D0;
		bool HasAttackMoveTarget;

		TeamClass * Team;
		FootClass * Member;

		CellClass * PatrolCell;

		FacingType Path[CONQUER_PATH_MAX];

		CDTimerClass<FrameTimerClass> PathDelay;

		enum {PATH_RETRY=10};
		int TryTryAgain;

		CDTimerClass<FrameTimerClass> BaseAttackTimer;

		CDTimerClass<FrameTimerClass> SightTimer;

		CDTimerClass<FrameTimerClass> BlockagePathDelay;

		ILocomotionPtr Locomotion;

		Coord HeadToCoord;

		char CurrentTube;
		char CurrentTubeDir;

		char NextWaypoint;

		bool IsToScatter;
		bool IsScanLimited;
		bool IsInitiated;
		bool IsNewNavCom;
		bool IsPlanningToLook;
		bool IsDeploying;
		bool IsFiring;

		bool unknown_bool_68E;
		bool ShouldEnterAbsorber;
		bool ShouldEnterOccupiable;
		bool ShouldGarrisonStructure;

		FootClass * ParasiteEatingMe;
		int LastBeParasitedStartFrame;
		ParasiteClass * ParasiteImUsing;
		CDTimerClass<FrameTimerClass> ParalysisTimer;

		bool unknown_bool_6AC;
		bool IsAttackedByLocomotor;
		bool IsLetGoByLocomotor;

		bool IsRotating;
		bool IsUnloading;
		bool IsNavQueueLoop;
		bool IsScattering;
		bool IsIdle;

		char IonBlastYDrawOffset;

		bool IsCrushing;
		bool IsOccupyingCell;
		bool IsToPathAroundBlockage;
		bool IsDroppedFromTeam;

		FootClass(NoInitClass const & x);
		FootClass(HouseClass * house);
		virtual ~FootClass(void) override;

		virtual void const * Get_Image_Data(void) const override;
		virtual bool CanBeSelected(void) const override;
		virtual int IronCurtain(int duration, HouseClass * source, bool force_shield) override;
		virtual int Do_MISSION_EATEN(void) override;
		virtual int Do_MISSION_UNLOAD(void) override;
		virtual Cell GetLastFlightMapCoords(void) const override;
		virtual void SetLastFlightMapCoords(Cell coord) override;
		virtual Cell ScanForTiberium(int range, unsigned unk) override;
		virtual bool EnterGrinder(void) override;
		virtual bool EnterBioReactor(void) override;
		virtual bool EnterTankBunker(void) override;
		virtual bool EnterBattleBunker(void) override;
		virtual bool GarrisonStructure(void) override;
		virtual bool IsParalyzed(void) const override;
		virtual void AddPassenger(FootClass * passenger) override;
		virtual bool Crash(ObjectClass * killer) override;
		virtual Coord GetAttackCoordinates(void) const override;
		virtual void vt_entry_47C(unsigned) override;
		virtual bool ForceCreate(Coord & coord, unsigned unk = 0) override;
		virtual int RespondMegaEventMission(void * event) override;
		virtual void ClearMegaMissionData(void) override;
		virtual bool HaveMegaMission(void) const override;
		virtual bool HaveAttackMoveTarget(void) const override;
		virtual int GetMegaMission(void) const override;
		virtual Coord GetAttackMoveCoords(void) override;
		virtual bool MegaMissionIsAttackMove(void) const override;
		virtual bool ContinueMegaMission(void) override;
		virtual void UpdateAttackMove(void) override;
		virtual bool RefreshMegaMission(void) override;


		virtual void ReceiveGunner(FootClass * gunner);
		virtual void RemoveGunner(FootClass * gunner);
		virtual bool Should_Delete_Off_Map(void);
		virtual bool Is_Considered_Slow(void);
		bool Is_Entering_Structure(void) const;
		virtual bool Deploy_To_Fire(void) const {return(false);};
		virtual void AddSensorsAt(Cell cell);
		virtual void RemoveSensorsAt(Cell cell);
		virtual Coord Likely_Coord(void) const;
		virtual void On_Movement_Blocked(void);
		virtual bool JumpJet_To_Walk(void) {return(false);};
		virtual bool Start_Driver(Coord & headto);
		virtual bool Stop_Driver(void);
		virtual bool Is_Docked_For_Repair(void);
		virtual bool ChronoWarpTo(Coord dest);
		virtual void Draw_Object(ShapeSet const * shapefile, int shapenum, Point2D const & xy, Rect const & rect, Dir256 rotation=DIR_N, int scale=0x0100, int zadjust=0, ZGradientType zgrad=ZGRAD_GROUND, bool=false, int brightness=0, int tint=0, ShapeSet const * zshapefile=0, int zshapenum=0, Point2D zoff=Point2D(0,0), ShapeFlags_Type flags=SHAPE_NORMAL) const;
		virtual void Draw_A_VXL(VoxelDataStruct const & voxeldata, int frame, int key, VoxelIndexClass * cache, Rect const & cliprect, Point2D const & point, Matrix3D const & matrix, int brightness, ShapeFlags_Type flags, int unknown) const;
		virtual void Berzerk(void) {};
		virtual void Start_Fear(void) {};
		virtual void Stop_Fear(void) {};
		virtual void Do_Idle(int second) {};
		virtual int Offload_Tiberium_Bail(void);
		virtual BuildingClass * TryNearestDockBuilding(TypeList<BuildingTypeClass *> * list, unsigned unk2, unsigned unk3) const;
		virtual BuildingClass * FindCloserDockBuilding(BuildingTypeClass * type, unsigned unk2, unsigned unk3, int * distance) const;
		virtual BuildingClass * FindNearestDockBuilding(BuildingTypeClass * type, unsigned unk2, unsigned unk3) const;
		virtual void Overrun_Square(Cell const & cell, bool threaten) {};
		virtual int Current_Speed(void);
		virtual AbstractClass * Approach_Target(bool = false);
		virtual void Fixup_Path(PathStruct *) {};
		virtual void Set_Speed(double speed);
		virtual void Stop_Movement_Animation(void);
		virtual void vt_entry_54C(void);
		virtual bool Is_LZ_Clear(AbstractClass * target) const;


		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		bool Basic_Path(Cell cell, int path_offset = 0, int avoidance = 0);

		virtual void Compute_CRC(CRCEngine &) const override;
		virtual Coord Destination_Coord(TechnoClass * docker = NULL) const override;
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, int & param) override;
		virtual bool Can_Demolish(void) const override;
		bool Is_Recruitable(HouseClass const * house=NULL) const;
		bool Is_On_Priority_Mission(void) const;
		virtual bool Is_Moving_Onto_Bridge(TechnoClass * docker = NULL) const override;
		virtual bool Occupies_Cells(void) const override {return(IsOccupyingCell);};
		virtual void Delete_Me(void) override;

		virtual bool In_Air(void) const override;
		virtual bool Is_In_Same_Zone_As(ObjectClass const * object) const override;
		virtual bool Is_In_Same_Zone(Coord const & coord) const override;


		virtual void Assign_Destination(AbstractClass * target, bool = true) override;
		virtual bool Enter_Idle_Mode(bool initial=false, bool = true) override;
		virtual bool Is_Allowed_To_Leave_Map(void) const override;
		void Link_DropPod(void);
		Cell Move_Order(Cell const & where, bool consider_fog);
		void AbortMotion(void);
		void ResolveMegaTarget(void);
		void UpdatePassengerCoords(void);
		void RemovePassenger(FootClass * passenger);
		FootClass * RemoveFirstPassenger(void);

		virtual VisualType Visual_Character(bool raw = false, HouseClass const * = NULL) const override;
		virtual bool Limbo(void) override;
		virtual bool Unlimbo(Coord const & , Dir256 dir = DIR_N) override;
		virtual LayerType In_Which_Layer(void) const override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual bool Mark(MarkType mark=MARK_CHANGE) override;
		virtual int Get_Z_Adjust(void) const override;
		virtual ZGradientType Get_Z_Gradient(void) const override;
		virtual void Draw_Action_Line(bool force, int unknown) const override;
		void FUN_004DC340(void);
		void Draw_Voxel_Shadow(VoxelDataStruct const & voxeldata, int layer_index, int key, VoxelIndexClass * cache, Rect const & cliprect, Point2D const & point, Matrix3D const & matrix, bool force_cache, Surface * surface, int shadow_x, int shadow_y) const;

		virtual ActionType What_Action(ObjectClass const *, bool disallow_force = false) const override;
		virtual ActionType What_Action(Cell const &, bool check_fog = false, bool disallow_force = false) const override;
		virtual bool Active_Click_With(ActionType action, ObjectClass * object, bool) override;
		virtual bool Active_Click_With(ActionType action, Cell const & cell, Cell const & cell2, bool) override;

		virtual void Stun(void) override;
		virtual ResultType Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source=0, bool forced=false, bool=false, HouseClass * = 0) override;
		virtual void Death_Announcement(TechnoClass const * source=0) const override;
		virtual bool Captured(HouseClass * newowner, bool announce = true) override;

		virtual void AI(void) override;
		virtual void Sell_Back(int control) override;
		virtual void Set_Waypoint_Path(PathType path, char index) override;
		bool Execute_Waypoint_Path(WaypointClass * waypoint);
		virtual AbstractClass * Greatest_Threat(ThreatType threat, Coord const & coord, bool) const override;
		virtual void Detach(AbstractClass const * target, bool all) override;
		virtual void Detach_All(bool all=true) override;
		virtual int Do_MISSION_RETREAT(void) override;
		virtual int Do_MISSION_ENTER(void) override;
		virtual int Do_MISSION_MOVE(void) override;
		virtual int Do_MISSION_CAPTURE(void) override;
		virtual int Do_MISSION_ATTACK(void) override;
		virtual int Do_MISSION_GUARD(void) override;
		virtual int Do_MISSION_HUNT(void) override;
		virtual int Do_MISSION_GUARD_AREA(void) override;
		virtual int Do_MISSION_RESCUE(void) override;
		virtual int Do_MISSION_PATROL(void) override;
		virtual bool Is_Allowed_To_Recloak(void) const override;
		virtual bool Is_In_Team(void) const override {return(Team!=NULL);};
		virtual void Advance_Waypoint_Path(void) override;
		void Remove_From_Team(void) {if (Team != NULL) Team->Remove(this);}
		double Threat_Avoidance_Value(void) const;

#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const override;
#endif

		void Handle_Navigation_List(void);
		void Queue_Navigation_List(AbstractClass * target);
		void Clear_Navigation_List(void);
		virtual void Per_Cell_Process(PCPType why) override;
		virtual MoveType Can_Enter_Cell(CellClass const * cellptr, FacingType dir = FACING_NONE, int cell_height = -1, CellClass const * = 0, bool = true) const override;
		virtual MoveType Can_Reach(CellClass const * current_cell, FacingType facing, int & cell_height, bool & onto_bridge, CellClass const * adjacent_cell) const override;
		virtual void Set_Coord(Coord const & coord) override;
		int Optimize_Moves(PathStruct *path, MoveType threshhold);
		virtual void Override_Mission(MissionType mission, AbstractClass * tarcom, AbstractClass * navcom) override;
		virtual bool Restore_Mission(void) override;
		Cell Adjust_Dest(Cell const & cell) const;
		BuildingClass * Adjacent_Overpowerable_Building(void);


		Cell Safety_Point(Cell const & src, Cell const & dst, int start, int max);
		int Rescue_Mission(AbstractClass * tarcom);
		bool Tiberium_Check(Cell & center);
		bool Goto_Tiberium(int rad, unsigned allow_weighted = false);
		Cell Search_For_Tiberium(int rad, bool allow_weighted = false);
		Cell Search_For_Tiberium_Weighted(int rad);

		bool Weed_Check(Cell & center, int x, int y);
		bool Goto_Weed(int rad);
		Cell Search_For_Weed(int rad);

	private:
		PathStruct * Find_Path(Cell const & dest, FacingType * final_moves, int maxlen, MoveType threshhold, int path_offset, int avoidance);
};

inline void FootClass::Draw_Object(ShapeSet const * shapefile, int shapenum, Point2D const & xy, Rect const & rect, Dir256 rotation, int scale, int zadjust, ZGradientType zgrad, bool zwrite, int brightness, int tint, ShapeSet const * zshapefile, int zshapenum, Point2D zoff, ShapeFlags_Type negflags) const
{
	Techno_Draw_Object(shapefile, shapenum, xy, rect, rotation, scale, zadjust, zgrad, zwrite, brightness, tint, zshapefile, zshapenum, zoff, negflags);
}

inline FootClass * AbstractClass::As_FootClass(void)
{
	return(dynamic_cast<FootClass *>(this));
}

inline FootClass const * AbstractClass::As_FootClass(void) const
{
	return(dynamic_cast<FootClass const *>(this));
}

extern DynamicVectorClass<FootClass *> Feet;
extern AircraftTrackerClass AircraftTracker;
