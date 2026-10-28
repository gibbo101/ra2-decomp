// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "grphmitm.h"

class ConvertClass;
class Wstring;

class GraphicMenuAnimItem : public GraphicMenuItem
{
	public:
		GraphicMenuAnimItem(int id, MSEngine & engine, Point2D const & origin, Rect const & rect, Wstring const & image, Wstring const & palette, int rate, bool loop);
		virtual ~GraphicMenuAnimItem(void) override;

		virtual bool Is_Mouse_Over(Point2D const & mouse) override;

	public:

		Rect ActiveRect;

		int Reserved;

		ConvertClass * Drawer;
};
