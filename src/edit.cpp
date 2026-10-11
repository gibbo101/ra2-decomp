// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "edit.h"
#include "_keyboar.h"
#include "_surface.h"
#include "dialog.h"
#include "dsurface.h"
#include "font.h"
#include "globals.h"
#include "keyboard.h"
#include "scheme.h"
#include "vector.h"
#include "color.hh"

// FUNCTION: 0x004C30B0
EditClass::~EditClass(void)
{
	if (Has_Focus()) {
		Clear_Focus();
	}
}

// FUNCTION: 0x004C32A0
void EditClass::Draw_Background(void)
{
	Draw_Box (Rect(X, Y, Width, Height), BOXSTYLE_BOX, true);
}
