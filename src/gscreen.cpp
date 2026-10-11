// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "gscreen.h"
#include "_bench.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "_tooltip.h"
#include "_xmouse.h"
#include "bench.h"
#include "cctooltip.h"
#include "gadget.h"
#include "goptions.h"
#include "keyboard.h"
#include "session.h"
#include "surface.h"
#include "tactical.h"
#include "bench.hh"

// FUNCTION: 0x004F4220
GScreenClass::GScreenClass(void) :
	ScreenX(0),
	ScreenY(0),
	DrawFlags(GS_REDRAW_ALL)
{
}

// FUNCTION: 0x004F42B0
void GScreenClass::Init(TheaterType theater)
{
	Init_Clear();
	Init_IO();
}

// FUNCTION: 0x004F42D0
void GScreenClass::Init_Clear(void)
{
	DrawFlags = GS_REDRAW_ALL;
}
