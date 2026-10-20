// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "suprtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "findmake.h"
#include "globals.h"
#include "mixfile.h"
#include "mouse.h"
#include "noinit.h"
#include "sun.h"
#include "swizzle.h"
#include "weapon.h"

// FUNCTION: 0x006CE380 $E2
// FUNCTION: 0x006CE3B0 $E5
// FUNCTION: 0x006CE3D0 $E8
// FUNCTION: 0x006CE3F0 $E11
// FUNCTION: 0x006CE410 $E14
// FUNCTION: 0x006CE430 $E17
// FUNCTION: 0x006CE450 $E20
// FUNCTION: 0x006CE490 $E23
// FUNCTION: 0x006CE4C0 $E26
// FUNCTION: 0x006CE4F0 $E29
// FUNCTION: 0x006CE520 $E32
// FUNCTION: 0x006CE530 $E35

// FUNCTION: 0x006CE570
SuperWeaponType Special_From_Name(char const * name)
{
	static const char * _names[SUPER_COUNT] = {
		"MultiMissile",
		"IronCurtain",
		"LightningStorm",
		"ChronoSphere",
		"ChronoWarp",
		"ParaDrop",
		"AmerParaDrop",
		"PsychicDominator",
		"SpyPlane",
		"GeneticConverter",
		"ForceShield",
		"PsychicReveal"
	};

	for (int i = SUPER_FIRST; i < SUPER_COUNT; i++) {
		if (strcmpi(_names[i], name) == 0) {
			return(SuperWeaponType)(i);
		}
	}
	return(SUPER_NONE);
}

// FUNCTION: 0x006CE7C0
HRESULT STDMETHODCALLTYPE SuperWeaponTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SuperWeaponTypeClass;
	return(S_OK);
}

// FUNCTION: 0x006CE8D0
HRESULT STDMETHODCALLTYPE SuperWeaponTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
