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
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0055AD30 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0055AD50 SYMBOL
// _$E123
// SYNTHETIC: GAMEMD 0x0055AD80 SYMBOL
// _$E126
// SYNTHETIC: GAMEMD 0x0055ADA0 SYMBOL
// _$E129
// SYNTHETIC: GAMEMD 0x0055ADC0 SYMBOL
// _$E132
// SYNTHETIC: GAMEMD 0x0055ADE0 SYMBOL
// _$E135
// SYNTHETIC: GAMEMD 0x0055AE00 SYMBOL
// _$E138
// SYNTHETIC: GAMEMD 0x0055AE20 SYMBOL
// _$E141
// SYNTHETIC: GAMEMD 0x0055AE60 SYMBOL
// _$E144
// SYNTHETIC: GAMEMD 0x0055AE90 SYMBOL
// _$E147
// SYNTHETIC: GAMEMD 0x0055AEC0 SYMBOL
// _$E150
// SYNTHETIC: GAMEMD 0x0055AEF0 SYMBOL
// _$E153
// SYNTHETIC: GAMEMD 0x0055AF00 SYMBOL
// _$E156
// SYNTHETIC: GAMEMD 0x0055AF20 SYMBOL
// _$E159

namespace Detail { extern unsigned MinFrameRate; extern unsigned BufferZoneWidth; unsigned GetMinFrameRate(void); }
namespace FPSCounter { extern unsigned CurrentFrameRate; extern bool ReducedEffects; }

// FUNCTION: GAMEMD 0x0055AF40
void __fastcall FUN_0055AF40(int value)
{
	Detail::MinFrameRate = value;
}

// FUNCTION: GAMEMD 0x0055AF50
void __fastcall FUN_0055AF50(int value)
{
	Detail::BufferZoneWidth = value;
}

// FUNCTION: GAMEMD 0x0055AF60
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

#include "bomblist.h"
#include "disklaser.h"
#include "kamikaze.h"
#include "radsite.h"

bool FUN_0053A110(void);
bool FUN_0053A120(void);
bool FUN_0053BAD0(void);
bool FUN_0053B400(void);
void FUN_0053A6C0(void);

