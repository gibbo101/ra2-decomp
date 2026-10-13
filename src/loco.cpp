// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "loco.h"
#include "_map.h"
#include "_tactica.h"
#include "assert.h"
#include "cell.h"
#include "coord.h"
#include "foot.h"
#include "globals.h"
#include "map.h"
#include "swizzle.h"
#include "tactical.h"
#include "fire.hh"
#include "move.hh"
#include "visual.hh"
#include "zgrad.hh"

// FUNCTION: 0x0055A4D0 $E2
// FUNCTION: 0x0055A500 $E5
// FUNCTION: 0x0055A520 $E8
// FUNCTION: 0x0055A540 $E11
// FUNCTION: 0x0055A560 $E14
// FUNCTION: 0x0055A580 $E17
// FUNCTION: 0x0055A5A0 $E20
// FUNCTION: 0x0055A5E0 $E23
// FUNCTION: 0x0055A610 $E26
// FUNCTION: 0x0055A640 $E29
// FUNCTION: 0x0055A670 $E32
// FUNCTION: 0x0055A680 $E35

// FUNCTION: 0x0055A9B0
LONG STDMETHODCALLTYPE LocomotionClass::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	*ppvObject = NULL;

	if (riid == IID_IUnknown) {
		*ppvObject = (IUnknown *)(ILocomotion *)this;
	}
	if (riid == IID_IPersistStream) {
		*ppvObject = (IPersistStream *)this;
	}
	if (riid == IID_ILocomotion) {
		*ppvObject = (ILocomotion *)this;
	}
	if (riid == IID_IPersist) {
		*ppvObject = (IPersist *)this;
	}
	if (*ppvObject == NULL) {
		return(E_NOINTERFACE);
	}

	AddRef();
	return(S_OK);
}

// FUNCTION: 0x0055ABB0
ZGradientType STDMETHODCALLTYPE LocomotionClass::Z_Gradient(void)
{
	return(ZGRAD_90DEG);
}

// FUNCTION: 0x0055ABD0
Point2DStruct STDMETHODCALLTYPE LocomotionClass::Draw_Point(void)
{
	Point2DStruct pt;
	pt.X = 0;
	pt.Y = 0;
	return(pt);
}

// FUNCTION: 0x0055AC70
CoordStruct STDMETHODCALLTYPE LocomotionClass::Destination(void)
{
	CoordStruct coord;
	coord.X = COORD_NONE.X;
	coord.Y = COORD_NONE.Y;
	coord.Z = COORD_NONE.Z;
	return(coord);
}
