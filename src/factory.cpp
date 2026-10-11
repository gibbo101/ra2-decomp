// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "factory.h"
#include "_rules.h"
#include "bound.h"
#include "building.h"
#include "crc.h"
#include "dbgprint.h"
#include "globals.h"
#include "house.h"
#include "rules.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "voc.h"
#include "super.hh"

// FUNCTION: 0x004CA230
HRESULT STDMETHODCALLTYPE FactoryClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_FactoryClass;
	return(S_OK);
}
