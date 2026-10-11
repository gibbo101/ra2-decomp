// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "animtype.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_theater.h"
#include "ccfile.h"
#include "data.h"
#include "dbgprint.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "mixfile.h"
#include "overtype.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "warhead.h"

// FUNCTION: 0x00428970
HRESULT STDMETHODCALLTYPE AnimTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00428990
HRESULT STDMETHODCALLTYPE AnimTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AnimTypeClass;
	return(S_OK);
}
