// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "parasite.h"
#include "anim.h"
#include "crc.h"
#include "foot.h"
#include "globals.h"
#include "notify.h"
#include "swizzle.h"
#include "_map.h"
#include "cell.h"
#include "house.h"
#include "inline.h"
#include "weapon.h"
#include "partsys.h"
#include "_rules.h"
#include "rules.h"
#include "warhead.h"
#include "scenario.h"
#include <new.h>

namespace Unsorted { extern bool ScenarioStarted; }

// SYNTHETIC: GAMEMD 0x00628F80 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00628FB0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00628FD0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00628FF0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00629010 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00629030 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00629050 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00629090 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006290C0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006290F0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00629120 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00629130 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00629150 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00629170 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x00629190 SYMBOL
// _$E128
// SYNTHETIC: GAMEMD 0x006291D0 SYMBOL
// _$E126
DynamicVectorClass<ParasiteClass *> Parasites;

// FUNCTION: GAMEMD 0x00629210
ParasiteClass::ParasiteClass(void)
{
	Parasites.Add(this);
}

// FUNCTION: GAMEMD 0x006292B0
ParasiteClass::ParasiteClass(FootClass * owner)
{
	Owner = owner;
	Victim = NULL;
	DamageDeliveryTimer = 0;
	SuppressionTimer = 0;
	GrappleAnimGotInvalid = false;
	GrappleAnimFrame = 0;
	GrappleAnimDelay = 0;
	GrappleAnim = NULL;
	GrappleState = 0;
	Parasites.Add(this);
}

// FUNCTION: GAMEMD 0x00629390
ParasiteClass::ParasiteClass(NoInitClass const & x) :
	AbstractClass(x)
{
}

// FUNCTION: GAMEMD 0x006293E0
ParasiteClass::~ParasiteClass(void)
{
	FUN_00629530();
	Parasites.Delete(this);
}

// FUNCTION: GAMEMD 0x006294D0
void ParasiteClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	if (GrappleAnim != NULL) {
		crc(GrappleAnim->Fetch_ID());
	}
	crc(GrappleState);
	crc(GrappleAnimFrame);
	crc(GrappleAnimDelay);
	crc(GrappleAnimGotInvalid);
}

// FUNCTION: GAMEMD 0x00629530
void ParasiteClass::FUN_00629530(void)
{
	GrappleAnimFrame = 0;
	GrappleAnimDelay = 0;
	GrappleState = 0;
	if (GrappleAnim != NULL) {
		GrappleAnim->Delete_Me();
		GrappleAnim = NULL;
	}
	if (GrappleAnimGotInvalid) {
		PointerExpiredNotification::NotifyInvalidAnim.Delete(this);
		GrappleAnimGotInvalid = false;
	}
}

