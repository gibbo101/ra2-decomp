// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "infatype.h"
#include "_map.h"
#include "_rules.h"
#include "cell.h"
#include "findmake.h"
#include "globals.h"
#include "infantry.h"
#include "mouse.h"
#include "rules.h"
#include "sun.h"
#include "tracker.h"
#include <cstdio>

// FUNCTION: 0x00523C20
Cell const * InfantryTypeClass::Occupy_List(bool) const
{
	static Cell const _list[] = {Cell(0, 0), REFRESH_EOL};

	return(&_list[0]);
}

// FUNCTION: 0x005247D0
Coord const InfantryTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord fix = coord;
	if (fix.Z < Map.Get_Height_GL(fix)) {
		fix.Z = Map.Get_Height_GL(fix);
	}
	return(fix);
}

// FUNCTION: 0x00524C70
HRESULT STDMETHODCALLTYPE InfantryTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_InfantryTypeClass;
	return(S_OK);
}
