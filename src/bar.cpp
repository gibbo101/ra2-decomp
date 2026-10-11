// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "bar.h"
#include "_convert.h"
#include "_surface.h"
#include "_xmouse.h"
#include "convert.h"
#include "dsurface.h"
#include "xmouse.h"

// FUNCTION: 0x0042E0B0
ProgressBarClass::ProgressBarClass(int x, int y, int width, int height, int forecolor, int backcolor, int bordercolor) :
	X(x),
	Y(y),
	Width(width),
	Height(height),
	BarColor(forecolor),
	BackColor(backcolor),
	BorderColor(bordercolor),
	CurrentValue(0),
	LastDisplayCurrent(0),
	IsDrawn(false)
{
}

// FUNCTION: 0x0042E100
bool ProgressBarClass::Is_Horizontal(void) const
{
	if (Width > Height) return(true);
	return(false);
}

// FUNCTION: 0x0042E110
bool ProgressBarClass::Update(double value)
{
	CurrentValue = value;

	if (!IsDrawn || value - LastDisplayCurrent >= .1) {
		Redraw();
		return(true);
	}
	return(false);
}
