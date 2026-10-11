// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "teamtype.h"
#include "_rules.h"
#include "aitrig.h"
#include "ccini.h"
#include "ccrand.h"
#include "dbgprint.h"
#include "ddist.h"
#include "findmake.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "rules.h"
#include "scenario.h"
#include "script.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "tagtype.h"
#include "taskforc.h"
#include "team.h"
#include "tracker.h"
#include "vector.h"
#include "waypoint.h"

// FUNCTION: 0x006F1B90
HRESULT STDMETHODCALLTYPE TeamTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006F1C40
HRESULT STDMETHODCALLTYPE TeamTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TeamTypeClass;
	return(S_OK);
}
