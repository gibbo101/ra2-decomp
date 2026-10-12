// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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
#include "overlay.hh"
#include "ramp.hh"
#include <new>
#include <cmath>

int SubzoneHash(unsigned int const & key);

#define IS_TILE_IN_SET(setname, setsize) (tile >= IsometricTileTypeClass::setname && tile < IsometricTileTypeClass::setname + setsize)

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern double DAT_007E1708, DAT_007E1710, DAT_007E1718, DAT_007E1720, DAT_007E1728;
extern double DAT_007E1730, DAT_007E1738, DAT_007E1740, DAT_007ED3C0, DAT_007ED3C8;
extern double DAT_00ABD488, DAT_00ABDDA0, DAT_00ABDC38, DAT_00ABDC30, DAT_00ABDA58;
extern double DAT_00ABD458, DAT_00ABDA60, DAT_00ABDC20;
extern int DAT_00ABDE88, DAT_00ABDD98, DAT_00ABDC10, DAT_00ABDC14, DAT_00ABDC18;
extern short DAT_00ABD480, DAT_00ABD482;

// FUNCTION: 0x00561710
void Init_00561710(void)
{
	DAT_00ABD488 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);
}

// FUNCTION: 0x00561740
void Init_00561740(void)
{
	DAT_00ABDDA0 = DAT_00ABD488 / DAT_007ED3C0;
}

// FUNCTION: 0x00561760
void Init_00561760(void)
{
	DAT_00ABDC38 = DAT_007E1718 / DAT_00ABDDA0;
}

// FUNCTION: 0x00561780
void Init_00561780(void)
{
	DAT_00ABDC30 = DAT_007E1720 * DAT_007ED3C8;
}

// FUNCTION: 0x005617A0
void Init_005617A0(void)
{
	DAT_00ABDA58 = DAT_007E1728 * DAT_007ED3C8;
}

// FUNCTION: 0x005617C0
void Init_005617C0(void)
{
	DAT_00ABD458 = DAT_007E1730 * DAT_007ED3C8;
}

// FUNCTION: 0x005617E0
void Init_005617E0(void)
{
	DAT_00ABDE88 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABD458 - DAT_00ABDA58) * DAT_00ABD488));
}

// FUNCTION: 0x00561820
void Init_00561820(void)
{
	DAT_00ABDA60 = FUN_004CADE0(DAT_00ABDE88 * DAT_007E1740);
}

// FUNCTION: 0x00561850
void Init_00561850(void)
{
	DAT_00ABDC20 = FUN_004CADE0((DAT_00ABDE88 * 2) / DAT_00ABD488);
}

// FUNCTION: 0x00561880
void Init_00561880(void)
{
	DAT_00ABDD98 = (int)((DAT_00ABDE88 * 4) + DAT_007E1738);
}

// FUNCTION: 0x005618B0
void Init_005618B0(void)
{
	DAT_00ABD480 = 0;
	DAT_00ABD482 = 0;
}

// FUNCTION: 0x005618C0
void Init_005618C0(void)
{
	DAT_00ABDC10 = 0;
	DAT_00ABDC14 = 0;
	DAT_00ABDC18 = 0;
}

// FUNCTION: 0x0056CB80
int SubzoneHash(unsigned int const & key)
{
	return(key & 0xF | ((key >> 12) & 0xF));
}

// FUNCTION: 0x00577A50
Cell MapClass::Pick_Random_Location(void) const
{
	int x = Map.MapRect.X + Random_Pick(0, Map.MapRect.Width-1);
	int y = Map.MapRect.Y + Random_Pick(0, Map.MapRect.Height-1);
	return(Cell(x, y));
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
