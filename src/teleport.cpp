// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "teleport.h"
#include "foot.h"
#include "globals.h"

// FUNCTION: 0x00718090
boolean TeleportLocomotionClass::Is_Stationary(void)
{
	if (Is_Moving() == false) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00719C60
HRESULT STDMETHODCALLTYPE TeleportLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TeleportLocomotion;
	return(S_OK);
}

// FUNCTION: 0x00719E20
LayerType STDMETHODCALLTYPE TeleportLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}
