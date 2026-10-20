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

void __cdecl FUN_004068E0(char const * format, ...);
extern void FUN_00561910(void);
extern void FUN_005638D0(void);

unsigned int Pick_Random_UInt(unsigned int minval, unsigned int maxval);

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
	return((*this)[cell].IsMapped);
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
			if (cellptr->IsFogVisible && cellptr->IsFogMapped) continue;
			if (!cellptr->IsMapped) continue;
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
					Map[cell].IsMapped = false;
					Map[cell].IsVisible = false;
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
				Map[cell].IsVisible = true;
				Map[cell].IsMapped = true;
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

// FUNCTION: 0x0056DAE0
bool MapClass::Unregister_Subzone_Connections(Cell const & cell)
{
	int index = Zone_Connection_Index(cell, 3);
	if (index == -1) {
		Compute_Zone_Connections();
		index = Zone_Connection_Index(cell, 3);
		if (index == -1) {
			return(false);
		}
	}

	bool result = false;

	while (index != -1) {
		if (ZoneConnections[index].IsPassable) {
			Unregister_Subzone_Connection(&ZoneConnections[index]);
			ZoneConnections[index].IsPassable = false;
			result = true;
		}
		index = Zone_Connection_Index(cell, 3, index + 1);
	};

	return(result);
}

// FUNCTION: 0x0056DB70
bool MapClass::Register_Subzone_Connections(Cell const & cell)
{
	int index = Zone_Connection_Index(cell, 3);
	if (index == -1) {
		Compute_Zone_Connections();
		index = Zone_Connection_Index(cell, 3);
		if (index == -1) {
			return(false);
		}
	}

	bool result = false;

	while (index != -1) {
		if (!ZoneConnections[index].IsPassable) {
			ZoneConnections[index].IsPassable = true;
			Register_Subzone_Connection(&ZoneConnections[index]);
			if (!Is_Same_Cell_Zone(ZoneConnections[index].From, ZoneConnections[index].To)) {
				result = true;
			}
		}
		index = Zone_Connection_Index(cell, 3, index + 1);
	};

	return(result);
}

// FUNCTION: 0x0056E7C0
bool MapClass::Is_Clear_To_Move(Cell const & cell, int width, int height, SpeedType speed, int zone, MZoneType check, int cell_height, bool checkbridge, bool block_overlays)
{
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			Cell newcell = Cell(x,y) + cell;
			CellClass * cellptr = &Map[newcell];
			if ((block_overlays && cellptr->Overlay != OVERLAY_NONE) || !cellptr->Is_Clear_To_Move(speed, false, false, zone, check, cell_height, checkbridge)) {
				return(false);
			}
		}
	}
	return(true);
}

// FUNCTION: 0x0056E890
bool MapClass::Base_Region(Cell const & cell, HousesType & house, ZoneType & zone) const
{
	if (cell != CELL_NONE && In_Radar(cell)) {
		for (int index = HOUSE_FIRST; index < Houses.Count(); index++) {
			HouseClass * h = Houses[index];

			if (h && !h->IsDefeated && h->Get_Base_Coord() != COORD_NONE) {
				zone = h->Which_Zone(cell);
				if (zone != ZONE_NONE) {
					house = h->Class->House;
					return(true);
				}
			}
		}
	}
	return(false);
}

enum {
	BRIDGE_MIDDLE_OK,
	BRIDGE_MIDDLE_DAMAGED_1,
	BRIDGE_MIDDLE_DAMAGED_2,
	BRIDGE_MIDDLE_DAMAGED_3,
	BRIDGE_MIDDLE_DESTROYED
};

// FUNCTION: 0x0056E990
void MapClass::Set_Bridge_End_State(Cell const & cell, bool damaged, bool recursive)
{
	CellClass * cellptr = &Map[cell];
	if (!recursive) {
		if (cellptr->ITType == ISOTILE_NONE || cellptr->ITType == ISOTILE_NONE_LEGACY) {
			return;
		}
		IsometricTileTypeClass * isotype = IsometricTileTypes[cellptr->ITType];
		if (!isotype->Is_Randomized(cellptr->SubTile)) {
			return;
		}
		Point2D point;
		TacticalMap->Coord_To_Pixel((Coord)cell, point);
		point.Y -= 15 * cellptr->Height;
		TacticalMap->Register_Dirty_Area(Rect(point - Point2D(128, 128), 256, 256));
	}

	if (damaged != (bool)cellptr->IsBridgeDamaged) {
		cellptr->IsBridgeDamaged = damaged;
		IsometricTileType ittype = cellptr->ITType;
		Map.Radar_Background(cellptr->CellID);
		FacingType facing = FACING_FIRST;
		for (int i = 0; i < FACING_COUNT; i++) {
			Cell adjacent = AdjacentCell[(int)facing & 0x07] + cell;
			CellClass * c = &Map[adjacent];
			if (c->ITType == ittype) {
				Set_Bridge_End_State(adjacent, damaged, true);
			}
			facing = FacingType(unsigned(facing + FACING_45) % FACING_COUNT);
		}
	}
}

// FUNCTION: 0x0056EB80
void MapClass::Set_Bridge_Middle_State(Cell const & cell, IsometricTileType new_tile, IsometricTileType match_tile, int cell_height, bool recursive)
{
	CellClass * cellptr = &Map[cell];
	IsometricTileType old_tile;
	if (!recursive) {
		Point2D point;
		TacticalMap->Coord_To_Pixel((Coord)cell, point);
		point.Y -= 15 * cellptr->Height;
		TacticalMap->Register_Dirty_Area(Rect(point - Point2D(128, 128), 256, 256));
		old_tile = cellptr->ITType;
		if (new_tile == old_tile) {
			return;
		}
	} else {
		old_tile = match_tile;
	}

	bool changed = cellptr->ITType != new_tile;
	if (changed) {
		cellptr->ITType = new_tile;
		cellptr->Recalc_Attributes(cell_height);
		Map.Radar_Background(cellptr->CellID);
		FacingType facing = FACING_FIRST;
		for (int i = 0; i < FACING_COUNT; i++) {
			Cell adjacent = Adjacent_Cell(cell, facing);
			CellClass * c = &Map[adjacent];
			bool matches = c->ITType == old_tile;
			if (matches) {
				Set_Bridge_Middle_State(adjacent, new_tile, old_tile, cell_height, true);
			}
			facing = FacingType(unsigned(facing + FACING_45) % FACING_COUNT);
		}
	}
}

// FUNCTION: 0x0056ED40
void MapClass::Internal_Damage_Train_Bridge_EW_BottomRight(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_EW_FULL4) {
			if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_EW_TRANSITION2) {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_DAMAGED;
			}
		} else {
			adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_TRANSITION1;
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1);
	if (ittype == IsometricTileTypeClass::BridgeBottomRight1 || ittype == IsometricTileTypeClass::BridgeBottomRight2) {
		Set_Bridge_End_State(adjacent, true, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_OK) {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_OK), ISOTILE_INVALID, -1, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2)  {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
	}
}


// FUNCTION: 0x0056EE40
void MapClass::Internal_Damage_Train_Bridge_EW_TopLeft(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_EW_FULL4) {
			if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_EW_TRANSITION1) {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_DAMAGED;
			}
		} else {
			adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_TRANSITION2;
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1);
	if (ittype == IsometricTileTypeClass::BridgeTopLeft1 || ittype == IsometricTileTypeClass::BridgeTopLeft2) {
		Set_Bridge_End_State(adjacent, true, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_OK) {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_1), ISOTILE_INVALID, -1, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_1)  {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
	}
}


// FUNCTION: 0x0056EF50
void MapClass::Internal_Destroy_Train_Bridge_EW_BottomRight(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_EW_DAMAGED) {
			if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_EW_END2) {
				Internal_Destroy_Train_Bridge_EW_BottomRight(adjacent, dir);
				adjacentptr->Set_Under_Bridge(FACING_N, false);
				adjacentptr->OverlayData = 0;
				adjacentptr->Overlay = OVERLAY_NONE;
				Map.Radar_Background(adjacentptr->CellID);
			}
		} else {
			adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_END1;
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1);
	if (ittype != IsometricTileTypeClass::BridgeBottomRight1 && ittype != IsometricTileTypeClass::BridgeBottomRight2) {
		if (ittype == IsometricTileTypeClass::BridgeMiddle1 || ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2) {
			Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
		} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3) {
			Internal_Destroy_Train_Bridge_EW_BottomRight(adjacent, dir);
			if ((adjacentptr->SubTile & 1) != 0) {
				Map[adjacent + Cell(-1, 0)].Destroy_Bridge();
				Map[adjacent + Cell(-1, -1)].Destroy_Bridge();
				Map[adjacent + Cell(-1, 1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent + Cell(-1, 0)].Height - BRIDGE_CELL_HEIGHT, 0);
			} else {
				Map[adjacent + Cell(0, 0)].Destroy_Bridge();
				Map[adjacent + Cell(0, -1)].Destroy_Bridge();
				Map[adjacent + Cell(0, 1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent].Height - BRIDGE_CELL_HEIGHT, 0);
			}
		}
	} else {
		Set_Bridge_End_State(adjacent, true, false);
	}
}


// FUNCTION: 0x0056F2F0
void MapClass::Internal_Destroy_Train_Bridge_EW_TopLeft(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_EW_DAMAGED) {
			if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_EW_END1) {
				Internal_Destroy_Train_Bridge_EW_TopLeft(adjacent, dir);
				adjacentptr->Set_Under_Bridge(FACING_N, false);
				adjacentptr->OverlayData = 0;
				adjacentptr->Overlay = OVERLAY_NONE;
				Map.Radar_Background(adjacentptr->CellID);
			}
		} else {
			adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_END2;
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1);
	if (ittype != IsometricTileTypeClass::BridgeTopLeft1 && ittype != IsometricTileTypeClass::BridgeTopLeft2) {
		if (ittype == IsometricTileTypeClass::BridgeMiddle1 || ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_1) {
			Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
		} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3) {
			Internal_Destroy_Train_Bridge_EW_TopLeft(adjacent, dir);
			if ((adjacentptr->SubTile & 1) != 0) {
				Map[adjacent + Cell(-1, 0)].Destroy_Bridge();
				Map[adjacent + Cell(-1, -1)].Destroy_Bridge();
				Map[adjacent + Cell(-1, 1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent + Cell(-1, 0)].Height - BRIDGE_CELL_HEIGHT, 0);
			} else {
				Map[adjacent + Cell(0, 0)].Destroy_Bridge();
				Map[adjacent + Cell(0, -1)].Destroy_Bridge();
				Map[adjacent + Cell(0, 1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent].Height - BRIDGE_CELL_HEIGHT, 0);
			}
		}
	} else {
		Set_Bridge_End_State(adjacent, true, false);
	}
}


// FUNCTION: 0x0056F690
void MapClass::Internal_Damage_Train_Bridge_NS_BottomLeft(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1) {
			if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_NS_FULL4) {
				if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_NS_TRANSITION1) {
					adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_DAMAGED;
				}
			} else {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_TRANSITION2;
			}
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1);
	if (ittype == IsometricTileTypeClass::BridgeBottomLeft1 || ittype == IsometricTileTypeClass::BridgeBottomLeft2) {
		Set_Bridge_End_State(adjacent, true, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_OK) {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_OK), ISOTILE_INVALID, -1, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2)  {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
	}
}


// FUNCTION: 0x0056F7A0
void MapClass::Internal_Damage_Train_Bridge_NS_TopRight(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1) {
			if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_NS_FULL4) {
				if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_NS_TRANSITION2) {
					adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_DAMAGED;
				}
			} else {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_TRANSITION1;
			}
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1);
	if (ittype == IsometricTileTypeClass::BridgeTopRight1 || ittype == IsometricTileTypeClass::BridgeTopRight2) {
		Set_Bridge_End_State(adjacent, true, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_OK) {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_1), ISOTILE_INVALID, -1, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_1)  {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
	}
}


static inline void Destroy_Bridge_At(Cell const & cell)
{
	Map[cell].Destroy_Bridge();
}

// FUNCTION: 0x0056F8B0
void MapClass::Internal_Destroy_Train_Bridge_NS_BottomLeft(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1) {
			if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_NS_DAMAGED) {
				if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_NS_END1) {
					Internal_Destroy_Train_Bridge_NS_BottomLeft(adjacent, dir);
					adjacentptr->Set_Under_Rail_Bridge(FACING_W, false);
					adjacentptr->OverlayData = 0;
					adjacentptr->Overlay = OVERLAY_NONE;
					Map.Radar_Background(adjacentptr->CellID);
				}
			} else {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_END2;
			}
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1);
	if (ittype != IsometricTileTypeClass::BridgeBottomLeft1 && ittype != IsometricTileTypeClass::BridgeBottomLeft2) {
		if (ittype == IsometricTileTypeClass::BridgeMiddle2 || ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2) {
			Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
		} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3) {
			Internal_Destroy_Train_Bridge_NS_BottomLeft(adjacent, dir);
			if (adjacentptr->SubTile > 4) {
				Map[adjacent + Cell(-1, -1)].Destroy_Bridge();
				Map[adjacent + Cell(0, -1)].Destroy_Bridge();
				Map[adjacent + Cell(1, -1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent + Cell(0, -1)].Height - BRIDGE_CELL_HEIGHT, 0);
			} else {
				Map[adjacent + Cell(-1, 0)].Destroy_Bridge();
				Map[adjacent + Cell(0, 0)].Destroy_Bridge();
				Destroy_Bridge_At(adjacent + Cell(1, 0));
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent].Height - BRIDGE_CELL_HEIGHT, 0);
			}
		}
	} else {
		Set_Bridge_End_State(adjacent, true, false);
	}
}


// FUNCTION: 0x0056FC80
void MapClass::Internal_Destroy_Train_Bridge_NS_TopRight(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1) {
			if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_NS_DAMAGED) {
				if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_NS_END2) {
					Internal_Destroy_Train_Bridge_NS_TopRight(adjacent, dir);
					adjacentptr->Set_Under_Rail_Bridge(FACING_W, false);
					adjacentptr->OverlayData = 0;
					adjacentptr->Overlay = OVERLAY_NONE;
					Map.Radar_Background(adjacentptr->CellID);
				}
			} else {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_END1;
			}
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::TrainBridgeSet + 1);
	if (ittype != IsometricTileTypeClass::BridgeTopRight1 && ittype != IsometricTileTypeClass::BridgeTopRight2) {
		if (ittype == IsometricTileTypeClass::BridgeMiddle2 || ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_1) {
			Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
		} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3) {
			Internal_Destroy_Train_Bridge_NS_TopRight(adjacent, dir);
			if (adjacentptr->SubTile > 4) {
				Map[adjacent + Cell(-1, -1)].Destroy_Bridge();
				Map[adjacent + Cell(0, -1)].Destroy_Bridge();
				Map[adjacent + Cell(1, -1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent + Cell(0, -1)].Height - BRIDGE_CELL_HEIGHT, 0);
			} else {
				Map[adjacent + Cell(-1, 0)].Destroy_Bridge();
				Map[adjacent + Cell(0, 0)].Destroy_Bridge();
				Destroy_Bridge_At(adjacent + Cell(1, 0));
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::TrainBridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent].Height - BRIDGE_CELL_HEIGHT, 0);
			}
		}
	} else {
		Set_Bridge_End_State(adjacent, true, false);
	}
}


