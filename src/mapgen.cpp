// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

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

extern int DAT_00ABED0C;
extern int DAT_00ABDFD0;
extern int DAT_00ABDFD4;
extern int DAT_00ABEC90;
extern int DAT_00ABECD0;
extern Cell DAT_00ABE304[8];
extern Point2D DAT_00ABE328[8];

// FUNCTION: 0x0058B740 $E185
// FUNCTION: 0x0058B760 $E183
// FUNCTION: 0x0058B770 $E188
// FUNCTION: 0x0058B780 $E191

// FUNCTION: 0x0058B7B0
void FUN_0058B7B0(void)
{
}

// FUNCTION: 0x0058B7C0
void FUN_0058B7C0(void)
{
}

// FUNCTION: 0x0058B7D0
int FUN_0058B7D0(void)
{
	return DAT_00ABED0C;
}

// FUNCTION: 0x0058B7E0
int FUN_0058B7E0(void)
{
	return DAT_00ABDFD4;
}

// FUNCTION: 0x0058B7F0
int FUN_0058B7F0(void)
{
	return DAT_00ABDFD0;
}

// FUNCTION: 0x0058B800
int FUN_0058B800(void)
{
	return DAT_00ABEC90;
}

// FUNCTION: 0x0058B810
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

// FUNCTION: 0x0058B820
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

// FUNCTION: 0x0058BB00
Point2D FUN_0058BB00(int index)
{
	if (index >= 0 && index < DAT_00ABED0C) {
		return DAT_00ABE328[index];
	}
	return Point2D(0, 0);
}

// FUNCTION: 0x0058BB30
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

// FUNCTION: 0x0058BC20
void FUN_0058BC20(void)
{
	DAT_00ABED0C = 0;
}

DynamicVectorClass<MapRegionClass *> MapRegionClass::MapRegions;

// FUNCTION: 0x0058BC30 $E197
// FUNCTION: 0x0058BC70 $E194

// FUNCTION: 0x0058BCD0
bool FUN_0058BCD0(int value)
{
	return value == 1 || value == 3 || value == 4;
}

static inline bool Is_Three_Or_Four(int value)
{
	return value == 3 || value == 4;
}

// FUNCTION: 0x0058BCF0
bool FUN_0058BCF0(int value)
{
	return !Is_Three_Or_Four(value);
}

// FUNCTION: 0x0058BD20
bool FUN_0058BD20(int value)
{
	return value == 3 || value == 4;
}

// FUNCTION: 0x0058BD40
bool FUN_0058BD40(int value)
{
	return value == 4 || value == 3;
}

// FUNCTION: 0x0058BD60
bool FUN_0058BD60(int value)
{
	return value == 3 || value == 4;
}

// FUNCTION: 0x0058BD80
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

// FUNCTION: 0x0058BDC0
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

// FUNCTION: 0x0058BE20
Cell MapRegionClass::Pick_Random_Map_Cell(void)
{
	int grid_size = MapCellStride * MapCellStride;
	unsigned int index;
	do {
		index = Pick_Random_UInt(0, grid_size - 1);
	} while (RMGCellData[index].CellID == Cell(0,0));
	return(RMGCellData[index].CellID);
}

// FUNCTION: 0x0058BEA0
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

// FUNCTION: 0x0058BF70
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

// FUNCTION: 0x0058C070
MapRegionClass::~MapRegionClass(void)
{
	if (Unknown0 != NULL) {
		delete Unknown0;
		Unknown0 = NULL;
	}
	MapRegions.Delete(this);
}

// FUNCTION: 0x0058C100
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

// FUNCTION: 0x0058C1C0
bool MapRegionClass::CellData::Is_Not_Inviolate(Cell const &cell)
{
	return(!MapRegionClass::Get_Cell_Data(cell).Inviolate);
}

// FUNCTION: 0x0058C1F0
bool MapRegionClass::CellData::Is_Interior(Cell const &cell)
{
	return(MapRegionClass::Get_Cell_Data(cell).Interior);
}

// FUNCTION: 0x0058C210
void MapRegionClass::CellData::Set_Inviolate(Cell const &cell)
{
	MapRegionClass::Get_Cell_Data(cell).Inviolate = true;
}

// FUNCTION: 0x0058C240
void MapRegionClass::CellData::Clear_Inviolate(Cell const &cell)
{
	MapRegionClass::Get_Cell_Data(cell).Inviolate = false;
}

