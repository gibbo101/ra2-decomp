// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "bullettype.h"
#include "_map.h"
#include "_rules.h"
#include "animtype.h"
#include "findmake.h"
#include "geometry.h"
#include "globals.h"
#include "incdec.h"
#include "rules.h"
#include "scheme.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"
#include "weapon.h"

#include <math.h>

extern char DAT_00889F64[];


// SYNTHETIC: GAMEMD 0x0046B9B0 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x0046B9D0 SYMBOL
// _$E57
// SYNTHETIC: GAMEMD 0x0046BA00 SYMBOL
// _$E60
// SYNTHETIC: GAMEMD 0x0046BA20 SYMBOL
// _$E63
// SYNTHETIC: GAMEMD 0x0046BA40 SYMBOL
// _$E66
// SYNTHETIC: GAMEMD 0x0046BA60 SYMBOL
// _$E69
// SYNTHETIC: GAMEMD 0x0046BA80 SYMBOL
// _$E72
// SYNTHETIC: GAMEMD 0x0046BAA0 SYMBOL
// _$E75
// SYNTHETIC: GAMEMD 0x0046BAE0 SYMBOL
// _$E78
// SYNTHETIC: GAMEMD 0x0046BB10 SYMBOL
// _$E81
// SYNTHETIC: GAMEMD 0x0046BB40 SYMBOL
// _$E84
// SYNTHETIC: GAMEMD 0x0046BB70 SYMBOL
// _$E87
// SYNTHETIC: GAMEMD 0x0046BB80 SYMBOL
// _$E90
// SYNTHETIC: GAMEMD 0x0046BBA0 SYMBOL
// _$E93

// FUNCTION: GAMEMD 0x0046BBC0
BulletTypeClass::BulletTypeClass(char const * name) :
	BASECLASS(name),
	IsAirburst(false),
	IsFloater(false),
	SubjectToCliffs(false),
	SubjectToElevation(false),
	SubjectToWalls(false),
	IsVeryHigh(false),
	IsShadow(true),
	IsArcing(false),
	IsDropping(false),
	Level(false),
	IsInvisible(false),
	IsProximityArmed(false),
	IsFueled(false),
	NoRotate(true),
	IsInaccurate(false),
	FlakScatter(false),
	IsAntiAircraft(false),
	IsAntiGround(true),
	IsDegenerate(false),
	IsBouncy(false),
	IsAnimPalette(false),
	FirersPalette(false),
	Cluster(1),
	AirburstWeapon(NULL),
	ShrapnelWeapon(NULL),
	ShrapnelCount(0),
	DetonationAltitude(0),
	Vertical(false),
	Elasticity(.75),
	Acceleration(3),
	Color(0),
	Trailer(NULL),
	ROT(0),
	CourseLockDuration(0),
	SpawnDelay(3),
	ScaledSpawnDelay(0),
	Scalable(false),
	Arming(0),
	AnimLow(0),
	AnimHigh(0),
	AnimRate(0),
	Flat(false)
{
	Create_ID();

	IsSentient = true;
	IsStealthy = true;
	IsSelectable = false;
	IsLegalTarget = false;
	IsInsignificant = true;
	IsImmune = true;
	IsFootprint = false;

	BulletTypes.Add(this);

	AbstractTypePtrTracker.Add(this);
}

// FUNCTION: GAMEMD 0x0046BDE0
BulletTypeClass::BulletTypeClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x0046BE10
BulletTypeClass::~BulletTypeClass(void)
{
	Detach_This_From_All(this, true);
	AbstractTypePtrTracker.Delete(this);
	BulletTypes.Delete(this);
}

