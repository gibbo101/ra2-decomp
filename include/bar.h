// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class ProgressBarClass
{
	public:
		ProgressBarClass(int x, int y, int width, int height, int forecolor, int backcolor, int bordercolor=0);

		bool Update(double value);
		void Redraw(void) const;

	private:

		void Outline(void) const;
		bool Is_Horizontal(void) const;
		bool Is_Outlined(void) const {return(BorderColor != 0);}

		int X,Y;

		int Width, Height;

		int BarColor;
		int BackColor;
		int BorderColor;

		double CurrentValue;

		double LastDisplayCurrent;

		bool IsDrawn;
};
