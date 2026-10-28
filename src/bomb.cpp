// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
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
#include "swizzle.h"
#include "techtype.h"
#include "voc.h"

// SYNTHETIC: GAMEMD 0x00438340 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00438370 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00438390 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004383B0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004383D0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004383F0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00438410 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00438450 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00438480 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004384B0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004384E0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004384F0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00438510 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00438530 SYMBOL
// _$E47

namespace Unsorted { extern int CurrentFrame; }

AudioEvent * __fastcall FUN_00750D40(Coord const & coord, AudioController * controller);

extern "C" const CLSID CLSID_BombClass;

DynamicVectorClass<BombClass *> Bombs;

// SYNTHETIC: GAMEMD 0x00438550 SYMBOL
// _$E195

// SYNTHETIC: GAMEMD 0x00438590 SYMBOL
// _$E193

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

// FUNCTION: GAMEMD 0x00438B40
HRESULT STDMETHODCALLTYPE BombClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) BombClass(NoInitClass());
		Audio.FUN_00405BE0();
		ShouldPlayTickingSound = false;
		Swizzle_Pointer(&OwnerHouse);
		Swizzle_Pointer(&Owner);
		Swizzle_Pointer(&Target);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00438BD0
HRESULT STDMETHODCALLTYPE BombClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x00438BF0
void BombListClass::Update(void)
{
	int i;
	for (i = Bombs.Count() - 1; i >= 0; i--) {
		BombClass * bomb = Bombs[i];
		if (bomb == NULL) {
			Bombs.Delete_Index(i);
		} else if (bomb->Target == NULL) {
			delete bomb;
			Bombs.Delete_Index(i);
		}
	}

	for (i = Bombs.Count() - 1; i >= 0; i--) {
		BombClass * bomb = Bombs[i];
		if (bomb->TickSound != -1) {
			if (bomb->Target->IsInLimbo) {
				bomb->Audio.Stop();
				bomb->ShouldPlayTickingSound = false;
			} else if (!bomb->ShouldPlayTickingSound) {
				Coord coord = bomb->Target->Position;
				VocClass::PlayAt(bomb->TickSound, coord, &bomb->Audio);
				bomb->ShouldPlayTickingSound = true;
			} else {
				Coord coord = bomb->Target->Position;
				FUN_00750D40(coord, &bomb->Audio);
			}
		}
	}

	if (UpdateDelay > 0) {
		UpdateDelay--;
		return;
	}
	UpdateDelay = 45;

	for (int k = Bombs.Count() - 1; k >= 0; k--) {
		BombClass * bomb = Bombs[k];
		bool wasvisible = bomb->Target->BombVisible;
		bool visible = false;
		if (bomb->OwnerHouse->Is_Player_Control()) {
			visible = true;
		} else {
			for (int d = 0; d < Detectors.Count(); d++) {
				TechnoClass * detector = Detectors[d];
				if (detector->Owner_HouseClass()->Is_Player_Control()) {
					Coord diff = detector->Center_Coord() - bomb->Target->Center_Coord();
					int dist = (int)fastmath::sqrt((double)diff.X * diff.X + (double)diff.Y * diff.Y + (double)diff.Z * diff.Z);
					if (dist < ((TechnoTypeClass *)detector->Techno_Type_Class())->BombSight << 8) {
						visible = true;
					}
				}
				if (visible) break;
			}
		}
		bomb->Target->BombVisible = visible;
		if (visible != wasvisible) {
			bomb->Target->IsToDisplay = true;
		}
	}
}

// FUNCTION: GAMEMD 0x00438E60
void BombListClass::FUN_00438E60(void)
{
}

