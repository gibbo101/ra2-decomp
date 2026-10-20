// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "loaddlg.h"
#include "typelist.h"

#include "isotype.hh"

class BuildingTypeClass;
class CellClass;
class MapPreviewClass;
struct CellNode;
template<typename T> class PriorityQueueClass;

#define RANDOM_MAP_FILE_NAME "RandMap.Sed"

enum BiomeType {
	BIOME_TUNDRA,
	BIOME_TAIGA,
	BIOME_TEMPERATE,
	BIOME_DESERT,
	BIOME_MUTATED,
	BIOME_COUNT,

	BIOME_FIRST = 0,
};

enum TimeOfDayType {
	TIME_OF_DAY_MORNING,
	TIME_OF_DAY_AFTERNOON,
	TIME_OF_DAY_DUSK,
	TIME_OF_DAY_NIGHT,
	TIME_OF_DAY_COUNT,

	TIME_OF_DAY_FIRST = 0,
};

enum MapSizeType {
	MAPSIZE_SMALL,
	MAPSIZE_MEDIUM,
	MAPSIZE_LARGE,
	MAPSIZE_VERY_LARGE,
	MAPSIZE_COUNT,

	MAPSIZE_FIRST = 0,
};

class MapRegionClass
{
public:
	struct CellData {

		Cell CellID;

		double HillHeight;

		double HillVolatility;

		double GreenTileChance;
		double SandTileChance;
		double RoughTileChance;
		double ForestChance;

		int RegionID;

		int SpreadID;

		int WaterMask;

		bool Interior;

		bool Inviolate;

		bool TilePlaced;
		bool Forested;

		bool Iced;

		bool InFillArea;

		bool InFillReach;

		bool CanExpand;

		CellData(void);

		static bool Is_Not_Inviolate(Cell const &cell);
		static bool Is_Interior(Cell const &cell);
		static void Set_Inviolate(Cell const &cell);
		static void Clear_Inviolate(Cell const &cell);
		static void Clear_Inviolates(void);
		static void Set_Region(Cell const &cell, int region_id);
		static int Get_Region(Cell const &cell);
		static void Set_Spread(Cell const &cell, int patch_id);
		static int Get_Spread(Cell const &cell);
		static void Reset_Spreads(bool);
	};

	public:

		MapRegionClass(Cell cell);
		~MapRegionClass(void);

		static MapRegionClass::CellData &Get_Cell_Data(Cell const &cell);
		static MapRegionClass::CellData &Get_Cell_Data(int index);

		static MapRegionClass *Create_Region_From_Cell(CellClass *cptr);
		static void Create_Water_Regions(void);
		static void Create_Land_Regions(bool unsplittable);
		static bool Make_Regions(void);
		static void Make_Ice_Regions(void);
		static void Destroy_Cell_Regions(void);
		static void Delete_All_Regions(void);
		static MapRegionClass *From_ID(int id);

		static Cell Pick_Random_Map_Cell(void);
		static Cell Pick_Random_Clear_Map_Cell(void);
		static Cell Pick_Random_Clear_Cell_In_Zone(int zone, int region_id);
		Cell Pick_Random_Cell(void);
		Cell Pick_Random_Clear_Cell(void);
		Cell Pick_Random_Clear_Interior_Cell(void);

		void Recount_Cells(void);
		void Clear_Cells(int new_id, int cell_height);
		DynamicVectorClass<Cell> *Build_Border_Cell_List(void);
		DynamicVectorClass<Cell> *Build_Border_Cell_List2(void);
		void Reassign_Border_Cells(int rings, unsigned int region_id);
		bool Grow(int rings);
		bool Claim_Expandable_Cells(void);
		bool Split_Region(void);
		void Increase_Height(char cell_height);
		void Mark_Interior_Cells(void);
		double Get_Angle_Score(const Cell & cell1, const Cell & cell2, double ref_angle);
		DynamicVectorClass<Cell> *Pick_Spread_Cells(DynamicVectorClass<Cell> &cells);
		void Pick_Start_Points(int first_waypoint);
		void Place_Neutral_Building(void);

