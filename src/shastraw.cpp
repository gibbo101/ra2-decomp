// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "shastraw.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F12F8;
extern const double DAT_007F1300;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0069EA30
int SHAStraw::Result(void * result) const
{
	return(SHA.Result(result));
}

// FUNCTION: 0x0069EA40 $E2
double DAT_00B077E0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0069EA70 $E5
double DAT_00B07830 = DAT_00B077E0 / DAT_007F12F8;

// FUNCTION: 0x0069EA90 $E8
double DAT_00B07818 = DAT_007E1718 / DAT_00B07830;

// FUNCTION: 0x0069EAB0 $E11
double DAT_00B07810 = DAT_007E1720 * DAT_007F1300;

// FUNCTION: 0x0069EAD0 $E14
double DAT_00B077E8 = DAT_007E1728 * DAT_007F1300;

// FUNCTION: 0x0069EAF0 $E17
double DAT_00B077C0 = DAT_007E1730 * DAT_007F1300;

// FUNCTION: 0x0069EB10 $E20
int DAT_00B07838 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B077C0 - DAT_00B077E8) * DAT_00B077E0));

// FUNCTION: 0x0069EB50 $E23
double DAT_00B077F0 = FUN_004CADE0(DAT_00B07838 * DAT_007E1740);

// FUNCTION: 0x0069EB80 $E26
double DAT_00B07808 = FUN_004CADE0((DAT_00B07838 * 2) / DAT_00B077E0);

// FUNCTION: 0x0069EBB0 $E29
int DAT_00B0782C = (int)(DAT_00B07838 * 4 + DAT_007E1738);

// FUNCTION: 0x0069EBE0 $E32
ShortPair DAT_00B077D8;

// FUNCTION: 0x0069EBF0 $E35
IntTriple DAT_00B077F8;
