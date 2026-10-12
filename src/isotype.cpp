// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "isotype.h"
#include "_alpha.h"
#include "_convert.h"
#include "_map.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_rules.h"
#include "_surface.h"
#include "_theater.h"
#include "_zbuffer.h"
#include "abuffer.h"
#include "alphalighting.h"
#include "animtype.h"
#include "ccfile.h"
#include "ccini.h"
#include "cell.h"
#include "conquer.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsurface.h"
#include "globals.h"
#include "isotile.h"
#include "lightcon.h"
#include "mixfile.h"
#include "palette.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"
#include "zbuffer.h"

// FUNCTION: 0x00543C60 $E2
// FUNCTION: 0x00543C90 $E5
// FUNCTION: 0x00543CB0 $E8
// FUNCTION: 0x00543CD0 $E11
// FUNCTION: 0x00543CF0 $E14
// FUNCTION: 0x00543D10 $E17
// FUNCTION: 0x00543D30 $E20
// FUNCTION: 0x00543D70 $E23
// FUNCTION: 0x00543DA0 $E26
// FUNCTION: 0x00543DD0 $E29
// FUNCTION: 0x00543E00 $E32
// FUNCTION: 0x00543E10 $E35

// FUNCTION: 0x00544E30
IsometricTileType IsometricTileTypeClass::Fixup_Tile_Type(IsometricTileType current_type)
{
	IsometricTileType new_type = current_type;
	if (current_type == ISOTILE_NONE) {
		return(new_type);
	}

	for (int i = 0; i < TileInsertTypes.Count(); i++) {
		TileInsertType *tileinsert = TileInsertTypes[i];
		if (tileinsert->Count > current_type) {
			break;
		}
		new_type += tileinsert->Offset;
	}
	return(new_type);
}

// FUNCTION: 0x00549B30
void IsometricTileTypeClass::Clear_Z_Shapes(void)
{
	CellShadowShapes = NULL;
	SlopeZShapes[0] = NULL;
	SlopeZShapes[1] = NULL;
	SlopeZShapes[2] = NULL;
	SlopeZShapes[3] = NULL;
}

// FUNCTION: 0x00549B50
Coord const IsometricTileTypeClass::Coord_Fixup(Coord const & coord) const
{
	return(coord);
}

// FUNCTION: 0x00549D70
HRESULT STDMETHODCALLTYPE IsometricTileTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00549D90
HRESULT STDMETHODCALLTYPE IsometricTileTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_IsometricTileTypeClass;
	return(S_OK);
}
