// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include <math.h>
#include "disklaser.h"
#include "crc.h"
#include "face.h"
#include "swizzle.h"
#include "techno.h"
#include "weapon.h"

// SYNTHETIC: GAMEMD 0x004A6A60 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004A6A90 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004A6AB0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004A6AD0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004A6AF0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004A6B10 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004A6B30 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004A6B70 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004A6BA0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004A6BD0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004A6C00 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004A6C10 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004A6C30 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004A6C50 SYMBOL
// _$E49

extern const CLSID CLSID_DiskLaserClass;
extern DynamicVectorClass<AbstractClass *> DAT_00B0F698;
extern const double DAT_007E5ED0;

// SYNTHETIC: GAMEMD 0x004A6C70 SYMBOL
// _$E78
// SYNTHETIC: GAMEMD 0x004A6CB0 SYMBOL
// _$E76
DynamicVectorClass<DiskLaserClass *> DiskLasers;

// SYNTHETIC: GAMEMD 0x004A6CF0 SYMBOL
// _$E81
Point2D DiskLaserClass::DrawCoords[16] = {
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 270.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 270.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 292.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 292.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 315.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 315.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 337.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 337.0) * 240.0)),
	Point2D((int)(fastmath::cos(0.0) * 240.0), (int)(fastmath::sin(0.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 22.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 22.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 45.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 45.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 67.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 67.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 90.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 90.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 112.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 112.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 135.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 135.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 157.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 157.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 180.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 180.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 202.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 202.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 235.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 235.0) * 240.0)),
	Point2D((int)(fastmath::cos(DAT_007E5ED0 * 257.0) * 240.0), (int)(fastmath::sin(DAT_007E5ED0 * 257.0) * 240.0))
};

static inline DirType Direction_Between(Coord const & coord1, Coord const & coord2)
{
	int x1 = coord1.X;
	int x2 = coord2.X;
	return DirType(fastmath::atan2((double)coord1.Y - (double)coord2.Y, (double)x2 - (double)x1));
}

// FUNCTION: GAMEMD 0x004A71A0
void DiskLaserClass::Fire(TechnoClass * owner, TechnoClass * target, WeaponTypeClass * weapon, int damage)
{
	if (owner == NULL) {
		Mark_For_Removal();
		return;
	}
	if (target == NULL) {
		Mark_For_Removal();
		return;
	}
	if (weapon == NULL) {
		Mark_For_Removal();
		return;
	}
	Owner = owner;
	Damage = damage;
	Target = target;
	Weapon = weapon;
	DirType dir = Direction_Between(owner->Center_Coord(), target->Center_Coord());
	unknown_34 = ((int)(((((unsigned int)dir.Raw >> 11) + 1) >> 1) & 0xf) + 8) % 16;
	unknown_30 = 0;
	unknown_38 = 0;
}

#include "_map.h"
#include "_rules.h"
#include "builtype.h"
#include "building.h"
#include "house.h"
#include "laser.h"
#include "map.h"
#include "rules.h"
#include "voc.h"

static inline int Coord_Dist2(int x, int y, int z, Coord const & b)
{
	Coord c = Coord(x, y, z) - b;
	return c.Length();
}

static inline int Coord_Dist(Coord const & a, Coord const & b)
{
	Coord c = a - b;
	return c.Length();
}

// FUNCTION: GAMEMD 0x004A7340
void DiskLaserClass::AI(void)
{
	if (unknown_30 < 0) {
		Mark_For_Removal();
		return;
	}
	if (unknown_30 > 0) {
		unknown_30--;
		return;
	}

	int distance;
	{
		Coord oc = Owner->Center_Coord();
		if (Owner->In_Air()) {
			oc.Z = Target->Center_Coord().Z;
		}
		distance = Coord_Dist(oc, Target->Center_Coord());
	}
	if (Target->Fetch_RTTI() == RTTI_BUILDING) {
		BuildingClass * building = (BuildingClass *)Target;
		if (building != NULL) {
			distance += -(building->Class->Height(false) + building->Class->Width()) * 64;
			if (distance < 0) {
				distance = 0;
			}
		}
	}
	if (distance > Weapon->Range) {
		Mark_For_Removal();
		return;
	}
	if (Owner->IsCrashing) {
		Mark_For_Removal();
		return;
	}

	Coord fire = Owner->Fire_Coord(0, Coord(0, 0, 0));
	int ia = (unknown_34 + unknown_38) % 16;
	int ib = (unknown_34 - unknown_38 + 16) % 16;
	int ic = (unknown_34 + unknown_38 + 1) % 16;
	int id = (unknown_34 - unknown_38 + 15) % 16;

	RGBClass color;
	RGBClass const * inner;
	RGBClass const * outer;
	if (Weapon->IsHouseColor) {
		inner = &Owner->House->LaserColor;
		color.Set_Red(inner->Get_Red() >> 1);
		color.Set_Green(inner->Get_Green() >> 1);
		color.Set_Blue(inner->Get_Blue() >> 1);
		outer = &color;
	} else {
		inner = &Weapon->LaserInnerColor;
		outer = &Weapon->LaserOuterColor;
	}
	RGBClass const * spread = &Weapon->LaserOuterSpread;

	if (ia == ib && unknown_38 != 0) {
		Coord start = fire + DrawCoords[ia];
		Coord end;
		if (Target != NULL && (Target->AbstractFlags & 2)) {
			end = Target->Target_Coord();
		} else {
			end = Target->As_Coord();
		}
		new LaserDrawClass(start, end, 0, true, *inner, *outer, *spread, Weapon->LaserDuration, false, true, 1.0f, 0.0f);
		MapClass::DamageArea(end, Damage, Owner, Weapon->WarheadPtr, true, NULL);
		if (Weapon->Sound.Count() > 0) {
			VocClass::PlayAt(Weapon->Sound.Pick(Owner->SoundRandomSeed), start);
		}
		unknown_30 = -1;
		return;
	}

	{
		int duration = 8 - unknown_38;
		Point2D const & pa = DrawCoords[ia];
		Point2D const & pb = DrawCoords[ib];
		Point2D const & pc = DrawCoords[ic];
		Point2D const & pd = DrawCoords[id];
		Coord a = fire + pa;
		Coord c = fire + pc;
		if (unknown_38 == 0 && Rule->DiskLaserChargeUp != -1) {
			VocClass::PlayAt(Rule->DiskLaserChargeUp, a);
		}
		new LaserDrawClass(a, c, 0, true, *inner, *outer, *spread, duration, false, true, 1.0f, 0.5f);
		a = fire + pb;
		c = fire + pd;
		new LaserDrawClass(a, c, 0, true, *inner, *outer, *spread, duration, false, true, 1.0f, 0.5f);
	}
	unknown_30 = 1;
	unknown_38++;
}


