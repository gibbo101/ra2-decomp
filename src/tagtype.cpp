// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tagtype.h"
#include "ahtoi.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "noinit.h"
#include "sun.h"
#include "swizzle.h"
#include "tag.h"
#include "tracker.h"
#include "trigtype.h"
#include "vector.h"

// FUNCTION: 0x006E63A0
HRESULT STDMETHODCALLTYPE TagTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TagTypeClass;
	return(S_OK);
}

// FUNCTION: 0x006E6470
HRESULT STDMETHODCALLTYPE TagTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
