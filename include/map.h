// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "crate.h"
#include "gscreen.h"
#include "rect.h"
#include "vector.h"

#include "house.hh"
#include "isotype.hh"
#include "mzone.hh"
#include "passblty.hh"
#include "speed.hh"
#include "zone.hh"

class AbstractClass;
class ObjectClass;
class HouseClass;
class Straw;
class Pipe;
class CellClass;
class BuildingTypeClass;
class FootClass;
class TechnoClass;
class WarheadTypeClass;
template<typename K, typename V>
class HashTableClass;

class MapClass: public GScreenClass
{
		typedef GScreenClass BASECLASS;

	public:

		MapClass(void);
		MapClass(NoInitClass const & x);
		virtual ~MapClass(void) override;

		Point2D PlayRect_To_Cell_Point(Point2D const & point) const;
		Point2D LocalRect_To_Cell_Point(Point2D const & point) const;
		Point2D Cell_To_PlayRect_Point(Point2D const & point) const;
		Point2D Cell_To_LocalRect_Point(Point2D const & point) const;
		Cell PlayRect_To_Cell(Cell const & cell);
		Cell LocalRect_To_Cell(Cell const & cell);
		Cell Cell_To_PlayRect(Cell const & cell);
		Cell Cell_To_LocalRect(Cell const & cell);

		virtual LONG STDMETHODCALLTYPE Is_Visible(CellStruct cell) override;
		CellClass & operator [] (Coord const & coord) const;
		CellClass & operator [] (Cell const & cell) const;
		int ID(CellClass * ptr) {return(Array.ID(ptr));};
		int ID(CellClass & ptr) {return(Array.ID(&ptr));};
		bool Is_Valid(Cell const & cell);

		virtual void One_Time(void) override;
		virtual void Init_Clear(void) override;
		virtual void Alloc_Cells(void);
		virtual void Free_Cells(void);
		virtual void Init_Cells(void);

		void Fresh_Map(void);
		void sub_657CE0(void);
		static int DamageArea(Coord const & coord, int damage, TechnoClass * source, WarheadTypeClass const * warhead, bool affect_tiberium, HouseClass * house);
		static void FlashbangWarheadAt(int damage, WarheadTypeClass const * warhead, Coord coord, bool force = false, int flags = 0);
		void Sight_From(Coord const & coord, int sightrange, HouseClass * house, bool incremental=false, bool=false, bool=false, bool=true);
		void RevealArea0(int, int, Coord * coord, int sightrange, HouseClass * house, bool, bool, bool, bool);
		void RevealArea1(Coord * coord, int sightrange, HouseClass * house, bool, bool, bool, bool, bool);
		void RevealArea2(Coord * coord, int sightrange, HouseClass * house, bool, bool, bool, bool, bool);
		void Shroud_Gap_Area(Coord const & coord, int radius);
		void Unshroud_Gap_Area(Coord const & coord, int radius);
		void RevealArea3(Coord const & coord, int height, int radius, int skip_reveal);
		bool In_Radar(Cell const & cell) const;
		bool In_Radar(Coord const & coord) const;
		void Place_Down(Cell const & cell, ObjectClass * object);
		void Pick_Up(Cell const & cell, ObjectClass * object);
		void AddContentAt(Cell const * coords, TechnoClass * content);
		void RemoveContentAt(Cell const * coords, TechnoClass * content);
		int Overpass(bool flag = false);
		ObjectClass * Close_Object(Coord const & coord) const;
		Cell Nearby_Location(Cell const & cell, SpeedType speed, int zone=-1, MZoneType check=MZONE_NORMAL, bool checkbridge=false, Point2D dimensions=Point2D(1,1), bool checkoverlay=false, bool checkheight=false, bool checkburrow=false, bool allowunderbridge=true, Cell const & nearto=Cell(0, 0), bool flag=false, bool buildable=false) const;
		bool Base_Region(Cell const & cell, HousesType & house, ZoneType & zone) const;
		int Cell_Region(Cell const & cell);
		Cell Region_To_Cell(int region) const;
		int FUN_00588990(void);
		int GetThreatPosed(Cell const & cell, HouseClass * house) const;
		int Cell_Threat(Cell const & cell, HouseClass const & house);
		Cell Pick_Random_Location(void) const;
		bool Place_Random_Crate(void);
		bool PlacePowerupCrate(Cell cell, int powerup);
		bool Remove_Crate(Cell const & cell);
		void Shroud_The_Map(HouseClass * house);
		void FUN_00577C90(HouseClass * house);
		void FUN_00578100(void);
		void Reshroud(HouseClass * house);
		void Reveal(HouseClass * house);
		void Reveal_The_Map(HouseClass * house);
		virtual void Detach(AbstractClass const * , bool all = true);