// FUNCTION: 0x00572230
void MapClass::Internal_Damage_High_Bridge_EW_BottomRight(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_EW_FULL4) {
			if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_EW_TRANSITION2) {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_DAMAGED;
			}
		} else {
			adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_TRANSITION1;
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::BridgeSet + 1);
	if (ittype == IsometricTileTypeClass::BridgeBottomRight1 || ittype == IsometricTileTypeClass::BridgeBottomRight2) {
		Set_Bridge_End_State(adjacent, true, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_OK) {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_OK), ISOTILE_INVALID, -1, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2)  {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
	}
}


// FUNCTION: 0x00572330
void MapClass::Internal_Damage_High_Bridge_EW_TopLeft(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_EW_FULL4) {
			if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_EW_TRANSITION1) {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_DAMAGED;
			}
		} else {
			adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_TRANSITION2;
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::BridgeSet + 1);
	if (ittype == IsometricTileTypeClass::BridgeTopLeft1 || ittype == IsometricTileTypeClass::BridgeTopLeft2) {
		Set_Bridge_End_State(adjacent, true, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_OK) {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_1), ISOTILE_INVALID, -1, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_1)  {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
	}
}


// FUNCTION: 0x00572440
void MapClass::Internal_Destroy_High_Bridge_EW_BottomRight(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_EW_DAMAGED) {
			if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_EW_END2) {
				Internal_Destroy_High_Bridge_EW_BottomRight(adjacent, dir);
				adjacentptr->Set_Under_Bridge(FACING_N, false);
				adjacentptr->OverlayData = 0;
				adjacentptr->Overlay = OVERLAY_NONE;
				Map.Radar_Background(adjacentptr->CellID);
			}
		} else {
			adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_END1;
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::BridgeSet + 1);
	if (ittype != IsometricTileTypeClass::BridgeBottomRight1 && ittype != IsometricTileTypeClass::BridgeBottomRight2) {
		if (ittype == IsometricTileTypeClass::BridgeMiddle1 || ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2) {
			Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
		} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3) {
			Internal_Destroy_High_Bridge_EW_BottomRight(adjacent, dir);
			if ((adjacentptr->SubTile & 1) != 0) {
				Map[adjacent + Cell(-1, 0)].Destroy_Bridge();
				Map[adjacent + Cell(-1, -1)].Destroy_Bridge();
				Map[adjacent + Cell(-1, 1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent + Cell(-1, 0)].Height - BRIDGE_CELL_HEIGHT, 0);
			} else {
				Map[adjacent + Cell(0, 0)].Destroy_Bridge();
				Map[adjacent + Cell(0, -1)].Destroy_Bridge();
				Map[adjacent + Cell(0, 1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent].Height - BRIDGE_CELL_HEIGHT, 0);
			}
		}
	} else {
		Set_Bridge_End_State(adjacent, true, false);
	}
}


// FUNCTION: 0x005727E0
void MapClass::Internal_Destroy_High_Bridge_EW_TopLeft(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_EW_DAMAGED) {
			if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_EW_END1) {
				Internal_Destroy_High_Bridge_EW_TopLeft(adjacent, dir);
				adjacentptr->Set_Under_Bridge(FACING_N, false);
				adjacentptr->OverlayData = 0;
				adjacentptr->Overlay = OVERLAY_NONE;
				Map.Radar_Background(adjacentptr->CellID);
			}
		} else {
			adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_EW_END2;
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::BridgeSet + 1);
	if (ittype != IsometricTileTypeClass::BridgeTopLeft1 && ittype != IsometricTileTypeClass::BridgeTopLeft2) {
		if (ittype == IsometricTileTypeClass::BridgeMiddle1 || ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_1) {
			Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
		} else if (ittype == IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3) {
			Internal_Destroy_High_Bridge_EW_TopLeft(adjacent, dir);
			if ((adjacentptr->SubTile & 1) != 0) {
				Map[adjacent + Cell(-1, 0)].Destroy_Bridge();
				Map[adjacent + Cell(-1, -1)].Destroy_Bridge();
				Map[adjacent + Cell(-1, 1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent + Cell(-1, 0)].Height - BRIDGE_CELL_HEIGHT, 0);
			} else {
				Map[adjacent + Cell(0, 0)].Destroy_Bridge();
				Map[adjacent + Cell(0, -1)].Destroy_Bridge();
				Map[adjacent + Cell(0, 1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle1 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent].Height - BRIDGE_CELL_HEIGHT, 0);
			}
		}
	} else {
		Set_Bridge_End_State(adjacent, true, false);
	}
}


// FUNCTION: 0x00572B80
void MapClass::Internal_Damage_High_Bridge_NS_BottomLeft(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1) {
			if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_NS_FULL4) {
				if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_NS_TRANSITION1) {
					adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_DAMAGED;
				}
			} else {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_TRANSITION2;
			}
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::BridgeSet + 1);
	if (ittype == IsometricTileTypeClass::BridgeBottomLeft1 || ittype == IsometricTileTypeClass::BridgeBottomLeft2) {
		Set_Bridge_End_State(adjacent, true, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_OK) {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_OK), ISOTILE_INVALID, -1, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2)  {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
	}
}


// FUNCTION: 0x00572C90
void MapClass::Internal_Damage_High_Bridge_NS_TopRight(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1) {
			if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_NS_FULL4) {
				if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_NS_TRANSITION2) {
					adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_DAMAGED;
				}
			} else {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_TRANSITION1;
			}
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::BridgeSet + 1);
	if (ittype == IsometricTileTypeClass::BridgeTopRight1 || ittype == IsometricTileTypeClass::BridgeTopRight2) {
		Set_Bridge_End_State(adjacent, true, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_OK) {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_1), ISOTILE_INVALID, -1, false);
	} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_1)  {
		Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
	}
}


// FUNCTION: 0x00572DA0
void MapClass::Internal_Destroy_High_Bridge_NS_BottomLeft(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1) {
			if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_NS_DAMAGED) {
				if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_NS_END1) {
					Internal_Destroy_High_Bridge_NS_BottomLeft(adjacent, dir);
					adjacentptr->Set_Under_Bridge(FACING_W, false);
					adjacentptr->OverlayData = 0;
					adjacentptr->Overlay = OVERLAY_NONE;
					Map.Radar_Background(adjacentptr->CellID);
				}
			} else {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_END2;
			}
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::BridgeSet + 1);
	if (ittype != IsometricTileTypeClass::BridgeBottomLeft1 && ittype != IsometricTileTypeClass::BridgeBottomLeft2) {
		if (ittype == IsometricTileTypeClass::BridgeMiddle2 || ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2) {
			Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
		} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3) {
			Internal_Destroy_High_Bridge_NS_BottomLeft(adjacent, dir);
			if (adjacentptr->SubTile > 4) {
				Map[adjacent + Cell(-1, -1)].Destroy_Bridge();
				Map[adjacent + Cell(0, -1)].Destroy_Bridge();
				Map[adjacent + Cell(1, -1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent + Cell(0, -1)].Height - BRIDGE_CELL_HEIGHT, 0);
			} else {
				Map[adjacent + Cell(-1, 0)].Destroy_Bridge();
				Map[adjacent + Cell(0, 0)].Destroy_Bridge();
				Destroy_Bridge_At(adjacent + Cell(1, 0));
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent].Height - BRIDGE_CELL_HEIGHT, 0);
			}
		}
	} else {
		Set_Bridge_End_State(adjacent, true, false);
	}
}


// FUNCTION: 0x00573170
void MapClass::Internal_Destroy_High_Bridge_NS_TopRight(Cell const & cell, FacingType dir)
{
	Cell adjacent = Adjacent_Cell(cell, dir);
	CellClass * adjacentptr = &Map[adjacent];
	if (adjacentptr->IsBridgeDeck) {
		if (adjacentptr->OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1) {
			if (adjacentptr->OverlayData > OVERLAYDATA_BRIDGE_NS_DAMAGED) {
				if (adjacentptr->OverlayData == OVERLAYDATA_BRIDGE_NS_END2) {
					Internal_Destroy_High_Bridge_NS_TopRight(adjacent, dir);
					adjacentptr->Set_Under_Bridge(FACING_W, false);
					adjacentptr->OverlayData = 0;
					adjacentptr->Overlay = OVERLAY_NONE;
					Map.Radar_Background(adjacentptr->CellID);
				}
			} else {
				adjacentptr->OverlayData = OVERLAYDATA_BRIDGE_NS_END1;
			}
		}
	}
	IsometricTileType ittype = IsometricTileType(adjacentptr->ITType - IsometricTileTypeClass::BridgeSet + 1);
	if (ittype != IsometricTileTypeClass::BridgeTopRight1 && ittype != IsometricTileTypeClass::BridgeTopRight2) {
		if (ittype == IsometricTileTypeClass::BridgeMiddle2 || ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_1) {
			Set_Bridge_Middle_State(adjacent, IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_2), ISOTILE_INVALID, -1, false);
		} else if (ittype == IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3) {
			Internal_Destroy_High_Bridge_NS_TopRight(adjacent, dir);
			if (adjacentptr->SubTile > 4) {
				Map[adjacent + Cell(-1, -1)].Destroy_Bridge();
				Map[adjacent + Cell(0, -1)].Destroy_Bridge();
				Map[adjacent + Cell(1, -1)].Destroy_Bridge();
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent + Cell(0, -1)].Height - BRIDGE_CELL_HEIGHT, 0);
			} else {
				Map[adjacent + Cell(-1, 0)].Destroy_Bridge();
				Map[adjacent + Cell(0, 0)].Destroy_Bridge();
				Destroy_Bridge_At(adjacent + Cell(1, 0));
				Set_Bridge_Middle_State(adjacent,  IsometricTileType(IsometricTileTypeClass::BridgeSet + IsometricTileTypeClass::BridgeMiddle2 + BRIDGE_MIDDLE_DAMAGED_3), ISOTILE_INVALID, Map[adjacent].Height - BRIDGE_CELL_HEIGHT, 0);
			}
		}
	} else {
		Set_Bridge_End_State(adjacent, true, false);
	}
}


bool FUN_00574600(int tile, int dir, CellClass * cell);
bool FUN_005746C0(int tile, CellClass * cell);

static inline bool Is_Concrete_Low_Bridge(Cell const & cell)
{
	OverlayType overlay = Map[cell].Overlay;
	return(overlay >= 0xCD && overlay <= 0xE8);
}

// FUNCTION: 0x00574600
bool FUN_00574600(int tile, int dir, CellClass * cell)
{
	if (dir == 2) {
		if (tile == IsometricTileTypeClass::BridgeBottomRight1 && cell->SubTile == 4) {
			return true;
		}
		if (tile == IsometricTileTypeClass::BridgeBottomRight2 && cell->SubTile == 4) {
			return true;
		}
		if ((tile == IsometricTileTypeClass::BridgeMiddle1 || tile == IsometricTileTypeClass::BridgeMiddle1 + 3 || tile == IsometricTileTypeClass::BridgeMiddle1 + 1 || tile == IsometricTileTypeClass::BridgeMiddle1 + 2) && cell->SubTile == 4) {
			return true;
		}
	} else if (dir == 4) {
		if (tile == IsometricTileTypeClass::BridgeBottomLeft1 && cell->SubTile == 2) {
			return true;
		}
		if (tile == IsometricTileTypeClass::BridgeBottomLeft2 && cell->SubTile == 2) {
			return true;
		}
		if ((tile == IsometricTileTypeClass::BridgeMiddle2 || tile == IsometricTileTypeClass::BridgeMiddle2 + 3 || tile == IsometricTileTypeClass::BridgeMiddle2 + 1 || tile == IsometricTileTypeClass::BridgeMiddle2 + 2) && cell->SubTile == 2) {
			return true;
		}
	}
	return false;
}

// FUNCTION: 0x005746C0
bool FUN_005746C0(int tile, CellClass * cell)
{
	if (tile == IsometricTileTypeClass::BridgeTopRight1 && cell->SubTile == 12) {
		return true;
	}
	if (tile == IsometricTileTypeClass::BridgeTopRight2 && cell->SubTile == 12) {
		return true;
	}
	if ((tile == IsometricTileTypeClass::BridgeMiddle1 || tile == IsometricTileTypeClass::BridgeMiddle1 + 3 || tile == IsometricTileTypeClass::BridgeMiddle1 + 1 || tile == IsometricTileTypeClass::BridgeMiddle1 + 2) && cell->SubTile == 4) {
		return true;
	}
	if (tile == IsometricTileTypeClass::BridgeTopLeft1 && cell->SubTile == 8) {
		return true;
	}
	if (tile == IsometricTileTypeClass::BridgeTopLeft2 && cell->SubTile == 8) {
		return true;
	}
	if ((tile == IsometricTileTypeClass::BridgeMiddle2 || tile == IsometricTileTypeClass::BridgeMiddle2 + 3 || tile == IsometricTileTypeClass::BridgeMiddle2 + 1 || tile == IsometricTileTypeClass::BridgeMiddle2 + 2) && cell->SubTile == 2) {
		return true;
	}
	return false;
}

// FUNCTION: 0x00574780
void MapClass::FUN_00574780(Cell const & cell)
{
	int overlay = Map[cell].Overlay;
	if (overlay >= 0x4A && overlay <= 0x52 || overlay >= 0x5C && overlay <= 0x5F || overlay == 0x64) {
		if (!Is_Low_Bridge(cell + Cell(0, -1))) {
			FUN_00575220(cell + Cell(0, 1));
		} else if (!Is_Low_Bridge(cell + Cell(0, -2))) {
			FUN_00575220(cell);
		} else {
			FUN_00575220(cell + Cell(0, -1));
		}
	} else if (overlay >= 0x53 && overlay <= 0x5B || overlay >= 0x60 && overlay <= 0x63 || overlay == 0x65) {
		if (!Is_Low_Bridge(cell + Cell(-1, 0))) {
			FUN_00575540(cell + Cell(1, 0));
		} else if (!Is_Low_Bridge(cell + Cell(-2, 0))) {
			FUN_00575540(cell);
		} else {
			FUN_00575540(cell - Cell(1, 0));
		}
	}
}

