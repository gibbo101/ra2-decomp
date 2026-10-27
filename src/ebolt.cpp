// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "ebolt.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "convert.h"
#include "dsurface.h"
#include "globals.h"
#include "partsys.h"
#include "random.h"
#include "rules.h"
#include "tactical.h"
#include "techno.h"
#include "unit.h"
#include "xsurface.h"

// SYNTHETIC: GAMEMD 0x004C1B80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004C1BB0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004C1BD0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004C1BF0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004C1C10 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004C1C30 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004C1C50 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004C1C90 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004C1CC0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004C1CF0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004C1D20 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004C1D30 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004C1D50 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004C1D70 SYMBOL
// _$E42

namespace Randomizer { extern Random2Class Global; }

struct FileSystem {
	static ConvertClass * PALETTE_PAL;
};

DynamicVectorClass<EBolt *> EBoltArray;

// SYNTHETIC: GAMEMD 0x004C1D90 SYMBOL
// _$E103
// SYNTHETIC: GAMEMD 0x004C1DD0 SYMBOL
// _$E101

// FUNCTION: GAMEMD 0x004C1E10
EBolt::EBolt(void)
{
	Point1 = Coord(0, 0, 0);
	unknown_18 = 0;
	Lifetime = 0x10000;
	AlternateColor = false;
	Random = 0;
	Owner = NULL;
	WeaponSlot = 0;
	Point2 = Point1;
}

// FUNCTION: GAMEMD 0x004C1E50
void EBolt::ClearOwner(void)
{
	if (Owner != NULL) {
		((UnitClass *)Owner)->ElectricBolt = NULL;
		Owner = NULL;
	}
}

// FUNCTION: GAMEMD 0x004C1E70
void __stdcall FUN_004C1E70(Coord start, Coord end, int color, int start_depth, int end_depth)
{
	Point2D p1;
	Point2D p2;
	TacticalMap->Coord_To_Pixel(start, p1);
	TacticalMap->Coord_To_Pixel(end, p2);
	int d1 = start_depth - Tactical::Z_Lepton_To_Pixel(start.Z) - 2;
	int d2 = end_depth - Tactical::Z_Lepton_To_Pixel(end.Z) - 2;
	DSurface::Composite->Draw_Depth_Shaded_Line(DSurface::ViewBounds, p1, p2, FileSystem::PALETTE_PAL->Convert_Pixel(color), d1, d2);
}

// FUNCTION: GAMEMD 0x004C2790
void EBolt::FUN_004C2790(void)
{
	if (Lifetime != 0) {
		Point2D p1;
		Point2D p2;
		TacticalMap->Coord_To_Pixel(Point1, p1);
		TacticalMap->Coord_To_Pixel(Point2, p2);
		if (Clip_Line_To_Rect(p1, p2, DSurface::ViewBounds)) {
			FUN_004C1F20(Point1, Point2, AlternateColor ? 5 : 10);
		}
		Random++;
	}
	Lifetime >>= 1;
}

// FUNCTION: GAMEMD 0x004C2830
void EBolt::FUN_004C2830(void)
{
	for (int i = EBoltArray.Count() - 1; i >= 0; i--) {
		EBolt * bolt = EBoltArray[i];
		if (bolt->Owner != NULL) {
			Coord source = bolt->Owner->Fire_Coord(bolt->WeaponSlot, Coord(0, 0, 0));
			if (source != COORD_NONE) {
				bolt->Point1 = source;
			}
		}
		bolt->FUN_004C2790();
		if (bolt->Lifetime == 0) {
			EBoltArray.Delete(bolt);
			delete bolt;
		}
	}
}

// FUNCTION: GAMEMD 0x004C29E0
void __fastcall FUN_004C29E0(void)
{
	for (int i = 0; i < EBoltArray.Count(); i++) {
		delete EBoltArray[i];
	}
	EBoltArray.Clear();
}

// FUNCTION: GAMEMD 0x004C2A60
void EBolt::Fire(Coord start, Coord end, int zadjust)
{
	Point1 = start;
	Point2 = end;
	unknown_18 = zadjust;
	Random = Randomizer::Global(0, 0x100);
	EBoltArray.Add(this);
	new ParticleSystemClass(Rule->DefaultSparkSystem, end, NULL, NULL, COORD_NONE, NULL);
}

// FUNCTION: GAMEMD 0x004C2B40
Coord EBolt::GetSourceCoords(void) const
{
	if (Owner != NULL) {
		return(Owner->Fire_Coord(WeaponSlot, Coord(0, 0, 0)));
	}
	return(COORD_NONE);
}

// FUNCTION: GAMEMD 0x004C2BD0
void EBolt::SetOwner(TechnoClass * owner, int weapon)
{
	if (owner != NULL && owner->RTTI == RTTI_UNIT && owner->IsActive && !owner->IsInLimbo) {
		Owner = owner;
		WeaponSlot = weapon;
	}
}

// FUNCTION: GAMEMD 0x004C2C10
EBolt::~EBolt(void)
{
	ClearOwner();
}
