// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "dialog.h"
#include "_font.h"
#include "_surface.h"
#include "data.h"
#include "font.h"
#include "gadget.h"
#include "lightcon.h"
#include "scheme.h"
#include "surface.h"
#include "vector.h"
#include "color.hh"

// FUNCTION: 0x004A67E0
unsigned char * Font_Palette(int color)
{
	static unsigned char _fpalette[16];

	memset(_fpalette, '\0', sizeof(_fpalette));
	memset(&_fpalette[11], color, 5);
	return(_fpalette);
}

extern int DAT_008A0170;
extern int DAT_008A0174;
extern int DAT_008A0178;
extern int DAT_008A017C;

// FUNCTION: 0x004A6820
void FUN_004A6820()
{
	DAT_008A0170 = 0;
	DAT_008A0174 = 0;
	DAT_008A0178 = 0;
	DAT_008A017C = 0;
}