// FUNCTION: 0x005749C0
void MapClass::FUN_005749C0(Cell const & cell)
{
	int overlay = Map[cell].Overlay;
	if (overlay >= 0xCD && overlay <= 0xD5 || overlay >= 0xDF && overlay <= 0xE2 || overlay == 0xE7) {
		if (!Is_Concrete_Low_Bridge(cell + Cell(0, -1))) {
			FUN_00575870(cell + Cell(0, 1));
		} else if (!Is_Concrete_Low_Bridge(cell + Cell(0, -2))) {
			FUN_00575870(cell);
		} else {
			FUN_00575870(cell + Cell(0, -1));
		}
	} else if (overlay >= 0xD6 && overlay <= 0xDE || overlay >= 0xE3 && overlay <= 0xE6 || overlay == 0xE8) {
		if (!Is_Concrete_Low_Bridge(cell + Cell(-1, 0))) {
			FUN_00575BA0(cell + Cell(1, 0));
		} else if (!Is_Concrete_Low_Bridge(cell + Cell(-2, 0))) {
			FUN_00575BA0(cell);
		} else {
			FUN_00575BA0(cell - Cell(1, 0));
		}
	}
}

// FUNCTION: 0x00575220
void MapClass::FUN_00575220(Cell const & cell)
{
	int back = 0;
	Cell c = cell;
	Cell start = c;
	int step = 1;
	int fwd = 0;
	do {
		c.X--;
		back++;
	} while (Is_Low_Bridge(c));
	c = start;
	do {
		c.X++;
		fwd++;
	} while (Is_Low_Bridge(c));
	if (fwd < back) {
		step = -1;
	}
	c = start;
	c.X = start.X - (back - fwd) / 2;
	int count = 4;
	while (true) {
		if (count <= 0) {
			break;
		}
		if (Map[c].Overlay != 0x64) {
			Cell tmp = Cell(c.X, c.Y - 1);
			for (int i = 0; i < 3; i++) {
				CellClass * cellptr = &Map[tmp];
				Coord coord = cellptr->Fetch_CellID();
				coord.Z = LEVEL_LEPTON_H * cellptr->Height;
				coord.X += Random_Double(-0.5, 0.5) * 50;
				coord.Y += Random_Double(-0.5, 0.5) * 50;
				new AnimClass(Rule->BridgeExplosions[Random_Pick(0, Rule->BridgeExplosions.Count() - 1)], coord, Random_Pick(1, 5));
				tmp = Cell(tmp.X, tmp.Y + 1);
			}
		}
		for (int j = 0; j < 3; j++) {
			if (FUN_0057BAA0(c)) {
				break;
			}
		}
		c.X += step;
		count--;
		int overlay = Map[c].Overlay;
		if (overlay < 0x4A || overlay > 0x65) {
			break;
		}
	}
	Update_Pathfinding_1();
	TacticalMap->IsFirstRender = true;
}

// FUNCTION: 0x00575540
void MapClass::FUN_00575540(Cell const & cell)
{
	int back = 0;
	Cell c = cell;
	Cell start = c;
	int step = 1;
	int fwd = 0;
	do {
		c.Y--;
		back++;
	} while (Is_Low_Bridge(c));
	c = start;
	do {
		c.Y++;
		fwd++;
	} while (Is_Low_Bridge(c));
	if (fwd < back) {
		step = -1;
	}
	c = start;
	c.Y = start.Y - (back - fwd) / 2;
	int count = 4;
	while (true) {
		if (count <= 0) {
			break;
		}
		if (Map[c].Overlay != 0x65) {
			Cell tmp = Cell(c.X - 1, c.Y);
			for (int i = 0; i < 3; i++) {
				CellClass * cellptr = &Map[tmp];
				Coord coord = cellptr->Fetch_CellID();
				coord.Z = LEVEL_LEPTON_H * cellptr->Height;
				coord.X += Random_Double(-0.5, 0.5) * 50;
				coord.Y += Random_Double(-0.5, 0.5) * 50;
				new AnimClass(Rule->BridgeExplosions[Random_Pick(0, Rule->BridgeExplosions.Count() - 1)], coord, Random_Pick(1, 5));
				tmp = Cell(tmp.X + 1, tmp.Y);
			}
		}
		for (int j = 0; j < 3; j++) {
			if (FUN_0057BAA0(c)) {
				break;
			}
		}
		c.Y += step;
		count--;
		int overlay = Map[c].Overlay;
		if (overlay < 0x4A || overlay > 0x65) {
			break;
		}
	}
	Update_Pathfinding_1();
	TacticalMap->IsFirstRender = true;
}

// FUNCTION: 0x00575870
void MapClass::FUN_00575870(Cell const & cell)
{
	int back = 0;
	Cell c = cell;
	Cell start = c;
	int step = 1;
	int fwd = 0;
	do {
		c.X--;
		back++;
	} while (Is_Concrete_Low_Bridge(c));
	c = start;
	do {
		c.X++;
		fwd++;
	} while (Is_Concrete_Low_Bridge(c));
	if (fwd < back) {
		step = -1;
	}
	c = start;
	c.X = start.X - (back - fwd) / 2;
	int count = 4;
	while (true) {
		if (count <= 0) {
			break;
		}
		if (Map[c].Overlay != 0xE7) {
			Cell tmp = Cell(c.X, c.Y - 1);
			for (int i = 0; i < 3; i++) {
				CellClass * cellptr = &Map[tmp];
				Coord coord = cellptr->Fetch_CellID();
				coord.Z = LEVEL_LEPTON_H * cellptr->Height;
				coord.X += Random_Double(-0.5, 0.5) * 50;
				coord.Y += Random_Double(-0.5, 0.5) * 50;
				new AnimClass(Rule->BridgeExplosions[Random_Pick(0, Rule->BridgeExplosions.Count() - 1)], coord, Random_Pick(1, 5));
				tmp = Cell(tmp.X, tmp.Y + 1);
			}
		}
		for (int j = 0; j < 3; j++) {
			if (FUN_0057CCF0(c)) {
				break;
			}
		}
		c.X += step;
		count--;
		int overlay = Map[c].Overlay;
		if (overlay < 0xCD || overlay > 0xE8) {
			break;
		}
	}
	Update_Pathfinding_1();
	TacticalMap->IsFirstRender = true;
}

// FUNCTION: 0x00575BA0
void MapClass::FUN_00575BA0(Cell const & cell)
{
	int back = 0;
	Cell c = cell;
	Cell start = c;
	int step = 1;
	int fwd = 0;
	do {
		c.Y--;
		back++;
	} while (Is_Concrete_Low_Bridge(c));
	c = start;
	do {
		c.Y++;
		fwd++;
	} while (Is_Concrete_Low_Bridge(c));
	if (fwd < back) {
		step = -1;
	}
	c = start;
	c.Y = start.Y - (back - fwd) / 2;
	int count = 4;
	while (true) {
		if (count <= 0) {
			break;
		}
		if (Map[c].Overlay != 0xE8) {
			Cell tmp = Cell(c.X - 1, c.Y);
			for (int i = 0; i < 3; i++) {
				CellClass * cellptr = &Map[tmp];
				Coord coord = cellptr->Fetch_CellID();
				coord.Z = LEVEL_LEPTON_H * cellptr->Height;
				coord.X += Random_Double(-0.5, 0.5) * 50;
				coord.Y += Random_Double(-0.5, 0.5) * 50;
				new AnimClass(Rule->BridgeExplosions[Random_Pick(0, Rule->BridgeExplosions.Count() - 1)], coord, Random_Pick(1, 5));
				tmp = Cell(tmp.X + 1, tmp.Y);
			}
		}
		for (int j = 0; j < 3; j++) {
			if (FUN_0057CCF0(c)) {
				break;
			}
		}
		c.Y += step;
		count--;
		int overlay = Map[c].Overlay;
		if (overlay < 0xCD || overlay > 0xE8) {
			break;
		}
	}
	Update_Pathfinding_1();
	TacticalMap->IsFirstRender = true;
}


// FUNCTION: 0x00575EE0
void MapClass::Spring_Bridge_Destruction_Triggers(Cell cell1, Cell cell2)
{
	bool is_east_west = false;
	Cell current;
	Cell end;
	if (cell1.Y == cell2.Y) {
		is_east_west = true;
		if (cell1.X > cell2.X) {
			swap(cell1, cell2);
		}
	} else if (cell1.Y > cell2.Y) {
		swap(cell1, cell2);
	}
	current = cell1;
	end = cell2;

	while (current != end) {
		CellClass * cptr = &Map[current];
		if (cptr->Tag != NULL) {
			cptr->Tag->Spring(TEVENT_BRIDGE_DESTROYED);
		}

		if (is_east_west) {
			cptr = &Map[Adjacent_Cell(current, FACING_S)];
			if (cptr->Tag != NULL) {
				cptr->Tag->Spring(TEVENT_BRIDGE_DESTROYED);
			}
			cptr = &Map[Adjacent_Cell(current, FACING_N)];
			if (cptr->Tag != NULL) {
				cptr->Tag->Spring(TEVENT_BRIDGE_DESTROYED);
			}
			cptr = &Map[Adjacent_Cell(cptr->CellID, FACING_N)];
			if (cptr->Tag != NULL) {
				cptr->Tag->Spring(TEVENT_BRIDGE_DESTROYED);
			}
			current += Cell(1, 0);
		} else {
			cptr = &Map[Adjacent_Cell(current, FACING_E)];
			if (cptr->Tag != NULL) {
				cptr->Tag->Spring(TEVENT_BRIDGE_DESTROYED);
			}
			cptr = &Map[Adjacent_Cell(current, FACING_W)];
			if (cptr->Tag != NULL) {
				cptr->Tag->Spring(TEVENT_BRIDGE_DESTROYED);
			}
			cptr = &Map[Adjacent_Cell(cptr->CellID, FACING_W)];
			if (cptr->Tag != NULL) {
				cptr->Tag->Spring(TEVENT_BRIDGE_DESTROYED);
			}
			current.Y++;
		}
	}
}

// FUNCTION: 0x00577920
void MapClass::Detach(AbstractClass const * target, bool all)
{
	if (target->RTTI == RTTI_TAG) {
		for (int index = 0; index < TaggedCells.Count(); index++) {
			CellClass *cellptr = &(*this)[TaggedCells[index]];
			if (cellptr != NULL && cellptr->Tag == (TagClass *)target) {
				cellptr->Attach_Tag(NULL);
				TaggedCells.Delete(cellptr->CellID);
				index--;
			}
		}
		InvalidCell.Detach(target);
	}

	if (target->RTTI == RTTI_TAG) {
		MapTags.Delete((TagClass *)target);
	}
}

// FUNCTION: 0x00577A50
Cell MapClass::Pick_Random_Location(void) const
{
	int x = Map.MapRect.X + Random_Pick(0, Map.MapRect.Width-1);
	int y = Map.MapRect.Y + Random_Pick(0, Map.MapRect.Height-1);
	return(Cell(x, y));
}

// FUNCTION: 0x00577AB0
void MapClass::Reshroud(HouseClass * house)
{
	if (house != NULL) {
		Houses[house->HeapID]->MapIsClear = false;
	}

	if (house == PlayerPtr || house == NULL) {
		if (house == NULL) {
			FUN_004068E0("We tried to reset the shroud without an owner");
		}
		Map.FUN_004ADEE0(false, false);
		Reset_Iterator();
		CellClass * cellptr = Iterate();
		while (cellptr != NULL) {
			cellptr->IsMapped = false;
			cellptr->IsVisible = false;
			cellptr->ShroudCounter = 1;
			cellptr->GapsCoveringThisCell = 0;
			cellptr->IsFogMapped = false;
			cellptr->IsFogVisible = false;
			cellptr = Iterate();
		}
		Map.FUN_004ADCD0(false, false);
		PlayerPtr->IsVisionary = false;
		Map.sub_657CE0();
		Flag_To_Redraw(GS_REDRAW_ALL);
	}
}

// FUNCTION: 0x00577BB0
void MapClass::Shroud_The_Map(HouseClass * house)
{
	if (house != NULL) {
		Houses[house->HeapID]->MapIsClear = false;
	}

	if (house == PlayerPtr || house == NULL) {
		if (house == NULL) {
			FUN_004068E0("We tried to reset the shroud without an owner");
		}
		Reset_Iterator();
		CellClass * cellptr = Iterate();
		while (cellptr != NULL) {
			cellptr->IsMapped = false;
			cellptr->IsVisible = false;
			cellptr->IsFogMapped = false;
			cellptr->IsFogVisible = false;
			cellptr = Iterate();
		}
		Map.FUN_004ADFF0(false, false);
		PlayerPtr->IsVisionary = false;
		Map.sub_657CE0();
		Flag_To_Redraw(GS_REDRAW_ALL);
	}
}

// FUNCTION: 0x00577C90
void MapClass::FUN_00577C90(HouseClass * house)
{
	if (house != NULL) {
		Houses[house->HeapID]->MapIsClear = false;
	}

	if (house == PlayerPtr || house == NULL) {
		if (house == NULL) {
			FUN_004068E0("We tried to reset the shroud without an owner");
		}
		Map.FUN_004ADEE0(false, false);
		Reset_Iterator();
		CellClass * cellptr = Iterate();
		while (cellptr != NULL) {
			cellptr->IncreaseShroudCounter();
			if (cellptr->ShroudCounter >= 0) {
				cellptr->IsMapped = false;
				cellptr->IsVisible = false;
				cellptr->IsFogMapped = false;
				cellptr->IsFogVisible = false;
			}
			cellptr = Iterate();
		}
		Map.FUN_004ADCD0(false, false);
		PlayerPtr->IsVisionary = false;
		Map.sub_657CE0();
		Flag_To_Redraw(GS_REDRAW_ALL);
	}
}

