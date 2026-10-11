// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "smudtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "ccini.h"
#include "ccrand.h"
#include "cell.h"
#include "draw.h"
#include "findmake.h"
#include "globals.h"
#include "isotype.h"
#include "lightcon.h"
#include "mixfile.h"
#include "mouse.h"
#include "scenario.h"
#include "smudge.h"
#include "sun.h"
#include "tactical.h"
#include "tracker.h"

// FUNCTION: 0x006B58B0
HRESULT STDMETHODCALLTYPE SmudgeTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006B58D0
HRESULT STDMETHODCALLTYPE SmudgeTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SmudgeTypeClass;
	return(S_OK);
}
