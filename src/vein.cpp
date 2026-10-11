// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "vein.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "astar.h"
#include "ccrand.h"
#include "cell.h"
#include "draw.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "inline.h"
#include "isotype.h"
#include "lightcon.h"
#include "mixfile.h"
#include "mouse.h"
#include "overtype.h"
#include "particle.h"
#include "partsys.h"
#include "ptype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "terrtype.h"
#include "tracker.h"
#include "vector.h"
#include "ramp.hh"
#include <cstdio>

static bool * GlobalGrowthState = NULL;

// FUNCTION: 0x0074D430
void VeinholeMonsterClass::Draw_All(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		VeinholeMonsters[i]->Draw_It();
	}
}

// FUNCTION: 0x0074D760
void VeinholeMonsterClass::Reset(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		delete VeinholeMonsters[i];
	}
	VeinholeMonsters.Clear();
}

// FUNCTION: 0x0074E880
void VeinholeMonsterClass::Clear_Global_Data(void)
{
	if (GlobalGrowthState) {
		delete [] GlobalGrowthState;
		GlobalGrowthState = NULL;
	}
}

// FUNCTION: 0x0074EEE0
HRESULT STDMETHODCALLTYPE VeinholeMonsterClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0074F2D0
HRESULT STDMETHODCALLTYPE VeinholeMonsterClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_VeinholeMonsterClass;
	return(S_OK);
}
