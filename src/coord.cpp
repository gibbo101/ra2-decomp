// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "coord.h"
#include "_map.h"
#include "inline.h"
#include "mouse.h"

// FUNCTION: 0x0049F550
Coord Adjacent_Coord_With_Height(Coord const & coord, FacingType dir)
{
	Coord newcoord = (coord + AdjacentPoint[(int)dir]);
	newcoord.Z += Map.Get_Height_GL(newcoord) - Map.Get_Height_GL(coord);
	return(newcoord);
}
