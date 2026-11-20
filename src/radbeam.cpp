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
	beam->SegmentLength = (type == 1) ? 10 : 20;
	return(beam);
}

// FUNCTION: GAMEMD 0x006593F0
RadBeam::RadBeam(int type) :
	Firer(NULL),
	Owner(NULL),
	IsInitialized(0),
	IsVisible(1),
	ZAdjust(0),
	Type(type),
	SegmentLength(0),
	Color(0, 0, 0),
	SourceLocation(0, 0, 0),
	TargetLocation(0, 0, 0),
	Period(0),
	Amplitude(0),
	unknown_48(0),
	unknown_50(0),
	FrameStep(1),
	unknown_58(false),
	Frame(0),
	Tint(0, 0, 0)
{
}

// FUNCTION: GAMEMD 0x00659470
void RadBeam::Set_Color(RGBClass const & color)
{
	Color = color;
}

// FUNCTION: GAMEMD 0x00659490
void RadBeam::Set_Z_Adjust(int zadjust)
{
	ZAdjust = zadjust;
}

// FUNCTION: GAMEMD 0x006594A0
void RadBeam::Set_Source(Coord const & coord)
{
	SourceLocation = coord;
}

// FUNCTION: GAMEMD 0x006594C0
void RadBeam::Set_Target(Coord const & coord)
{
	TargetLocation = coord;
}

// FUNCTION: GAMEMD 0x006594E0
void RadBeam::Set_Period(int period)
{
	Period = period;
}

// FUNCTION: GAMEMD 0x006594F0
void RadBeam::Set_Amplitude(double amplitude)
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
	FrameStep = value;
}

// FUNCTION: GAMEMD 0x00659550
void RadBeam::FUN_00659550(bool value)
{
	unknown_58 = value;
}

// FUNCTION: GAMEMD 0x00659560
void RadBeam::Hide(void)
{
	IsVisible = 0;
}

// FUNCTION: GAMEMD 0x00659570
unsigned char RadBeam::Is_Visible(void)
{
	return(IsVisible);
}

// FUNCTION: GAMEMD 0x00659580
void RadBeam::Set_Segment_Length(int value)
{
	SegmentLength = value;
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
	if (!IsInitialized) {
		IsInitialized = 1;
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
	if (IsVisible) {
		FUN_00659A30();
		for (Segment = 0; double(Segment) * SegmentLength < Length; Segment++) {
			FUN_00659AC0();
			unsigned short green = Tint.Get_Green();
			short color = DSurface::Build_Hicolor_Pixel(Tint.Get_Red(), green, Tint.Get_Blue());
			FUN_00659590(PointA, PointB, color);
		}
	}
	if (unknown_58) {
		Frame += FrameStep;
		if (Frame <= 0 || Frame >= Period) {
			FrameStep = -FrameStep;
			if (Frame < 0) {
				Frame = 0;
			}
		}
		return(false);
	}
	return(++Frame >= Period);
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
	SegmentCount = (int)(Length / SegmentLength);
	Point3D step(Delta.X, Delta.Y, Delta.Z);
	step /= SegmentCount;
	double phase = Frame;
	CurrentAmplitude = phase / Period * Amplitude;
	AndAnotherLocation = step;
	PointA = SourceLocation;
	Tint = Color;
}

// FUNCTION: GAMEMD 0x00659CA0
bool RadBeam::FUN_00659CA0(void)
{
	if (!IsInitialized) {
		IsInitialized = 1;
		FUN_00659E30();
	}
	if (IsVisible) {
		FUN_00659EE0();
		Segment = 0;
		if (SegmentCount > 0) {
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
				Segment++;
			} while (Segment < SegmentCount);
		}
	}
	return ++Frame >= Period;
}

// FUNCTION: GAMEMD 0x00659E30
void RadBeam::FUN_00659E30(void)
{
	Delta = TargetLocation - SourceLocation;
	int squared_length = Delta.X * Delta.X + Delta.Y * Delta.Y + Delta.Z * Delta.Z;
	Length = fastmath::sqrt(squared_length);
	SegmentCount = (int)(Length / SegmentLength);
	Point3D step(Delta.X, Delta.Y, Delta.Z);
	step /= SegmentCount;
	AnotherLocation = step;
}

