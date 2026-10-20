// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "mainopt.h"

#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_surface.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "gamedlg.h"
#include "globals.h"
#include "init.h"
#include "language\language.h"
#include "misc.h"
#include "mixfile.h"
#include "msgbox.h"
#include "newmenu.h"
#include "ownrdraw.h"
#include "sidebar.h"
#include "sounddlg.h"
#include "stimer.h"
#include "surface.h"
#include "wspipx.h"
#include "wwmouse.h"

#include "color.hh"

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
extern const double DAT_007ED3B0;
extern const double DAT_007ED3B8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0055F780 $E65
double DAT_00ABCE40 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// FUNCTION: 0x0055F7B0 $E68
double DAT_00ABCF48 = DAT_00ABCE40 / DAT_007ED3B0;

// FUNCTION: 0x0055F7D0 $E71
double DAT_00ABCF30 = DAT_007E1718 / DAT_00ABCF48;

// FUNCTION: 0x0055F7F0 $E74
double DAT_00ABCF28 = DAT_007E1720 * DAT_007ED3B8;

// FUNCTION: 0x0055F810 $E77
double DAT_00ABCE48 = DAT_007E1728 * DAT_007ED3B8;

// FUNCTION: 0x0055F830 $E80
double DAT_00ABCE20 = DAT_007E1730 * DAT_007ED3B8;

// FUNCTION: 0x0055F850 $E83
int DAT_00ABCF50 = (int)(FUN_004CAD50(DAT_00ABCE20 - DAT_00ABCE48) * DAT_007E1738 * DAT_00ABCE40);

// FUNCTION: 0x0055F890 $E86
double DAT_00ABCE50 = FUN_004CADE0(DAT_00ABCF50 * DAT_007E1740);

// FUNCTION: 0x0055F8C0 $E89
double DAT_00ABCE68 = FUN_004CADE0((DAT_00ABCF50 * 2) / DAT_00ABCE40);

// FUNCTION: 0x0055F8F0 $E92
int DAT_00ABCF44 = (int)(DAT_00ABCF50 * 4 + DAT_007E1738);

// FUNCTION: 0x0055F920 $E95
ShortPair DAT_00ABCE38;

// FUNCTION: 0x0055F930 $E98
IntTriple DAT_00ABCE58;
