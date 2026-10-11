// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "droppod.h"
#include "_map.h"
#include "_rand.h"
#include "_rules.h"
#include "anim.h"
#include "cell.h"
#include "combat.h"
#include "coord.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "map.h"
#include "noinit.h"
#include "rules.h"
#include "sun.h"
#include "weapon.h"
#include "layer.hh"
#include <new.h>

// FUNCTION: 0x004B63B0
HRESULT STDMETHODCALLTYPE DropPodLocomotionClass::Begin_Piggyback(ILocomotion * pointer)
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

// FUNCTION: 0x004B6400
HRESULT STDMETHODCALLTYPE DropPodLocomotionClass::End_Piggyback(ILocomotion ** pointer)
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