// FUNCTION: GAMEMD 0x00659EE0
void RadBeam::FUN_00659EE0(void)
{
	int period = Period;
	double phase;
	if (Frame < period / 2) {
		phase = Frame * 2;
	} else {
		phase = period - (Frame * 2) % period;
	}
	CurrentAmplitude = phase / (double)period * Amplitude;
	Delta = Delta + AnotherLocation * (int)(unknown_50 * Frame * unknown_48);
	int squared_length = Delta.X * Delta.X + Delta.Y * Delta.Y + Delta.Z * Delta.Z;
	Length = fastmath::sqrt(squared_length);
	SegmentCount = (int)(Length / SegmentLength);
	Point3D step(Delta.X, Delta.Y, Delta.Z);
	step /= SegmentCount;
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
void RadBeam::Update_All(void)
{
	int total = RadBeamArray.Count();
	int count = 0;
	int active = 0;
	for (int i = total - 1; i >= 0; --i) {
		RadBeam * beam = RadBeamArray[i];
		if (beam->Type == 2) {
			++count;
			if (beam->IsVisible)
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
			beam->Set_Segment_Length(step);
			finished = beam->FUN_00659650();
		} else if (beam->Type == 2) {
			if (beam->IsVisible) {
				if (NonCriticalRandomNumber(0, 99) < int(drop * 100.0)) {
					beam->Hide();
				}
			}
			finished = beam->FUN_00659CA0();
		} else {
			continue;
		}
		if (finished) {
			if (beam->Firer != NULL && beam->Firer->FiringRadBeam == beam) {
				beam->Firer->FiringRadBeam = NULL;
				beam->Firer = NULL;
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

// FUNCTION: GAMEMD 0x00659AC0
void RadBeam::FUN_00659AC0(void)
{
	double span = (Length < 360.0 ? 360.0 : Length) * 2.0;
	double i = Segment;
	double fraction = i / SegmentCount;
	double angle = fraction * span;
	double wave = fastmath::sin(angle * DAT_00839530) * CurrentAmplitude;
	Point3D offset = AndAnotherLocation * (int)i;
	PointA = SourceLocation + offset;
	double first_z = PointA.Z;
	first_z += wave;
	PointA.Z = first_z;
	double j = Segment + 1;
	fraction = j / SegmentCount;
	angle = fraction * span;
	wave = fastmath::sin(angle * DAT_00839530) * CurrentAmplitude;
	offset = AndAnotherLocation * (int)j;
	PointB = SourceLocation + offset;
	double next_z = PointB.Z;
	next_z += wave;
	PointB.Z = next_z;
	int height = (int)wave;
	unknown_B8 = abs(height) * 0.5 / Amplitude + 0.5;
	double red = Color.Get_Red() * unknown_B8;
	double green = Color.Get_Green() * unknown_B8;
	double blue = Color.Get_Blue() * unknown_B8;
	Tint.Set_Red(red);
	Tint.Set_Green(green);
	Tint.Set_Blue(blue);
}

// FUNCTION: GAMEMD 0x00659FF0
void RadBeam::FUN_00659FF0(void)
{
	double i = Segment;
	double fraction = i / SegmentCount;
	double angle = fraction * 180.0;
	double wave = fastmath::sin(angle * DAT_00839530) * CurrentAmplitude;
	Coord offset = AndAnotherLocation * (int)i;
	PointA = SourceLocation + offset;
	double first_z = PointA.Z;
	first_z += wave;
	PointA.Z = first_z;
	double j = Segment + 1;
	fraction = j / SegmentCount;
	angle = fraction * 180.0;
	wave = fastmath::sin(angle * DAT_00839530) * CurrentAmplitude;
	PointB = SourceLocation + AndAnotherLocation * (int)j;
	double next_z = PointB.Z;
	next_z += wave;
	PointB.Z = next_z;
	int height = (int)wave;
	unknown_B8 = abs(height) * 0.5 / Amplitude + 0.5;
	double red = Color.Get_Red() * unknown_B8;
	double green = Color.Get_Green() * unknown_B8;
	double blue = Color.Get_Blue() * unknown_B8;
	Tint.Set_Red(red);
	Tint.Set_Green(green);
	Tint.Set_Blue(blue);
}
