// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tube.h"
#include "_map.h"
#include "ccini.h"
#include "cell.h"
#include "coord.h"
#include "crc.h"
#include "globals.h"
#include "inline.h"
#include "tracker.h"
#include <cstdio>

// FUNCTION: 0x007281E0
HRESULT STDMETHODCALLTYPE TubeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x007286D0
HRESULT STDMETHODCALLTYPE TubeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TubeClass;
	return(S_OK);
}