		bool Is_Same_Cell_Zone(Cell const & from, Cell const & to, MZoneType mzone=MZONE_NORMAL, bool isbridge1=false, bool isbridge2=false, bool leavemap=false);
		int Get_Cell_Zone(Cell const & cell, MZoneType mzone=MZONE_NORMAL, bool bridge=false);
		int Get_Cell_Zone_ID(Cell const & cell);
		int Get_Cell_Zone_Index(Cell const & cell);
		int Get_Cell_Subzone_Index(Cell const & cell);
		void Update_Cell_Zone(Cell const & cell);
		void Update_Cell_Zone_Constructively(Cell const & cell);
		void ResetZones(Cell const & cell);
		void RecalculateSubZones(Cell const & cell);
		int Zone_Reset(void);
		int Zone_Span(CellZoneStruct * data, int zone, int & skip);
		void Compute_Zone_Connections(void);
		int Zone_Connection_Index(Cell const & cell, int maxdist, int index = 0);
		bool Register_Subzone_Connections(Cell const & cell);
		bool Unregister_Subzone_Connections(Cell const & cell);

		void Reset_All_Subzones(void);
		void Reset_Subzone(int subzone);
		int Subzone_Span(CellSubzoneStruct * seed, int subzone_level, int subzone_id, Rect const & bounds, Cell const & cell);
		void Register_Subzone_Zone_Connections(int subzone);
		void Register_Zone_Connection_Entries(ZoneConnectionClass & connection, int index);
		Cell Get_Bridge_Zone_Connection_Cell(CellClass * cptr, bool isbridge);
		Cell Get_Zone_Connection_Destination(Cell const & cell, Cell const & reference);
		Cell Find_Bridge_Span_End_Cell(Cell const & cell, Cell const & reference);
		Cell Find_Bridge_End_Cell_For_Subzone(Cell const & cell, int subzone_level, int subzone_id);
		bool Build_Reachable_Subzones(CellClass * cptr, int subzone_level, DynamicVectorClass<unsigned short> const & connections, FootClass const * foot);
		void Update_Cell_Subzones(Cell const & cell);
		void Register_Subzone_Connection(ZoneConnectionClass * connection);
		void Unregister_Subzone_Connection(ZoneConnectionClass * connection);

		bool Damage_Bridge(Cell const & cell);
		bool Damage_Low_Bridge(Cell const & cell);
		void Repair_Train_Bridge(Cell const & cell);
		void Repair_Bridge(Cell const & cell);
		bool Can_Repair_Bridge(Cell const & cell);
		void DestroyConcreteBridgeAt(Cell const & cell);
		void DestroyWoodBridgeAt(Cell const & cell);
		void FUN_00574780(Cell const & cell);
		void FUN_005749C0(Cell const & cell);
		void FUN_00575220(Cell const & cell);
		void FUN_00575540(Cell const & cell);
		void FUN_00575870(Cell const & cell);
		void FUN_00575BA0(Cell const & cell);
		bool FUN_00587180(Cell const & cell);
		bool FUN_0057BAA0(Cell const & cell);
		bool FUN_0057CCF0(Cell const & cell);
		void FUN_00576770(Cell const & cell);
		void Update_Pathfinding_1(void);

	private:

		void Set_Bridge_End_State(Cell const & cell, bool damaged, bool recursive);
		void Set_Bridge_Middle_State(Cell const & cell, IsometricTileType new_tile, IsometricTileType match_tile, int cell_height, bool recursive);

		void Internal_Damage_Train_Bridge_EW_BottomRight(Cell const & cell, FacingType dir);
		void Internal_Damage_Train_Bridge_EW_TopLeft(Cell const & cell, FacingType dir);
		void Internal_Destroy_Train_Bridge_EW_BottomRight(Cell const & cell, FacingType dir);
		void Internal_Destroy_Train_Bridge_EW_TopLeft(Cell const & cell, FacingType dir);
		void Internal_Damage_Train_Bridge_NS_BottomLeft(Cell const & cell, FacingType dir);
		void Internal_Damage_Train_Bridge_NS_TopRight(Cell const & cell, FacingType dir);
		void Internal_Destroy_Train_Bridge_NS_BottomLeft(Cell const & cell, FacingType dir);
		void Internal_Destroy_Train_Bridge_NS_TopRight(Cell const & cell, FacingType dir);
		void Internal_Damage_High_Bridge_EW_BottomRight(Cell const & cell, FacingType dir);
		void Internal_Damage_High_Bridge_EW_TopLeft(Cell const & cell, FacingType dir);
		void Internal_Destroy_High_Bridge_EW_BottomRight(Cell const & cell, FacingType dir);
		void Internal_Destroy_High_Bridge_EW_TopLeft(Cell const & cell, FacingType dir);
		void Internal_Damage_High_Bridge_NS_BottomLeft(Cell const & cell, FacingType dir);
		void Internal_Damage_High_Bridge_NS_TopRight(Cell const & cell, FacingType dir);
		void Internal_Destroy_High_Bridge_NS_BottomLeft(Cell const & cell, FacingType dir);
		void Internal_Destroy_High_Bridge_NS_TopRight(Cell const & cell, FacingType dir);

