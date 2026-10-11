// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "gauge.h"
#include "_convert.h"
#include "_surface.h"
#include "bound.h"
#include "control.h"
#include "convert.h"
#include "dialog.h"
#include "dsurface.h"
#include "scheme.h"
#include "vector.h"
#include "xmouse.h"
#include "color.hh"

// FUNCTION: 0x004E2500
GaugeClass::GaugeClass(unsigned id, int x, int y, int w, int h)
	: BASECLASS(id, x, y, w, h, LEFTHELD|LEFTPRESS|LEFTRELEASE, true)
{
	Set_Maximum(255);
	Set_Value(0);

	HasThumb = true;
	IsHorizontal = (w > h);
	IsColorized = true;

	ClickDiff = 0;
}

// FUNCTION: 0x004E2580
int GaugeClass::Set_Maximum(int value)
{
	if (value != MaxValue) {
		MaxValue = value;
		Flag_To_Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004E25A0
int GaugeClass::Set_Value(int value)
{
	value = Bound(value, 0, MaxValue);
	if (value != CurValue) {
		CurValue = value;
		Flag_To_Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004E25D0
int GaugeClass::Pixel_To_Value(int pixel)
{
	int maximum;

	if (IsHorizontal) {
		pixel -= X+1;
		maximum = Width;
	} else {
		pixel -= Y+1;
		maximum = Height;
	}
	maximum -= 2;
	pixel = Bound(pixel, 0, maximum);
	return(int(MaxValue * ((double)pixel / maximum)));
}

// FUNCTION: 0x004E2650
int GaugeClass::Value_To_Pixel(int value)
{
	int maximum;
	int start;
	if (IsHorizontal) {
		maximum = Width;
		start = X;
	} else {
		maximum = Height;
		start = Y;
	}
	maximum -= 2;
	return(int(start + (maximum * ((double)value / MaxValue))));
}

// FUNCTION: 0x004E2A50
TriColorGaugeClass::TriColorGaugeClass(unsigned id, int x, int y, int w, int h)
	: BASECLASS(id, x, y, w, h),
	RedLimit(0),
	YellowLimit(0)
{
}

// FUNCTION: 0x004E2AF0
int TriColorGaugeClass::Set_Red_Limit(int value)
{
	if (value >= 0 && value < MaxValue) {
			RedLimit = value;
		Flag_To_Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004E2B20
int TriColorGaugeClass::Set_Yellow_Limit(int value)
{
	if (value >= 0 && value < MaxValue) {
			YellowLimit = value;
		Flag_To_Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004E30B0
void GaugeClass::Use_Thumb(int value)
{
	HasThumb = value ? true : false;
}

extern int DAT_008B3F90;
extern int DAT_008B3F94;
extern int DAT_008B3F98;
extern int DAT_008B3F9C;

// FUNCTION: 0x004E3110
void FUN_004E3110(void)
{
	DAT_008B3F90 = 0;
	DAT_008B3F94 = 0;
	DAT_008B3F98 = 0;
	DAT_008B3F9C = 0;
}
