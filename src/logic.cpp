// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "logic.h"
#include "_bench.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "_vanim.h"
#include "aircraft.h"
#include "alphashp.h"
#include "anim.h"
#include "bench.h"
#include "blight.h"
#include "building.h"
#include "bullet.h"
#include "empulse.h"
#include "factory.h"
#include "globals.h"
#include "incdec.h"
#include "infantry.h"
#include "ion.h"
#include "ionblast.h"
#include "laser.h"
#include "light.h"
#include "mono.h"
#include "object.h"
#include "ovrlight.h"
#include "partsys.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "tactical.h"
#include "tag.h"
#include "terrain.h"
#include "tiberium.h"
#include "unit.h"
#include "vanim.h"
#include "vein.h"
#include "wave.h"
#include "bench.hh"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007ED368;
extern const double DAT_007ED370;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0055AD50 $E122
double DAT_00ABCCE0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0055AD80 $E125
double DAT_00ABCD30 = DAT_00ABCCE0 / DAT_007ED368;

// FUNCTION: 0x0055ADA0 $E128
double DAT_00ABCD18 = DAT_007E1718 / DAT_00ABCD30;

// FUNCTION: 0x0055ADC0 $E131
double DAT_00ABCD10 = DAT_007E1720 * DAT_007ED370;

// FUNCTION: 0x0055ADE0 $E134
double DAT_00ABCCE8 = DAT_007E1728 * DAT_007ED370;

// FUNCTION: 0x0055AE00 $E137
double DAT_00ABCCC0 = DAT_007E1730 * DAT_007ED370;

// FUNCTION: 0x0055AE20 $E140
int DAT_00ABCD38 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABCCC0 - DAT_00ABCCE8) * DAT_00ABCCE0));

// FUNCTION: 0x0055AE60 $E143
double DAT_00ABCCF0 = FUN_004CADE0(DAT_00ABCD38 * DAT_007E1740);

// FUNCTION: 0x0055AE90 $E146
double DAT_00ABCD08 = FUN_004CADE0((DAT_00ABCD38 * 2) / DAT_00ABCCE0);

// FUNCTION: 0x0055AEC0 $E149
int DAT_00ABCD2C = (int)(DAT_00ABCD38 * 4 + DAT_007E1738);

// FUNCTION: 0x0055AEF0 $E152
ShortPair DAT_00ABCCD8;

// FUNCTION: 0x0055AF00 $E155
IntTriple DAT_00ABCCF8;

namespace Detail { extern unsigned MinFrameRate; extern unsigned BufferZoneWidth; unsigned GetMinFrameRate(void); }
namespace FPSCounter { extern unsigned CurrentFrameRate; extern bool ReducedEffects; }
extern int DAT_00ABCD28;
extern int DAT_00ABCD20;
extern int DAT_00ABCD24;

// FUNCTION: 0x0055AF20
void __cdecl FUN_0055AF20(void)
{
	DAT_00ABCD28 = 0;
	DAT_00ABCD20 = 0x80;
	DAT_00ABCD24 = 0x80;
}

// FUNCTION: 0x0055AF40
void __fastcall FUN_0055AF40(int value)
{
	Detail::MinFrameRate = value;
}

// FUNCTION: 0x0055AF50
void __fastcall FUN_0055AF50(int value)
{
	Detail::BufferZoneWidth = value;
}

// FUNCTION: 0x0055AF60
unsigned Detail::GetMinFrameRate(void)
{
	if (FPSCounter::ReducedEffects) {
		if (FPSCounter::CurrentFrameRate >= Detail::MinFrameRate + Detail::BufferZoneWidth) {
			FPSCounter::ReducedEffects = false;
			return Detail::MinFrameRate;
		}
		return Detail::MinFrameRate + Detail::BufferZoneWidth;
	}
	if (FPSCounter::CurrentFrameRate < Detail::MinFrameRate) {
		FPSCounter::ReducedEffects = true;
		return Detail::MinFrameRate + Detail::BufferZoneWidth;
	}
	return Detail::MinFrameRate;
}

