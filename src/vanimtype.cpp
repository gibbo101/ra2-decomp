// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "vanimtype.h"
#include "animtype.h"
#include "ccfile.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "psystype.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "techtype.h"
#include "tracker.h"
#include "vector.h"
#include "warhead.h"

// FUNCTION: 0x0074B7D0
HRESULT STDMETHODCALLTYPE VoxelAnimTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_VoxelAnimTypeClass;
	return(S_OK);
}

// FUNCTION: 0x0074B8D0
HRESULT STDMETHODCALLTYPE VoxelAnimTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