		void Build_Neighbor_Regions(void);
		void Make_Region_Connections(void);
		unsigned Build_Neighbor_Region_Mask(Cell const & cell, bool & valid, int unknown, float threshold);
		bool Connect_Regions_With_Bridge(MapRegionClass * region1, MapRegionClass * region2);
		bool Is_Bridge_Allowed(Rect const & rect) const;
		bool Place_Bridge_Hut(Rect const & rect) const;
		bool Connect_Regions_With_Ramp(Cell const & cell, int region_id, float threshold);
		bool Build_South_Ramp(Cell const & cell1, Cell const & cell2, int region_id);
		bool Build_East_Ramp(Cell const & cell1, Cell const & cell2, int region_id);
		bool Build_North_Ramp(Cell const & cell1, Cell const & cell2, int region_id);
		bool Build_West_Ramp(Cell const & cell1, Cell const & cell2, int region_id);
		bool Build_Corner_Ramp_SE(Cell const & cell1, Cell const & cell2, int region_id);
		bool Build_Corner_Ramp_NE(Cell const & cell1, Cell const & cell2, int region_id);
		bool Build_Corner_Ramp_NW(Cell const & cell1, Cell const & cell2, int region_id);
		bool Build_Corner_Ramp_SW(Cell const & cell1, Cell const & cell2, int region_id);
		bool Is_Ramp_Area_Clear(Rect & rect, int region_id);
		void Flatten_Area(Rect const & rect, int region_id);
		void Flatten_Area(Rect const & rect);

	public:

		DynamicVectorClass<Cell> *Unknown0;

		DynamicVectorClass<int> *NeighborRegions;

		int ID;

		int CellCount;

		int CellHeight;

		bool ContainsWater;

		Cell Position;

		bool Unsplittable;

		bool HasConnections;

		int SplitCount;

		int Unknown20;

		bool Discardable;

		DynamicVectorClass<Cell> Cells;

		Rect RegionBounds;

		static DynamicVectorClass<MapRegionClass *> MapRegions;

		static int TotalCount;

		static int MapStartDiagonal;
		static int MapEndDiagonal;
};

class MapSeedClass : public LoadOptionsClass
{
		typedef LoadOptionsClass BASECLASS;

	public:
		MapSeedClass(void);
		void Reload_Description(void);

		bool Save(const char * name);
		bool Load(const char * name);
		bool Delete(void);

		virtual bool Load_File(const char * file_name) override;
		virtual bool Save_File(const char * file_name, const wchar_t * descr) override;
		virtual bool Delete_File(const char * file_name) override;
		virtual bool Read_File(FileEntryClass * entry, WIN32_FIND_DATAA * ff) override;
		virtual wchar_t const * GetUIString_Load(void) const override;
		virtual wchar_t const * GetUIString_Save(void) const override;
		virtual wchar_t const * GetUIString_Delete(void) const override;
		virtual wchar_t const * GetUIString_GameSaved(void) const override;

		void Get_Settings(HWND dialog);
		void Set_Settings(HWND dialog);
		void Set_Scroll_Bar(HWND handle, unsigned int min, unsigned int max, int position, bool enable);
		void Set_Checkbox(HWND handle, bool state, bool enable);

		void Randomize(int map_type);
		void Randomize(void);
		void Fixup_Settings(void);
		void Fixup_WDT_Settings(void);
		void Clamp_Setting(int & setting, int min, int max, bool use_midpoint) const;

	public:

		int Biome;

		int MapType;

		int Resources;

		int Hills;

		int Time;

		int WaterAmount;

		int NumPlayers;

		int Tiberium;

		int TiberiumLayout;

		int Vegetation;

		int Cities;

		int Width;
		int Height;

		int Accessibility;

		int Cliffs;

		int Seed;

		wchar_t MapDescription[128];
};

class MapGeneratorClass
{
	public:
		MapGeneratorClass(void);
		~MapGeneratorClass(void);

