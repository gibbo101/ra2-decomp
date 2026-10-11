// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ovrlight.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "bsurface.h"
#include "dsurface.h"
#include "getcpu.h"
#include "globals.h"
#include "rules.h"
#include "scenario.h"
#include "surface.h"
#include "tactical.h"

int *SpotLightMMXBuffer;

BSurface *SpotLightSurfaces[SpotLightClass::SPOTLIGHT_SURFACE_COUNT + SpotLightClass::SPOTLIGHT_EXTRA_SURFACE_COUNT];

// FUNCTION: 0x005FF800
void SpotLightClass::Clear_All(void)
{
	if (SpotLightMMXBuffer != NULL) {
		delete [] SpotLightMMXBuffer;
		SpotLightMMXBuffer = NULL;
	}
	for (int i = 0; i < SPOTLIGHT_SURFACE_COUNT + SPOTLIGHT_EXTRA_SURFACE_COUNT; i++) {
		delete SpotLightSurfaces[i];
		SpotLightSurfaces[i] = NULL;
	}
}
