// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "xsurface.h"
#include "assert.h"
#include "blit.h"
#include "blitblit.h"
#include "swap.h"

typedef int OutCode;

const int CODE_INSIDE = 0;

const int CODE_LEFT = 1;

const int CODE_RIGHT = 2;

const int CODE_BOTTOM = 4;

const int CODE_TOP = 8;

OutCode Compute_Code(double x, double y, Rect const & rect);

// FUNCTION: 0x007BADC0
bool XSurface::Draw_Rect(Rect const & cliprect, Rect const & crect, int color)
{
	Point2D ul(crect.X, crect.Y);
	Point2D ur(crect.X+crect.Width-1, crect.Y);
	Point2D ll(crect.X, crect.Y+crect.Height-1);
	Point2D lr(crect.X+crect.Width-1, crect.Y+crect.Height-1);

	Draw_Line(cliprect, ul, ur, color);
	Draw_Line(cliprect, ul, ll, color);
	Draw_Line(cliprect, ur, lr, color);
	Draw_Line(cliprect, ll, lr, color);
	return(true);
}

// FUNCTION: 0x007BB920
void XSurface::Draw_Circle(Point2D center, int radius, Rect rect, int color)
{
	Point2D pt(radius, 0);

	const int roundness_val = 2;

	int d = 3 - (roundness_val * radius);

	do {
		Draw_Line(rect, center + Point2D(-pt.X,  pt.Y), center + Point2D(pt.X,  pt.Y), color);
		Draw_Line(rect, center + Point2D(-pt.Y,  pt.X), center + Point2D(pt.Y,  pt.X), color);
		Draw_Line(rect, center + Point2D(-pt.X, -pt.Y), center + Point2D(pt.X, -pt.Y), color);
		Draw_Line(rect, center + Point2D(-pt.Y, -pt.X), center + Point2D(pt.Y, -pt.X), color);

		if (d < 0) {
			d += (4 * pt.Y) + 6;

		} else {
			d += 4 * (pt.Y - pt.X) + 10;
			pt.X--;
		}

		pt.Y++;

	} while (pt.X >= pt.Y);
}

// FUNCTION: 0x007BBE20
bool Blit_Clip(Rect & drect, Rect const & dwindow, Rect & srect, Rect const & swindow)
{
	if (drect.Width == srect.Width && drect.Height == srect.Height) {
		if (drect.X < 0) {
			srect.X += -drect.X;
			srect.Width -= -drect.X;
			drect.Width -= -drect.X;
			drect.X = 0;
		}
		if (drect.Y < 0) {
			srect.Y += -drect.Y;
			srect.Height -= -drect.Y;
			drect.Height -= -drect.Y;
			drect.Y = 0;
		}

		int rightspill = (drect.X+drect.Width) - dwindow.Width;
		if (rightspill > 0) {
			srect.Width -= rightspill;
			drect.Width -= rightspill;
		}
		int bottomspill = (drect.Y+drect.Height) - dwindow.Height;
		if (bottomspill > 0) {
			srect.Height -= bottomspill;
			drect.Height -= bottomspill;
		}

		if (srect.X < 0) {
			drect.X += -srect.X;
			srect.Width -= -srect.X;
			drect.Width -= -srect.X;
			srect.X = 0;
		}
		if (srect.Y < 0) {
			drect.Y += -srect.Y;
			srect.Height -= -srect.Y;
			drect.Height -= -srect.Y;
			srect.Y = 0;
		}

		rightspill = (srect.X+srect.Width) - swindow.Width;
		if (rightspill > 0) {
			srect.Width -= rightspill;
			drect.Width -= rightspill;
		}
		bottomspill = (srect.Y+srect.Height) - swindow.Height;
		if (bottomspill > 0) {
			srect.Height -= bottomspill;
			drect.Height -= bottomspill;
		}

	} else {
		drect = Intersect(drect, dwindow);
		srect = Intersect(srect, swindow);
	}

	return(drect.Is_Valid() && srect.Is_Valid());
}

// FUNCTION: 0x007BC2B0
bool Clip_Line_To_Rect(Point2D & point1, Point2D & point2, Rect const & rect)
{
	int outcode0;
	int outcode1;
	int outcodeOut;

	double x = rect.X;
	double y = rect.Y;

	double x0 = point1.X;
	double y0 = point1.Y;
	double x1 = point2.X;
	double y1 = point2.Y;

	double slope_y = (x1 - x0) / (y1 - y0);
	double slope_x = (y1 - y0) / (x1 - x0);

	outcode0 = Compute_Code(x0, y0, rect);
	outcode1 = Compute_Code(x1, y1, rect);

AGAIN:
	if (outcode0 == CODE_INSIDE && outcode1 == CODE_INSIDE) {
		point1.X = (int)x0;
		point1.Y = (int)y0;
		point2.X = (int)x1;
		point2.Y = (int)y1;
		return(true);
	}

	if (outcode0 & outcode1) {
		return(false);
	}

	outcodeOut = (outcode0 != CODE_INSIDE) ? outcode0 : outcode1;

	if (outcodeOut & CODE_TOP) {
		x = (rect.Y - y0) * slope_y + x0;
		y = rect.Y;
	} else if (outcodeOut & CODE_BOTTOM) {
		x = ((rect.Y + rect.Height - 1) - y0) * slope_y + x0;
		y = (rect.Height + rect.Y - 1);
	} else if (outcodeOut & CODE_RIGHT) {
		y = ((rect.X + rect.Width - 1) - x0) * slope_x + y0;
		x = (rect.Width + rect.X - 1);
	} else if (outcodeOut & CODE_LEFT) {
		y = (rect.X - x0) * slope_x + y0;
		x = rect.X;
	}

	if (outcodeOut == outcode0) {
		x0 = x;
		y0 = y;
		outcode0 = Compute_Code(x0, y0, rect);
	} else {
		x1 = x;
		y1 = y;
		outcode1 = Compute_Code(x1, y1, rect);
	}
	goto AGAIN;

	return(false);
}

// FUNCTION: 0x007BC580
OutCode Compute_Code(double x, double y, Rect const & rect)
{
	OutCode code = CODE_INSIDE;

	int xx = rect.X + rect.Width;
	if (x >= xx) {
		code |= CODE_RIGHT;
	} else if (x < rect.X) {
		code |= CODE_LEFT;
	}

	int yy = rect.Y + rect.Height;
	if (y >= yy) {
		code |= CODE_BOTTOM;
	} else if (y < rect.Y) {
		code |= CODE_TOP;
	}

	return(code);
}
