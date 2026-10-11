// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "taskforc.h"
#include "ahtoi.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "sun.h"
#include "swizzle.h"
#include "techtype.h"
#include "tracker.h"
#include "trim.h"
#include "vector.h"
#include <cstdio>

// FUNCTION: 0x006E8680
HRESULT STDMETHODCALLTYPE TaskForceClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006E8710
HRESULT STDMETHODCALLTYPE TaskForceClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TaskForceClass;
	return(S_OK);
}