		void Generate_Random_Map(bool full_init, HWND dialog);
		void Init_Map(bool full_init);
		void Cleanup(void);
		void Update_Progress(int percent_progress);
		void Seed_Land(void);
		int FUN_0059BBC0(int size, Rect const & bounds, Point2D const & seed, bool flag, Cell const & center, double ratio, bool flag2);
		double FUN_0059B940(Cell const & from, Cell const & to, Rect const & rect, Cell const & center, float weight_x, float weight_y);
		bool FUN_0059BAB0(Cell const & cell, Rect const & rect, bool flag, double scale_x, double scale_y);
		void Partition_Rect(DynamicVectorClass<Rect> & rects, int count, Rect const & area);
		static void Green_Shores(void);
		bool Green_Region_Tiles(int region_id);
		void Vary_Water_Tiles(void);
		static void Clear_Shore_Tiles(void);
		static void Scatter_Green_Tiles(void);
		void Seed_Land_Type0(void);
		void Seed_Land_Type1(void);
		void Seed_Land_Type2(void);

		void Set_Cell_Data_Region(Cell const &cell, int region_id);
		int Get_Cell_Data_Region(Cell const &cell);
		void Set_Cell_Data_Spread(Cell const &cell, int patch_id);
		int Get_Cell_Data_Spread(Cell const &cell);
		void Clear_Cell_Data_Spreads(void);

		void Load_Settings(void);
		void Init_Regions(void);
		void Make_Regions(void);
		void Fixup_Cliff_Corners(void);
		void Fixup_Cliff_Edges(void);
		bool Expand_All_High_Ground(void);
		bool Grow_Region(int id, int rings, Rect rect, char set_height, char height);
		bool Clear_Region_Border(int id, int rings, int new_id);
		void Clear_Region_Cells(int id, int new_id, int cell_height, IsometricTileType ittype);
		DynamicVectorClass<Cell> *Build_Region_Border_Cell_List(int id);
		static bool Region_Cell_Count_At_Least(Rect const & rect, int region_id, int, int count);
		void Set_Height_Of_ID(int id, char cell_height);
		void Increase_Height_Of_ID(int id, char cell_height);
		void Increase_Height_Except_ID(int cell_height, int id);

		void Seed_Water(void);
		bool Seed_Lake(const Cell & cell);
		bool Seed_Arctic_Lake(const Cell & cell);
		bool Seed_River(const Cell & cell, double start_angle, bool is_branch);
		bool Seed_Arctic_River(const Cell & cell, double start_angle);
		bool Grow_Water_Region(int region_id, float spread_scale, Rect const & bounds, Cell const & origin, bool claim_frontier);
		bool Place_Waterfall(int region_id, Cell const & cell1, Cell const & cell2, int direction, bool & placed, double & head_x, double & head_y);
		int Get_Target_Water_Amount(void);
		void Generate_Swamp(DynamicVectorClass<Cell> &cells, int last, CellNode *nodes, PriorityQueueClass<CellNode> *queue);
		void Seed_Ice(DynamicVectorClass<Cell> &cells, IsometricTileType last);
		void Smooth_Ice(void);

		void Seed_Hill_Anchors(void);
		void Seed_Hill_Heights(void);
		void Generate_Hills(void);

		void Seed_Vegetation(void);
		void Seed_Arctic_Vegetation(void);
		void Generate_Vegetation(void);
		void Generate_Arctic_Vegetation(void);
		int Place_Forest(CellClass const * cellptr, int count, double density);
		void Place_Tile_Patch(CellClass * cellptr, IsometricTileType ittype, int count, int origin_id, bool on_pavement);
		void Generate_Mold(void);
		void Generate_Crystals(const Cell & cell);

		void Create_Tiberium(void);
		void Create_Tiberium_Patch(Cell const &cell, int count, int patch_id, bool flag);

		void Generate_Urban_Areas(void);
		DynamicVectorClass<Cell> * Create_Urban_Area(Cell const & cell, int size);
		void Generate_Urban_Roads(const DynamicVectorClass<Cell> & cells);
		void Generate_Urban_Buildings(const DynamicVectorClass<Cell> & cells);
		void Generate_Urban_Units(const DynamicVectorClass<Cell> & cells);
		void Generate_Urban_Pavement(DynamicVectorClass<Cell> & cells);
		void Generate_Rural_Areas(void);
		void Generate_Rural_Buildings(const Cell & cell);
		void Generate_Rural_Units(const Cell & cell);
		bool Generate_Rural_Roads(const Cell & cell);
		void Generate_Lights(void);
		void Generate_Neutral_Buildings(void);
		void Generate_Veinholes(void);

