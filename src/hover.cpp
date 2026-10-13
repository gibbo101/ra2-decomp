// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "hover.h"
#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "infantry.h"
#include "inline.h"
#include "ion.h"
#include "overtype.h"
#include "rules.h"
#include "tube.h"
#include "unit.h"
#include "tube.hh"

// FUNCTION: 0x00513D00
HoverLocomotionClass::~HoverLocomotionClass(void)
{
}

// FUNCTION: 0x00514C80
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Moving_Now(void)
{
	return(Is_Moving() && Height != 0.0);
}

// FUNCTION: 0x005163A0
bool HoverLocomotionClass::Is_Moving1(void)
{
	if (Is_Moving()) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00516FC0
boolean STDMETHODCALLTYPE HoverLocomotionClass::Shove(DirStruct dir)
{
	if (Push(dir)) {
		Do_Shove();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00517020
void HoverLocomotionClass::Do_Shove(void)
{
	WasShoved = true;
	ShoveAccum = Random_Pick(20, 30);
	if (Percent_Chance(50)) {
		ShoveAccum = -ShoveAccum;
	}
}

// FUNCTION: 0x00517070
HRESULT STDMETHODCALLTYPE HoverLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_HoverLocomotion;
	return(S_OK);
}

// FUNCTION: 0x00517100
LayerType STDMETHODCALLTYPE HoverLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x00517210
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	Coord coord = Head_To_Coord();

	if (coord.As_Cell() == ((Coord &)to).As_Cell() && abs(coord.Z - to.Z) <= LEVEL_LEPTON_H) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005172B0 HoverLocomotionClass::Fetch_Object_Size

// FUNCTION: 0x00517310 ?$TPoint2D@H::operator==
template<class T>
bool TPoint2D<T>::operator == (TPoint2D<T> const & rvalue) const
{
	return(X==rvalue.X && Y==rvalue.Y);
}

template class TPoint2D<int>;
