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

// SYNTHETIC: GAMEMD 0x005FF1D0 SYMBOL
// _$E75
DynamicVectorClass<SpotLightClass *> SpotLights;

// SYNTHETIC: GAMEMD 0x005FF210 SYMBOL
// _$E73

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

int DAT_00AC18B0;
int DAT_00AC18B4;
int DAT_00AC18B8;
int DAT_00AC18BC;

// FUNCTION: GAMEMD 0x00600320
void FUN_00600320(void)
{
	DAT_00AC18B0 = 0;
	DAT_00AC18B4 = 0;
	DAT_00AC18B8 = 0;
	DAT_00AC18BC = 0;
}
