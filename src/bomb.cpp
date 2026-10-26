// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "bomb.h"
#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "bomblist.h"
#include "builtype.h"
#include "building.h"
#include "cell.h"
#include "combat.h"
#include "crc.h"
#include "globals.h"
#include "house.h"
#include "isotype.h"
#include "map.h"
#include "noinit.h"
#include "rules.h"
#include "techno.h"

namespace Unsorted { extern int CurrentFrame; }

extern "C" const CLSID CLSID_BombClass;

DynamicVectorClass<BombClass *> Bombs;

// SYNTHETIC: GAMEMD 0x00438550 SYMBOL
// _$E150

// SYNTHETIC: GAMEMD 0x00438590 SYMBOL
// _$E148

// FUNCTION: GAMEMD 0x004385D0
BombClass::BombClass(void) :
	BASECLASS(),
	ShouldPlayTickingSound(false)
{
	OwnerHouse = NULL;
	Target = NULL;
	Harmless = false;
	Audio.FUN_00405BE0();
	TickSound = Rule->BombTickingSound;
	Bombs.Add(this);
}

// FUNCTION: GAMEMD 0x00438670
BombClass::~BombClass(void)
{
	Audio.FUN_00405C00();
	Bombs.Delete(this);
}

// FUNCTION: GAMEMD 0x004386F0
BombClass::BombClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x00438720
void BombClass::Detonate(void)
{
	if (Target != NULL && !Harmless) {
		bool limbo = Target->IsInLimbo;
		Target->AttachedBomb = NULL;
		if (limbo) {
			Target->BombVisible = false;
			Harmless = true;
		} else {
			Target->BombVisible = false;
			Harmless = true;
			Coord coord = Target->Position;
			MapClass::DamageArea(coord, Rule->IvanDamage, Owner, Rule->IvanWarhead, true, NULL);
			new AnimClass(Combat_Anim(Rule->IvanDamage, Rule->IvanWarhead, Map[Cell(coord)].Land_Type(), coord), coord, 0, 1, ShapeFlags_Type(0x2600), Get_Explosion_Z(coord));
			if (Target != NULL && Target->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)Target)->Class->IsBridgeRepairHut) {
				bool found = false;
				for (int y = -2; y < 3; y++) {
					for (int x = -2; x < 3; x++) {
						IsometricTileType tile = Map[Cell(x, y) + Target->Get_Cell()].ITType;
						OverlayType overlay = Map[Cell(x, y) + Target->Get_Cell()].Overlay;
						if ((tile >= IsometricTileTypeClass::TrainBridgeSet && tile < IsometricTileTypeClass::TrainBridgeSet + 16) || (overlay >= 0x4A && overlay <= 0x65)) {
							found = true;
						}
					}
				}
				if (found) {
					Map.DestroyWoodBridgeAt(Target->Get_Cell());
				} else {
					Map.DestroyConcreteBridgeAt(Target->Get_Cell());
				}
			}
		}
		Owner = NULL;
		Target = NULL;
		OwnerHouse = NULL;
		Audio.End();
		ShouldPlayTickingSound = false;
	}
}

// FUNCTION: GAMEMD 0x004389B0
void BombClass::Disarm(void)
{
	if (Target != NULL) {
		Target->AttachedBomb = NULL;
		Target->BombVisible = false;
	}
	Harmless = true;
	Owner = NULL;
	Target = NULL;
	OwnerHouse = NULL;
	Audio.End();
	ShouldPlayTickingSound = false;
}

// FUNCTION: GAMEMD 0x004389F0
BOOL BombClass::Is_Death_Bomb(void) const
{
	return(DeathBomb);
}

// FUNCTION: GAMEMD 0x00438A00
int BombClass::Get_Current_Flicker_Frame(void) const
{
	if (DeathBomb == 1) return(12);
	int rate = Rule->IvanIconFlickerRate;
	int stage = (Unsorted::CurrentFrame - PlantingFrame) / (Rule->IvanTimedDelay / 6);
	stage *= 2;
	if (Unsorted::CurrentFrame % (rate * 2) >= rate) stage++;
	if (stage >= 12) stage = 11;
	return(stage);
}

// FUNCTION: GAMEMD 0x00438A70
bool BombClass::Time_To_Explode(void) const
{
	if (DeathBomb == 0 && Unsorted::CurrentFrame > DetonationFrame && !Harmless) return(true);
	return(false);
}

// FUNCTION: GAMEMD 0x00438A90
void BombClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	if (OwnerHouse != NULL) crc(OwnerHouse->Fetch_ID());
	if (Owner != NULL) crc(Owner->Fetch_ID());
	if (Target != NULL) crc(Target->Fetch_ID());
	crc(Harmless);
}

// FUNCTION: GAMEMD 0x00438B00
HRESULT STDMETHODCALLTYPE BombClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BombClass;
	return(S_OK);
}