// FUNCTION: GAMEMD 0x004A7900
void DiskLaserClass::PointerGotInvalid(AbstractClass * invalid)
{
	if (invalid == Owner) {
		unknown_30 = -1;
		DAT_00B0F698.Add(this);
	}
	if (invalid == Target) {
		unknown_30 = -1;
		DAT_00B0F698.Add(this);
	}
}

// FUNCTION: GAMEMD 0x004A79D0
void DiskLaserClass::Mark_For_Removal(void)
{
	unknown_30 = -1;
	DAT_00B0F698.Add(this);
}

// FUNCTION: GAMEMD 0x004A7A30
DiskLaserClass::DiskLaserClass(void) :
	BASECLASS()
{
	Owner = NULL;
	Target = NULL;
	Weapon = NULL;
	unknown_30 = 0;
	unknown_34 = 0;
	unknown_38 = 0;
	Damage = 0;
	DiskLasers.Add(this);
}

// FUNCTION: GAMEMD 0x004A7AD0
DiskLaserClass::DiskLaserClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x004A7B00
DiskLaserClass::~DiskLaserClass(void)
{
	DiskLasers.Delete(this);
}

// FUNCTION: GAMEMD 0x004A7B80
void DiskLaserClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
}

// FUNCTION: GAMEMD 0x004A7B90
HRESULT STDMETHODCALLTYPE DiskLaserClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) DiskLaserClass(NoInitClass());
		Swizzle_Pointer(&Owner);
		Swizzle_Pointer(&Target);
		Swizzle_Pointer(&Weapon);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004A7C10
HRESULT STDMETHODCALLTYPE DiskLaserClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x004A7C30
HRESULT STDMETHODCALLTYPE DiskLaserClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_DiskLaserClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004A7C70
int DiskLaserClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x004A7C80
RTTIType DiskLaserClass::Fetch_RTTI(void) const
{
	return(RTTI_DISKLASER);
}

// SYNTHETIC: GAMEMD 0x004A7C90
// ??_GDiskLaserClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004A7D20
// ?Clear@?$DynamicVectorClass@PAVDiskLaserClass@@@@UAEXXZ
template class DynamicVectorClass<DiskLaserClass *>;

// TEMPLATE: GAMEMD 0x004A7D50
// ?ID@?$DynamicVectorClass@PAVDiskLaserClass@@@@UAEHPBQAVDiskLaserClass@@@Z

// TEMPLATE: GAMEMD 0x004A7D70
// ?GetItem@?$VectorClass@PAVDiskLaserClass@@@@UBEPAVDiskLaserClass@@H@Z

// TEMPLATE: GAMEMD 0x004A7D80
// ??8?$VectorClass@PAVDiskLaserClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x004A7DC0
// ?Resize@?$VectorClass@PAVDiskLaserClass@@@@UAE_NHPBQAVDiskLaserClass@@@Z

// TEMPLATE: GAMEMD 0x004A7E70
// ?Clear@?$VectorClass@PAVDiskLaserClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x004A7EA0
// ?ID@?$VectorClass@PAVDiskLaserClass@@@@UAEHABQAVDiskLaserClass@@@Z
template class VectorClass<DiskLaserClass *>;

// TEMPLATE: GAMEMD 0x004A7EE0
// ?ID@?$VectorClass@PAVDiskLaserClass@@@@UAEHPBQAVDiskLaserClass@@@Z

// TEMPLATE: GAMEMD 0x004A7F00
// ?Resize@?$DynamicVectorClass@PAVDiskLaserClass@@@@UAE_NHPBQAVDiskLaserClass@@@Z

// TEMPLATE: GAMEMD 0x004A7FB0
// ?ID@?$DynamicVectorClass@PAVDiskLaserClass@@@@UAEHABQAVDiskLaserClass@@@Z

// TEMPLATE: GAMEMD 0x004A7FE0
// ?Add@?$DynamicVectorClass@PAVDiskLaserClass@@@@QAE_NABQAVDiskLaserClass@@@Z

// TEMPLATE: GAMEMD 0x004A8030
// ??_G?$VectorClass@PAVDiskLaserClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004A8080
// ??_G?$DynamicVectorClass@PAVDiskLaserClass@@@@UAEPAXI@Z