// FUNCTION: GAMEMD 0x00438E70
void BombListClass::Plant(TechnoClass * source, ObjectClass * target)
{
	if (source != NULL && source->Fetch_RTTI() == RTTI_INFANTRY) {
		if (target != NULL && target->AttachedBomb == NULL) {
			BombClass * bomb = new BombClass;
			bomb->OwnerHouse = source->Owner_HouseClass();
			bomb->Owner = source;
			bomb->Target = target;
			bomb->PlantingFrame = Unsorted::CurrentFrame;
			bomb->DetonationFrame = Rule->IvanTimedDelay + bomb->PlantingFrame;
			bomb->DeathBomb = false;
			target->AttachedBomb = bomb;
			Bombs.Add(bomb);
			UpdateDelay = 1;
			if (Rule->BombAttachSound != -1 && source->House->Is_Player_Control()) {
				Coord coord = target->Position;
				VocClass::PlayAt(Rule->BombAttachSound, coord, NULL);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00439030
void BombListClass::FUN_00439030(BombClass * bomb)
{
	Bombs.Add(bomb);
}

// FUNCTION: GAMEMD 0x00439080
void BombListClass::AddDetector(TechnoClass * detector)
{
	Detectors.Add(detector);
}

// FUNCTION: GAMEMD 0x004390D0
void BombListClass::RemoveDetector(TechnoClass * detector)
{
	Detectors.Delete(detector);
}

// FUNCTION: GAMEMD 0x00439110
void BombListClass::FUN_00439110(void)
{
	UpdateDelay = 45;
	Bombs.Delete_All();
	Detectors.Delete_All();
}

// FUNCTION: GAMEMD 0x00439150
void BombListClass::PointerGotInvalid(AbstractClass * target)
{
	Bombs.Delete((BombClass *)target);
	for (int index = 0; index < Bombs.Count(); index++) {
		BombClass * bomb = Bombs[index];
		if (bomb != NULL && bomb->Owner == target) {
			bomb->Owner = NULL;
		}
	}
}

// FUNCTION: GAMEMD 0x004391C0
HRESULT BombListClass::Save(IStream * stream)
{
	int count = Bombs.Count();
	HRESULT result = stream->Write(&count, sizeof(count), NULL);
	if (SUCCEEDED(result)) {
		for (int index = 0; index < count; index++) {
			result = stream->Write(&Bombs[index], sizeof(BombClass *), NULL);
			if (FAILED(result)) {
				return(result);
			}
		}
		count = Detectors.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (SUCCEEDED(result)) {
			for (int index = 0; index < count; index++) {
				result = stream->Write(&Detectors[index], sizeof(TechnoClass *), NULL);
				if (FAILED(result)) {
					return(result);
				}
			}
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00439260
HRESULT BombListClass::Load(IStream * stream)
{
	int count;
	int index;
	BombClass * bomb;
	TechnoClass * detector;
	HRESULT result = stream->Read(&count, sizeof(count), NULL);
	if (FAILED(result)) {
		return(result);
	}
	for (index = 0; index < count; index++) {
		result = stream->Read(&bomb, sizeof(bomb), NULL);
		if (FAILED(result)) {
			return(result);
		}
		Bombs.Add(bomb);
	}
	for (index = 0; index < count; index++) {
		Swizzle_Pointer(&Bombs[index]);
	}

	result = stream->Read(&count, sizeof(count), NULL);
	if (SUCCEEDED(result)) {
		for (int item = 0; item < count; item++) {
			result = stream->Read(&detector, sizeof(detector), NULL);
			if (FAILED(result)) {
				return(result);
			}
			Detectors.Add(detector);
		}
		for (index = 0; index < count; index++) {
			Swizzle_Pointer(&Detectors[index]);
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004393D0
int BombClass::Fetch_Object_Size(void) const
{
	return sizeof(BombClass);
}

// FUNCTION: GAMEMD 0x004393E0
RTTIType BombClass::Fetch_RTTI(void) const
{
	return RTTI_BOMB;
}

// SYNTHETIC: GAMEMD 0x004393F0
// ??_GBombClass@@UAEPAXI@Z
