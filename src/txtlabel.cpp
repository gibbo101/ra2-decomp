// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "txtlabel.h"
#include "_surface.h"
#include "dialog.h"
#include "dsurface.h"
#include "scheme.h"
#include "vector.h"
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
extern const double DAT_007F5B30;
extern const double DAT_007F5B38;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x0072A250 SYMBOL
// _$E28
double DAT_00B0F978 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0072A280 SYMBOL
// _$E31
double DAT_00B0F9C8 = DAT_00B0F978 / DAT_007F5B30;

// SYNTHETIC: GAMEMD 0x0072A2A0 SYMBOL
// _$E34
double DAT_00B0F9B0 = DAT_007E1718 / DAT_00B0F9C8;

// SYNTHETIC: GAMEMD 0x0072A2C0 SYMBOL
// _$E37
double DAT_00B0F9A8 = DAT_007E1720 * DAT_007F5B38;

// SYNTHETIC: GAMEMD 0x0072A2E0 SYMBOL
// _$E40
double DAT_00B0F980 = DAT_007E1728 * DAT_007F5B38;

// SYNTHETIC: GAMEMD 0x0072A300 SYMBOL
// _$E43
double DAT_00B0F958 = DAT_007E1730 * DAT_007F5B38;

// SYNTHETIC: GAMEMD 0x0072A320 SYMBOL
// _$E46
int DAT_00B0F9D0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0F958 - DAT_00B0F980) * DAT_00B0F978));

// SYNTHETIC: GAMEMD 0x0072A360 SYMBOL
// _$E49
double DAT_00B0F988 = FUN_004CADE0(DAT_00B0F9D0 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0072A390 SYMBOL
// _$E52
double DAT_00B0F9A0 = FUN_004CADE0((DAT_00B0F9D0 * 2) / DAT_00B0F978);

// SYNTHETIC: GAMEMD 0x0072A3C0 SYMBOL
// _$E55
int DAT_00B0F9C4 = (int)(DAT_00B0F9D0 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0072A3F0 SYMBOL
// _$E58
ShortPair DAT_00B0F970;

// SYNTHETIC: GAMEMD 0x0072A400 SYMBOL
// _$E61
IntTriple DAT_00B0F990;

struct IntTriple2 {
	int A, B, C;
	IntTriple2() { C = 0; A = 128; B = 128; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// SYNTHETIC: GAMEMD 0x0072A420 SYMBOL
// _$E64
IntTriple2 DAT_00B0F9B8;

// FUNCTION: GAMEMD 0x0072A440
TextLabelClass::TextLabelClass(wchar_t * txt, int x, int y, int color, TextPrintType style) :
	GadgetClass(x, y, 1, 1, 0, 0),
	Text(txt),
	UserData1(0),
	UserData2(0),
	Style(style),
	Color(color),
	PixWidth(-1),
	AnimDword3C(0),
	SkipDraw(false),
	Animate(false),
	AnimPos(0),
	AnimTiming(0)
{
}

// FUNCTION: GAMEMD 0x0072A660
// TextLabelClass::Set_Text

// SYNTHETIC: GAMEMD 0x0072A670
// ??_GTextLabelClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0072A690 SYMBOL
// _$E67
IntQuad DAT_00B0FA08;
