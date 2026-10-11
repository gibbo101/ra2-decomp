// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "particle.h"
#include "_alpha.h"
#include "_convert.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_zbuffer.h"
#include "abuffer.h"
#include "building.h"
#include "cell.h"
#include "convert.h"
#include "draw.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "inline.h"
#include "mouse.h"
#include "partsys.h"
#include "psystype.h"
#include "ptype.h"
#include "rgb.h"
#include "rules.h"
#include "scenario.h"
#include "sun.h"
#include "surface.h"
#include "swizzle.h"
#include "tactical.h"
#include "tracker.h"
#include "unit.h"
#include "wave.h"
#include "zbuffer.h"
#include "draw.hh"

// FUNCTION: 0x0062D780
int Approximate_Distance(Point2D & pt)
{
	return((int)(abs(pt.X) + abs(pt.Y)) >> 1);
}
