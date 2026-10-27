// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "dial8.h"
#include "_convert.h"
#include "_surface.h"
#include "convert.h"
#include "dialog.h"
#include "dsurface.h"
#include "inline.h"
#include "scheme.h"
#include "surface.h"
#include "vector.h"
#include "wwmouse.h"
#include "_xmouse.h"

// SYNTHETIC: GAMEMD 0x004A5390 SYMBOL
// _$E2

// FUNCTION: GAMEMD 0x004A53B0
Dial8Class::Dial8Class(int id, int x, int y, int w, int h, Dir256 dir) :
	BASECLASS(id, x, y, w, h, LEFTPRESS | LEFTHELD | LEFTRELEASE, true)
{
	FaceX = X + (Width / 2);
	FaceY = Y + (Height / 2);

	Direction = dir;
	Facing = Dir_Facing(Direction);
	OldFacing = Facing;

	FacePoint[FACING_N][0] = FaceX;
	FacePoint[FACING_N][1] = FaceY - (h * 8 / 2) / 10;

	FacePoint[FACING_NE][0] = FaceX + (w * 7 * 8 / 2) / 100;
	FacePoint[FACING_NE][1] = FaceY - (h * 7 * 8 / 2) / 100;

	FacePoint[FACING_E][0] = FaceX + (w * 8 / 2) / 10;
	FacePoint[FACING_E][1] = FaceY;

	FacePoint[FACING_SE][0] = FaceX + (w * 7 * 8 / 2) / 100;
	FacePoint[FACING_SE][1] = FaceY + (h * 7 * 8 / 2) / 100;

	FacePoint[FACING_S][0] = FaceX;
	FacePoint[FACING_S][1] = FaceY + (h * 8 / 2) / 10;

	FacePoint[FACING_SW][0] = FaceX - (w * 7 * 8 / 2) / 100;
	FacePoint[FACING_SW][1] = FaceY + (h * 7 * 8 / 2) / 100;

	FacePoint[FACING_W][0] = FaceX - (w * 8 / 2) / 10;
	FacePoint[FACING_W][1] = FaceY;

	FacePoint[FACING_NW][0] = FaceX - (w * 7 * 8 / 2) / 100;
	FacePoint[FACING_NW][1] = FaceY - (h * 7 * 8 / 2) / 100;

	FaceLine[FACING_N][0] = FaceX;
	FaceLine[FACING_N][1] = FaceY - (h * 6 / 2) / 10;

	FaceLine[FACING_NE][0] = FaceX + (w * 7 * 6 / 2) / 100;
	FaceLine[FACING_NE][1] = FaceY - (h * 7 * 6 / 2) / 100;

	FaceLine[FACING_E][0] = FaceX + (w * 6 / 2) / 10;
	FaceLine[FACING_E][1] = FaceY;

	FaceLine[FACING_SE][0] = FaceX + (w * 7 * 6 / 2) / 100;
	FaceLine[FACING_SE][1] = FaceY + (h * 7 * 6 / 2) / 100;

	FaceLine[FACING_S][0] = FaceX;
	FaceLine[FACING_S][1] = FaceY + (h * 6 / 2) / 10;

	FaceLine[FACING_SW][0] = FaceX - (w * 7 * 6 / 2) / 100;
	FaceLine[FACING_SW][1] = FaceY + (h * 7 * 6 / 2) / 100;

	FaceLine[FACING_W][0] = FaceX - (w * 6 / 2) / 10;
	FaceLine[FACING_W][1] = FaceY;

	FaceLine[FACING_NW][0] = FaceX - (w * 7 * 6 / 2) / 100;
	FaceLine[FACING_NW][1] = FaceY - (h * 7 * 6 / 2) / 100;
}

// FUNCTION: GAMEMD 0x004A5660
int Dial8Class::Action(unsigned flags, KeyNumType &key, int modifier)
{
	static int is_sel = 0;

	Sticky_Process(flags);

	if (flags & LEFTPRESS) {
		is_sel = 1;
	}

	if (flags & LEFTPRESS || ((flags & LEFTHELD) && is_sel)) {
		Direction = ::Direction(Point2D(FaceX, FaceY), Get_Mouse_Point()).As_Dir256();

		Facing = Dir_Facing(Direction);

		if (Facing != OldFacing) {
			OldFacing = Facing;
			BASECLASS::Action(flags, key, 0);
			return(true);
		} else {
			key = KN_NONE;
			BASECLASS::Action(0, key, 0);
			return(true);
		}
	} else {
		if (flags & LEFTRELEASE) {
			key = KN_NONE;
			is_sel = 0;
		}
		return(BASECLASS::Action(0, key, 0));
	}
}


// FUNCTION: GAMEMD 0x004A57B0
int Dial8Class::Draw_Me(int forced)
{
	int scheme = GadgetClass::Get_Color_Scheme();

	if (BASECLASS::Draw_Me(forced)) {
		MouseCursor->Hide_Mouse();

		Draw_Box(Rect(X, Y, Width, Height), BOXSTYLE_DOWN, true);
		for (int i = 0; i < FACING_COUNT; i++) {
			Draw_Box(Rect(FacePoint[i][0] - 1, FacePoint[i][1] - 1, 3, 3), BOXSTYLE_RAISED, false);
		}

		DSurface::Temp->Draw_Line(Point2D(FaceX + 1, FaceY + 1), Point2D(FaceLine[Facing][0] + 1, FaceLine[Facing][1] + 1), NormalDrawer->Convert_Pixel(ColorSchemes[scheme]->Shadow));
		DSurface::Temp->Draw_Line(Point2D(FaceX, FaceY), Point2D(FaceLine[Facing][0], FaceLine[Facing][1]), NormalDrawer->Convert_Pixel(ColorSchemes[scheme]->Highlight));

		MouseCursor->Show_Mouse();

		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004A5950
Dir256 Dial8Class::Get_Direction(void) const
{
	return(Direction);
}

// FUNCTION: GAMEMD 0x004A5960
void Dial8Class::Set_Direction(Dir256 dir)
{
	Direction = dir;
	Facing = Dir_Facing(Direction);
	OldFacing = Facing;
	Flag_To_Redraw();
}

// SYNTHETIC: GAMEMD 0x004A59C0 SYMBOL
// _$E31
const Rect RECT_NONE_2(0, 0, 0, 0);

// SYNTHETIC: GAMEMD 0x004A59A0
// ??_GDial8Class@@UAEPAXI@Z

