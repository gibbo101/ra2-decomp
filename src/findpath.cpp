// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "foot.h"
#include "_astar.h"
#include "_map.h"
#include "astar.h"
#include "bench.h"
#include "inline.h"

#include <math.h>


// SYNTHETIC: GAMEMD 0x004CB990 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x004CB9B0 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x004CB9E0 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x004CBA00 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x004CBA20 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x004CBA40 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x004CBA60 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x004CBA80 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x004CBAC0 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x004CBAF0 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x004CBB20 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x004CBB50 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x004CBB60 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x004CBB80 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x004CBBA0
PathStruct * FootClass::Find_Path(Cell const & dest, FacingType * final_moves, int maxlen, MoveType threshhold, int path_offset, int avoidance)
{
	PathStruct * path;
	Cell source = Destination_Coord();

	if (!final_moves) return(NULL);

	source = Follow_Path(source, path_offset, Path);
	path = Search.Find_Path(source, dest, this, final_moves, -1, MZONE_NONE, (AStarClass::ObstacleAvoidanceType)avoidance);

	return(path);
}
