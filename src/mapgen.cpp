// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG

#include "always.h"
#include "mapgen.h"
#include "_map.h"
#include "_palette.h"
#include "_rules.h"
#include "_tactica.h"
#include "animtype.h"
#include "astar.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "conquer.h"
#include "coord.h"
#include "data.h"
#include "dbgprint.h"
#include "fastmath.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "index.h"
#include "init.h"
#include "inline.h"
#include "isotype.h"
#include "language\language.h"
#include "netshare.h"
#include "nodes.h"
#include "overtype.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "progress.h"
#include "queue.h"
#include "rules.h"
#include "smartdeform.h"
#include "smudtype.h"
#include "stringtable.h"
#include "tactical.h"
#include "tagtype.h"
#include "terrain.h"
#include "terrtype.h"
#include "tiberium.h"
#include "trigtype.h"
#include "unit.h"
#include "unittype.h"
#include "vector.h"
#include "vein.h"
#include "wdtnet.h"
#include "winfix.h"
#include "worlddom.h"
#include "ramp.hh"

double Random_Fraction(void);

unsigned int Pick_Random_UInt(unsigned int minval, unsigned int maxval);

double Sample_Normal(double mean, double standard_dev);

extern char const * DAT_0082B180[7];

MapGeneratorClass RandomMapGen;

Random2Class RMGRandom;

#define RMG_RANDOM_DOUBLE(max_value) ((unsigned int)RMGRandom() * (double)(max_value)) * (1.0 / UINT_MAX)
#define RMG_RANDOM_DOUBLE2(max_value) ((unsigned int)RMGRandom() * (1.0 / UINT_MAX)) * (double)(max_value)

struct DirtRoadLink {
	DirtRoadLink(void);
	DirtRoadLink(Cell const & cell, FacingType dir);
	DirtRoadLink(DirtRoadLink const & that);

	Cell Offset;
	FacingType Facing;
};

struct DirtRoadTile {
	DirtRoadTile(DirtRoadLink const & link1);
	DirtRoadTile(DirtRoadLink const & link1, DirtRoadLink const & link2);
	DirtRoadTile(DirtRoadLink const & link1, DirtRoadLink const & link2, DirtRoadLink const & link3);
	DirtRoadTile(DirtRoadLink const & link1, DirtRoadLink const & link2, DirtRoadLink const & link3, DirtRoadLink const & link4);
	DirtRoadTile(void);
	~DirtRoadTile(void);

	int Count;
	DirtRoadLink * Links;
	IsometricTileType ITType;
};

struct DirtRoadNode {
	DirtRoadNode(void);
	DirtRoadNode(IsometricTileType road_tile, Cell const & origin, IsometricTileType cap_tile, Cell const & cap_origin, int link_index);
	DirtRoadNode(DirtRoadNode const & that);

	IsometricTileType Tile;
	Cell Origin;
	IsometricTileType CapTile;
	Cell CapOrigin;
	int BackLinkIndex;
};

extern DirtRoadTile DirtRoadTiles[];

class NormalDistribution
{
	public:
		NormalDistribution(double (*function)(void));
		double operator() (void);

		bool HasSpare;
		double Spare;
		double (*Rand)(void);
};

NormalDistribution RMGGaussian(Random_Fraction);

namespace UI {
	HWND __fastcall BeginDialog(int resource, DLGPROC proc, int param);
	void __fastcall FocusOnWindow(HWND window);
	void __fastcall EndDialog(HWND window);
	bool Updated(void);
}
extern bool (*RMGCallback)();
BOOL CALLBACK Map_Seed_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
class RefPtrBase
{
	public:
		RefPtrBase(void);
		virtual ~RefPtrBase(void);
		void Set(void * ptr);

		void * Ptr;
};

class TerritoryRef : public RefPtrBase
{
	public:
		TerritoryRef(void) {}
		~TerritoryRef(void) {}
};

TerritoryRef __fastcall WDT_Get_Territory_Ref(int index);
char * CalcRandomMapDigest(void);
void __fastcall FUN_005E7EB0(MapPreviewClass * preview);
extern int FUN_00734FD0(void);
extern int DAT_00ABED0C;
extern int DAT_00ABDFD0;
extern int DAT_00ABDFD4;
extern int DAT_00ABEC90;
extern int DAT_00ABECD0;
extern Cell DAT_00ABE304[8];
extern Point2D DAT_00ABE328[8];
extern char const * DAT_0082B034[5];
extern char const * DAT_0082B048[4];
extern char const * DAT_0082B060[4];
extern char const * DAT_0082B070[4];
struct BiomeNameEntry {
	char Name[0x70];
};
extern BiomeNameEntry DAT_007E1B88[2];
extern char DAT_00ABECA0[];
extern int DAT_0082B0A8[];
extern int DAT_0082B0BC[];
extern int DAT_0082B10C[];
extern int DAT_0082B120[];
extern int DAT_0082B0F8[];
extern int DAT_00ABED40[];
extern int DAT_0082B0D0[];
extern int DAT_00ABED18[];
extern int DAT_0082B080[];
extern int DAT_0082B094[];
extern int * DAT_00ABE260;
extern int * DAT_00ABE27C;

struct TerritoryData {
	void * VTable;
	int RefCount;
	int Index;
	unsigned short Seed;
	unsigned char Pad0[0x12 - 0x0E];
	unsigned char Width;
	unsigned char Height;
	unsigned char Pad1[0x18 - 0x14];
	int Resources;
	unsigned char MapType;
	unsigned char Biome;
	unsigned char Time;
	unsigned char CliffsMin;
	unsigned char CliffsMax;
	unsigned char Cliffs;
	unsigned char AccessibilityMin;
	unsigned char AccessibilityMax;
	unsigned char Accessibility;
	unsigned char HillsMin;
	unsigned char HillsMax;
	unsigned char Hills;
	unsigned char WaterMin;
	unsigned char WaterMax;
	unsigned char Water;
	unsigned char TiberiumMin;
	unsigned char TiberiumMax;
	unsigned char Tiberium;
	unsigned char LayoutMin;
	unsigned char LayoutMax;
	unsigned char Layout;
	unsigned char VegetationMin;
	unsigned char VegetationMax;
	unsigned char Vegetation;
	unsigned char CitiesMin;
	unsigned char CitiesMax;
	unsigned char Cities;
};

// SYNTHETIC: GAMEMD 0x0058B740 SYMBOL
// _$E185
// SYNTHETIC: GAMEMD 0x0058B760 SYMBOL
// _$E183
// SYNTHETIC: GAMEMD 0x0058B770 SYMBOL
// _$E188
// SYNTHETIC: GAMEMD 0x0058B780 SYMBOL
// _$E191

// FUNCTION: GAMEMD 0x0058B7B0
void FUN_0058B7B0(void)
{
}

// FUNCTION: GAMEMD 0x0058B7C0
void FUN_0058B7C0(void)
{
}

// FUNCTION: GAMEMD 0x0058B7D0
int FUN_0058B7D0(void)
{
	return DAT_00ABED0C;
}

// FUNCTION: GAMEMD 0x0058B7E0
int FUN_0058B7E0(void)
{
	return DAT_00ABDFD4;
}

// FUNCTION: GAMEMD 0x0058B7F0
int FUN_0058B7F0(void)
{
	return DAT_00ABDFD0;
}

// FUNCTION: GAMEMD 0x0058B800
int FUN_0058B800(void)
{
	return DAT_00ABEC90;
}

// FUNCTION: GAMEMD 0x0058B810
int FUN_0058B810(void)
{
	return DAT_00ABECD0;
}

static inline Point2D Start_Point(int index)
{
	if (index >= 0 && index < DAT_00ABED0C) {
		return DAT_00ABE328[index];
	}
	return Point2D(0, 0);
}

// FUNCTION: GAMEMD 0x0058B820
void FUN_0058B820(void)
{
	DAT_00ABED0C = 0;
	if (Scen == NULL || TacticalMap == NULL) {
		return;
	}
	for (int i = 0; i < 8; i++) {
		if (!Scen->Is_Valid_Waypoint(i)) {
			break;
		}
		CellClass * cell = &Map[Scen->Get_Waypoint_Cell(i)];
		int x;
		int y;
		TacticalMap->Lepton_To_Map_Pixel(cell->Center_Coord().X, cell->Center_Coord().Y, x, y);
		DAT_00ABE304[DAT_00ABED0C] = Scen->Get_Waypoint_Cell(i);
		DAT_00ABE328[DAT_00ABED0C] = Point2D(x / 60, y / 30);
		DAT_00ABED0C++;
	}

	Map.Reset_Iterator();
	int minx = 10000;
	int miny = 10000;
	int maxx = 0;
	int maxy = 0;
	CellClass * cptr;
	while ((cptr = Map.Iterate()) != NULL) {
		if (Map.In_Local_Radar(cptr->CellID, true)) {
			int x;
			int y;
			TacticalMap->Lepton_To_Map_Pixel(cptr->Center_Coord().X, cptr->Center_Coord().Y, x, y);
			int px = x / 60;
			int py = y / 30;
			if (px < minx) minx = px;
			if (px > maxx) maxx = px;
			if (py < miny) miny = py;
			if (py > maxy) maxy = py;
		}
	}
	DAT_00ABDFD0 = miny;
	DAT_00ABDFD4 = minx;
	DAT_00ABEC90 = maxx - minx;
	DAT_00ABECD0 = maxy - miny;

	if (Scen != NULL) {
		Scen->NumberStartingPoints = DAT_00ABED0C;
		for (int j = 0; j < Scen->NumberStartingPoints; j++) {
			Scen->StartingPoints[j] = Start_Point(j);
			Scen->HouseHomeCells[j] = DAT_00ABE304[j];
		}
		Scen->StartX = DAT_00ABDFD4;
		Scen->StartY = DAT_00ABDFD0;
		Scen->Width = DAT_00ABEC90;
		Scen->Height = DAT_00ABECD0;
	}
}

// FUNCTION: GAMEMD 0x0058BB00
Point2D FUN_0058BB00(int index)
{
	if (index >= 0 && index < DAT_00ABED0C) {
		return DAT_00ABE328[index];
	}
	return Point2D(0, 0);
}

// FUNCTION: GAMEMD 0x0058BB30
void FUN_0058BB30(void)
{
	if (Scen != NULL) {
		Scen->NumberStartingPoints = DAT_00ABED0C;
		for (int j = 0; j < Scen->NumberStartingPoints; j++) {
			Scen->StartingPoints[j] = Start_Point(j);
			Scen->HouseHomeCells[j] = DAT_00ABE304[j];
		}
		Scen->StartX = DAT_00ABDFD4;
		Scen->StartY = DAT_00ABDFD0;
		Scen->Width = DAT_00ABEC90;
		Scen->Height = DAT_00ABECD0;
	}
}

// FUNCTION: GAMEMD 0x0058BC20
void FUN_0058BC20(void)
{
	DAT_00ABED0C = 0;
}

DynamicVectorClass<MapRegionClass *> MapRegionClass::MapRegions;

// SYNTHETIC: GAMEMD 0x0058BC30 SYMBOL
// _$E197
// SYNTHETIC: GAMEMD 0x0058BC70 SYMBOL
// _$E194

// FUNCTION: GAMEMD 0x0058BCD0
bool FUN_0058BCD0(int value)
{
	return value == 1 || value == 3 || value == 4;
}

static inline bool Is_Three_Or_Four(int value)
{
	return value == 3 || value == 4;
}

// FUNCTION: GAMEMD 0x0058BCF0
bool FUN_0058BCF0(int value)
{
	return !Is_Three_Or_Four(value);
}

// FUNCTION: GAMEMD 0x0058BD20
bool FUN_0058BD20(int value)
{
	return value == 3 || value == 4;
}

// FUNCTION: GAMEMD 0x0058BD40
bool FUN_0058BD40(int value)
{
	return value == 4 || value == 3;
}

// FUNCTION: GAMEMD 0x0058BD60
bool FUN_0058BD60(int value)
{
	return value == 3 || value == 4;
}

