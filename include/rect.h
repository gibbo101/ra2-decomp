// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "point.h"

#include <cstddef>

template<class T>
class TRect
{
	public:
		TRect(void) {}
		TRect(T x, T y, T w, T h) : X(x), Y(y), Width(w), Height(h) {}
		TRect(TPoint2D<T> const & point, T w, T h) : X(point.X), Y(point.Y), Width(w), Height(h) {}

		bool operator == (TRect<T> const & rvalue) const {return(X==rvalue.X && Y==rvalue.Y && Width==rvalue.Width && Height==rvalue.Height);}
		bool operator != (TRect<T> const & rvalue) const {return(X!=rvalue.X || Y!=rvalue.Y || Width!=rvalue.Width || Height!=rvalue.Height);}

		TRect<T> const & operator += (TPoint2D<T> const & point) {X += point.X;Y += point.Y;return(*this);}
		TRect<T> const & operator -= (TPoint2D<T> const & point) {X -= point.X;Y -= point.Y;return(*this);}
		TRect<T> const operator + (TPoint2D<T> const & point) const {return(TRect<T>(Top_Left() + point, Width, Height));}
		TRect<T> const operator - (TPoint2D<T> const & point) const {return(TRect<T>(Top_Left() - point, Width, Height));}

		TRect<T> const Bias_To(TRect<T> const & rect) const {return(TRect<T>(Top_Left() + rect.Top_Left(), Width, Height));}

		void Set(T x, T y, T w, T h) {X = x; Y = y; Width = w; Height = h;}

		bool Is_Overlapping(TRect<T> const & rect) const {return(X < rect.X+rect.Width && Y < rect.Y+rect.Height && X+Width > rect.X && Y+Height > rect.Y);}

		bool Is_Valid(void) const {return(Width > 0 && Height > 0);}
		__declspec(property(get=Is_Valid)) bool IsValid;

		int Size(void) const {return(int(Width) * int(Height));}

		TPoint2D<T> Top_Left(void) const {return(TPoint2D<T>(X, Y));}
		__declspec(property(get=Top_Left)) TPoint2D<T> TopLeft;

		TPoint2D<T> Top_Right(void) const {return(TPoint2D<T>(T(X + Width - 1), Y));}
		__declspec(property(get=Top_Right)) TPoint2D<T> TopRight;

		TPoint2D<T> Bottom_Left(void) const {return(TPoint2D<T>(X, T(Y + Height - 1)));}
		__declspec(property(get=Bottom_Left)) TPoint2D<T> BottomLeft;

		TPoint2D<T> Bottom_Right(void) const {return(TPoint2D<T>(T(X + Width - 1), T(Y + Height - 1)));}
		__declspec(property(get=Bottom_Right)) TPoint2D<T> BottomRight;

		bool Is_Point_Within(TPoint2D<T> const & point) const {return(point.X >= X && point.X < X+Width && point.Y >= Y && point.Y < Y+Height);}

	public:

		T X;
		T Y;

		T Width;
		T Height;
};

template<class T>
TPoint2D<T> const Bias_To(TPoint2D<T> const & point, TRect<T> const & rect)
{
	return(TPoint2D<T>(T(point.X + rect.X), T(point.Y + rect.Y)));
}

template<class T>
TRect<T> const Union(TRect<T> const & rect1, TRect<T> const & rect2)
{
	if (rect1.Is_Valid()) {
		if (rect2.Is_Valid()) {
			TRect<T> result = rect1;

			if (result.X > rect2.X) {
				result.Width += T(result.X-rect2.X);
				result.X = rect2.X;
			}
			if (result.Y > rect2.Y) {
				result.Height += T(result.Y-rect2.Y);
				result.Y = rect2.Y;
			}
			if (result.X+result.Width < rect2.X+rect2.Width) {
				result.Width = T(((rect2.X+rect2.Width)-result.X)+1);
			}
			if (result.Y+result.Height < rect2.Y+rect2.Height) {
				result.Height = T(((rect2.Y+rect2.Height)-result.Y)+1);
			}
			return(result);
		}
		return(rect1);
	}
	return(rect2);
}

template<class T>
TRect<T> const Intersect(TRect<T> const & bounding_rect, TRect<T> const & draw_rect, T * x, T * y)
{
	TRect<T> bad_rect(0, 0, 0, 0);
	TRect<T> new_draw_rect = draw_rect;

	if (!bounding_rect.Is_Valid() || !draw_rect.Is_Valid()) return(bad_rect);

	if (new_draw_rect.X < bounding_rect.X) {
		new_draw_rect.Width -= T(bounding_rect.X - new_draw_rect.X);
		new_draw_rect.X = bounding_rect.X;
	}
	if (new_draw_rect.Width < 1) return(bad_rect);

	if (new_draw_rect.Y < bounding_rect.Y) {
		new_draw_rect.Height -= T(bounding_rect.Y - new_draw_rect.Y);
		new_draw_rect.Y = bounding_rect.Y;
	}
	if (new_draw_rect.Height < 1) return(bad_rect);

	if (new_draw_rect.X + new_draw_rect.Width > bounding_rect.X + bounding_rect.Width) {
		new_draw_rect.Width -= T((new_draw_rect.X + new_draw_rect.Width) - (bounding_rect.X + bounding_rect.Width));
	}
	if (new_draw_rect.Width < 1) return(bad_rect);

	if (new_draw_rect.Y + new_draw_rect.Height > bounding_rect.Y + bounding_rect.Height) {
		new_draw_rect.Height -= T((new_draw_rect.Y + new_draw_rect.Height) - (bounding_rect.Y + bounding_rect.Height));
	}
	if (new_draw_rect.Height < 1) return(bad_rect);

	if (x != NULL) {
		*x -= T(new_draw_rect.X - draw_rect.X);
	}
	if (y != NULL) {
		*y -= T(new_draw_rect.Y - draw_rect.Y);
	}

	return(new_draw_rect);
}

template<class T>
TRect<T> const Intersect(TRect<T> const & rect1, TRect<T> const & rect2)
{
	return(Intersect(rect1, rect2, (T*)NULL, (T*)NULL));
}

typedef TRect<int> Rect;

// Defined here, so every module including this header holds its own copy.
const Rect RECT_NONE(0,0,0,0);
