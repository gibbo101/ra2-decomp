// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "goptions.h"
#include "_keyboar.h"
#include "_map.h"
#include "data.h"
#include "dbgprint.h"
#include "gamedlg.h"
#include "language\language.h"
#include "loaddlg.h"
#include "ownrdraw.h"
#include "queue.h"
#include "restate.h"
#include "scenario.h"
#include "wonline.h"
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
extern const double DAT_007EA5E0;
extern const double DAT_007EA5E8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x004F0EF0 $E70
double DAT_00A8EE00 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004F0F20 $E73
double DAT_00A8EE70 = DAT_00A8EE00 / DAT_007EA5E0;

// FUNCTION: 0x004F0F40 $E76
double DAT_00A8EE58 = DAT_007E1718 / DAT_00A8EE70;

// FUNCTION: 0x004F0F60 $E79
double DAT_00A8EE50 = DAT_007E1720 * DAT_007EA5E8;

// FUNCTION: 0x004F0F80 $E82
double DAT_00A8EE08 = DAT_007E1728 * DAT_007EA5E8;

// FUNCTION: 0x004F0FA0 $E85
double DAT_00A8EDE0 = DAT_007E1730 * DAT_007EA5E8;

// FUNCTION: 0x004F0FC0 $E88
int DAT_00A8EE78 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00A8EDE0 - DAT_00A8EE08) * DAT_00A8EE00));

// FUNCTION: 0x004F1000 $E91
double DAT_00A8EE10 = FUN_004CADE0(DAT_00A8EE78 * DAT_007E1740);

// FUNCTION: 0x004F1030 $E94
double DAT_00A8EE28 = FUN_004CADE0((DAT_00A8EE78 * 2) / DAT_00A8EE00);

// FUNCTION: 0x004F1060 $E97
int DAT_00A8EE6C = (int)(DAT_00A8EE78 * 4 + DAT_007E1738);

// FUNCTION: 0x004F1090 $E100
ShortPair DAT_00A8EDFC;

// FUNCTION: 0x004F10A0 $E103
IntTriple DAT_00A8EE18;
