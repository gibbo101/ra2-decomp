// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "scroll.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "bound.h"
#include "building.h"
#include "cell.h"
#include "dbgprint.h"
#include "goptions.h"
#include "house.h"
#include "incdec.h"
#include "init.h"
#include "inline.h"
#include "misc.h"
#include "overtype.h"
#include "rules.h"
#include "suprtype.h"
#include "surface.h"
#include "tactical.h"
#include "waypoint.h"
#include "special.hh"

// FUNCTION: 0x006922E0
void ScrollClass::AI(KeyNumType &input, Point2D const & xy)
{
	Scroll_AI();
	BASECLASS::AI(input, xy);
}
