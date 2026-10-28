// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <stdlib.h>
#include "radbeam.h"
#include "object.h"
#include "techno.h"
#include "surface.h"
#include "_tactica.h"

// SYNTHETIC: GAMEMD 0x00658E80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00658EB0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00658ED0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00658EF0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00658F10 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00658F30 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00658F50 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00658F90 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00658FC0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00658FF0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00659020 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00659030 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00659050 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00659070 SYMBOL
// _$E41


// SYNTHETIC: GAMEMD 0x00659090 SYMBOL
// _$E77
// SYNTHETIC: GAMEMD 0x006590D0 SYMBOL
// _$E75
DynamicVectorClass<RadBeam *> RadBeamArray;

// FUNCTION: GAMEMD 0x00659110
RadBeam * RadBeam::Allocate(int type)
{
	RadBeam * beam = new RadBeam(type);
	RadBeamArray.Add(beam);
	beam->unknown_18 = (type == 1) ? 10 : 20;
	return(beam);
}

// FUNCTION: GAMEMD 0x006593F0
RadBeam::RadBeam(int type) :
	unknown_0(NULL),
	Owner(NULL),
	unknown_8(0),
	unknown_9(1),
	ZAdjust(0),
	Type(type),
	unknown_18(0),
	Color(0, 0, 0),
	SourceLocation(0, 0, 0),
	TargetLocation(0, 0, 0),
	Period(0),
	Amplitude(0),
	unknown_48(0),
	unknown_50(0),
	unknown_54(1),
	unknown_58(false),
	unknown_7C(0),
	Tint(0, 0, 0)
{
}

// FUNCTION: GAMEMD 0x00659470
void RadBeam::FUN_00659470(RGBClass const & color)
{
	Color = color;
}

// FUNCTION: GAMEMD 0x00659490
void RadBeam::FUN_00659490(int zadjust)
{
	ZAdjust = zadjust;
}

// FUNCTION: GAMEMD 0x006594A0
void RadBeam::FUN_006594A0(Coord const & coord)
{
	SourceLocation = coord;
}

// FUNCTION: GAMEMD 0x006594C0
void RadBeam::FUN_006594C0(Coord const & coord)
{
	TargetLocation = coord;
}

// FUNCTION: GAMEMD 0x006594E0
void RadBeam::FUN_006594E0(int period)
{
	Period = period;
}

// FUNCTION: GAMEMD 0x006594F0
void RadBeam::FUN_006594F0(double amplitude)
{
	Amplitude = amplitude;
}

// FUNCTION: GAMEMD 0x00659510
void RadBeam::FUN_00659510(double value)
{
	unknown_48 = value;
}

// FUNCTION: GAMEMD 0x00659530
void RadBeam::FUN_00659530(int value)
{
	unknown_50 = value;
}

// FUNCTION: GAMEMD 0x00659540
void RadBeam::FUN_00659540(int value)
{
	unknown_54 = value;
}

// FUNCTION: GAMEMD 0x00659550
void RadBeam::FUN_00659550(bool value)
{
	unknown_58 = value;
}

// FUNCTION: GAMEMD 0x00659560
void RadBeam::FUN_00659560(void)
{
	unknown_9 = 0;
}

// FUNCTION: GAMEMD 0x00659570
unsigned char RadBeam::FUN_00659570(void)
{
	return(unknown_9);
}

// FUNCTION: GAMEMD 0x00659580
void RadBeam::FUN_00659580(int value)
{
	unknown_18 = value;
}

class Tactical
{
	public:
		static int Z_Lepton_To_Pixel(int lepton);
		bool Coord_To_Pixel(Coord const & coord, Point2D & pixel);
};

class DSurface : public Surface
{
	public:
		static DSurface * Temp;
		static Rect ViewBounds;
};

extern bool __fastcall Line_In_Bounds(Point2D & start, Point2D & end, Rect const & bounds);
extern const double DAT_007F04C0;
extern const double DAT_007E2800;
extern const double DAT_00839530;
extern const double DAT_007E1738;

struct Drawing {
	static unsigned char RedShiftRight;
	static unsigned char GreenShiftRight;
	static unsigned char BlueShiftRight;
	static int RedShiftLeft;
	static int GreenShiftLeft;
	static int BlueShiftLeft;
};

// FUNCTION: GAMEMD 0x00659590
void RadBeam::FUN_00659590(Coord const & from, Coord const & to, short color)
{
	Point2D p1;
	TacticalMap->Coord_To_Pixel(from, p1);
	Point2D p2;
	TacticalMap->Coord_To_Pixel(to, p2);
	Rect bounds = DSurface::ViewBounds;
	if (Line_In_Bounds(p1, p2, bounds)) {
		int d1 = ZAdjust - Tactical::Z_Lepton_To_Pixel(from.Z) - 2;
		int d2 = -2 - Tactical::Z_Lepton_To_Pixel(to.Z);
		DSurface::Temp->Draw_Depth_Shaded_Line(DSurface::ViewBounds, p1, p2, color, d1, d2, false);
	}
}

