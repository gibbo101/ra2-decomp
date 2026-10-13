// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "language\language.h"

#include "_keyboar.h"
#include "_surface.h"
#include "ccfile.h"
#include "convert.h"
#include "data.h"
#include "globals.h"
#include "ini.h"
#include "keyboard.h"
#include "mschoice.h"
#include "msfont.h"
#include "newmenu.h"
#include "netshare.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "session.h"
#include "theme.h"
#include "vector.h"
#include "wdtnet.h"
#include "worlddom.h"

#define IID_DEFINED
#include "wolapi\wolapi.h"

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F72A0;
extern const double DAT_007F72A8;

// FUNCTION: 0x0076C0A0 $E31
double DAT_00B72A30 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0076C0D0 $E34
double DAT_00B72A80 = DAT_00B72A30 / DAT_007F72A0;

// FUNCTION: 0x0076C0F0 $E37
double DAT_00B72A68 = DAT_007E1718 / DAT_00B72A80;

// FUNCTION: 0x0076C110 $E40
double DAT_00B72A60 = DAT_007E1720 * DAT_007F72A8;

// FUNCTION: 0x0076C130 $E43
double DAT_00B72A38 = DAT_007E1728 * DAT_007F72A8;

// FUNCTION: 0x0076C150 $E46
double DAT_00B72A10 = DAT_007E1730 * DAT_007F72A8;

// FUNCTION: 0x0076C170 $E49
int DAT_00B72A88 = (int)(fastmath::tan(DAT_00B72A10 - DAT_00B72A38) * DAT_007E1738 * DAT_00B72A30);

// FUNCTION: 0x0076C1B0 $E52
double DAT_00B72A40 = fastmath::atan(DAT_00B72A88 * DAT_007E1740);

// FUNCTION: 0x0076C1E0 $E55
double DAT_00B72A58 = fastmath::atan((DAT_00B72A88 * 2) / DAT_00B72A30);

// FUNCTION: 0x0076C210 $E58
int DAT_00B72A7C = (int)(DAT_00B72A88 * 4 + DAT_007E1738);

struct WDTShortPair { short a; short b; WDTShortPair(void) { a = 0; b = 0; } };
struct WDTIntTriple { int a; int b; int c; WDTIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x0076C240 $E61
WDTShortPair DAT_00B72A28;

// FUNCTION: 0x0076C250 $E64
WDTIntTriple DAT_00B72A48;
