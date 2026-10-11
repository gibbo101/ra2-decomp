// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "airctype.h"
#include "_mixfile.h"
#include "_rules.h"
#include "aircraft.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "mixfile.h"
#include "rules.h"
#include "sun.h"
#include "tracker.h"

// FUNCTION: 0x0041CB70
Cell const * AircraftTypeClass::Occupy_List(bool) const
{
	static Cell const _list[] = {Cell(0,0), REFRESH_EOL};
	return(_list);
}

// FUNCTION: 0x0041CE90
HRESULT STDMETHODCALLTYPE AircraftTypeClass::Save(IStream *stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0041CEB0
HRESULT STDMETHODCALLTYPE AircraftTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AircraftTypeClass;
	return(S_OK);
}
