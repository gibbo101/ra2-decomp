// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x005FEFC0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005FEFF0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005FF010 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005FF030 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005FF050 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005FF070 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005FF090 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005FF0D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005FF100 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005FF130 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005FF160 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005FF170 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005FF190 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005FF1B0 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005FF1D0 SYMBOL
// _$E120
DynamicVectorClass<SpotLightClass *> SpotLights;

// SYNTHETIC: GAMEMD 0x005FF210 SYMBOL
// _$E118

int *SpotLightMMXBuffer;

BSurface *SpotLightSurfaces[SpotLightClass::SPOTLIGHT_SURFACE_COUNT + SpotLightClass::SPOTLIGHT_EXTRA_SURFACE_COUNT];

// FUNCTION: GAMEMD 0x005FF250
SpotLightClass::SpotLightClass(Coord pos, int size) :
	Radius(0),
	Position(pos),
	Size(size),
	DisableFlags(0)
{
	SpotLights.Add(this);
}

// FUNCTION: GAMEMD 0x005FF2D0
SpotLightClass::~SpotLightClass(void)
{
	SpotLights.Delete(this);
}

// FUNCTION: GAMEMD 0x005FF320
void SpotLightClass::AI(void)
{
	Radius += SPOTLIGHT_RADIUS_STEP;
	if (Radius >= SPOTLIGHT_MAX_RADIUS) {
		delete this;
	}
}

// FUNCTION: GAMEMD 0x005FF390
void SpotLightClass::Update_All(void)
{
	for (int i = SpotLights.Count() - 1; i >= 0; i--) {
		SpotLights[i]->AI();
	}
}

// FUNCTION: GAMEMD 0x005FF800
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

// FUNCTION: GAMEMD 0x005FFFA0
void SpotLightClass::Draw_All(void)
{
	for (int i = SpotLights.Count() - 1; i >= 0; i--) {
		SpotLights[i]->Draw_It();
	}
}

// TEMPLATE: GAMEMD 0x005FFFC0
// ?Clear@?$DynamicVectorClass@PAVSpotLightClass@@@@UAEXXZ
template class DynamicVectorClass<SpotLightClass *>;

// TEMPLATE: GAMEMD 0x005FFFF0
// ?ID@?$DynamicVectorClass@PAVSpotLightClass@@@@UAEHPBQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x00600010
// ?GetItem@?$VectorClass@PAVSpotLightClass@@@@UBEPAVSpotLightClass@@H@Z

// TEMPLATE: GAMEMD 0x00600020
// ??8?$VectorClass@PAVSpotLightClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x00600060
// ?Resize@?$VectorClass@PAVSpotLightClass@@@@UAE_NHPBQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x00600110
// ?Clear@?$VectorClass@PAVSpotLightClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x00600140
// ?ID@?$VectorClass@PAVSpotLightClass@@@@UAEHABQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x00600180
// ?ID@?$VectorClass@PAVSpotLightClass@@@@UAEHPBQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x006001A0
// ?Resize@?$DynamicVectorClass@PAVSpotLightClass@@@@UAE_NHPBQAVSpotLightClass@@@Z

// TEMPLATE: GAMEMD 0x00600250
// ?ID@?$DynamicVectorClass@PAVSpotLightClass@@@@UAEHABQAVSpotLightClass@@@Z

// SYNTHETIC: GAMEMD 0x00600280
// ??_G?$VectorClass@PAVSpotLightClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x006002D0
// ??_G?$DynamicVectorClass@PAVSpotLightClass@@@@UAEPAXI@Z
