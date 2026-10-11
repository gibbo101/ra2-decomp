// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "isotile.h"
#include "_isotile.h"
#include "_map.h"
#include "anim.h"
#include "cell.h"
#include "globals.h"
#include "isotype.h"
#include "overtype.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"

// FUNCTION: 0x005439F0
HRESULT STDMETHODCALLTYPE IsometricTileClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00543AB0
HRESULT STDMETHODCALLTYPE IsometricTileClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_IsometricTileClass;
	return(S_OK);
}