		bool Damage_Train_Bridge(Cell const & cell);
		bool Destroy_Train_Bridge_Span(Cell const & cell, FacingType dir, Rect * dirty);
		void Destroy_Train_Bridge_Connections(Cell const & cell);
		bool Damage_High_Bridge(Cell const & cell);
		bool Destroy_High_Bridge_Span(Cell const & cell, FacingType dir, Rect * dirty);
		void Destroy_High_Bridge_Connections(Cell const & cell);
		void Spring_Bridge_Destruction_Triggers(Cell cell1, Cell cell2);

		void Repair_High_Bridge_Span(CellClass * cptr, FacingType dir, Rect * dirty = NULL);
		void Repair_Train_Bridge_Span(CellClass * cptr, FacingType dir, Rect * dirty);
		void Repair_All_High_Bridges(void);
		void Repair_High_Bridge_From_Cell(Cell const & cell);
		void Repair_Train_Bridge_From_Cell(Cell const & cell);

		static int Get_Low_Bridge_EW_Neighbor_Mask(Cell const & cell);
		static int Get_Low_Bridge_NS_Neighbor_Mask(Cell const & cell);
		bool Damage_Low_Bridge_EW(Cell const & cell);
		bool Damage_Low_Bridge_NS(Cell const & cell);
		void Damage_Low_Bridge_Piece_EW(Cell const & cell);
		void Damage_Low_Bridge_Piece_NS(Cell const & cell);
		void Spring_Low_Bridge_Destroyed_EW(Cell cell);
		void Spring_Low_Bridge_Destroyed_NS(Cell cell);
		void Repair_Low_Bridge_Span(Cell const & cell);
		void Repair_Low_Bridge_EW(Cell const & cell);
		void Repair_Low_Bridge_NS(Cell const & cell);
		static bool Is_Low_Bridge(Cell const & cell);

	public:

		void Smoothen_Ice(Cell const & cell, bool smooth_shore);
		void Smoothen_Full_Ice(Cell const & cell, bool not_cracked);
		void Smoothen_Ice_Shore(Cell const & cell);
		void Smoothen_Ice_Edge(Cell const & cell);
		bool Crack_Ice(CellClass * cellptr, FootClass * object);
		bool Break_Ice(CellClass * cellptr, FootClass * object);
		static bool Is_Tile_Water_Ice(IsometricTileType tile);
		bool Ice_Growth_AI(void);
		void Ice_Solidification_AI(void);
		void Recalc_Ice_Cells(void);

		void Mark_Fill_Area(Cell const & origin, bool on_water);
		bool Rebuild_Cliffs_At(Cell const & cell);
		bool Build_All_Cliffs(int, int region_id);
		bool FUN_0057A0C0(int, int);
		bool Expand_High_Ground(CellClass * cellptr, int region_id);
		bool Place_Cliff(CellClass * cellptr, int region_id);
		int Get_High_Ground_Mask(CellClass * cellptr);
		bool Rebuild_Shores_At(Cell const & origin);
		bool Build_All_Shores(int region_id);
		bool Expand_Water(CellClass * cellptr, int region_id);
		void Prune_Water(CellClass * cellptr);
		bool Place_Shore(CellClass * cellptr, int pass, int region_id);
		int Get_Water_Mask(CellClass * cellptr, int control);
		static int Straight_Shore_Length(CellClass * cellptr, FacingType direction);
		bool Pick_Random_Tile_Variant(IsometricTileType tile, int max_tile, int base_height, int seed, bool & success, bool flat_height);
		static bool Is_Tile_Shore(IsometricTileType tile);
		static bool Is_Tile_Cliff(IsometricTileType tile, int subtile);

		bool Deform_Terrain(Cell const & cell, bool forced);
		void Terrain_Deformation_AI(void);
		void Collapse_Cliff(CellClass * cptr);
		void Area_Reduce_Tiberium(Cell const & cell);
		void Recalc_Cells_In_Rect(Rect const & rect);
		void Recalc_Cells_In_List(DynamicVectorClass<Cell> const & vec);
		void Set_WasUnderBridge_Flags(void);

