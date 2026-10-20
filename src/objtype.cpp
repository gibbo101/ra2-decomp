// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "objtype.h"
#include "sun.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_theater.h"
#include "aircraft.h"
#include "airctype.h"
#include "building.h"
#include "builtype.h"
#include "ccfile.h"
#include "coord.h"
#include "globals.h"
#include "house.h"
#include "mixfile.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "unittype.h"
#include "vector.h"
#include "voc.h"
#include "rawfile.h"
#include "data.h"

extern char DAT_00889F64[];
extern int DAT_00AC1474;

extern bool DAT_00AC1488;
extern bool DAT_00AC1489;
extern bool DAT_00AC148A;
extern bool DAT_00AC148B;

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
	static void const * PIPBRD_SHP;
	static void const * PIPS_SHP;
	static void const * PIPS2_SHP;
	static void const * TALKBUBL_SHP;
};

// FUNCTION: 0x005F6FF0 $E55
// FUNCTION: 0x005F7030 $E52
DynamicVectorClass<ObjectTypeClass *> ObjectTypeClass::ObjectTypes;

static inline bool _Add_Type(DynamicVectorClass<ObjectTypeClass *> & v, ObjectTypeClass * p) { return v.Add(p); }

// FUNCTION: 0x005F7090
ObjectTypeClass::ObjectTypeClass(char const * ininame) :
	BASECLASS(ininame),
	RadialColor(0, 0, 0),
	Armor(ARMOR_NONE),
	MaxStrength(0),
	ImageData(NULL),
	ImageAllocated(false),
	AlphaImageData(NULL),
	NoSpawnAlt(false),
	MaxSize(0),
	CrushSound(VOC_NONE),
	AmbientSound(-1),
	GraphicName(),
	AlternateArcticArt(false),
	ArcticArtInUse(false),
	AlphaGraphicName(),
	IsTheater(false),
	IsCrushable(false),
	IsBombable(true),
	IsStealthy(false),
	IsSelectable(true),
	IsLegalTarget(true),
	IsInsignificant(false),
	IsImmune(false),
	IsSentient(false),
	IsFootprint(true),
	IsVoxel(false),
	IsNewTheater(false),
	IsHasRadialIndicator(false),
	IsIgnoresFirestorm(false),
	UseLineTrail(false),
	LineTrailColor(128, 128, 128),
	LineTrailColorDecrement(16)
{
	GraphicName = IniName;
	AlphaGraphicName = DAT_00889F64;
	_Add_Type(ObjectTypes, this);
}

// FUNCTION: 0x005F7320
ObjectTypeClass::ObjectTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	RadialColor(x),
	GraphicName(x),
	AlphaGraphicName(x),
	Voxel(x),
	AuxVoxel(x),
	AuxVoxel2(x),
	LineTrailColor(x),
	ImageData(NULL)
{
}

// FUNCTION: 0x005F7400
ObjectTypeClass::~ObjectTypeClass(void)
{
	ObjectTypes.Delete(this);
	if (ImageAllocated && ImageData != NULL) {
		delete [] (char *)ImageData;
		ImageData = NULL;
		ImageAllocated = false;
	}
	Free_Voxels();
}

// FUNCTION: 0x005F75B0
int ObjectTypeClass::Max_Pips(void) const
{
	return(0);
}

// FUNCTION: 0x005F75C0
Point3D ObjectTypeClass::Pixel_Dimensions(void) const
{
	return(Point3D(10, 10, 10));
}

// FUNCTION: 0x005F75E0
Point3D ObjectTypeClass::Lepton_Dimensions(void) const
{
	return(Point3D(CELL_LEPTON_W, CELL_LEPTON_H, 2 * DAT_00AC1474));
}

// FUNCTION: 0x005F7610
int ObjectTypeClass::Cost_Of(HouseClass * house) const
{
	return(0);
}

// FUNCTION: 0x005F7620
int ObjectTypeClass::Time_To_Build(void) const
{
	return(0);
}

// FUNCTION: 0x005F7630
void const * ObjectTypeClass::Get_Cameo_Data(void) const
{
	return(NULL);
}

// FUNCTION: 0x005F7640
Cell const * ObjectTypeClass::Occupy_List(bool) const
{
	static Cell const _list[] = {Cell(0, 0), REFRESH_EOL};
	return(_list);
}

// FUNCTION: 0x005F76A0 $E58

// FUNCTION: 0x005F76B0
void FUN_005F76B0(void)
{
	if (FileSystem::PIPBRD_SHP == NULL) {
		RawFileClass file("PIPBRD.SHP");
		FileSystem::PIPBRD_SHP = Load_Alloc_Data(file);
		DAT_00AC1488 = FileSystem::PIPBRD_SHP != NULL;
		if (FileSystem::PIPBRD_SHP == NULL) {
			FileSystem::PIPBRD_SHP = FileSystem::LoadFile("PIPBRD.SHP", false);
		}
	}
	if (FileSystem::PIPS_SHP == NULL) {
		RawFileClass file("PIPS.SHP");
		FileSystem::PIPS_SHP = Load_Alloc_Data(file);
		DAT_00AC1489 = FileSystem::PIPS_SHP != NULL;
		if (FileSystem::PIPS_SHP == NULL) {
			FileSystem::PIPS_SHP = FileSystem::LoadFile("PIPS.SHP", false);
		}
	}
	if (FileSystem::PIPS2_SHP == NULL) {
		RawFileClass file("PIPS2.SHP");
		FileSystem::PIPS2_SHP = Load_Alloc_Data(file);
		DAT_00AC148A = FileSystem::PIPS2_SHP != NULL;
		if (FileSystem::PIPS2_SHP == NULL) {
			FileSystem::PIPS2_SHP = FileSystem::LoadFile("PIPS2.SHP", false);
		}
	}
	if (FileSystem::TALKBUBL_SHP == NULL) {
		RawFileClass file("TALKBUBL.SHP");
		FileSystem::TALKBUBL_SHP = Load_Alloc_Data(file);
		DAT_00AC148B = FileSystem::TALKBUBL_SHP != NULL;
		if (FileSystem::TALKBUBL_SHP == NULL) {
			FileSystem::TALKBUBL_SHP = FileSystem::LoadFile("TALKBUBL.SHP", false);
		}
	}
}

// FUNCTION: 0x005F9950
HRESULT STDMETHODCALLTYPE ObjectTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x005F9970
HRESULT STDMETHODCALLTYPE ObjectTypeClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	HRESULT res = BASECLASS::GetSizeMax(pcbSize);
	if (SUCCEEDED(res)) {
		res = S_OK;
	}
	return(res);
}
