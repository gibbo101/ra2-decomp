// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "walk.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "aircraft.h"
#include "building.h"
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
#include "unit.h"
#include "layer.hh"

// FUNCTION: 0x0075C640
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WalkLocomotion;
	return(S_OK);
}

// FUNCTION: 0x0075C7E0
LayerType STDMETHODCALLTYPE WalkLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x0075C850
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::Begin_Piggyback(ILocomotion * pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (Piggybacker == NULL) {
		Piggybacker = pointer;
		return(S_OK);
	}
	return(E_FAIL);
}

// FUNCTION: 0x0075C8A0
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::End_Piggyback(ILocomotion ** pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (Piggybacker != NULL) {
		*pointer = Piggybacker;
		Piggybacker.Detach();
		return(S_OK);
	}
	return(S_FALSE);
}

// FUNCTION: 0x0075CA80
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	Coord headto = Head_To_Coord();
	if (headto.As_Cell() == Coord(to).As_Cell() && abs(headto.Z - to.Z) <= LEVEL_LEPTON_H) {
		return(true);
	}
	return(false);
}
