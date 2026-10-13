// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "fog.h"
#include "_convert.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "convert.h"
#include "coord.h"
#include "crc.h"
#include "draw.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "inline.h"
#include "lightcon.h"
#include "objtype.h"
#include "rect.h"
#include "scheme.h"
#include "shapeset.h"
#include "smudtype.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "terrain.h"
#include "vector.h"

// FUNCTION: 0x004D0560 $E2
// FUNCTION: 0x004D0590 $E5
// FUNCTION: 0x004D05B0 $E8
// FUNCTION: 0x004D05D0 $E11
// FUNCTION: 0x004D05F0 $E14
// FUNCTION: 0x004D0610 $E17
// FUNCTION: 0x004D0630 $E20
// FUNCTION: 0x004D0670 $E23
// FUNCTION: 0x004D06A0 $E26
// FUNCTION: 0x004D06D0 $E29
// FUNCTION: 0x004D0700 $E32
// FUNCTION: 0x004D0710 $E35

// FUNCTION: 0x004D27D0
HRESULT STDMETHODCALLTYPE FoggedObjectClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_FoggedObjectClass;
	return(S_OK);
}
