// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "cell.h"
#include "_alpha.h"
#include "_bench.h"
#include "_convert.h"
#include "_logic.h"
#include "_mixfile.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "abuffer.h"
#include "anim.h"
#include "astar.h"
#include "bench.h"
#include "building.h"
#include "builtype.h"
#include "combat.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsurface.h"
#include "fog.h"
#include "globals.h"
#include "houstype.h"
#include "incdec.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "isotype.h"
#include "light.h"
#include "lightcalc.h"
#include "lightcon.h"
#include "logic.h"
#include "mixfile.h"
#include "overlay.h"
#include "overtype.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "smudtype.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "terrain.h"
#include "tiberium.h"
#include "tracker.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vein.h"
#include "vox.h"
#include "warhead.h"
#include "bench.hh"
#include "ramp.hh"
#include "tube.hh"

// FUNCTION: 0x004810A0
int CellClass::Spot_Index(Coord const & coord)
{
	Point2D rel = Coord_Fraction(coord);

	if (Distance(rel, Point2D(CELL_LEPTON/2,CELL_LEPTON/2)) < 60) {
		return(0);
	}

	int index = 0;
	if (rel.X > CELL_LEPTON/2) index |= 0x01;
	if (rel.Y > CELL_LEPTON/2) index |= 0x02;
	if (index == 0) return(0);
	return(index+1);
}

// FUNCTION: 0x00485200
HRESULT STDMETHODCALLTYPE CellClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_CellClass;
	return(S_OK);
}

// FUNCTION: 0x00487E50
int STDMETHODCALLTYPE CellClass::What_Am_I(void) const
{
	return(RTTI_CELL);
}
