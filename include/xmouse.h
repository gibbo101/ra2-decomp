// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "rect.h"

class Surface;
class ShapeSet;

class Mouse {
	public:
		virtual ~Mouse(void) {}

		virtual void Set_Cursor(Point2D const & hotspot, ShapeSet const * cursor, int shape) = 0;

		virtual bool Is_Hidden(void) const = 0;
		virtual void Hide_Mouse(void) = 0;
		virtual void Show_Mouse(void) = 0;

		virtual void Release_Mouse(void) = 0;
		virtual void Capture_Mouse(void) = 0;
		virtual bool Is_Captured(void) const = 0;

		virtual void Conditional_Hide_Mouse(Rect region) = 0;
		virtual void Conditional_Show_Mouse(void) = 0;

		virtual int Get_Mouse_State(void) const = 0;
		virtual int Get_Mouse_X(void) const = 0;
		virtual int Get_Mouse_Y(void) const = 0;
		virtual Point2D Get_Mouse_Point(void) const = 0;
		virtual void Set_Mouse_Point(int x, int y) = 0;

		virtual void Draw_Mouse(Surface * scr, bool issidebarsurface = false) = 0;
		virtual void Erase_Mouse(Surface * scr, bool issidebarsurface = false) = 0;

		virtual void Convert_Coordinate(int & x, int & y) const = 0;
};
