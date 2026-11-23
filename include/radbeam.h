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
		static void Update_All(void);

		void Set_Color(RGBClass const & color);
		void Set_Z_Adjust(int zadjust);
		void Set_Source(Coord const & coord);
		void Set_Target(Coord const & coord);
		void Set_Period(int period);
		void Set_Amplitude(double amplitude);
		void FUN_00659510(double value);
		void FUN_00659530(int value);
		void FUN_00659540(int value);
		void FUN_00659550(bool value);
		void Hide(void);
		unsigned char Is_Visible(void);
		void Set_Segment_Length(int value);
		void FUN_00659590(Coord const & from, Coord const & to, short color);
		bool FUN_00659650(void);
		void FUN_006599C0(void);
		void FUN_00659A30(void);
		void FUN_00659AC0(void);
		bool FUN_00659CA0(void);
		void FUN_00659E30(void);
		void FUN_00659EE0(void);
		void FUN_00659FF0(void);

		TechnoClass * Firer;
		AbstractClass * Owner;
		unsigned char IsInitialized;
		unsigned char IsVisible;
		int ZAdjust;
		int Type;
		unsigned unknown_14;
		double SegmentLength;
		RGBClass Color;
		Coord SourceLocation;
		Coord TargetLocation;
		int Period;
		double Amplitude;
		double unknown_48;
		int unknown_50;
		int FrameStep;
		bool unknown_58;
		Point3D Delta;
		double Length;
		Coord AnotherLocation;
		int Frame;
		double CurrentAmplitude;
		int Segment;
		int SegmentCount;
		Coord AndAnotherLocation;
		Coord PointA;
		Coord PointB;
		unsigned unknown_B4;
		double unknown_B8;
		RGBClass Tint;
		unsigned unknown_C4;
};
