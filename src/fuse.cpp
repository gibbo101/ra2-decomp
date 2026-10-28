// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "fuse.h"
#include "geometry.h"
#include "rect.h"

// SYNTHETIC: GAMEMD 0x004E0F10 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x004E0F40 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x004E0F60 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x004E0F80 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x004E0FA0 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x004E0FC0 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x004E0FE0 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x004E1020 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x004E1050 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x004E1080 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x004E10B0 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x004E10C0 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x004E10E0 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x004E1100
FuseClass::FuseClass(void) :
	Timer(0),
	Arming(0),
	HeadTo(0,0,0),
	Proximity(0)
{
}

// FUNCTION: GAMEMD 0x004E1130
void FuseClass::Arm_Fuse(Coord const & location, Coord const & target, int arming, int timeto)
{
	timeto = MAX(timeto, arming);
	Timer = timeto;
	Arming = arming;
	HeadTo = target;
	Proximity = location.Distance_To(target);
}

// FUNCTION: GAMEMD 0x004E11F0
FuseResultType FuseClass::Fuse_Checkup(Coord const & newlocation)
{
	int	proximity;

	if (Arming) {
		return(FUSE_WAIT);
	} else {
		proximity = newlocation.Distance_To(HeadTo) / 2;
		if (proximity < 0x0010*2) return(FUSE_EXPLODE_CLOSE);

		if (proximity < CELL_LEPTON_W && proximity > Proximity) {
			return(FUSE_EXPLODE_FAR);
		}
		Proximity = proximity;
	}
	return(FUSE_WAIT);
}
