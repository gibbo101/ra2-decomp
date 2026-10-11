// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tevent.h"
#include "building.h"
#include "builtype.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "team.h"
#include "techno.h"
#include "tracker.h"
#include "vector.h"

// FUNCTION: 0x0071F880
HRESULT STDMETHODCALLTYPE TEventClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_EventClass;
	return(S_OK);
}

// FUNCTION: 0x0071F930
HRESULT STDMETHODCALLTYPE TEventClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
