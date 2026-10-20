// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).


#include "always.h"
#include "mouse.h"
#include "_mixfile.h"
#include "animtype.h"
#include "builtype.h"
#include "cell.h"
#include "data.h"
#include "hashtable.h"
#include "isotype.h"
#include "mixfile.h"
#include "overtype.h"
#include "rawfile.h"
#include "scenario.h"
#include "shapeset.h"
#include "smudtype.h"
#include "swizzle.h"
#include "terrtype.h"
#include "xmouse.h"

#define MOUSE_HOTSPOT_CENTER 12345
#define MOUSE_HOTSPOT_MAX 54321

ShapeSet const * MouseClass::MouseShapes;

static bool MouseShapesLoaded;
void * __fastcall FileSystem_LoadWholeFileEx(char const * name, bool & loaded);

// FUNCTION: 0x005BD9F0 $E133
// FUNCTION: 0x005BDA20 $E130
CDTimerClass<SystemTimerClass> MouseClass::Timer = 0;

// FUNCTION: 0x005BDA40
MouseClass::MouseClass(void) :
	IsSmall(false),
	CurrentMouseShape(MOUSE_NORMAL),
	NormalMouseShape(MOUSE_NORMAL),
	Frame(0)
{
}

// FUNCTION: 0x005BDA80
void MouseClass::Set_Default_Mouse(MouseType mouse, bool size)
{
	NormalMouseShape = mouse;
	Override_Mouse_Shape(mouse, size);
}

// FUNCTION: 0x005BDAA0
void MouseClass::Revert_Mouse_Shape(void)
{
	Override_Mouse_Shape(NormalMouseShape, false);
}

// FUNCTION: 0x005BDAB0
void MouseClass::Mouse_Small(bool wsmall)
{
	if (IsSmall == wsmall) {
		return;
	}

	IsSmall = wsmall;

	MouseCursor->Set_Cursor(Get_Mouse_Hotspot(CurrentMouseShape), MouseShapes, Get_Mouse_Current_Frame(CurrentMouseShape, wsmall));
}

// FUNCTION: 0x005BDB90
int MouseClass::Get_Mouse_Start_Frame(MouseType mouse, bool wsmall) const
{
	MouseStruct const * control = &MouseControl[mouse];

	if (wsmall) {
		if (control->SmallFrame != -1) {
			return(control->SmallFrame);
		}
	}

	return(control->StartFrame);
}

// FUNCTION: 0x005BDBC0
int MouseClass::Get_Mouse_Current_Frame(MouseType mouse, bool wsmall) const
{
	return(Frame + Get_Mouse_Start_Frame(mouse, wsmall));
}

// FUNCTION: 0x005BDC00
Point2D MouseClass::Get_Mouse_Hotspot(MouseType mouse) const
{
	Point2D hotspot(0,0);

	MouseStruct const * control = &MouseControl[mouse];

	if (control->X == MOUSE_HOTSPOT_CENTER) {
		hotspot.X = MouseShapes->Get_Width() / 2;
	}
	if (control->X == MOUSE_HOTSPOT_MAX) {
		hotspot.X = MouseShapes->Get_Width();
	}

	if (control->Y == MOUSE_HOTSPOT_CENTER) {
		hotspot.Y = MouseShapes->Get_Height() / 2;
	}
	if (control->Y == MOUSE_HOTSPOT_MAX) {
		hotspot.Y = MouseShapes->Get_Height();
	}

	return(hotspot);
}

// FUNCTION: 0x005BDC80
bool MouseClass::Override_Mouse_Shape(MouseType mouse, bool wsmall)
{
	MouseStruct const * control = &MouseControl[mouse];
	static bool startup = false;

	if (control->SmallFrame == -1) {
		wsmall = false;
	}

	if (!startup || (MouseShapes && ((mouse != CurrentMouseShape) || (wsmall != IsSmall)))) {
		startup = true;

		Timer = control->FrameRate;
		Frame = 0;

		MouseCursor->Set_Cursor(Get_Mouse_Hotspot(mouse), MouseShapes, Get_Mouse_Current_Frame(mouse, wsmall));
		CurrentMouseShape = mouse;
		IsSmall = wsmall;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005BDDC0
void MouseClass::AI(KeyNumType &input, Point2D const & xy)
{
	MouseStruct const * control = &MouseControl[CurrentMouseShape];

	if (control->FrameRate && Timer == 0) {

		Frame++;
		if (IsSmall) {
			Frame %= control->SmallFrameCount;
		} else {
			Frame %= control->FrameCount;
		}
		Timer = control->FrameRate;

		MouseCursor->Set_Cursor(Get_Mouse_Hotspot(CurrentMouseShape), MouseShapes, Get_Mouse_Current_Frame(CurrentMouseShape, IsSmall));
	}

	BASECLASS::AI(input, xy);
}

// FUNCTION: 0x005BDF30
void MouseClass::One_Time(void)
{
	BASECLASS::One_Time();
	MouseShapes = (ShapeSet const *)FileSystem_LoadWholeFileEx("MOUSE.SHA", MouseShapesLoaded);
}
