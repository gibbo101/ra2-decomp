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

// FUNCTION: 0x00645D10
const float Quaternion::operator [](int i) const
{
	return (&X)[i];
}

// FUNCTION: 0x00645D20
Quaternion Quaternion::operator=(const Quaternion & q)
{
	X = q[0];
	Y = q[1];
	Z = q[2];
	W = q[3];

	return(*this);
}

// FUNCTION: 0x00646980
Matrix3D Build_Matrix3D(const Quaternion &q)
{
	Matrix3D m;

	m[0].X = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
	m[0].Y = 2.0f * (q[0] * q[1] - q[2] * q[3]);
	m[0].Z = 2.0f * (q[2] * q[0] + q[1] * q[3]);

	m[1].X = 2.0f * (q[0] * q[1] + q[2] * q[3]);
	m[1].Y = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
	m[1].Z = 2.0f * (q[1] * q[2] - q[0] * q[3]);

	m[2].X = 2.0f * (q[2] * q[0] - q[1] * q[3]);
	m[2].Y = 2.0f * (q[1] * q[2] + q[0] * q[3]);
	m[2].Z = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);

	m[0].W = m[1].W = m[2].W = 0.0f;

	return(m);
}
