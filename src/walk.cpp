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
#include "fastmath.h"
#include "coord.h"

extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F69C0;
extern const double DAT_007F69C8;

// FUNCTION: 0x0075A8A0 $E85
double DAT_00B45BD0 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0075A8D0 $E88
double DAT_00B45C20 = DAT_00B45BD0 / DAT_007F69C0;

// FUNCTION: 0x0075A8F0 $E91
double DAT_00B45C08 = DAT_007E1718 / DAT_00B45C20;

// FUNCTION: 0x0075A910 $E94
double DAT_00B45C00 = DAT_007E1720 * DAT_007F69C8;

// FUNCTION: 0x0075A930 $E97
double DAT_00B45BD8 = DAT_007E1728 * DAT_007F69C8;

// FUNCTION: 0x0075A950 $E100
double DAT_00B45BB0 = DAT_007E1730 * DAT_007F69C8;

// FUNCTION: 0x0075A970 $E103
int DAT_00B45C28 = (int)(fastmath::tan(DAT_00B45BB0 - DAT_00B45BD8) * DAT_007E1738 * DAT_00B45BD0);

// FUNCTION: 0x0075A9B0 $E106
double DAT_00B45BE0 = fastmath::atan(DAT_00B45C28 * DAT_007E1740);

// FUNCTION: 0x0075A9E0 $E109
double DAT_00B45BF8 = fastmath::atan((DAT_00B45C28 * 2) / DAT_00B45BD0);

// FUNCTION: 0x0075AA10 $E112
int DAT_00B45C1C = (int)(DAT_00B45C28 * 4 + DAT_007E1738);

// FUNCTION: 0x0075AA40 $E115
Cell DAT_00B45BC8(0, 0);

// FUNCTION: 0x0075AA50 $E118
Coord DAT_00B45BE8(0, 0, 0);

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
