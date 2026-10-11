// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "unittype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_rules.h"
#include "bsurface.h"
#include "convert.h"
#include "face.h"
#include "findmake.h"
#include "globals.h"
#include "inline.h"
#include "mixfile.h"
#include "mouse.h"
#include "rules.h"
#include "sun.h"
#include "tracker.h"
#include "unit.h"
#include "color.hh"

// FUNCTION: 0x00747600
TPoint2D<int> UnitTypeClass::Turret_Adjust(Dir256 dir, TPoint2D<int> const & xy) const
{
	return(xy);
}

// FUNCTION: 0x00747EB0
Coord const UnitTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord fix = coord;
	if (fix.Z < Map.Get_Height_GL(fix)) {
		fix.Z = Map.Get_Height_GL(fix);
	}
	return(fix);
}

// FUNCTION: 0x00747F30
HRESULT STDMETHODCALLTYPE UnitTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_UnitTypeClass;
	return(S_OK);
}

// FUNCTION: 0x007480B0
HRESULT STDMETHODCALLTYPE UnitTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
