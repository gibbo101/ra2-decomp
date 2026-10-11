// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "smudge.h"
#include "_map.h"
#include "ccini.h"
#include "cell.h"
#include "globals.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"
#include <cstdio>

// FUNCTION: 0x006B4F00
HRESULT STDMETHODCALLTYPE SmudgeClass::Save(IStream *stream, int cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x006B4F50
HRESULT STDMETHODCALLTYPE SmudgeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SmudgeClass;
	return(S_OK);
}
