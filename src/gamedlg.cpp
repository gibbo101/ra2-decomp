// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "gamedlg.h"

#include "_map.h"
#include "_tooltip.h"
#include "cctooltip.h"
#include "data.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "globals.h"
#include "init.h"
#include "language\language.h"
#include "ownrdraw.h"
#include "queue.h"
#include "session.h"
#include "techno.h"

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
extern const double DAT_007E9370;
extern const double DAT_007E9378;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x004E1B10 $E84
double DAT_008B3EB8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004E1B40 $E87
double DAT_008B3F08 = DAT_008B3EB8 / DAT_007E9370;

// FUNCTION: 0x004E1B60 $E90
double DAT_008B3EF0 = DAT_007E1718 / DAT_008B3F08;

// FUNCTION: 0x004E1B80 $E93
double DAT_008B3EE8 = DAT_007E1720 * DAT_007E9378;

// FUNCTION: 0x004E1BA0 $E96
double DAT_008B3EC0 = DAT_007E1728 * DAT_007E9378;

// FUNCTION: 0x004E1BC0 $E99
double DAT_008B3E98 = DAT_007E1730 * DAT_007E9378;

// FUNCTION: 0x004E1BE0 $E102
int DAT_008B3F10 = (int)(fastmath::tan(DAT_008B3E98 - DAT_008B3EC0) * DAT_007E1738 * DAT_008B3EB8);

// FUNCTION: 0x004E1C20 $E105
double DAT_008B3EC8 = fastmath::atan(DAT_008B3F10 * DAT_007E1740);

// FUNCTION: 0x004E1C50 $E108
double DAT_008B3EE0 = fastmath::atan((DAT_008B3F10 * 2) / DAT_008B3EB8);

// FUNCTION: 0x004E1C80 $E111
int DAT_008B3F04 = (int)(DAT_008B3F10 * 4 + DAT_007E1738);

// FUNCTION: 0x004E1CB0 $E114
ShortPair DAT_008B3EB0;

// FUNCTION: 0x004E1CC0 $E117
IntTriple DAT_008B3ED0;
