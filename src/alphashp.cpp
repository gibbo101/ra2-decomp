// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "alphashp.h"
#include "_alpha.h"
#include "_tactica.h"
#include "crc.h"
#include "globals.h"
#include "object.h"
#include "objtype.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "tracker.h"
#include "vector.h"

static bool BrightnessCalculated = false;

// FUNCTION: 0x00420E40
HRESULT STDMETHODCALLTYPE AlphaShapeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		return(S_OK);
	}
	return(result);
}

// FUNCTION: 0x004216C0
void AlphaShapeClass::Calculate_Brightness_Table(void)
{
	BrightnessCalculated = true;
	for (int i = 0; i < 256*256; i++) {
		int low = i % 256;
		int high = i / 256;
		int brightness = MAX(0, MIN(255, low * high / 127));
		BrightnessTable[0][i] = brightness;
	}
}
