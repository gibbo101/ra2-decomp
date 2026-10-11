// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "terrain.h"
#include "_convert.h"
#include "_map.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "anim.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "draw.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "inline.h"
#include "lightcon.h"
#include "rect.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "terrtype.h"
#include "tracker.h"
#include "warhead.h"
#include "draw.hh"
#include <cstdio>

// FUNCTION: 0x0071CF30
HRESULT STDMETHODCALLTYPE TerrainClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0071D310
HRESULT STDMETHODCALLTYPE TerrainClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TerrainClass;
	return(S_OK);
}
