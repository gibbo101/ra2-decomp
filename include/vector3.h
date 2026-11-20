// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "assert.h"
#include "fastmath.h"
#include "visualc.h"

class Vector3
{
public:
	float X;
	float Y;
	float Z;

	inline Vector3(void) {};
	inline Vector3(float x, float y, float z) { X = x; Y = y; Z = z; }

	inline void	Set(float x, float y, float z) { X = x; Y = y; Z = z; }
	inline void	Set(const Vector3 & that) { X = that.X; Y = that.Y; Z = that.Z; }



	float & operator [](int i) { return((&X)[i]); }
	const float operator [](int i) const { return((&X)[i]); }

	void Normalize(void);
	inline float Length(void) const;
	inline float Length2(void) const;

	inline Vector3 operator-() const { return(Vector3(-X,-Y,-Z)); }
	inline Vector3 operator+() const { return(*this); }

	inline Vector3 & operator += (const Vector3 & v) { X += v.X; Y += v.Y; Z += v.Z; return(*this); }
	inline Vector3 & operator -= (const Vector3 & v) { X -= v.X; Y -= v.Y; Z -= v.Z; return(*this); }
	inline Vector3 & operator *= (float k) { X=X*k; Y=Y*k; Z=Z*k; return(*this); }
	inline Vector3 & operator /= (float k) { X=X/k; Y=Y/k; Z=Z/k; return(*this); }

	inline friend Vector3 operator * (const Vector3 &a,double k);
	inline friend Vector3 operator * (float k,const Vector3 &a);
	inline friend Vector3 operator / (const Vector3 &a,double k);

	inline friend Vector3 operator + (const Vector3 &a,const Vector3 &b);
	inline friend Vector3 operator - (const Vector3 &a,const Vector3 &b);

	static inline Vector3 Cross_Product(const Vector3 &a,const Vector3 &b);
	static inline void Cross_Product(const Vector3 &a,const Vector3 &b,Vector3 * result);
	static inline float Cross_Product_X(const Vector3 &a,const Vector3 &b);
	static inline float Cross_Product_Y(const Vector3 &a,const Vector3 &b);
	static inline float Cross_Product_Z(const Vector3 &a,const Vector3 &b);

};

inline Vector3 operator * (const Vector3 &a,double k)
{
	return(Vector3((a.X * k),(a.Y * k),(a.Z * k)));
}

inline Vector3 operator * (float k, const Vector3 &a)
{
	return(Vector3((a.X * k),(a.Y * k),(a.Z * k)));
}

inline Vector3 operator / (const Vector3 &a,double k)
{
	return(Vector3((a.X / k),(a.Y / k),(a.Z / k)));
}

inline Vector3 operator + (const Vector3 &a,const Vector3 &b)
{
	return(Vector3(
		a.X+b.X,
		a.Y+b.Y,
		a.Z+b.Z)
	);
}

inline Vector3 operator - (const Vector3 &a,const Vector3 &b)
{
	return(Vector3(
		a.X-b.X,
		a.Y-b.Y,
		a.Z-b.Z)
	);
}

inline float operator * (const Vector3 &a,const Vector3 &b)
{
	return	(a.X*b.X +
			 a.Y*b.Y +
			 a.Z*b.Z);
}

inline Vector3 Vector3::Cross_Product(const Vector3 &a,const Vector3 &b)
{
	return(Vector3(
		(a.Y * b.Z - a.Z * b.Y),
		(a.Z * b.X - a.X * b.Z),
		(a.X * b.Y - a.Y * b.X))
	);
}

inline void Vector3::Cross_Product(const Vector3 &a,const Vector3 &b,Vector3 * set_result)
{
	assert(set_result != &a);
	set_result->X = (a.Y * b.Z - a.Z * b.Y);
	set_result->Y = (a.Z * b.X - a.X * b.Z);
	set_result->Z = (a.X * b.Y - a.Y * b.X);
}

inline float Vector3::Cross_Product_X(const Vector3 &a,const Vector3 &b)
{
	return(a.Y * b.Z - a.Z * b.Y);
}

inline float Vector3::Cross_Product_Y(const Vector3 &a,const Vector3 &b)
{
	return(a.Z * b.X - a.X * b.Z);
}

inline float Vector3::Cross_Product_Z(const Vector3 &a,const Vector3 &b)
{
	return(a.X * b.Y - a.Y * b.X);
}

inline void Vector3::Normalize(void)
{
	float len = Length();
	if (len != 0.0) {
		X /= len;
		Y /= len;
		Z /= len;
	}
}

inline Vector3 Normalize(const Vector3 & vec)
{
	float len = vec.Length();
	if (len != 0.0) {
		return(vec / len);
	}
	return(vec);
}

inline float Vector3::Length(void) const
{
	return((float)fastmath::sqrt(X*X + Y*Y + Z*Z));
}

inline float Vector3::Length2(void) const
{
	return(X*X + Y*Y + Z*Z);
}
