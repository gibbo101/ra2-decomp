// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "infantry.h"
#include "_bench.h"
#include "_convert.h"
#include "_mixfile.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "anim.h"
#include "astar.h"
#include "bench.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "data.h"
#include "draw.h"
#include "fly.h"
#include "fog.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "ilocos.h"
#include "incdec.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "isotype.h"
#include "language\language.h"
#include "lightcon.h"
#include "mixfile.h"
#include "mono.h"
#include "overlay.h"
#include "overtype.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "team.h"
#include "tiberium.h"
#include "tracker.h"
#include "tube.h"
#include "tube.hh"
#include "unit.h"
#include "vox.h"
#include "warhead.h"
#include "weapon.h"
#include "bench.hh"

// FUNCTION: 0x00521B00
HRESULT STDMETHODCALLTYPE InfantryClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x00523300
HRESULT STDMETHODCALLTYPE InfantryClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_InfantryClass;
	return(S_OK);
}
