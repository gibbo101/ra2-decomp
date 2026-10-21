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

// FUNCTION: 0x005FF1D0 $E75
DynamicVectorClass<SpotLightClass *> SpotLights;

// FUNCTION: 0x005FF210 $E73

int *SpotLightMMXBuffer;

BSurface *SpotLightSurfaces[SpotLightClass::SPOTLIGHT_SURFACE_COUNT + SpotLightClass::SPOTLIGHT_EXTRA_SURFACE_COUNT];

// FUNCTION: 0x005FF250
SpotLightClass::SpotLightClass(Coord pos, int size) :
	Radius(0),
	Position(pos),
	Size(size),
	DisableFlags(0)
{
	SpotLights.Add(this);
}

// FUNCTION: 0x005FF2D0
SpotLightClass::~SpotLightClass(void)
{
	SpotLights.Delete(this);
}

// FUNCTION: 0x005FF320
void SpotLightClass::AI(void)
{
	Radius += SPOTLIGHT_RADIUS_STEP;
	if (Radius >= SPOTLIGHT_MAX_RADIUS) {
		delete this;
	}
}

// FUNCTION: 0x005FF390
void SpotLightClass::Update_All(void)
{
	for (int i = SpotLights.Count() - 1; i >= 0; i--) {
		SpotLights[i]->AI();
	}
}

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

// FUNCTION: 0x005FFFA0
void SpotLightClass::Draw_All(void)
{
	for (int i = SpotLights.Count() - 1; i >= 0; i--) {
		SpotLights[i]->Draw_It();
	}
}

// FUNCTION: 0x005FFFC0 ?$DynamicVectorClass@PAVSpotLightClass@@::Clear
template class DynamicVectorClass<SpotLightClass *>;

// FUNCTION: 0x005FFFF0 ?$DynamicVectorClass@PAVSpotLightClass@@::ID

// FUNCTION: 0x00600010 ?$VectorClass@PAVSpotLightClass@@::GetItem

// FUNCTION: 0x00600020 ?$VectorClass@PAVSpotLightClass@@::operator==

// FUNCTION: 0x00600060 ?$VectorClass@PAVSpotLightClass@@::Resize

// FUNCTION: 0x00600110 ?$VectorClass@PAVSpotLightClass@@::Clear

// FUNCTION: 0x00600140 ?$VectorClass@PAVSpotLightClass@@::ID

// FUNCTION: 0x00600180 ?$VectorClass@PAVSpotLightClass@@::ID

// FUNCTION: 0x006001A0 ?$DynamicVectorClass@PAVSpotLightClass@@::Resize

// FUNCTION: 0x00600250 ?$DynamicVectorClass@PAVSpotLightClass@@::ID

// FUNCTION: 0x00600280 ?$DynamicVectorClass@PAVSpotLightClass@@::~?$DynamicVectorClass@PAVSpotLightClass@@

// FUNCTION: 0x006002D0 ?$VectorClass@PAVSpotLightClass@@::~?$VectorClass@PAVSpotLightClass@@

int DAT_00AC18B0;
int DAT_00AC18B4;
int DAT_00AC18B8;
int DAT_00AC18BC;

// FUNCTION: 0x00600320
void FUN_00600320(void)
{
	DAT_00AC18B0 = 0;
	DAT_00AC18B4 = 0;
	DAT_00AC18B8 = 0;
	DAT_00AC18BC = 0;
}
