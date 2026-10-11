// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mech.h"
#include "_map.h"
#include "_rules.h"
#include "assert.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "noinit.h"
#include "overtype.h"
#include "rules.h"
#include "tactical.h"
#include "tube.h"
#include "layer.hh"
#include <new.h>

// FUNCTION: 0x005AFF50
MechLocomotionClass::~MechLocomotionClass(void)
{
}

// FUNCTION: 0x005B0060
boolean STDMETHODCALLTYPE MechLocomotionClass::Process(void)
{
	Movement_AI(true);
	return(Is_Moving());
}

// FUNCTION: 0x005B1960
HRESULT STDMETHODCALLTYPE MechLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_MechLocomotion;
	return(S_OK);
}

// FUNCTION: 0x005B19A0
HRESULT STDMETHODCALLTYPE MechLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) MechLocomotionClass(NoInitClass());
	}
	return(result);
}

// FUNCTION: 0x005B19D0
LayerType STDMETHODCALLTYPE MechLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x005B1AA0
boolean STDMETHODCALLTYPE MechLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	CoordStruct coord = Head_To_Coord();

	if (Coord(coord).As_Cell() == ((Coord &)to).As_Cell() && abs(coord.Z - to.Z) <= LEVEL_LEPTON_H) {
		return(true);
	}

	return(false);
}
