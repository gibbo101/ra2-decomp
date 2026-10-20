// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "scroll.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "bound.h"
#include "building.h"
#include "cell.h"
#include "dbgprint.h"
#include "goptions.h"
#include "house.h"
#include "incdec.h"
#include "init.h"
#include "inline.h"
#include "misc.h"
#include "overtype.h"
#include "rules.h"
#include "suprtype.h"
#include "surface.h"
#include "tactical.h"
#include "waypoint.h"
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
extern const double DAT_007F1078;
extern const double DAT_007F1080;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00692050 $E81
double DAT_00B05660 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00692080 $E84
double DAT_00B056B8 = DAT_00B05660 / DAT_007F1078;

// FUNCTION: 0x006920A0 $E87
double DAT_00B056A0 = DAT_007E1718 / DAT_00B056B8;

// FUNCTION: 0x006920C0 $E90
double DAT_00B05698 = DAT_007E1720 * DAT_007F1080;

// FUNCTION: 0x006920E0 $E93
double DAT_00B05668 = DAT_007E1728 * DAT_007F1080;

// FUNCTION: 0x00692100 $E96
double DAT_00B05630 = DAT_007E1730 * DAT_007F1080;

// FUNCTION: 0x00692120 $E99
int DAT_00B056C0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B05630 - DAT_00B05668) * DAT_00B05660));

// FUNCTION: 0x00692160 $E102
double DAT_00B05670 = FUN_004CADE0(DAT_00B056C0 * DAT_007E1740);

// FUNCTION: 0x00692190 $E105
double DAT_00B05688 = FUN_004CADE0((DAT_00B056C0 * 2) / DAT_00B05660);

// FUNCTION: 0x006921C0 $E108
int DAT_00B056B4 = (int)(DAT_00B056C0 * 4 + DAT_007E1738);

// FUNCTION: 0x006921F0 $E111
ShortPair DAT_00B05658;

// FUNCTION: 0x00692200 $E114
IntTriple DAT_00B05678;

// FUNCTION: 0x006922E0
void ScrollClass::AI(KeyNumType &input, Point2D const & xy)
{
	Scroll_AI();
	BASECLASS::AI(input, xy);
}
