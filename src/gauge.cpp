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

// FUNCTION: GAMEMD 0x004E2500
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

// FUNCTION: GAMEMD 0x004E2580
int GaugeClass::Set_Maximum(int value)
{
	if (value != MaxValue) {
		MaxValue = value;
		Flag_To_Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004E25A0
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

// FUNCTION: GAMEMD 0x004E25D0
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

// FUNCTION: GAMEMD 0x004E2650
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

// FUNCTION: GAMEMD 0x004E2690
int GaugeClass::Draw_Me(int forced)
{
	if (BASECLASS::Draw_Me(forced)) {
		Conditional_Hide_Mouse(Rect(X, Y, Width, Height));
		Draw_Box (Rect(X, Y, Width, Height), BOXSTYLE_DOWN, true);

		if (IsColorized) {
			int middle = Value_To_Pixel(CurValue);
			int color = ColorSchemes[GadgetClass::Get_Color_Scheme()]->Bright;
			if (IsHorizontal) {
				if (middle >= (X + 1))
					LogicalSurface->Fill_Rect(Rect(X+1, Y+1, middle-X-1, Height-2), NormalDrawer->Convert_Pixel(color));
			} else {
				if (middle >= (Y + 1))
					LogicalSurface->Fill_Rect(Rect(X+1, Y+1, Width-2, middle-Y-1), NormalDrawer->Convert_Pixel(color));
			}
		}

		if (HasThumb) {
			Draw_Thumb();
		}

		Conditional_Show_Mouse();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004E2830
int GaugeClass::Action(unsigned flags, KeyNumType &key, int modifier)
{
	if (!HasThumb) {
		return(false);
	}

	Sticky_Process(flags);

	if ((flags & LEFTPRESS) || ((flags & LEFTHELD) && StuckOn == this)) {
		if (flags & LEFTPRESS) {
			int curpix = Value_To_Pixel(CurValue);
			int clickpix = (IsHorizontal ? Get_Mouse_X() : Get_Mouse_Y());

			if ( (clickpix > curpix) && (clickpix - curpix) < Thumb_Pixels()) {
				ClickDiff = (clickpix - curpix);
			} else {
				ClickDiff = 0;
			}

			int testval = Pixel_To_Value(IsHorizontal ?
				Get_Mouse_X() - ClickDiff : Get_Mouse_Y() - ClickDiff);

			while (testval < CurValue && ClickDiff > 0) {
				ClickDiff--;
				testval = Pixel_To_Value(IsHorizontal ?
				Get_Mouse_X() - ClickDiff : Get_Mouse_Y() - ClickDiff);
			}
		}

		if (!Set_Value(Pixel_To_Value(IsHorizontal ?
			Get_Mouse_X() - ClickDiff : Get_Mouse_Y() - ClickDiff))) {

			flags &= ~(LEFTHELD|LEFTRELEASE|LEFTPRESS);
			BASECLASS::Action(0, key, 0);
			key = KN_NONE;
			return(true);
		}

	} else {
		flags &= ~LEFTHELD;
	}
	return(BASECLASS::Action(flags, key, 0));
}

// FUNCTION: GAMEMD 0x004E29A0
void GaugeClass::Draw_Thumb(void)
{
	int x = Value_To_Pixel(CurValue);

	if ((x + 4) > Value_To_Pixel(MaxValue)) {
		x = Value_To_Pixel(MaxValue) - 2;
	}

	if (x < X) {
		x = X;
	}

	if (IsHorizontal) {
		Draw_Box(Rect(x, Y, 4, Height), BOXSTYLE_RAISED, true);
	} else {
		Draw_Box(Rect(X, x, Width, 4),  BOXSTYLE_RAISED, true);
	}
}

// FUNCTION: GAMEMD 0x004E2A50
TriColorGaugeClass::TriColorGaugeClass(unsigned id, int x, int y, int w, int h)
	: BASECLASS(id, x, y, w, h),
	RedLimit(0),
	YellowLimit(0)
{
}

// FUNCTION: GAMEMD 0x004E2AF0
int TriColorGaugeClass::Set_Red_Limit(int value)
{
	if (value >= 0 && value < MaxValue) {
			RedLimit = value;
		Flag_To_Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004E2B20
int TriColorGaugeClass::Set_Yellow_Limit(int value)
{
	if (value >= 0 && value < MaxValue) {
			YellowLimit = value;
		Flag_To_Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004E2B50
int TriColorGaugeClass::Draw_Me(int forced)
{
	if (BASECLASS::Draw_Me(forced)) {
		Conditional_Hide_Mouse(Rect(X, Y, Width, Height));
		Draw_Box (Rect(X, Y, Width, Height), (IsDisabled ? BOXSTYLE_RAISED : BOXSTYLE_DOWN), true);

		int red    = Value_To_Pixel(RedLimit);
		int yellow = Value_To_Pixel(YellowLimit);
		int middle = Value_To_Pixel(CurValue);

		if (CurValue <= RedLimit) {
			if (IsHorizontal) {
				LogicalSurface->Fill_Rect(Rect(X+1, Y+1, middle-X+1, Height-2), NormalDrawer->Convert_Pixel(PINK));
			} else {
				LogicalSurface->Fill_Rect(Rect(X+1, Y+1, Width-2, middle-Y+1), NormalDrawer->Convert_Pixel(PINK));
			}
		} else if (CurValue > RedLimit && CurValue <= YellowLimit) {
			if (IsHorizontal) {
				LogicalSurface->Fill_Rect(Rect(X+1, Y+1, red-X+1,    Height-2), NormalDrawer->Convert_Pixel(PINK));
				LogicalSurface->Fill_Rect(Rect(red-X+1, Y+1, middle-X-1, Height-2), NormalDrawer->Convert_Pixel(YELLOW));
			} else {
				LogicalSurface->Fill_Rect(Rect(X+1, Y+1, Width-2, red-Y+1),    NormalDrawer->Convert_Pixel(PINK));
				LogicalSurface->Fill_Rect(Rect(X+1, red-Y+1, X+Width-2, middle-Y+1), NormalDrawer->Convert_Pixel(YELLOW));
			}
		} else if (CurValue > YellowLimit && CurValue <= MaxValue) {
			if (IsHorizontal) {
				LogicalSurface->Fill_Rect(Rect(X+1,    Y+1, red-X+1,    Height-2), NormalDrawer->Convert_Pixel(PINK));
				LogicalSurface->Fill_Rect(Rect(red,    Y+1, yellow-X+1, Height-2), NormalDrawer->Convert_Pixel(YELLOW));
				LogicalSurface->Fill_Rect(Rect(yellow, Y+1, middle-X+1, Height-2), NormalDrawer->Convert_Pixel(GREEN));
			} else {
				LogicalSurface->Fill_Rect(Rect(X+1, Y+1,    Width-2, red-Y+1),    NormalDrawer->Convert_Pixel(PINK));
				LogicalSurface->Fill_Rect(Rect(X+1, red,    Width-2, yellow-Y+1), NormalDrawer->Convert_Pixel(YELLOW));
				LogicalSurface->Fill_Rect(Rect(X+1, yellow, Width-2, middle-Y+1), NormalDrawer->Convert_Pixel(GREEN));
			}
		}

		if (HasThumb) {
			Draw_Thumb();
		}

		Conditional_Show_Mouse();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004E30B0
void GaugeClass::Use_Thumb(int value)
{
	HasThumb = value ? true : false;
}

// SYNTHETIC: GAMEMD 0x004E30D0
// ??_GGaugeClass@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x004E30F0
// ??_GTriColorGaugeClass@@UAEPAXI@Z
