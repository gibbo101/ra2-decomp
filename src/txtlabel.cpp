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
#include "geometry.h"
#include "vector.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x0072A250 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x0072A280 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x0072A2A0 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x0072A2C0 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x0072A2E0 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x0072A300 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x0072A320 SYMBOL
// _$E46
// SYNTHETIC: GAMEMD 0x0072A360 SYMBOL
// _$E49
// SYNTHETIC: GAMEMD 0x0072A390 SYMBOL
// _$E52
// SYNTHETIC: GAMEMD 0x0072A3C0 SYMBOL
// _$E55
// SYNTHETIC: GAMEMD 0x0072A3F0 SYMBOL
// _$E58
// SYNTHETIC: GAMEMD 0x0072A400 SYMBOL
// _$E61
// SYNTHETIC: GAMEMD 0x0072A420 SYMBOL
// _$E64

// SYNTHETIC: GAMEMD 0x0072A690 SYMBOL
// _$E67
const Rect RECT_NONE_2(0, 0, 0, 0);

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

