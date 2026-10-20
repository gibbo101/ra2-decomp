// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mainloop.h"
#include "_bench.h"
#include "_command.h"
#include "_logic.h"
#include "_map.h"
#include "_palette.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_timer.h"
#include "_xmouse.h"
#include "bench.h"
#include "command.h"
#include "conquer.h"
#include "data.h"
#include "debug.h"
#include "dialog.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "internet.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "logic.h"
#include "misc.h"
#include "mpscore.h"
#include "msgbox.h"
#include "msgloop.h"
#include "mstimer.h"
#include "netdlg.h"
#include "nullmgr.h"
#include "pcx.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "stats.h"
#include "stimer.h"
#include "surface.h"
#include "tactical.h"
#include "theme.h"
#include "timer.h"
#include "tracker.h"
#include "wonline.h"
#include "bench.hh"
#include "special.hh"

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
extern const double DAT_007ED3A0;
extern const double DAT_007ED3A8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0055CA00 $E106
double DAT_00ABCD80 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0055CA30 $E109
double DAT_00ABCDF0 = DAT_00ABCD80 / DAT_007ED3A0;

// FUNCTION: 0x0055CA50 $E112
double DAT_00ABCDD0 = DAT_007E1718 / DAT_00ABCDF0;

// FUNCTION: 0x0055CA70 $E115
double DAT_00ABCDC8 = DAT_007E1720 * DAT_007ED3A8;

// FUNCTION: 0x0055CA90 $E118
double DAT_00ABCD98 = DAT_007E1728 * DAT_007ED3A8;

// FUNCTION: 0x0055CAB0 $E121
double DAT_00ABCD60 = DAT_007E1730 * DAT_007ED3A8;

// FUNCTION: 0x0055CAD0 $E124
int DAT_00ABCDF8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABCD60 - DAT_00ABCD98) * DAT_00ABCD80));

// FUNCTION: 0x0055CB10 $E127
double DAT_00ABCDA0 = FUN_004CADE0(DAT_00ABCDF8 * DAT_007E1740);

// FUNCTION: 0x0055CB40 $E130
double DAT_00ABCDC0 = FUN_004CADE0((DAT_00ABCDF8 * 2) / DAT_00ABCD80);

// FUNCTION: 0x0055CB70 $E133
int DAT_00ABCDEC = (int)(DAT_00ABCDF8 * 4 + DAT_007E1738);

// FUNCTION: 0x0055CBA0 $E136
ShortPair DAT_00ABCD78;

// FUNCTION: 0x0055CBB0 $E139
IntTriple DAT_00ABCDB0;
