// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "aitrig.h"
#include "_rules.h"
#include "airctype.h"
#include "builtype.h"
#include "crc.h"
#include "findmake.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "infatype.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "taskforc.h"
#include "techtype.h"
#include "trim.h"
#include "unittype.h"
#include "vector.h"

// FUNCTION: 0x0041E500
HRESULT STDMETHODCALLTYPE AITriggerTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AITriggerTypeClass;
	return(S_OK);
}

// FUNCTION: 0x0041E5C0
HRESULT STDMETHODCALLTYPE AITriggerTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
