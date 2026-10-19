// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "always.h"

#include "point.h"
#include "sun.h"
#include "win.h"

class ShapeSet;
class ColorScheme;

class ProgressScreenClass
{
	public:
		ProgressScreenClass(void);
		virtual ~ProgressScreenClass(void);

		void Initialize(double progress, char count, void * dialog);
		void End(void);
		void Free_Buffer(void);
		void Set_Side(int side);
		int Get_Side(void) const;
		ColorScheme * Side_Scheme(void) const;
		ColorScheme * Loading_Scheme(void) const;

		void Set_Progress_Percent(int index, double value, Point2D pt=Point2D(-1,-1));

	public:
		double PlayerProgress[MAX_PLAYERS];
		double MainProgress;
		int Field50;
		void * Buffer;
		bool BufferOwned;
		int Field5C;
		bool IsActive;
		char PlayerCount;
		bool Field62;
		void * Dialog;
		Point2D Pos;
		bool Field70;
		int Field74;
		int Field78;
		int Percentage;
		int Side;
};

extern ProgressScreenClass Progress;
