// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "comtypes.h"

namespace fastmath { double __cdecl sqrt(double x); }

template<class T> class TRect;

template<class T>
class TPoint2D {
	public:
		TPoint2D(void) {}
		TPoint2D(T x, T y) : X(x), Y(y) {}

		bool operator == (TPoint2D<T> const & rvalue) const;
		bool operator != (TPoint2D<T> const & rvalue) const {return(X!=rvalue.X || Y!=rvalue.Y);}

		TPoint2D<T> const & operator += (TPoint2D<T> const & rvalue) {X += rvalue.X;Y += rvalue.Y;return(*this);}
		TPoint2D<T> const & operator -= (TPoint2D<T> const & rvalue) {X -= rvalue.X;Y -= rvalue.Y;return(*this);}
		TPoint2D<T> const operator - (TPoint2D<T> const & rvalue) const {return(TPoint2D<T>(T(X - rvalue.X), T(Y - rvalue.Y)));}
		TPoint2D<T> const operator + (TPoint2D<T> const & rvalue) const {return(TPoint2D<T>(T(X + rvalue.X), T(Y + rvalue.Y)));}

		TPoint2D<T> const operator * (T rvalue) const {return(TPoint2D<T>(T(X * rvalue), T(Y * rvalue)));}
		TPoint2D<T> const & operator *= (T rvalue) {X *= rvalue; Y *= rvalue;return(*this);}
		TPoint2D<T> const operator / (T rvalue) const {if (rvalue == T(0)) return(TPoint2D<T>(0,0));return(TPoint2D<T>(T(X / rvalue), T(Y / rvalue)));}
		TPoint2D<T> const & operator /= (T rvalue) {if (rvalue != T(0)) {X /= rvalue;Y /= rvalue;}return(*this);}

		TPoint2D<T> const operator * (TPoint2D<T> const & rvalue) const {return(TPoint2D<T>(T(X * rvalue.X), T(Y * rvalue.Y)));}
		T Dot_Product(TPoint2D<T> const & rvalue) const {return((T(X * rvalue.X + Y * rvalue.Y)));}
		TPoint2D<T> const Cross_Product(TPoint2D<T> const & rvalue) const {return(TPoint2D<T>(T(Y - rvalue.Y), T(rvalue.X - X)));}

		TPoint2D<T> const operator - (void) const {return(TPoint2D<T>(T(-X), T(-Y)));}

		T Length(void) const {return(T(fastmath::sqrt((double)X*(double)X + (double)Y*(double)Y)));}
		TPoint2D<T> const Normalize(void) const {
			double len = fastmath::sqrt(X*X + Y*Y);
			if (len != 0.0) {
				return(TPoint2D<T>((T)(X / len), (T)(Y / len)));
			} else {
				return(*this);
			}
		}

		T Distance_To(TPoint2D<T> const & point) const {return((*this - point).Length());}

	public:
		T X;
		T Y;
};

class Point2D : public TPoint2D<int>
{
	public:
		Point2D(void) {}
		Point2D(int x, int y) : TPoint2D<int>(x, y) {}
		Point2D(Point2DStruct const & rvalue) : TPoint2D<int>(rvalue.X, rvalue.Y) {}
		Point2D(TPoint2D<int> const & rvalue) : TPoint2D<int>(rvalue) {}

		operator Point2DStruct (void) const {Point2DStruct pt;pt.X = X;pt.Y = Y;return(pt);}

		bool operator == (Point2D const & rvalue) const {return(X==rvalue.X && Y==rvalue.Y);}

		Point2D const & operator += (Point2D const & rvalue) {X += rvalue.X;Y += rvalue.Y;return(*this);}
		Point2D const & operator -= (Point2D const & rvalue) {X -= rvalue.X;Y -= rvalue.Y;return(*this);}
		Point2D const operator - (Point2D const & rvalue) const {return(Point2D(int(X - rvalue.X), int(Y - rvalue.Y)));}
		Point2D const operator + (Point2D const & rvalue) const {return(Point2D(int(X + rvalue.X), int(Y + rvalue.Y)));}
};

template<class T>
T Distance(TPoint2D<T> const & point1, TPoint2D<T> const & point2)
{
	return((point1 - point2).Length());
}

template<class T>
TPoint2D<T> const Cross_Product(TPoint2D<T> const & lvalue, TPoint2D<T> const & rvalue)
{
	return(lvalue.Cross_Product(rvalue));
}