// FUNCTION: 0x00577F30
void MapClass::Reveal_The_Map(HouseClass * house)
{
	if (house != NULL) {
		Houses[house->HeapID]->MapIsClear = true;
	}

	if (house == PlayerPtr || house == NULL) {
		if (house == NULL) {
			FUN_004068E0("We tried to clear the shroud without an owner");
		}
		bool edge = (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.MPGameMode != NULL && !Session.MPGameMode->vt_entry_04();
		if (!PlayerPtr->IsVisionary) {
			PlayerPtr->IsVisionary = true;
			Map.Reset_Iterator();
			CellClass * cellptr = Map.Iterate();
			while (cellptr != NULL) {
				int x = cellptr->CellID.X;
				int y = cellptr->CellID.Y;
				if (!edge || !((x == 7 && y == PlayRect.Width + 5) ||
					(x == 13 && y == PlayRect.Width + 11) ||
					(x == PlayRect.Height + 13 && y == PlayRect.Width + PlayRect.Height - 15))) {
					Map.Shadow_Map_Cell(cellptr->CellID, PlayerPtr);
				}
				cellptr = Map.Iterate();
			}
			Map.sub_657CE0();
			Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
		}
	}
}

// FUNCTION: 0x00578080
int MapClass::Get_Height_GL(Coord const & coord)
{
	return(Map[coord].Get_Height(coord));
}

// FUNCTION: 0x00578100
void MapClass::FUN_00578100(void)
{
	Reset_Iterator();
	CellClass * cellptr = Iterate();
	while (cellptr != NULL) {
		if (cellptr->IsToShroud) {
			cellptr->IsMapped = false;
			cellptr->IsVisible = false;
			cellptr->IsFogMapped = false;
			cellptr->IsFogVisible = false;
			cellptr->IsToShroud = false;
			cellptr->VisibilityChanged = true;
			TacticalMap->Flag_Cell(*cellptr);
			if (cellptr->ShadowFrame == -2) {
				Map.Reveal_Nearby_Technos(cellptr, PlayerPtr, true);
			}
		}
		cellptr = Iterate();
	}

	Reset_Iterator();
	cellptr = Iterate();
	while (cellptr != NULL) {
		Cell cell = cellptr->Center_Coord().As_Cell();
		char frame = TacticalMap->Cell_Shadow(cell, false);
		if (frame != cellptr->ShadowFrame) {
			cellptr->ShadowFrame = frame;
			cellptr->VisibilityChanged = true;
			TacticalMap->Flag_Cell(*cellptr);
			if (cellptr->ShadowFrame == -2) {
				Map.Reveal_Nearby_Technos(cellptr, PlayerPtr, true);
			}
		}
		cellptr = Iterate();
	}
}

// FUNCTION: 0x00578290
CellClass * MapClass::Iterate(void)
{
	CellClass ** iter = IterCell;

	if (IterColumn != 0) {
		IterX++;
		IterY--;
		IterColumn--;
		IterCell = iter - (MAP_CELL_W - 1);
		return(*iter);
	}

	unsigned x = IterX;
	unsigned y = IterY;
	IterX = y;
	IterY = x;

	if (((IterX + IterY) - (PlayRect.Width + 1)) % 2) {
		IterY++;
		IterColumn = PlayRect.Width - 1;
	} else {
		IterX++;
		IterColumn = PlayRect.Width - 2;
	}

	IterCell = &Array[IterX + (IterY * MAP_CELL_H)];

	return(*iter);
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

// FUNCTION: 0x00578460
bool MapClass::In_Local_Radar(Cell const & cell, bool useheight) const
{
	int x = cell.X;
	int y = cell.Y;
	int cell_height = 0;

	if (useheight) {
		CellClass *cellptr = &Map[cell];

		cell_height = cellptr->Height;
		if (cellptr->Ramp && x + y < PlayRect.Width + 2*LocalRect.Y + 4 + cell_height) {
			cell_height++;
		}
	}

	if ((x + y > PlayRect.Width + 2*LocalRect.Y + cell_height) &&
		(x + y <= PlayRect.Width + 2*(LocalRect.Y + LocalRect.Height + 1) + cell_height) &&
		(x - y < 2*(LocalRect.X + LocalRect.Width) - PlayRect.Width) &&
		(y - x < PlayRect.Width - 2*LocalRect.X)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00578540
bool MapClass::In_Local_Radar(CellClass const * cell, bool useheight) const
{
	int x = cell->CellID.X;
	int y = cell->CellID.Y;
	int cell_height = 0;
	if (useheight) {
		cell_height = cell->Height;
		if (cell->Ramp && x + y < PlayRect.Width + 2*LocalRect.Y + 4 + cell_height) {
			cell_height++;
		}
	}

	if ((x + y > PlayRect.Width + 2*LocalRect.Y + cell_height) &&
		(x + y <= PlayRect.Width + 2*(LocalRect.Y + LocalRect.Height + 1) + cell_height) &&
		(x - y < 2*(LocalRect.X + LocalRect.Width) - PlayRect.Width) &&
		(y - x < PlayRect.Width - 2*LocalRect.X)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005785F0
bool MapClass::In_Local_Radar(Coord const & coord) const
{
	return(In_Local_Radar(coord.As_Cell()));
}

// FUNCTION: 0x00578640
bool MapClass::In_Area_Radar(Rect const & rect, Cell const & cell, bool useheight) const
{
	int x = cell.X;
	int y = cell.Y;
	int cell_height = 0;
	if (useheight) {
		CellClass *cellptr = &Map[cell];

		cell_height = cellptr->Height;
		if (cellptr->Ramp && x + y < PlayRect.Width + 2*rect.Y + 4 + cell_height) {
			cell_height++;
		}
	}

	if ((x + y > PlayRect.Width + 2*rect.Y + cell_height) &&
		(x + y <= PlayRect.Width + 2*(rect.Y + rect.Height + 1) + cell_height) &&
		(x - y < 2*(rect.X + rect.Width) - PlayRect.Width) &&
		(y - x < PlayRect.Width - 2*rect.X)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00578710
CellClass * MapClass::Local_Iterate(void)
{
	while (LocalIterX + LocalIterY <= PlayRect.Width + 2 * (LocalRect.Height + LocalRect.Y) + 12) {

		int x = LocalIterX;
		int y = LocalIterY;

		Cell cell(x, y);

		if (x - y >= 2 * (LocalRect.X + LocalRect.Width) - PlayRect.Width - 2) {
			int t = ((LocalIterX - PlayRect.Width) + LocalIterY - 1) & 1;
			bool odd = (t == 1);

			int delta = odd ? 2 : 1;

			int w = LocalRect.Width;
			w -= delta;

			LocalIterX -= w;
			LocalIterY += w;

			if (odd) {
				LocalIterY++;
			} else {
				LocalIterX++;
			}
		} else {
			LocalIterX++;
			LocalIterY--;
		}

		if (In_Local_Radar(cell, true)) {
			return(&(*this)[cell]);
		}
	}

	return(NULL);
}

// FUNCTION: 0x00578850
void MapClass::Reset_Local_Iterator(void)
{
	LocalIterX = LocalRect.X + LocalRect.Y + 1;
	LocalIterY = PlayRect.Width - LocalRect.X + LocalRect.Y;
}

// FUNCTION: 0x00578880
CellClass * MapClass::Get_Local_Grid_Cell(Cell const & cell, int spacing, bool unbiased)
{
	if (spacing == 1) {
		return(&Map[cell]);
	}

	int x = cell.X;
	int y = cell.Y;
	Cell local(LocalRect.X + LocalRect.Y + 1, PlayRect.Width - LocalRect.X + LocalRect.Y);
	int xx = (6000 * spacing + local.X - x) % spacing;
	int yy = (6000 * spacing + local.Y - y) % spacing;

	if (xx > 0) {
		x += spacing - xx;
	}
	if (yy > 0) {
		y += spacing - yy;
	}

	int dsum = local.X + local.Y;
	if (unbiased || (y + x < dsum + 2 * LocalRect.Height / 3)) {
		return(&Map[Cell(x, y)]);
	} else if (y + x < dsum + 4 * LocalRect.Height / 3 && spacing > 2) {
		return(&Map[Cell(x, y) - Cell((spacing / 2), (spacing / 2))]);
	} else {
		return(&Map[Cell(x, y) - Cell((spacing - 1), (spacing - 1))]);
	}
}

// FUNCTION: 0x00578AC0
void MapClass::Increment_Redraw_Counter(void)
{
	Redraws++;
}

// FUNCTION: 0x00578AD0
bool MapClass::Try_Open_Gate(FootClass * foot, Cell const & cell)
{
	FootClass * optr = (FootClass *)Map[cell].Cell_Occupier();
	while (optr != NULL) {

		if (foot != optr) {

			BuildingClass * bptr = (BuildingClass *)optr;
			if (bptr->RTTI == RTTI_BUILDING && bptr->Class->IsGate) {

				if (bptr->House->Is_Ally(foot)) {
					return(bptr->Open_Gate());
				}

				if (bptr->Is_Gate_Open()) {
					return(true);
				}
			}
		}

		optr = (FootClass *)optr->Next;
	}
	return(true);
}

// FUNCTION: 0x00578B70
int MapClass::Cell_To_Local_Sector_Index(Cell const & cell)
{
	int w1 = (2 * LocalRect.Width - 1) % 3;
	int w2 = (2 * LocalRect.Width - 1) / 3;

	int h1 = (2 * LocalRect.Height - 1) % 3;
	int h2 = (2 * LocalRect.Height - 1) / 3;

	int x1 = LocalRect.X + LocalRect.Y + 1;
	int y1 = PlayRect.Width - LocalRect.X + LocalRect.Y;

	Cell pos = Cell(x1, y1);

	int x2 = (cell.X + (pos.Y - (w1 ? 1 : 0) - pos.X - cell.Y)) / w2;
	int y2 = (cell.X + (cell.Y - (h1 ? 1 : 0) - pos.X - pos.Y)) / h2;

	if (x2 == 3) {
		x2 = 2;
	}
	if (y2 == 3) {
		y2 = 2;
	}

	return(x2 + 3 * y2 + 1);
}

// FUNCTION: 0x00578C50
Cell MapClass::Local_Sector_Index_To_Cell(int index)
{
	int w1 = (2 * LocalRect.Width - 1) % 3;
	int w2 = (2 * LocalRect.Width - 1) / 3;

	int h1 = (2 * LocalRect.Height - 1) % 3;
	int h2 = (2 * LocalRect.Height - 1) / 3;

	int x = (index - 1) % 3;
	int y = (index - 1) / 3;

	int sector_x1 = (w1 ? 1 : 0) + (x * w2);
	int sector_y1 = (h1 ? 1 : 0) + (y * h2);

	Cell pos = Cell(LocalRect.X + LocalRect.Y + 1, PlayRect.Width - LocalRect.X + LocalRect.Y);

	int sector_x2 = sector_x1 + ((sector_x1 & 1) ? 1 : 0);
	int sector_y2 = sector_y1 + ((sector_y1 & 1) ? 1 : 0);

	int x2 = (sector_y2 + sector_x2) / 2;
	int y2 = (sector_y1 - sector_x1) / 2;

	return(Cell(x2,y2) + pos);
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

// FUNCTION: 0x00578E60
bool MapClass::Build_All_Cliffs(int, int region_id)
{
	bool expanded = true;

	bool is_temp_data = false;
	if (RMGCellData == NULL) {
		RMGCellData = new MapRegionClass::CellData[MapCellStride * MapCellStride];
		is_temp_data = true;
	}

	int r = MapCellStride;
	int s = r * r;
	for (int i = 0; i < s; i++) {
		MapRegionClass::Get_Cell_Data(i).InFillReach = true;
	}

	Map.Set_Cursor_Shape(NULL);

	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL && expanded) {
		expanded = Expand_High_Ground(cptr, region_id);
		cptr = Map.Iterate();
	}

	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr != NULL && expanded) {
		if (cptr->Ramp == 0 && cptr->Is_Tile_Clear()) {
			expanded = Place_Cliff(cptr, region_id);
		}
		cptr = Map.Iterate();
	}

	ObjectClass * obj = Map.PendingObjectPtr;
	Map.PendingObject = NULL;
	if (obj) {
		delete obj;
		Map.PendingObjectPtr = NULL;
	}

	if (is_temp_data) {
		delete [] RMGCellData;
		RMGCellData = NULL;
	}

	return(expanded);
}

// FUNCTION: 0x00579010
bool MapClass::Expand_High_Ground(CellClass * cellptr, int region_id)
{
	bool expand = false;
	int mask = Get_High_Ground_Mask(cellptr);

	if ((mask & (MG_FACINGF_N | MG_FACINGF_W)) == (MG_FACINGF_N | MG_FACINGF_W) &&
		(mask & (MG_FACINGF_SW | MG_FACINGF_NE)) == (MG_FACINGF_SW | MG_FACINGF_NE)) {
		expand = true;
	}

	if ((mask & MG_FACINGF_W) != 0) {
		if ((Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(1, 0)]) & MG_FACINGF_E) != 0) {
			expand = true;
		}
	}

	if ((mask & MG_FACINGF_E) != 0) {
		if ((Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(-1, 0)]) & MG_FACINGF_W) != 0) {
			expand = true;
		}
	}

	if ((mask & MG_FACINGF_S) != 0) {
		if ((Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(0, -1)]) & MG_FACINGF_N) != 0) {
			expand = true;
		}
	}

	if ((mask & MG_FACINGF_N) != 0) {
		if ((Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(0, 1)]) & MG_FACINGF_S) != 0) {
			expand = true;
		}
	}

	if ((mask & (MG_FACINGF_SW | MG_FACINGF_NE)) == (MG_FACINGF_SW | MG_FACINGF_NE) &&
		((mask & (MG_FACINGF_S | MG_FACINGF_E)) != (MG_FACINGF_S | MG_FACINGF_E) ||
		 (mask & (MG_FACINGF_N | MG_FACINGF_NW | MG_FACINGF_W)) != 0) &&
		((mask & (MG_FACINGF_N | MG_FACINGF_W)) != (MG_FACINGF_N | MG_FACINGF_W) ||
		 (mask & (MG_FACINGF_S | MG_FACINGF_SE | MG_FACINGF_E)) != 0)) {
		expand = true;
	}

	if ((mask & (MG_FACINGF_NW | MG_FACINGF_SE)) == (MG_FACINGF_NW | MG_FACINGF_SE) &&
		((mask & (MG_FACINGF_W | MG_FACINGF_S)) != (MG_FACINGF_W | MG_FACINGF_S) ||
		 (mask & (MG_FACINGF_N | MG_FACINGF_E | MG_FACINGF_NE)) != 0) &&
		((mask & (MG_FACINGF_N | MG_FACINGF_E)) != (MG_FACINGF_N | MG_FACINGF_E) ||
		 (mask & (MG_FACINGF_W | MG_FACINGF_SW | MG_FACINGF_S)) != 0)) {
		expand = true;
	}

	if ((mask & (MG_FACINGF_W | MG_FACINGF_S | MG_FACINGF_SE)) == (MG_FACINGF_W | MG_FACINGF_SE) ||
		(mask & (MG_FACINGF_N | MG_FACINGF_W | MG_FACINGF_NE)) == (MG_FACINGF_W | MG_FACINGF_NE) ||
		(mask & (MG_FACINGF_SW | MG_FACINGF_S | MG_FACINGF_E)) == (MG_FACINGF_SW | MG_FACINGF_E) ||
		(mask & (MG_FACINGF_N | MG_FACINGF_NW | MG_FACINGF_E)) == (MG_FACINGF_NW | MG_FACINGF_E) ||
		(mask & (MG_FACINGF_S | MG_FACINGF_E | MG_FACINGF_NE)) == (MG_FACINGF_S | MG_FACINGF_NE) ||
		(mask & (MG_FACINGF_NW | MG_FACINGF_W | MG_FACINGF_S)) == (MG_FACINGF_NW | MG_FACINGF_S) ||
		(mask & (MG_FACINGF_N | MG_FACINGF_SE | MG_FACINGF_E)) == (MG_FACINGF_N | MG_FACINGF_SE) ||
		(mask & (MG_FACINGF_N | MG_FACINGF_W | MG_FACINGF_SW)) == (MG_FACINGF_N | MG_FACINGF_SW)) {
		expand = true;
	}

	if ((mask & (MG_FACINGF_N | MG_FACINGF_S)) == (MG_FACINGF_N | MG_FACINGF_S) ||
		(mask & (MG_FACINGF_W | MG_FACINGF_E)) == (MG_FACINGF_W | MG_FACINGF_E)) {
		expand = true;
	}

	if (expand) {
		int cell_region_id = RandomMapGen.Get_Cell_Data_Region(cellptr->Fetch_CellID());
		if (cell_region_id > 0 && cell_region_id != region_id && region_id != -1) {
			return(false);
		}
		cellptr->Height += 4;
		RandomMapGen.Set_Cell_Data_Region(cellptr->Fetch_CellID(), region_id);
		for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
			Expand_High_Ground(&cellptr->Adjacent_Cell(dir), region_id);
		}
	}
	return(true);
}

