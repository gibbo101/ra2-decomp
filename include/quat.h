// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "noinit.h"
#include "vector3.h"

class Matrix3D;

class Quaternion
{
	public:

		Quaternion(const Vector3 & axis,float angle);
		Quaternion(NoInitClass const & x) {}

		Quaternion(float x = 0.0, float y = 0.0, float z = 0.0, float w = 1.0);
		Quaternion operator=(const Quaternion & q);

		void Set(float a = 0.0, float b = 0.0, float c = 0.0, float d = 1.0);
		void Make_Identity(void) { Set(); };

		void Scale(float s);

		float & operator [](int i);
		const float operator [](int i) const;

		Quaternion operator-() const { return(Quaternion(-X,-Y,-Z,-W)); }

		float Length(void) const { return((float)fastmath::sqrt(Length2())); }
		float Length2(void) const { return(X * X + Y * Y + Z * Z + W * W); }

		Quaternion & Make_Closest(const Quaternion & q);

		void Normalize(void);

		void Invert(void);

	public:
		float X;
		float Y;
		float Z;
		float W;
};

inline Quaternion::Quaternion(const Vector3 & axis,float angle)
{
	float s = fastmath::sin(angle/2);
	float c = fastmath::cos(angle/2);
	X = s * axis[0];
	Y = s * axis[1];
	Z = s * axis[2];
	W = c;
}

Quaternion operator * (const Quaternion & a,const Quaternion & b);
Quaternion operator / (const Quaternion & a,const Quaternion & b);

Quaternion Inverse(const Quaternion & q);
Quaternion Conjugate(const Quaternion & q);

Quaternion Axis_To_Quat(const Vector3 &a, float angle);

Quaternion Trackball(float x0, float y0, float x1, float y1, float size);

Quaternion Slerp(const Quaternion &a, const Quaternion &b, float alpha);

Quaternion Build_Quaternion(Matrix3D const &);

Matrix3D Build_Matrix3D(Quaternion const &);

inline void Quaternion::Invert(void)
{
	X = -X;
	Y = -Y;
	Z = -Z;
	W = -W;
}