// FUNCTION: GAMEMD 0x006295B0
HRESULT STDMETHODCALLTYPE ParasiteClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (result >= 0) {
		new (this) ParasiteClass(NoInitClass());
		if (GrappleAnim != NULL) {
			PointerExpiredNotification::NotifyInvalidAnim.Add(this);
		}
		Swizzle_Pointer(&Owner);
		Swizzle_Pointer(&Victim);
		Swizzle_Pointer(&GrappleAnim);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006296B0
HRESULT STDMETHODCALLTYPE ParasiteClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x006296D0
HRESULT STDMETHODCALLTYPE ParasiteClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ParasiteClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00629710
void ParasiteClass::FUN_00629710(void)
{
	GrappleAnimDelay = 0;
	GrappleAnimFrame = 0;
}

// FUNCTION: GAMEMD 0x00629720
bool ParasiteClass::UpdateGrapple(void)
{
	DirType dir = Victim->PrimaryFacing.Current();
	int base;
	int limit;
	switch (GrappleState) {
		case 0:
		case 1:
			base = 0;
			limit = 3;
			break;
		case 2:
			base = 80;
			limit = 4;
			break;
		default:
			base = 160;
			limit = 4;
			break;
	}
	if (++GrappleAnimDelay >= limit) {
		GrappleAnimDelay = 0;
		if (++GrappleAnimFrame >= 10) {
			return true;
		}
		if (GrappleAnim != NULL) {
			GrappleAnim->Animation.Set_Rate(128);
			GrappleAnim->Animation.Set_Stage(GrappleAnimFrame + base + dir.As_Dir8() * 10);
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x00629E90
void FootClass::CreateWakes(Coord coords)
{
	double rad = -PrimaryFacing.Current().As_Radian();
	double cosine = fastmath::cos(rad);
	double sine = fastmath::sin(rad);
	for (int i = 0; i < 3; i++) {
		double dx = Scen->RandomNumber(-180, 180);
		double dy = Scen->RandomNumber(-64, 64);
		int ox = (int)(dx * cosine - dy * sine);
		int oy = (int)(dx * sine + dy * cosine);
		Coord wake = coords + Coord(ox, oy, 0);
		new AnimClass(Rule->Wake, wake, 0, 1, ShapeFlags_Type(0x600), 0, false);
	}
}

// FUNCTION: GAMEMD 0x00629FD0
void ParasiteClass::AI(void)
{
	if (Victim == NULL) {
		return;
	}
	if (Owner->Techno_Type_Class()->Naval && Owner->Techno_Type_Class()->Organic) {
		UpdateSquid();
		return;
	}
	WeaponTypeClass * weapon = Owner->Get_Class_Weapon_Data(0)->Weapon;
	WarheadTypeClass const * warhead = weapon->WarheadPtr;
	Coord coord = Victim->Position;
	if (DamageDeliveryTimer != 0) {
		return;
	}
	DamageDeliveryTimer = weapon->ROF;
	Victim->ParalysisTimer = warhead->Paralyzes;
	int damage;
	if (Victim->Fetch_RTTI() == RTTI_INFANTRY) {
		damage = Victim->Strength;
		Victim->Take_Damage(damage, 0, warhead, Owner, true, true, NULL);
	} else {
		new ParticleSystemClass(Rule->DefaultSparkSystem, coord, NULL, NULL, COORD_NONE, NULL);
		DirType dir = Victim->PrimaryFacing.Current();
		if (weapon->Anim.Count() != 0) {
			AnimTypeClass const * type = weapon->Anim[dir.As_Dir8()];
			if (type != NULL) {
				new AnimClass(type, coord, 0, 1, ShapeFlags_Type(0x600), 0, false);
			}
		}
		dir = dir.Left_90();
		int side = Scen->RandomNumber(0, 1) ? -2 : 2;
		double rad = dir.As_Radian();
		Coord pushed((int)(fastmath::cos(rad) * side + coord.X), (int)(coord.Y - fastmath::sin(rad) * side), coord.Z);
		Victim->vt_entry_3D8(pushed, 1.5f, false);
		damage = weapon->Attack;
		Victim->Take_Damage(damage, 0, warhead, Owner, false, true, NULL);
	}
}

// FUNCTION: GAMEMD 0x0062A260
void ParasiteClass::Detach(AbstractClass const * target, bool all)
{
	if (target == Owner) {
		Owner = NULL;
		return;
	}
	if (target == Victim) {
		if (!Unsorted::ScenarioStarted) {
			Victim = NULL;
			return;
		}
		if (SuppressionTimer > 0 && !Owner->IsIronCurtained()) {
			Victim->ParasiteEatingMe = NULL;
			Victim = NULL;
			Owner->Delete_Me();
			return;
		}
		Coord coord = FUN_0062AC30();
		ScenarioInit++;
		if (coord != COORD_NONE && Owner->Unlimbo(coord, Victim->PrimaryFacing.Current().As_Dir256())) {
			ScenarioInit--;
			if (Owner->ShouldBeReselectOnUnlimbo && Owner->House->Is_Player_Control()) {
				Owner->ShouldBeReselectOnUnlimbo = false;
				Owner->Select();
			}
			if (Owner->OldTeam != NULL) {
				Owner->OldTeam->AddMember(Owner, false);
			}
			if (!Owner->HaveMegaMission()) {
				Owner->Assign_Archive_Target(NULL);
				Owner->Assign_Target(NULL);
				Owner->Assign_Destination(NULL, true);
			}
			Owner->Enter_Idle_Mode(false, true);
			Owner->UpdateSight(false, false, false, NULL, 0);
			Coord pos = Owner->Position;
			Map.RevealArea3(pos, Owner->LastSightRange - 3, Owner->LastSightRange + 3, 0);
			Victim = NULL;
		} else {
			ScenarioInit--;
			Owner->Strength = 0;
			Owner->Delete_Me();
			Victim = NULL;
		}
		return;
	}
	if (target == GrappleAnim) {
		GrappleAnim = NULL;
		GrappleAnimGotInvalid = true;
	}
}

// FUNCTION: GAMEMD 0x0062A4A0
void ParasiteClass::ExitUnit(void)
{
	bool naval = Owner->Techno_Type_Class()->Naval;
	int face = Victim->PrimaryFacing.Current().As_Dir8();
	DirType dir;
	if (face >= 0 && face <= 2) {
		dir = Victim->PrimaryFacing.Current().Right_90();
	} else {
		dir = Victim->PrimaryFacing.Current().Left_90();
	}
	Cell cell;
	Coord coord;
	CellClass * cellptr;
	if (naval) {
		cell = Adjacent_Cell(Victim->Get_Cell(), dir.As_Dir8());
		goto placed;
	}
	if (SuppressionTimer == 0) {
		cell = Victim->Get_Cell();
placed:
		cellptr = &Map[cell];
		if (!cellptr->Is_Clear_To_Move(Owner->Techno_Type_Class()->Speed, false, false, Map.Get_Cell_Zone(cell, Owner->Techno_Type_Class()->MZone, false), Owner->Techno_Type_Class()->MZone, -1, true)) {
			cell = Victim->Nearby_Location(NULL);
		}
		coord = COORD_NONE;
		if (!(cell == CELL_NONE)) {
			coord = Map[cell].Center_Coord();
		}
		if (cell == CELL_NONE || coord == COORD_NONE || !CanExistOnVictimCell() || !Owner->Unlimbo(coord, dir.As_Dir256())) {
			Owner->Strength = 0;
			Owner->Delete_Me();
		} else {
			if (Owner->ShouldBeReselectOnUnlimbo && Owner->House->Is_Player_Control()) {
				Owner->ShouldBeReselectOnUnlimbo = false;
				Owner->Select();
			}
			if (Owner->OldTeam != NULL) {
				Owner->OldTeam->AddMember(Owner, false);
			}
			if (!Owner->HaveMegaMission()) {
				Owner->Assign_Archive_Target(NULL);
				Owner->Assign_Target(NULL);
				Owner->Assign_Destination(NULL, true);
			}
			Owner->Enter_Idle_Mode(false, true);
			Owner->UpdateSight(false, false, false, NULL, 0);
			Owner->ParalysisTimer = Owner->Get_Class_Weapon_Data(0)->Weapon->ROF * 3;
			FUN_00629530();
		}
		Victim->AngleRotatedSideways = 0;
		Victim->ParalysisTimer = 0;
		Victim->ParasiteEatingMe = NULL;
		Victim = NULL;
	} else {
		Owner->Strength = 0;
		Victim->ParalysisTimer = 0;
		Victim->ParasiteEatingMe = NULL;
		Owner->Delete_Me();
		Victim = NULL;
	}
}

// FUNCTION: GAMEMD 0x0062A8E0
bool ParasiteClass::CanInfect(FootClass * victim) const
{
	if (victim == NULL || victim->IsInLimbo || !victim->IsActive || victim->Strength == 0 || victim->ParasiteEatingMe != NULL) {
		return false;
	}
	if (!victim->Techno_Type_Class()->Parasiteable || victim->BunkerLinkedItem != NULL) {
		return false;
	}
	if (Owner != NULL && Owner->Techno_Type_Class()->Naval && victim->Get_Cell_Ptr() != NULL && !victim->Get_Cell_Ptr()->Is_Tile_Water()) {
		return false;
	}
	return true;
}

// FUNCTION: GAMEMD 0x0062AB40
bool ParasiteClass::CanExistOnVictimCell(void) const
{
	if (Victim == NULL) {
		return false;
	}
	if (Victim->In_Air()) {
		return false;
	}
	if (!Owner->Techno_Type_Class()->Naval) {
		if (Owner != NULL && Owner->Fetch_RTTI() == RTTI_UNIT && Victim->Get_Cell_Ptr()->Cell_Terrain(false) != NULL) {
			return false;
		}
		LandType land = Victim->Get_Cell_Ptr()->Land_Type();
		if (land == LAND_WATER || land == LAND_BEACH || land == LAND_ROCK) {
			bool blocked = true;
			if (Victim->Get_Cell_Ptr()->IsUnderBridge) {
				blocked = false;
			}
			int overlay = Victim->Get_Cell_Ptr()->Overlay;
			if (overlay >= 0x4A && overlay <= 0x63) {
				blocked = false;
			}
			if ((overlay < 0xCD || overlay > 0xE6) && blocked) {
				return false;
			}
		}
	}
	return Victim->Get_Cell_Ptr()->Cell_Building() ? false : true;
}

// FUNCTION: GAMEMD 0x0062AC30
Coord ParasiteClass::FUN_0062AC30(void)
{
	if (Victim == NULL) {
		return COORD_NONE;
	}
	Coord coord;
	if (CanExistOnVictimCell()) {
		coord = Victim->Position;
		if (Owner != NULL && Owner->Fetch_RTTI() == RTTI_UNIT) {
			coord = Map[coord].Center_Coord();
			if (Victim->IsOnBridge) {
				coord.Z += BRIDGE_LEPTON_HEIGHT;
			}
		}
		if (Victim->IsOnBridge) {
			Owner->IsOnBridge = true;
		} else {
			Owner->IsOnBridge = false;
		}
		return coord;
	}
	FacingType face = Victim->PrimaryFacing.Current().As_Dir8();
	CellClass * victimcell = Victim->Get_Cell_Ptr();
	CellClass * cell = &victimcell->Adjacent_Cell(face);
	if (!Owner->Techno_Type_Class()->Naval) {
		LandType land = cell->Land_Type();
		if (land == LAND_WATER || land == LAND_BEACH || land == LAND_ROCK) {
			bool blocked = true;
			if (Victim->Get_Cell_Ptr()->IsUnderBridge) {
				blocked = false;
			}
			int overlay = Victim->Get_Cell_Ptr()->Overlay;
			if (overlay >= 0x4A && overlay <= 0x63) {
				blocked = false;
			}
			if ((overlay < 0xCD || overlay > 0xE6) && blocked) {
				return COORD_NONE;
			}
		}
	}
	coord = cell->Closest_Free_Spot(cell->Center_Coord(), false, false, false);
	if (Owner != NULL && Owner->Fetch_RTTI() == RTTI_UNIT) {
		coord = Map[coord].Center_Coord();
	}
	if (Victim->IsOnBridge) {
		if (Map[coord].IsUnderBridge) {
			coord.Z += BRIDGE_LEPTON_HEIGHT;
			Owner->IsOnBridge = true;
		} else {
			Owner->IsOnBridge = false;
		}
	} else {
		if (Map[coord].IsUnderBridge) {
			if (coord.Z < victimcell->Center_Coord().Z) {
				coord.Z += BRIDGE_LEPTON_HEIGHT;
				Owner->IsOnBridge = true;
			} else {
				Owner->IsOnBridge = false;
			}
		} else {
			Owner->IsOnBridge = false;
		}
	}
	if (coord != COORD_NONE) {
		return coord;
	}
	return COORD_NONE;
}

// FUNCTION: GAMEMD 0x0062AF50
int ParasiteClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x0062AF60
RTTIType ParasiteClass::Fetch_RTTI(void) const
{
	return(RTTI_PARASITE);
}

// SYNTHETIC: GAMEMD 0x0062AF70
// ??_GParasiteClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0062B070
// ?Clear@?$DynamicVectorClass@PAVParasiteClass@@@@UAEXXZ
template class DynamicVectorClass<ParasiteClass *>;

// TEMPLATE: GAMEMD 0x0062B0A0
// ?ID@?$DynamicVectorClass@PAVParasiteClass@@@@UAEHPBQAVParasiteClass@@@Z

// TEMPLATE: GAMEMD 0x0062B0C0
// ?GetItem@?$VectorClass@PAVParasiteClass@@@@UBEPAVParasiteClass@@H@Z

// TEMPLATE: GAMEMD 0x0062B0D0
// ??8?$VectorClass@PAVParasiteClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0062B110
// ?Resize@?$VectorClass@PAVParasiteClass@@@@UAE_NHPBQAVParasiteClass@@@Z

// TEMPLATE: GAMEMD 0x0062B1C0
// ?Clear@?$VectorClass@PAVParasiteClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0062B1F0
// ?ID@?$VectorClass@PAVParasiteClass@@@@UAEHABQAVParasiteClass@@@Z

// TEMPLATE: GAMEMD 0x0062B230
// ?ID@?$VectorClass@PAVParasiteClass@@@@UAEHPBQAVParasiteClass@@@Z

// TEMPLATE: GAMEMD 0x0062B250
// ?Resize@?$DynamicVectorClass@PAVParasiteClass@@@@UAE_NHPBQAVParasiteClass@@@Z

// TEMPLATE: GAMEMD 0x0062B300
// ?ID@?$DynamicVectorClass@PAVParasiteClass@@@@UAEHABQAVParasiteClass@@@Z

// SYNTHETIC: GAMEMD 0x0062B330
// ??_G?$VectorClass@PAVParasiteClass@@@@UAEPAXI@Z
template class VectorClass<ParasiteClass *>;

// SYNTHETIC: GAMEMD 0x0062B380
// ??_G?$DynamicVectorClass@PAVParasiteClass@@@@UAEPAXI@Z
