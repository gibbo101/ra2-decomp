// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "quat.h"
#include "fastmath.h"
#include "matrix3d.h"
#define SLERP_EPSILON 1e-5

// FUNCTION: 0x00645C50
void Quaternion::Set(float x, float y, float z, float w)
{
	X=x;
	Y=y;
	Z=z;
	W=w;
}

// FUNCTION: 0x00645C70
void Quaternion::Normalize(void)
{
	float len2 = X * X + Y * Y + Z * Z + W * W;

	if (0.0f != len2) {
		X /= len2;
		Y /= len2;
		Z /= len2;
		W /= len2;
	}
}

// FUNCTION: 0x00645CD0
void Quaternion::Scale(float s)
{
	X = (float)(s * X);
	Y = (float)(s * Y);
	Z = (float)(s * Z);
	W = (float)(s * W);
}

// FUNCTION: 0x00645D00
float & Quaternion::operator [](int i)
{
	return (&X)[i];
}
