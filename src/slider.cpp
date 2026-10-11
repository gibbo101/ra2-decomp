// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "slider.h"
#include "_mixfile.h"
#include "control.h"
#include "dialog.h"
#include "gauge.h"
#include "mixfile.h"
#include "shapebtn.h"
#include "xmouse.h"
#include "dialog.hh"

// FUNCTION: 0x006B1D00
SliderClass::~SliderClass(void)
{
	if (PlusGadget) {
		delete PlusGadget;
		PlusGadget = 0;
	}
	if (MinusGadget) {
		delete MinusGadget;
		MinusGadget = 0;
	}
}

// FUNCTION: 0x006B1D40
int SliderClass::Set_Maximum(int value)
{
	if (BASECLASS::Set_Maximum(value)) {
		Recalc_Thumb();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006B1DC0
void SliderClass::Set_Thumb_Size(int value)
{
	Thumb = MIN(value, MaxValue);
	Thumb = MAX(Thumb, 1);
	Flag_To_Redraw();
	Recalc_Thumb();
}

// FUNCTION: 0x006B1E50
int SliderClass::Set_Value(int value)
{
	value = MIN(value, MaxValue-Thumb);

	if (BASECLASS::Set_Value(value)) {
		Recalc_Thumb();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006B1EE0
void SliderClass::Recalc_Thumb(void)
{
	int length = IsHorizontal ? Width : Height;
	int size   = int(length * ((double)Thumb / MaxValue));
	ThumbSize  = MAX(size, 4);
	int start  = int(length * ((double)CurValue / MaxValue));
	ThumbStart = MIN(start, length-ThumbSize);
}

// FUNCTION: 0x006B2070
void SliderClass::Draw_Thumb(void)
{
	if (IsHorizontal) {
		Draw_Box(Rect(X+ThumbStart, Y, ThumbSize, Height), BOXSTYLE_RAISED, true);
	} else {
		Draw_Box(Rect(X, Y+ThumbStart, Width, ThumbSize), BOXSTYLE_RAISED, true);
	}
}
