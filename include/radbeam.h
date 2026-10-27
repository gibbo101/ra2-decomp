// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "coord.h"
#include "rgb.h"
#include "vector.h"

class TechnoClass;
class AbstractClass;

class RadBeam
{
	public:
		RadBeam(int type = 0);

		static RadBeam * Allocate(int type);
		static void FUN_006591B0(void);

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
		void FUN_00659560(void);
		unsigned char FUN_00659570(void);
		void FUN_00659580(int value);
		void FUN_00659590(Coord const & from, Coord const & to, short color);
		bool FUN_00659650(void);
		void FUN_006599C0(void);
		void FUN_00659A30(void);
		void FUN_00659AC0(void);
		bool FUN_00659CA0(void);
		void FUN_00659E30(void);
		void FUN_00659EE0(void);
		void FUN_00659FF0(void);

		TechnoClass * unknown_0;
		AbstractClass * Owner;
		unsigned char unknown_8;
		unsigned char unknown_9;
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
		Coord Delta;
		double Length;
		Coord AnotherLocation;
		int unknown_7C;
		double unknown_80;
		int unknown_88;
		int unknown_8C;
		Coord AndAnotherLocation;
		Coord PointA;
		Coord PointB;
		unsigned unknown_B4;
		double unknown_B8;
		RGBClass Tint;
		unsigned unknown_C4;
};
