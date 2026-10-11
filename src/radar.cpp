// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "radar.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "bsurface.h"
#include "builtype.h"
#include "cell.h"
#include "convert.h"
#include "dbgprint.h"
#include "dialog.h"
#include "draw.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "language\language.h"
#include "lightcon.h"
#include "mixfile.h"
#include "movies.h"
#include "revent.h"
#include "rules.h"
#include "scheme.h"
#include "tactical.h"
#include "voc.h"
#include "vox.h"
#include "vqa.h"

// FUNCTION: 0x00653760
Cell RadarClass::Click_Cell_Calc(Point2D const & point) const
{
	Point2D pixel = point;
	Coord coord = TacticalMap->Pixel_To_Coord(pixel);

	if (coord != COORD_NONE) {
		if (Debug_Map) {
			return(TacticalMap->Pixel_To_Cell(pixel));
		} else {
			return(TacticalMap->Pixel_To_Cell(pixel));
		}
	}

	return(CELL_NONE);
}

// FUNCTION: 0x00653810
bool RadarClass::Map_Cell(Cell const & cell, HouseClass * house)
{
	if (BASECLASS::Map_Cell(cell, house)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00653F70
void RadarClass::Set_Tactical_Position(Coord const & coord)
{
	TacticalMap->Set_Tactical_Position(coord);
}

// FUNCTION: 0x00653F90
bool RadarClass::Cell_On_Radar(Cell const & cell)
{
	return(In_Local_Radar(cell));
}

// FUNCTION: 0x00655890
int RadarTrackingStruct::Hash_Old(RadarTrackingStruct const & s)
{
	return(((unsigned int)s.Object) + 251 * s.Position.X);
}

// FUNCTION: 0x006558B0
int RadarTrackingStruct::Hash2(RadarTrackingStruct const & s)
{
	return(s.Position.X + 251 * s.Position.Y);
}