// FUNCTION: 0x0058C270
void MapRegionClass::CellData::Clear_Inviolates(void)
{
	int grid_size = MapCellStride * MapCellStride;
	for (int i = 0; i < grid_size; i++) {
		MapRegionClass::Get_Cell_Data(grid_size).Inviolate = false;
	}
}

// FUNCTION: 0x0058C2A0
MapRegionClass::CellData & MapRegionClass::Get_Cell_Data(Cell const & cell)
{
	return(RMGCellData[cell.X + cell.Y * MapCellStride]);
}

// FUNCTION: 0x0058C2C0
MapRegionClass::CellData & MapRegionClass::Get_Cell_Data(int index)
{
	return(RMGCellData[index]);
}

// FUNCTION: 0x0058C2D0
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

// FUNCTION: 0x0058C320
Cell MapRegionClass::Pick_Random_Cell(void)
{
	int grid_size = MapCellStride * MapCellStride;
	unsigned int index = 0;
	do {
		index = Pick_Random_UInt(0, grid_size - 1);
	} while (RMGCellData[index].RegionID != ID || RMGCellData[index].CellID == Cell(0,0));

	return(RMGCellData[index].CellID);
}

// FUNCTION: 0x0058C3B0
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

// FUNCTION: 0x0058C4A0
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

// FUNCTION: 0x0058C5A0
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

// FUNCTION: 0x0058C6C0
void MapRegionClass::CellData::Reset_Spreads(bool)
{
	int grid_size = MapCellStride * MapCellStride;
	for (int i = 0; i < grid_size; i++) {
		RMGCellData[i].SpreadID = -1;
	}
}

// FUNCTION: 0x0058C6F0
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

// FUNCTION: 0x0058CE90
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

// FUNCTION: 0x0058CF90
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

// FUNCTION: 0x0058D010
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

// FUNCTION: 0x0058D070
void MapRegionClass::CellData::Set_Region(Cell const &cell, int region_id)
{
	if (RMGCellData != NULL) {
		CellData & data = MapRegionClass::Get_Cell_Data(cell);
		data.RegionID = region_id;
	}
}

// FUNCTION: 0x0058D0A0
int MapRegionClass::CellData::Get_Region(Cell const &cell)
{
	if (RMGCellData == NULL) {
		return(-1);
	}
	return(MapRegionClass::Get_Cell_Data(cell).RegionID);
}

// FUNCTION: 0x0058D0D0
void MapRegionClass::CellData::Set_Spread(Cell const &cell, int patch_id)
{
	MapRegionClass::Get_Cell_Data(cell).SpreadID = patch_id;
}

// FUNCTION: 0x0058D100
int MapRegionClass::CellData::Get_Spread(Cell const &cell)
{
	return(MapRegionClass::Get_Cell_Data(cell).SpreadID);
}

// FUNCTION: 0x0058D120
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

// FUNCTION: 0x0058D1C0
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

// FUNCTION: 0x0058D410
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

// FUNCTION: 0x0058D5A0
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

// FUNCTION: 0x0058E5D0
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

// FUNCTION: 0x0058E740
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

// FUNCTION: 0x0058E9B0
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

// FUNCTION: 0x0058EBC0
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

// FUNCTION: 0x0058EEA0
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

// FUNCTION: 0x0058EF10
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

// FUNCTION: 0x0058F0C0
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

// FUNCTION: 0x005902C0
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

// FUNCTION: 0x00595CE0
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

// FUNCTION: 0x00597A10
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

// FUNCTION: 0x00597D50
bool MapSeedClass::Delete_File(const char * file_name)
{
	return(BASECLASS::Delete_File(file_name));
}

// FUNCTION: 0x00598000
double Random_Fraction(void)
{
	return RMG_RANDOM_DOUBLE(1);
}

// FUNCTION: 0x00598030
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

// FUNCTION: 0x005980A0
NormalDistribution::NormalDistribution(double (*function)(void))
{
	HasSpare = false;
	Spare = 0;
	Rand = function;
}

// FUNCTION: 0x005980C0
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
	s = fastmath::sqrt((-l - l) / s);
	Spare = s * v;
	HasSpare = true;
	return(s * u);
}

// FUNCTION: 0x0059D480
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

