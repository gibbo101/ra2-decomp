// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "statbtn.h"
#include "_surface.h"
#include "blit.h"
#include "dialog.h"
#include "font.h"
#include "scheme.h"
#include "surface.h"
#include "vector.h"
#include "color.hh"

// FUNCTION: 0x006C6610
StaticButtonClass::~StaticButtonClass(void)
{
	if (String != NULL) {
		delete [] String;
	}
}