		CellClass * Iterate(void);
		void Reset_Iterator(void);
		CellClass * Local_Iterate(void);
		void Reset_Local_Iterator(void);
		CellClass * Get_Local_Grid_Cell(Cell const & cell, int spacing, bool unbiased);
		bool In_Local_Radar(Rect const & rect, bool useheight = true) const;
		bool In_Local_Radar(Cell const & cell, bool useheight = true) const;
		bool In_Local_Radar(CellClass const * cell, bool useheight = true) const;
		bool In_Local_Radar(Coord const & rect) const;
		bool In_Area_Radar(Rect const & rect, Cell const & cell, bool useheight = true) const;
		int Cell_To_Local_Sector_Index(Cell const & cell);
		Cell Local_Sector_Index_To_Cell(int index);

		bool Is_Shrouded(Coord const & coord);
		bool Is_Fogged(Coord const & coord);
		void Init_Fog_System(void);
		void Deinit_Fog_System(void);
		void Reveal_Nearby_Technos(CellClass * cptr, HouseClass * house, bool onradar);
		CellClass * Find_Nearby_Shroud(FootClass * foot);

		int Get_Height_GL(Coord const & coord);
		void Increment_Redraw_Counter(void);
		bool Try_Open_Gate(FootClass * foot, Cell const & cell);
		bool Is_Something_Nearby(Cell const & cell, int radius);
		int Region_Threat(HouseClass * house, int level, int from_subzone, int to_subzone);
		bool Is_Area_Available(Rect const & rect, int house);
		bool Is_Clear_To_Move(Cell const & cell, int width, int height, SpeedType speed, int zone, MZoneType check, int cell_height, bool checkbridge, bool block_overlays);
		Cell Closest_Edge_Cell(Cell const & cell, bool inset = false);
		Cell Clip_To_Map(Cell const & cell);
		Cell Closest_Passable_Cell(Cell const & tarcell, Cell const & objcell);
		bool Check_Map_Integrity(void);
		void Initialize_Wall_Ownership(void);
		Coord Firestorm_On_Path(Coord const & from, Coord const & to, HouseClass * house);
		void Place_Firestorm_Wall(Cell const & cell, HouseClass * owner, BuildingTypeClass * type);
		void Place_Wall(Cell const & cell, HouseClass * owner, BuildingTypeClass * type);
		void Shutdown(void);

		int Write_Binary_1(Pipe & pipe);
		int Write_Binary_2(Pipe & pipe);
		int Write_Binary_3(Pipe & pipe);
		int Write_Binary_4(Pipe & pipe);
		int Write_Binary_5(Pipe & pipe);
		bool Read_Binary_1(Straw & straw);
		bool Read_Binary_2(Straw & straw);
		bool Read_Binary_3(Straw & straw);
		bool Read_Binary_4(Straw & straw);
		bool Read_Binary_5(Straw & straw);

		int Validate(void);

		virtual bool Is_Scrolling(void) const {return(false);};
		virtual void Logic(void);
		virtual void Set_Map_Dimensions(Rect const & size, bool, int cell_height, bool);
		virtual void Set_Local_Dimensions(Rect const & size);

		bool unknown_10;

		ZONE_PAIR_HASH_SET * ZoneAdjacency;

		unsigned short * Zones[MZONE_COUNT];

		int ZoneCount;

		DynamicVectorClass<ZoneConnectionClass> ZoneConnections;

		CellZoneStruct * CellZones;
		int CellZoneCount;
		CellSubzoneStruct * CellSubzones;

		int SubzoneTrackingEntryCount[SUBZONE_COUNT];
		SUBZONE_CONNECTION_HASH_SET * SubzoneConnectionHashTable[SUBZONE_COUNT];
		DynamicVectorClass<SubzoneTrackingStruct> SubzoneTracking[SUBZONE_COUNT];

		DynamicVectorClass<Cell> PendingBridgeCells;

		Rect PlayRect;

		Rect LocalRect;

		unsigned IterX;
		unsigned IterY;
		unsigned IterColumn;
		CellClass ** IterCell;
		int LocalIterX;
		int LocalIterY;

		Rect MapRect;

		int TotalValue;

		VectorClass<CellClass *> Array;

		// Lowest cell level on the map, or 13 before it is computed.
		int MaxLevel;

	protected:

		static int const RadiusCount[11];
		static Cell const RadiusOffset[];
		static Cell const OcclusionOffset[];

		int	XSize;
		int	YSize;
		int	Size;

		CrateClass Crates[256];

	protected:
		friend class CellClass;

		unsigned char Redraws;

		DynamicVectorClass<Cell> TaggedCells;
};

extern CellClass BlubCell;
extern CellClass InvalidCell;

int SubzoneHash(unsigned int const & key);
extern int MZonePassability[MZONE_COUNT][PASSABLE_COUNT];
