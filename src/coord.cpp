// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "coord.h"
#include "_map.h"
#include "inline.h"
#include "mouse.h"

// SYNTHETIC: GAMEMD 0x0049F0E0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0049F110 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0049F130 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0049F150 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0049F170 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0049F190 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0049F1B0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0049F1F0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0049F220 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0049F250 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0049F280 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0049F290 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0049F2B0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0049F2D0 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x0049F2F0 SYMBOL
// _$E92
Cell const AdjacentCell[FACING_COUNT] = {
	Cell(0,  -1),
	Cell(1,  -1),
	Cell(1,   0),
	Cell(1,   1),
	Cell(0,   1),
	Cell(-1,  1),
	Cell(-1,  0),
	Cell(-1, -1),
};

// SYNTHETIC: GAMEMD 0x0049F3A0 SYMBOL
// _$E95
Point2D const AdjacentPoint[FACING_COUNT] = {
	Point2D(0,              -CELL_LEPTON_H),
	Point2D(CELL_LEPTON_W,  -CELL_LEPTON_H),
	Point2D(CELL_LEPTON_W,  0             ),
	Point2D(CELL_LEPTON_W,  CELL_LEPTON_H ),
	Point2D(0,              CELL_LEPTON_H ),
	Point2D(-CELL_LEPTON_W, CELL_LEPTON_H ),
	Point2D(-CELL_LEPTON_W, 0             ),
	Point2D(-CELL_LEPTON_W, -CELL_LEPTON_H),
};

static inline Coord _Move_Coord(Coord const & start, DirType const & dir, int distance)
{
	return(Move_Coord(start, dir, distance));
}

// FUNCTION: GAMEMD 0x0049F420
Coord Coord_Scatter(Coord const & coord, int distance, bool lock)
{
	Coord newcoord;

	newcoord = _Move_Coord(coord, Random_Dir(DIR_N, DIR_MAX), distance);

	unsigned int cell_x = newcoord.X / CELL_LEPTON_W;
	unsigned int cell_y = newcoord.Y / CELL_LEPTON_H;
	if (cell_x >= MAP_CELL_W || cell_y >= MAP_CELL_H) newcoord = coord;

	if (lock) {
		newcoord = newcoord - Point2D(newcoord.X & (CELL_LEPTON_W-1), newcoord.Y & (CELL_LEPTON_H-1)) + Point2D(CELL_LEPTON_W/2, CELL_LEPTON_H/2);
	}

	return(newcoord);
}

// FUNCTION: GAMEMD 0x0049F550
Coord Adjacent_Coord_With_Height(Coord const & coord, FacingType dir)
{
	Coord newcoord = (coord + AdjacentPoint[(int)dir]);
	newcoord.Z += Map.Get_Height_GL(newcoord) - Map.Get_Height_GL(coord);
	return(newcoord);
}
