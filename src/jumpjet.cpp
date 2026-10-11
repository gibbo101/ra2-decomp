// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "jumpjet.h"
#include "_map.h"
#include "_rules.h"
#include "building.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "ion.h"
#include "mouse.h"
#include "rules.h"
#include "layer.hh"

// FUNCTION: 0x0054B710
HRESULT STDMETHODCALLTYPE JumpjetLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_JumpjetLocomotion;
	return(S_OK);
}
