// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"

// FUNCTION: 0x004F4030 $E2
// FUNCTION: 0x004F4060 $E5
// FUNCTION: 0x004F4080 $E8
// FUNCTION: 0x004F40A0 $E11
// FUNCTION: 0x004F40C0 $E14
// FUNCTION: 0x004F40E0 $E17
// FUNCTION: 0x004F4100 $E20
// FUNCTION: 0x004F4140 $E23
// FUNCTION: 0x004F4170 $E26
// FUNCTION: 0x004F41A0 $E29
// FUNCTION: 0x004F41D0 $E32
// FUNCTION: 0x004F41E0 $E35

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

// FUNCTION: 0x004F4240
LONG GScreenClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	if (riid != IID_IUnknown && riid != IID_IGameMap) {
		return(E_NOINTERFACE);
	}

	*ppvObject = reinterpret_cast<IGameMap *>(this);

	reinterpret_cast<IUnknown *>(*ppvObject)->AddRef();
	return(S_OK);
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
