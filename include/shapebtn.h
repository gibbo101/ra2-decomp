// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "point.h"
#include "toggle.h"

class ConvertClass;
class ShapeSet;

class ShapeButtonClass : public ToggleClass
{
		typedef ToggleClass BASECLASS;

	public:
		ShapeButtonClass(void);
		ShapeButtonClass(unsigned id, ShapeSet const * shapes, int x, int y, int override_width = 0, int override_height = 0, bool faded = false);
		virtual ~ShapeButtonClass(void) override;
		virtual int Draw_Me(int forced=false) override;
		virtual void Set_Shape(ShapeSet const * data, int override_width = 0, int override_height = 0);
		ShapeSet const * Get_Shape_Data(void) {return(ShapeData);};

		enum ShapeButtonClassEnums {
			UP_SHAPE,
			DOWN_SHAPE,
			DISABLED_SHAPE
		};

		bool IsToFlash;
		int FlashDelay;
		int FlashCounter;

		bool ReflectButtonState;

		Point2D DrawOffset;

		bool DrawOnSidebar;

		ConvertClass *ShapeDrawer;

		bool IsDrawn;

		bool DrawFaded;

	protected:

		ShapeSet const * ShapeData;

		bool IsShapeLoaded;
};
