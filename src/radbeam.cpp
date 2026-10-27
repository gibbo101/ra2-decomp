// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "radbeam.h"
#include "techno.h"

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
	unknown_C0(0),
	unknown_C1(0),
	unknown_C2(0)
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
