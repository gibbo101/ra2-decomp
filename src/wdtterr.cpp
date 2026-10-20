// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

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
extern const double DAT_007F7320;
extern const double DAT_007F7328;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0076F720 $E2
double DAT_00B72B30 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0076F750 $E5
double DAT_00B72B80 = DAT_00B72B30 / DAT_007F7320;

// FUNCTION: 0x0076F770 $E8
double DAT_00B72B68 = DAT_007E1718 / DAT_00B72B80;

// FUNCTION: 0x0076F790 $E11
double DAT_00B72B60 = DAT_007E1720 * DAT_007F7328;

// FUNCTION: 0x0076F7B0 $E14
double DAT_00B72B38 = DAT_007E1728 * DAT_007F7328;

// FUNCTION: 0x0076F7D0 $E17
double DAT_00B72B10 = DAT_007E1730 * DAT_007F7328;

// FUNCTION: 0x0076F7F0 $E20
int DAT_00B72B88 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B72B10 - DAT_00B72B38) * DAT_00B72B30));

// FUNCTION: 0x0076F830 $E23
double DAT_00B72B40 = FUN_004CADE0(DAT_00B72B88 * DAT_007E1740);

// FUNCTION: 0x0076F860 $E26
double DAT_00B72B58 = FUN_004CADE0((DAT_00B72B88 * 2) / DAT_00B72B30);

// FUNCTION: 0x0076F890 $E29
int DAT_00B72B7C = (int)(DAT_00B72B88 * 4 + DAT_007E1738);

// FUNCTION: 0x0076F8C0 $E32
ShortPair DAT_00B72B28;

// FUNCTION: 0x0076F8D0 $E35
IntTriple DAT_00B72B48;
