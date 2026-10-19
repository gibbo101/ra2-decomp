// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "worlddom.h"

#include "_mixfile.h"
#include "_pk.h"
#include "addon.h"
#include "grphmenu.h"
#include "language\language.h"
#include "mapgen.h"
#include "mixfile.h"
#include "ownrdraw.h"
#include "wdtnet.h"
#include "wonline.h"

#include <cmath>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F7908;
extern const double DAT_007F7910;

// FUNCTION: 0x007AF0B0 $E35
double DAT_00B77EE8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x007AF0E0 $E38
double DAT_00B77F88 = DAT_00B77EE8 / DAT_007F7908;

// FUNCTION: 0x007AF100 $E41
double DAT_00B77F40 = DAT_007E1718 / DAT_00B77F88;

// FUNCTION: 0x007AF120 $E44
double DAT_00B77F38 = DAT_007E1720 * DAT_007F7910;

// FUNCTION: 0x007AF140 $E47
double DAT_00B77EF8 = DAT_007E1728 * DAT_007F7910;

// FUNCTION: 0x007AF160 $E50
double DAT_00B77EB8 = DAT_007E1730 * DAT_007F7910;

// FUNCTION: 0x007AF180 $E53
int DAT_00B77F90 = (int)(DAT_007E1738 * (fastmath::tan(DAT_00B77EB8 - DAT_00B77EF8) * DAT_00B77EE8));

// FUNCTION: 0x007AF1C0 $E56
double DAT_00B77F00 = fastmath::atan(DAT_00B77F90 * DAT_007E1740);

// FUNCTION: 0x007AF1F0 $E59
double DAT_00B77F28 = fastmath::atan((DAT_00B77F90 * 2) / DAT_00B77EE8);

// FUNCTION: 0x007AF220 $E62
int DAT_00B77F80 = (int)(DAT_00B77F90 * 4 + DAT_007E1738);

struct WDTShortPair { short a; short b; WDTShortPair(void) { a = 0; b = 0; } };
struct WDTIntTriple { int a; int b; int c; WDTIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x007AF250 $E65
WDTShortPair DAT_00B77EE0;

// FUNCTION: 0x007AF260 $E68
WDTIntTriple DAT_00B77F10;
