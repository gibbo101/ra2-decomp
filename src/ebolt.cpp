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

struct FileSystem {
	static ConvertClass * PALETTE_PAL;
};

DynamicVectorClass<EBolt *> EBoltArray;

#include "ccrand.h"

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

struct BoltSegment
{
	Coord Start[3];
	Coord End[3];
	int Length;
	int Deviation;
	int StartDepth;
	int EndDepth;
};

// FUNCTION: GAMEMD 0x004C1F20
void EBolt::FUN_004C1F20(Coord start, Coord end, int color)
{
	BoltSegment stack[8];
	Coord mids[3];
	Coord starts[3];
	Coord ends[3];
	int offsets[6];
	bool first = true;
	int depth = 0;

	int length = start.Distance_To(end);
	if (length != 0) {
		int threshold = (length * 102) >> 8;
		int deviation = (length * 23) >> 8;
		int start_z = unknown_18;
		BoltSegment * seg = stack;
		int end_z = 0;
		for (int i = 0; i < 3; i++) {
			starts[i] = start;
			ends[i] = end;
		}

		for (;;) {
			if (length > 64 && depth < 8) {
				for (int j = 0; j < 3; j++) {
					mids[j].X = (starts[j].X + ends[j].X) >> 1;
					mids[j].Y = (starts[j].Y + ends[j].Y) >> 1;
					mids[j].Z = (starts[j].Z + ends[j].Z) >> 1;
				}
				if (first) {
					first = false;
					double dev = deviation;
					for (int k = 0; k < 6; k++) {
						offsets[k] = fastmath::sin(Random * M_PI / (k + 7)) * dev;
					}
					int zoff = (offsets[2] + offsets[4] + deviation * 2) >> 1;
					for (int m = 0; m < 3; m++) {
						mids[m].X += offsets[0] + offsets[3];
						mids[m].Y += offsets[1] + offsets[5];
						mids[m].Z += zoff;
					}
				}
				if (length <= 128) {
					mids[0].X += NonCriticalRandomNumber(-1, 1) * deviation * 2;
					mids[0].Y += NonCriticalRandomNumber(-1, 1) * deviation * 2;
					mids[0].Z += NonCriticalRandomNumber(-1, 1) * deviation * 2;
				} else {
					mids[0].X += Sim_Random_Pick(-deviation, deviation);
					mids[0].Y += Sim_Random_Pick(-deviation, deviation);
					mids[0].Z += Sim_Random_Pick(-deviation, deviation);
				}
				if (length > threshold) {
					for (int n = 1; n < 3; n++) {
						mids[n].X = mids[0].X + (Sim_Random_Pick(-deviation, deviation) >> 1);
						mids[n].Y = mids[0].Y + (Sim_Random_Pick(-deviation, deviation) >> 1);
						mids[n].Z = mids[0].Z + (Sim_Random_Pick(-deviation, deviation) >> 1);
					}
				} else {
					for (int n2 = 1; n2 < 3; n2++) {
						mids[n2].X += Sim_Random_Pick(-deviation, deviation);
						mids[n2].Y += Sim_Random_Pick(-deviation, deviation);
						mids[n2].Z += Sim_Random_Pick(-deviation, deviation);
					}
				}
				length >>= 1;
				deviation >>= 1;
				int mid_z = (end_z + start_z) >> 1;
				for (int p = 0; p < 3; p++) {
					seg->Start[p] = mids[p];
					seg->End[p] = ends[p];
				}
				seg->Length = length;
				seg->Deviation = deviation;
				seg->StartDepth = mid_z;
				seg->EndDepth = end_z;
				seg++;
				depth++;
				memcpy(ends, mids, sizeof(ends));
				end_z = mid_z;
			} else {
				FUN_004C1E70(starts[1], ends[1], color, start_z, end_z);
				FUN_004C1E70(starts[2], ends[2], color, start_z, end_z);
				FUN_004C1E70(starts[0], ends[0], 15, start_z, end_z);
				if (--depth < 0) break;
				seg--;
				memcpy(ends, seg->End, sizeof(ends));
				memcpy(starts, seg->Start, sizeof(starts));
				length = seg->Length;
				deviation = seg->Deviation;
				start_z = seg->StartDepth;
				end_z = seg->EndDepth;
			}
		}
	}
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
	Random = NonCriticalRandomNumber(0, 0x100);
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

// FUNCTION: GAMEMD 0x004C2C30
// Coord::Coord

// TEMPLATE: GAMEMD 0x004C2C40
// ?Clear@?$DynamicVectorClass@PAVEBolt@@@@UAEXXZ
template class DynamicVectorClass<EBolt *>;

// TEMPLATE: GAMEMD 0x004C2C70
// ?ID@?$DynamicVectorClass@PAVEBolt@@@@UAEHPBQAVEBolt@@@Z

// TEMPLATE: GAMEMD 0x004C2C90
// ?GetItem@?$VectorClass@PAVEBolt@@@@UBEPAVEBolt@@H@Z

// TEMPLATE: GAMEMD 0x004C2CA0
// ??8?$VectorClass@PAVEBolt@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x004C2CE0
// ?Resize@?$VectorClass@PAVEBolt@@@@UAE_NHPBQAVEBolt@@@Z

// TEMPLATE: GAMEMD 0x004C2D90
// ?Clear@?$VectorClass@PAVEBolt@@@@UAEXXZ
template class VectorClass<EBolt *>;

// TEMPLATE: GAMEMD 0x004C2DC0
// ?ID@?$VectorClass@PAVEBolt@@@@UAEHABQAVEBolt@@@Z

// TEMPLATE: GAMEMD 0x004C2E00
// ?ID@?$VectorClass@PAVEBolt@@@@UAEHPBQAVEBolt@@@Z

// TEMPLATE: GAMEMD 0x004C2E20
// ?Resize@?$DynamicVectorClass@PAVEBolt@@@@UAE_NHPBQAVEBolt@@@Z

// TEMPLATE: GAMEMD 0x004C2ED0
// ?ID@?$DynamicVectorClass@PAVEBolt@@@@UAEHABQAVEBolt@@@Z

// SYNTHETIC: GAMEMD 0x004C2F00
// ??_G?$VectorClass@PAVEBolt@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x004C2F50
// ??_G?$DynamicVectorClass@PAVEBolt@@@@UAEPAXI@Z
