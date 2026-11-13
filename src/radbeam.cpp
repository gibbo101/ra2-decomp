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

#include "tactical.h"
#include "dsurface.h"

extern bool __fastcall Line_In_Bounds(Point2D & start, Point2D & end, Rect const & bounds);
extern const double DAT_007F04C0;
extern const double DAT_007E2800;
extern const double DAT_00839530;
extern const double DAT_007E1738;

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

// FUNCTION: GAMEMD 0x00659650
bool RadBeam::FUN_00659650(void)
{
	if (!unknown_8) {
		unknown_8 = 1;
		FUN_006599C0();
	}
	if (unknown_58 && Owner != NULL) {
		Coord target;
		if (!(Owner->AbstractFlags & 2)) {
			target = Owner->As_Coord();
		} else {
			target = ((ObjectClass *)Owner)->Target_Coord();
		}
		TargetLocation = target;
		FUN_006599C0();
	}
	if (unknown_9) {
		FUN_00659A30();
		for (unknown_88 = 0; double(unknown_88) * unknown_18 < Length; unknown_88++) {
			FUN_00659AC0();
			unsigned short green = Tint.Get_Green();
			short color = DSurface::Build_Hicolor_Pixel(Tint.Get_Red(), green, Tint.Get_Blue());
			FUN_00659590(PointA, PointB, color);
		}
	}
	if (unknown_58) {
		unknown_7C += unknown_54;
		if (unknown_7C <= 0 || unknown_7C >= Period) {
			unknown_54 = -unknown_54;
			if (unknown_7C < 0) {
				unknown_7C = 0;
			}
		}
		return(false);
	}
	return(++unknown_7C >= Period);
}

// FUNCTION: GAMEMD 0x006599C0
void RadBeam::FUN_006599C0(void)
{
	Delta = TargetLocation - SourceLocation;
	int squared_length = Delta.X * Delta.X + Delta.Y * Delta.Y + Delta.Z * Delta.Z;
	Length = fastmath::sqrt(squared_length);
}

// FUNCTION: GAMEMD 0x00659A30
void RadBeam::FUN_00659A30(void)
{
	unknown_8C = (int)(Length / unknown_18);
	Point3D step(Delta.X, Delta.Y, Delta.Z);
	step /= unknown_8C;
	double phase = unknown_7C;
	unknown_80 = phase / Period * Amplitude;
	AndAnotherLocation = step;
	PointA = SourceLocation;
	Tint = Color;
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
				short color = DSurface::Build_Hicolor_Pixel(Tint);
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
	int squared_length = Delta.X * Delta.X + Delta.Y * Delta.Y + Delta.Z * Delta.Z;
	Length = fastmath::sqrt(squared_length);
	unknown_8C = (int)(Length / unknown_18);
	Point3D step(Delta.X, Delta.Y, Delta.Z);
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
	int squared_length = Delta.X * Delta.X + Delta.Y * Delta.Y + Delta.Z * Delta.Z;
	Length = fastmath::sqrt(squared_length);
	unknown_8C = (int)(Length / unknown_18);
	Point3D step(Delta.X, Delta.Y, Delta.Z);
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

#include "cast.h"
#include "random.h"
#include "_rand.h"
#include "bound.h"
#include "session.h"

namespace FPSCounter {
extern unsigned CurrentFrameRate;
}

// FUNCTION: GAMEMD 0x006591B0
void RadBeam::FUN_006591B0(void)
{
	int total = RadBeamArray.Count();
	int count = 0;
	int active = 0;
	for (int i = total - 1; i >= 0; --i) {
		RadBeam * beam = RadBeamArray[i];
		if (beam->Type == 2) {
			++count;
			if (beam->unknown_9)
				++active;
		}
	}
	double requested_rate = Session.DesiredFrameRate;
	double rate = FPSCounter::CurrentFrameRate / requested_rate;
	rate = rate * rate * rate;
	double excess = double(active) / count - rate;
	double drop = 0.0;
	if (excess > 0.0)
		drop = count * excess;
	drop /= active;
	double inverse_rate = 1.0 / rate;
	for (i = total - 1; i >= 0; --i) {
		RadBeam * beam = RadBeamArray[i];
		bool finished;
		if (beam->Type == 1) {
			int step = int(inverse_rate * 10.0);
			if (step < 10)
				step = 10;
			else if (step > 100)
				step = 100;
			beam->FUN_00659580(step);
			finished = beam->FUN_00659650();
		} else if (beam->Type == 2) {
			if (beam->unknown_9) {
				if (NonCriticalRandomNumber(0, 99) < int(drop * 100.0)) {
					beam->FUN_00659560();
				}
			}
			finished = beam->FUN_00659CA0();
		} else {
			continue;
		}
		if (finished) {
			if (beam->unknown_0 != NULL && beam->unknown_0->FiringRadBeam == beam) {
				beam->unknown_0->FiringRadBeam = NULL;
				beam->unknown_0 = NULL;
			}
			if (beam->Owner != NULL && (beam->Owner->AbstractFlags & 1)) {
				if (As_Techno(beam->Owner)->FiringRadBeam == beam) {
					As_Techno(beam->Owner)->FiringRadBeam = NULL;
					beam->Owner = NULL;
				}
			}
			RadBeamArray.Delete(beam);
			delete beam;
		}
	}
}
