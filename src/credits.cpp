// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "credits.h"
#include "_bench.h"
#include "_map.h"
#include "_rules.h"
#include "_surface.h"
#include "bench.h"
#include "bound.h"
#include "dialog.h"
#include "dsurface.h"
#include "globals.h"
#include "house.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "tab.h"
#include "voc.h"
#include "vox.h"
#include "bench.hh"
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
extern const double DAT_007E5D80;
extern const double DAT_007E5D88;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntTripleB {
	int A, B, C;
	IntTripleB() { C = 0; A = 128; B = 128; }
};


// SYNTHETIC: GAMEMD 0x004A2160 SYMBOL
// _$E73
double DAT_0089F900 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x004A2190 SYMBOL
// _$E76
double DAT_0089F950 = DAT_0089F900 / DAT_007E5D80;

// SYNTHETIC: GAMEMD 0x004A21B0 SYMBOL
// _$E79
double DAT_0089F938 = DAT_007E1718 / DAT_0089F950;

// SYNTHETIC: GAMEMD 0x004A21D0 SYMBOL
// _$E82
double DAT_0089F930 = DAT_007E1720 * DAT_007E5D88;

// SYNTHETIC: GAMEMD 0x004A21F0 SYMBOL
// _$E85
double DAT_0089F908 = DAT_007E1728 * DAT_007E5D88;

// SYNTHETIC: GAMEMD 0x004A2210 SYMBOL
// _$E88
double DAT_0089F8E0 = DAT_007E1730 * DAT_007E5D88;

// SYNTHETIC: GAMEMD 0x004A2230 SYMBOL
// _$E91
int DAT_0089F958 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089F8E0 - DAT_0089F908) * DAT_0089F900));

// SYNTHETIC: GAMEMD 0x004A2270 SYMBOL
// _$E94
double DAT_0089F910 = FUN_004CADE0(DAT_0089F958 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x004A22A0 SYMBOL
// _$E97
double DAT_0089F928 = FUN_004CADE0((DAT_0089F958 * 2) / DAT_0089F900);

// SYNTHETIC: GAMEMD 0x004A22D0 SYMBOL
// _$E100
int DAT_0089F94C = (int)(DAT_0089F958 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x004A2300 SYMBOL
// _$E103
ShortPair DAT_0089F8F8;

// SYNTHETIC: GAMEMD 0x004A2310 SYMBOL
// _$E106
IntTriple DAT_0089F918;

// SYNTHETIC: GAMEMD 0x004A2330 SYMBOL
// _$E109
IntTripleB DAT_0089F940;

// FUNCTION: GAMEMD 0x004A2350
CreditClass::CreditClass(void) :
	Credits(0),
	Current(0),
	IsToRedraw(false),
	IsUp(false),
	IsAudible(false),
	Countdown(0)
{
}