template<class T>
class TPoint3D : public TPoint2D<T> {
		typedef TPoint2D<T> BASECLASS;

	public:
		using BASECLASS::X;
		using BASECLASS::Y;

		TPoint3D(void) {}
		TPoint3D(T x, T y, T z) : BASECLASS(x, y), Z(z) {}
		TPoint3D(BASECLASS const & rvalue, T z ) : BASECLASS(rvalue), Z(z) {}

		bool operator == (TPoint3D<T> const & rvalue) const {return(X==rvalue.X && Y==rvalue.Y && Z==rvalue.Z);}
		bool operator != (TPoint3D<T> const & rvalue) const {return(X!=rvalue.X || Y!=rvalue.Y || Z!=rvalue.Z);}

		TPoint3D<T> const & operator += (TPoint3D<T> const & rvalue) {X += rvalue.X;Y += rvalue.Y;Z += rvalue.Z;return(*this);}
		TPoint2D<T> const & operator += (TPoint2D<T> const & rvalue) {BASECLASS::operator += (rvalue);return(*this);}
		TPoint3D<T> const & operator -= (TPoint3D<T> const & rvalue) {X -= rvalue.X;Y -= rvalue.Y;Z -= rvalue.Z;return(*this);}
		TPoint2D<T> const & operator -= (TPoint2D<T> const & rvalue) {BASECLASS::operator -= (rvalue);return(*this);}
		TPoint3D<T> const operator - (TPoint3D<T> const & rvalue) const {return(TPoint3D<T>(X - rvalue.X, Y - rvalue.Y, Z - rvalue.Z));}
		TPoint3D<T> const operator - (TPoint2D<T> const & rvalue) const {return(TPoint3D<T>(X - rvalue.X, Y - rvalue.Y, Z));}
		TPoint3D<T> const operator + (TPoint3D<T> const & rvalue) const {return(TPoint3D<T>(X + rvalue.X, Y + rvalue.Y, Z + rvalue.Z));}
		TPoint3D<T> const operator + (TPoint2D<T> const & rvalue) const {return(TPoint3D<T>(X + rvalue.X, Y + rvalue.Y, Z));}

		TPoint3D<T> const operator * (T rvalue) const {return(TPoint3D<T>(X * rvalue, Y * rvalue, Z * rvalue));}
		TPoint3D<T> const & operator *= (T rvalue) {X *= rvalue;Y *= rvalue;Z *= rvalue;return(*this);}
		TPoint3D<T> const operator / (T rvalue) const {if (rvalue == T(0)) return(TPoint3D<T>(0,0,0));return(TPoint3D<T>(X / rvalue, Y / rvalue, Z / rvalue));}
		TPoint3D<T> const & operator /= (T rvalue) {if (rvalue != T(0)) {X /= rvalue;Y /= rvalue;Z /= rvalue;}return(*this);}

		TPoint3D<T> const operator * (TPoint3D<T> const & rvalue) const {return(TPoint3D<T>(X * rvalue.X, Y * rvalue.Y, Z * rvalue.Z));}
		T Dot_Product(TPoint3D<T> const & rvalue) const {return(T(X * rvalue.X + Y * rvalue.Y + Z * rvalue.Z));}
		TPoint3D<T> const Cross_Product(TPoint3D<T> const & rvalue) const {return(TPoint3D<T>(Y * rvalue.Z - Z * rvalue.Y, Z * rvalue.X - X * rvalue.Z, X * rvalue.Y - Y * rvalue.X));}

		TPoint3D<T> const operator - (void) const {return(TPoint3D<T>(-X, -Y, -Z));}

		T Length(void) const {return(T(fastmath::sqrt((double)X*(double)X + (double)Y*(double)Y + (double)Z*(double)Z)));}
		TPoint3D<T> const Normalize(void) const {
			double len = fastmath::sqrt(X*X + Y*Y + Z*Z);
			if (len != 0.0) {
				return(TPoint3D<T>((T)(X / len), (T)(Y / len), (T)(Z / len)));
			} else {
				return(*this);
			}
		}

		T Distance_To(TPoint3D<T> const & point) const {return((*this - point).Length());}
		T Distance_To(TPoint2D<T> const & point) const {return(BASECLASS::Distance_To(point));}

	public:

		T Z;
};

typedef TPoint3D<int> Point3D;

template<class T>
TPoint3D<T> const Cross_Product(TPoint3D<T> const & lvalue, TPoint3D<T> const & rvalue)
{
	return(lvalue.Cross_Product(rvalue));
}
