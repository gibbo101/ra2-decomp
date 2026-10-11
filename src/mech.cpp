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

// FUNCTION: 0x005AFEF0
MechLocomotionClass::MechLocomotionClass(void) :
	BASECLASS(),
	DestinationCoord(COORD_NONE),
	HeadToCoord(COORD_NONE),
	IsMoving(false)
{
}

// FUNCTION: 0x005AFF50
// FUNCTION: 0x005B1B50
MechLocomotionClass::~MechLocomotionClass(void)
{
}

// FUNCTION: 0x005AFF70
boolean STDMETHODCALLTYPE MechLocomotionClass::Is_Moving(void)
{
	return(IsMoving);
}

// FUNCTION: 0x005AFF80
CoordStruct STDMETHODCALLTYPE MechLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(COORD_NONE);
}

// FUNCTION: 0x005B0060
boolean STDMETHODCALLTYPE MechLocomotionClass::Process(void)
{
	Movement_AI(true);
	return(Is_Moving());
}

// FUNCTION: 0x005B0120
void STDMETHODCALLTYPE MechLocomotionClass::Stop_Moving(void)
{
	DestinationCoord = COORD_NONE;
	if (HeadToCoord == COORD_NONE) {
		IsMoving = false;
	}
}

// FUNCTION: 0x005B01A0
void STDMETHODCALLTYPE MechLocomotionClass::Force_Immediate_Destination(CoordStruct coord)
{
	HeadToCoord = coord;
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

extern int DAT_00ABEE60;
extern int DAT_00ABEE64;
extern int DAT_00ABEE68;
extern int DAT_00ABEE6C;

// FUNCTION: 0x005B1B80
void FUN_005B1B80(void)
{
	DAT_00ABEE60 = 0;
	DAT_00ABEE64 = 0;
	DAT_00ABEE68 = 0;
	DAT_00ABEE6C = 0;
}
