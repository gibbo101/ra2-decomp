// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "control.h"
#include "face.h"

#include "facing.hh"

class Dial8Class : public ControlClass
{
		typedef ControlClass BASECLASS;

	public:

		Dial8Class(int id, int x, int y, int w, int h, Dir256 dir);

		Dir256 Get_Direction(void) const;
		void Set_Direction(Dir256 dir);

		virtual int Draw_Me(int forced = false) override;

	protected:

		virtual int Action(unsigned flags, KeyNumType &key, int modifier) override;

	private:
		int FaceX;
		int FaceY;
		int FacePoint[FACING_COUNT][2];
		int FaceLine[FACING_COUNT][2];
		Dir256 Direction;
		FacingType Facing;
		FacingType OldFacing;

};
