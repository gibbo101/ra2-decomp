// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "map.h"
#include "_alpha.h"
#include "_map.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_tactica.h"
#include "_zbuffer.h"
#include "anim.h"
#include "astar.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "conquer.h"
#include "dbgprint.h"
#include "foot.h"
#include "globals.h"
#include "hashtable.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "inline.h"
#include "isotype.h"
#include "lcwpipe.h"
#include "lcwstraw.h"
#include "lzopipe.h"
#include "lzostraw.h"
#include "mapgen.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "psystype.h"
#include "rules.h"
#include "smartdeform.h"
#include "swap.h"
#include "tactical.h"
#include "tag.h"
#include "tiberium.h"
#include "tube.h"
#include "vein.h"
#include "zbuffer.h"
#include "multiplayer.h"
#include "overlay.hh"
#include "ramp.hh"
#include <new>
#include <cmath>

int SubzoneHash(unsigned int const & key);
extern int DAT_00ABDE88;

#define IS_TILE_IN_SET(setname, setsize) (tile >= IsometricTileTypeClass::setname && tile < IsometricTileTypeClass::setname + setsize)

extern void FUN_00561910(void);
extern void FUN_005638D0(void);

extern int DAT_007ED3CC[];
extern int DAT_007ED3D0[];
namespace Unsorted { extern Cell CellSpreadTable[]; }

// FUNCTION: 0x005616F0 $E2
// FUNCTION: 0x00561710 $E5
// FUNCTION: 0x00561740 $E8
// FUNCTION: 0x00561760 $E11
// FUNCTION: 0x00561780 $E14
// FUNCTION: 0x005617A0 $E17
// FUNCTION: 0x005617C0 $E20
// FUNCTION: 0x005617E0 $E23
// FUNCTION: 0x00561820 $E26
// FUNCTION: 0x00561850 $E29
// FUNCTION: 0x00561880 $E32
// FUNCTION: 0x005618B0 $E35
// FUNCTION: 0x005618C0 $E38
// FUNCTION: 0x005618E0 $E41

// FUNCTION: 0x00561900
void Init_00561900(void)
{
	FUN_00561910();
}

// FUNCTION: 0x005638C0
void Init_005638C0(void)
{
	FUN_005638D0();
}

// FUNCTION: 0x00565060 $E223
// FUNCTION: 0x00565080 $E221
CellClass InvalidCell;

// FUNCTION: 0x00565090
MapClass::MapClass(void)
{
	MaxLevel = 13;
}

// FUNCTION: 0x00565190
MapClass::MapClass(NoInitClass const & x) :
	BASECLASS(x),
	unknown_10(false),
	ZoneAdjacency(NULL),
	ZoneConnections(),
	PendingBridgeCells(),
	Array(x),
	Crates(),
	TaggedCells()
{
	int i;

	for (i = 0; i < MZONE_COUNT; i++) {
		Zones[i] = NULL;
	}
	for (i = 0; i < SUBZONE_COUNT; i++) {
		SubzoneConnectionHashTable[i] = NULL;
	}
	for (i = 0; i < SUBZONE_COUNT; i++) {
		SubzoneTracking[i].Clear();
		SubzoneConnectionHashTable[i] = NULL;
	}
	ZoneAdjacency = NULL;
}

// FUNCTION: 0x005652C0
MapClass::~MapClass(void)
{
	VeinholeMonsterClass::Reset();
	VeinholeMonsterClass::Clear_Global_Data();
	TiberiumClass::Deinit_Tiberium_Spread_System();
	TiberiumClass::Deinit_Tiberium_Growth_System();

	if (CellZones != NULL) {
		delete [] CellZones;
		CellZones = NULL;
	}

	if (ZoneAdjacency != NULL) {
		delete ZoneAdjacency;
		ZoneAdjacency = NULL;
	}

	if (CellSubzones != NULL) {
		delete [] CellSubzones;
		CellSubzones = NULL;
	}

	for (int i = 0; i < SUBZONE_COUNT; i++) {
		if (SubzoneConnectionHashTable[i] != NULL)  {
			delete SubzoneConnectionHashTable[i];
			SubzoneConnectionHashTable[i] = NULL;
		}
	}
}

// FUNCTION: 0x00565470
Point2D MapClass::PlayRect_To_Cell_Point(Point2D const & point) const
{
	int x = ((point.Y + 1) >> 1) + point.X;
	int y = ((point.Y >> 1) - point.X) + PlayRect.Width;
	return(Point2D(x, y));
}

// FUNCTION: 0x005654A0
Point2D MapClass::LocalRect_To_Cell_Point(Point2D const & point) const
{
	return(PlayRect_To_Cell_Point(point + LocalRect.Top_Left()));
}

// FUNCTION: 0x005654E0
Point2D MapClass::Cell_To_PlayRect_Point(Point2D const & point) const
{
	int px = point.X;
	int py = point.Y;
	int w = PlayRect.Width;

	int d;
	if ((w & 1) != 0) {
		d = px - py + 1;
	} else {
		d = px - py;
	}
	int x = w / 2 + (d >> 1);
	int y = px + py - w;
	return(Point2D(x, y));
}

// FUNCTION: 0x00565520
Point2D MapClass::Cell_To_LocalRect_Point(Point2D const & point) const
{
	return(Cell_To_PlayRect_Point(point) - LocalRect.Top_Left());
}

// FUNCTION: 0x00565580
Cell MapClass::PlayRect_To_Cell(Cell const & cell)
{
	Point2D pt = PlayRect_To_Cell_Point(Point2D(cell.X, cell.Y));
	return(Cell(pt.X, pt.Y));
}

// FUNCTION: 0x005655C0
Cell MapClass::LocalRect_To_Cell(Cell const & cell)
{
	Point2D pt = LocalRect_To_Cell_Point(Point2D(cell.X, cell.Y));
	return(Cell(pt.X, pt.Y));
}

// FUNCTION: 0x00565610
Cell MapClass::Cell_To_PlayRect(Cell const & cell)
{
	Point2D pt = Cell_To_PlayRect_Point(Point2D(cell.X, cell.Y));
	return(Cell(pt.X, pt.Y));
}

// FUNCTION: 0x00565660
Cell MapClass::Cell_To_LocalRect(Cell const & cell)
{
	Point2D pt = Cell_To_LocalRect_Point(Point2D(cell.X, cell.Y));
	return(Cell(pt.X, pt.Y));
}

// FUNCTION: 0x005656D0
LONG STDMETHODCALLTYPE MapClass::Is_Visible(CellStruct cell)
{
	return((*this)[cell].IsFogVisible);
}

// FUNCTION: 0x00565730
CellClass & MapClass::operator[](Coord const & coord) const
{
	int x = coord.X / CELL_LEPTON_W;
	int y = coord.Y / CELL_LEPTON_H;

	int cellnum = x + y * MAP_CELL_H;

	if (cellnum >= 0 && cellnum < Array.Length() && Array[cellnum] != NULL) {
		return(*Array[cellnum]);
	}

	InvalidCell.CellID = Cell(x, y);
	return(InvalidCell);
}

