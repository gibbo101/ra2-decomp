// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tiberium.h"
#include "_map.h"
#include "animtype.h"
#include "astar.h"
#include "ccini.h"
#include "cell.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "overtype.h"
#include "priority.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#define MAX_SPREAD_DELAY	50
#define MAX_GROWTH_DELAY	50
#define MAX_GROWTH_STAGE	11

// FUNCTION: 0x00721E40
HRESULT STDMETHODCALLTYPE TiberiumClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TiberiumClass;
	return(S_OK);
}
