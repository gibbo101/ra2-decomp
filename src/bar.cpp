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
#include <math.h>
#include "geometry.h"
#include "rect.h"

extern int DAT_0089C358;
extern int DAT_0089C35C;
extern int DAT_0089C360;

// SYNTHETIC: GAMEMD 0x0042E090 SYMBOL
// _$E2

// FUNCTION: GAMEMD 0x0042E0B0
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

// FUNCTION: GAMEMD 0x0042E100
bool ProgressBarClass::Is_Horizontal(void) const
{
	if (Width > Height) return(true);
	return(false);
}

// FUNCTION: GAMEMD 0x0042E110
bool ProgressBarClass::Update(double value)
{
	CurrentValue = value;

	if (!IsDrawn || value - LastDisplayCurrent >= .1) {
		Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0042E150
void ProgressBarClass::Outline(void) const
{
	if (Is_Outlined()) {
		LogicalSurface->Draw_Rect(Rect(X, Y, Width, Height), NormalDrawer->Convert_Pixel(BorderColor));
	}
}

// FUNCTION: GAMEMD 0x0042E1C0
void ProgressBarClass::Redraw(void) const
{
	Hide_Mouse();

	Outline();

	int x = X;
	int y = Y;
	int w = Width;
	int h = Height;
	if (Is_Outlined()) {
		x += 1;
		y += 1;
		w -= 2;
		h -= 2;
	}

	int size = Is_Horizontal() ? w : h;

	int fill = CurrentValue * size;

	if (fill > 0) {
		if (Is_Horizontal()) {
			LogicalSurface->Fill_Rect(Rect(x, y, fill, h), NormalDrawer->Convert_Pixel(BarColor));
		} else {
			LogicalSurface->Fill_Rect(Rect(x, y+fill, w, h), NormalDrawer->Convert_Pixel(BarColor));
		}
	}

	if (w-fill > 0) {
		if (Is_Horizontal()) {
			LogicalSurface->Fill_Rect(Rect(x+fill, y, w, h), NormalDrawer->Convert_Pixel(BackColor));
		} else {
			LogicalSurface->Fill_Rect(Rect(x, y, w, fill-1), NormalDrawer->Convert_Pixel(BackColor));
		}
	}

	Show_Mouse();

	ProgressBarClass * bar = (ProgressBarClass *)this;
	bar->LastDisplayCurrent = CurrentValue;
	bar->IsDrawn = true;
}

// TEMPLATE: GAMEMD 0x0042E410
// ??0?$TRect@H@@QAE@HHHH@Z
template class TRect<int>;

void Reset_Bar_View(void)
{
	DAT_0089C360 = 0;
	DAT_0089C358 = 0x80;
	DAT_0089C35C = 0x80;
}
