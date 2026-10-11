// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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

// FUNCTION: 0x0055AC70
CoordStruct STDMETHODCALLTYPE LocomotionClass::Destination(void)
{
	CoordStruct coord;
	coord.X = COORD_NONE.X;
	coord.Y = COORD_NONE.Y;
	coord.Z = COORD_NONE.Z;
	return(coord);
}
