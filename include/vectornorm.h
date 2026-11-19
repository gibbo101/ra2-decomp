// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once
#include "vector3.h"

inline float Vector3_Length(Vector3 const & vector)
{
	double y = vector.Y;
	double z = vector.Z;
	double x = vector.X;
	return fastmath::sqrt(y * y + (x * x + z * z));
}
