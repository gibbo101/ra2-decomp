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

// FUNCTION: 0x004A2350
CreditClass::CreditClass(void) :
	Credits(0),
	Current(0),
	IsToRedraw(false),
	IsUp(false),
	IsAudible(false),
	Countdown(0)
{
}
