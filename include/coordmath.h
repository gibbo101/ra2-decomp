// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "coord.h"

// Convert map-coordinate differences to a Cartesian displacement (reversed map Y).
template<class T>
inline TPoint3D<T> Coordinate_Displacement(TPoint2D<int> const & destination, TPoint2D<int> const & source, T height_difference)
{
	TPoint3D<T> result;
	result.X = destination.X - source.X;
	result.Y = source.Y - destination.Y;
	result.Z = height_difference;
	return result;
}
