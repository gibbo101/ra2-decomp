// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "facing.h"

// FUNCTION: 0x004C91C0
FacingClass::FacingClass(void) :
	DesiredFacing(DIR_N),
	StartFacing(DIR_N),
	ROT(DIR_N)
{
}

// FUNCTION: 0x004C91E0
FacingClass::FacingClass(int rate) :
	DesiredFacing(DIR_N),
	StartFacing(DIR_N),
	ROT(DIR_N)
{
	Set_ROT(rate);
}

// FUNCTION: 0x004C9300
bool FacingClass::Set(DirType const & facing)
{
	if (Current() != facing) {
		DesiredFacing = facing;
		StartFacing = facing;
		RotationTimer = 0;
		return(true);
	}

	RotationTimer = 0;
	return(false);
}

// FUNCTION: 0x004C93D0
DirType FacingClass::Current(void) const
{
	if (Is_Rotating()) {
		short diff = Turn_Arc();
		short rot = abs(diff) / ROT.Facing;
		DirType dir = DesiredFacing;

		if (rot > 0) {
			dir.Facing -= RotationTimer * (diff / rot);
		}
		return(dir);
	}
	return(DesiredFacing);
}

// FUNCTION: 0x004C9470
DirType FacingClass::Desired(void) const
{
	return(DesiredFacing);
}

// FUNCTION: 0x004C9480
bool FacingClass::Is_Rotating(void) const
{
	return(ROT.Facing > 0 && RotationTimer);
}

// FUNCTION: 0x004C94B0
bool FacingClass::Is_Rotating_CW(void) const
{
	if (Is_Rotating()) {
		return(Turn_Arc() > 0);
	}
	return(false);
}

// FUNCTION: 0x004C9530
DirType FacingClass::Difference(void)
{
	return(Desired() - Current());
}

// FUNCTION: 0x004C95D0
DirType FacingClass::Difference(DirType const & facing)
{
	return(facing - Current());
}

// FUNCTION: 0x004C9680
void FacingClass::Set_ROT(int rate)
{
	ROT.From_Dir256((Dir256)MIN(rate, DIR_S - 1));
}