// FUNCTION: 0x00579320 $E230
const Cell CliffVariantOffsets[] = {
	Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0),
	Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0),
	Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0),
	Cell(0, 0), Cell(1, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0), Cell(0, 0),
};

// FUNCTION: 0x00579620
bool MapClass::Place_Cliff(CellClass * cellptr, int region_id)
{
	int random6 = Pick_Random_UInt(0, 5);
	int mask = Get_High_Ground_Mask(cellptr);

	int variant = -1;
	int south;
	int east;
	int southeast;

	if ((mask & (MG_FACINGF_N | MG_FACINGF_W)) == (MG_FACINGF_N | MG_FACINGF_W)) {

		south = Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(0, 1)]);
		east = Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(1, 0)]);

		if ((south & MG_FACINGF_W) != 0 || (east & MG_FACINGF_N) != 0) {
			variant = 34;
		} else {
			variant = random6 % 3 + 9;
		}

	} else if ((mask & (MG_FACINGF_N | MG_FACINGF_E)) == (MG_FACINGF_N | MG_FACINGF_E)) {
		variant = 39;

	} else if ((mask & (MG_FACINGF_E | MG_FACINGF_S)) == (MG_FACINGF_E | MG_FACINGF_S)) {
		variant = 33;

	} else if ((mask & (MG_FACINGF_S | MG_FACINGF_W)) == (MG_FACINGF_S | MG_FACINGF_W)) {
		variant = 40;
	}

	if (variant == -1) {

		if ((mask & MG_FACINGF_E) != 0) {

			south = Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(0, 1)]);
			southeast = Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(1, 1)]);

			if ((mask & MG_FACINGF_SE) != 0 && (mask & (MG_FACINGF_SW | MG_FACINGF_S)) == 0 && (south & (MG_FACINGF_E | MG_FACINGF_S)) != (MG_FACINGF_E | MG_FACINGF_S)) {
				variant = random6 % 3 + 35;
			} else if ((mask & (MG_FACINGF_SE | MG_FACINGF_S)) != 0 || (southeast & MG_FACINGF_E) != 0 || (random6 & 1) == 0) {
				variant = 38;
			} else {
				variant = 1;
			}

		} else if ((mask & MG_FACINGF_W) != 0) {

			south = Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(0, 1)]);

			if ((mask & MG_FACINGF_SW) == 0 || (mask & (MG_FACINGF_SE | MG_FACINGF_S)) != 0 || (south & (MG_FACINGF_S | MG_FACINGF_W)) == (MG_FACINGF_S | MG_FACINGF_W)) {
				variant = 18;
			} else {
				variant = random6 % 3 + 15;
			}

		} else if ((mask & MG_FACINGF_S) != 0) {

			east = Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(1, 0)]);
			southeast = Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(1, 1)]);

			if ((mask & MG_FACINGF_SE) == 0 || (mask & (MG_FACINGF_NE | MG_FACINGF_E)) != 0 || (east & (MG_FACINGF_E | MG_FACINGF_S)) == (MG_FACINGF_E | MG_FACINGF_S)) {
				if ((mask & (MG_FACINGF_E | MG_FACINGF_SE)) != 0 || (southeast & MG_FACINGF_S) != 0 || (random6 & 1) == 0) {
					variant = 26;
				} else {
					variant = 21;
				}
			} else {
				variant = random6 % 3 + 23;
			}

		} else if ((mask & MG_FACINGF_N) != 0) {

			east = Get_High_Ground_Mask(&Map[cellptr->Fetch_CellID() + Cell(1, 0)]);

			if ((mask & MG_FACINGF_NE) == 0 || (mask & (MG_FACINGF_E | MG_FACINGF_SE)) != 0 || (east & (MG_FACINGF_N | MG_FACINGF_E)) == (MG_FACINGF_N | MG_FACINGF_E)) {
				variant = 8;
			} else {
				variant = random6 % 3 + 5;
			}
		}
	}

	if (variant == -1) {

		if ((mask & MG_FACINGF_NE) != 0) {
			variant = 2;

		} else if ((mask & MG_FACINGF_SE) != 0) {
			variant = (random6 & 1) + 29;

		} else if ((mask & MG_FACINGF_SW) != 0) {
			variant = 22;

		} else if ((mask & MG_FACINGF_NW) != 0) {
			variant = random6 % 3 + 12;

		} else {
			goto done;
		}
	}

	if (variant <= 0) {
		goto done;
	}

	Map.PendingObject = IsometricTileTypes[IsometricTileTypeClass::CliffSet + variant - 1];

	{
		Cell target = cellptr->Fetch_CellID();
		target = CliffVariantOffsets[variant - 1] + target;
		Map.Set_Cursor_Pos(target);

		bool success = true;
		bool picked = Pick_Random_Tile_Variant(ISOTILE_CLEAR, 0, cellptr->Height, region_id, success, false);

		if (!picked && !success) {
			return(false);
		}
	}

done:
	return(true);
}

// FUNCTION: 0x0057A0C0
bool MapClass::FUN_0057A0C0(int region_id, bool flat_height)
{
	Map.Set_Cursor_Shape(NULL);

	bool is_temp_data = false;
	if (RMGCellData == NULL) {
		RMGCellData = new MapRegionClass::CellData[MapCellStride * MapCellStride];
		is_temp_data = true;
	}

	int r = MapCellStride;
	int s = r * r;
	for (int i = 0; i < s; i++) {
		MapRegionClass::Get_Cell_Data(i).WaterMask = -1;
	}

	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	bool expanded = true;
	while (cptr != NULL && expanded) {
		expanded = Expand_Water(cptr, region_id, flat_height);
		cptr = Map.Iterate();
	}

	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr != NULL && expanded) {
		Prune_Water(cptr, region_id, flat_height);
		cptr = Map.Iterate();
	}

	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr != NULL && expanded) {
		expanded = Place_Shore(cptr, 1, region_id, flat_height);
		cptr = Map.Iterate();
	}

	Map.Reset_Iterator();
	cptr = Map.Iterate();
	while (cptr != NULL && expanded) {
		expanded = Place_Shore(cptr, 2, region_id, flat_height);
		cptr = Map.Iterate();
	}

	ObjectClass * obj = Map.PendingObjectPtr;
	Map.PendingObject = NULL;
	if (obj) {
		delete obj;
		Map.PendingObjectPtr = NULL;
	}

	if (is_temp_data) {
		delete [] RMGCellData;
		RMGCellData = NULL;
	}

	return(expanded);
}

// FUNCTION: 0x0057A320
void MapClass::Prune_Water(CellClass * cellptr, int region_id, bool flat_height)
{
	IsometricTileType itype = cellptr->ITType;
	if (itype >= IsometricTileTypeClass::WaterSet && itype < IsometricTileType(IsometricTileTypeClass::WaterSet + WATER_COUNT-2)) {
		int mask = Get_Water_Mask(cellptr, 2);
		if (mask == (MG_FACINGF_N | MG_FACINGF_NE | MG_FACINGF_E | MG_FACINGF_SE | MG_FACINGF_NW) ||
			mask == (MG_FACINGF_SE | MG_FACINGF_S | MG_FACINGF_SW | MG_FACINGF_W | MG_FACINGF_NW) ||
			mask == (MG_FACINGF_N | MG_FACINGF_NE | MG_FACINGF_SW | MG_FACINGF_W | MG_FACINGF_NW) ||
			mask == (MG_FACINGF_NE | MG_FACINGF_E | MG_FACINGF_SE | MG_FACINGF_S | MG_FACINGF_SW) ||
			mask == (MG_FACINGF_N | MG_FACINGF_E | MG_FACINGF_SE | MG_FACINGF_NW) ||
			mask == (MG_FACINGF_SE | MG_FACINGF_S | MG_FACINGF_W | MG_FACINGF_NW) ||
			mask == (MG_FACINGF_N | MG_FACINGF_NE | MG_FACINGF_SW | MG_FACINGF_W) ||
			mask == (MG_FACINGF_NE | MG_FACINGF_E | MG_FACINGF_S | MG_FACINGF_SW)) {

			cellptr->ITType = ISOTILE_NONE;
			cellptr->SubTile = 0;
			if (flat_height) {
				MapRegionClass::Get_Cell_Data(cellptr->CellID).RegionID = region_id;
			}
			for (FacingType facing = FACING_N; facing < FACING_COUNT; facing++) {
				CellClass * adjptr = &cellptr->Adjacent_Cell(facing);
				if (My_In_Radar(adjptr->Fetch_CellID())) {
					MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(adjptr->Fetch_CellID());
					data.WaterMask = -1;
					data.WaterMask = Get_Water_Mask(adjptr, 2);
				}
			}
		}
	}
}

// FUNCTION: 0x0057A430
bool MapClass::Expand_Water(CellClass * cellptr, int region_id, bool flat_height)
{
	bool expand = false;

	MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(cellptr->Fetch_CellID());
	data.WaterMask = Get_Water_Mask(cellptr, 2);

	int mask = Get_Water_Mask(cellptr, 0);
	if (mask > 0) {

		if (mask == (MG_FACINGF_NE | MG_FACINGF_E | MG_FACINGF_S) ||
			mask == (MG_FACINGF_E | MG_FACINGF_S | MG_FACINGF_SW)) {
			expand = true;
		}

		if ((mask & (MG_FACINGF_W | MG_FACINGF_N)) == (MG_FACINGF_W | MG_FACINGF_N) && (mask & (MG_FACINGF_NE | MG_FACINGF_SW)) == 0 ||
			(mask & (MG_FACINGF_E | MG_FACINGF_N)) == (MG_FACINGF_E | MG_FACINGF_N) && (mask & (MG_FACINGF_SE | MG_FACINGF_NW)) == 0 ||
			(mask & (MG_FACINGF_E | MG_FACINGF_S)) == (MG_FACINGF_E | MG_FACINGF_S) && (mask & (MG_FACINGF_NE | MG_FACINGF_SW)) == 0 ||
			(mask & (MG_FACINGF_S | MG_FACINGF_W)) == (MG_FACINGF_S | MG_FACINGF_W) && (mask & (MG_FACINGF_SE | MG_FACINGF_NW)) == 0) {
			expand = true;
		}

		if ((mask & MG_FACINGF_W) != 0) {
			int mask1 = Get_Water_Mask(&Map[cellptr->Fetch_CellID() + Cell(1, 0)], 0);
			int mask2 = Get_Water_Mask(&Map[cellptr->Fetch_CellID() + Cell(2, 0)], 0);
			if ((mask1 & MG_FACINGF_E) != 0 || (mask2 & MG_FACINGF_E) != 0) {
				expand = true;
			}
		}

		if ((mask & MG_FACINGF_E) != 0) {
			int mask1 = Get_Water_Mask(&Map[cellptr->Fetch_CellID() + Cell(-1, 0)], 0);
			int mask2 = Get_Water_Mask(&Map[cellptr->Fetch_CellID() + Cell(-2, 0)], 0);
			if ((mask1 & MG_FACINGF_W) != 0 || (mask2 & MG_FACINGF_W) != 0) {
				expand = true;
			}
		}

		if ((mask & MG_FACINGF_S) != 0) {
			int mask1 = Get_Water_Mask(&Map[cellptr->Fetch_CellID() + Cell(0, -1)], 0);
			int mask2 = Get_Water_Mask(&Map[cellptr->Fetch_CellID() + Cell(0, -2)], 0);
			if ((mask1 & MG_FACINGF_N) != 0 || (mask2 & MG_FACINGF_N) != 0) {
				expand = true;
			}
		}

		if ((mask & MG_FACINGF_N) != 0) {
			int mask1 = Get_Water_Mask(&Map[cellptr->Fetch_CellID() + Cell(0, 1)], 0);
			int mask2 = Get_Water_Mask(&Map[cellptr->Fetch_CellID() + Cell(0, 2)], 0);
			if ((mask1 & MG_FACINGF_S) != 0 || (mask2 & MG_FACINGF_S) != 0) {
				expand = true;
			}
		}

		if ((mask & (MG_FACINGF_N | MG_FACINGF_S)) == (MG_FACINGF_N | MG_FACINGF_S) ||
			(mask & (MG_FACINGF_E | MG_FACINGF_W)) == (MG_FACINGF_E | MG_FACINGF_W) ||
			expand) {

			int cell_region_id = RandomMapGen.Get_Cell_Data_Region(cellptr->Fetch_CellID());
			if (cell_region_id > 0 && cell_region_id != region_id && !flat_height) {
				return(false);
			}
			cellptr->ITType = IsometricTileType(IsometricTileTypeClass::WaterSet);
			cellptr->SubTile = 0;
			if (flat_height) {
				RandomMapGen.Set_Cell_Data_Region(cellptr->Fetch_CellID(), 0);
			} else {
				RandomMapGen.Set_Cell_Data_Region(cellptr->Fetch_CellID(), region_id);
			}
			for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
				CellClass * adjptr = &cellptr->Adjacent_Cell(dir);
				if (My_In_Radar(adjptr->Fetch_CellID())) {
					MapRegionClass::Get_Cell_Data(adjptr->Fetch_CellID()).WaterMask = -1;
					Expand_Water(adjptr, region_id, flat_height);
				}
			}
		}
	}
	return(true);
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