		Cell Plan_Paved_Road_Junction(Cell const & cell, Rect const & rect, FacingType dir);
		Cell Plan_Paved_Road(Cell const & cell, Rect const & rect, FacingType dir);
		static bool Can_Place_Paved_Road(Rect const & rect, bool overlap_road, bool overlap_road_end);
		static bool Can_Place_Paved_Road_End(Rect const & rect, bool overlap_road);
		static bool Can_Place_Paved_Road_Junction(Rect & rect);
		static bool Can_Place_Misc_Pavement(Rect const & rect);
		static bool Find_Dirt_Road_Tile(FacingType facing, Cell const & target_cell, int max_connections, IsometricTileType & out_ittype, Cell & out_origin, int & out_link_index);
		static void Place_Tile(IsometricTileType ittype, const Cell & cell, int region = -1, int height = -1);
		static bool Can_Place_Tile(IsometricTileType ittype, Cell const & cell, bool collision, bool in_map);
		static void Remove_Tile(IsometricTileType ittype, Cell const & cell);
		static void Mark_Tile(IsometricTileType ittype, Cell const & cell, bool placed);
		static bool Is_Area_Cliff_Free(const Cell & cell);
		static bool Is_Cell_Area_Clear(Rect const & rect, bool check_data);
		static Rect Get_Cell_Bounding_Rect(const DynamicVectorClass<Cell> & cells);

		IsometricTileType Pick_Alternate_Cliff(IsometricTileType type);

		double Get_Angle_Score(const Cell & cell1, const Cell & cell2, double ref_angle);
		double Get_Ice_Score(const Cell & cell1, const Cell & cell2, double random_range) const;
		double Get_Spread_Score(const Cell & cell1, const Cell & cell2, int spread_step) const;
		double Get_Tiberium_Score(const Cell & cell1, const Cell & cell2) const;
		double Get_Urban_Score(const Cell & cell1, const Cell & cell2) const;
		double Get_Forest_Score(const Cell & cell1, const Cell & cell2) const;
		double Get_Tile_Patch_Score(const Cell & cell1, const Cell & cell2) const;

		bool Init_Start_Points(void);

	public:

		MapSeedClass SeedData;

		MapSeedClass * MapSeeder;

		MapPreviewClass * MapPreview;

		int LocalWidth;
		int LocalHeight;

		TypeList<int> RMGLevelLightSettings;
		TypeList<int> TemperateAmbientLight;
		TypeList<int> SnowAmbientLight;
		TypeList<int> TemperateAmbientRed;
		TypeList<int> TemperateAmbientGreen;
		TypeList<int> TemperateAmbientBlue;
		TypeList<int> SnowAmbientRed;
		TypeList<int> SnowAmbientGreen;
		TypeList<int> SnowAmbientBlue;
		TypeList<int> RMGVegetationMinimums;
		TypeList<int> RMGVegetationMaximums;

		int RMGMinimumTiberium;
		int RMGMaximumTiberium;

		TypeList<BuildingTypeClass *> TemperateOrePatchLamps;
		TypeList<BuildingTypeClass *> SnowOrePatchLamps;

		int MaxTrees;

		bool PreviewStale;

		int SeededWaterAmount;

		int WorkingRegionID;

		int CellHeight;

		bool PlaceWaterfall;
};

extern MapGeneratorClass RandomMapGen;

inline bool My_In_Radar(Cell const &cell)
{
	int x = cell.X;
	int y = cell.Y;
	return (y + x > MapRegionClass::MapStartDiagonal &&
			x - y < MapRegionClass::MapStartDiagonal &&
			y - x < MapRegionClass::MapStartDiagonal &&
			y + x <= MapRegionClass::MapEndDiagonal) ? true : false;
}

void Create_Zone_Region(CellClass *cptr, MZoneType mzone, int zone);

void Do_Random_Map(HWND, bool (*callback)());
int Do_Random_Map_Dialog(bool (*callback)());

extern MapRegionClass::CellData *RMGCellData;
extern unsigned int MapCellStride;
