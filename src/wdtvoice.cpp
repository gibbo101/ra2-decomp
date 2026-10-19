// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "_mixfile.h"
#include "ccfile.h"
#include "ccrand.h"
#include "data.h"
#include "dsaudio.h"
#include "globals.h"
#include "goptions.h"
#include "ini.h"
#include "mixfile.h"
#include "wdtnet.h"

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
extern const double DAT_007F7340;
extern const double DAT_007F7348;

// FUNCTION: 0x00770080 $E18
double DAT_00B72BB0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x007700B0 $E21
double DAT_00B72C00 = DAT_00B72BB0 / DAT_007F7340;

// FUNCTION: 0x007700D0 $E24
double DAT_00B72BE8 = DAT_007E1718 / DAT_00B72C00;

// FUNCTION: 0x007700F0 $E27
double DAT_00B72BE0 = DAT_007E1720 * DAT_007F7348;

// FUNCTION: 0x00770110 $E30
double DAT_00B72BB8 = DAT_007E1728 * DAT_007F7348;

// FUNCTION: 0x00770130 $E33
double DAT_00B72B90 = DAT_007E1730 * DAT_007F7348;

// FUNCTION: 0x00770150 $E36
int DAT_00B72C08 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B72B90 - DAT_00B72BB8) * DAT_00B72BB0));

// FUNCTION: 0x00770190 $E39
double DAT_00B72BC0 = FUN_004CADE0(DAT_00B72C08 * DAT_007E1740);

// FUNCTION: 0x007701C0 $E42
double DAT_00B72BD8 = FUN_004CADE0((DAT_00B72C08 * 2) / DAT_00B72BB0);

// FUNCTION: 0x007701F0 $E45
int DAT_00B72BFC = (int)(DAT_00B72C08 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00770220 $E48
ShortPair DAT_00B72BA8;

// FUNCTION: 0x00770230 $E51
IntTriple DAT_00B72BC8;