// FUNCTION: 0x005657A0
CellClass & MapClass::operator[](Cell const & cell) const
{
	int cellnum = cell.X + cell.Y * MAP_CELL_H;

	if (cellnum >= 0 && cellnum < MAP_CELL_TOTAL && Array[cellnum] != NULL) {
		return(*Array[cellnum]);
	}

	InvalidCell.CellID = cell;
	return(InvalidCell);
}

// FUNCTION: 0x005657E0
bool MapClass::Is_Valid(Cell const & cell)
{
	int cellnum = cell.X + cell.Y * MAP_CELL_H;

	if (Array[cellnum] != NULL) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00565800
void MapClass::One_Time(void)
{
	BASECLASS::One_Time();

	XSize = MAP_CELL_W;
	YSize = MAP_CELL_H;
	Size = XSize * YSize;

	Alloc_Cells();

	if (ZoneAdjacency == NULL) {
		ZoneAdjacency = new ZONE_PAIR_HASH_SET(20, 256, SubzoneHash);
	}

	int i;
	for (i = 0; i < ARRAY_SIZE(SubzoneTracking); i++) {
		SubzoneTracking[i].Clear();
		SubzoneConnectionHashTable[i] = new SUBZONE_CONNECTION_HASH_SET(20, 256, SubzoneHash);
	}

	for (i = 0; i < MZONE_COUNT; i++) {
		Zones[i] = NULL;
	}
}

// FUNCTION: 0x005659F0
void MapClass::Init_Clear(void)
{
	DebugString("MapClass::Init_Clear entry\n");
	Call_Back();
	DepthBuffer->Fill(0xFFFF);
	AlphaBuffer->Fill(127);
	Call_Back();
	BASECLASS::Init_Clear();
	Call_Back();
	for (int index = 0; index < ARRAY_SIZE(Crates); index++) {
		Crates[index].Init();
	}
	MaxLevel = 13;
	Redraws = 0;
	DebugString("MapClass::Init_Clear done\n");
}

// FUNCTION: 0x00565AA0
void MapClass::Alloc_Cells(void)
{
	new (&Array) VectorClass<CellClass *>;
	Array.Resize(Size);

	for (int i = 0; i < Size; i++) {
		Array[i] = NULL;
	}
}

// FUNCTION: 0x00565B00
void MapClass::Free_Cells(void)
{
	int i;

	for (i = 0; i < Array.Length(); i++) {
		CellClass * cptr = Array[i];
		if (cptr != NULL) {
			cptr->BridgeDeckCell = NULL;
			cptr->UnusedCell = 0;
		}
	}

	for (i = 0; i < Array.Length(); i++) {
		if (Array[i] != NULL) {
			delete Array[i];
			Array[i] = NULL;
		}
	}

	IsometricTileTypeClass::Free_Unused_Drawers(0, true);

	if (Array.Length() < MAP_CELL_TOTAL) {
		Array.Resize(MAP_CELL_TOTAL);
		for (i = 0; i < Array.Length(); i++) {
			Array[i] = NULL;
		}
	}
}

// FUNCTION: 0x00565BC0
void MapClass::Init_Cells(void)
{
	TotalValue = 0;
	for (int y = 0; y < MAP_CELL_H; y++) {
		for (int x = 0; x < MAP_CELL_W; x++) {
			int cellnum = x + y * MAP_CELL_H;
			if (cellnum < Array.Length() && Array[cellnum] != NULL) {
				new (Array[cellnum]) CellClass;
			}
		}
	}
}

// FUNCTION: 0x00567110
void MapClass::Fresh_Map(void)
{
	int i;

	if (CellZones != NULL) {
		delete [] CellZones;
		CellZones = NULL;
	}
	if (CellSubzones != NULL) {
		delete [] CellSubzones;
		CellSubzones = NULL;
	}

	CellZoneCount = (PlayRect.Width + PlayRect.Height + 1) * (PlayRect.Width + PlayRect.Height + 1);

	CellZones = new CellZoneStruct[CellZoneCount];
	CellSubzones = new CellSubzoneStruct[CellZoneCount];

	Search.Update_Map_Dimensions(PlayRect);

	for (i = 0; i < SUBZONE_COUNT; i++) {
		int v = (1 << (i + 1));
		SubzoneTracking[i].Set_Growth_Step((4 * PlayRect.Width * PlayRect.Height) / (v * v));
	}

	Overpass();
	Compute_Zone_Connections();
	Zone_Reset();
	for (int index = SUBZONE_COUNT - 1; index >= 0; index--) {
		SubzoneTracking[index].Clear();
		Reset_Subzone(index);
	}
	Search.Reset();
}

// FUNCTION: 0x00567230
void MapClass::Set_Local_Dimensions(Rect const & size)
{
	LocalRect = Intersect(size, PlayRect);
	LocalRect.X = MAX(LocalRect.X, 2);
	LocalRect.Y = MAX(LocalRect.Y, 2);
	LocalRect.Width = MIN(LocalRect.Width, PlayRect.Width - LocalRect.X - 2);
	LocalRect.Height = MIN(LocalRect.Height, PlayRect.Height - LocalRect.Y - (2 * 3));

	Map.Flag_To_Redraw(GS_REDRAW_ALL);
	for (int i = 0; i < Technos.Count(); i++) {
		TechnoClass * t = Technos[i];
		bool was = t->IsInPlayfield;
		t->IsInPlayfield = In_Local_Radar(t->PositionCell);
		if (!was && t->IsInPlayfield && t->House->Is_Player_Control() && t->RTTI != RTTI_BUILDING && t->IsActive && !t->IsInLimbo) {
			t->Look();
		}
	}
}

// FUNCTION: 0x00567370
void MapClass::RevealArea0(int, int, Coord * coord, int sightrange, HouseClass * house, bool a, bool b, bool c, bool d)
{
	RevealArea1(coord, sightrange, house, a, b, c, d, true);
}

// FUNCTION: 0x005678E0
void MapClass::RevealArea2(Coord * xcoord, int sightrange, HouseClass * house, bool incremental, bool dont_map, bool unfog, bool byheight, bool increase)
{
	int cell_height = xcoord->Z / DAT_00ABDE88;

	Coord coord = *xcoord;
	coord.X = coord.X + (Tactical::Z_Lepton_To_Pixel(coord.Z) / -30) * CELL_LEPTON;
	coord.Y = coord.Y + (Tactical::Z_Lepton_To_Pixel(coord.Z) / -30) * CELL_LEPTON;

	Cell cell = coord.As_Cell();
	Cell hoffset = cell - xcoord->As_Cell() - Cell(2, 2);

	bool edge = (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.MPGameMode != NULL && !Session.MPGameMode->vt_entry_04();

	if (!In_Radar(cell)) return;
	if (!sightrange) return;
	if (sightrange >= 11 || sightrange > 10) {
		sightrange = 10;
	}

	int count = RadiusCount[sightrange];
	Cell const * ptr = &RadiusOffset[0];
	Cell const * ptr2 = &OcclusionOffset[0];
	if (!Rule->IsRevealByHeight && incremental) {
		if (sightrange > 2) {
			ptr += RadiusCount[sightrange - 3];
			ptr2 += RadiusCount[sightrange - 3];
			count -= RadiusCount[sightrange - 3];
		}
	}
	ptr2--;

	if (house != NULL && PlayerPtr != NULL) {
		if (house != PlayerPtr) {
			if ((house->RadarSpied & (1 << PlayerPtr->Class->House)) != 0) {
				house = PlayerPtr;
			}
		}
		if (house != PlayerPtr) {
			if (house->Is_Ally(PlayerPtr) && Rule->IsAllyReveal) {
				house = PlayerPtr;
			}
		}
	}

	if (house != PlayerPtr) {
		return;
	}

	while (count--) {
		Cell newcell;
		ptr2++;
		newcell = cell + *ptr++;

		if (!In_Radar(newcell)) continue;
		if (abs(newcell.X - cell.X) > sightrange) continue;
		if (Distance(newcell, cell) > sightrange) continue;

		CellClass * cellptr = &(*this)[newcell];
		if (byheight && Rule->IsRevealByHeight) {
			Cell offcell;
			offcell.X = newcell.X - hoffset.X;
			offcell.Y = newcell.Y - hoffset.Y;
			if (Map[*ptr2 + offcell].Height > cell_height + 3) continue;
		}

		int cx = cellptr->CellID.X;
		int cy = cellptr->CellID.Y;
		cellptr->IsToFog = false;
		if (unfog) {
			if (cellptr->IsPlot && cellptr->IsCursorHere) continue;
			if (!cellptr->IsFogVisible) continue;
			Map.Fog_Map_Cell(newcell, house);
		} else {
			if (edge) {
				if (cx == 7) {
					if (cy == PlayRect.Width + 5) continue;
				} else if (cx == 13) {
					if (cy == PlayRect.Width + 11) continue;
				}
				if (cx == PlayRect.Height + 13) {
					if (cy == PlayRect.Width + PlayRect.Height - 15) continue;
				}
			}
			Map.RevealFogShroud(newcell, house, increase);
		}
	}
}

static inline void Reveal_Radius(int & height, int & radius)
{
	if (radius > 11) {
		radius = 11;
	} else if (radius < 3) {
		radius = 3;
	}

	if (height < 0) {
		height = 0;
		goto base;
	}
	if (height > 8) {
		height = 8;
	}
	if (height == 0) {
base:
		radius = DAT_007ED3D0[radius];
	} else {
		radius = DAT_007ED3D0[radius] - DAT_007ED3CC[height];
	}
}

// FUNCTION: 0x00567DA0
void MapClass::RevealArea3(Coord const & coord, int height, int radius, int skipreveal)
{
	Reveal_Radius(height, radius);

	if (!skipreveal) {
		Coord c = coord;
		c.X += (Tactical::Z_Lepton_To_Pixel(c.Z) / -30) * CELL_LEPTON;
		c.Y += (Tactical::Z_Lepton_To_Pixel(c.Z) / -30) * CELL_LEPTON;
		Cell center = c.As_Cell();
		Cell const * ptr = Unsorted::CellSpreadTable;
		if (height != 0) {
			ptr = &Unsorted::CellSpreadTable[DAT_007ED3CC[height]];
		}
		while (radius--) {
			Cell cell = *ptr++ + center;
			CellClass * cptr = &(*this)[cell];
			char occ = TacticalMap->Cell_Shadow(cell, false);
			if (occ != cptr->ShadowFrame) {
				cptr->ShadowFrame = occ;
				cptr->VisibilityChanged = true;
				TacticalMap->Flag_Cell(*cptr);
			}
		}
	}
}

// FUNCTION: 0x00567F70
void MapClass::Shroud_Gap_Area(Coord const & coord, int radius)
{
	Coord c = coord;
	c.X += (Tactical::Z_Lepton_To_Pixel(c.Z) / -30) << 8;
	c.Y += (Tactical::Z_Lepton_To_Pixel(c.Z) / -30) << 8;
	Cell center = c.As_Cell();

	for (int dx = -2 - radius; dx < radius + 2; dx++) {
		for (int dy = -2 - radius; dy < radius + 2; dy++) {
			if (dx * dx + dy * dy < (radius + 1) * (radius + 1)) {
				Cell cell = center + Cell(dx, dy);
				if (Map[cell].ShroudCounter == 0) {
					Map[cell].ShroudCounter = 1;
					Map[cell].IsFogVisible = false;
					Map[cell].IsAltUnknown = false;
				}
			}
		}
	}
}

// FUNCTION: 0x00568140
void MapClass::Unshroud_Gap_Area(Coord const & coord, int radius)
{
	Coord c = coord;
	c.X += (Tactical::Z_Lepton_To_Pixel(c.Z) / -30) << 8;
	c.Y += (Tactical::Z_Lepton_To_Pixel(c.Z) / -30) << 8;
	Cell center = c.As_Cell();

	for (int dx = -2 - radius; dx < radius + 2; dx++) {
		for (int dy = -2 - radius; dy < radius + 2; dy++) {
			if (dx * dx + dy * dy < (radius + 1) * (radius + 1)) {
				Cell cell = center + Cell(dx, dy);
				Map[cell].ShroudCounter = 0;
				Map[cell].GapsCoveringThisCell = 0;
				Map[cell].IsAltUnknown = true;
				Map[cell].IsFogVisible = true;
			}
		}
	}
}

// FUNCTION: 0x00568300
bool MapClass::In_Radar(Cell const & cell) const
{
	int x = cell.X;
	int y = cell.Y;
	if ((x + y > PlayRect.Width) &&
		(x - y < PlayRect.Width) &&
		(y - x < PlayRect.Width) &&
		(x + y <= PlayRect.Width + 2 * PlayRect.Height)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00568350
bool MapClass::In_Radar(Coord const & coord) const
{
	return(In_Radar(coord.As_Cell()));
}

static inline BuildingClass * Building_If_Building(AbstractClass * t)
{
	return(t->RTTI == RTTI_BUILDING ? (BuildingClass *)t : (BuildingClass *)0);
}

#define Cell_In_Range(c, len) ((c).X + (c).Y * 512 >= 0 && (c).X + (c).Y * 512 < (len) && Array[(c).X + (c).Y * 512] != NULL)

// FUNCTION: 0x005687F0
void MapClass::RemoveContentAt(Cell const * coords, TechnoClass * content)
{
	if (!content) return;
	Cell const * list;
	int height;
	int length = Array.Length();

	if (content->Class_Of()->IsFootprint && content->In_Which_Layer() == LAYER_GROUND) {
		Cell xlist[32];
		List_Copy(content->Occupy_List(), ARRAY_SIZE(xlist), xlist);
		for (list = xlist; *list != REFRESH_EOL; list++) {
			Cell newcell = *coords + *list;
			if (Cell_In_Range(newcell, length)) {
				(*this)[newcell].Occupy_Up(content, content->IsOnBridge);
				(*this)[newcell].Recalc_Attributes();
			}
		}

		if (content->RTTI == RTTI_BUILDING) {
			BuildingClass * building = Building_If_Building(content);
			if (building->Class->CanHideThings) {
				height = building->Class->OccupyHeight - 1;
				if (height <= 0) height = 1;

				char seen[2048];
				memset(seen, 0, sizeof(seen));
				for (list = xlist; *list != REFRESH_EOL; list++) {
					Cell newcell = *coords + *list;
					int rx = list->X;
					int ry = list->Y;
					for (int i = height; i > 0; i--) {
						if (newcell.X < 0 || newcell.Y < 0) continue;
						if (rx < 0) rx += 16;
						if (ry < 0) ry += 16;
						if (!seen[ry * 16 + rx]) {
							seen[ry * 16 + rx] = 1;
							if (Cell_In_Range(newcell, length)) {
								CellClass & cell = (*this)[newcell];
								if (cell.OccupyHeightsCoveringMe) cell.OccupyHeightsCoveringMe--;
							}
						}
						newcell.X--;
						newcell.Y--;
						rx--;
						ry--;
					}
				}

				for (int i = 0; i < 8; i++) {
					if (building->Class->RemoveOccupy[i].X != 0xFFFF || building->Class->RemoveOccupy[i].Y != 0xFFFF) {
						Cell newcell = *coords;
						newcell.X += building->Class->RemoveOccupy[i].X;
						newcell.Y += building->Class->RemoveOccupy[i].Y;
						if (Cell_In_Range(newcell, length)) {
							CellClass & cell = (*this)[newcell];
							if (cell.OccupyHeightsCoveringMe) cell.OccupyHeightsCoveringMe--;
						}
					}
				}
			}
		}
	}
}

// FUNCTION: 0x00568BB0
int MapClass::Overpass(bool flag)
{
	int i;
	int value = 0;

	for (i = 0; i < AnimClass::Array.Count(); i++) {
		if (AnimClass::Array[i]->DeleteOnMapCleanup) {
			delete AnimClass::Array[i];
			i--;
		}
	}

	Reset_Iterator();

	CellClass * iter = Iterate();
	while (iter) {
		iter->IsHorizontalLine = false;
		iter->IsVerticalLine = false;
		iter = Iterate();
	}

	Reset_Iterator();

	iter = Iterate();
	while (iter) {
		iter->UnusedCell = NULL;
		iter->Init_Drawer();
		iter->IsAnimAttached = false;

		if (iter->Tag) {
			if (iter->Tag->Is_Cross_Horizontal()) {
				for (i = 0; i < Map.MapRect.Width; i++) {
					Map[Cell(i + MapRect.X, iter->CellID.Y)].IsHorizontalLine = true;
				}
			} else if (iter->Tag->Is_Cross_Vertical()) {
				for (i = 0; i < Map.MapRect.Height; i++) {
					Map[Cell(iter->CellID.X, i + MapRect.Y)].IsVerticalLine = true;
				}
			}
		}

		if (flag) {
			value += iter->Tiberium_Adjust(false);
		} else {
			value += iter->Tiberium_Value();
		}
		iter->Recalc_Attributes();
		if (iter->Overlay != OVERLAY_NONE) {
			if (OverlayTypes[iter->Overlay]->IsWall) {
				iter->Set_Wall_Owner();
			}
		}

		iter = Iterate();
	}
	return(value);
}

// FUNCTION: 0x0056A080
void MapClass::Repair_All_High_Bridges(void)
{
	for (int y = MapRect.Y; y < MapRect.Y + MapRect.Height; y++) {
		for (int x = MapRect.X; x < MapRect.X + MapRect.Width; x++) {
			Cell cell(x, y);

			if (!Is_Valid(cell)) {
				continue;
			}
			bool valid = Is_Valid(cell);
			CellClass * cptr = &Map[cell];

			int bridgetile = cptr->ITType - IsometricTileTypeClass::BridgeSet + 1;

			if ((bridgetile == IsometricTileTypeClass::BridgeTopLeft1 || bridgetile == IsometricTileTypeClass::BridgeTopLeft2) && cptr->SubTile == 8) {
				Repair_High_Bridge_Span(cptr, FACING_E);
			}

			if ((bridgetile == IsometricTileTypeClass::BridgeMiddle1 || bridgetile == IsometricTileTypeClass::BridgeMiddle1 + 3 || bridgetile == IsometricTileTypeClass::BridgeMiddle1 + 4 || bridgetile == IsometricTileTypeClass::BridgeMiddle1 + 1 || bridgetile == IsometricTileTypeClass::BridgeMiddle1 + 2) && cptr->SubTile == 5) {

				if (bridgetile == IsometricTileTypeClass::BridgeMiddle1 + 4) {
					Set_Bridge_Middle_State(cell, IsometricTileType(IsometricTileTypeClass::BridgeMiddle1 + IsometricTileTypeClass::BridgeSet - 1), ISOTILE_INVALID, -1, false);

					Cell base = Adjacent_Cell(cell, FACING_W);
					Map[base].Height += BRIDGE_CELL_HEIGHT;
					Map[Adjacent_Cell(base, FACING_N)].Height += BRIDGE_CELL_HEIGHT;
					Map[Adjacent_Cell(base, FACING_S)].Height += BRIDGE_CELL_HEIGHT;
				}

				Repair_High_Bridge_Span(cptr, FACING_E);
			}

			if ((bridgetile == IsometricTileTypeClass::BridgeTopRight1 || bridgetile == IsometricTileTypeClass::BridgeTopRight2) && cptr->SubTile == 12) {
				Repair_High_Bridge_Span(cptr, FACING_S);
			}

			if ((bridgetile == IsometricTileTypeClass::BridgeMiddle2 || bridgetile == IsometricTileTypeClass::BridgeMiddle2 + 3 || bridgetile == IsometricTileTypeClass::BridgeMiddle2 + 4 || bridgetile == IsometricTileTypeClass::BridgeMiddle2 + 2 || bridgetile == IsometricTileTypeClass::BridgeMiddle2 + 1) && cptr->SubTile == 7) {

				if (bridgetile == IsometricTileTypeClass::BridgeMiddle2 + 4) {
					Set_Bridge_Middle_State(cell, IsometricTileType(IsometricTileTypeClass::BridgeMiddle2 + IsometricTileTypeClass::BridgeSet - 1), ISOTILE_INVALID, -1, false);

					Cell base = Adjacent_Cell(cell, FACING_N);
					Map[base].Height += BRIDGE_CELL_HEIGHT;
					Map[Adjacent_Cell(base, FACING_E)].Height += BRIDGE_CELL_HEIGHT;
					Map[Adjacent_Cell(base, FACING_W)].Height += BRIDGE_CELL_HEIGHT;
				}

				Repair_High_Bridge_Span(cptr, FACING_S);
			}
		}
	}
}

// FUNCTION: 0x0056AE40
int MapClass::Write_Binary_1(Pipe & pipe)
{
	int total = 0;

	LCWPipe comp(LCWPipe::COMPRESS);
	comp.Put_To(&pipe);

	for (int i = 0; i < Array.Length(); i++) {
		CellClass * cellptr = Array[i];
		if (cellptr) {
			total += comp.Put(&cellptr->ITType, 2);
		} else {
			total += comp.Put(&InvalidCell.ITType, 2);
		}
	}

	for (int j = 0; j < Array.Length(); j++) {
		CellClass * cellptr = Array[j];
		if (cellptr) {
			total += comp.Put(&cellptr->SubTile, 1);
		} else {
			total += comp.Put(&InvalidCell.SubTile, 1);
		}
	}

	for (int k = 0; k < Array.Length(); k++) {
		CellClass * cellptr = Array[k];
		if (cellptr) {
			total += comp.Put(&cellptr->Height, 1);
		} else {
			total += comp.Put(&InvalidCell.Height, 1);
		}
	}

	return(total);
}

// FUNCTION: 0x0056AF40
int MapClass::Write_Binary_2(Pipe & pipe)
{
	int total = 0;

	LCWPipe comp(LCWPipe::COMPRESS);
	comp.Put_To(&pipe);

	int i;
	for (i = 0; i < Array.Length(); i++) {
		CellClass * cellptr = Array[i];
		if (i != 0 && cellptr) {
			if (In_Radar(cellptr->Fetch_CellID())) {
				if (cellptr->Fetch_CellID() != CELL_NONE && cellptr != &InvalidCell && (cellptr->ITType != ISOTILE_NONE || cellptr->Height > 0)) {
					Cell cell = cellptr->Fetch_CellID();
					total += comp.Put(&cell, sizeof(cell));
					total += comp.Put(&cellptr->ITType, sizeof(cellptr->ITType));
					total += comp.Put(&cellptr->SubTile, sizeof(cellptr->SubTile));
					total += comp.Put(&cellptr->Height, sizeof(cellptr->Height));
				}
			}
		}
	}

	Cell empty = CELL_NONE;
	total += comp.Put(&empty, sizeof(empty));
	for (i = 0; i < 100; i++) {
		total += comp.Put(&empty, sizeof(empty));
	}
	comp.End();

	return(total);
}

// FUNCTION: 0x0056B0F0
int MapClass::Write_Binary_3(Pipe & pipe)
{
	int total = 0;

	for (int i = 0; i < Array.Length(); i++) {
		CellClass * cellptr = Array[i];
		if (i != 0 && cellptr) {
			if (In_Radar(cellptr->Fetch_CellID())) {
				if (cellptr->Fetch_CellID() != CELL_NONE && cellptr != &InvalidCell && (cellptr->ITType != ISOTILE_NONE || cellptr->Height > 0)) {
					Cell cell = cellptr->Fetch_CellID();
					total += pipe.Put(&cell, sizeof(cell));
					total += pipe.Put(&cellptr->ITType, sizeof(cellptr->ITType));
					total += pipe.Put(&cellptr->SubTile, sizeof(cellptr->SubTile));
					total += pipe.Put(&cellptr->Height, sizeof(cellptr->Height));
				}
			}
		}
	}

	Cell empty = CELL_NONE;
	total += pipe.Put(&empty, sizeof(empty));

	return(total);
}

// FUNCTION: 0x0056B250
int MapClass::Write_Binary_4(Pipe & pipe)
{
	LZOPipe comp(LZOPipe::COMPRESS);
	comp.Put_To(&pipe);
	int total = 0;

	for (int i = 0; i < Array.Length(); i++) {
		CellClass * cellptr = Array[i];
		if (i != 0 && cellptr) {
			if (In_Radar(cellptr->Fetch_CellID())) {
				if (cellptr->Fetch_CellID() != CELL_NONE && cellptr != &InvalidCell && (cellptr->ITType != ISOTILE_NONE || cellptr->Height > 0)) {
					Cell cell = cellptr->Fetch_CellID();
					total += comp.Put(&cell, sizeof(cell));
					total += comp.Put(&cellptr->ITType, sizeof(cellptr->ITType));
					total += comp.Put(&cellptr->SubTile, sizeof(cellptr->SubTile));
					total += comp.Put(&cellptr->Height, sizeof(cellptr->Height));
				}
			}
		}
	}

	Cell empty = CELL_NONE;
	total += comp.Put(&empty, sizeof(empty));
	total += comp.End();

	return(total);
}

// FUNCTION: 0x0056B3F0
int MapClass::Write_Binary_5(Pipe & pipe)
{
	LZOPipe comp(LZOPipe::COMPRESS);
	comp.Put_To(&pipe);
	int total = 0;

	for (int i = 0; i < Array.Length(); i++) {
		CellClass * cellptr = Array[i];
		if (i != 0 && cellptr) {
			if (In_Radar(cellptr->Fetch_CellID())) {
				if (cellptr->Fetch_CellID() != CELL_NONE && cellptr != &InvalidCell && (cellptr->ITType != ISOTILE_NONE || cellptr->Height > 0)) {
					Cell cell = cellptr->Fetch_CellID();
					total += comp.Put(&cell, sizeof(cell));
					total += comp.Put(&cellptr->ITType, sizeof(cellptr->ITType));
					total += comp.Put(&cellptr->SubTile, sizeof(cellptr->SubTile));
					total += comp.Put(&cellptr->Height, sizeof(cellptr->Height));
					total += comp.Put(&cellptr->IsIceGrowthAllowed, sizeof(cellptr->IsIceGrowthAllowed));
				}
			}
		}
	}

	Cell empty = CELL_NONE;
	total += comp.Put(&empty, sizeof(empty));
	total += comp.End();

	return(total);
}

// FUNCTION: 0x0056B5A0
bool MapClass::Read_Binary_1(Straw & straw)
{
	LCWStraw decomp(LCWStraw::DECOMPRESS);
	decomp.Get_From(&straw);

	int i;

	for (i = 0; i < MAP_CELL_TOTAL/16; i++) {
		CellClass * cellptr = &(*this)[Cell(i % 128, i / 128)];
		if (cellptr != NULL) {
			cellptr->ITType = ISOTILE_NONE;
			decomp.Get(&cellptr->ITType, sizeof(cellptr->ITType)/2);
			cellptr->ITType = IsometricTileTypeClass::Fixup_Tile_Type(cellptr->ITType);
		} else {
			decomp.Get(&InvalidCell.ITType, sizeof(InvalidCell.ITType)/2);
		}
	}

	for (i = 0; i < MAP_CELL_TOTAL/16; i++) {
		CellClass * cellptr = &(*this)[Cell(i % 128, i / 128)];
		if (cellptr != NULL) {
			decomp.Get(&cellptr->SubTile, sizeof(cellptr->SubTile));
		} else {
			decomp.Get(&InvalidCell.SubTile, sizeof(InvalidCell.SubTile));
		}
	}

	for (i = 0; i < MAP_CELL_TOTAL/16; i++) {
		CellClass * cellptr = &(*this)[Cell(i % 128, i / 128)];
		if (cellptr != NULL) {
			decomp.Get(&cellptr->Height, sizeof(cellptr->Height));
		} else {
			decomp.Get(&InvalidCell.Height, sizeof(InvalidCell.Height));
		}
	}

	new (&InvalidCell) CellClass;
	return(true);
}

// FUNCTION: 0x0056B780
bool MapClass::Read_Binary_2(Straw & straw)
{
	LCWStraw decomp(LCWStraw::DECOMPRESS);
	decomp.Get_From(&straw);

	Cell cell;
	decomp.Get(&cell, sizeof(cell));

	while (cell != CELL_NONE) {
		CellClass * cellptr = &(*this)[cell];
		if (cellptr != NULL && cellptr != &InvalidCell) {
			cellptr->ITType = ISOTILE_NONE;
			decomp.Get(&cellptr->ITType, sizeof(cellptr->ITType));
			cellptr->ITType = IsometricTileTypeClass::Fixup_Tile_Type(cellptr->ITType);
			decomp.Get(&cellptr->SubTile, sizeof(cellptr->SubTile));
			decomp.Get(&cellptr->Height, sizeof(cellptr->Height));
		} else {
			char tmp[sizeof(cellptr->ITType) + sizeof(cellptr->SubTile) + sizeof(cellptr->Height)];
			decomp.Get(&tmp, sizeof(tmp));
		}
		cell = CELL_NONE;
		decomp.Get(&cell, sizeof(cell));
	}
	new (&InvalidCell) CellClass;
	return(true);
}

// FUNCTION: 0x0056B8A0
bool MapClass::Read_Binary_3(Straw & straw)
{
	Cell cell;
	straw.Get(&cell, sizeof(cell));

	while (cell != CELL_NONE) {
		CellClass * cellptr = &(*this)[cell];
		if (cellptr != NULL && cellptr != &InvalidCell) {
			cellptr->ITType = ISOTILE_NONE;
			straw.Get(&cellptr->ITType, sizeof(cellptr->ITType));
			cellptr->ITType = IsometricTileTypeClass::Fixup_Tile_Type(cellptr->ITType);
			straw.Get(&cellptr->SubTile, sizeof(cellptr->SubTile));
			straw.Get(&cellptr->Height, sizeof(cellptr->Height));
		} else {
			char tmp[sizeof(cellptr->ITType) + sizeof(cellptr->SubTile) + sizeof(cellptr->Height)];
			straw.Get(&tmp, sizeof(tmp));
		}
		cell = CELL_NONE;
		straw.Get(&cell, sizeof(cell));
	}
	new (&InvalidCell) CellClass;
	return(true);
}

// FUNCTION: 0x0056B9A0
bool MapClass::Read_Binary_4(Straw & straw)
{
	LZOStraw decomp(LZOStraw::DECOMPRESS);
	decomp.Get_From(&straw);

	Cell cell;
	decomp.Get(&cell, sizeof(cell));

	while (cell != CELL_NONE) {
		CellClass * cellptr = &(*this)[cell];
		if (cellptr != NULL && cellptr != &InvalidCell) {
			cellptr->ITType = ISOTILE_NONE;
			decomp.Get(&cellptr->ITType, sizeof(cellptr->ITType));
			cellptr->ITType = IsometricTileTypeClass::Fixup_Tile_Type(cellptr->ITType);
			decomp.Get(&cellptr->SubTile, sizeof(cellptr->SubTile));
			decomp.Get(&cellptr->Height, sizeof(cellptr->Height));
		} else {
			char tmp[sizeof(cellptr->ITType) + sizeof(cellptr->SubTile) + sizeof(cellptr->Height)];
			decomp.Get(&tmp, sizeof(tmp));
		}
		cell = CELL_NONE;
		decomp.Get(&cell, sizeof(cell));
	}
	new (&InvalidCell) CellClass;
	return(true);
}

// FUNCTION: 0x0056BAC0
bool MapClass::Read_Binary_5(Straw & straw)
{
	LZOStraw decomp(LZOStraw::DECOMPRESS);
	decomp.Get_From(&straw);

	Cell cell;
	decomp.Get(&cell, sizeof(cell));

	while (cell != CELL_NONE) {
		CellClass * cellptr = &(*this)[cell];
		if (cellptr != NULL && cellptr != &InvalidCell) {
			cellptr->ITType = ISOTILE_NONE;
			decomp.Get(&cellptr->ITType, sizeof(cellptr->ITType));
			cellptr->ITType = IsometricTileTypeClass::Fixup_Tile_Type(cellptr->ITType);
			decomp.Get(&cellptr->SubTile, sizeof(cellptr->SubTile));
			decomp.Get(&cellptr->Height, sizeof(cellptr->Height));
			decomp.Get(&cellptr->IsIceGrowthAllowed, sizeof(cellptr->IsIceGrowthAllowed));
		} else {
			char tmp[sizeof(cellptr->ITType) + sizeof(cellptr->SubTile) + sizeof(cellptr->Height) + sizeof(cellptr->IsIceGrowthAllowed)];
			decomp.Get(&tmp, sizeof(tmp));
		}
		cell = CELL_NONE;
		decomp.Get(&cell, sizeof(cell));
	}
	new (&InvalidCell) CellClass;
	return(true);
}

// FUNCTION: 0x0056BBE0
void MapClass::Logic(void)
{
	if (Session.Type != GAME_NORMAL && Session.Options.Goodies) {
		for (int index = 0; index < ARRAY_SIZE(Crates); index++) {
			if (Crates[index].Is_Expired()) {
				Crates[index].Remove_It();
				Place_Random_Crate();
			}
		}
	}
}

// FUNCTION: 0x0056BC50
int MapClass::Cell_Region(Cell const & cell)
{
	return((cell.X / REGION_WIDTH) + 1) + (((cell.Y / REGION_HEIGHT) + 1) * MAP_REGION_WIDTH);
}

// FUNCTION: 0x0056BC80
Cell MapClass::Region_To_Cell(int region) const
{
	return(Cell((region % MAP_REGION_WIDTH) * REGION_WIDTH - 2, (region / MAP_REGION_WIDTH) * REGION_HEIGHT - 2));
}

// FUNCTION: 0x0056BCD0
int MapClass::GetThreatPosed(Cell const & cell, HouseClass * house) const
{
	Cell c = Map[cell].Fetch_CellID();
	return(house->Regions[Map.Cell_Region(c)].Threat_Value());
}

// FUNCTION: 0x0056BD40
bool MapClass::Place_Random_Crate(void)
{
	int crateindex = 0;
	for (crateindex = 0; crateindex < ARRAY_SIZE(Crates); crateindex++) {
		if (!Crates[crateindex].Is_Valid()) break;
	}
	if (crateindex == ARRAY_SIZE(Crates)) {
		return(false);
	}

	for (int index = 0; index < 1000; index++) {
		Cell cell = Map.Pick_Random_Location();
		if (Map[cell].Land_Type() == LAND_WATER) {
			cell = Map.Nearby_Location(cell, SPEED_FLOAT);
		} else {
			cell = Map.Nearby_Location(cell, SPEED_TRACK);
		}

		if (Crates[crateindex].Create_Crate(cell)) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x0056BEC0
bool MapClass::PlacePowerupCrate(Cell cell, int powerup)
{
	if (Map[cell].Land_Type() == LAND_WATER) {
		cell = Map.Nearby_Location(cell, SPEED_FLOAT);
	} else {
		cell = Map.Nearby_Location(cell, SPEED_TRACK);
	}

	int crateindex = 0;
	for (crateindex = 0; crateindex < ARRAY_SIZE(Crates); crateindex++) {
		if (!Crates[crateindex].Is_Valid()) break;
	}
	if (crateindex != ARRAY_SIZE(Crates) && Crates[crateindex].Create_Crate(cell)) {
		if (powerup != 20) {
			Map[cell].OverlayData = powerup;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0056C020
bool MapClass::Remove_Crate(Cell const & cell)
{
	if (Session.Type != GAME_NORMAL) {
		for (int index = 0; index < ARRAY_SIZE(Crates); index++) {
			if (Crates[index].Is_Here(cell)) {
				return(Crates[index].Remove_It());
			}
		}
	} else {
		CellClass * cellptr = &(*this)[cell];
		if (cellptr->Overlay != OVERLAY_NONE && OverlayTypes[cellptr->Overlay]->IsCrate) {
			Rect dirty = Union(cellptr->Overlay_Render_Rect(), cellptr->Overlay_Shadow_Render_Rect());
			dirty.Y -= TacticalRect.Y;
			TacticalMap->Register_Dirty_Area(dirty);
			cellptr->Overlay = OVERLAY_NONE;
			cellptr->OverlayData = 0;
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x0056C200
int FUN_0056C200(void)
{
	return(1);
}

// FUNCTION: 0x0056C210
ObjectClass * MapClass::Close_Object(Coord const & coord) const
{
	ObjectClass * object = 0;
	int distance = 0;
	Cell cell = coord.As_Cell();

	static Cell _offsets[] = {Cell(0,0), Cell(-1,0), Cell(1,0), Cell(0,-1), Cell(0,1), Cell(-1,1), Cell(1,1), Cell(1,-1), Cell(-1,-1)};
	for (int index = 0; index < ARRAY_SIZE(_offsets); index++) {
		Cell newcell = Cell(cell + _offsets[index]);
		if (In_Radar(newcell)) {
			for (ObjectClass * o = (*this)[newcell].Cell_Occupier(); o != NULL; o = o->Next) {
				if (!(o->AbstractFlags & 1) || ((TechnoClass *)o)->IsOwnedByPlayer || ((TechnoClass *)o)->Cloak != CLOAKED) {
					int d=-1;
					if (o->RTTI == RTTI_BUILDING) {
						d = Distance(coord, (Coord)newcell);
						if (d > 0x00B5) d = -1;
					} else {
						d = Distance(coord, o->Center_Coord());
					}
					if (d >= 0 && (!object || d < distance)) {
						distance = d;
						object = o;
					}
				}
			}
		}
	}

	if (object && distance > 0xB5) {
		object = 0;
	}
	Cell unused = cell + _offsets[0];
	return(object);
}

// FUNCTION: 0x0056C500 $E227

// FUNCTION: 0x0056CB80
int SubzoneHash(unsigned int const & key)
{
	return(key & 0xF | ((key >> 12) & 0xF));
}

// FUNCTION: 0x0056D3A0
int MapClass::Get_Cell_Zone_ID(Cell const & cell)
{
	return(CellZones[Get_Cell_Zone_Index(cell)].ZoneID);
}

// FUNCTION: 0x0056D3F0
int MapClass::Get_Cell_Zone_Index(Cell const & cell)
{
	int index = cell.X + cell.Y * (PlayRect.Height + PlayRect.Width + 1);
	if (index < 0) {
		return(0);
	}
	if (index >= CellZoneCount) {
		index = CellZoneCount - 1;
	}
	return(index);
}

// FUNCTION: 0x0056D430
int MapClass::Get_Cell_Subzone_Index(Cell const & cell)
{
	return(cell.X + cell.Y * (PlayRect.Height + PlayRect.Width + 1));
}

// FUNCTION: 0x0056D460
void MapClass::ResetZones(Cell const & cell)
{
	CellZoneStruct * cell_data = &CellZones[Get_Cell_Zone_Index(cell)];

	int stride = Map.PlayRect.Width + Map.PlayRect.Height + 1;
	int offsets[FACING_COUNT] = {
		-(stride + 0),
		-(stride - 1),
		1,
		stride + 1,
		stride + 0,
		stride - 1,
		-1,
		-(stride + 1),
	};

	if (cell_data->Passability != PASSABLE_OUTSIDE) {
		CellZoneStruct * land_neighbor = NULL;
		bool has_land_neighbor = false;
		for (FacingType i = FACING_FIRST; i < FACING_COUNT; i++) {
			land_neighbor = &cell_data[offsets[i]];
			if (land_neighbor->Passability == PASSABLE_LAND) {
				has_land_neighbor = true;
				break;
			}
		}

		if (has_land_neighbor && land_neighbor != NULL) {
			int transitions = 0;
			int prev_zone = 0;

			for (FacingType j = FACING_FIRST; j < FACING_COUNT; j++) {
				CellZoneStruct * neighbor = &cell_data[offsets[j]];
				if (Zones[MZONE_NORMAL][neighbor->ZoneID] != Zones[MZONE_NORMAL][prev_zone] && neighbor->Passability != PASSABLE_OUTSIDE) {
					prev_zone = neighbor->ZoneID;
					transitions++;
				}
			}

			if (transitions <= 3 && cell_data->Passability == PASSABLE_LAND) {
				cell_data->ZoneID = land_neighbor->ZoneID;
				return;
			}
		}

		Map.Zone_Reset();
	}
}

// FUNCTION: 0x0056D5A0
void MapClass::Update_Cell_Zone_Constructively(Cell const & cell)
{
	CellZoneStruct * cell_data = &CellZones[Get_Cell_Zone_Index(cell)];
	int passability = cell_data->Passability;

	int stride = Map.PlayRect.Width + Map.PlayRect.Height + 1;
	int offsets[FACING_COUNT] = {
		-(stride + 0),
		-(stride - 1),
		1,
		stride + 1,
		stride + 0,
		stride - 1,
		-1,
		-(stride + 1),
	};

	if (passability != PASSABLE_OUTSIDE) {
		CellZoneStruct * land_neighbor = NULL;
		bool has_land_neighbor = false;
		for (FacingType i = FACING_FIRST; i < FACING_COUNT; i++) {
			land_neighbor = &cell_data[offsets[i]];
			if (land_neighbor->Passability == passability) {
				has_land_neighbor = true;
				break;
			}
		}

		if (has_land_neighbor && land_neighbor != NULL) {
			int transitions = 0;
			int prev_zone = 0;

			for (FacingType j = FACING_FIRST; j < FACING_COUNT; j++) {
				if (Zones[MZONE_NORMAL][cell_data[offsets[j]].ZoneID] != Zones[MZONE_NORMAL][prev_zone] && cell_data[offsets[j]].Passability != PASSABLE_OUTSIDE) {
					prev_zone = cell_data[offsets[j]].ZoneID;
					transitions++;
				}
			}

			if (transitions <= 3) {
				cell_data->ZoneID = land_neighbor->ZoneID;
				return;
			}
		}

		Map.Zone_Reset();
	}
}

// FUNCTION: 0x0056DA10
int MapClass::Zone_Connection_Index(Cell const & cell, int maxdist, int index)
{
	for (int i = index; i < ZoneConnections.Count(); i++) {
		ZoneConnectionClass * con = &ZoneConnections[i];
		if (con->Type == 0) {
			Cell f = con->From;
			Cell t = con->To;
			if ((f.X - t.X) == 0) {
				if (cell.Y >= f.Y && cell.Y <= t.Y && abs(cell.X - f.X) <= maxdist) {
					return(i);
				}
			} else {
				if (cell.X <= t.X && cell.X >= f.X && abs(cell.Y - f.Y) <= maxdist) {
					return(i);
				}
			}
		}
	}
	return(-1);
}

// FUNCTION: 0x00577A50
Cell MapClass::Pick_Random_Location(void) const
{
	int x = Map.MapRect.X + Random_Pick(0, Map.MapRect.Width-1);
	int y = Map.MapRect.Y + Random_Pick(0, Map.MapRect.Height-1);
	return(Cell(x, y));
}

// FUNCTION: 0x00578350
void MapClass::Reset_Iterator(void)
{
	IterX = 1;
	IterY = PlayRect.Width;
	IterColumn = PlayRect.Width - 1;
	IterCell = &Array[IterX + (IterY * MAP_CELL_H)];
}

// FUNCTION: 0x00578390
bool MapClass::In_Local_Radar(Rect const & rect, bool useheight) const
{
	return(In_Local_Radar(Cell(rect.X, rect.Y), useheight) &&
		In_Local_Radar(Cell(rect.X + rect.Width - 1, rect.Y), useheight) &&
		In_Local_Radar(Cell(rect.X, rect.Y + rect.Height - 1), useheight) &&
		In_Local_Radar(Cell(rect.X + rect.Width - 1, rect.Y + rect.Height - 1), useheight) ? 1 : 0);
}

// FUNCTION: 0x005785F0
bool MapClass::In_Local_Radar(Coord const & coord) const
{
	return(In_Local_Radar(coord.As_Cell()));
}

// FUNCTION: 0x00578D60
bool MapClass::Is_Tile_Shore(IsometricTileType tile)
{
	if (IS_TILE_IN_SET(ShorePieces, SHORE_PIECES_COUNT)) return(true);

	return(false);
}

// FUNCTION: 0x00578D80
bool MapClass::Is_Tile_Cliff(IsometricTileType tile, int subtile)
{
	if (IS_TILE_IN_SET(CliffSet, CLIFF_COUNT)) return(true);

	if (IS_TILE_IN_SET(WaterfallEast, WATERFALL_EAST_COUNT)) {
		if (tile == IsometricTileTypeClass::WaterfallEast || tile == IsometricTileTypeClass::WaterfallEast + WATERFALL_EAST_COUNT-1) {
			if (subtile == 0 || subtile == 4) {
				return(false);
			}
		}
		return(true);
	}

	if (IS_TILE_IN_SET(WaterfallWest, WATERFALL_WEST_COUNT)) {
		if (tile == IsometricTileTypeClass::WaterfallWest || tile == IsometricTileTypeClass::WaterfallWest + WATERFALL_WEST_COUNT-1) {
			if (subtile == 1 || subtile == 3) {
				return(false);
			}
		}
		return(true);
	}

	if (IS_TILE_IN_SET(WaterfallSouth, WATERFALL_SOUTH_COUNT)) {
		if (tile == IsometricTileTypeClass::WaterfallSouth || tile == IsometricTileTypeClass::WaterfallSouth + WATERFALL_SOUTH_COUNT-1) {
			if (subtile == 0 || subtile == 1) {
				return(false);
			}
		}
		return(true);
	}

	if (IS_TILE_IN_SET(WaterfallNorth, WATERFALL_NORTH_COUNT)) {
		if (tile == IsometricTileTypeClass::WaterfallNorth || tile == IsometricTileTypeClass::WaterfallNorth + WATERFALL_NORTH_COUNT-1) {
			if (subtile == 2 || subtile == 3) {
				return(false);
			}
		}
		return(true);
	}

	if (IS_TILE_IN_SET(CliffRamps, CLIFF_RAMPS_COUNT)) return(true);

	return(false);
}

// FUNCTION: 0x0057A900
int MapClass::Straight_Shore_Length(CellClass * cellptr, FacingType direction)
{
	FacingType snap[4] = {
		FACING_E,
		FACING_S,
		FACING_E,
		FACING_S,
	};

	int count = 1;
	FacingType s_direction = snap[direction / 2];
	CellClass * adj1 = &cellptr->Adjacent_Cell(s_direction);
	CellClass * adj2 = &adj1->Adjacent_Cell(direction);
	bool side_1_is_land = adj1->Is_Tile_Water() == false;
	bool side_2_is_water = adj2->Is_Tile_Water();

	while (side_1_is_land && side_2_is_water) {
		count++;
		adj1 = &adj1->Adjacent_Cell(s_direction);
		adj2 = &adj2->Adjacent_Cell(s_direction);
		side_1_is_land = adj1->Is_Tile_Water() == false;
		side_2_is_water = adj2->Is_Tile_Water();
	}

	return(count);
}

// FUNCTION: 0x005868A0
void MapClass::Recalc_Cells_In_Rect(Rect const & rect)
{
	DynamicVectorClass<Cell> cells;
	for (int x = rect.X; x < rect.X + rect.Width; x++) {
		for (int y = rect.Y; y < rect.Y + rect.Height; y++) {
			cells.Add(Cell(x,y));
		}
	}
	Recalc_Cells_In_List(cells);
}