// FUNCTION: 0x0055B720
void LogicClass::Environment_AI(void)
{
	if (Rule->IsShroudGrow && Rule->ShroudRate != 0 && Scen->ShroudTimer == 0) {
		Scen->ShroudTimer = TICKS_PER_MINUTE * Rule->ShroudRate;
		Map.Encroach_Shadow();
	}

	if (Scen->Special.IsFogOfWar && Rule->FogRate != 0 && Scen->FogTimer == 0) {
		Scen->FogTimer = TICKS_PER_MINUTE * Rule->FogRate;
		Map.Encroach_Fog();
	}

	for (int index = 0; index < Count(); index++) {
		FootClass *foot = (FootClass *)(*this)[index];
		if (foot->Is_Foot() && foot->Locomotion != NULL && !foot->IsSinking) {
			foot->Locomotion->Process();
		}
	}
}

// FUNCTION: 0x0055B880
void LogicClass::Detach(AbstractClass const * target, bool all)
{
	if (Is_Target_Tag(target)) {
		for (int index = 0; index < LogicTags.Count(); index++) {
			if (target == (AbstractClass *)LogicTags[index]) {
				LogicTags.Delete_Index(index);
				index--;
			}
		}
	}
}

// FUNCTION: 0x0055B8F0
void FUN_0055B8F0(void)
{
	int index;
	for (index = Anims.Count() - 1; index >= 0; index--) {
		AnimClass * anim = Anims[index];
		if (anim->IsSubmittedToLayer) anim->AI();
	}
	for (index = BuildingLights.Count() - 1; index >= 0; index--) {
		BuildingLightClass * blight = BuildingLights[index];
		if (blight->IsSubmittedToLayer) blight->AI();
	}
	for (index = Bullets.Count() - 1; index >= 0; index--) {
		BulletClass * bullet = Bullets[index];
		if (bullet->IsSubmittedToLayer) bullet->AI();
	}
	for (index = Buildings.Count() - 1; index >= 0; index--) {
		BuildingClass * building = Buildings[index];
		if (building->IsSubmittedToLayer) building->AI();
	}
	for (index = Aircraft.Count() - 1; index >= 0; index--) {
		AircraftClass * aircraft = Aircraft[index];
		if (aircraft->IsSubmittedToLayer) aircraft->AI();
	}
	for (index = Infantry.Count() - 1; index >= 0; index--) {
		InfantryClass * infantry = Infantry[index];
		if (infantry->IsSubmittedToLayer) infantry->AI();
	}
	for (index = Units.Count() - 1; index >= 0; index--) {
		UnitClass * unit = Units[index];
		if (unit->IsSubmittedToLayer) unit->AI();
	}
	for (index = ParticleSystems.Count() - 1; index >= 0; index--) {
		ParticleSystemClass * partsys = ParticleSystems[index];
		if (partsys->IsSubmittedToLayer) partsys->AI();
	}
	for (index = Terrains.Count() - 1; index >= 0; index--) {
		TerrainClass * terrain = Terrains[index];
		if (terrain->IsSubmittedToLayer) terrain->AI();
	}
	for (index = VoxelAnims.Count() - 1; index >= 0; index--) {
		VoxelAnimClass * voxelanim = VoxelAnims[index];
		if (voxelanim->IsSubmittedToLayer) voxelanim->AI();
	}
	for (index = Waves.Count() - 1; index >= 0; index--) {
		WaveClass * wave = Waves[index];
		if (wave->IsSubmittedToLayer) wave->AI();
	}
}

// FUNCTION: 0x0055BAA0
bool LogicClass::Submit(ObjectClass const * object, bool sort)
{
	if (object->IsSubmittedToLayer) {
		return(true);
	}

	if (BASECLASS::Submit(object, sort)) {
		((ObjectClass *)object)->IsSubmittedToLayer = true;
		return(true);
	}

	return(false);
}

// FUNCTION: 0x0055BAE0
void LogicClass::Remove(ObjectClass * object)
{
	if (object->IsSubmittedToLayer) {
		Delete(object);
		object->IsSubmittedToLayer = false;
	}
}
