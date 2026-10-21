// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <climits>

#include "coord.h"
#include "ftimer.h"
#include "noinit.h"
#include "timer.h"

enum FuseResultType {
	FUSE_WAIT,
	FUSE_EXPLODE_CLOSE,
	FUSE_EXPLODE_FAR,
};

class FuseClass {
	public:
		FuseClass(void);
		FuseClass(NoInitClass const &) {};
		~FuseClass(void) {};

		void Arm_Fuse(Coord const & location, Coord const & target, int arming=0, int time=INT_MAX);
		FuseResultType Fuse_Checkup(Coord const & newlocation);
		Coord Fuse_Target(void) {return(HeadTo);}

		CDTimerClass<FrameTimerClass> Timer;

	private:

		CDTimerClass<FrameTimerClass> Arming;

		Coord HeadTo;

		int Proximity;
};
