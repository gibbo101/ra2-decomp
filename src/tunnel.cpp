// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "tunnel.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "anim.h"
#include "cell.h"
#include "combat.h"
#include "coord.h"
#include "face.h"
#include "fastmath.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "map.h"
#include "noinit.h"
#include "rules.h"
#include "scenario.h"
#include "sun.h"
#include "tactical.h"
#include "layer.hh"
#include "visual.hh"
#include <new.h>

// FUNCTION: 0x0072A090
MoveType STDMETHODCALLTYPE TunnelLocomotionClass::Can_Enter_Cell(CellStruct cell)
{
	if (!Debug_Map && !Map[cell].Can_Burrow_Here()) {
		return(MOVE_NO);
	}
	return(MOVE_OK);
}

// FUNCTION: 0x0072A110
HRESULT STDMETHODCALLTYPE TunnelLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TunnelLocomotion;
	return(S_OK);
}
