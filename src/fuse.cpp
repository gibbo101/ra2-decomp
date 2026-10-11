// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "fuse.h"

// FUNCTION: 0x004E1100
FuseClass::FuseClass(void) :
	Timer(0),
	Arming(0),
	HeadTo(0,0,0),
	Proximity(0)
{
}

// FUNCTION: 0x004E1130
void FuseClass::Arm_Fuse(Coord const & location, Coord const & target, int arming, int timeto)
{
	timeto = MAX(timeto, arming);
	Timer = timeto;
	Arming = arming;
	HeadTo = target;
	Proximity = location.Distance_To(target);
}