// FUNCTION: GAMEMD 0x0055AFB0
void LogicClass::AI(void)
{
	int index;

	FramesThisSecond++;

	for (LogicTriggerID = 0; LogicTriggerID < LogicTags.Count(); LogicTriggerID++) {
		TagClass * trig = LogicTags[LogicTriggerID];

		if (Scen->IsCrateBeenPickedUp) {
			if (trig->Spring(TEVENT_PICKUP_CRATE_ANY)) continue;
		}

		if (Scen->IsGlobalChanged) {
			if (trig->Spring(TEVENT_GLOBAL_SET)) continue;
			if (trig->Spring(TEVENT_GLOBAL_CLEAR)) continue;
			if (trig->Spring(TEVENT_LOCAL_SET)) continue;
			if (trig->Spring(TEVENT_LOCAL_CLEAR)) continue;
		}

		if (Scen->IsAmbientLightChanged) {
			if (trig->Spring(TEVENT_AMBIENT_LESS_THAN)) continue;
			if (trig->Spring(TEVENT_AMBIENT_GREATER_THAN)) continue;
		}

		if (trig->Spring(TEVENT_TIME)) continue;
		if (trig->Spring(TEVENT_RANDOM_TIME)) continue;

		if (Scen->MissionTimer.Is_Active() && Scen->MissionTimer == 0) {
			if (trig->Spring(TEVENT_MISSION_TIMER_EXPIRED)) continue;
		}
	}

	if (Scen->MissionTimer.Is_Active() && Scen->MissionTimer == 0) {
		Scen->MissionTimer.Stop();
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
	}

	Scen->IsGlobalChanged = false;
	Scen->IsBridgeChanged = false;
	Scen->IsAmbientLightChanged = false;
	Scen->IsCrateBeenPickedUp = false;

	if (Rule->IsShroudGrow && Rule->ShroudRate != 0 && Scen->ShroudTimer == 0) {
		Scen->ShroudTimer = TICKS_PER_MINUTE * Rule->ShroudRate;
		Map.Encroach_Shadow();
	}

	if (Frame % 120 == 0) {
		Map.FUN_00578100();
	}

	if (Scen->Special.IsFogOfWar && Rule->FogRate != 0 && Scen->FogTimer == 0) {
		Scen->FogTimer = TICKS_PER_MINUTE * Rule->FogRate;
		Map.Encroach_Fog();
	}

	if (Scen->DesiredAmbientLight != Scen->CurrentAmbientLight && Rule->AmbientLightChangeRate != 0 && Scen->AmbientChangeTimer == 0) {
		if (FUN_0053A110() || FUN_0053A120() || FUN_0053BAD0()) {
			Scen->AmbientChangeTimer = Scen->NukeAmbientChangeRate;
		} else if (FUN_0053B400()) {
			Scen->AmbientChangeTimer = Scen->DominatorAmbientChangeRate;
		} else {
			Scen->AmbientChangeTimer = TICKS_PER_MINUTE * Rule->AmbientLightChangeRate;
		}

		Scen->DesiredAmbientLight = MAX(Scen->DesiredAmbientLight, 0);

		int value = 100.0 * Rule->AmbientLightChangeStep;

		if (Scen->DesiredAmbientLight < Scen->CurrentAmbientLight) {
			Scen->CurrentAmbientLight -= value;
			Scen->CurrentAmbientLight = MAX(Scen->CurrentAmbientLight, Scen->DesiredAmbientLight);
		} else {
			Scen->CurrentAmbientLight += value;
			Scen->CurrentAmbientLight = MIN(Scen->CurrentAmbientLight, Scen->DesiredAmbientLight);
		}
		Scen->IsAmbientLightChanged = true;
		Map.Update_Cell_Colors();
		Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
	}

	TiberiumClass::Tiberium_Growth();
	TiberiumClass::Tiberium_Spread();
	BombListInstance.Update();
	KamikazeInstance.Update();

	DynamicVectorClass<TeamClass *> teams;
	for (index = 0; index < Teams.Count(); index++) {
		teams.Add(Teams[index]);
	}

	for (index = 0; index < teams.Count(); index++) {
		teams[index]->AI();
	}

	for (index = DiskLasers.Count() - 1; index >= 0; index--) {
		DiskLasers[index]->AI();
	}

	SpotLightClass::Update_All();
	LaserDrawClass::Update_All();
	FUN_0053A6C0();

	for (index = RadSites.Count() - 1; index >= 0; index--) {
		RadSites[index]->AI();
	}

	LightSourceClass::Process_Lighting(6);
	EMPulseClass::Update_All();

	for (index = 0; index < Count(); index++) {
		(*this)[index]->AI();
	}

	if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
		for (index = 0; index < MoveFlashes.Count(); index++) {
			MoveFlashes[index]->AI();
		}
	}

	IonBlastClass::Update_All();
	AlphaShapeClass::Update_All();

	Map.Logic();

	TacticalMap->AI();

	for (index = 0; index < Factories.Count(); index++) {
		Factories[index]->AI();
	}

	for (HousesType house = HOUSE_FIRST; house < Houses.Count(); house++) {
		HouseClass * hptr = Houses[house];
		if (hptr) {
			hptr->AI();
		}
	}

	if (Map.Object_To_Follow() != NULL) {
		TacticalMap->Set_Tactical_Position(Coord(Map.Object_To_Follow()->Position));
	}
}

// FUNCTION: GAMEMD 0x0055B720
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

// FUNCTION: GAMEMD 0x0055B880
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

// FUNCTION: GAMEMD 0x0055B8F0
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

// FUNCTION: GAMEMD 0x0055BAA0
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

// FUNCTION: GAMEMD 0x0055BAE0
void LogicClass::Remove(ObjectClass * object)
{
	if (object->IsSubmittedToLayer) {
		Delete(object);
		object->IsSubmittedToLayer = false;
	}
}

// TEMPLATE: GAMEMD 0x0055BB40
// ??0?$VectorClass@PAVTeamClass@@@@QAE@HPBQAVTeamClass@@@Z
template class VectorClass<TeamClass *>;