// FUNCTION: 0x0057A9C0 $E234
Cell ShorePieceOffsets[] = {
	Cell(0, -1),
	Cell(0, -1),
	Cell(0, -1),
	Cell(0, -1),
	Cell(0, -2),
	Cell(-1, -2),
	Cell(-1, -1),
	Cell(-1, -1),
	Cell(-1, 0),
	Cell(-1, 0),
	Cell(-1, 0),
	Cell(-1, 0),
	Cell(-2, -1),
	Cell(-2, 0),
	Cell(-1, 0),
	Cell(-1, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(-1, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(0, -1),
	Cell(0, 0),
	Cell(0, -1),
	Cell(0, -1),
	Cell(-1, -1),
	Cell(-1, -1),
	Cell(-1, 0),
	Cell(-1, 0),
	Cell(0, 0),
	Cell(0, 0),
	Cell(0, -1),
	Cell(0, -1),
	Cell(0, 0),
	Cell(0, 0)
};

// FUNCTION: 0x0057ACF0
bool MapClass::Place_Shore(CellClass * cellptr, int pass, int region_id, bool flat_height)
{
	int random = Pick_Random_UInt(0, 5);

	int mask;
	if (pass == 2) {
		mask = Get_Water_Mask(cellptr, 1);
	} else {
		mask = Get_Water_Mask(cellptr, 0);
	}
	if (mask == 0) {
		return(true);
	}

	int index;
	int count;
	CellClass * adj1;
	CellClass * adj2;
	bool side_1_is_land;
	bool side_2_is_water;

	if (pass == 2) {

		if ((mask & (MG_FACINGF_N | MG_FACINGF_W)) == (MG_FACINGF_N | MG_FACINGF_W)) {
			if ((mask & (MG_FACINGF_NE | MG_FACINGF_SW)) == (MG_FACINGF_NE | MG_FACINGF_SW)) {
				index = (random & 1) + 23;
			} else {
				if ((mask & MG_FACINGF_NE) != 0) {
				index = 21;
			} else {
				index = 30;
			}
			}
		} else if ((mask & (MG_FACINGF_N | MG_FACINGF_E)) == (MG_FACINGF_N | MG_FACINGF_E)) {
			if ((mask & (MG_FACINGF_NW | MG_FACINGF_SE)) == (MG_FACINGF_NW | MG_FACINGF_SE)) {
				index = (random & 1) + 15;
			} else {
				if ((mask & MG_FACINGF_SE) != 0) {
				index = 14;
			} else {
				index = 22;
			}
			}
		} else if ((mask & (MG_FACINGF_S | MG_FACINGF_E)) == (MG_FACINGF_S | MG_FACINGF_E)) {
			if ((mask & (MG_FACINGF_NE | MG_FACINGF_SW)) == (MG_FACINGF_NE | MG_FACINGF_SW)) {
				index = (random & 1) + 7;
			} else {
				if ((mask & MG_FACINGF_NE) != 0) {
				index = 13;
			} else {
				index = 6;
			}
			}
		} else {
			if ((mask & (MG_FACINGF_W | MG_FACINGF_S)) != (MG_FACINGF_W | MG_FACINGF_S)) {
				return(true);
			}
			if ((mask & (MG_FACINGF_NW | MG_FACINGF_SE)) == (MG_FACINGF_NW | MG_FACINGF_SE)) {
				index = (random & 1) + 31;
			} else {
				if ((mask & MG_FACINGF_SE) != 0) {
				index = 5;
			} else {
				index = 29;
			}
			}
		}

	} else {
		index = -1;
	if ((mask & MG_FACINGF_E) != 0) {
		count = 1;
		adj1 = &cellptr->Adjacent_Cell(FACING_S);
		adj2 = &adj1->Adjacent_Cell(FACING_E);
		side_1_is_land = adj1->Is_Tile_Water() == false;
		side_2_is_water = adj2->Is_Tile_Water();
		while (side_1_is_land && side_2_is_water) {
			count++;
			adj1 = &adj1->Adjacent_Cell(FACING_S);
			adj2 = &adj2->Adjacent_Cell(FACING_S);
			side_1_is_land = adj1->Is_Tile_Water() == false;
			side_2_is_water = adj2->Is_Tile_Water();
		}
		if ((count & 1) != 0 && (mask & MG_FACINGF_N) == 0) {
			index = 12;
		} else if ((mask & MG_FACINGF_SE) == 0 || (mask & (MG_FACINGF_S | MG_FACINGF_SW)) != 0) {
			index = 12;
		} else {
			index = abs(random % 3) + 9;
		}
	} else if ((mask & MG_FACINGF_W) != 0) {
		count = 1;
		adj1 = &cellptr->Adjacent_Cell(FACING_S);
		adj2 = &adj1->Adjacent_Cell(FACING_W);
		side_1_is_land = adj1->Is_Tile_Water() == false;
		side_2_is_water = adj2->Is_Tile_Water();
		while (side_1_is_land && side_2_is_water) {
			count++;
			adj1 = &adj1->Adjacent_Cell(FACING_S);
			adj2 = &adj2->Adjacent_Cell(FACING_S);
			side_1_is_land = adj1->Is_Tile_Water() == false;
			side_2_is_water = adj2->Is_Tile_Water();
		}
		if ((count & 1) != 0 && (mask & MG_FACINGF_N) == 0) {
			index = 28;
		} else if ((mask & MG_FACINGF_SW) == 0 || (mask & (MG_FACINGF_SE | MG_FACINGF_S)) != 0) {
			index = 28;
		} else {
			index = random % 3 + 25;
		}
	} else if ((mask & MG_FACINGF_S) != 0) {
		count = 1;
		adj1 = &cellptr->Adjacent_Cell(FACING_E);
		adj2 = &adj1->Adjacent_Cell(FACING_S);
		side_1_is_land = adj1->Is_Tile_Water() == false;
		side_2_is_water = adj2->Is_Tile_Water();
		while (side_1_is_land && side_2_is_water) {
			count++;
			adj1 = &adj1->Adjacent_Cell(FACING_E);
			adj2 = &adj2->Adjacent_Cell(FACING_E);
			side_1_is_land = adj1->Is_Tile_Water() == false;
			side_2_is_water = adj2->Is_Tile_Water();
		}
		if ((count & 1) != 0) {
			index = 4;
		} else if ((mask & MG_FACINGF_SE) == 0 || (mask & (MG_FACINGF_NE | MG_FACINGF_E)) != 0) {
			index = 4;
		} else {
			index = random % 3 + 1;
		}
	} else if ((mask & MG_FACINGF_N) != 0) {
		count = 1;
		adj1 = &cellptr->Adjacent_Cell(FACING_E);
		adj2 = &adj1->Adjacent_Cell(FACING_N);
		side_1_is_land = adj1->Is_Tile_Water() == false;
		side_2_is_water = adj2->Is_Tile_Water();
		while (side_1_is_land && side_2_is_water) {
			count++;
			adj1 = &adj1->Adjacent_Cell(FACING_E);
			adj2 = &adj2->Adjacent_Cell(FACING_E);
			side_1_is_land = adj1->Is_Tile_Water() == false;
			side_2_is_water = adj2->Is_Tile_Water();
		}
		if ((count & 1) != 0) {
			index = 20;
		} else if ((mask & MG_FACINGF_NE) == 0 || (mask & (MG_FACINGF_E | MG_FACINGF_SE)) != 0) {
			index = 20;
		} else {
			index = random % 3 + 17;
		}
	}

	if (index == -1) {

		if ((mask & MG_FACINGF_NE) != 0) {
			index = (random & 1) + 35;
		} else if ((mask & MG_FACINGF_SE) != 0) {
			index = (random & 1) + 33;
		} else if ((mask & MG_FACINGF_SW) != 0) {
			index = (random & 1) + 39;
		} else {
			if ((mask & MG_FACINGF_NW) == 0) {
				return(true);
			}
			index = (random & 1) + 37;
		}
	}
	}

	if (index > 0) {

		Map.PendingObject = IsometricTileTypes[IsometricTileTypeClass::ShorePieces + index - 1];

		Cell cellid = cellptr->Fetch_CellID();
		Cell cursor = Map.Set_Cursor_Pos(cellid + ShorePieceOffsets[index - 1]);

		bool result = true;
		if (pass == 1) {
			Pick_Random_Tile_Variant(ISOTILE_CLEAR, ISOTILE_CLEAR, cellptr->Height, region_id, result, flat_height);
		} else {
			if (pass != 2) {
				return(true);
			}
			Pick_Random_Tile_Variant(IsometricTileTypeClass::ShorePieces, IsometricTileTypeClass::ShorePieces + SHORE_PIECES_COUNT - 1, cellptr->Height, region_id, result, flat_height);
		}

		if (!result) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: 0x0057B210
int MapClass::Get_Water_Mask(CellClass * cellptr, int control)
{
	int mask = 0;
	Cell cell = cellptr->Fetch_CellID();

	IsometricTileType water_start = IsometricTileType(IsometricTileTypeClass::WaterSet);
	IsometricTileType water_end = IsometricTileType(water_start + WATER_COUNT);

	bool is_water = cellptr->Is_Tile_Water();
	int x = cell.X;
	int y = cell.Y;

	if (!My_In_Radar(cell)) {
		return(mask);
	}

	if (control == 0) {
		if (!cellptr->CellClass::Is_Tile_Clear()) {
			return(mask);
		}
	} else if (control == 1) {
		if (cellptr->ITType >= water_start && cellptr->ITType < water_end) {
			return(mask);
		}
	}

	MapRegionClass::CellData & data = MapRegionClass::Get_Cell_Data(cellptr->Fetch_CellID());
	if (!data.InFillReach) {
		return(mask);
	}
	if (data.WaterMask >= 0) {
		return(data.WaterMask);
	}

	CellClass ** cptr = &Array[x + y * MAP_CELL_H - MAP_CELL_W - 1];

	CellClass * tptr = cptr[0];
	if (tptr && tptr->Is_Tile_Water() || cptr[0] == NULL && is_water) {
		mask |= MG_FACINGF_NW;
	}
	cptr += 1;
	tptr = cptr[0];
	if (tptr && tptr->Is_Tile_Water() || cptr[0] == NULL && is_water) {
		mask |= MG_FACINGF_N;
	}
	cptr += 1;
	tptr = cptr[0];
	if (tptr && tptr->Is_Tile_Water() || cptr[0] == NULL && is_water) {
		mask |= MG_FACINGF_NE;
	}
	cptr += MAP_CELL_W - 2;
	tptr = cptr[0];
	if (tptr && tptr->Is_Tile_Water() || cptr[0] == NULL && is_water) {
		mask |= MG_FACINGF_W;
	}
	cptr += 2;
	tptr = cptr[0];
	if (tptr && tptr->Is_Tile_Water() || cptr[0] == NULL && is_water) {
		mask |= MG_FACINGF_E;
	}
	cptr += MAP_CELL_W - 2;
	tptr = cptr[0];
	if (tptr && tptr->Is_Tile_Water() || cptr[0] == NULL && is_water) {
		mask |= MG_FACINGF_SW;
	}
	cptr += 1;
	tptr = cptr[0];
	if (tptr && tptr->Is_Tile_Water() || cptr[0] == NULL && is_water) {
		mask |= MG_FACINGF_S;
	}
	cptr += 1;
	tptr = cptr[0];
	if (tptr && tptr->Is_Tile_Water() || cptr[0] == NULL && is_water) {
		mask |= MG_FACINGF_SE;
	}
	return(mask);
}

// FUNCTION: 0x0057B790
void MapClass::Area_Reduce_Tiberium(Cell const & cell)
{
	for (int x = -5; x <= 5; x++) {
		for (int y = -5; y <= 5; y++) {
			Cell c1 = Cell(cell + Cell(x,y));
			Cell c2 = (cell - c1);
			if (c2.Length() <= 5) {
				CellClass * cellptr = &Map[cell];
				if (cellptr != NULL) {
					cellptr->Reduce_Tiberium(1);
				}
			}
		}
	}
}

// FUNCTION: 0x0057B870
int MapClass::Get_Low_Bridge_EW_Neighbor_Mask(Cell const & cell)
{
	OverlayType o1 = Map[cell + Cell(-1, 0)].Overlay;
	OverlayType o2 = Map[cell + Cell(1, 0)].Overlay;

	int index = 0;
	switch (o2) {
		case OVERLAY_LOWBRIDGE_05:
		case OVERLAY_LOWBRIDGE_07:
		case OVERLAY_LOWBRIDGE_09:
		case OVERLAY_LOWBRIDGE_20:
			index |= 1;
			break;
		case OVERLAY_LOWBRIDGE_08:
		case OVERLAY_LOWBRIDGE_27:
			index |= 2;
			break;
		default:
			break;
	}

	switch (o1) {
		case OVERLAY_LOWBRIDGE_06:
		case OVERLAY_LOWBRIDGE_07:
		case OVERLAY_LOWBRIDGE_08:
		case OVERLAY_LOWBRIDGE_22:
			index |= 4;
			break;
		case OVERLAY_LOWBRIDGE_09:
		case OVERLAY_LOWBRIDGE_27:
			index |= 8;
			break;
		default:
			break;
	}

	return(index);
}

// FUNCTION: 0x0057B990
int MapClass::Get_Low_Bridge_NS_Neighbor_Mask(Cell const & cell)
{
	OverlayType o1 = Map[cell + Cell(0, -1)].Overlay;
	OverlayType o2 = Map[cell + Cell(0, 1)].Overlay;

	int index = 0;
	switch (o1) {
		case OVERLAY_LOWBRIDGE_14:
		case OVERLAY_LOWBRIDGE_16:
		case OVERLAY_LOWBRIDGE_18:
		case OVERLAY_LOWBRIDGE_24:
			index |= 1;
			break;
		case OVERLAY_LOWBRIDGE_17:
		case OVERLAY_LOWBRIDGE_28:
			index |= 2;
			break;
		default:
			break;
	}

	switch (o2) {
		case OVERLAY_LOWBRIDGE_15:
		case OVERLAY_LOWBRIDGE_16:
		case OVERLAY_LOWBRIDGE_17:
		case OVERLAY_LOWBRIDGE_26:
			index |= 4;
			break;
		case OVERLAY_LOWBRIDGE_18:
		case OVERLAY_LOWBRIDGE_28:
			index |= 8;
			break;
		default:
			break;
	}

	return(index);
}

// FUNCTION: 0x0057BAA0
bool MapClass::Damage_Low_Bridge(Cell const & cell)
{
	OverlayType otype = Map[cell].Overlay;
	if (otype >= OVERLAY_LOWBRIDGE_01 && otype <= OVERLAY_LOWBRIDGE_09 || otype >= OVERLAY_LOWBRIDGE_19 && otype <= OVERLAY_LOWBRIDGE_22 || otype == OVERLAY_LOWBRIDGE_27) {
		if (!Is_Low_Bridge(cell + Cell(0, -1))) {
			return(Damage_Low_Bridge_EW(cell + Cell(0, 1)));
		} else {
			if (!Is_Low_Bridge(cell + Cell(0, -2))) {
				return(Damage_Low_Bridge_EW(cell));
			} else {
				return(Damage_Low_Bridge_EW(cell - Cell(0, 1)));
			}
		}
	} else if (otype >= OVERLAY_LOWBRIDGE_10 && otype <= OVERLAY_LOWBRIDGE_18 || otype >= OVERLAY_LOWBRIDGE_23 && otype <= OVERLAY_LOWBRIDGE_26 || otype == OVERLAY_LOWBRIDGE_28) {
		if (!Is_Low_Bridge(cell + Cell(-1, 0))) {
			return(Damage_Low_Bridge_NS(cell + Cell(1, 0)));
		} else {
			if (!Is_Low_Bridge(cell + Cell(-2, 0))) {
				return(Damage_Low_Bridge_NS(cell));
			} else {
				return(Damage_Low_Bridge_NS(cell - Cell(1, 0)));
			}
		}
	}
	return(false);
}

#define LB2(n) ((OverlayType)(204 + (n)))

// FUNCTION: 0x0057C870
void MapClass::Spring_Low_Bridge_Destroyed_NS(Cell cell)
{
	Cell tmp = cell;
	do {
		tmp = Adjacent_Cell(tmp, FACING_N);
	} while (Is_Low_Bridge(tmp));

	Cell start = Adjacent_Cell(tmp, FACING_S);

	tmp = cell;
	do {
		tmp = Adjacent_Cell(tmp, FACING_S);
	} while (Is_Low_Bridge(tmp));

	Cell end = Adjacent_Cell(tmp, FACING_N);

	Spring_Bridge_Destruction_Triggers(start, end);
}

// FUNCTION: 0x0057C990
void MapClass::Spring_Low_Bridge_Destroyed_EW(Cell cell)
{
	Cell tmp = cell;
	do {
		tmp = Adjacent_Cell(tmp, FACING_W);
	} while (Is_Low_Bridge(tmp));

	Cell start = Adjacent_Cell(tmp, FACING_E);

	tmp = cell;
	do {
		tmp = Adjacent_Cell(tmp, FACING_E);
	} while (Is_Low_Bridge(tmp));

	Cell end = Adjacent_Cell(tmp, FACING_W);

	Spring_Bridge_Destruction_Triggers(start, end);
}

// FUNCTION: 0x0057CAB0
int MapClass::FUN_0057CAB0(Cell const & cell)
{
	OverlayType o1 = Map[cell + Cell(-1, 0)].Overlay;
	OverlayType o2 = Map[cell + Cell(1, 0)].Overlay;

	int index = 0;
	switch (o2) {
		case LB2(5):
		case LB2(7):
		case LB2(9):
		case LB2(20):
			index |= 1;
			break;
		case LB2(8):
		case LB2(27):
			index |= 2;
			break;
		default:
			break;
	}

	switch (o1) {
		case LB2(6):
		case LB2(7):
		case LB2(8):
		case LB2(22):
			index |= 4;
			break;
		case LB2(9):
		case LB2(27):
			index |= 8;
			break;
		default:
			break;
	}

	return(index);
}

// FUNCTION: 0x0057CBE0
int MapClass::FUN_0057CBE0(Cell const & cell)
{
	OverlayType o1 = Map[cell + Cell(0, -1)].Overlay;
	OverlayType o2 = Map[cell + Cell(0, 1)].Overlay;

	int index = 0;
	switch (o1) {
		case LB2(14):
		case LB2(16):
		case LB2(18):
		case LB2(24):
			index |= 1;
			break;
		case LB2(17):
		case LB2(28):
			index |= 2;
			break;
		default:
			break;
	}

	switch (o2) {
		case LB2(15):
		case LB2(16):
		case LB2(17):
		case LB2(26):
			index |= 4;
			break;
		case LB2(18):
		case LB2(28):
			index |= 8;
			break;
		default:
			break;
	}

	return(index);
}

// FUNCTION: 0x0057CCF0
bool MapClass::FUN_0057CCF0(Cell const & cell)
{
	OverlayType otype = Map[cell].Overlay;
	if (otype >= LB2(1) && otype <= LB2(9) || otype >= LB2(19) && otype <= LB2(22) || otype == LB2(27)) {
		if (!Is_Low_Bridge2(cell + Cell(0, -1))) {
			return(FUN_0057CF60(cell + Cell(0, 1)));
		} else {
			if (!Is_Low_Bridge2(cell + Cell(0, -2))) {
				return(FUN_0057CF60(cell));
			} else {
				return(FUN_0057CF60(cell - Cell(0, 1)));
			}
		}
	} else if (otype >= LB2(10) && otype <= LB2(18) || otype >= LB2(23) && otype <= LB2(26) || otype == LB2(28)) {
		if (!Is_Low_Bridge2(cell + Cell(-1, 0))) {
			return(FUN_0057D530(cell + Cell(1, 0)));
		} else {
			if (!Is_Low_Bridge2(cell + Cell(-2, 0))) {
				return(FUN_0057D530(cell));
			} else {
				return(FUN_0057D530(cell - Cell(1, 0)));
			}
		}
	}
	return(false);
}

// FUNCTION: 0x0057DAF0
void MapClass::FUN_0057DAF0(Cell cell)
{
	Cell tmp = cell;
	do {
		tmp = Adjacent_Cell(tmp, FACING_N);
	} while (Is_Low_Bridge2(tmp));

	Cell start = Adjacent_Cell(tmp, FACING_S);

	tmp = cell;
	do {
		tmp = Adjacent_Cell(tmp, FACING_S);
	} while (Is_Low_Bridge2(tmp));

	Cell end = Adjacent_Cell(tmp, FACING_N);

	Spring_Bridge_Destruction_Triggers(start, end);
}

// FUNCTION: 0x0057DC20
void MapClass::FUN_0057DC20(Cell cell)
{
	Cell tmp = cell;
	do {
		tmp = Adjacent_Cell(tmp, FACING_W);
	} while (Is_Low_Bridge2(tmp));

	Cell start = Adjacent_Cell(tmp, FACING_E);

	tmp = cell;
	do {
		tmp = Adjacent_Cell(tmp, FACING_E);
	} while (Is_Low_Bridge2(tmp));

	Cell end = Adjacent_Cell(tmp, FACING_W);

	Spring_Bridge_Destruction_Triggers(start, end);
}

// FUNCTION: 0x0057DD50
void MapClass::Damage_Low_Bridge_Piece_EW(Cell const & cell)
{
	if (!Is_Low_Bridge(cell)) {
		return;
	}

	CellClass * cellptr1 = &Map[cell];
	CellClass * cellptr2 = &Map[cell - Cell(0, 1)];
	CellClass * cellptr3 = &Map[cell + Cell(0, 1)];

	bool kill = false;
	Rect draw_rect = Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect());

	OverlayType lookup[16] = {
		OVERLAY_NONE,
		OVERLAY_LOWBRIDGE_06,
		OVERLAY_LOWBRIDGE_09,
		OVERLAY_NONE,
		OVERLAY_LOWBRIDGE_05,
		OVERLAY_LOWBRIDGE_07,
		OVERLAY_LOWBRIDGE_09,
		OVERLAY_NONE,
		OVERLAY_LOWBRIDGE_08,
		OVERLAY_LOWBRIDGE_08,
		OVERLAY_LOWBRIDGE_27,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE
	};

	int mask = Get_Low_Bridge_EW_Neighbor_Mask(cell);
	OverlayType current = Map[cell].Overlay;

	if (mask > 0) {
		OverlayType newoverlay;

		if (current >= OVERLAY_LOWBRIDGE_19) {
			if (current == OVERLAY_LOWBRIDGE_19) {
				newoverlay = OVERLAY_LOWBRIDGE_20;
			} else if (current == OVERLAY_LOWBRIDGE_21) {
				newoverlay = OVERLAY_LOWBRIDGE_22;
			} else {
				return;
			}
		} else {
			newoverlay = lookup[mask];
			if (current == newoverlay) {
				return;
			}
		}

		cellptr3->Overlay = cellptr2->Overlay = cellptr1->Overlay = newoverlay;

		draw_rect = Union(draw_rect, Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
		TacticalMap->Register_Dirty_Area(draw_rect, false);

		if (newoverlay == OVERLAY_LOWBRIDGE_27) {
			kill = true;
			Map.Radar_Background(cell);
			Map.Radar_Background(cell + Cell(0, 1));
			Map.Radar_Background(cell - Cell(0, 1));
		}

		cellptr1->Recalc_Attributes(-1);
		cellptr2->Recalc_Attributes(-1);
		cellptr3->Recalc_Attributes(-1);
		cellptr1->Kill_Illegal_Occupiers(kill);
		cellptr2->Kill_Illegal_Occupiers(kill);
		cellptr3->Kill_Illegal_Occupiers(kill);
	}
}

// FUNCTION: 0x0057E2A0
void MapClass::Damage_Low_Bridge_Piece_NS(Cell const & cell)
{
	if (!Is_Low_Bridge(cell)) {
		return;
	}

	CellClass * cellptr1 = &Map[cell];
	CellClass * cellptr2 = &Map[cell - Cell(1, 0)];
	CellClass * cellptr3 = &Map[cell + Cell(1, 0)];

	bool kill = false;
	Rect draw_rect = Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect());

	OverlayType lookup[16] = {
		OVERLAY_NONE,
		OVERLAY_LOWBRIDGE_15,
		OVERLAY_LOWBRIDGE_18,
		OVERLAY_NONE,
		OVERLAY_LOWBRIDGE_14,
		OVERLAY_LOWBRIDGE_16,
		OVERLAY_LOWBRIDGE_18,
		OVERLAY_NONE,
		OVERLAY_LOWBRIDGE_17,
		OVERLAY_LOWBRIDGE_17,
		OVERLAY_LOWBRIDGE_28,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE
	};

	int mask = Get_Low_Bridge_NS_Neighbor_Mask(cell);
	OverlayType current = cellptr1->Overlay;

	if (mask > 0) {
		OverlayType newoverlay;

		if (current >= OVERLAY_LOWBRIDGE_23) {
			if (current == OVERLAY_LOWBRIDGE_23) {
				newoverlay = OVERLAY_LOWBRIDGE_24;
			} else if (current == OVERLAY_LOWBRIDGE_25) {
				newoverlay = OVERLAY_LOWBRIDGE_26;
			} else {
				return;
			}
		} else {
			newoverlay = lookup[mask];
			if (newoverlay == current) {
				return;
			}
		}

		cellptr3->Overlay = cellptr2->Overlay = cellptr1->Overlay = newoverlay;

		draw_rect = Union(draw_rect, Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
		TacticalMap->Register_Dirty_Area(draw_rect, false);

		if (newoverlay == OVERLAY_LOWBRIDGE_28) {
			kill = true;
			Map.Radar_Background(cell);
			Map.Radar_Background(cell + Cell(1, 0));
			Map.Radar_Background(cell - Cell(1, 0));
		}

		cellptr1->Recalc_Attributes(-1);
		cellptr2->Recalc_Attributes(-1);
		cellptr3->Recalc_Attributes(-1);
		cellptr1->Kill_Illegal_Occupiers(kill);
		cellptr2->Kill_Illegal_Occupiers(kill);
		cellptr3->Kill_Illegal_Occupiers(kill);
	}
}

// FUNCTION: 0x0057E7A0
void MapClass::FUN_0057E7A0(Cell const & cell)
{
	if (!Is_Low_Bridge2(cell)) {
		return;
	}

	CellClass * cellptr1 = &Map[cell];
	CellClass * cellptr2 = &Map[cell - Cell(0, 1)];
	CellClass * cellptr3 = &Map[cell + Cell(0, 1)];

	bool kill = false;
	Rect draw_rect = Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect());

	OverlayType lookup[16] = {
		OVERLAY_NONE,
		LB2(6),
		LB2(9),
		OVERLAY_NONE,
		LB2(5),
		LB2(7),
		LB2(9),
		OVERLAY_NONE,
		LB2(8),
		LB2(8),
		LB2(27),
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE
	};

	int mask = FUN_0057CAB0(cell);
	OverlayType current = Map[cell].Overlay;

	if (mask > 0) {
		OverlayType newoverlay;

		if (current >= LB2(19)) {
			if (current == LB2(19)) {
				newoverlay = LB2(20);
			} else if (current == LB2(21)) {
				newoverlay = LB2(22);
			} else {
				return;
			}
		} else {
			newoverlay = lookup[mask];
			if (current == newoverlay) {
				return;
			}
		}

		cellptr3->Overlay = cellptr2->Overlay = cellptr1->Overlay = newoverlay;

		draw_rect = Union(draw_rect, Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
		TacticalMap->Register_Dirty_Area(draw_rect, false);

		if (newoverlay == LB2(27)) {
			kill = true;
			Map.Radar_Background(cell);
			Map.Radar_Background(cell + Cell(0, 1));
			Map.Radar_Background(cell - Cell(0, 1));
		}

		cellptr1->Recalc_Attributes(-1);
		cellptr2->Recalc_Attributes(-1);
		cellptr3->Recalc_Attributes(-1);
		cellptr1->Kill_Illegal_Occupiers(kill);
		cellptr2->Kill_Illegal_Occupiers(kill);
		cellptr3->Kill_Illegal_Occupiers(kill);
	}
}

// FUNCTION: 0x0057ED00
void MapClass::FUN_0057ED00(Cell const & cell)
{
	if (!Is_Low_Bridge2(cell)) {
		return;
	}

	CellClass * cellptr1 = &Map[cell];
	CellClass * cellptr2 = &Map[cell - Cell(1, 0)];
	CellClass * cellptr3 = &Map[cell + Cell(1, 0)];

	bool kill = false;
	Rect draw_rect = Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect());

	OverlayType lookup[16] = {
		OVERLAY_NONE,
		LB2(15),
		LB2(18),
		OVERLAY_NONE,
		LB2(14),
		LB2(16),
		LB2(18),
		OVERLAY_NONE,
		LB2(17),
		LB2(17),
		LB2(28),
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE,
		OVERLAY_NONE
	};

	int mask = FUN_0057CBE0(cell);
	OverlayType current = cellptr1->Overlay;

	if (mask > 0) {
		OverlayType newoverlay;

		if (current >= LB2(23)) {
			if (current == LB2(23)) {
				newoverlay = LB2(24);
			} else if (current == LB2(25)) {
				newoverlay = LB2(26);
			} else {
				return;
			}
		} else {
			newoverlay = lookup[mask];
			if (newoverlay == current) {
				return;
			}
		}

		cellptr3->Overlay = cellptr2->Overlay = cellptr1->Overlay = newoverlay;

		draw_rect = Union(draw_rect, Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
		TacticalMap->Register_Dirty_Area(draw_rect, false);

		if (newoverlay == LB2(28)) {
			kill = true;
			Map.Radar_Background(cell);
			Map.Radar_Background(cell + Cell(1, 0));
			Map.Radar_Background(cell - Cell(1, 0));
		}

		cellptr1->Recalc_Attributes(-1);
		cellptr2->Recalc_Attributes(-1);
		cellptr3->Recalc_Attributes(-1);
		cellptr1->Kill_Illegal_Occupiers(kill);
		cellptr2->Kill_Illegal_Occupiers(kill);
		cellptr3->Kill_Illegal_Occupiers(kill);
	}
}

// FUNCTION: 0x0057F200
void MapClass::Repair_Low_Bridge_Span(Cell const & cell)
{
	OverlayType overlay = Map[cell].Overlay;

	if (overlay >= OVERLAY_LOWBRIDGE_01 && overlay <= OVERLAY_LOWBRIDGE_09 || overlay >= OVERLAY_LOWBRIDGE_19 && overlay <= OVERLAY_LOWBRIDGE_22 || overlay == OVERLAY_LOWBRIDGE_27) {
		if (!Is_Low_Bridge(cell - Cell(0, 1))) {
			Repair_Low_Bridge_EW(cell + Cell(0, 1));
		} else {
			if (!Is_Low_Bridge(cell - Cell(0, 2))) {
				Repair_Low_Bridge_EW(cell);
			} else {
				Repair_Low_Bridge_EW(cell - Cell(0, 1));
			}
		}
	} else if (overlay >= OVERLAY_LOWBRIDGE_10 && overlay <= OVERLAY_LOWBRIDGE_18 || overlay >= OVERLAY_LOWBRIDGE_23 && overlay <= OVERLAY_LOWBRIDGE_26 || overlay == OVERLAY_LOWBRIDGE_28) {
		if (!Is_Low_Bridge(cell - Cell(1, 0))) {
			Repair_Low_Bridge_NS(cell + Cell(1, 0));
		} else {
			if (!Is_Low_Bridge(cell - Cell(2, 0))) {
				Repair_Low_Bridge_NS(cell);
			} else {
				Repair_Low_Bridge_NS(cell - Cell(1, 0));
			}
		}
	}
}

// FUNCTION: 0x0057F440
void MapClass::FUN_0057F440(Cell const & cell)
{
	OverlayType overlay = Map[cell].Overlay;

	if (overlay >= LB2(1) && overlay <= LB2(9) || overlay >= LB2(19) && overlay <= LB2(22) || overlay == LB2(27)) {
		if (!Is_Low_Bridge2(cell - Cell(0, 1))) {
			FUN_005800D0(cell + Cell(0, 1));
		} else {
			if (!Is_Low_Bridge2(cell - Cell(0, 2))) {
				FUN_005800D0(cell);
			} else {
				FUN_005800D0(cell - Cell(0, 1));
			}
		}
	} else if (overlay >= LB2(10) && overlay <= LB2(18) || overlay >= LB2(23) && overlay <= LB2(26) || overlay == LB2(28)) {
		if (!Is_Low_Bridge2(cell - Cell(1, 0))) {
			FUN_00580600(cell + Cell(1, 0));
		} else {
			if (!Is_Low_Bridge2(cell - Cell(2, 0))) {
				FUN_00580600(cell);
			} else {
				FUN_00580600(cell - Cell(1, 0));
			}
		}
	}
}

// FUNCTION: 0x0057F6A0
void MapClass::Repair_Low_Bridge_EW(Cell const & cell)
{
	Cell cellnum = cell;
	bool changed = false;

	do {
		cellnum.X--;
	} while (Is_Low_Bridge(cellnum));
	cellnum.X++;

	Rect recalc_rect(0, 0, 0, 0);

	do {
		CellClass * cellptr1 = &Map[cellnum];
		CellClass * cellptr2 = &Map[cellnum - Cell(0, 1)];
		CellClass * cellptr3 = &Map[cellnum + Cell(0, 1)];

		Rect draw_rect = Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect());

		OverlayType otype = cellptr1->Overlay;
		OverlayType newtype = otype;

		switch (otype) {
			case OVERLAY_LOWBRIDGE_19:
			case OVERLAY_LOWBRIDGE_20:
				newtype = OVERLAY_LOWBRIDGE_19;
				break;

			case OVERLAY_LOWBRIDGE_21:
			case OVERLAY_LOWBRIDGE_22:
				newtype = OVERLAY_LOWBRIDGE_21;
				break;

			case OVERLAY_LOWBRIDGE_05:
			case OVERLAY_LOWBRIDGE_06:
			case OVERLAY_LOWBRIDGE_07:
			case OVERLAY_LOWBRIDGE_08:
			case OVERLAY_LOWBRIDGE_09:
			case OVERLAY_LOWBRIDGE_27:
				newtype = (OverlayType)(Pick_Random_UInt(0, 3) + OVERLAY_LOWBRIDGE_01);
				{ Rect r(cellnum.X, cellnum.Y - 1, 1, 3);
				recalc_rect = Union(recalc_rect, r); }
				changed = true;
				break;

			default:
				break;
		}

		if (newtype != otype) {
			cellptr1->Overlay = newtype;
			cellptr2->Overlay = newtype;
			cellptr3->Overlay = newtype;

			draw_rect = Union(draw_rect, Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
			TacticalMap->Register_Dirty_Area(draw_rect, false);

			bool tmp0 = otype == OVERLAY_LOWBRIDGE_27;
			if (tmp0) {
				Map.Radar_Background(cellnum);
				Map.Radar_Background(cellnum + Cell(0, 1));
				Map.Radar_Background(cellnum - Cell(0, 1));
			}

			cellptr1->Recalc_Attributes(-1);
			cellptr2->Recalc_Attributes(-1);
			cellptr3->Recalc_Attributes(-1);
			cellptr1->Kill_Illegal_Occupiers(false);
			cellptr2->Kill_Illegal_Occupiers(false);
			cellptr3->Kill_Illegal_Occupiers(false);
		}

		cellnum.X++;
	} while (Is_Low_Bridge(cellnum));

	if (changed) {
		Update_Pathfinding_1();
	}

	if (recalc_rect.Is_Valid() != 0) {
		Recalc_Cells_In_Rect(recalc_rect);
	}
}

// FUNCTION: 0x0057FBC0
void MapClass::Repair_Low_Bridge_NS(Cell const & cell)
{
	Cell cellnum = cell;
	Rect recalc_rect(0, 0, 0, 0);
	bool changed = false;

	do {
		cellnum.Y--;
	} while (Is_Low_Bridge(cellnum));
	cellnum.Y++;

	do {
		CellClass * cellptr1 = &Map[cellnum];
		CellClass * cellptr2 = &Map[cellnum - Cell(1, 0)];
		CellClass * cellptr3;
		cellptr3 = &Map[cellnum + Cell(1, 0)];

		Rect draw_rect = Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect());

		OverlayType otype = cellptr1->Overlay;
		OverlayType newtype = otype;

		switch (otype) {
			case OVERLAY_LOWBRIDGE_23:
			case OVERLAY_LOWBRIDGE_24:
				newtype = OVERLAY_LOWBRIDGE_23;
				break;

			case OVERLAY_LOWBRIDGE_25:
			case OVERLAY_LOWBRIDGE_26:
				newtype = OVERLAY_LOWBRIDGE_25;
				break;

			case OVERLAY_LOWBRIDGE_14:
			case OVERLAY_LOWBRIDGE_15:
			case OVERLAY_LOWBRIDGE_16:
			case OVERLAY_LOWBRIDGE_17:
			case OVERLAY_LOWBRIDGE_18:
			case OVERLAY_LOWBRIDGE_28:
				newtype = (OverlayType)(Pick_Random_UInt(0, 3) + OVERLAY_LOWBRIDGE_10);
				{ Rect r(cellnum.X - 1, cellnum.Y, 3, 1);
				recalc_rect = Union(recalc_rect, r); }
				changed = true;
				break;

			default:
				break;
		}

		if (newtype != otype) {
			cellptr1->Overlay = newtype;
			cellptr2->Overlay = newtype;
			cellptr3->Overlay = newtype;

			draw_rect = Union(draw_rect, Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
			TacticalMap->Register_Dirty_Area(draw_rect, false);

			if (otype != OVERLAY_LOWBRIDGE_28) {
			} else {
				Map.Radar_Background(cellnum);
				Map.Radar_Background(cellnum + Cell(1, 0));
				Map.Radar_Background(cellnum - Cell(1, 0));
			}

			cellptr1->Recalc_Attributes(-1);
			cellptr2->Recalc_Attributes(-1);
			cellptr3->Recalc_Attributes(-1);
			cellptr1->Kill_Illegal_Occupiers(false);
			cellptr2->Kill_Illegal_Occupiers(false);
			cellptr3->Kill_Illegal_Occupiers(false);
		}

		cellnum.Y += 1;
	} while (Is_Low_Bridge(cellnum));

	if (changed) {
		Update_Pathfinding_1();
	}

	if (!recalc_rect.Is_Valid()) goto skip0;
	Recalc_Cells_In_Rect(recalc_rect);
skip0:;
}

// FUNCTION: 0x005800D0
void MapClass::FUN_005800D0(Cell const & cell)
{
	Cell cellnum = cell;
	bool changed = false;

	do {
		cellnum.X--;
	} while (Is_Low_Bridge2(cellnum));
	cellnum.X++;

	Rect recalc_rect(0, 0, 0, 0);

	do {
		CellClass * cellptr1 = &Map[cellnum];
		CellClass * cellptr2 = &Map[cellnum - Cell(0, 1)];
		CellClass * cellptr3 = &Map[cellnum + Cell(0, 1)];

		Rect draw_rect = Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect());

		OverlayType otype = cellptr1->Overlay;
		OverlayType newtype = otype;

		switch (otype) {
			case LB2(19):
			case LB2(20):
				newtype = LB2(19);
				break;

			case LB2(21):
			case LB2(22):
				newtype = LB2(21);
				break;

			case LB2(5):
			case LB2(6):
			case LB2(7):
			case LB2(8):
			case LB2(9):
			case LB2(27):
				newtype = (OverlayType)(Pick_Random_UInt(0, 3) + LB2(1));
				{ Rect r(cellnum.X, cellnum.Y - 1, 1, 3);
				recalc_rect = Union(recalc_rect, r); }
				changed = true;
				break;

			default:
				break;
		}

		if (newtype != otype) {
			cellptr1->Overlay = newtype;
			cellptr2->Overlay = newtype;
			cellptr3->Overlay = newtype;

			draw_rect = Union(draw_rect, Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
			TacticalMap->Register_Dirty_Area(draw_rect, false);

			bool tmp0 = otype == LB2(27);
			if (tmp0) {
				Map.Radar_Background(cellnum);
				Map.Radar_Background(cellnum + Cell(0, 1));
				Map.Radar_Background(cellnum - Cell(0, 1));
			}

			cellptr1->Recalc_Attributes(-1);
			cellptr2->Recalc_Attributes(-1);
			cellptr3->Recalc_Attributes(-1);
			cellptr1->Kill_Illegal_Occupiers(false);
			cellptr2->Kill_Illegal_Occupiers(false);
			cellptr3->Kill_Illegal_Occupiers(false);
		}

		cellnum.X++;
	} while (Is_Low_Bridge2(cellnum));

	if (changed) {
		Update_Pathfinding_1();
	}

	if (recalc_rect.Is_Valid() != 0) {
		Recalc_Cells_In_Rect(recalc_rect);
	}
}

// FUNCTION: 0x00580600
void MapClass::FUN_00580600(Cell const & cell)
{
	Cell cellnum = cell;
	Rect recalc_rect(0, 0, 0, 0);
	bool changed = false;

	do {
		cellnum.Y--;
	} while (Is_Low_Bridge2(cellnum));
	cellnum.Y++;

	do {
		CellClass * cellptr1 = &Map[cellnum];
		CellClass * cellptr2 = &Map[cellnum - Cell(1, 0)];
		CellClass * cellptr3;
		cellptr3 = &Map[cellnum + Cell(1, 0)];

		Rect draw_rect = Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect());

		OverlayType otype = cellptr1->Overlay;
		OverlayType newtype = otype;

		switch (otype) {
			case LB2(23):
			case LB2(24):
				newtype = LB2(23);
				break;

			case LB2(25):
			case LB2(26):
				newtype = LB2(25);
				break;

			case LB2(14):
			case LB2(15):
			case LB2(16):
			case LB2(17):
			case LB2(18):
			case LB2(28):
				newtype = (OverlayType)(Pick_Random_UInt(0, 3) + LB2(10));
				{ Rect r(cellnum.X - 1, cellnum.Y, 3, 1);
				recalc_rect = Union(recalc_rect, r); }
				changed = true;
				break;

			default:
				break;
		}

		if (newtype != otype) {
			cellptr1->Overlay = newtype;
			cellptr2->Overlay = newtype;
			cellptr3->Overlay = newtype;

			draw_rect = Union(draw_rect, Union(cellptr1->Overlay_Render_Rect(), cellptr1->Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
			TacticalMap->Register_Dirty_Area(draw_rect, false);

			if (otype != LB2(28)) {
			} else {
				Map.Radar_Background(cellnum);
				Map.Radar_Background(cellnum + Cell(1, 0));
				Map.Radar_Background(cellnum - Cell(1, 0));
			}

			cellptr1->Recalc_Attributes(-1);
			cellptr2->Recalc_Attributes(-1);
			cellptr3->Recalc_Attributes(-1);
			cellptr1->Kill_Illegal_Occupiers(false);
			cellptr2->Kill_Illegal_Occupiers(false);
			cellptr3->Kill_Illegal_Occupiers(false);
		}

		cellnum.Y += 1;
	} while (Is_Low_Bridge2(cellnum));

	if (changed) {
		Update_Pathfinding_1();
	}

	if (!recalc_rect.Is_Valid()) goto skip0;
	Recalc_Cells_In_Rect(recalc_rect);
skip0:;
}

// FUNCTION: 0x00580B20
bool MapClass::Is_Low_Bridge(Cell const & cell)
{
	OverlayType overlay = Map[cell].Overlay;
	return(overlay >= 0x4A && overlay <= 0x65);
}

// FUNCTION: 0x00580B70
bool MapClass::Is_Low_Bridge2(Cell const & cell)
{
	OverlayType overlay = Map[cell].Overlay;
	return(overlay >= LB2(1) && overlay <= LB2(28));
}

// FUNCTION: 0x00580BC0
CellClass * MapClass::Find_Nearby_Shroud(FootClass * foot)
{
	Cell cells[24];

	Cell center = foot->Center_Coord().As_Cell();
	int found = 0;

	int xx = center.X;
	int yy = center.Y;

	int radius = 32;
	if (PlayRect.Width + PlayRect.Height <= 32) {
		radius = PlayRect.Width + PlayRect.Height;
	}

	for (int ring = 0; ring < radius; ring++) {
		for (int x = -ring; x <= ring; x++) {
			Cell cell = Cell(xx + x, yy - ring);
			if (In_Local_Radar(cell, true)) {
				Coord coord = cell.As_Coord(0);
				coord.Z = Get_Height_GL(coord);
				if (Is_Shrouded(coord)) {
					cells[found++] = cell;
				}
			}
			if (found == 24) {
				break;
			}

			cell = Cell(xx + x, yy + ring);
			if (In_Local_Radar(cell, true)) {
				Coord coord = cell.As_Coord(0);
				coord.Z = Get_Height_GL(coord);
				if (Is_Shrouded(coord)) {
					cells[found++] = cell;
				}
			}
			if (found == 24) {
				break;
			}
		}
		if (found == 24) {
			break;
		}

		for (int y = -ring + 1; y <= ring - 1; y++) {
			Cell cell = Cell(xx - ring, yy + y);
			if (In_Local_Radar(cell, true)) {
				Coord coord = cell.As_Coord(0);
				coord.Z = Get_Height_GL(coord);
				if (Is_Shrouded(coord)) {
					cells[found++] = cell;
				}
			}
			if (found == 24) {
				break;
			}

			cell = Cell(xx + ring, yy + y);
			if (In_Local_Radar(cell, true)) {
				Coord coord = cell.As_Coord(0);
				coord.Z = Get_Height_GL(coord);
				if (Is_Shrouded(coord)) {
					cells[found++] = cell;
				}
			}
			if (found == 24) {
				break;
			}
		}
		if (found == 24) {
			break;
		}
	}

	if (found > 0) {
		return(&Map[cells[Scen->RandomNumber(0, found - 1)]]);
	}

	return(&Map[center]);
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
