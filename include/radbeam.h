// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "coord.h"
#include "rgb.h"

class TechnoClass;

class RadBeam
{
	public:
		static RadBeam * Allocate(int type);

		void FUN_00659470(RGBClass const & color);
		void FUN_00659490(int zadjust);
		void FUN_006594A0(Coord const & coord);
		void FUN_006594C0(Coord const & coord);
		void FUN_006594E0(int period);
		void FUN_006594F0(double amplitude);
		void FUN_00659510(double value);
		void FUN_00659530(int value);
		void FUN_00659540(int value);
		void FUN_00659550(bool value);

		unsigned unknown_0;
		TechnoClass * Owner;
		unsigned char unknown_8;
		int ZAdjust;
		int Type;
		unsigned unknown_14;
		double unknown_18;
		RGBClass Color;
		Coord SourceLocation;
		Coord TargetLocation;
		int Period;
		double Amplitude;
		double unknown_48;
		int unknown_50;
		int unknown_54;
		bool unknown_58;
		unsigned unknown_5C;
		unsigned unknown_60;
		unsigned unknown_64;
		double unknown_68;
		Coord AnotherLocation;
		unsigned unknown_7C;
		double unknown_80;
		unsigned unknown_88;
		unsigned unknown_8C;
		Coord AndAnotherLocation;
		unsigned unknown_9C;
		unsigned unknown_A0;
		unsigned unknown_A4;
		unsigned unknown_A8;
		unsigned unknown_AC;
		unsigned unknown_B0;
		unsigned unknown_B4;
		double unknown_B8;
		unsigned char unknown_C0;
		unsigned unknown_C4;
};