// FUNCTION: 0x005A0090
void MapGeneratorClass::Set_Cell_Data_Region(Cell const &cell, int region_id)
{
	MapRegionClass::CellData::Set_Region(cell, region_id);
}

// FUNCTION: 0x005A00C0
int MapGeneratorClass::Get_Cell_Data_Region(Cell const &cell)
{
	return(MapRegionClass::CellData::Get_Region(cell));
}

// FUNCTION: 0x005A0100
void MapGeneratorClass::Set_Cell_Data_Spread(Cell const &cell, int patch_id)
{
	MapRegionClass::CellData::Set_Spread(cell, patch_id);
}

// FUNCTION: 0x005A1190
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

// FUNCTION: 0x005A1E50
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

// FUNCTION: 0x005A1F60
bool MapGeneratorClass::Expand_All_High_Ground(void)
{
	bool success = true;
	Map.Set_Cursor_Shape(NULL);
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

// FUNCTION: 0x005A5AE0
double MapGeneratorClass::Get_Urban_Score(const Cell & cell1, const Cell & cell2) const
{
	static const double _random_range = 3.0;

	int x = cell1.X - cell2.X;
	int y = cell1.Y - cell2.Y;
	double dist = MAX(abs(x), abs(y));
	return(RMG_RANDOM_DOUBLE(_random_range) + dist);
}

// FUNCTION: 0x005A6BA0
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

// FUNCTION: 0x005A7250
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

// FUNCTION: 0x005A7640
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

// FUNCTION: 0x005A9920
DirtRoadLink::DirtRoadLink(Cell const & cell, FacingType dir) :
	Offset(cell),
	Facing(dir)
{
}

// FUNCTION: 0x005A9940
DirtRoadLink::DirtRoadLink(void) :
	Offset(0, 0),
	Facing(FACING_NONE)
{
}

// FUNCTION: 0x005A9960
DirtRoadLink::DirtRoadLink(DirtRoadLink const & that) :
	Offset(that.Offset),
	Facing(that.Facing)
{
}

// FUNCTION: 0x005A9980
DirtRoadNode::DirtRoadNode(IsometricTileType road_tile, Cell const & origin, IsometricTileType cap_tile, Cell const & cap_origin, int link_index) :
	Tile(road_tile),
	Origin(origin),
	CapTile(cap_tile),
	CapOrigin(cap_origin),
	BackLinkIndex(link_index)
{
}

// FUNCTION: 0x005A99B0
DirtRoadNode::DirtRoadNode(void) :
	Tile(ISOTILE_NONE),
	Origin(Cell(0, 0)),
	CapTile(ISOTILE_NONE),
	CapOrigin(Cell(0, 0)),
	BackLinkIndex(-1)
{
}

// FUNCTION: 0x005A99F0
DirtRoadNode::DirtRoadNode(DirtRoadNode const & that) :
	Tile(that.Tile),
	Origin(that.Origin),
	CapTile(that.CapTile),
	CapOrigin(that.CapOrigin),
	BackLinkIndex(that.BackLinkIndex)
{
}

// FUNCTION: 0x005A9A20
DirtRoadTile::DirtRoadTile(DirtRoadLink const & link1) :
	ITType(ISOTILE_CLEAR)
{
	Count = 1;
	Links = new DirtRoadLink[1];
	Links[0] = link1;
}

// FUNCTION: 0x005A9A70
DirtRoadTile::DirtRoadTile(DirtRoadLink const & link1, DirtRoadLink const & link2) :
	ITType(ISOTILE_CLEAR)
{
	Count = 2;
	Links = new DirtRoadLink[2];
	Links[0] = link1;
	Links[1] = link2;
}

// FUNCTION: 0x005A9AE0
DirtRoadTile::DirtRoadTile(DirtRoadLink const & link1, DirtRoadLink const & link2, DirtRoadLink const & link3) :
	ITType(ISOTILE_CLEAR)
{
	Count = 3;
	Links = new DirtRoadLink[3];
	Links[0] = link1;
	Links[1] = link2;
	Links[2] = link3;
}

// FUNCTION: 0x005A9B60
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

// FUNCTION: 0x005A9BF0
DirtRoadTile::DirtRoadTile()
{
	Count = 0;
	Links = NULL;
}

// FUNCTION: 0x005A9C00
DirtRoadTile::~DirtRoadTile(void)
{
	delete Links;
	Links = NULL;
}
