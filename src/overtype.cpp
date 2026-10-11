// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "overtype.h"
#include "_convert.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_theater.h"
#include "animtype.h"
#include "ccfile.h"
#include "ccini.h"
#include "data.h"
#include "dbgprint.h"
#include "draw.h"
#include "findmake.h"
#include "init.h"
#include "mixfile.h"
#include "overlay.h"
#include "resource.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"

// FUNCTION: 0x005FE4C0
Cell const * OverlayTypeClass::Occupy_List(bool) const
{
	static Cell const _simple[] = {Cell(0, 0), REFRESH_EOL};

	return(_simple);
}

// FUNCTION: 0x005FEA30
Coord const OverlayTypeClass::Coord_Fixup(Coord const & coord) const
{
	return(coord);
}

// FUNCTION: 0x005FEC10
HRESULT STDMETHODCALLTYPE OverlayTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x005FEC30
HRESULT STDMETHODCALLTYPE OverlayTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_OverlayTypeClass;
	return(S_OK);
}
