// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "objtype.h"
#include "sun.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_theater.h"
#include "aircraft.h"
#include "airctype.h"
#include "building.h"
#include "builtype.h"
#include "ccfile.h"
#include "coord.h"
#include "globals.h"
#include "house.h"
#include "mixfile.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "unittype.h"
#include "vector.h"
#include "voc.h"

// FUNCTION: 0x005F75C0
Point3D ObjectTypeClass::Pixel_Dimensions(void) const
{
	return(Point3D(10, 10, 10));
}

// FUNCTION: 0x005F7640
Cell const * ObjectTypeClass::Occupy_List(bool) const
{
	static Cell const _list[] = {Cell(0, 0), REFRESH_EOL};
	return(_list);
}

// FUNCTION: 0x005F9950
HRESULT STDMETHODCALLTYPE ObjectTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x005F9970
HRESULT STDMETHODCALLTYPE ObjectTypeClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	HRESULT res = BASECLASS::GetSizeMax(pcbSize);
	if (SUCCEEDED(res)) {
		res = S_OK;
	}
	return(res);
}
