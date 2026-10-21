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

// FUNCTION: 0x004E11F0
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

int DAT_008B3E78;
int DAT_008B3E7C;
int DAT_008B3E80;
int DAT_008B3E84;

// FUNCTION: 0x004E12B0
void FUN_004E12B0(void)
{
	DAT_008B3E78 = 0;
	DAT_008B3E7C = 0;
	DAT_008B3E80 = 0;
	DAT_008B3E84 = 0;
}
