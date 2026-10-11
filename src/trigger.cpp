// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "trigger.h"
#include "ccrand.h"
#include "crc.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "taction.h"
#include "tevent.h"
#include "tracker.h"
#include "trigtype.h"
#include "vector.h"

// FUNCTION: 0x00726820
HRESULT STDMETHODCALLTYPE TriggerClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TriggerClass;
	return(S_OK);
}

// FUNCTION: 0x007268D0
HRESULT STDMETHODCALLTYPE TriggerClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
