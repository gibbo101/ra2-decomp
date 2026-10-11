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
#include "inline.h"
#include "scheme.h"
#include "surface.h"
#include "vector.h"

// FUNCTION: 0x004A53B0
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

// FUNCTION: 0x004A5960
void Dial8Class::Set_Direction(Dir256 dir)
{
	Direction = dir;
	Facing = Dir_Facing(Direction);
	OldFacing = Facing;
	Flag_To_Redraw();
}
