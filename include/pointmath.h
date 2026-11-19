// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "point.h"

namespace fastmath {
	template<class T> inline double length_squared(TPoint3D<T> const & point)
	{
		return (double)point.X*(double)point.X + (double)point.Y*(double)point.Y + (double)point.Z*(double)point.Z;
	}
}
