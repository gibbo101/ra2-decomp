// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "wave.h"
#include "_logic.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_zbuffer.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "dsurface.h"
#include "fastmath.h"
#include "globals.h"
#include "goptions.h"
#include "logic.h"
#include "overtype.h"
#include "rules.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tracker.h"
#include "vector.h"
#include "warhead.h"
#include "weapon.h"
#include "zbuffer.h"

FacingType Facing_Between_Points(Point2D const & pt1, Point2D const & pt2);

// FUNCTION: 0x0075F230
FacingType Facing_Between_Points(Point2D const & pt1, Point2D const & pt2)
{
	int xdiff = pt2.X - pt1.X;
	int ydiff = pt1.Y - pt2.Y;

	double tangent = fastmath::sin(DEG_TO_RAD(22.5)) / fastmath::cos(DEG_TO_RAD(22.5));

	if (xdiff != 0) {
		double slope = (double)ydiff / (double)xdiff;

		if (slope < 1.0 / tangent && slope >= tangent) {
			return((xdiff > 0) ? FACING_NE : FACING_SW);
		}

		if (slope < tangent && slope >= -tangent) {
			return((xdiff > 0) ? FACING_E : FACING_W);
		}

		if (slope < -tangent && slope >= -1.0 / tangent) {
			return((xdiff > 0) ? FACING_SE : FACING_NW);
		}
	}
	return((ydiff > 0) ? FACING_N : FACING_S);
}

// FUNCTION: 0x0075F540
Coord Lerp(Coord const & a, Coord const & b, float t)
{
	double inv = 1.0 - t;
	return(Coord(int((a.X * inv) + (b.X * t)),
				 int((a.Y * inv) + (b.Y * t)),
				 int((a.Z * inv) + (b.Z * t))));
}

// FUNCTION: 0x0075F5C0
Point2D Lerp(Point2D const & a, Point2D const & b, float t)
{
	double inv = 1.0 - t;
	int x = int((a.X * inv) + (b.X * t));
	int y = int((a.Y * inv) + (b.Y * t));
	return(Point2D(x,y));
}
