// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "facing.h"
#include <math.h>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E8828;
extern const double DAT_007E8830;

// FUNCTION: 0x004C81F0 $E2
double DAT_008B3A20 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004C8220 $E5
double DAT_008B3A70 = DAT_008B3A20 / DAT_007E8828;

// FUNCTION: 0x004C8240 $E8
double DAT_008B3A58 = DAT_007E1718 / DAT_008B3A70;

// FUNCTION: 0x004C8260 $E11
double DAT_008B3A50 = DAT_007E1720 * DAT_007E8830;

// FUNCTION: 0x004C8280 $E14
double DAT_008B3A28 = DAT_007E1728 * DAT_007E8830;

// FUNCTION: 0x004C82A0 $E17
double DAT_008A3A00 = DAT_007E1730 * DAT_007E8830;

// FUNCTION: 0x004C82C0 $E20
int DAT_008B3A78 = (int)(fastmath::tan(DAT_008A3A00 - DAT_008B3A28) * DAT_007E1738 * DAT_008B3A20);

// FUNCTION: 0x004C8300 $E23
double DAT_008B3A30 = fastmath::atan(DAT_008B3A78 * DAT_007E1740);

// FUNCTION: 0x004C8330 $E26
double DAT_008B3A48 = fastmath::atan((DAT_008B3A78 * 2) / DAT_008B3A20);

// FUNCTION: 0x004C8360 $E29
int DAT_008B3A6C = (int)(DAT_008B3A78 * 4 + DAT_007E1738);

struct FacingShortPair { short a; short b; FacingShortPair(void) { a = 0; b = 0; } };
struct FacingIntTriple { int a; int b; int c; FacingIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x004C8390 $E32
FacingShortPair DAT_008B3A18;

// FUNCTION: 0x004C83A0 $E35
FacingIntTriple DAT_008B3A38;


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
