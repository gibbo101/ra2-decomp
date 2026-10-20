// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "slider.h"
#include "_mixfile.h"
#include "control.h"
#include "dialog.h"
#include "gauge.h"
#include "mixfile.h"
#include "shapebtn.h"
#include "xmouse.h"
#include "dialog.hh"

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

// FUNCTION: 0x006B1B20
SliderClass::SliderClass(unsigned id, int x, int y, int w, int h, int belong_to_list)
	: BASECLASS(id, x, y, w, h)
{
	BelongToList = belong_to_list ? true : false;

	PlusGadget = 0;
	MinusGadget = 0;
	if (!BelongToList) {
		PlusGadget  = new ShapeButtonClass(id, (const ShapeSet *)FileSystem::LoadFile("BTN-PLUS.SHP", false), X+Width+2, Y);
		MinusGadget = new ShapeButtonClass(id, (const ShapeSet *)FileSystem::LoadFile("BTN-MINS.SHP", false), X-6, Y);

		if (PlusGadget) {
			PlusGadget->Make_Peer(*this);
			PlusGadget->Add(*this);
			PlusGadget->Flag_To_Redraw();
		}
		if (MinusGadget) {
			MinusGadget->Make_Peer(*this);
			MinusGadget->Add(*this);
			MinusGadget->Flag_To_Redraw();
		}
	}
	Set_Thumb_Size(1);
	Recalc_Thumb();
	IsColorized = 0;
}

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

// FUNCTION: 0x006B1F50
int SliderClass::Action(unsigned flags, KeyNumType &key, int modifier)
{
	if (flags & LEFTPRESS) {
		int mouse;
		int edge;

		if (IsHorizontal) {
			mouse = Get_Mouse_X();
			edge = X;
		} else {
			mouse = Get_Mouse_Y();
			edge = Y;
		}
		edge += 1;

		if (mouse < edge+ThumbStart) {
			Bump(true);
			BASECLASS::Action(0, key, 0);
			key = KN_NONE;
			return(true);
		} else {
			if (mouse > edge+ThumbStart+ThumbSize) {
				Bump(false);
				BASECLASS::Action(0, key, 0);
				key = KN_NONE;
				return(true);
			} else {
				BASECLASS::Action(flags, key, 0);
				key = KN_NONE;
				return(true);
			}
		}
	}
	return(BASECLASS::Action(flags, key, 0));
}

// FUNCTION: 0x006B2000
int SliderClass::Bump(int up)
{
	if (up) {
		return(Set_Value(CurValue - Thumb));
	}
	return(Set_Value(CurValue + Thumb));
}

// FUNCTION: 0x006B2040
int SliderClass::Step(int up)
{
	if (up) {
		return(Set_Value(CurValue - 1));
	}
	return(Set_Value(CurValue + 1));
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

// FUNCTION: 0x006B20F0
int SliderClass::Draw_Me(int forced)
{
	if (BelongToList) {
		if (ControlClass::Draw_Me(forced)) {
			Draw_Box (Rect(X, Y, Width, Height), BOXSTYLE_DOWN, true);
			Draw_Thumb();
			return(true);
		}
	}
	return(BASECLASS::Draw_Me(forced));
}

// FUNCTION: 0x006B2160
void SliderClass::Peer_To_Peer(unsigned flags, KeyNumType & , ControlClass & whom)
{
	if (flags & LEFTRELEASE) {
		if (&whom == PlusGadget) {
			Step(false);
		}
		if (&whom == MinusGadget) {
			Step(true);
		}
	}
}

// Initialisers of geometry.h constants.
// FUNCTION: 0x006B21A0 $E2
// FUNCTION: 0x006B21D0 $E5
// FUNCTION: 0x006B21F0 $E8
// FUNCTION: 0x006B2210 $E11
// FUNCTION: 0x006B2230 $E14
// FUNCTION: 0x006B2250 $E17
