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