// FUNCTION: GAMEMD 0x0046BEE0
bool BulletTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {

		Arming = ini.Get_Int(Name(), "Arm", Arming);
		ROT = ini.Get_Int(Name(), "ROT", ROT);
		CourseLockDuration = ini.Get_Int(Name(), "CourseLockDuration", CourseLockDuration);
		Elasticity = ini.Get_Float(Name(), "Elasticity", Elasticity);
		Acceleration = ini.Get_Int(Name(), "Acceleration", Acceleration);
		Color = ini.Get_Scheme_Index(Name(), "Color", Color);
		IsArcing = ini.Get_Bool(Name(), "Arcing", IsArcing);
		IsFloater = ini.Get_Bool(Name(), "Floater", IsFloater);
		SubjectToCliffs = ini.Get_Bool(Name(), "SubjectToCliffs", SubjectToCliffs);
		SubjectToElevation = ini.Get_Bool(Name(), "SubjectToElevation", SubjectToElevation);
		SubjectToWalls = ini.Get_Bool(Name(), "SubjectToWalls", SubjectToWalls);
		IsVeryHigh = ini.Get_Bool(Name(), "VeryHigh", IsVeryHigh);
		IsShadow = ini.Get_Bool(Name(), "Shadow", IsShadow);
		IsDropping = ini.Get_Bool(Name(), "Dropping", IsDropping);
		Level = ini.Get_Bool(Name(), "Level", Level);
		IsInvisible = ini.Get_Bool(Name(), "Inviso", IsInvisible);
		IsProximityArmed = ini.Get_Bool(Name(), "Proximity", IsProximityArmed);
		IsFueled = ini.Get_Bool(Name(), "Ranged", IsFueled);
		IsInaccurate = ini.Get_Bool(Name(), "Inaccurate", IsInaccurate);
		FlakScatter = ini.Get_Bool(Name(), "FlakScatter", FlakScatter);
		IsAntiAircraft = ini.Get_Bool(Name(), "AA", IsAntiAircraft);
		IsAntiGround = ini.Get_Bool(Name(), "AG", IsAntiGround);
		IsDegenerate = ini.Get_Bool(Name(), "Degenerates", IsDegenerate);
		IsBouncy = ini.Get_Bool(Name(), "Bouncy", IsBouncy);
		IsAirburst = ini.Get_Bool(Name(), "Airburst", IsAirburst);
		Cluster = ini.Get_Int(Name(), "Cluster", Cluster);
		Scalable = ini.Get_Bool(Name(), "Scalable", Scalable);

		if (ini.Get_String(Name(), "Image", DAT_00889F64, GraphicName, GraphicName.Size()) > 0) {
			Trailer = TGet_Class(ArtINI, Graphic_Name(), "Trailer", Trailer);
			SpawnDelay = ArtINI.Get_Int(Graphic_Name(), "SpawnDelay", SpawnDelay);
			NoRotate = !ArtINI.Get_Bool(Graphic_Name(), "Rotates", !NoRotate);
			Flat = ArtINI.Get_Bool(Graphic_Name(), "Flat", Flat);
		}

		AirburstWeapon = TGet_Class(ini, Name(), "AirburstWeapon", AirburstWeapon);
		ShrapnelWeapon = TGet_Class(ini, Name(), "ShrapnelWeapon", ShrapnelWeapon);
		ShrapnelCount = ini.Get_Int(Name(), "ShrapnelCount", ShrapnelCount);
		DetonationAltitude = ini.Get_Int(Name(), "DetonationAltitude", DetonationAltitude);
		Vertical = ini.Get_Bool(Name(), "Vertical", Vertical);
		FirersPalette = ini.Get_Bool(Name(), "FirersPalette", FirersPalette);

		AnimLow = ArtINI.Get_Int(Graphic_Name(), "AnimLow", AnimLow);
		AnimHigh = ArtINI.Get_Int(Graphic_Name(), "AnimHigh", AnimHigh);
		AnimRate = ArtINI.Get_Int(Graphic_Name(), "AnimRate", AnimRate);

		IsAnimPalette = ArtINI.Get_Bool(Graphic_Name(), "AnimPalette", IsAnimPalette);

		if (!IsInvisible) {
			Fetch_Normal_Image();
		}
		if (IsVoxel) {
			Fetch_Voxel_Image();
		}

		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0046C440
BulletType BulletTypeClass::From_Name(char const * name)
{
	if (stricmp(name, "<none>")) {
		for (int index = 0; index < BulletTypes.Count(); index++) {
			if (stricmp(name, BulletTypes[index]->Name()) == 0) {
				return(BulletType(index));
			}
		}
		BulletTypeClass *ptr = new BulletTypeClass(name);
		return(BulletType(BulletTypes.ID(ptr)));
	}
	return(BULLET_NONE);
}

// FUNCTION: GAMEMD 0x0046C4D0
const char * BulletTypeClass::Name_From(BulletType bullet)
{
	if ((unsigned)bullet < (unsigned)BulletTypes.Count()) {
		return(BulletTypes[bullet]->Name());
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0046C4F0
Coord const BulletTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord c = coord;
	int height = Map.Get_Height_GL(c);
	int z = c.Z;
	if (z <= height) {
		z = Map.Get_Height_GL(c) + 1;
	}
	c.Z = z;
	return(c);
}

// FUNCTION: GAMEMD 0x0046C560
void BulletTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(IsAirburst);
	crc(IsShadow);
	crc(IsArcing);
	crc(IsDropping);
	crc(Level);
	crc(IsInvisible);
	crc(IsProximityArmed);
	crc(IsFueled);
	crc(NoRotate);
	crc(IsInaccurate);
	crc(FlakScatter);
	crc(IsAntiAircraft);
	crc(IsAntiGround);
	crc(IsDegenerate);
	crc(IsBouncy);
	crc(Elasticity);
	crc(Acceleration);
	crc(ROT);
	crc(Arming);
	crc(Flat);
}

// FUNCTION: GAMEMD 0x0046C6A0
HRESULT STDMETHODCALLTYPE BulletTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) BulletTypeClass(NoInitClass());

		Fetch_Voxel_Image();
		Fetch_Normal_Image();
		Swizzle_Pointer(&Trailer);
		Swizzle_Pointer(&AirburstWeapon);
		Swizzle_Pointer(&ShrapnelWeapon);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0046C730
HRESULT STDMETHODCALLTYPE BulletTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0046C750
HRESULT STDMETHODCALLTYPE BulletTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BulletTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0046C790
BulletTypeClass * BulletTypeClass::FindOrAllocate(const char * name)
{
	if (stricmp("<none>", name) && stricmp("none", name)) {
		for (int index = 0; index < BulletTypes.Count(); index++) {
			if (stricmp(BulletTypes[index]->Name(), name) == 0) {
				return(BulletTypes[index]);
			}
		}
		return(new BulletTypeClass(name));
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0046C820
void BulletTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (target == Trailer) {
		Trailer = NULL;
	}
}

// FUNCTION: GAMEMD 0x0046C840
void BulletTypeClass::SetScaledSpawnDelay(int delay)
{
	ScaledSpawnDelay = delay;
}

// FUNCTION: GAMEMD 0x0046C850
RTTIType BulletTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_BULLETTYPE);
}

// FUNCTION: GAMEMD 0x0046C860
int BulletTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x0046C870
// BulletTypeClass::Create_And_Place

// FUNCTION: GAMEMD 0x0046C880
// BulletTypeClass::Create_One_Of

// SYNTHETIC: GAMEMD 0x0046C890
// ??_GBulletTypeClass@@UAEPAXI@Z
