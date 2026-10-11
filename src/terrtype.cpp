// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "terrtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_theater.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "mixfile.h"
#include "rules.h"
#include "shapeset.h"
#include "sun.h"
#include "terrain.h"
#include "terrtype.h"
#include "tracker.h"

// FUNCTION: 0x0071E0D0
Coord const TerrainTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord tmp = coord;
	if (tmp.Z < Map.Get_Height_GL(tmp)) {
		tmp.Z = Map.Get_Height_GL(tmp);
	}
	return(tmp);
}

// FUNCTION: 0x0071E240
HRESULT STDMETHODCALLTYPE TerrainTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0071E260
HRESULT STDMETHODCALLTYPE TerrainTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TerrainTypeClass;
	return(S_OK);
}
