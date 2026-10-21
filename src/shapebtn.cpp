// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "shapebtn.h"

#include "_convert.h"
#include "_surface.h"
#include "convert.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsurface.h"
#include "shapeset.h"

// FUNCTION: GAMEMD 0x0069DCF0
ShapeButtonClass::ShapeButtonClass(void) :
	BASECLASS(0, 0, 0, 0, 0),
	IsToFlash(false),
	FlashDelay(0),
	FlashCounter(0),
	ReflectButtonState(false),
	DrawOffset(0, 0),
	DrawOnSidebar(false),
	ShapeDrawer(NormalDrawer),
	DrawFaded(false),
	IsShapeLoaded(false)
{
}

// FUNCTION: GAMEMD 0x0069DD30
ShapeButtonClass::ShapeButtonClass(unsigned id, ShapeSet const * shape, int x, int y, int override_width, int override_height, bool faded) :
	BASECLASS(id, x, y, 0, 0),
	IsToFlash(false),
	FlashDelay(0),
	ReflectButtonState(false),
	DrawOffset(0, 0),
	DrawOnSidebar(false),
	ShapeDrawer(NormalDrawer),
	DrawFaded(faded),
	IsShapeLoaded(false)
{
	Set_Shape(shape, override_width, override_height);
}

// FUNCTION: GAMEMD 0x0069DDC0
ShapeButtonClass::~ShapeButtonClass(void)
{
	ClearShape();
}

// FUNCTION: GAMEMD 0x0069DE00
void ShapeButtonClass::Set_Shape(ShapeSet const * data, int override_width, int override_height)
{
	ClearShape();
	ShapeData = data;
	if (ShapeData) {
		Width = ShapeData->Get_Width();
		Height = ShapeData->Get_Height();
	} else {
		Width = 0;
		Height = 0;
	}
	if (override_width != 0) {
		Width = override_width;
	}
	if (override_height != 0) {
		Height = override_height;
	}
}

// FUNCTION: GAMEMD 0x0069DE70
void ShapeButtonClass::ClearShape(void)
{
	if (IsShapeLoaded && ShapeData) {
		delete (void *)ShapeData;
		ShapeData = 0;
		IsShapeLoaded = false;
		Width = 0;
		Height = 0;
	}
}

// FUNCTION: GAMEMD 0x0069DEA0
void ShapeButtonClass::SetLoadedStatus(bool loaded)
{
	IsShapeLoaded = loaded;
}

// FUNCTION: GAMEMD 0x0069DEB0
int ShapeButtonClass::Draw_Me(int forced)
{
	if (ControlClass::Draw_Me(forced) && ShapeData) {
		int shapenum = 0;
		if (IsDisabled) {
			shapenum = DISABLED_SHAPE;
		} else {
			if (ReflectButtonState) {
				if (IsToFlash) {
					shapenum = (IsOn != 0) + 3;
				} else {
					shapenum = (IsOn != 0);
				}
			} else {
				shapenum = (IsPressed != 0);
			}
		}
		Surface *surf = SidebarSurface;
		if (!DrawOnSidebar) {
			surf = DSurface::Temp;
		}
		Rect rect = surf->Get_Rect();
		Draw_Shape(*surf, *ShapeDrawer, ShapeData, shapenum, Point2D(DrawOffset.X + X, DrawOffset.Y + Y), rect, DrawFaded ? SHAPE_ALPHA : SHAPE_NORMAL);
		IsDrawn = true;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0069DFC0
bool ShapeButtonClass::StartFlashing(int delay, int initdelay, bool start)
{
	if (FlashDelay == 0) {
		FlashDelay = delay;
		FlashCounter = delay + initdelay;
		IsToFlash = start;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0069DFF0
bool ShapeButtonClass::StopFlashing(void)
{
	if (FlashDelay != 0) {
		IsToFlash = false;
		FlashCounter = 0;
		FlashDelay = 0;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0069E010
bool ShapeButtonClass::AI(void)
{
	if (IsDisabled) {
		if (FlashDelay != 0) {
			IsToFlash = false;
			FlashCounter = 0;
			FlashDelay = 0;
			return(true);
		}
	} else if (FlashCounter != 0) {
		FlashCounter--;
		if (FlashCounter == 0) {
			IsToFlash = !IsToFlash;
			FlashCounter = FlashDelay;
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0069E050
bool ShapeButtonClass::IsFlashing(void)
{
	return(FlashDelay != 0);
}

// FUNCTION: GAMEMD 0x0069E060
bool ShapeButtonClass::IsFlashOn(void)
{
	return(IsToFlash);
}
