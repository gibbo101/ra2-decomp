// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#pragma once

#include "face.h"
#include "point.h"

inline Point2D Move_Point(Point2D const & start, DirType const & dir, int distance)
{
	double radians = dir.As_Radian();
	return(Point2D(start.X + fastmath::cos(radians) * distance,
	               start.Y - fastmath::sin(radians) * distance));
}
