// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "empulse.h"
#include "_map.h"
#include "_rules.h"
#include "aircraft.h"
#include "anim.h"
#include "building.h"
#include "ccrand.h"
#include "cell.h"
#include "crc.h"
#include "globals.h"
#include "house.h"
#include "infantry.h"
#include "infatype.h"
#include "mouse.h"
#include "rules.h"
#include "sun.h"
#include "tag.h"
#include "tracker.h"
#include "unit.h"
#include "unittype.h"

// FUNCTION: 0x004C5470
void EMPulseClass::Reset(void)
{
	while (EMPulses.Count()) {
		delete EMPulses[0];
	}
}

// FUNCTION: 0x004C59F0
HRESULT STDMETHODCALLTYPE EMPulseClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_EMPulseClass;
	return(S_OK);
}

// FUNCTION: 0x004C5A80
HRESULT STDMETHODCALLTYPE EMPulseClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