// FUNCTION: GAMEMD 0x006599C0
void RadBeam::FUN_006599C0(void)
{
	Delta = TargetLocation - SourceLocation;
	Length = fastmath::sqrt(Delta.X * Delta.X + Delta.Y * Delta.Y + Delta.Z * Delta.Z);
}

// FUNCTION: GAMEMD 0x00659CA0
bool RadBeam::FUN_00659CA0(void)
{
	if (!unknown_8) {
		unknown_8 = 1;
		FUN_00659E30();
	}
	if (unknown_9) {
		FUN_00659EE0();
		unknown_88 = 0;
		if (unknown_8C > 0) {
			do {
				FUN_00659FF0();
				short color = (Tint.Get_Blue() >> Drawing::BlueShiftRight << Drawing::BlueShiftLeft)
					| (Tint.Get_Red() >> Drawing::RedShiftRight << Drawing::RedShiftLeft)
					| (Tint.Get_Green() >> Drawing::GreenShiftRight << Drawing::GreenShiftLeft);
				Coord * b = &PointB;
				Coord * a = &PointA;
				Point2D p1;
				TacticalMap->Coord_To_Pixel(*a, p1);
				Point2D p2;
				TacticalMap->Coord_To_Pixel(*b, p2);
				Rect bounds = DSurface::ViewBounds;
				if (Line_In_Bounds(p1, p2, bounds)) {
					int d1 = ZAdjust - Tactical::Z_Lepton_To_Pixel(a->Z) - 2;
					int d2 = -2 - Tactical::Z_Lepton_To_Pixel(b->Z);
					DSurface::Temp->Draw_Depth_Shaded_Line(DSurface::ViewBounds, p1, p2, color, d1, d2, false);
				}
				unknown_88++;
			} while (unknown_88 < unknown_8C);
		}
	}
	return ++unknown_7C >= Period;
}

// FUNCTION: GAMEMD 0x00659E30
void RadBeam::FUN_00659E30(void)
{
	Delta = TargetLocation - SourceLocation;
	Length = fastmath::sqrt(Delta.X * Delta.X + Delta.Y * Delta.Y + Delta.Z * Delta.Z);
	unknown_8C = (int)(Length / unknown_18);
	Coord step(Delta.X, Delta.Y, Delta.Z);
	step /= unknown_8C;
	AnotherLocation = step;
}

// FUNCTION: GAMEMD 0x00659EE0
void RadBeam::FUN_00659EE0(void)
{
	int period = Period;
	double phase;
	if (unknown_7C < period / 2) {
		phase = unknown_7C * 2;
	} else {
		phase = period - (unknown_7C * 2) % period;
	}
	unknown_80 = phase / (double)period * Amplitude;
	Delta = Delta + AnotherLocation * (int)(unknown_50 * unknown_7C * unknown_48);
	Length = fastmath::sqrt(Delta.X * Delta.X + Delta.Y * Delta.Y + Delta.Z * Delta.Z);
	unknown_8C = (int)(Length / unknown_18);
	Coord step(Delta.X, Delta.Y, Delta.Z);
	step /= unknown_8C;
	AndAnotherLocation = step;
}

// TEMPLATE: GAMEMD 0x0065A1B0
// ??_0?$TPoint3D@H@@QAEABV0@H@Z
template class TPoint3D<int>;

// TEMPLATE: GAMEMD 0x0065A1E0
// ?Clear@?$DynamicVectorClass@PAVRadBeam@@@@UAEXXZ
template class DynamicVectorClass<RadBeam *>;

// TEMPLATE: GAMEMD 0x0065A210
// ?ID@?$DynamicVectorClass@PAVRadBeam@@@@UAEHPBQAVRadBeam@@@Z

// TEMPLATE: GAMEMD 0x0065A230
// ?GetItem@?$VectorClass@PAVRadBeam@@@@UBEPAVRadBeam@@H@Z

// TEMPLATE: GAMEMD 0x0065A240
// ??8?$VectorClass@PAVRadBeam@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0065A280
// ?Resize@?$VectorClass@PAVRadBeam@@@@UAE_NHPBQAVRadBeam@@@Z

// TEMPLATE: GAMEMD 0x0065A330
// ?Clear@?$VectorClass@PAVRadBeam@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0065A360
// ?ID@?$VectorClass@PAVRadBeam@@@@UAEHABQAVRadBeam@@@Z

// TEMPLATE: GAMEMD 0x0065A3A0
// ?ID@?$VectorClass@PAVRadBeam@@@@UAEHPBQAVRadBeam@@@Z

// TEMPLATE: GAMEMD 0x0065A3C0
// ?Resize@?$DynamicVectorClass@PAVRadBeam@@@@UAE_NHPBQAVRadBeam@@@Z

// TEMPLATE: GAMEMD 0x0065A470
// ?ID@?$DynamicVectorClass@PAVRadBeam@@@@UAEHABQAVRadBeam@@@Z

// SYNTHETIC: GAMEMD 0x0065A4A0
// ??_G?$VectorClass@PAVRadBeam@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0065A4F0
// ??_G?$DynamicVectorClass@PAVRadBeam@@@@UAEPAXI@Z