// FUNCTION: GAMEMD 0x0058BD80
bool FUN_0058BD80(int a, int b)
{
	switch (a) {
		case 0:
			return b == 3;
		case 1:
		case 3:
		case 4:
			return b != 3;
		case 2:
			return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x0058BDC0
MapRegionClass::CellData::CellData(void) :
	CellID(0,0),
	HillHeight(0),
	HillVolatility(0),
	GreenTileChance(0),
	SandTileChance(0),
	RoughTileChance(0),
	ForestChance(0),
	RegionID(0),
	SpreadID(0),
	WaterMask(-1),
	Interior(false),
	Inviolate(false),
	TilePlaced(false),
	Forested(false),
	Iced(false),
	InFillArea(false),
	InFillReach(true),
	CanExpand(false)
{
}

// FUNCTION: GAMEMD 0x0058BE20
Cell MapRegionClass::Pick_Random_Map_Cell(void)
{
	int grid_size = MapCellStride * MapCellStride;
	unsigned int index;
	do {
		index = Pick_Random_UInt(0, grid_size - 1);
	} while (RMGCellData[index].CellID == Cell(0,0));
	return(RMGCellData[index].CellID);
}

// FUNCTION: GAMEMD 0x0058BEA0
Cell MapRegionClass::Pick_Random_Clear_Map_Cell(void)
{
	int tries = 0;
	while (true) {
		Cell cell = Pick_Random_Map_Cell();
		if (++tries > 200) {
			return(Cell(0,0));
		}
		if (Map[cell].Is_Tile_Clear()) {
			return(cell);
		}
	}
}

// FUNCTION: GAMEMD 0x0058BF70
MapRegionClass::MapRegionClass(Cell cell) :
	Unknown0(0),
	NeighborRegions(0),
	ID(TotalCount),
	CellCount(0),
	CellHeight(Map[cell].Height),
	ContainsWater(Map[cell].Is_Tile_With_Water()),
	Position(cell),
	Unsplittable(false),
	HasConnections(true),
	SplitCount(0),
	Unknown20(0),
	Discardable(false),
	Cells(),
	RegionBounds(0,0,0,0)
{
	Cells.Set_Growth_Step(100000);

	TotalCount++;
	MapRegions.Add(this);
}

// FUNCTION: GAMEMD 0x0058C070
MapRegionClass::~MapRegionClass(void)
{
	if (Unknown0 != NULL) {
		delete Unknown0;
		Unknown0 = NULL;
	}
	MapRegions.Delete(this);
}

// FUNCTION: GAMEMD 0x0058C100
void MapRegionClass::Delete_All_Regions(void)
{
	MapRegionClass::TotalCount = 0;

	for (int i = MapRegionClass::MapRegions.Count() - 1; i >= 0; i--) {
		MapRegionClass * region = MapRegionClass::MapRegions[i];
		if (region != NULL) {
			delete region;
		}
	}
}

// FUNCTION: GAMEMD 0x0058C1C0
bool MapRegionClass::CellData::Is_Not_Inviolate(Cell const &cell)
{
	return(!MapRegionClass::Get_Cell_Data(cell).Inviolate);
}

// FUNCTION: GAMEMD 0x0058C1F0
bool MapRegionClass::CellData::Is_Interior(Cell const &cell)
{
	return(MapRegionClass::Get_Cell_Data(cell).Interior);
}

// FUNCTION: GAMEMD 0x0058C210
void MapRegionClass::CellData::Set_Inviolate(Cell const &cell)
{
	MapRegionClass::Get_Cell_Data(cell).Inviolate = true;
}

// FUNCTION: GAMEMD 0x0058C240
void MapRegionClass::CellData::Clear_Inviolate(Cell const &cell)
{
	MapRegionClass::Get_Cell_Data(cell).Inviolate = false;
}

// FUNCTION: GAMEMD 0x0058C270
void MapRegionClass::CellData::Clear_Inviolates(void)
{
	int grid_size = MapCellStride * MapCellStride;
	for (int i = 0; i < grid_size; i++) {
		MapRegionClass::Get_Cell_Data(grid_size).Inviolate = false;
	}
}

// FUNCTION: GAMEMD 0x0058C2A0
MapRegionClass::CellData & MapRegionClass::Get_Cell_Data(Cell const & cell)
{
	return(RMGCellData[cell.X + cell.Y * MapCellStride]);
}

// FUNCTION: GAMEMD 0x0058C2C0
MapRegionClass::CellData & MapRegionClass::Get_Cell_Data(int index)
{
	return(RMGCellData[index]);
}

// FUNCTION: GAMEMD 0x0058C2D0
void MapRegionClass::Recount_Cells(void)
{
	CellCount = 0;
	int grid_size = MapCellStride * MapCellStride;

	for (int i = 0; i < grid_size; i++) {
		if (RMGCellData[i].RegionID == ID && RMGCellData[i].CellID != Cell(0,0)) {
			CellCount++;
		}
	}
}

// FUNCTION: GAMEMD 0x0058C320
Cell MapRegionClass::Pick_Random_Cell(void)
{
	int grid_size = MapCellStride * MapCellStride;
	unsigned int index = 0;
	do {
		index = Pick_Random_UInt(0, grid_size - 1);
	} while (RMGCellData[index].RegionID != ID || RMGCellData[index].CellID == Cell(0,0));

	return(RMGCellData[index].CellID);
}

// FUNCTION: GAMEMD 0x0058C3B0
Cell MapRegionClass::Pick_Random_Clear_Cell(void)
{
	int tries = 0;
	while (true) {
		Cell cell = Pick_Random_Cell();
		if (++tries > 200) {
			return(Cell(0,0));
		}
		if (Map[cell].Is_Tile_Clear()) {
			return(cell);
		}
	}
}

// FUNCTION: GAMEMD 0x0058C4A0
Cell MapRegionClass::Pick_Random_Clear_Interior_Cell(void)
{
	MapRegionClass::CellData & data = RMGCellData[0];
	int grid_size = MapCellStride * MapCellStride;
	int tries = 0;
	do {
		data = RMGCellData[Pick_Random_UInt(0, grid_size - 1)];
		if (++tries >= 100) {
			return(Cell(0,0));
		}
	} while (data.RegionID != ID || data.CellID == Cell(0, 0) || !data.Interior || !Map[data.CellID].Is_Tile_Clear());
	return(data.CellID);
}

// FUNCTION: GAMEMD 0x0058C5A0
Cell MapRegionClass::Pick_Random_Clear_Cell_In_Zone(int zone, int region_id)
{
	MapRegionClass::CellData & data = RMGCellData[0];
	int grid_size = MapCellStride * MapCellStride;
	int tries = 0;
	CellClass * cellptr;
	do {
		data = RMGCellData[Pick_Random_UInt(0, grid_size - 1)];
		if (++tries >= 100) {
			return(Cell(0,0));
		}
		cellptr = &Map[data.CellID];
	} while (data.CellID == Cell(0,0)
			|| Map.Get_Cell_Zone(cellptr->CellID, MZONE_NORMAL, false) != zone
			|| !cellptr->Is_Tile_Clear()
			|| (region_id != -1 && data.RegionID != region_id));
	return(data.CellID);
}

// FUNCTION: GAMEMD 0x0058C6C0
void MapRegionClass::CellData::Reset_Spreads(bool)
{
	int grid_size = MapCellStride * MapCellStride;
	for (int i = 0; i < grid_size; i++) {
		RMGCellData[i].SpreadID = -1;
	}
}

// FUNCTION: GAMEMD 0x0058C6F0
double MapRegionClass::Get_Angle_Score(const Cell & cell1, const Cell & cell2, double ref_angle)
{
	static const double _angle_weight = 1.5;
	static const double _length_weight = 0.15;

	int dx = cell2.X - cell1.X;
	int dy = cell2.Y - cell1.Y;

	double length = fastmath::sqrt((double)(dx * dx + dy * dy));

	double angle = (dx != 0) ? fastmath::atan(-((double)dy / (double)dx)) : M_PI_2;
	if (dx < 0) {
		angle += M_PI;
	}

	double angle_diff = fabs(angle - ref_angle);
	while (angle_diff >= M_PI * 2) {
		angle_diff -= M_PI * 2;
	}
	if (angle_diff > M_PI) {
		angle_diff = M_PI * 2 - angle_diff;
	}

	return(RMG_RANDOM_DOUBLE2(2.5) + _length_weight * length + _angle_weight * angle_diff);
}

// FUNCTION: GAMEMD 0x0058CE90
void MapRegionClass::Destroy_Cell_Regions(void)
{
	if (RMGCellData != NULL) {
		int grid_size = MapCellStride * MapCellStride;
		for (int i = 0; i < grid_size; i++) {
			RMGCellData[i].RegionID = -1;
			RMGCellData[i].SpreadID = -1;
		}
	}

	for (int i = MapRegionClass::MapRegions.Count() - 1; i >= 0; i--) {
		MapRegionClass * region = MapRegionClass::MapRegions[i];
		if (region != NULL) {
			delete region;
		}
	}

	MapRegionClass::TotalCount = 0;
}

// FUNCTION: GAMEMD 0x0058CF90
void MapRegionClass::Create_Water_Regions(void)
{
	int grid_size = MapCellStride * MapCellStride;

	for (int i = 0; i < grid_size; i++) {
		Cell cell = RMGCellData[i].CellID;
		CellClass *cptr = &Map[cell];
		if (RMGCellData[i].RegionID == -1 && RMGCellData[i].CellID != Cell(0,0)) {
			if (cptr->Is_Tile_With_Water() || cptr->Is_Tile_Swamp()) {
				Create_Region_From_Cell(cptr);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0058D010
void MapRegionClass::Create_Land_Regions(bool unsplittable)
{
	int grid_size = MapCellStride * MapCellStride;

	for (int i = 0; i < grid_size; i++) {
		if (RMGCellData[i].RegionID == -1 && RMGCellData[i].CellID != Cell(0,0)) {
			CellClass *cptr = &Map[RMGCellData[i].CellID];
			MapRegionClass *rgn = Create_Region_From_Cell(cptr);
			if (rgn != NULL) {
				rgn->Unsplittable = unsplittable;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0058D070
void MapRegionClass::CellData::Set_Region(Cell const &cell, int region_id)
{
	if (RMGCellData != NULL) {
		CellData & data = MapRegionClass::Get_Cell_Data(cell);
		data.RegionID = region_id;
	}
}

// FUNCTION: GAMEMD 0x0058D0A0
int MapRegionClass::CellData::Get_Region(Cell const &cell)
{
	if (RMGCellData == NULL) {
		return(-1);
	}
	return(MapRegionClass::Get_Cell_Data(cell).RegionID);
}

// FUNCTION: GAMEMD 0x0058D0D0
void MapRegionClass::CellData::Set_Spread(Cell const &cell, int patch_id)
{
	MapRegionClass::Get_Cell_Data(cell).SpreadID = patch_id;
}

// FUNCTION: GAMEMD 0x0058D100
int MapRegionClass::CellData::Get_Spread(Cell const &cell)
{
	return(MapRegionClass::Get_Cell_Data(cell).SpreadID);
}

// FUNCTION: GAMEMD 0x0058D120
void MapRegionClass::Clear_Cells(int new_id, int cell_height)
{
	int grid_size = MapCellStride * MapCellStride;

	for (int i = 0; i < grid_size; i++) {
		if (RMGCellData[i].RegionID == ID) {
			RMGCellData[i].RegionID = new_id;
			RMGCellData[i].CanExpand = false;
			Cell cell = RMGCellData[i].CellID;
			CellClass *cptr = &Map[cell];
			cptr->ITType = ISOTILE_CLEAR;
			cptr->SubTile = 0;
			if (cell_height != -1) {
				cptr->Height = cell_height;
			}
		}
	}
	CellCount = 0;
}

// FUNCTION: GAMEMD 0x0058D1C0
void MapRegionClass::Reassign_Border_Cells(int rings, unsigned int region_id)
{
	DynamicVectorClass<Cell> * cells = Build_Border_Cell_List();
	if (rings > 1) {
		CellData::Reset_Spreads(false);
	}

	for (int i = 0; i < rings; i++) {

		for (int j = cells->Count() - 1; j >= 0; j--) {
			Cell c = (*cells)[j];
			CellData::Set_Region(c, region_id);
		}

		if (i < rings - 1) {
			DynamicVectorClass<Cell> * ncells = new DynamicVectorClass<Cell>;
			ncells->Set_Growth_Step(cells->Count());

			for (int k = cells->Count() - 1; k >= 0; k--) {
				Cell cell = (*cells)[k];
				for (int dir = 0; dir < FACING_COUNT; dir++) {
					Cell ncell = cell + AdjacentCell[dir & 7];
					if (My_In_Radar(ncell)) {
						MapRegionClass::CellData * data = &MapRegionClass::Get_Cell_Data(ncell);
						if (data->RegionID == ID && data->SpreadID != i + 1) {
							ncells->Add(ncell);
							data->SpreadID = i + 1;
						}
					}
				}
			}
			delete cells;
			cells = ncells;
		}
	}

	delete cells;

	if (rings > 1) {
		CellData::Reset_Spreads(false);
	}
}

// FUNCTION: GAMEMD 0x0058D410
DynamicVectorClass<Cell> *MapRegionClass::Build_Border_Cell_List(void)
{
	DynamicVectorClass<Cell> *cells = new DynamicVectorClass<Cell>;

	Map.Reset_Iterator();
	CellClass *cptr = Map.Iterate();

	while (cptr != NULL) {

		if (CellData::Get_Region(cptr->Fetch_CellID()) == ID) {
			bool add = false;

			for (int facing = FACING_FIRST; facing < FACING_COUNT; facing++) {
				Cell ncell = cptr->Adjacent_Cell((FacingType)facing).Fetch_CellID();
				if (My_In_Radar(ncell)) {
					if (CellData::Get_Region(ncell) != ID) {
						add = true;
					}
				}
			}

			if (add) {
				cells->Add(cptr->Fetch_CellID());
			}
		}

		cptr = Map.Iterate();
	}

	return(cells);
}

// FUNCTION: GAMEMD 0x0058D5A0
void MapRegionClass::Increase_Height(char cell_height)
{
	Map.Reset_Iterator();
	CellClass *cptr = Map.Iterate();
	while (cptr != NULL) {
		int rid = CellData::Get_Region(cptr->Fetch_CellID());
		if (rid == ID) {
			cptr->Height += cell_height;
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0058E5D0
DynamicVectorClass<Cell> *MapRegionClass::Build_Border_Cell_List2(void)
{
	DynamicVectorClass<Cell> *cells = new DynamicVectorClass<Cell>;
	cells->Set_Growth_Step(MAX(10, CellCount / 2));

	for (int i = Cells.Count() - 1; i >= 0; i--) {
		Cell cell = Cells[i];
		bool add = false;
		for (int facing = FACING_FIRST; facing < FACING_COUNT; facing++) {
			Cell ncell = AdjacentCell[facing & 7] + cell;
			if (My_In_Radar(ncell)) {
				if (CellData::Get_Region(ncell) != ID) {
					add = true;
					break;
				}
			}
		}

		if (add) {
			cells->Add(cell);
		}
	}

	return(cells);
}

// FUNCTION: GAMEMD 0x0058E740
bool MapRegionClass::Grow(int rings)
{
	DynamicVectorClass<Cell> * cells = Build_Border_Cell_List();

	for (int i = 0; i < rings; i++) {
		DynamicVectorClass<Cell> * ncells = new DynamicVectorClass<Cell>;
		ncells->Set_Growth_Step(3 * cells->Count());

		for (int j = 0; j < cells->Count(); j++) {
			for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
				Cell const & cell = (*cells)[j];
				Cell ncell = Adjacent_Cell(cell, dir);
				if (My_In_Radar(ncell)) {
					if (Map[ncell].Height == CellHeight) {
						MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(ncell);
						int id = data.RegionID;
						if (id == -1 && (Map[ncell].Is_Tile_Clear() || Map[ncell].Is_Tile_Clear_To_Green_LAT())) {
							ncells->Add(ncell);
							data.RegionID = ID;
							Map[ncell].Height = CellHeight;
						} else if (id != ID) {
							delete cells;
							delete ncells;
							return(false);
						}
					}
				}
			}
		}

		delete cells;
		cells = ncells;
	}

	delete cells;
	return(true);
}

// FUNCTION: GAMEMD 0x0058E9B0
bool MapRegionClass::Claim_Expandable_Cells(void)
{
	int i;
	int j;

	DynamicVectorClass<Cell> * cells = Build_Border_Cell_List();

	DynamicVectorClass<Cell> ncells;
	ncells.Set_Growth_Step(50000);

	for (i = 0; i < cells->Count(); i++) {
		ncells.Add((*cells)[i]);
	}

	delete cells;

	while (ncells.Count() > 0) {
		j = ncells.Count() - 1;
		Cell c = ncells[j];
		ncells.Delete_Index(j);
		for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
			Cell ncell = Adjacent_Cell(c, dir);
			if (My_In_Radar(ncell)) {
				MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(ncell);
				int id = data.RegionID;
				if (id == -1 && data.CanExpand) {
					data.RegionID = ID;
					ncells.Add(ncell);
				}
			}
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x0058EBC0
void MapRegionClass::Make_Ice_Regions(void)
{
	int i;
	for (i = 0; i < MapRegionClass::MapRegions.Count(); i++) {
		MapRegionClass::MapRegions[i]->Cells.Clear();
		MapRegionClass::MapRegions[i]->CellCount = 0;
		MapRegionClass::MapRegions[i]->RegionBounds = Rect(9999, 9999, 0, 0);
	}

	int grid_size = MapCellStride * MapCellStride;
	for (i = grid_size - 1; i >= 0; i--) {
		CellData & data = MapRegionClass::Get_Cell_Data(i);
		Cell cell = data.CellID;
		int y = cell.Y;
		int x = cell.X;
		if (My_In_Radar(cell)) {
			int id = data.RegionID;
			if (id >= 0 && id < MapRegionClass::MapRegions.Count()) {
				MapRegionClass::MapRegions[id]->CellCount++;
				MapRegionClass::MapRegions[id]->Cells.Add(cell);

				Rect & rect = MapRegionClass::MapRegions[id]->RegionBounds;
				if (!rect.Width) {
					rect.X = x;
					rect.Y = y;
					rect.Width = 1;
					rect.Height = 1;
				}

				int old_x = rect.X;
				if (x >= rect.X + rect.Width) {
					rect.Width = x - old_x + 1;
				}
				if (x < old_x) {
					int old_width = rect.Width;
					rect.X = x;
					rect.Width = old_x - x + old_width;
				}

				int old_y = rect.Y;
				if (y >= rect.Y + rect.Height) {
					rect.Height = y - old_y + 1;
				}
				if (y < old_y) {
					int old_height = rect.Height;
					rect.Y = y;
					rect.Height = old_y - y + old_height;
				}
			}
		}
	}

	double size_limit = ((double)RandomMapGen.SeedData.Cliffs * 0.005 + 0.05) * (double)(int)RandomMapGen.LocalWidth * (double)(int)RandomMapGen.LocalHeight;
	int limit = (int)(size_limit + size_limit);

	while (true) {
		bool restart = false;

		for (int i = 0; i < MapRegionClass::MapRegions.Count(); i++) {
			MapRegionClass * region = MapRegionClass::MapRegions[i];
			if (!region->Unsplittable && !region->ContainsWater) {
				if (region->CellCount > limit) {
					if (region->Split_Region()) {
						delete region;
						restart = true;
						break;
					}
				} else {
					region->Unsplittable = true;
				}
			}
		}

		if (!restart) {
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x0058EEA0
void MapRegionClass::Mark_Interior_Cells(void)
{
	int grid_size = MapCellStride * MapCellStride;
	for (int i = 0; i < grid_size; i++) {
		MapRegionClass::Get_Cell_Data(i).Interior = false;
	}

	Reassign_Border_Cells(8, -2);

	for (int j = 0; j < grid_size; j++) {
		CellData & data = RMGCellData[j];
		if (data.RegionID == ID) {
			RMGCellData[j].Interior = true;
		} else if (data.RegionID == -2) {
			RMGCellData[j].RegionID = ID;
		}
	}
}

// FUNCTION: GAMEMD 0x0058EF10
bool MapRegionClass::Make_Regions(void)
{
	bool success = RandomMapGen.Expand_All_High_Ground();
	Destroy_Cell_Regions();
	Create_Land_Regions(false);
	if (success) {
		int i;
		for (i = 0; i < MapRegions.Count(); i++) {
			MapRegions[i]->Build_Neighbor_Regions();
		}
		for (i = 0; i < MapRegions.Count(); i++) {
			MapRegions[i]->Make_Region_Connections();
		}
		for (i = 0; i < MapRegions.Count(); i++) {
			delete MapRegions[i]->NeighborRegions;
			MapRegions[i]->NeighborRegions = NULL;
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x0058F0C0
void MapRegionClass::Build_Neighbor_Regions(void)
{
	NeighborRegions = new DynamicVectorClass<int>;
	bool * adjacent = new bool[MapRegionClass::TotalCount];
	for (int i = 0; i < MapRegionClass::TotalCount; ++i) {
		adjacent[i] = false;
	}
	DynamicVectorClass<Cell> * cells = Build_Border_Cell_List();

	for (int j = 0; j < cells->Count(); j++) {
		Cell cell = (*cells)[j];
		for (int facing = 0; facing < FACING_COUNT; facing++) {
			Cell ncell = AdjacentCell[facing & 7] + cell;
			if (My_In_Radar(ncell)) {
				int id = CellData::Get_Region(ncell);
				if (id >= 0) {
					adjacent[id] = true;
				}
			}
		}
	}

	for (int k = 0; k < MapRegionClass::TotalCount; k++) {
		if (adjacent[k] && k != ID) {
			NeighborRegions->Add(k);
		}
	}

	Recount_Cells();

	delete cells;
	delete[] adjacent;
}

// FUNCTION: GAMEMD 0x005902C0
bool MapRegionClass::Is_Bridge_Allowed(Rect const & rect) const
{
	int height = Map[Cell(rect.X, rect.Y)].Height;

	if (My_In_Radar(Cell(rect.X, rect.Y)) &&
		My_In_Radar(Cell(rect.X + rect.Width - 1, rect.Y)) &&
		My_In_Radar(Cell(rect.X, rect.Y + rect.Height - 1)) &&
		My_In_Radar(Cell(rect.X + rect.Width - 1,rect.Y + rect.Height - 1))) {
		for (int y = rect.Y; y < rect.Y + rect.Height + 1; y++) {
			for (int x = rect.X; x < rect.X + rect.Width + 1; x++) {
				CellClass *cptr = &Map[Cell(x,y)];
				if (cptr->Overlay != OVERLAY_NONE) {
					return(false);
				}
				if (cptr->Height != height) {
					return(false);
				}
				if (!cptr->Is_Tile_Clear() && !cptr->Is_Tile_With_Water()) {
					return(false);
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005904B0
bool MapRegionClass::Place_Bridge_Hut(Rect const & rect) const
{
	for (int y = rect.Y; y < rect.Y + rect.Height + 1; y++) {
		for (int x = rect.X; x < rect.X + rect.Width + 1; x++) {
			CellClass *cptr = &Map[Cell(x,y)];
			if (cptr->Overlay == OVERLAY_NONE && cptr->Is_Tile_Clear() && cptr->Cell_Occupier() == NULL) {
				HouseClass *hptr = House_From_HousesType(HouseTypeClass::From_Name("Neutral"));
				StructType caba = BuildingTypeClass::From_Name("CABHUT");
				BuildingClass *thehut = new BuildingClass(BuildingTypes[caba], hptr);
				thehut->Unlimbo(cptr->CellID);
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005905D0
void MapRegionClass::Make_Region_Connections(void)
{
	if (ContainsWater) {
		for (int i = 0; i < NeighborRegions->Count() - 1; i++) {
			MapRegionClass * region = From_ID((*NeighborRegions)[i]);
			if (region->NeighborRegions->Count() > 1 || region->CellCount > 50) {
				for (int k = i + 1; k < NeighborRegions->Count(); k++) {
					MapRegionClass * region2 = From_ID((*NeighborRegions)[k]);
					if ((region2->NeighborRegions->Count() > 1 || region2->CellCount > 50) && !region2->ContainsWater) {
						if (region2->CellHeight == region->CellHeight && region->CellHeight == CellHeight) {
							Connect_Regions_With_Bridge(region, region2);
						}
					}
				}
			}
		}
	} else {
		for (int i = 0; i < NeighborRegions->Count(); i++) {
			MapRegionClass * region = From_ID((*NeighborRegions)[i]);
			if (region->ID > ID && region->CellHeight != CellHeight) {
				int total = 0;
				int target = ((int)Pick_Random_UInt(0, 100) < RandomMapGen.SeedData.Accessibility ? Pick_Random_UInt(1, 2) : 0) + 1;

				int tries = 0;

				int id1 = region->CellHeight > CellHeight ? region->ID : ID;
				int id2 = region->CellHeight > CellHeight ? ID : region->ID;

				MapRegionClass * region2 = From_ID(id1);
				DynamicVectorClass<Cell> * cells = region2->Build_Border_Cell_List();

				while (total < target && tries < 100) {
					Cell cell = (*cells)[Pick_Random_UInt(0, cells->Count() - 1)];
					if (Map.In_Local_Radar(cell)) {
						total += region2->Connect_Regions_With_Ramp(cell, id2, (float)tries * 0.01f) ? 1 : 0;
					}
					tries++;
				}
				if (total == 0) {
					HasConnections = false;
				}

				delete cells;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00590970
bool MapRegionClass::Connect_Regions_With_Ramp(Cell const & cell, int region_id, float threshold)
{
	bool valid = true;
	unsigned mask = Build_Neighbor_Region_Mask(cell, valid, region_id, threshold);
	if (!valid) {
		return(false);
	}

	bool result = false;

	if (mask == (MG_FACINGF_N | MG_FACINGF_NE | MG_FACINGF_E | MG_FACINGF_SE | MG_FACINGF_S | MG_FACINGF_SW | MG_FACINGF_W | MG_FACINGF_NW)) {
		return(false);
	}

	{
		Cell c1;
		Cell c2;

		if ((mask & (MG_FACINGF_N | MG_FACINGF_E)) == (MG_FACINGF_N | MG_FACINGF_E)
			&& (mask & (MG_FACINGF_S | MG_FACINGF_SW | MG_FACINGF_W)) == 0) {
			c1.X = cell.X - 1;
			c1.Y = cell.Y - 5;
			c2.X = cell.X + 5;
			c2.Y = cell.Y + 1;
			if ((mask & MG_FACINGF_NW) != 0) {
				++c1.Y;
			}
			if ((mask & MG_FACINGF_SE) != 0) {
				--c2.X;
			}
			result = Build_Corner_Ramp_SW(c1, c2, region_id);
		}

		if (!result && (mask & (MG_FACINGF_E | MG_FACINGF_S)) == (MG_FACINGF_E | MG_FACINGF_S)
			&& (mask & (MG_FACINGF_NE | MG_FACINGF_SW)) == 0) {
			c1.X = cell.X + 5;
			c1.Y = cell.Y - 1;
			c2.X = cell.X - 1;
			c2.Y = cell.Y + 5;
			if ((mask & MG_FACINGF_SW) != 0) {
				--c2.Y;
			}
			if ((mask & MG_FACINGF_NE) != 0) {
				--c1.X;
			}
			result = Build_Corner_Ramp_NW(c1, c2, region_id);
		}

		if (!result && (mask & (MG_FACINGF_S | MG_FACINGF_W)) == (MG_FACINGF_S | MG_FACINGF_W)
			&& (mask & (MG_FACINGF_N | MG_FACINGF_NE | MG_FACINGF_E)) == 0) {
			c1.X = cell.X + 1;
			c1.Y = cell.Y + 5;
			c2.X = cell.X - 5;
			c2.Y = cell.Y - 1;
			if ((mask & MG_FACINGF_NW) != 0) {
				--c2.X;
			}
			if ((mask & MG_FACINGF_SE) != 0) {
				--c1.Y;
			}
			result = Build_Corner_Ramp_NE(c1, c2, region_id);
		}

		if (!result && (mask & (MG_FACINGF_N | MG_FACINGF_W)) == (MG_FACINGF_N | MG_FACINGF_W)
			&& (mask & (MG_FACINGF_E | MG_FACINGF_SE | MG_FACINGF_S)) == 0) {
			c1.X = cell.X - 5;
			c1.Y = cell.Y + 1;
			c2.X = cell.X + 1;
			c2.Y = cell.Y - 5;
			if ((mask & MG_FACINGF_NE) != 0) {
				++c2.Y;
			}
			if ((mask & MG_FACINGF_SW) != 0) {
				++c1.X;
			}
			result = Build_Corner_Ramp_NE(c1, c2, region_id);
		}

		if (!result && (mask & (MG_FACINGF_SW | MG_FACINGF_W | MG_FACINGF_NW)) == 0
			&& (mask & (MG_FACINGF_N | MG_FACINGF_S)) != 0) {
			c1.X = cell.X - 1;
			c1.Y = cell.Y - 4;
			c2.X = c1.X;
			c2.Y = cell.Y + 4;
			if ((mask & MG_FACINGF_N) != 0) {
				++c1.Y;
				++c2.Y;
			}
			if ((mask & MG_FACINGF_S) != 0) {
				--c2.Y;
				--c1.Y;
			}

			c1.X += Pick_Random_UInt(0, 1);
			c2.X += Pick_Random_UInt(0, 1);

			result = Build_West_Ramp(c1, c2, region_id);
		}

		if (!result && (mask & (MG_FACINGF_NE | MG_FACINGF_E | MG_FACINGF_SE)) == 0
			&& (mask & (MG_FACINGF_N | MG_FACINGF_S)) != 0) {
			c1.X = cell.X + 1;
			c1.Y = cell.Y + 4;
			c2.X = c1.X;
			c2.Y = cell.Y - 4;
			if ((mask & MG_FACINGF_N) != 0) {
				++c2.Y;
				++c1.Y;
			}
			if ((mask & MG_FACINGF_S) != 0) {
				--c1.Y;
				--c2.Y;
			}

			c1.X += Pick_Random_UInt(0, 1) - 1;
			c2.X += Pick_Random_UInt(0, 1) - 1;

			result = Build_East_Ramp(c1, c2, region_id);
		}

		if (!result && (mask & (MG_FACINGF_N | MG_FACINGF_NE | MG_FACINGF_NW)) == 0
			&& (mask & (MG_FACINGF_E | MG_FACINGF_W)) != 0) {
			c1.X = cell.X + 4;
			c1.Y = cell.Y - 1;
			c2.X = cell.X - 4;
			c2.Y = c1.Y;
			if ((mask & MG_FACINGF_W) != 0) {
				++c2.X;
				++c1.X;
			}
			if ((mask & MG_FACINGF_E) != 0) {
				--c1.X;
				--c2.X;
			}

			c1.Y += Pick_Random_UInt(0, 1);
			c2.Y += Pick_Random_UInt(0, 1);

			result = Build_North_Ramp(c1, c2, region_id);
		}

		if (!result && (mask & (MG_FACINGF_SE | MG_FACINGF_S | MG_FACINGF_SW)) == 0
			&& (mask & (MG_FACINGF_E | MG_FACINGF_W)) != 0) {
			c1.X = cell.X - 4;
			c1.Y = cell.Y + 1;
			c2.X = cell.X + 4;
			c2.Y = c1.Y;
			if ((mask & MG_FACINGF_W) != 0) {
				++c1.X;
				++c2.X;
			}
			if ((mask & MG_FACINGF_E) != 0) {
				--c2.X;
				--c1.X;
			}

			c1.Y += Pick_Random_UInt(0, 1) - 1;
			c2.Y += Pick_Random_UInt(0, 1) - 1;

			result = Build_South_Ramp(c1, c2, region_id);
		}
	}

	if (threshold > 0.5f) {
		if (!result) {
			if ((mask & MG_FACINGF_S) == 0) {
				Cell first(cell.X - 4, cell.Y + 1);
				Cell second(cell.X + 4, cell.Y + 1);
				result = Build_South_Ramp(first, second, region_id);
			}

			if (!result && (mask & MG_FACINGF_E) == 0) {
				Cell first(cell.X + 1, cell.Y + 4);
				Cell second(cell.X + 1, cell.Y - 4);
				result = Build_East_Ramp(first, second, region_id);
			}

			if (!result && (mask & MG_FACINGF_S) == 0) {
				Cell first(cell.X + 4, cell.Y - 1);
				Cell second(cell.X - 4, cell.Y - 1);
				result = Build_North_Ramp(first, second, region_id);
			}

			if (!result && (mask & MG_FACINGF_W) == 0) {
				Cell first(cell.X - 1, cell.Y - 4);
				Cell second(cell.X - 1, cell.Y + 4);
				result = Build_West_Ramp(first, second, region_id);
			}
		}
	}

	return(result);
}

// FUNCTION: GAMEMD 0x00590FD0
unsigned MapRegionClass::Build_Neighbor_Region_Mask(Cell const & cell, bool & valid, int unknown, float threshold)
{
	static const double lo = 5.0;
	static const double hi = 15.0;
	int min_count = (int)(((hi - lo) * (1.0f - threshold)) + lo);
	Cell c = (cell - Cell(2, 2));
	int ox = c.X;
	int oy = c.Y;

	if (!MapGeneratorClass::Region_Cell_Count_At_Least(Rect(ox, oy, 5, 5), ID, unknown, min_count)) {
		valid = false;
		return(0);
	}

	unsigned mask = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			if (y != 1 || x != 1) {
				if (MapGeneratorClass::Region_Cell_Count_At_Least(Rect(ox + 5 * (x - 1), oy + 5 * (y - 1), 5, 5), ID, unknown, min_count)) {
					static int _facing_bits[3][3] = {
						MG_FACINGF_NW, MG_FACINGF_N, MG_FACINGF_NE,
						MG_FACINGF_W, 0, MG_FACINGF_E,
						MG_FACINGF_SW, MG_FACINGF_S, MG_FACINGF_SE
					};
					mask |= _facing_bits[y][x];
				}
			}
		}
	}

	return(mask);
}

// FUNCTION: GAMEMD 0x00594010
bool MapRegionClass::Is_Ramp_Area_Clear(Rect & rect, int id)
{
	if (Map.In_Local_Radar(Cell(rect.X, rect.Y)) &&
		Map.In_Local_Radar(Cell(rect.X + rect.Width - 1, rect.Y)) &&
		Map.In_Local_Radar(Cell(rect.X, rect.Y + rect.Height - 1)) &&
		Map.In_Local_Radar(Cell(rect.X + rect.Width - 1,rect.Y + rect.Height - 1))) {

		for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
			for (int x = rect.X; x < rect.X + rect.Width; x++) {
				Cell cell(x, y);
				int rid = CellData::Get_Region(cell);
				if (rid != ID && rid != id) {
					return(false);
				}
				CellClass * cptr = &Map[cell];
				if (!cptr->Is_Tile_Clear()) {
					return(false);
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00594190
void MapRegionClass::Flatten_Area(Rect const & rect, int region_id)
{
	for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
		for (int x = rect.X; x < rect.X + rect.Width; x++) {
			Cell cell(x, y);
			if (My_In_Radar(cell)) {
				CellData::Set_Region(cell, region_id);
				CellClass * cellptr = &Map[cell];
				cellptr->Height = From_ID(region_id)->CellHeight;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005942F0
void MapRegionClass::Flatten_Area(Rect const & rect)
{
	for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
		for (int x = rect.X; x < rect.X + rect.Width; x++) {
			Cell cell(x, y);
			if (My_In_Radar(cell)) {
				CellData::Set_Region(cell, ID);
				Map[cell].Height = CellHeight;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005943E0
MapRegionClass * MapRegionClass::From_ID(int id)
{
	for (int i = 0; i < MapRegions.Count(); i++) {
		if (MapRegions[i]->ID == id) {
			return(MapRegions[i]);
		}
	}
	return(NULL);
}

void __fastcall Create_Zone_Region(CellClass * cptr, MZoneType mzone, int zone);

// FUNCTION: GAMEMD 0x00594700
void Create_Zone_Regions(MZoneType mzone, int zone)
{
	MapRegionClass::Destroy_Cell_Regions();

	Map.Reset_Iterator();
	CellClass * cptr;
	while ((cptr = Map.Iterate()) != NULL) {
		if (My_In_Radar(cptr->CellID) &&
			MapRegionClass::Get_Cell_Data(cptr->CellID).RegionID == -1 &&
			cptr->Passability == PASSABLE_LAND &&
			Map.Get_Cell_Zone(cptr->CellID, mzone, false) == zone) {
			Create_Zone_Region(cptr, mzone, zone);
		}
	}
}

// FUNCTION: GAMEMD 0x00594870
void MapRegionClass::Pick_Start_Points(int first_waypoint)
{
	Rect bounds = Map.LocalRect;
	bounds.X += 4;
	bounds.Y += 4;
	bounds.Width -= 8;
	bounds.Height -= 8;
	DynamicVectorClass<Cell> cells;

	int target = Pick_Random_UInt(0, 2) + Unknown20 * 15;
	for (int tries = 0; cells.Count() < target && tries < 300; tries++) {
		Cell c = Cells[Pick_Random_UInt(0, Cells.Count() - 1)];
		Rect rect(c.X - 3, c.Y - 3, 6, 6);
		if (MapGeneratorClass::Can_Place_Paved_Road(rect, false, false) && Map.In_Area_Radar(bounds, c, true)) {
			cells.Add(c);
		}
	}

	DynamicVectorClass<Cell> * layout = Pick_Spread_Cells(cells);
	if (layout == NULL) {
		return;
	}

	for (int k = 0; k < 8; k++) {
		Scen->HouseHomeCells[k] = CELL_NONE;
	}

	int wp = first_waypoint;
	for (int i = 0; i < Unknown20; i++) {
		Scen->Set_Waypoint((WAYPOINT)wp, (*layout)[i]);
		Map[Scen->Get_Waypoint_Cell((WAYPOINT)wp)].IsWaypoint = true;
		Scen->HouseHomeCells[i] = (*layout)[i];
		wp++;
	}

	for (int j = 0; j < Unknown20; j++) {
		if (layout->Count() > 0) {
			layout->Delete_Index(0);
		}
	}

	if (Unknown0 != NULL) {
		delete Unknown0;
	}
	Unknown0 = layout;
}

// FUNCTION: GAMEMD 0x00595400
void MapRegionClass::Place_Neutral_Building(void)
{
	HouseClass * neutral = House_From_HousesType(HouseTypeClass::From_Name("Neutral"));
	bool placed = false;
	int tries = 0;

	BuildingClass * building = new BuildingClass(Rule->NeutralTechBuildings[Pick_Random_UInt(0, Rule->NeutralTechBuildings.Count() - 1)], neutral);

	while (!placed) {
		if (tries >= 100) {
			break;
		}

		Cell building_cell = Cells[Pick_Random_UInt(0, Cells.Count() - 1)];
		int height = Map[building_cell].Height;

		Cell const * list = building->Class->Occupy_List(true);
		bool ok = true;
		while (*list != REFRESH_EOL) {
			CellClass * cellptr = &Map[building_cell + *list];
			if (cellptr->Cell_Occupier() != NULL || !cellptr->Is_Tile_Clear() || cellptr->Height != height || cellptr->Land_Type() == LAND_ROCK || !Map.In_Local_Radar(cellptr) || !MapRegionClass::CellData::Is_Not_Inviolate(cellptr->CellID)) {
				ok = false;
			}
			list++;
			if (!ok) break;
		}

		if (ok) {
			if (building->Unlimbo(Coord(building_cell, 0))) {
				placed = true;
			}
		}
		tries++;
	}

	if (!placed) {
		if (building != NULL) {
			delete building;
		}
	}
}

// FUNCTION: GAMEMD 0x00595680
MapSeedClass::MapSeedClass(void) :
	Biome(0),
	MapType(1),
	Resources(1),
	Hills(0),
	Time(1),
	WaterAmount(0),
	NumPlayers(2),
	Tiberium(0),
	TiberiumLayout(0),
	Vegetation(0),
	Cities(0),
	Width(0),
	Height(0),
	Accessibility(0),
	Cliffs(0),
	Seed(-1)
{
	Extension = "SED";
	MapDescription[0] = L'\0';
	Description = MapDescription;
	if (FUN_00734FD0()) {
		wcscpy(MapDescription, StringTable::LoadString("TXT_RANDOM_MAP_DESCRIPTION", NULL, "D:\\ra2mdpost\\MapGen.cpp", 3934));
	}
}

// FUNCTION: GAMEMD 0x00595710
void MapSeedClass::Reload_Description(void)
{
	wcscpy(MapDescription, StringTable::LoadString("TXT_RANDOM_MAP_DESCRIPTION", NULL, "D:\\ra2mdpost\\MapGen.cpp", 3934));
}

// FUNCTION: GAMEMD 0x00595740
MapGeneratorClass::MapGeneratorClass(void) :
	SeedData(),
	MapSeeder(NULL),
	MapPreview(NULL),
	LocalWidth(0),
	LocalHeight(0),
	RMGMinimumTiberium(2500),
	RMGMaximumTiberium(5500),
	MaxTrees(500),
	PreviewStale(true),
	SeededWaterAmount(0),
	CellHeight(4),
	PlaceWaterfall(false)
{
}

// FUNCTION: GAMEMD 0x00595BC0
int Do_Random_Map_Dialog(bool (*callback)())
{
	LONG res = 0;

	RandomMapGen.Load_Settings();

	HWND dialog = UI::BeginDialog(0x105, Map_Seed_Dialog_Proc, 0);
	if (dialog) {
		RMGCallback = callback;
		RandomMapGen.SeedData.Callback = callback;
		SetWindowLongA(dialog, DWL_USER, (LONG)&res);
		SendMessageA(dialog, 0x4A9, 0, 1);
		UI::FocusOnWindow(dialog);
		while (res == 0) {
			if (UI::Updated() == 1) {
				break;
			}
			if (callback != NULL) {
				callback();
			}
			Title_Screen_Restore(false);
		}
		UI::EndDialog(dialog);
	}

	RMGCallback = MapGen_Call_Back;
	RandomMapGen.SeedData.Callback = NULL;

	if (RandomMapGen.MapPreview != NULL) {
		if (RandomMapGen.MapPreview->Get_Preview_Surface() != NULL) {
			RawFileClass file("RandMap.img");
			Write_PCX_File(file, *RandomMapGen.MapPreview->Get_Preview_Surface(), &GamePalette);
		}
		delete RandomMapGen.MapPreview;
		RandomMapGen.MapPreview = NULL;
	}

	if (RandomMapGen.MapSeeder != NULL) {
		delete RandomMapGen.MapSeeder;
		RandomMapGen.MapSeeder = NULL;
	}

	return(res);
}

// FUNCTION: GAMEMD 0x00595CE0
void Clean_Up_RMCache(void)
{
	WIN32_FIND_DATA *ff;

	DynamicVectorClass<WIN32_FIND_DATA *> files;

	ff = new WIN32_FIND_DATA;
	HANDLE handle = FindFirstFile("rmcache\\*.mmp", ff);

	if (handle != INVALID_HANDLE_VALUE) {
		files.Add(ff);

		ff = new WIN32_FIND_DATA;
		while (FindNextFile(handle, ff) != 0) {
			files.Add(ff);
			ff = new WIN32_FIND_DATA;
		}
		FindClose(handle);
	}

	delete ff;

	while (files.Count() > 70) {
		FILETIME oldesttime;
		oldesttime.dwLowDateTime = 0xFFFFFFFF;
		oldesttime.dwHighDateTime = 0x7FFFFFFF;

		int oldest = -1;
		for (int index = 0; index < files.Count(); index++) {
			WIN32_FIND_DATA * file = files[index];

			if (file->ftLastAccessTime.dwHighDateTime != 0) {
				if (CompareFileTime(&file->ftLastAccessTime, &oldesttime) == -1) {
					oldest = index;
					oldesttime.dwLowDateTime = files[index]->ftLastAccessTime.dwLowDateTime;
					oldesttime.dwHighDateTime = files[index]->ftLastAccessTime.dwHighDateTime;
				}
			} else if (file->ftCreationTime.dwHighDateTime != 0) {
				if (CompareFileTime(&file->ftCreationTime, &oldesttime) == -1) {
					oldest = index;
					oldesttime.dwLowDateTime = files[index]->ftCreationTime.dwLowDateTime;
					oldesttime.dwHighDateTime = files[index]->ftCreationTime.dwHighDateTime;
				}
			} else if (file->ftLastWriteTime.dwHighDateTime != 0) {
				if (CompareFileTime(&file->ftLastWriteTime, &oldesttime) == -1) {
					oldest = index;
					oldesttime.dwLowDateTime = files[index]->ftLastWriteTime.dwLowDateTime;
					oldesttime.dwHighDateTime = files[index]->ftLastWriteTime.dwHighDateTime;
				}
			}
		}

		if (oldest == -1) {
			break;
		}

		if (!DeleteFile(files[oldest]->cFileName)) {
			break;
		}

		delete files[oldest];
		files.Delete_Index(oldest);
	}

	while (files.Count()) {
		delete files[0];
		files.Delete_Index(0);
	}
}

// FUNCTION: GAMEMD 0x00595FB0
void Do_Random_Map(HWND dialog, bool (*callback)())
{
	TerritoryRef territory;
	if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
		territory.Set(WDT_Get_Territory_Ref(Session.unknown_08).Ptr);
	}
	if (territory.Ptr != NULL) {
		RandomMapGen.SeedData.NumPlayers = 4;
	}
	char *digest = CalcRandomMapDigest();
	char name[128];
	memset(name, 0, sizeof(name));
	strncpy(name, "rmcache\\", sizeof(name));
	strncat(name, digest, sizeof(name));
	delete digest;
	strncat(name, ".mmp", sizeof(name));
	CCFileClass cfile(name);

	if (cfile.Is_Available()) {
		if (RandomMapGen.MapPreview == NULL) {
			RandomMapGen.MapPreview = new MapPreviewClass;
		}
		if (RandomMapGen.MapPreview->Read_PCX_Preview(name)) {
			RawFileClass file("RandMap.img");
			Write_PCX_File(file, *RandomMapGen.MapPreview->Get_Preview_Surface(), &GamePalette);
			delete RandomMapGen.MapPreview;
			RandomMapGen.MapPreview = NULL;
			return;
		}

		delete RandomMapGen.MapPreview;
		RandomMapGen.MapPreview = NULL;
	}

	RMGCallback = callback;
	RandomMapGen.SeedData.Callback = callback;
	if (RandomMapGen.SeedData.Seed == -1) {
		RandomMapGen.SeedData.Seed = Sim_Random_Pick(0U, 65535U);
	}
	RandomMapGen.Generate_Random_Map(true, dialog);
	RandomMapGen.MapPreview->Create_Preview();
	if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
		FUN_005E7EB0(RandomMapGen.MapPreview);
	}

	if (RandomMapGen.MapSeeder != NULL) {
		delete RandomMapGen.MapSeeder;
	}

	RandomMapGen.MapSeeder = new MapSeedClass;
	memcpy(RandomMapGen.MapSeeder, &RandomMapGen.SeedData, sizeof(RandomMapGen.SeedData));

	Scen->Set_Scenario_Name("RandomMap");
	Title_Screen_Restore(false);

	if (RandomMapGen.MapPreview != NULL) {
		if (RandomMapGen.MapPreview->Get_Preview_Surface() != NULL) {
			RawFileClass file("RandMap.img");
			Write_PCX_File(file, *RandomMapGen.MapPreview->Get_Preview_Surface(), &GamePalette);
			WIN32_FIND_DATA ff;
			if (FindFirstFile("rmcache", &ff) == INVALID_HANDLE_VALUE) {
				CreateDirectory("rmcache", 0);
			}
			CopyFile("RandMap.img", name, FALSE);
			Clean_Up_RMCache();
		}
		delete RandomMapGen.MapPreview;
		RandomMapGen.MapPreview = NULL;
	}

	if (RandomMapGen.MapSeeder != NULL) {
		delete RandomMapGen.MapSeeder;
		RandomMapGen.MapSeeder = NULL;
	}
}

// FUNCTION: GAMEMD 0x00596C70
void MapSeedClass::Get_Settings(HWND dialog)
{
	HWND handle;
	int old;

	old = MapType;
	handle = GetDlgItem(dialog, 0x405);
	MapType = ComboBox_GetItemData(handle, ComboBox_GetCurSel(handle));
	if (old != MapType) {
		RandomMapGen.PreviewStale = true;
	}

	old = Biome;
	handle = GetDlgItem(dialog, 0x407);
	Biome = ComboBox_GetItemData(handle, ComboBox_GetCurSel(handle));
	if (old != Biome) {
		RandomMapGen.PreviewStale = true;
	}

	old = Resources;
	handle = GetDlgItem(dialog, 0x408);
	Resources = ComboBox_GetItemData(handle, ComboBox_GetCurSel(handle));
	if (old != Resources) {
		RandomMapGen.PreviewStale = true;
	}

	old = Time;
	handle = GetDlgItem(dialog, 0x3EA);
	Time = ComboBox_GetItemData(handle, ComboBox_GetCurSel(handle));
	if (old != Time) {
		RandomMapGen.PreviewStale = true;
	}

	old = Width;
	handle = GetDlgItem(dialog, 0x406);
	Height = Width = ComboBox_GetItemData(handle, ComboBox_GetCurSel(handle));
	if (old != Width) {
		RandomMapGen.PreviewStale = true;
	}

	int old_players = NumPlayers;
	handle = GetDlgItem(dialog, 0x3EB);
	NumPlayers = SendMessageA(handle, WM_USER, 0, 0);
	if (old_players != NumPlayers) {
		RandomMapGen.PreviewStale = true;
	} else if (!RandomMapGen.PreviewStale) {
		goto done;
	}

	Randomize(MapType);
	if (RandomMapGen.MapPreview != NULL) {
		delete RandomMapGen.MapPreview;
	}
	RandomMapGen.MapPreview = NULL;
	RandomMapGen.PreviewStale = false;

done:
	Fixup_Settings();
}

// FUNCTION: GAMEMD 0x00596E50
void MapSeedClass::Set_Settings(HWND dialog)
{
	HWND handle;
	LRESULT item;
	int i;

	Fixup_Settings();

	handle = GetDlgItem(dialog, 0x3EA);
	while (SendMessageA(handle, CB_GETCOUNT, 0, 0) > 0) {
		SendMessageA(handle, CB_DELETESTRING, 0, 0);
	}
	for (i = 0; i < 4; i++) {
		item = SendMessageA(handle, 0x4C2, 0, (LPARAM)StringTable::LoadString(DAT_0082B060[i], NULL, "D:\\ra2mdpost\\MapGen.cpp", 4883));
		SendMessageA(handle, CB_SETITEMDATA, item, i);
	}
	item = SendMessageA(handle, 0x4BE, 0, (LPARAM)StringTable::LoadString(DAT_0082B060[Time], NULL, "D:\\ra2mdpost\\MapGen.cpp", 4887));
	SendMessageA(handle, CB_SETCURSEL, item, 0);

	handle = GetDlgItem(dialog, 0x406);
	while (SendMessageA(handle, CB_GETCOUNT, 0, 0) > 0) {
		SendMessageA(handle, CB_DELETESTRING, 0, 0);
	}
	for (i = 0; i < 4; i++) {
		item = SendMessageA(handle, 0x4C2, 0, (LPARAM)StringTable::LoadString(DAT_0082B070[i], NULL, "D:\\ra2mdpost\\MapGen.cpp", 4898));
		SendMessageA(handle, CB_SETITEMDATA, item, i);
	}
	item = SendMessageA(handle, 0x4BE, 0, (LPARAM)StringTable::LoadString(DAT_0082B070[Width], NULL, "D:\\ra2mdpost\\MapGen.cpp", 4902));
	SendMessageA(handle, CB_SETCURSEL, item, 0);

	handle = GetDlgItem(dialog, 0x405);
	while (SendMessageA(handle, CB_GETCOUNT, 0, 0) > 0) {
		SendMessageA(handle, CB_DELETESTRING, 0, 0);
	}
	for (i = 1; i < 5; i++) {
		item = SendMessageA(handle, 0x4C2, 0, (LPARAM)StringTable::LoadString(DAT_0082B034[i], NULL, "D:\\ra2mdpost\\MapGen.cpp", 4914));
		SendMessageA(handle, CB_SETITEMDATA, item, i);
	}
	item = SendMessageA(handle, 0x4BE, 0, (LPARAM)StringTable::LoadString(DAT_0082B034[MapType], NULL, "D:\\ra2mdpost\\MapGen.cpp", 4918));
	SendMessageA(handle, CB_SETCURSEL, item, 0);

	handle = GetDlgItem(dialog, 0x407);
	while (SendMessageA(handle, CB_GETCOUNT, 0, 0) > 0) {
		SendMessageA(handle, CB_DELETESTRING, 0, 0);
	}
	for (i = 0; i < 2; i++) {
		item = SendMessageA(handle, 0x4C2, 0, (LPARAM)StringTable::LoadString(DAT_007E1B88[i].Name, NULL, "D:\\ra2mdpost\\MapGen.cpp", 4930));
		SendMessageA(handle, CB_SETITEMDATA, item, i);
	}
	item = SendMessageA(handle, 0x4BE, 0, (LPARAM)StringTable::LoadString(DAT_007E1B88[Biome].Name, NULL, "D:\\ra2mdpost\\MapGen.cpp", 4934));
	SendMessageA(handle, CB_SETCURSEL, item, 0);

	handle = GetDlgItem(dialog, 0x408);
	while (SendMessageA(handle, CB_GETCOUNT, 0, 0) > 0) {
		SendMessageA(handle, CB_DELETESTRING, 0, 0);
	}
	for (i = 0; i < 4; i++) {
		item = SendMessageA(handle, 0x4C2, 0, (LPARAM)StringTable::LoadString(DAT_0082B048[i], NULL, "D:\\ra2mdpost\\MapGen.cpp", 4945));
		SendMessageA(handle, CB_SETITEMDATA, item, i);
	}
	item = SendMessageA(handle, 0x4BE, 0, (LPARAM)StringTable::LoadString(DAT_0082B048[Resources], NULL, "D:\\ra2mdpost\\MapGen.cpp", 4949));
	SendMessageA(handle, CB_SETCURSEL, item, 0);

	handle = GetDlgItem(dialog, 0x3FB);
	sprintf(DAT_00ABECA0, "%d", Seed);
	SendMessageA(handle, 0x4B4, 0, (LPARAM)DAT_00ABECA0);

	handle = GetDlgItem(dialog, 0x3EB);
	int players = NumPlayers;
	SendMessageA(handle, 0x406, 1, 0x80002);
	SendMessageA(handle, 0x405, 1, players);
	NumPlayers = players;
}

// FUNCTION: GAMEMD 0x00597260
void MapSeedClass::Randomize(int map_type)
{
	WaterAmount = Sim_Random_Pick(DAT_0082B0A8[map_type], DAT_0082B0BC[map_type]);
	Hills = Sim_Random_Pick(DAT_0082B10C[map_type], DAT_0082B120[map_type]);
	Cities = Sim_Random_Pick(DAT_00ABED40[map_type], DAT_0082B0F8[map_type]);
	Accessibility = Sim_Random_Pick(DAT_00ABED18[map_type], DAT_0082B0D0[map_type]);
	Cliffs = Sim_Random_Pick(DAT_0082B080[map_type], DAT_0082B094[map_type]);
	Tiberium = Resources * 20;
	TiberiumLayout = Sim_Random_Pick(0, 100);

	int low = DAT_00ABE260[map_type];
	int high = DAT_00ABE27C[map_type];
	if (low < 0) {
		low = 0;
	} else if (low > 100) {
		low = 100;
	}
	if (high < 0) {
		high = 0;
	} else if (high > 100) {
		high = 100;
	}
	if (low > high) {
		low = high;
	}
	Vegetation = Sim_Random_Pick(low, high);
	Seed = Sim_Random_Pick(0, 65535);
}

// FUNCTION: GAMEMD 0x00597380
void MapSeedClass::Randomize(void)
{
	Biome = Sim_Random_Pick(0U, 100U) >= 50;
	MapType = Sim_Random_Pick(1, 4);
	Time = Sim_Random_Pick(0, 3);
	Resources = Sim_Random_Pick(0, 3);
	Height = Width = Sim_Random_Pick(0, 3);
	Randomize(MapType);
	wcscpy(MapDescription, StringTable::LoadString("TXT_RANDOM_MAP_DESCRIPTION", NULL, "D:\\ra2mdpost\\MapGen.cpp", 3934));
	Seed = Sim_Random_Pick(0, 65535);
	Fixup_Settings();
}

#define WDT ((TerritoryData *)territory.Ptr)

// FUNCTION: GAMEMD 0x00597430
void MapSeedClass::Fixup_WDT_Settings(void)
{
	TerritoryRef territory;
	if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
		territory.Set(WDT_Get_Territory_Ref(Session.unknown_08).Ptr);
	}

	if (territory.Ptr != NULL) {
		Seed = WDT->Seed == 0 ? Sim_Random_Pick(0U, 65535U) : WDT->Seed;

		switch (WDT->Width) {
			case 0:
				Width = 0;
				break;
			case 1:
				Width = 1;
				break;
			case 2:
				Width = 2;
				break;
			case 3:
				Width = 3;
				break;
			default:
				Width = 1;
				break;
		}

		switch (WDT->Height) {
			case 0:
				Height = 0;
				break;
			case 1:
				Height = 1;
				break;
			case 2:
				Height = 2;
				break;
			case 3:
				Height = 3;
				break;
			default:
				Height = 1;
				break;
		}

		NumPlayers = 4;

		switch (WDT->Time) {
			case 2:
				Time = 0;
				break;
			case 0:
				Time = 1;
				break;
			case 1:
				Time = 2;
				break;
			case 3:
				Time = 3;
				break;
			default:
				Time = 1;
				break;
		}

		MapType = WDT->MapType;
		Biome = WDT->Biome;
		Cliffs = WDT->Cliffs;
		Accessibility = WDT->Accessibility;
		Hills = WDT->Hills;
		WaterAmount = WDT->Water;
		Tiberium = WDT->Tiberium;
		TiberiumLayout = WDT->Layout;
		Vegetation = WDT->Vegetation;
		Cities = WDT->Cities;
		Resources = WDT->Resources;
	}
}

#undef WDT

// FUNCTION: GAMEMD 0x005975E0
void MapSeedClass::Fixup_Settings(void)
{
	if (Resources >= 4) {
		Resources = 3;
	} else if (Resources < 0) {
		Resources = 0;
	}
	if (MapType >= 5) {
		MapType = 4;
	} else if (MapType < 0) {
		MapType = 0;
	}
	if (Time >= 4) {
		Time = 3;
	} else if (Time < 0) {
		Time = 0;
	}
	if (Hills > 100) {
		Hills = 100;
	} else if (Hills < 0) {
		Hills = 0;
	}
	if (WaterAmount > 100) {
		WaterAmount = 100;
	} else if (WaterAmount < 0) {
		WaterAmount = 0;
	}
	if (NumPlayers > 8) {
		NumPlayers = 8;
	} else if (NumPlayers < 2) {
		NumPlayers = 2;
	}
	if (Tiberium > 100) {
		Tiberium = 100;
	} else if (Tiberium < 1) {
		Tiberium = 1;
	}
	if (TiberiumLayout > 100) {
		TiberiumLayout = 100;
	} else if (TiberiumLayout < 0) {
		TiberiumLayout = 0;
	}
	if (Vegetation > 100) {
		Vegetation = 100;
	} else if (Vegetation < 0) {
		Vegetation = 0;
	}
	if (Cities > 100) {
		Cities = 100;
	} else if (Cities < 0) {
		Cities = 0;
	}
	if (Width >= 4) {
		Width = 3;
	} else if (Width < 0) {
		Width = 0;
	}
	if (Height >= 4) {
		Height = 3;
	} else if (Height < 0) {
		Height = 0;
	}
	if (Accessibility > 100) {
		Accessibility = 100;
	} else if (Accessibility < 0) {
		Accessibility = 0;
	}
	if (Cliffs > 100) {
		Cliffs = 100;
	} else if (Cliffs < 0) {
		Cliffs = 0;
	}
	if (Seed > 65535) {
		Seed = 65535;
	} else if (Seed < 0) {
		Seed = 0;
	}
}

// FUNCTION: GAMEMD 0x00597730
bool MapSeedClass::Save(const char * name)
{
	if (name) {
		return(Save_File(name, MapDescription));
	}

	if (BASECLASS::Save(MapDescription)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00597760
bool MapSeedClass::Save_File(const char * file_name, const wchar_t * descr)
{
	if (file_name != NULL) {
		DebugString("Saving random map: %s - %ls\n", file_name, descr);
		CCFileClass file(file_name);
		INIClass ini;
		if (descr) {
			wcscpy(MapDescription, descr);
		}
		ini.Put_UnicodeString("RandomMap", "Description", descr);
		ini.Put_Int("RandomMap", "Width", Width, 0);
		ini.Put_Int("RandomMap", "Height", Height, 0);
		ini.Put_Int("RandomMap", "NumPlayers", NumPlayers, 0);
		ini.Put_Int("RandomMap", "Seed", Seed, 0);
		ini.Put_Int("RandomMap", "MapType", MapType, 0);
		ini.Put_Int("RandomMap", "Theater", Biome, 0);
		ini.Put_Int("RandomMap", "Time", Time, 0);
		ini.Put_Int("RandomMap", "RegionSize", Cliffs, 0);
		ini.Put_Int("RandomMap", "Ruggedness", Hills, 0);
		ini.Put_Int("RandomMap", "Accessibility", Accessibility, 0);
		ini.Put_Int("RandomMap", "WaterAmount", WaterAmount, 0);
		ini.Put_Int("RandomMap", "Tiberium", Tiberium, 0);
		ini.Put_Int("RandomMap", "TiberiumLayout", TiberiumLayout, 0);
		ini.Put_Int("RandomMap", "Vegetation", Vegetation, 0);
		ini.Put_Int("RandomMap", "UrbanPresence", Cities, 0);
		ini.Put_Int("RandomMap", "Resources", Resources, 0);
		ini.Save(file);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00597A10
bool MapSeedClass::Load(const char * name)
{
	if (name) {
		return(Load_File(name));
	}

	if (BASECLASS::Load()) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00597D40
bool MapSeedClass::Delete(void)
{
	return(BASECLASS::Delete());
}

// FUNCTION: GAMEMD 0x00597D50
bool MapSeedClass::Delete_File(const char * file_name)
{
	return(BASECLASS::Delete_File(file_name));
}

extern wchar_t DAT_00887734[];

// FUNCTION: GAMEMD 0x00597D60
bool MapSeedClass::Read_File(FileEntryClass * entry, WIN32_FIND_DATAA * ff)
{
	wchar_t buffer[128];

	if (entry != NULL && ff != NULL) {
		if (stricmp(ff->cFileName, RANDOM_MAP_FILE_NAME) && stricmp(ff->cFileName, "lastmap.sed")) {
			CCFileClass file(ff->cFileName);
			INIClass ini;
			if (ini.Load(file, false)) {
				if (ini.Get_UnicodeString("RandomMap", "Description", DAT_00887734, buffer, 128) > 0) {
					wcsncpy(entry->Descr, buffer, 128);
					entry->Old = false;
					entry->Valid = true;
				} else {
					entry->Descr[0] = 0;
					entry->Old = true;
					entry->Valid = false;
				}
				entry->Scenario = 0;
				entry->House = HOUSE_FIRST;
				strncpy(entry->Filename, ff->cFileName, 32);
				if (!strlen(entry->Filename)) {
					strncpy(entry->Filename, ff->cAlternateFileName, 32);
				}
				entry->DateTime.dwHighDateTime = ff->ftLastWriteTime.dwHighDateTime;
				entry->DateTime.dwLowDateTime = ff->ftLastWriteTime.dwLowDateTime;
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00597F80
wchar_t const * MapSeedClass::GetUIString_Load(void) const
{
	return StringTable::LoadString("GUI:LoadMapMenu", NULL, "D:\\ra2mdpost\\MapGen.cpp", 5572);
}

// FUNCTION: GAMEMD 0x00597FA0
wchar_t const * MapSeedClass::GetUIString_Save(void) const
{
	return StringTable::LoadString("GUI:SaveMapMenu", NULL, "D:\\ra2mdpost\\MapGen.cpp", 5578);
}

// FUNCTION: GAMEMD 0x00597FC0
wchar_t const * MapSeedClass::GetUIString_Delete(void) const
{
	return StringTable::LoadString("GUI:DeleteMapMenu", NULL, "D:\\ra2mdpost\\MapGen.cpp", 5584);
}

// FUNCTION: GAMEMD 0x00597FE0
wchar_t const * MapSeedClass::GetUIString_GameSaved(void) const
{
	return StringTable::LoadString("GUI:MapSaved", NULL, "D:\\ra2mdpost\\MapGen.cpp", 5590);
}

// FUNCTION: GAMEMD 0x00598000
double Random_Fraction(void)
{
	return RMG_RANDOM_DOUBLE(1);
}

// FUNCTION: GAMEMD 0x00598030
unsigned int Pick_Random_UInt(unsigned int minval, unsigned int maxval)
{
	unsigned int value = 0;
	while (true) {
		value = ((Random_Fraction() * (maxval - minval + 1)) + (double)minval);
		if (value <= maxval) {
			break;
		}
	}
	return(value);
}

// FUNCTION: GAMEMD 0x005980A0
NormalDistribution::NormalDistribution(double (*function)(void))
{
	HasSpare = false;
	Spare = 0;
	Rand = function;
}

// FUNCTION: GAMEMD 0x005980C0
double NormalDistribution::operator ()(void)
{
	if (HasSpare) {
		HasSpare = false;
		return Spare;
	}

	double u = 0.0;
	double v = 0.0;
	double s = 0.0;

	do {
		u = 2.0 * Rand() - 1.0;
		v = 2.0 * Rand() - 1.0;
		s = v * v + u * u;
	} while (s >= 1.0 || s == 0.0);

	double l = log(s);
	double f = fastmath::sqrt((-l - l) / s);
	double r1 = f * u;
	double r2 = f * v;
	Spare = r2;
	HasSpare = true;
	return(r1);
}

// FUNCTION: GAMEMD 0x00598160
double Sample_Normal(double mean, double standard_dev)
{
	return(standard_dev * RMGGaussian() + mean);
}

// FUNCTION: GAMEMD 0x00598180
double Sample_Truncated_Normal(double mean, double scale, double lower_bound, double upper_bound)
{
	double value;

	if (mean - scale > upper_bound || mean + scale < lower_bound) {
		scale = (upper_bound - lower_bound) * 0.5;
		mean = scale + lower_bound;
	}
	do {
		do {
			value = Sample_Normal(mean, scale);
		}
		while (value < lower_bound);
	}
	while (value > upper_bound);
	return(value);
}

inline double Sample_Truncated_Normal_Inline(double mean, double scale, double lower_bound, double upper_bound)
{
	double value;

	if (mean - scale > upper_bound || mean + scale < lower_bound) {
		scale = (upper_bound - lower_bound) * 0.5;
		mean = scale + lower_bound;
	}
	do {
		value = Sample_Normal(mean, scale);
	} while (value < lower_bound || value > upper_bound);
	return(value);
}

// FUNCTION: GAMEMD 0x00599530
void MapGeneratorClass::Cleanup(void)
{
	SeededWaterAmount = 0;
	WorkingRegionID = 0;

	if (RMGCellData != NULL) {
		delete [] RMGCellData;
		RMGCellData = NULL;
	}

	for (int i = MapRegionClass::MapRegions.Count() - 1; i >= 0; i--) {
		MapRegionClass * region = MapRegionClass::MapRegions[i];
		if (region != NULL) {
			delete region;
		}
	}

	MapRegionClass::TotalCount = 0;
	Map.Overpass(true);
	DebugString("RMG: Compute Radar Image\n");
	Map.Set_Local_Dimensions(Map.LocalRect);
	Map.Compute_Radar_Image();
}

// FUNCTION: GAMEMD 0x0059A670
void MapGeneratorClass::Update_Progress(int percent_progress)
{
	if (Scen->IsReadingScenario) {
		Session.Update_Progress(percent_progress + 99);
	} else {
		Progress.Set_Progress_Percent(0, percent_progress);
	}
}

// FUNCTION: GAMEMD 0x0059A6C0
void MapGeneratorClass::Seed_Land(void)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		cptr->ITType = IsometricTileTypeClass::WaterSet;
		cptr = Map.Iterate();
	}

	switch (SeedData.MapType) {
		case 0:
			Seed_Land_Type0();
			break;
		case 1:
			Seed_Land_Type1();
			break;
		case 2:
			Seed_Land_Type2();
			break;
	}

	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr != NULL) {
		if (cptr->ITType == IsometricTileTypeClass::WaterSet) {
			bool inland = true;
			for (int facing = FACING_N; facing < FACING_COUNT; facing += 2) {
				if (!cptr->Adjacent_Cell((FacingType)facing).Is_Tile_Clear()) {
					inland = false;
				}
			}
			if (inland) {
				cptr->ITType = ISOTILE_CLEAR;
			}
		}
		cptr = Map.Iterate();
	}

	Map.FUN_0057A0C0(0, 0);

	if (RMGCellData != NULL) {
		int grid_size = MapCellStride * MapCellStride;
		for (int i = 0; i < grid_size; i++) {
			MapRegionClass::Get_Cell_Data(i).RegionID = -1;
			MapRegionClass::Get_Cell_Data(i).SpreadID = -1;
		}
	}

	for (int i = MapRegionClass::MapRegions.Count() - 1; i >= 0; i--) {
		MapRegionClass * region = MapRegionClass::MapRegions[i];
		if (region != NULL) {
			delete region;
		}
	}

	MapRegionClass::TotalCount = 0;

	Green_Shores();
}

// FUNCTION: GAMEMD 0x0059A8F0
void MapGeneratorClass::Partition_Rect(DynamicVectorClass<Rect> & rects, int count, Rect const & area)
{
	int root = (int)fastmath::sqrt((double)count);
	int cols = (root * root == count) ? root : root + 1;
	int rows = (cols * root >= count) ? root : cols;
	int cell_w = area.Width / cols;
	int cell_h = area.Height / cols;
	int step_x = 0;
	int step_y = 0;
	int skip_x = 0;
	int skip_y = 0;

	if (Random_Fraction() < 0.5) {
		cell_w = area.Width / rows;
		skip_y = cell_h;
		step_x = cell_w;
	} else {
		cell_h = area.Height / rows;
		skip_x = cell_w;
		step_y = cell_h;
	}

	int empty = rows * cols - count;
	Point2D pos = area.Top_Left();

	DynamicVectorClass<int> lengths;
	DynamicVectorClass<int> open;
	for (int i = 0; i < rows; i++) {
		lengths.Add(cols);
		open.Add(i);
	}

	for (int n = 0; n < empty; n++) {
		unsigned int last = open.Count() - 1;
		unsigned int pick;
		do {
			pick = (unsigned int)(RMG_RANDOM_DOUBLE(last + 1));
		} while (pick > last);
		lengths[open[pick]] = root;
		open.Delete_Index(pick);
	}

	for (int row = 0; row < rows; row++) {
		int length = lengths[row];
		if (length < cols) {
			if (skip_x > 0) {
				pos.X += cell_w / 2;
			} else {
				pos.Y += cell_h / 2;
			}
		}
		for (int j = 0; j < length; j++) {
			rects.Add(Rect(pos.X + 2, pos.Y + 2, cell_w - 4, cell_h - 4));
			pos.X += skip_x;
			pos.Y += skip_y;
		}
		pos.X += step_x;
		pos.Y += step_y;
		if (skip_x > 0) {
			pos.X = area.X;
		} else {
			pos.Y = area.Y;
		}
	}
}

static inline double Random_Scale(void)
{
	return RMG_RANDOM_DOUBLE2(0.05) + 0.45;
}

static inline int Double_Area(Rect const & rect)
{
	return rect.Width * rect.Height * 2;
}

static inline int Double_Area(int width, int height)
{
	return width * height * 2;
}

// FUNCTION: GAMEMD 0x0059B6D0
void MapGeneratorClass::Green_Shores(void)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		if (cptr->Is_Tile_Shore()) {
			for (int facing = FACING_N; facing < FACING_COUNT; facing += 2) {
				CellClass * adjacent = &cptr->Adjacent_Cell((FacingType)facing);
				if (adjacent->Is_Tile_Clear()) {
					adjacent->ITType = IsometricTileTypeClass::GreenTile;
				}
			}
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0059B740
void MapGeneratorClass::Scatter_Green_Tiles(void)
{
	DynamicVectorClass<CellClass *> candidates;
	candidates.Set_Growth_Step(10000);

	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		if (cptr->Is_Tile_Clear_To_Green_LAT()) {
			for (int facing = FACING_N; facing < FACING_COUNT; facing += 2) {
				CellClass * adjacent = &cptr->Adjacent_Cell((FacingType)facing);
				if (adjacent->Is_Tile_Clear()) {
					candidates.Add(adjacent);
				}
			}
		}
		cptr = Map.Iterate();
	}

	int passes = candidates.Count() / 3;
	if (passes > 1000) {
		passes = 1000;
	}
	for (int pass = passes; pass > 0; pass--) {
		int index = Pick_Random_UInt(0, candidates.Count() - 1);
		CellClass * chosen = candidates[index];
		candidates.Delete_Index(index);
		chosen->ITType = IsometricTileTypeClass::GreenTile;
		for (int facing = FACING_N; facing < FACING_COUNT; facing += 2) {
			CellClass * adjacent = &chosen->Adjacent_Cell((FacingType)facing);
			if (adjacent->Is_Tile_Clear()) {
				candidates.Add(adjacent);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0059B940
double MapGeneratorClass::FUN_0059B940(Cell const & from, Cell const & to, Rect const & rect, Cell const & center, float weight_x, float weight_y)
{
	if (!(rect == RECT_NONE)) {
		int u = abs(((to.X - to.Y) >> 1) - ((center.X - center.Y) >> 1));
		int v = abs(((to.X + to.Y) >> 1) - ((center.X + center.Y) >> 1));
		if (u == 0 && v == 0) {
			static const float zero = 0.0f;
			return zero;
		}
		double length = fastmath::sqrt((double)(v * v + u * u));
		double ratio_u = u / length;
		double ratio_v = v / length;
		int x = from.X - to.X;
		int y = from.Y - to.Y;
		double steps = MAX(abs(x), abs(y));
		return (weight_x * ratio_u + ratio_v * weight_y) * steps;
	}
	int dx = from.X - to.X;
	int dy = from.Y - to.Y;
	return fastmath::sqrt((double)(dy * dy + dx * dx));
}

// FUNCTION: GAMEMD 0x0059BAB0
bool MapGeneratorClass::FUN_0059BAB0(Cell const & cell, Rect const & rect, bool flag, double scale_x, double scale_y)
{
	if (rect == RECT_NONE) {
		return true;
	}
	if (flag) {
		int a = cell.X - rect.X * 2 - cell.Y + Map.PlayRect.Width - 1;
		int b = cell.X - rect.Y * 2 - Map.PlayRect.Width + cell.Y - 1;
		double u = a * 0.5 - rect.Width * 0.5;
		double v = b * 0.5 - rect.Height * 0.5;
		return (u * u) * scale_x + (v * v) * scale_y < 1.0;
	}
	return rect.Is_Point_Within(Point2D(cell.X, cell.Y));
}

// FUNCTION: GAMEMD 0x0059C530
void MapGeneratorClass::Clear_Shore_Tiles(void)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		if (cptr->ITType >= IsometricTileTypeClass::ShorePieces && cptr->ITType < IsometricTileTypeClass::ShorePieces + 42) {
			cptr->ITType = ISOTILE_CLEAR;
			cptr->SubTile = 0;
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0059C580
void MapGeneratorClass::Seed_Water(void)
{
	WorkingRegionID++;

	bool river_seeded;
	bool lake_seeded;
	int i;
	int j;

	if ((SeedData.MapType == 3 || SeedData.MapType == 4) && SeedData.WaterAmount > 20) {
		i = 0;
		river_seeded = false;
		while (!river_seeded && i < 10) {
			river_seeded = Seed_River(Cell(0, 0), 0.0, 0);
			i++;
		}
		if (river_seeded) {
			WorkingRegionID++;
		}
	}

	j = 0;
	lake_seeded = false;
	while (!lake_seeded && j < 10) {
		lake_seeded = Seed_Lake(Cell(0, 0));
		j++;
	}
	if (lake_seeded) {
		WorkingRegionID++;
	}
}

// FUNCTION: GAMEMD 0x0059C630
void MapGeneratorClass::Vary_Water_Tiles(void)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		if (cptr->ITType == IsometricTileTypeClass::WaterSet && cptr->SubTile == 0) {
			CellClass * east = &cptr->Adjacent_Cell(FACING_E);
			CellClass * south = &cptr->Adjacent_Cell(FACING_S);
			CellClass * southeast = &cptr->Adjacent_Cell(FACING_SE);
			if (east->ITType == IsometricTileTypeClass::WaterSet && east->SubTile == 0
				&& south->ITType == IsometricTileTypeClass::WaterSet && south->SubTile == 0
				&& southeast->ITType == IsometricTileTypeClass::WaterSet && southeast->SubTile == 0
				&& Pick_Random_UInt(1, 10) != 1) {
				int pick = Pick_Random_UInt(0, 241);
				int tile_index;
				if (pick >= 240) {
					tile_index = 247 - pick;
				} else {
					tile_index = pick / 40;
				}
				MapGeneratorClass::Place_Tile((IsometricTileType)(IsometricTileTypeClass::WaterSet + tile_index), cptr->CellID, MapRegionClass::CellData::Get_Region(cptr->CellID), -1);
			} else {
				cptr->ITType = (IsometricTileType)(IsometricTileTypeClass::WaterSet + 8 + Pick_Random_UInt(0, 200) / 40);
			}
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0059C800
bool MapGeneratorClass::Green_Region_Tiles(int region_id)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		if (MapRegionClass::CellData::Get_Region(cptr->CellID) == region_id) {
			if (cptr->ITType == ISOTILE_CLEAR || cptr->ITType == 0xFFFF) {
				cptr->ITType = IsometricTileTypeClass::GreenTile;
			}
		}
		cptr = Map.Iterate();
	}
	return true;
}

// FUNCTION: GAMEMD 0x0059C880
void MapGeneratorClass::Clear_Region_Cells(int id, int new_id, int cell_height, IsometricTileType ittype)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(cptr->Fetch_CellID());
		if (data.RegionID == id) {
			data.RegionID = new_id;
			data.CanExpand = false;
			cptr->ITType = ittype;
			cptr->SubTile = 0;
			cptr->Height = cell_height == -1 ? CellHeight : cell_height;
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0059D440
int MapGeneratorClass::Get_Target_Water_Amount(void)
{
	if (SeedData.WaterAmount == 0) {
		return(0);
	}
	return(0.008 * SeedData.WaterAmount * LocalWidth * LocalHeight + 100.0);
}

// FUNCTION: GAMEMD 0x0059D480
double MapGeneratorClass::Get_Spread_Score(const Cell & cell1, const Cell & cell2, int spread_step) const
{
	static const float scale = 0.5;
	static const float _random_range = 10.0f;
	static const float _step_weight = 0.02f;

	int x = cell1.X - cell2.X;
	int y = cell1.Y - cell2.Y;
	float dist = fastmath::sqrt(x * x + y * y);

	float random_value = _random_range * (float)Random_Fraction();
	float step_term = (spread_step * _step_weight);
	float dist_term = (dist * scale);

	return(random_value + dist_term - step_term);
}

// FUNCTION: GAMEMD 0x0059E640
void MapGeneratorClass::Set_Height_Of_ID(int id, char cell_height)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		if (MapRegionClass::CellData::Get_Region(cptr->Fetch_CellID()) == id) {
			cptr->Height = cell_height;
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0059E6C0
void MapGeneratorClass::Increase_Height_Of_ID(int id, char cell_height)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		if (MapRegionClass::CellData::Get_Region(cptr->Fetch_CellID()) == id) {
			cptr->Height += cell_height;
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x005A0090
void MapGeneratorClass::Set_Cell_Data_Region(Cell const &cell, int region_id)
{
	MapRegionClass::CellData::Set_Region(cell, region_id);
}

// FUNCTION: GAMEMD 0x005A00C0
int MapGeneratorClass::Get_Cell_Data_Region(Cell const &cell)
{
	return(MapRegionClass::CellData::Get_Region(cell));
}

// FUNCTION: GAMEMD 0x005A0100
void MapGeneratorClass::Set_Cell_Data_Spread(Cell const &cell, int patch_id)
{
	MapRegionClass::CellData::Set_Spread(cell, patch_id);
}

// FUNCTION: GAMEMD 0x005A0130
int MapGeneratorClass::Get_Cell_Data_Spread(Cell const &cell)
{
	return(MapRegionClass::CellData::Get_Spread(cell));
}

// FUNCTION: GAMEMD 0x005A0160
bool MapGeneratorClass::Grow_Region(int id, int rings, Rect rect, char set_height, char height)
{
	DynamicVectorClass<Cell> * cells = Build_Region_Border_Cell_List(id);

	DynamicVectorClass<Cell> * ncells;
	for (int i = 0; i < rings; i++) {
		ncells = new DynamicVectorClass<Cell>;
		ncells->Set_Growth_Step(3 * cells->Count());

		for (int j = 0; j < cells->Count(); j++) {
			for (int dir = 0; dir < FACING_COUNT; dir++) {
				Cell ncell = Adjacent_Cell((*cells)[j], (FacingType)dir);
				if (My_In_Radar(ncell) && rect.Is_Point_Within(Point2D(ncell.X, ncell.Y))) {
					MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(ncell);
					int rid = data.RegionID;
					bool reclaim = set_height && rid == id - 1;
					CellClass * cptr = &Map[ncell];
					if ((rid == 0 || reclaim) && (cptr->Is_Tile_Clear() || (reclaim && cptr->Is_Tile_With_Water()))) {
						ncells->Add(ncell);
						data.RegionID = id;
						if (set_height && cptr->Is_Tile_Clear()) {
							Map[ncell].Height = height;
						}
					} else if (rid != id) {
						delete cells;
						delete ncells;
						return(false);
					}
				}
			}
		}

		delete cells;
		cells = ncells;
	}

	delete cells;
	return(true);
}

// FUNCTION: GAMEMD 0x005A0410
bool MapGeneratorClass::Clear_Region_Border(int id, int rings, int new_id)
{
	int i;
	int j;

	DynamicVectorClass<Cell> * cells = Build_Region_Border_Cell_List(id);

	if (rings > 1) {
		Map.Reset_Iterator();
		CellClass *cptr = Map.Iterate();
		while (cptr != NULL) {
			MapRegionClass::Get_Cell_Data(cptr->Fetch_CellID()).SpreadID = 0;
			cptr = Map.Iterate();
		}
	}

	for (i = 0; i < rings; i++) {

		for (j = cells->Count() - 1; j >= 0; j--) {
			Cell c = (*cells)[j];
			MapRegionClass::CellData::Set_Region(c, new_id);
			CellClass *cptr = &Map[c];
			cptr->ITType = ISOTILE_CLEAR;
			cptr->SubTile = 0;
			cptr->Height = CellHeight;
		}

		if (i < rings - 1) {
			DynamicVectorClass<Cell> * ncells = new DynamicVectorClass<Cell>;
			ncells->Set_Growth_Step(cells->Count());

			for (j = cells->Count() - 1; j >= 0; j--) {
				Cell c = (*cells)[j];
				for (int dir = 0; dir < FACING_COUNT; dir++) {
					Cell ncell = Adjacent_Cell(c, (FacingType)dir);
					if (My_In_Radar(ncell)) {
						MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(ncell);
						if (data.RegionID == id && data.SpreadID != i + 1) {
							ncells->Add(ncell);
							data.SpreadID = i + 1;
						}
					}
				}
			}

			delete cells;
			cells = ncells;
		}
	}

	delete cells;

	if (rings > 1) {
		Map.Reset_Iterator();
		CellClass *cptr = Map.Iterate();
		while (cptr != NULL) {
			MapRegionClass::Get_Cell_Data(cptr->Fetch_CellID()).SpreadID = 0;
			cptr = Map.Iterate();
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x005A0700
DynamicVectorClass<Cell> *MapGeneratorClass::Build_Region_Border_Cell_List(int id)
{
	DynamicVectorClass<Cell> *cells = new DynamicVectorClass<Cell>;
	cells->Set_Growth_Step(LocalWidth * LocalHeight);

	int grid_size = MapCellStride * MapCellStride;
	for (int i = 0; i < grid_size; i++) {
		Cell cell = RMGCellData[i].CellID;
		if (cell != Cell(0, 0) && MapRegionClass::CellData::Get_Region(cell) == id) {
			bool add = false;
			for (FacingType facing = FACING_FIRST; facing < FACING_COUNT; facing++) {
				Cell ncell = Adjacent_Cell(cell, facing);
				if (My_In_Radar(ncell)) {
					if (MapRegionClass::CellData::Get_Region(ncell) != id) {
						add = true;
						break;
					}
				}
			}

			if (add) {
				cells->Add(cell);
			}
		}
	}

	return(cells);
}

// FUNCTION: GAMEMD 0x005A1190
double MapGeneratorClass::Get_Angle_Score(const Cell & cell1, const Cell & cell2, double ref_angle)
{
	static const double _random_weight = 2.0;
	static const double _angle_weight = 1.5;

	int dx = cell2.X - cell1.X;
	int dy = cell2.Y - cell1.Y;

	double angle = (dx != 0) ? fastmath::atan(-((double)dy / (double)dx)) : M_PI_2;
	if (dx < 0) {
		angle += M_PI;
	}

	double angle_diff = fabs(angle - ref_angle);
	while (angle_diff >= M_PI * 2) {
		angle_diff -= M_PI * 2;
	}
	if (angle_diff > M_PI) {
		angle_diff = M_PI * 2 - angle_diff;
	}

	return(RMG_RANDOM_DOUBLE2(_random_weight) + _angle_weight * angle_diff);
}

// FUNCTION: GAMEMD 0x005A1270
void MapGeneratorClass::Clear_Cell_Data_Spreads(void)
{
	Map.Reset_Iterator();
	CellClass *cptr = Map.Iterate();
	while (cptr != NULL) {
		Set_Cell_Data_Spread(cptr->Fetch_CellID(), 0);
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x005A12D0
void MapGeneratorClass::Increase_Height_Except_ID(int cell_height, int id)
{
	Map.Reset_Iterator();
	CellClass *cptr = Map.Iterate();
	while (cptr != NULL) {
		int rid = Get_Cell_Data_Region(cptr->Fetch_CellID());
		if (rid != id) {
			cptr->Height += cell_height;
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x005A1350
IsometricTileType MapGeneratorClass::Pick_Alternate_Cliff(IsometricTileType type)
{
	int cliff = type - IsometricTileTypeClass::CliffSet;
	switch (cliff) {
		case 4:
			cliff = 4 + Pick_Random_UInt(1, 2);
			break;
		case 5:
			cliff = 4 + Pick_Random_UInt(0, 1) * 2;
			break;
		case 6:
			cliff = 4 + Pick_Random_UInt(0, 1);
			break;
		case 8:
			cliff = 8 + Pick_Random_UInt(1, 2);
			break;
		case 9:
			cliff = 8 + Pick_Random_UInt(0, 1) * 2;
			break;
		case 10:
			cliff = 8 + Pick_Random_UInt(0, 1);
			break;
		case 11:
			cliff = 11 + Pick_Random_UInt(1, 2);
			break;
		case 12:
			cliff = 11 + Pick_Random_UInt(0, 1) * 2;
			break;
		case 13:
			cliff = 11 + Pick_Random_UInt(0, 1);
			break;
		case 14:
			cliff = 14 + Pick_Random_UInt(1, 2);
			break;
		case 15:
			cliff = 14 + Pick_Random_UInt(0, 1) * 2;
			break;
		case 16:
			cliff = 14 + Pick_Random_UInt(0, 1);
			break;
		case 22:
			cliff = 22 + Pick_Random_UInt(1, 2);
			break;
		case 23:
			cliff = 22 + Pick_Random_UInt(0, 1) * 2;
			break;
		case 24:
			cliff = 22 + Pick_Random_UInt(0, 1);
			break;
		case 28:
			cliff = 29;
			break;
		case 29:
			cliff = 28;
			break;
		case 34:
			cliff = 34 + Pick_Random_UInt(1, 2);
			break;
		case 35:
			cliff = 34 + Pick_Random_UInt(0, 1) * 2;
			break;
		case 36:
			cliff = 34 + Pick_Random_UInt(0, 1);
			break;
	}
	return (IsometricTileType)(IsometricTileTypeClass::CliffSet + cliff);
}

// FUNCTION: GAMEMD 0x005A19E0
void MapGeneratorClass::Fixup_Cliff_Corners(void)
{
	Map.Reset_Iterator();
	CellClass *cptr = Map.Iterate();
	while (cptr != NULL) {
		CellClass *c5 = &cptr->Adjacent_Cell((FacingType)5);
		CellClass *c1 = &cptr->Adjacent_Cell((FacingType)1);
		CellClass *c3 = &cptr->Adjacent_Cell((FacingType)3);
		CellClass *c2 = &cptr->Adjacent_Cell((FacingType)2);
		CellClass *c6 = &cptr->Adjacent_Cell((FacingType)6);

		int mask = Map.Get_High_Ground_Mask(cptr);
		if (mask == 0x83) {
			if (Map.Get_High_Ground_Mask(c3) == 0x83 && c2->Is_Tile_Clear() && Pick_Random_UInt(0, 1) == 0) {
				c2->Height -= 4;
				Set_Cell_Data_Region(c2->CellID, Get_Cell_Data_Region(cptr->CellID));
			}
		} else if (mask == 0x38) {
			if (Map.Get_High_Ground_Mask(c3) == 0x38 && c6->Is_Tile_Clear() && Pick_Random_UInt(0, 1) == 0) {
				c6->Height -= 4;
				Set_Cell_Data_Region(c6->CellID, Get_Cell_Data_Region(cptr->CellID));
			}
		} else if (mask == 0xe0) {
			if (Map.Get_High_Ground_Mask(c5) == 0xe0 && Map.Get_High_Ground_Mask(c1) == 0xe0 && cptr->Is_Tile_Clear()) {
				cptr->Height += 4;
				Set_Cell_Data_Region(cptr->CellID, Get_Cell_Data_Region(c6->CellID));
			}
		}

		cptr = Map.Iterate();
	}
}

static inline bool Is_Special_Map_Type(MapSeedClass const & seed)
{
	return(seed.MapType == 3 || seed.MapType == 4);
}

// FUNCTION: GAMEMD 0x005A1C70
void MapGeneratorClass::Init_Regions(void)
{
	int i;

	MapRegionClass::Destroy_Cell_Regions();
	MapRegionClass::Create_Water_Regions();

	for (i = 0; i < MapRegionClass::MapRegions.Count(); i++) {
		MapRegionClass *ptr = MapRegionClass::MapRegions[i];
		if (ptr->ContainsWater) {
			ptr->Grow(4 + (ptr->CellCount > 8000) + (!Is_Special_Map_Type(SeedData)));
			ptr->Claim_Expandable_Cells();
		}
	}

	MapRegionClass::Create_Land_Regions(false);
}

// FUNCTION: GAMEMD 0x005A1E10
void MapGeneratorClass::Make_Regions(void)
{
	if (SeedData.MapType == 4 || SeedData.MapType == 3) {
		MapRegionClass::Make_Ice_Regions();
		MapRegionClass::Make_Regions();
		Fixup_Cliff_Corners();
		Map.Build_All_Cliffs(0, -1);
		Fixup_Cliff_Edges();
	}
}

// FUNCTION: GAMEMD 0x005A1E50
bool MapGeneratorClass::Region_Cell_Count_At_Least(Rect const & rect, int region_id, int, int count)
{
	Cell cell(rect.X, rect.Y);
	int actual = 0;

	for (int y = 0; y < rect.Height; y++) {
		for (int x = 0; x < rect.Width; x++) {
			if (My_In_Radar(cell + Cell(x, y))) {
				if (MapRegionClass::CellData::Get_Region(cell + Cell(x, y)) == region_id) {
					actual++;
					if (actual >= count) {
						return(true);
					}
				}
			}
		}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x005A1F60
bool MapGeneratorClass::Expand_All_High_Ground(void)
{
	bool success = true;
	Map.SetActiveFoundation(NULL);
	Map.Reset_Iterator();
	CellClass *cptr = cptr = Map.Iterate();
	while (cptr != NULL) {
		if (!success) {
			break;
		}
		success = Map.Expand_High_Ground(cptr, -1);
		cptr = Map.Iterate();
	}
	return(success);
}

// FUNCTION: GAMEMD 0x005A1FB0
bool MapGeneratorClass::Init_Start_Points(void)
{
	int player_index = 0;

	if (SeedData.NumPlayers > 0) {
		do {
			Cell seed = Scen->Get_Waypoint_Cell((WAYPOINT)player_index);

			CellNode * nodes = new CellNode[800];
			PriorityQueueClass<CellNode> * queue = new PriorityQueueClass<CellNode>(800);

			Clear_Cell_Data_Spreads();
			int spread_count = 0;
			player_index++;
			queue->Clear();

			int node_count = 1;
			nodes[0].Element = seed;
			nodes[0].Score = 0.0f;
			MapRegionClass::Get_Cell_Data(seed).SpreadID = player_index;
			queue->Insert(nodes[0]);

			CellNode * node = queue->Extract_Min();
			while (node != NULL && spread_count < 400) {
				MapRegionClass::Get_Cell_Data(node->Element).Inviolate = true;

				FacingType dir = FACING_FIRST;
				CellNode * newnode = &nodes[node_count];
				for (; dir < FACING_COUNT; dir++) {
					Cell ncell = Adjacent_Cell(node->Element, dir);
					if (My_In_Radar(ncell) && MapRegionClass::Get_Cell_Data(ncell).SpreadID == 0) {
						if (Map[ncell].Is_Tile_Clear()) {
							newnode->Element = ncell;
							int dx = ncell.X - seed.X;
							int dy = ncell.Y - seed.Y;
							newnode->Score = fastmath::sqrt((double)(dx * dx + dy * dy));
							MapRegionClass::Get_Cell_Data(ncell).SpreadID = player_index;
							node_count++;
							queue->Insert(*newnode++);
						}
					}
				}

				spread_count++;
				node = queue->Extract_Min();
			}

			delete[] nodes;
			delete queue;

		} while (player_index < SeedData.NumPlayers);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005A28C0
void MapGeneratorClass::Create_Tiberium_Patch(Cell const &cell, int count, int patch_id, bool use_gems)
{
	int marker_index = 0;
	CellNode *nodes = new CellNode[10 * count];
	PriorityQueueClass<CellNode> *queue = new PriorityQueueClass<CellNode>(10 * count);

	bool queue_was_reset_for_spread = false;
	Cell spread_origin = cell;
	int placed_count = 0;
	int restart_count = 0;
	int overlay_base = use_gems ? 27 : 102;

	queue->Clear();
	CellNode *node = NULL;

	while (placed_count < count) {
		if (restart_count >= 10) {
			break;
		}

		if (node == NULL) {
			queue->Clear();
			Clear_Cell_Data_Spreads();

			marker_index = 0;
			nodes[marker_index].Element = cell;
			nodes[marker_index].Score = 0;
			marker_index++;
			Set_Cell_Data_Spread(cell, patch_id);
			queue->Insert(nodes[0]);
			node = queue->Extract_Min();
			queue_was_reset_for_spread = false;
			restart_count++;
			spread_origin = cell;
		}

		CellClass *cellptr = &Map[node->Element];
		if (MapRegionClass::CellData::Is_Not_Inviolate(node->Element)) {
			if (!queue_was_reset_for_spread) {
				spread_origin = node->Element;
				queue_was_reset_for_spread = true;
				queue->Clear();
				marker_index = 0;

				if (!use_gems) {
					char name[20];
					sprintf(name, "TIBTRE0%d", (int)Pick_Random_UInt(1, 3) % 10);
					new TerrainClass(TerrainTypes[TerrainTypeClass::From_Name(name)], spread_origin);
				}
			}

			if (cellptr->Overlay == OVERLAY_NONE) {
				cellptr->Overlay = OverlayType(Pick_Random_UInt(0, 11) + overlay_base);
				placed_count++;
			} else if (cellptr->OverlayData < 11) {
				cellptr->OverlayData++;
				placed_count++;
			}
		}

		CellNode *newnode = &nodes[marker_index];
		for (int dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
			Cell newcell = Adjacent_Cell(node->Element, (FacingType)dir);
			if (Map.In_Local_Radar(newcell, true)) {
				CellClass *newcellptr = &Map[newcell];
				if (newcellptr->Is_Tile_Clear()) {
					if ((newcellptr->Overlay == OVERLAY_NONE && Get_Cell_Data_Spread(newcell) != patch_id)
							|| (newcellptr->OverlayData < 11 && newcellptr->Tiberium_Type_Here() != TIBERIUM_NONE)) {
						newnode->Element = newcell;
						newnode->Score = Get_Tiberium_Score(spread_origin, newcell);
						Set_Cell_Data_Spread(newcell, patch_id);
						marker_index++;
						queue->Insert(*newnode++);
					}
				}
			}
		}

		node = queue->Extract_Min();
	}

	delete[] nodes;
	delete queue;
}

// FUNCTION: GAMEMD 0x005A2ED0
double MapGeneratorClass::Get_Tiberium_Score(const Cell & cell1, const Cell & cell2) const
{
	static const double _random_range = 5.0;

	int x = cell1.X - cell2.X;
	int y = cell1.Y - cell2.Y;
	double dist = fastmath::sqrt(x * x + y * y);
	return(RMG_RANDOM_DOUBLE(_random_range) + dist);
}

// FUNCTION: GAMEMD 0x005A2F50
void MapGeneratorClass::Seed_Hill_Heights(void)
{
	static const double scale = 0.025;

	int i;
	int grid_size = MapCellStride * MapCellStride;
	double height_step = SeedData.Hills * 0.0001 + 0.1;
	double min_volatility = SeedData.Hills * 0.0025;
	double max_volatility = SeedData.Hills * 0.005;

	if (min_volatility >= scale) {
		for (i = 0; i < grid_size; i++) {
			MapRegionClass::CellData * pdata = &MapRegionClass::Get_Cell_Data(i);
			MapRegionClass::CellData & data = *pdata;
			Cell cell = data.CellID;
			if (cell.X != 0 || cell.Y != 0) {
				data.HillVolatility = 0;
				double up_height = 0.0;
				double height_trend = 0.0;

				if (My_In_Radar(cell - Cell(1, 1))) {
					up_height = MapRegionClass::Get_Cell_Data(cell - Cell(1, 1)).HillHeight;
				}

				Cell newcell = cell - Cell(1, 0);
				if (My_In_Radar(newcell)) {
					MapRegionClass::CellData & data2 = MapRegionClass::Get_Cell_Data(newcell);
					pdata->HillHeight += data2.HillHeight;
					data.HillVolatility = data2.HillVolatility;
					height_trend += data2.HillHeight - up_height;
				} else {
					pdata->HillHeight = data.HillHeight;
					data.HillVolatility = min_volatility;
				}

				newcell = cell - Cell(0, 1);
				if (My_In_Radar(newcell)) {
					MapRegionClass::CellData & data2 = MapRegionClass::Get_Cell_Data(newcell);
					double height2 = data2.HillHeight;
					data.HillHeight = height2 + data.HillHeight;
					data.HillVolatility = data2.HillVolatility + data.HillVolatility;
					height_trend += data2.HillHeight - up_height;
				} else {
					pdata->HillHeight = data.HillHeight;
					data.HillVolatility = min_volatility + data.HillVolatility;
				}

				data.HillHeight *= 0.5;
				data.HillVolatility *= 0.5;
				if (height_trend > 0.0) {
					height_trend = height_step;
				} else if (height_trend < 0.0) {
					height_trend = -height_step;
				}
				if (data.Inviolate) {
					data.HillHeight = (data.HillHeight <= 0.0) ? 0.0 : data.HillHeight;
					data.HillVolatility = 0.0025;
				}
				const double mean = 0.0;
				data.HillVolatility += Sample_Truncated_Normal_Inline(mean, scale, -data.HillVolatility, max_volatility - data.HillVolatility);

				double min_height = -2.0 - data.HillHeight;
				double max_height = 2.0 - data.HillHeight;
				double trend;
				if (height_trend < min_height) {
					trend = min_height;
				} else if (height_trend > max_height) {
					trend = max_height;
				} else {
					trend = height_trend;
				}
				data.HillHeight += Sample_Truncated_Normal_Inline(trend, data.HillVolatility, min_height, max_height);
			}
		}

		for (i = 0; i < grid_size; i++) {
			MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(i);
			if (data.HillHeight < 0) {
				data.HillHeight = (int)(data.HillHeight - 0.5);
			} else if (data.HillHeight > 0) {
				data.HillHeight = (int)(data.HillHeight + 0.5);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005A33F0
void MapGeneratorClass::Seed_Hill_Anchors(void)
{
	Map.Reset_Iterator();

	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		if (cptr->Is_Tile_Shore()) {
			for (int dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
				Cell cell = Adjacent_Cell(cptr->Fetch_CellID(), (FacingType)dir);
				if (My_In_Radar(cell) && Map[cell].Is_Tile_Clear()) {
					MapRegionClass::Get_Cell_Data(cell).HillHeight = 0.5;
					MapRegionClass::Get_Cell_Data(cell).Inviolate = true;
					break;
				}
			}
		} else if (cptr->Is_Tile_Cliff()) {
			for (int dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
				Cell cell = Adjacent_Cell(cptr->Fetch_CellID(), (FacingType)dir);
				if (My_In_Radar(cell) && Map[cell].Is_Tile_Clear()) {
					MapRegionClass::Get_Cell_Data(cell).Inviolate = true;
					break;
				}
			}
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x005A35F0
void MapGeneratorClass::Generate_Hills(void)
{
	Seed_Hill_Anchors();
	Seed_Hill_Heights();

	Init_Deform_Grid_RMG();

	int grid_size = MapCellStride * MapCellStride;
	for (int i = 0; i < grid_size; i++) {
		MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(i);
		if (data.CellID != Cell(0, 0)) {
			CellClass * cellptr = &Map[data.CellID];
			int change = cellptr->Height + data.HillHeight - Get_Deformed_Cell_Height(cellptr);
			int steps = abs(change);
			int direction = change < 0 ? -1 : 1;
			while (steps > 0) {
				Deform_Cell_RMG(data.CellID, direction, 0, Get_Deformable_Cell_Corners(cellptr, direction));
				steps--;
			}
		}
	}

	Commit_Deform_Grid_RMG();

	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();

	Cell offsets[] = { Cell(0, 0), Cell(1, 0), Cell(1, 1), Cell(0, 1)};
	RampType ramps[] = { RAMP_MID_SE, RAMP_MID_SW, RAMP_MID_NW, RAMP_MID_NE };

	while (cptr != NULL) {
		int height_step = 0;
		bool raise = true;
		if (cptr->Ramp == RAMP_CORNER_NW) {
			for (int i = 0; i < ARRAY_SIZE(offsets); i++) {
				if (Map[cptr->Fetch_CellID() + offsets[i]].Ramp != RAMP_CORNER_NW + i) {
					raise = false;
				}
			}
		} else if (cptr->Ramp == RAMP_MID_SE) {
			height_step += 1;
			for (int i = 0; i < ARRAY_SIZE(offsets); i++) {
				if (Map[cptr->Fetch_CellID() + offsets[i]].Ramp != ramps[i]) {
					raise = false;
				}
			}
		} else {
			raise = false;
		}

		if (raise) {
			for (int i = 0; i < ARRAY_SIZE(offsets); i++) {
				CellClass * cellptr = &Map[cptr->Fetch_CellID() + offsets[i]];
				cellptr->ITType = ISOTILE_NONE;
				cellptr->SubTile = 0;
				cellptr->Ramp = RAMP_NONE;
				cellptr->Height += height_step;
			}
		}

		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x005A38C0
void MapGeneratorClass::Seed_Vegetation(void)
{
	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();

	static const double rough = 0.005;
	static const double green = 0.02;
	static const double sand = 0.005;
	double density = SeedData.Vegetation * 0.01;

	while (cptr != NULL) {
		MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(cptr->Fetch_CellID());
		if (cptr->Is_Tile_Shore()) {
			for (int x = cptr->Fetch_CellID().X - 2; x < cptr->Fetch_CellID().X + 3; x++) {
				for (int y = cptr->Fetch_CellID().Y - 2; y < cptr->Fetch_CellID().Y + 3; y++) {
					if (My_In_Radar(Cell(x, y))) {
						data.RoughTileChance = rough;
						data.GreenTileChance = density * green * 10.0;
						data.SandTileChance = 0;
					}
				}
			}
		} else if (data.RoughTileChance == 0.0) {
			data.RoughTileChance = rough;
			data.GreenTileChance = density * green;
			data.SandTileChance = sand;
		}
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x005A3A60
void MapGeneratorClass::Seed_Arctic_Vegetation(void)
{
	static const double rough = 0.005;
	static const double sand = 0.001;

	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();

	while (cptr != NULL) {
		MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(cptr->Fetch_CellID());
		data.RoughTileChance = rough;
		data.SandTileChance = sand;
		cptr = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x005A4280
void MapGeneratorClass::Generate_Arctic_Vegetation(void)
{
	CellClass * cptr;

	Clear_Cell_Data_Spreads();

	Map.Reset_Iterator();
	cptr = Map.Iterate();

	while (cptr != NULL) {
		MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(cptr->Fetch_CellID());
		if (cptr->Is_Tile_Clear() && cptr->Ramp == RAMP_NONE && !data.Inviolate) {
			Cell cell = cptr->Fetch_CellID();
			int origin_id = cell.X + cell.Y * MAP_CELL_W;
			if ((unsigned int)RMGRandom() * (1.0 / UINT_MAX) < data.RoughTileChance) {
				double patch_size = Sample_Truncated_Normal_Inline(20, 15, 4, 60);
				Place_Tile_Patch(cptr, IsometricTileTypeClass::RoughTile, patch_size, origin_id, false);
			}
		}
		cptr = Map.Iterate();
	}

	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr != NULL) {
		cptr->Fixup_LAT();
		cptr = Map.Iterate();
	}

	int tree_count = (int)((SeedData.Width * 0.1 + 0.7) * (SeedData.Vegetation * 0.01) * MaxTrees);
	for (int tries = 0; tree_count > 0 && tries < 100; tries++) {
		Cell cell = MapRegionClass::Pick_Random_Clear_Map_Cell();
		tree_count -= Place_Forest(&Map[cell], Sample_Truncated_Normal_Inline(25, 10, 10, 35), Sample_Truncated_Normal_Inline(0.2, 0.1, 0.05, 0.4));
	}
}

// FUNCTION: GAMEMD 0x005A4AE0
double MapGeneratorClass::Get_Forest_Score(const Cell & cell1, const Cell & cell2) const
{
	static const double _random_range = 5.0;

	int x = cell1.X - cell2.X;
	int y = cell1.Y - cell2.Y;
	double dist = fastmath::sqrt(x * x + y * y);
	return(RMG_RANDOM_DOUBLE(_random_range) + dist);
}

// FUNCTION: GAMEMD 0x005A4FA0
double MapGeneratorClass::Get_Tile_Patch_Score(const Cell & cell1, const Cell & cell2) const
{
	static const double _random_range = 5.0;

	int x = cell1.X - cell2.X;
	int y = cell1.Y - cell2.Y;
	double dist = fastmath::sqrt(x * x + y * y);
	return(RMG_RANDOM_DOUBLE(_random_range) + dist);
}

void Init_Dirt_Roads(void);
int Dirt_Road_Tile_Index(IsometricTileType ittype);

// FUNCTION: GAMEMD 0x005A5020
void MapGeneratorClass::Generate_Urban_Areas(void)
{
	int count = (SeedData.Cities + 0.32) * 3;
	int tries = 0;
	int placed = 0;

	Init_Dirt_Roads();

	while (true) {
		if (placed >= count) {
			break;
		}
		Cell cell(0, 0);
		int tries_here = 0;
		while (true) {
			cell = MapRegionClass::Pick_Random_Clear_Map_Cell();
			if (++tries_here >= 20) break;
			if (MapRegionClass::CellData::Is_Not_Inviolate(cell) || Is_Area_Cliff_Free(cell)) {
				break;
			}
		}

		if (Random_Fraction() < (1.0 - SeedData.Cities)) {
			if (Generate_Rural_Roads(cell)) {
				Generate_Rural_Buildings(cell);
				Generate_Rural_Units(cell);
				placed++;
			}
		} else {
			if (tries_here != 20) {
				int size = Sample_Truncated_Normal_Inline(2 * Map.PlayRect.Width * Map.PlayRect.Height / 30, 300, 100, 2000);
				DynamicVectorClass<Cell> * cells = Create_Urban_Area(cell, size);
				if (cells != NULL) {
					Generate_Urban_Roads(*cells);
					Generate_Urban_Buildings(*cells);
					Generate_Urban_Units(*cells);
					Generate_Urban_Pavement(*cells);
					placed++;
					delete cells;
				}
			}
			tries++;
		}

		if (tries >= 10) {
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x005A5AE0
double MapGeneratorClass::Get_Urban_Score(const Cell & cell1, const Cell & cell2) const
{
	static const double _random_range = 3.0;

	int x = cell1.X - cell2.X;
	int y = cell1.Y - cell2.Y;
	double dist = MAX(abs(x), abs(y));
	return(RMG_RANDOM_DOUBLE(_random_range) + dist);
}

// FUNCTION: GAMEMD 0x005A6920
void MapGeneratorClass::Generate_Urban_Units(const DynamicVectorClass<Cell> & cells)
{
	HouseClass * neutral = House_From_HousesType(HouseTypeClass::From_Name("Neutral"));
	int count = Pick_Random_UInt(1, 5);

	int tries = 0;
	int placed = 0;

	if (count > 0) {
		while (true) {
			if (tries >= 2 * count) {
				break;
			}

			int unit_index = Pick_Random_UInt(0, ARRAY_SIZE(DAT_0082B180) - 1);
			FootClass * foot = NULL;
			if (unit_index <= 2) {
				foot = new InfantryClass(InfantryTypes[InfantryTypeClass::From_Name(DAT_0082B180[unit_index])], neutral);
			} else {
				foot = new UnitClass(UnitTypes[UnitTypeClass::From_Name(DAT_0082B180[unit_index])], neutral);
			}

			int tries_here = 0;
			bool was_placed = false;
			while (!was_placed) {
				if (tries_here >= 60) {
					break;
				}
				Cell unit_cell = cells[Pick_Random_UInt(0, cells.Count() - 1)];
				if (Map.In_Local_Radar(unit_cell) && Map[unit_cell].Cell_Occupier() == NULL) {
					if (foot->Unlimbo(Coord(unit_cell), (Dir256)(Pick_Random_UInt(0, 7) * 32))) {
						foot->Strength = foot->Techno_Type_Class()->MaxStrength;
						foot->EstimatedHealth = foot->Strength;
						foot->Mark(MARK_CHANGE);
						was_placed = true;
						placed++;
					}
				}
				tries_here++;
			}

			if (!was_placed) {
				delete foot;
			}

			tries++;
			if (placed >= count) {
				break;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005A6BA0
bool MapGeneratorClass::Is_Area_Cliff_Free(const Cell & cell)
{
	int yy = cell.Y;
	int xx = cell.X;

	int xmin = xx - 7;
	int ymin = yy - 7;
	int xmax = xx + 7;
	int ymax = yy + 7;

	for (int x = xmin; x < xmax; x++) {
		for (int y = ymin; y < ymax; y++) {
			if (Map[Cell(x, y)].Is_Tile_Cliff()) {
				return(false);
			}
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005A6C10
void MapGeneratorClass::Place_Tile(IsometricTileType ittype, const Cell & cell, int region, int height)
{
	IsometricTileTypeClass * itype = IsometricTileTypes[ittype];
	IsoTileSet * set = (IsoTileSet *)itype->Get_Image_Data();
	if (set != NULL) {
		for (int y = 0; y < itype->Height; y++) {
			for (int x = 0; x < itype->Width; x++) {
				Cell ncell = cell + Cell(x, y);
				if (My_In_Radar(ncell)) {
					CellClass * cptr = &Map[ncell];
					int subtile = itype->SubTile_Index(x, y);
					IsoTileRecord const * record = set->Fetch_Record_Pointer_Unsafe(subtile);
					if (record != NULL) {
						cptr->ITType = itype->HeapID;
						cptr->SubTile = subtile;
						if (height != -1) {
							cptr->Height = height + record->Height - 4;
						}
						cptr->Ramp = record->RampType;
						MapRegionClass::CellData::Set_Region(ncell, region);
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005A6D80
Cell MapGeneratorClass::Plan_Paved_Road_Junction(Cell const & cell, Rect const & rect, FacingType dir)
{
	Cell wcell = cell;

	int margin_x = 0;
	int margin_y = 0;
	int margin_width = 0;
	int margin_height = 0;
	Rect tile_size;

	bool south = dir == FACING_S;

	if (dir == FACING_E) {
		tile_size = Rect(0, 0, 1, 3);
		margin_y = -3;
		margin_height = 6;
	} else {
		tile_size = Rect(0, 0, 3, 1);
		margin_x = -3;
		margin_width = 6;
	}

	while (true) {
		if (!(wcell.X < rect.X + rect.Width && wcell.Y < rect.Y + rect.Height)) return(Cell(0, 0));
		if (Map[wcell].ITType == IsometricTileTypeClass::PaveTile) {
			break;
		}
		wcell = Adjacent_Cell(wcell, dir);
	}

	while (true) {
		if (!(wcell.X < rect.X + rect.Width && wcell.Y < rect.Y + rect.Height)) return(Cell(0, 0));
		if (Can_Place_Paved_Road(Rect(margin_x + wcell.X, margin_y + wcell.Y, margin_width + tile_size.Width, margin_height + tile_size.Height), south, south)) {
			break;
		}
		wcell = Adjacent_Cell(wcell, dir);
	}

	int length = MAX(wcell.Y - cell.Y, wcell.X - cell.X);
	if (length > 0) {
		int shorten = Pick_Random_UInt(0, length);
		while (shorten > 0) {
			Cell back = Adjacent_Cell(wcell, Facing_Sub(dir, FACING_180));
			if (!Can_Place_Paved_Road(Rect(margin_x + back.X, margin_y + back.Y, margin_width + tile_size.Width, margin_height + tile_size.Height), false, false)) break;
			wcell = back;
			shorten--;
		}
	}

	return(wcell);
}

// FUNCTION: GAMEMD 0x005A7070
Cell MapGeneratorClass::Plan_Paved_Road(Cell const & cell, Rect const & rect, FacingType dir)
{
	bool south = dir == FACING_S;
	Cell wcell = cell;

	int margin_x = 0;
	int margin_y = 0;
	int margin_width = 0;
	int margin_height = 0;
	Rect tile_size;

	if (dir == FACING_E) {
		tile_size = Rect(0, 0, 1, 3);
		margin_y = -3;
		margin_height = 6;
	} else {
		tile_size = Rect(0, 0, 3, 1);
		margin_x = -3;
		margin_width = 6;
	}

	while (true) {
		if (!(wcell.X < rect.X + rect.Width + 1 && wcell.Y < rect.Y + rect.Height + 1)) break;
		if (!Can_Place_Paved_Road(Rect(margin_x + wcell.X, margin_y + wcell.Y, margin_width + tile_size.Width, margin_height + tile_size.Height), south, south)) {
			break;
		}
		{ Cell next = Adjacent_Cell(wcell, dir); wcell = next; }
	}

	wcell = Adjacent_Cell(wcell, Facing_Sub(dir, FACING_180));

	int length = MAX(wcell.X - cell.X, wcell.Y - cell.Y);
	if (length > 6) {
		int shorten = Pick_Random_UInt(0, 3);
		while (shorten > 0) {
			Cell back = Adjacent_Cell(wcell, Facing_Sub(dir, FACING_180));
			shorten--;
			wcell = back;
		}
	}

	return(wcell);
}

// FUNCTION: GAMEMD 0x005A7250
bool MapGeneratorClass::Can_Place_Paved_Road(Rect const & rect, bool overlap_road, bool overlap_road_end)
{
	if (My_In_Radar(Cell(rect.X, rect.Y)) &&
		My_In_Radar(Cell(rect.X + rect.Width - 1, rect.Y)) &&
		My_In_Radar(Cell(rect.X, rect.Y + rect.Height - 1)) &&
		My_In_Radar(Cell(rect.X + rect.Width - 1, rect.Y + rect.Height - 1))) {
		for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
			for (int x = rect.X; x < rect.X + rect.Width; x++) {
				CellClass * cptr = &Map[Cell(x, y)];
				if (cptr->Is_Tile_Paved_Road()) {
					if (!overlap_road) {
						return(false);
					}
				} else if (cptr->Is_Tile_Paved_Road_End()) {
					if (!overlap_road_end) {
						return(false);
					}
				} else if (!cptr->Is_Tile_Clear() && !cptr->Is_Tile_Misc_Pavement() && !cptr->Is_Tile_Pavement()) {
					return(false);
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005A7440
bool MapGeneratorClass::Can_Place_Paved_Road_End(Rect const & rect, bool overlap_road)
{
	if (My_In_Radar(Cell(rect.X, rect.Y)) &&
		My_In_Radar(Cell(rect.X + rect.Width - 1, rect.Y)) &&
		My_In_Radar(Cell(rect.X, rect.Y + rect.Height - 1)) &&
		My_In_Radar(Cell(rect.X + rect.Width - 1, rect.Y + rect.Height - 1))) {

		int height = Map[Cell(rect.X, rect.Y)].Height;

		for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
			for (int x = rect.X; x < rect.X + rect.Width; x++) {
				CellClass * cptr = &Map[Cell(x, y)];
				if (cptr->Height != height) {
					return(false);
				}
				if (cptr->Is_Tile_Paved_Road() || cptr->Is_Tile_Paved_Road_End()) {
					if (!overlap_road) {
						return(false);
					}
				} else if (!cptr->Is_Tile_Clear() && !cptr->Is_Tile_Misc_Pavement() && !cptr->Is_Tile_Pavement()) {
					return(false);
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005A7640
Rect MapGeneratorClass::Get_Cell_Bounding_Rect(const DynamicVectorClass<Cell> & cells)
{
	Cell cmin;
	Cell cmax;
	cmin.X = 999;
	cmax.X = 0;
	cmin.Y = 999;
	cmax.Y = 0;
	for (int i = cells.Count() - 1; i >= 0; i--) {
		Cell cell = cells[i];
		cmin.X = MIN(cell.X, cmin.X);
		cmin.Y = MIN(cell.Y, cmin.Y);
		cmax.X = MAX(cell.X, cmax.X);
		cmax.Y = MAX(cell.Y, cmax.Y);
	}
	return(Rect(cmin.X, cmin.Y, (cmax.X - cmin.X) + 1, (cmax.Y - cmin.Y) + 1));
}

// FUNCTION: GAMEMD 0x005A76E0
bool MapGeneratorClass::Can_Place_Paved_Road_Junction(Rect & rect)
{
	if (My_In_Radar(Cell(rect.X, rect.Y)) &&
		My_In_Radar(Cell(rect.X + rect.Width - 1, rect.Y)) &&
		My_In_Radar(Cell(rect.X, rect.Y + rect.Height - 1)) &&
		My_In_Radar(Cell(rect.X + rect.Width - 1,rect.Y + rect.Height - 1))) {

		for (int y = rect.Y; y < rect.Y + 3; y++) {
			for (int x = rect.X; x < rect.X + 3; x++) {
				CellClass * cptr = &Map[Cell(x, y)];
				if ((cptr->ITType < IsometricTileTypeClass::PavedRoads || cptr->ITType > IsometricTileTypeClass::PavedRoads + 1) && !cptr->Is_Tile_Road_Median()) {
					return(false);
				}
			}
		}
		for (int x = rect.X; x < rect.X + 3; x++) {
			CellClass * cptr = &Map[Cell(x, rect.Y + 3)];
			if (!cptr->Is_Tile_Pavement() && !cptr->Is_Tile_Clear() && !cptr->Is_Tile_Misc_Pavement()) {
				return(false);
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005A7B40
bool MapGeneratorClass::Can_Place_Misc_Pavement(Rect const & rect)
{
	if (My_In_Radar(Cell(rect.X, rect.Y)) &&
		My_In_Radar(Cell((rect.X - 1) + rect.Width, rect.Y)) &&
		My_In_Radar(Cell(rect.X, (rect.Y - 1) + rect.Height)) &&
		My_In_Radar(Cell((rect.X - 1) + rect.Width, (rect.Y - 1) + rect.Height))) {

		for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
			for (int x = rect.X; x < rect.X + rect.Width; x++) {
				CellClass *cptr = &Map[Cell(x,y)];
				IsometricTileType itype = cptr->ITType;
				if (!cptr->Is_Tile_Pavement() && (itype < IsometricTileTypeClass::MiscPaveTile || itype > IsometricTileTypeClass::MiscPaveTile + 7)) {
					return(false);
				}
				if (cptr->Cell_Occupier() != NULL) {
					return(false);
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005A7D00
bool MapGeneratorClass::Can_Place_Tile(IsometricTileType ittype, Cell const & cell, bool collision, bool in_map)
{
	IsometricTileTypeClass * itype = IsometricTileTypes[ittype];
	IsoTileSet * set = (IsoTileSet *)itype->Get_Image_Data();
	if (set != NULL) {
		for (int y = 0; y < itype->Height; y++) {
			for (int x = 0; x < itype->Width; x++) {
				Cell ncell = cell + Cell(x, y);
				if (My_In_Radar(ncell)) {
					CellClass * cptr = &Map[ncell];
					int subtile = itype->SubTile_Index(x, y);
					IsoTileRecord const * record = set->Fetch_Record_Pointer_Unsafe(subtile);
					if (record != NULL) {
						if ((!cptr->Is_Tile_Clear() && !cptr->Is_Tile_Ramp()) || cptr->Ramp != record->RampType || collision && MapRegionClass::Get_Cell_Data(ncell).TilePlaced) {
							return(false);
						}
					}
				} else if (in_map) {
					return(false);
				}
			}
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005A7E90
void MapGeneratorClass::Remove_Tile(IsometricTileType ittype, Cell const & cell)
{
	IsometricTileTypeClass * itype = IsometricTileTypes[ittype];
	IsoTileSet * set = (IsoTileSet *)itype->Get_Image_Data();
	if (set != NULL) {
		for (int y = 0; y < itype->Height; y++) {
			for (int x = 0; x < itype->Width; x++) {
				Cell ncell = cell + Cell(x, y);
				if (My_In_Radar(ncell)) {
					CellClass * cptr = &Map[ncell];
					int subtile = itype->SubTile_Index(x, y);
					IsoTileRecord const * record = set->Fetch_Record_Pointer_Unsafe(subtile);
					if (record != NULL) {
						cptr->ITType = TILE_NONE;
						cptr->SubTile = 0;
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005A7FA0
void MapGeneratorClass::Mark_Tile(IsometricTileType ittype, Cell const & cell, bool placed)
{
	IsometricTileTypeClass * itype = IsometricTileTypes[ittype];
	IsoTileSet * set = (IsoTileSet *)itype->Get_Image_Data();
	if (set != NULL) {
		for (int y = 0; y < itype->Height; y++) {
			for (int x = 0; x < itype->Width; x++) {
				Cell ncell = cell + Cell(x, y);
				if (My_In_Radar(ncell)) {
					int subtile = itype->SubTile_Index(x, y);
					IsoTileRecord const * record = set->Fetch_Record_Pointer_Unsafe(subtile);
					if (record != NULL) {
						MapRegionClass::Get_Cell_Data(ncell).TilePlaced = placed;
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005A80B0
void MapGeneratorClass::Generate_Rural_Areas(void)
{
	Init_Dirt_Roads();

	int count = (SeedData.Cities + 0.49) * 2;
	int tries = 0;
	int placed = 0;

	while (true) {
		if (placed >= count) {
			break;
		}
		Cell cell(0, 0);
		int tries_here = 0;
		while (true) {
			cell = MapRegionClass::Pick_Random_Clear_Map_Cell();
			if (++tries_here >= 20) break;
			if (MapRegionClass::CellData::Is_Not_Inviolate(cell) && Can_Place_Paved_Road(Rect(cell.X - 4, cell.Y - 4, 8, 8), false, false) && Map.In_Local_Radar(cell)) {
				break;
			}
		}

		if (tries_here != 20) {
			if (Generate_Rural_Roads(cell)) {
				Generate_Rural_Buildings(cell);
				Generate_Rural_Units(cell);
				placed++;
			}
		}

		tries++;
		if (tries >= 10) {
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x005A82E0
void MapGeneratorClass::Generate_Rural_Buildings(const Cell & cell)
{
	static char const * _names[] = {
		"ABAN01",
		"ABAN02",
		"ABAN03",
		"ABAN04",
		"ABAN05",
		"ABAN06",
		"ABAN07",
		"ABAN08",
		"ABAN09",
		"ABAN10",
		"ABAN11",
		"ABAN12",
		"ABAN13",
		"ABAN14",
		"ABAN15",
		"CA0001",
		"CA0002",
		"CA0003",
		"CA0004",
		"CA0005",
		"CA0006",
		"CA0007",
		"CA0008",
		"CA0009",
		"CA0010",
		"CA0011",
		"CA0012",
		"CA0013",
		"CA0014",
		"CA0015",
		"CA0016",
		"CA0017",
		"CA0018",
		"CA0019",
		"CA0020",
		"CA0021"
	};

	int count = Pick_Random_UInt(8, 12);
	int tries = 0;
	int placed = 0;

	CellClass & center = Map[cell];
	int height = center.Height;

	int min_building = 0;
	int max_building = ARRAY_SIZE(_names) - 1;
	if (SeedData.Biome == BIOME_TUNDRA) {
		min_building = 15;
	}

	DynamicVectorClass<char *> names;
	for (int i = 0; i < ARRAY_SIZE(_names); i++) {
		names.Add((char *&)_names[i]);
	}

	HouseClass * neutral = House_From_HousesType(HouseTypeClass::From_Name("Neutral"));

	if (count > 0) {
		while (placed < count) {
			if (tries >= 2 * count) {
				break;
			}
			int name_index = Pick_Random_UInt(min_building, max_building);
			StructType bindex = BuildingTypeClass::From_Name(names[name_index]);
			if (bindex != STRUCT_NONE) {
				BuildingClass * building = new BuildingClass(BuildingTypes[bindex], neutral);

				int tries_here = 0;
				bool was_placed = false;
				while (!was_placed) {
					if (tries_here >= 60) {
						break;
					}

					Cell building_cell = cell + Cell(Pick_Random_UInt(0, 14) - 7, Pick_Random_UInt(0, 14) - 7);

					Cell const * list = building->Class->Occupy_List(true);
					bool ok = true;
					while (*list != REFRESH_EOL) {
						CellClass * cellptr = &Map[building_cell + *list];
						if (cellptr->Cell_Occupier() != NULL || !cellptr->Is_Tile_Clear() || cellptr->Height != height || cellptr->Land_Type() == LAND_ROCK || !Map.In_Local_Radar(cellptr)) {
							ok = false;
						}
						list++;
						if (!ok) break;
					}

					if (ok) {
						if (building->Unlimbo(Coord(building_cell, 0))) {
							was_placed = true;
							placed++;
							names.Delete_Index(name_index);
							max_building--;
						}
					}
					tries_here++;
				}

				if (!was_placed) {
					delete building;
				}

				tries++;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005A8720
void MapGeneratorClass::Generate_Rural_Units(const Cell & cell)
{
	static char const * _names[] = {
		"CIV1",
		"CIV2",
		"CIV3",
		"PICK",
		"WINI",
		"CAR"
	};

	HouseClass * neutral = House_From_HousesType(HouseTypeClass::From_Name("Neutral"));
	int count = Pick_Random_UInt(1, 4);

	int tries = 0;
	int placed = 0;

	while (placed < count) {
		if (tries >= 2 * count) {
			break;
		}

		int unit_index = Pick_Random_UInt(0, ARRAY_SIZE(_names) - 1);
		FootClass * foot = NULL;
		if (unit_index <= 2) {
			foot = new InfantryClass(InfantryTypes[InfantryTypeClass::From_Name(_names[unit_index])], neutral);
		} else {
			foot = new UnitClass(UnitTypes[UnitTypeClass::From_Name(_names[unit_index])], neutral);
		}

		int tries_here = 0;
		bool was_placed = false;
		while (!was_placed) {
			if (tries_here >= 60) {
				break;
			}
			Cell unit_cell = cell + Cell(Pick_Random_UInt(0, 16) - 8, Pick_Random_UInt(0, 16) - 8);
			if (Map.In_Local_Radar(unit_cell) && Map[unit_cell].Cell_Occupier() == NULL) {
				if (foot->Unlimbo(Coord(unit_cell), (Dir256)(Pick_Random_UInt(0, 7) * 32))) {
					foot->Strength = foot->Techno_Type_Class()->MaxStrength;
					foot->EstimatedHealth = foot->Strength;
					foot->Mark(MARK_CHANGE);
					was_placed = true;
					placed++;
				}
			}
			tries_here++;
		}

		if (!was_placed) {
			delete foot;
		}

		tries++;
	}
}

namespace Unsorted { extern bool ArmageddonMode; }

// FUNCTION: GAMEMD 0x005A91E0
void MapGeneratorClass::Generate_Lights(void)
{
	StructType glite = BuildingTypeClass::From_Name("GALITE");
	if (glite != STRUCT_NONE) {
		HouseClass * hptr = House_From_HousesType(HouseTypeClass::From_Name("Special"));
		if (hptr != NULL) {
			bool old = Unsorted::ArmageddonMode;
			Unsorted::ArmageddonMode = true;
			int i;
			for (i = 0; i < SeedData.NumPlayers; i++) {
				Cell waypoint = Scen->Get_Waypoint_Cell(i);
				int tries = 20;

				static const int _light_counts[] = {0, 0, 2, 4};
				int light_count = _light_counts[SeedData.Time];

				bool can_place = false;
				int j;
				while (tries >= 0 && !can_place) {
					double angle = Random_Fraction() * DEG_TO_RAD(360);
					can_place = true;

					for (j = 0; j < light_count; j++) {
						Coord pc = Move_Coord(Coord(waypoint), j * DEG_TO_RAD(360) / light_count + angle, 8 * CELL_LEPTON);
						Cell place_cell = pc.As_Cell();
						if (!BuildingTypes[glite]->Legal_Placement(place_cell, hptr) || !Map.In_Radar(place_cell)) {
							can_place = false;
							break;
						}
					}

					if (can_place) {
						for (j = 0; j < light_count; j++) {
							Coord pc = Move_Coord(Coord(waypoint), j * DEG_TO_RAD(360) / light_count + angle, 8 * CELL_LEPTON);
						Cell place_cell = pc.As_Cell();
							BuildingClass * light = new BuildingClass(BuildingTypes[glite], hptr);
							if (!light->Unlimbo(Coord(place_cell))) {
								delete light;
							}
						}
					}

					tries--;
				}
			}
			Unsorted::ArmageddonMode = old;
		}
	}
}

// FUNCTION: GAMEMD 0x005A95B0
void MapGeneratorClass::Generate_Neutral_Buildings(void)
{
	MapRegionClass * region;
	int j;
	if (SeedData.MapType == 2) {
		int count = Pick_Random_UInt(0, 2);
		int i = 0;
		while (i < count) {
			j = 0;
			if (MapRegionClass::MapRegions.Count() > 0) {
				do {
					region = MapRegionClass::MapRegions[j];
					MapRegionClass * same = region;
					region = same;
					if (MapRegionClass::MapRegions[j]->Unknown20 > 0) {
						region->Place_Neutral_Building();
					}
					j++;
				} while (j < MapRegionClass::MapRegions.Count());
			}
			i++;
		}
		return;
	}

	HouseClass * neutral = House_From_HousesType(HouseTypeClass::From_Name("Neutral"));
	int count = Pick_Random_UInt(0, 4);

	for (int i = 0; i < count; i++) {
		bool placed = false;
		int tries = 0;

		BuildingClass * building = new BuildingClass(Rule->NeutralTechBuildings[Pick_Random_UInt(0, Rule->NeutralTechBuildings.Count() - 1)], neutral);

		for (; !placed; ++tries) {
			if (tries >= 100) {
				break;
			}

			Cell building_cell = MapRegionClass::Pick_Random_Clear_Map_Cell();
			int height = Map[building_cell].Height;

			Cell const * list = building->Class->Occupy_List(true);
			bool ok = true;
			while (*list != REFRESH_EOL) {
				CellClass * cellptr = &Map[building_cell + *list];
				if (cellptr->Cell_Occupier() != NULL || !cellptr->Is_Tile_Clear() || cellptr->Height != height || cellptr->Land_Type() == LAND_ROCK || !Map.In_Local_Radar(cellptr) || !MapRegionClass::CellData::Is_Not_Inviolate(cellptr->CellID)) {
					ok = false;
				}
				list++;
				if (!ok) break;
			}

			if (ok) {
				if (building->Unlimbo(Coord(building_cell, 0))) {
					placed = true;
				}
			}
		}

		if (!placed) {
			if (building != NULL) {
				delete building;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005A9920
DirtRoadLink::DirtRoadLink(Cell const & cell, FacingType dir) :
	Offset(cell),
	Facing(dir)
{
}

// FUNCTION: GAMEMD 0x005A9940
DirtRoadLink::DirtRoadLink(void) :
	Offset(0, 0),
	Facing(FACING_NONE)
{
}

// FUNCTION: GAMEMD 0x005A9960
DirtRoadLink::DirtRoadLink(DirtRoadLink const & that) :
	Offset(that.Offset),
	Facing(that.Facing)
{
}

// FUNCTION: GAMEMD 0x005A9980
DirtRoadNode::DirtRoadNode(IsometricTileType road_tile, Cell const & origin, IsometricTileType cap_tile, Cell const & cap_origin, int link_index) :
	Tile(road_tile),
	Origin(origin),
	CapTile(cap_tile),
	CapOrigin(cap_origin),
	BackLinkIndex(link_index)
{
}

// FUNCTION: GAMEMD 0x005A99B0
DirtRoadNode::DirtRoadNode(void) :
	Tile(ISOTILE_NONE),
	Origin(Cell(0, 0)),
	CapTile(ISOTILE_NONE),
	CapOrigin(Cell(0, 0)),
	BackLinkIndex(-1)
{
}

// FUNCTION: GAMEMD 0x005A99F0
DirtRoadNode::DirtRoadNode(DirtRoadNode const & that) :
	Tile(that.Tile),
	Origin(that.Origin),
	CapTile(that.CapTile),
	CapOrigin(that.CapOrigin),
	BackLinkIndex(that.BackLinkIndex)
{
}

// FUNCTION: GAMEMD 0x005A9A20
DirtRoadTile::DirtRoadTile(DirtRoadLink const & link1) :
	ITType(ISOTILE_CLEAR)
{
	Count = 1;
	Links = new DirtRoadLink[1];
	Links[0] = link1;
}

// FUNCTION: GAMEMD 0x005A9A70
DirtRoadTile::DirtRoadTile(DirtRoadLink const & link1, DirtRoadLink const & link2) :
	ITType(ISOTILE_CLEAR)
{
	Count = 2;
	Links = new DirtRoadLink[2];
	Links[0] = link1;
	Links[1] = link2;
}

// FUNCTION: GAMEMD 0x005A9AE0
DirtRoadTile::DirtRoadTile(DirtRoadLink const & link1, DirtRoadLink const & link2, DirtRoadLink const & link3) :
	ITType(ISOTILE_CLEAR)
{
	Count = 3;
	Links = new DirtRoadLink[3];
	Links[0] = link1;
	Links[1] = link2;
	Links[2] = link3;
}

// FUNCTION: GAMEMD 0x005A9B60
DirtRoadTile::DirtRoadTile(DirtRoadLink const & link1, DirtRoadLink const & link2, DirtRoadLink const & link3, DirtRoadLink const & link4) :
	ITType(ISOTILE_CLEAR)
{
	Count = 4;
	Links = new DirtRoadLink[4];
	Links[0] = link1;
	Links[1] = link2;
	Links[2] = link3;
	Links[3] = link4;
}

// FUNCTION: GAMEMD 0x005A9BF0
DirtRoadTile::DirtRoadTile()
{
	Count = 0;
	Links = NULL;
}

// FUNCTION: GAMEMD 0x005A9C00
DirtRoadTile::~DirtRoadTile(void)
{
	delete Links;
	Links = NULL;
}

// SYNTHETIC: GAMEMD 0x005A9C20 SYMBOL
// _$E203
// SYNTHETIC: GAMEMD 0x005AC190 SYMBOL
// _$E201
DirtRoadTile DirtRoadTiles[] = {
	/* 0 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(2, 0), FACING_NE)),
	/* 1 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(1, 0), FACING_E)),
	/* 2 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(1, 1), FACING_SE)),
	/* 3 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(1, 0), FACING_SW)),
	/* 4 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 5 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_N), DirtRoadLink(Cell(1, 1), FACING_NW)),
	/* 6 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_NE), DirtRoadLink(Cell(1, 1), FACING_E)),
	/* 7 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_NE), DirtRoadLink(Cell(1, 1), FACING_SE)),
	/* 8 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_NE), DirtRoadLink(Cell(0, 1), FACING_S)),
	/* 9 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_NE), DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 10 */ DirtRoadTile(DirtRoadLink(Cell(2, 0), FACING_NE), DirtRoadLink(Cell(1, 1), FACING_NW)),
	/* 11 */ DirtRoadTile(DirtRoadLink(Cell(2, 0), FACING_NE), DirtRoadLink(Cell(0, 0), FACING_N)),
	/* 12 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_E), DirtRoadLink(Cell(1, 2), FACING_SE)),
	/* 13 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_E), DirtRoadLink(Cell(0, 1), FACING_S)),
	/* 14 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_E), DirtRoadLink(Cell(1, 0), FACING_SW)),
	/* 15 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_E), DirtRoadLink(Cell(1, 1), FACING_NW)),
	/* 16 */ DirtRoadTile(DirtRoadLink(Cell(2, 1), FACING_SE), DirtRoadLink(Cell(0, 1), FACING_S)),
	/* 17 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_SE), DirtRoadLink(Cell(1, 0), FACING_SW)),
	/* 18 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_SE), DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 19 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_S), DirtRoadLink(Cell(1, 0), FACING_SW)),
	/* 20 */ DirtRoadTile(DirtRoadLink(Cell(0, 1), FACING_S), DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 21 */ DirtRoadTile(DirtRoadLink(Cell(0, 1), FACING_S), DirtRoadLink(Cell(1, 1), FACING_NW)),
	/* 22 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_SW), DirtRoadLink(Cell(1, 1), FACING_NW)),
	/* 23 */ DirtRoadTile(DirtRoadLink(Cell(0, 1), FACING_W), DirtRoadLink(Cell(1, 1), FACING_NW)),
	/* 24 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(1, 0), FACING_E), DirtRoadLink(Cell(0, 1), FACING_S)),
	/* 25 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_E), DirtRoadLink(Cell(0, 1), FACING_S), DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 26 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 1), FACING_S), DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 27 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(1, 0), FACING_E), DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 28 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(1, 0), FACING_E), DirtRoadLink(Cell(0, 1), FACING_S), DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 29 */ DirtRoadTile(DirtRoadLink(Cell(2, 0), FACING_NE), DirtRoadLink(Cell(2, 2), FACING_SE), DirtRoadLink(Cell(1, 1), FACING_SW)),
	/* 30 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(1, 1), FACING_SW), DirtRoadLink(Cell(2, 2), FACING_SE)),
	/* 31 */ DirtRoadTile(DirtRoadLink(Cell(2, 0), FACING_NE), DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(1, 1), FACING_SW)),
	/* 32 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(2, 0), FACING_NE), DirtRoadLink(Cell(2, 2), FACING_SE)),
	/* 33 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(1, 1), FACING_SW), DirtRoadLink(Cell(2, 0), FACING_NE), DirtRoadLink(Cell(2, 2), FACING_SE)),
	/* 34 */ DirtRoadTile(DirtRoadLink(Cell(3, 0), FACING_NE), DirtRoadLink(Cell(3, 2), FACING_E), DirtRoadLink(Cell(0, 2), FACING_W)),
	/* 35 */ DirtRoadTile(DirtRoadLink(Cell(3, 0), FACING_NE), DirtRoadLink(Cell(1, 2), FACING_SW)),
	/* 36 */ DirtRoadTile(DirtRoadLink(Cell(3, 0), FACING_NE), DirtRoadLink(Cell(1, 2), FACING_SW)),
	/* 37 */ DirtRoadTile(DirtRoadLink(Cell(3, 0), FACING_NE), DirtRoadLink(Cell(1, 2), FACING_SW)),
	/* 38 */ DirtRoadTile(DirtRoadLink(Cell(3, 0), FACING_NE), DirtRoadLink(Cell(1, 2), FACING_SW)),
	/* 39 */ DirtRoadTile(DirtRoadLink(Cell(2, 0), FACING_NE), DirtRoadLink(Cell(1, 1), FACING_SW)),
	/* 40 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_NE), DirtRoadLink(Cell(1, 0), FACING_SW)),
	/* 41 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_NE), DirtRoadLink(Cell(1, 0), FACING_SW)),
	/* 42 */ DirtRoadTile(DirtRoadLink(Cell(1, 2), FACING_SW)),
	/* 43 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_SW)),
	/* 44 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_SW)),
	/* 45 */ DirtRoadTile(DirtRoadLink(Cell(3, 0), FACING_NE)),
	/* 46 */ DirtRoadTile(DirtRoadLink(Cell(2, 0), FACING_NE)),
	/* 47 */ DirtRoadTile(DirtRoadLink(Cell(2, 0), FACING_NE)),
	/* 48 */ DirtRoadTile(DirtRoadLink(Cell(4, 0), FACING_NE), DirtRoadLink(Cell(1, 1), FACING_SW)),
	/* 49 */ DirtRoadTile(DirtRoadLink(Cell(2, 0), FACING_NE), DirtRoadLink(Cell(1, 3), FACING_SW)),
	/* 50 */ DirtRoadTile(),
	/* 51 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(3, 0), FACING_E)),
	/* 52 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(3, 0), FACING_E)),
	/* 53 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(3, 0), FACING_E)),
	/* 54 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(3, 0), FACING_E)),
	/* 55 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(2, 0), FACING_E)),
	/* 56 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(1, 0), FACING_E)),
	/* 57 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(0, 0), FACING_E)),
	/* 58 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(0, 0), FACING_E)),
	/* 59 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 60 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W)),
	/* 61 */ DirtRoadTile(DirtRoadLink(Cell(0, 1), FACING_W)),
	/* 62 */ DirtRoadTile(DirtRoadLink(Cell(2, 0), FACING_E)),
	/* 63 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_E)),
	/* 64 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_E)),
	/* 65 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(3, 1), FACING_E)),
	/* 66 */ DirtRoadTile(DirtRoadLink(Cell(0, 1), FACING_W), DirtRoadLink(Cell(3, 0), FACING_E)),
	/* 67 */ DirtRoadTile(),
	/* 68 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(3, 3), FACING_SE)),
	/* 69 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(3, 3), FACING_SE)),
	/* 70 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(3, 3), FACING_SE)),
	/* 71 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(3, 3), FACING_SE)),
	/* 72 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(2, 2), FACING_SE)),
	/* 73 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(1, 1), FACING_SE)),
	/* 74 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(1, 1), FACING_SE)),
	/* 75 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW)),
	/* 76 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW)),
	/* 77 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW)),
	/* 78 */ DirtRoadTile(DirtRoadLink(Cell(3, 3), FACING_SE)),
	/* 79 */ DirtRoadTile(DirtRoadLink(Cell(1, 2), FACING_SE)),
	/* 80 */ DirtRoadTile(DirtRoadLink(Cell(2, 2), FACING_SE)),
	/* 81 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(4, 2), FACING_SE)),
	/* 82 */ DirtRoadTile(DirtRoadLink(Cell(1, 1), FACING_NW), DirtRoadLink(Cell(2, 4), FACING_SE)),
	/* 83 */ DirtRoadTile(),
	/* 84 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 3), FACING_S)),
	/* 85 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 3), FACING_S)),
	/* 86 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 3), FACING_S)),
	/* 87 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 3), FACING_S)),
	/* 88 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 2), FACING_S)),
	/* 89 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 1), FACING_S)),
	/* 90 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 0), FACING_S)),
	/* 91 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 0), FACING_S)),
	/* 92 */ DirtRoadTile(DirtRoadLink(Cell(0, 3), FACING_S)),
	/* 93 */ DirtRoadTile(DirtRoadLink(Cell(0, 2), FACING_S)),
	/* 94 */ DirtRoadTile(DirtRoadLink(Cell(0, 2), FACING_S)),
	/* 95 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N)),
	/* 96 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N)),
	/* 97 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N)),
	/* 98 */ DirtRoadTile(DirtRoadLink(Cell(1, 0), FACING_N), DirtRoadLink(Cell(0, 3), FACING_S)),
	/* 99 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(1, 3), FACING_S)),
	/* 100 */ DirtRoadTile(),
	/* 101 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(0, 0), FACING_E)),
	/* 102 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(0, 0), FACING_E)),
	/* 103 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 0), FACING_S)),
	/* 104 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 0), FACING_S)),
	/* 105 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(0, 0), FACING_E)),
	/* 106 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_W), DirtRoadLink(Cell(0, 0), FACING_E)),
	/* 107 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 0), FACING_S)),
	/* 108 */ DirtRoadTile(DirtRoadLink(Cell(0, 0), FACING_N), DirtRoadLink(Cell(0, 0), FACING_S))
};

// FUNCTION: GAMEMD 0x005AC1C0
int Dirt_Road_Tile_Index(IsometricTileType ittype)
{
	if (ittype >= IsometricTileTypeClass::DirtRoadCurve && ittype < IsometricTileTypeClass::DirtRoadCurve + 101) {
		return(ittype - IsometricTileTypeClass::DirtRoadCurve);
	}
	if (ittype < IsometricTileTypeClass::DirtRoadSlopes || ittype >= IsometricTileTypeClass::DirtRoadSlopes + 8) {
		return(0);
	}
	return(ittype - IsometricTileTypeClass::DirtRoadSlopes + 101);
}

// FUNCTION: GAMEMD 0x005AC1F0
void Init_Dirt_Roads(void)
{
	int i;
	for (i = 0; i < 101; i++) {
		DirtRoadTiles[i].ITType = IsometricTileType(IsometricTileTypeClass::DirtRoadCurve + i);
	}
	for (i = 0; i < 8; i++) {
		DirtRoadTiles[i + 101].ITType = IsometricTileType(IsometricTileTypeClass::DirtRoadSlopes + i);
	}
}

// SYNTHETIC: GAMEMD 0x005AC270
// ??_GMapSeedClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x005AC340
// ??_G?$PriorityQueueClass@UCellNode@@@@QAEPAXI@Z
template class PriorityQueueClass<CellNode>;

// TEMPLATE: GAMEMD 0x005AC370
// ??9?$TPoint2D@F@@QBE_NABV0@@Z
template class TPoint2D<short>;

// TEMPLATE: GAMEMD 0x005AC3A0
// ?Clear@?$DynamicVectorClass@PAVMapRegionClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005AC3D0
// ?ID@?$DynamicVectorClass@PAVMapRegionClass@@@@UAEHPBQAVMapRegionClass@@@Z
// TEMPLATE: GAMEMD 0x005AC3F0
// ?GetItem@?$VectorClass@PAVMapRegionClass@@@@UBEPAVMapRegionClass@@H@Z
template class DynamicVectorClass<MapRegionClass *>;

// TEMPLATE: GAMEMD 0x005AC400
// ??0?$TypeList@PBVBuildingTypeClass@@@@QAE@XZ
template class TypeList<BuildingTypeClass const *>;

// TEMPLATE: GAMEMD 0x005AC430
// ??4?$DynamicVectorClass@PBVBuildingTypeClass@@@@QAEAAV0@ABV0@@Z
template class DynamicVectorClass<BuildingTypeClass const *>;

// TEMPLATE: GAMEMD 0x005AC4C0
// ?Clear@?$DynamicVectorClass@PAU_WIN32_FIND_DATAA@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005AC4F0
// ?ID@?$DynamicVectorClass@PAU_WIN32_FIND_DATAA@@@@UAEHPBQAU_WIN32_FIND_DATAA@@@Z
// TEMPLATE: GAMEMD 0x005AC510
// ?GetItem@?$VectorClass@PAU_WIN32_FIND_DATAA@@@@UBEPAU_WIN32_FIND_DATAA@@H@Z
template class DynamicVectorClass<WIN32_FIND_DATAA *>;

// TEMPLATE: GAMEMD 0x005AC520
// ?Clear@?$DynamicVectorClass@V?$TRect@H@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005AC550
// ?ID@?$DynamicVectorClass@V?$TRect@H@@@@UAEHPBV?$TRect@H@@@Z
// TEMPLATE: GAMEMD 0x005AC570
// ?GetItem@?$VectorClass@V?$TRect@H@@@@UBE?AV?$TRect@H@@H@Z
template class DynamicVectorClass<Rect>;

// TEMPLATE: GAMEMD 0x005AC5A0
// ?Clear@?$DynamicVectorClass@PAVCellClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005AC5D0
// ?ID@?$DynamicVectorClass@PAVCellClass@@@@UAEHPBQAVCellClass@@@Z
template class DynamicVectorClass<CellClass *>;

// TEMPLATE: GAMEMD 0x005AC5F0
// ?Clear@?$DynamicVectorClass@N@@UAEXXZ
// TEMPLATE: GAMEMD 0x005AC620
// ?ID@?$DynamicVectorClass@N@@UAEHPBN@Z
// TEMPLATE: GAMEMD 0x005AC640
// ?GetItem@?$VectorClass@N@@UBENH@Z
template class DynamicVectorClass<double>;

// SYNTHETIC: GAMEMD 0x005AC650
// ??_G?$TypeList@PBVBuildingTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x005AC6A0
// ??8?$VectorClass@PAVMapRegionClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005AC6E0
// ?Resize@?$VectorClass@PAVMapRegionClass@@@@UAE_NHPBQAVMapRegionClass@@@Z
// TEMPLATE: GAMEMD 0x005AC790
// ?Clear@?$VectorClass@PAVMapRegionClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005AC7C0
// ?ID@?$VectorClass@PAVMapRegionClass@@@@UAEHABQAVMapRegionClass@@@Z
// TEMPLATE: GAMEMD 0x005AC800
// ?ID@?$VectorClass@PAVMapRegionClass@@@@UAEHPBQAVMapRegionClass@@@Z
template class VectorClass<MapRegionClass *>;

// TEMPLATE: GAMEMD 0x005AC820
// ?Resize@?$DynamicVectorClass@PAVMapRegionClass@@@@UAE_NHPBQAVMapRegionClass@@@Z
// TEMPLATE: GAMEMD 0x005AC8D0
// ?ID@?$DynamicVectorClass@PAVMapRegionClass@@@@UAEHABQAVMapRegionClass@@@Z
// TEMPLATE: GAMEMD 0x005AC900
// ??0?$DynamicVectorClass@PBVBuildingTypeClass@@@@QAE@IPBQBVBuildingTypeClass@@@Z
// TEMPLATE: GAMEMD 0x005AC960
// ?Extract_Min@?$PriorityQueueClass@UCellNode@@@@QAEPAUCellNode@@XZ
// TEMPLATE: GAMEMD 0x005ACA60
// ??8?$VectorClass@PAU_WIN32_FIND_DATAA@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005ACAA0
// ?Resize@?$VectorClass@PAU_WIN32_FIND_DATAA@@@@UAE_NHPBQAU_WIN32_FIND_DATAA@@@Z
// TEMPLATE: GAMEMD 0x005ACB50
// ?Clear@?$VectorClass@PAU_WIN32_FIND_DATAA@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005ACB80
// ?ID@?$VectorClass@PAU_WIN32_FIND_DATAA@@@@UAEHABQAU_WIN32_FIND_DATAA@@@Z
// TEMPLATE: GAMEMD 0x005ACBC0
// ?ID@?$VectorClass@PAU_WIN32_FIND_DATAA@@@@UAEHPBQAU_WIN32_FIND_DATAA@@@Z
// TEMPLATE: GAMEMD 0x005ACBE0
// ?Resize@?$DynamicVectorClass@PAU_WIN32_FIND_DATAA@@@@UAE_NHPBQAU_WIN32_FIND_DATAA@@@Z
// TEMPLATE: GAMEMD 0x005ACC90
// ?ID@?$DynamicVectorClass@PAU_WIN32_FIND_DATAA@@@@UAEHABQAU_WIN32_FIND_DATAA@@@Z
template class VectorClass<WIN32_FIND_DATAA *>;

// TEMPLATE: GAMEMD 0x005ACCC0
// ??8?$VectorClass@V?$TRect@H@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005ACD30
// ?Resize@?$VectorClass@V?$TRect@H@@@@UAE_NHPBV?$TRect@H@@@Z
// TEMPLATE: GAMEMD 0x005ACE20
// ?Clear@?$VectorClass@V?$TRect@H@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005ACE50
// ?ID@?$VectorClass@V?$TRect@H@@@@UAEHABV?$TRect@H@@@Z
// TEMPLATE: GAMEMD 0x005ACEB0
// ?ID@?$VectorClass@V?$TRect@H@@@@UAEHPBV?$TRect@H@@@Z
// TEMPLATE: GAMEMD 0x005ACED0
// ?Resize@?$DynamicVectorClass@V?$TRect@H@@@@UAE_NHPBV?$TRect@H@@@Z
// TEMPLATE: GAMEMD 0x005ACFC0
// ?ID@?$DynamicVectorClass@V?$TRect@H@@@@UAEHABV?$TRect@H@@@Z
template class VectorClass<Rect>;

// TEMPLATE: GAMEMD 0x005AD010
// ?Resize@?$DynamicVectorClass@PAVCellClass@@@@UAE_NHPBQAVCellClass@@@Z
// TEMPLATE: GAMEMD 0x005AD0C0
// ?ID@?$DynamicVectorClass@PAVCellClass@@@@UAEHABQAVCellClass@@@Z
// TEMPLATE: GAMEMD 0x005AD0F0
// ??8?$VectorClass@N@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005AD130
// ?Resize@?$VectorClass@N@@UAE_NHPBN@Z
// TEMPLATE: GAMEMD 0x005AD1F0
// ?Clear@?$VectorClass@N@@UAEXXZ
// TEMPLATE: GAMEMD 0x005AD220
// ?ID@?$VectorClass@N@@UAEHABN@Z
// TEMPLATE: GAMEMD 0x005AD270
// ?ID@?$VectorClass@N@@UAEHPBN@Z
// TEMPLATE: GAMEMD 0x005AD290
// ?Resize@?$DynamicVectorClass@N@@UAE_NHPBN@Z
// TEMPLATE: GAMEMD 0x005AD350
// ?ID@?$DynamicVectorClass@N@@UAEHABN@Z
// SYNTHETIC: GAMEMD 0x005AD390
// ??_G?$VectorClass@PAVMapRegionClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005AD3E0
// ??_G?$DynamicVectorClass@PAVMapRegionClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005AD430
// ??_G?$VectorClass@PAU_WIN32_FIND_DATAA@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005AD480
// ??_G?$DynamicVectorClass@PAU_WIN32_FIND_DATAA@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005AD4D0
// ??_G?$VectorClass@V?$TRect@H@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005AD520
// ??_G?$DynamicVectorClass@V?$TRect@H@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005AD570
// ??_G?$DynamicVectorClass@PAVCellClass@@@@UAEPAXI@Z
template class VectorClass<CellClass *>;
template class VectorClass<double>;

// SYNTHETIC: GAMEMD 0x005AD5C0
// ??_G?$VectorClass@N@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005AD610
// ??_G?$DynamicVectorClass@N@@UAEPAXI@Z
template class DynamicVectorClass<double>;

// TEMPLATE: GAMEMD 0x005AD660
// ??0?$TypeList@PBVBuildingTypeClass@@@@QAE@ABV0@@Z
template class TypeList<BuildingTypeClass const *>;

// TEMPLATE: GAMEMD 0x005AD6F0
// ??0?$VectorClass@PAD@@QAE@HPBQAD@Z
template class VectorClass<char *>;

// TEMPLATE: GAMEMD 0x005AD740
// ??0?$VectorClass@PAVMapRegionClass@@@@QAE@HPBQAVMapRegionClass@@@Z
// TEMPLATE: GAMEMD 0x005AD790
// ?Delete_Index@?$DynamicVectorClass@PAVMapRegionClass@@@@QAE_NH@Z
template class DynamicVectorClass<MapRegionClass *>;

// TEMPLATE: GAMEMD 0x005AD7D0
// ?Add@?$DynamicVectorClass@PBVBuildingTypeClass@@@@QAE_NABQBVBuildingTypeClass@@@Z
template class DynamicVectorClass<BuildingTypeClass const *>;

// TEMPLATE: GAMEMD 0x005AD820
// ??0?$VectorClass@PAVCellClass@@@@QAE@HPBQAVCellClass@@@Z
template class VectorClass<CellClass *>;

// TEMPLATE: GAMEMD 0x005AD870
// ?Heapify@?$PriorityQueueClass@UCellNode@@@@QAEXH@Z
template class PriorityQueueClass<CellNode>;

// TEMPLATE: GAMEMD 0x005AD930
// ?Increase_Table_Size@?$IndexClass@HPAVMapRegionClass@@@@AAE_NH@Z
// TEMPLATE: GAMEMD 0x005AD9C0
// ?search_compfunc@?$IndexClass@HPAVMapRegionClass@@@@CAHPBX0@Z
template class IndexClass<int, MapRegionClass *>;

// TEMPLATE: GAMEMD 0x005AD9F0
// ??0?$VectorClass@PAU_WIN32_FIND_DATAA@@@@QAE@HPBQAU_WIN32_FIND_DATAA@@@Z
template class VectorClass<WIN32_FIND_DATAA *>;

// TEMPLATE: GAMEMD 0x005ADA40
// ??0?$VectorClass@V?$TRect@H@@@@QAE@HPBV?$TRect@H@@@Z
template class VectorClass<TRect<int> >;

// TEMPLATE: GAMEMD 0x005ADAB0
// ??0?$VectorClass@N@@QAE@HPBN@Z
template class VectorClass<double>;

#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0058B530 SYMBOL
// _$E206
// SYNTHETIC: GAMEMD 0x0058B560 SYMBOL
// _$E209
// SYNTHETIC: GAMEMD 0x0058B580 SYMBOL
// _$E212
// SYNTHETIC: GAMEMD 0x0058B5A0 SYMBOL
// _$E215
// SYNTHETIC: GAMEMD 0x0058B5C0 SYMBOL
// _$E218
// SYNTHETIC: GAMEMD 0x0058B5E0 SYMBOL
// _$E221
// SYNTHETIC: GAMEMD 0x0058B600 SYMBOL
// _$E224
// SYNTHETIC: GAMEMD 0x0058B640 SYMBOL
// _$E227
// SYNTHETIC: GAMEMD 0x0058B670 SYMBOL
// _$E230
// SYNTHETIC: GAMEMD 0x0058B6A0 SYMBOL
// _$E233
// SYNTHETIC: GAMEMD 0x0058B6D0 SYMBOL
// _$E236
// SYNTHETIC: GAMEMD 0x0058B6E0 SYMBOL
// _$E239
// SYNTHETIC: GAMEMD 0x0058B700 SYMBOL
// _$E242
// SYNTHETIC: GAMEMD 0x0058B720 SYMBOL
// _$E245
const Rect RECT_NONE_2(0, 0, 0, 0);
