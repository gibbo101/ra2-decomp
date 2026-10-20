// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "terrtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_theater.h"
#include "ccfile.h"
#include "ccini.h"
#include "data.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "mixfile.h"
#include "rules.h"
#include "shapeset.h"
#include "sun.h"
#include "terrain.h"
#include "terrtype.h"
#include "tracker.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007F54F8;
extern const double DAT_007F5500;

// FUNCTION: 0x0071D390 $E2
// FUNCTION: 0x0071D3C0 $E5
// FUNCTION: 0x0071D3E0 $E8
// FUNCTION: 0x0071D400 $E11
// FUNCTION: 0x0071D420 $E14
// FUNCTION: 0x0071D440 $E17
// FUNCTION: 0x0071D460 $E20
// FUNCTION: 0x0071D4A0 $E23
// FUNCTION: 0x0071D4D0 $E26
// FUNCTION: 0x0071D500 $E29
// FUNCTION: 0x0071D530 $E32
// FUNCTION: 0x0071D540 $E35
// FUNCTION: 0x0071D560 $E38

// FUNCTION: 0x0071D580 $E115
static Cell const _OccupyLists[BSIZE_COUNT][10] = {
	{ Cell(0, 0), REFRESH_EOL },
	{ Cell(0, 0), Cell(1, 0), REFRESH_EOL },
	{ Cell(0, 0), Cell(0, 1), REFRESH_EOL },
	{ Cell(0, 0), Cell(1, 0), Cell(0, 1), Cell(1, 1), REFRESH_EOL },
	{ Cell(0, 0), Cell(1, 0), Cell(0, 1), Cell(1, 1), Cell(0, 2), Cell(1, 2), REFRESH_EOL },
	{ Cell(0, 0), Cell(1, 0), Cell(2, 0), Cell(0, 1), Cell(1, 1), Cell(2, 1), REFRESH_EOL },
	{ Cell(0, 0), Cell(1, 0), Cell(2, 0), Cell(0, 1), Cell(1, 1), Cell(2, 1), Cell(0, 2), Cell(1, 2), Cell(2, 2), REFRESH_EOL },
	{ Cell(0, 0), Cell(1, 0), Cell(2, 0), Cell(3, 0), Cell(0, 1), Cell(1, 1), Cell(2, 1), Cell(3, 1), REFRESH_EOL },
};

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

// FUNCTION: 0x0071DA80
TerrainTypeClass::TerrainTypeClass(char const * ininame) :
	BASECLASS(ininame),
	HeapID(TERRAIN_NONE),
	Foundation(BSIZE_FIRST),
	RadarColor(0, 0, 0),
	TemperateOccupationBits(7),
	SnowOccupationBits(7),
	AnimationRate(0),
	AnimationProbability(0),
	IsWaterBased(false),
	IsTiberiumSpawn(false),
	IsFlammable(false),
	IsAnimated(false),
	IsVeinhole(false),
	Occupy(NULL)
{
	Create_ID();
	IsSentient = true;
	IsStealthy = true;
	IsSelectable = false;
	IsLegalTarget = false;
	IsInsignificant = true;

	TerrainTypes.Add(this);

	HeapID = (TerrainType)TerrainTypes.ID(this);

	MaxStrength = -1;
	Armor = ARMOR_WOOD;
}

// FUNCTION: 0x0071DBD0
TerrainTypeClass::TerrainTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	RadarColor(x)
{
}

// FUNCTION: 0x0071DC00
TerrainTypeClass::~TerrainTypeClass(void)
{
	if (ImageData != NULL) {
		delete (ShapeSet *)ImageData;
		ImageData = NULL;
	}
	Detach_This_From_All(this, true);
	TerrainTypes.Delete(this);
}

// FUNCTION: 0x0071DCA0
void TerrainTypeClass::Init(TheaterType theater)
{
	for (TerrainType index = TERRAIN_FIRST; index < TerrainTypes.Count(); index++) {
		TerrainTypeClass * terrain = TerrainTypes[index];
		char fullname[_MAX_FNAME + _MAX_EXT];

		if (terrain->IsTheater) {
			_makepath(fullname, NULL, NULL, terrain->Name(), Theaters[theater].Suffix);
			if (terrain->ImageAllocated && terrain->ImageData != NULL) {
				delete (ShapeSet *)terrain->ImageData;
			}
			terrain->ImageData = NULL;
			terrain->ImageAllocated = false;
			CCFileClass file(fullname);
			terrain->ImageData = Load_Alloc_Data(file);
		}
	}
}

// FUNCTION: 0x0071DD80
TerrainType TerrainTypeClass::From_Name(char const * name)
{
	TerrainType index;

	if (name != NULL) {
		for (index = TERRAIN_FIRST; index < TerrainTypes.Count(); index++) {
			if (stricmp(name, TerrainTypes[index]->Name()) == 0) {
				return(index);
			}
		}
	}
	return(TERRAIN_NONE);
}

// FUNCTION: 0x0071DDD0
bool TerrainTypeClass::Create_And_Place(Cell const & cell, HouseClass *) const
{
	if (new TerrainClass(this, cell)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0071DE10
ObjectClass * TerrainTypeClass::Create_One_Of(HouseClass *) const
{
	return(new TerrainClass(this, CELL_NONE));
}

// FUNCTION: 0x0071DE40
Cell const * TerrainTypeClass::Occupy_List(bool) const
{
	if (Occupy != NULL) return(Occupy);

	static Cell const _simple[1] = {
		REFRESH_EOL
	};
	return(&_simple[0]);
}

// FUNCTION: 0x0071DE90 $E118
// FUNCTION: 0x0071DEA0
bool TerrainTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {

		if (MaxStrength == -1) {
			MaxStrength = Rule->TreeStrength;
		}

		IsVeinhole = ini.Get_Bool(Name(), "IsVeinhole", IsVeinhole);
		if (IsVeinhole) {
			IsSentient = false;
			IsLegalTarget = true;
		}

		IsWaterBased = ini.Get_Bool(Name(), "WaterBound", IsWaterBased);
		IsTiberiumSpawn = ini.Get_Bool(Name(), "SpawnsTiberium", IsTiberiumSpawn);
		IsFlammable = ini.Get_Bool(Name(), "IsFlammable", IsFlammable);

		Foundation = ArtINI.Get_BSizeType(Graphic_Name(), "Foundation", Foundation);
		Occupy = _OccupyLists[Foundation];

		if (!IsTheater) {
			char filename[512];
			_makepath(filename, 0, 0, Graphic_Name(), ".SHP");
			ImageData = FileSystem::LoadFile(filename, false);
		}

		ShapeSet const * image = (ShapeSet const *)Get_Image_Data();
		if (image) {
			RadarColor = image->Get_Color(0);
		}

		RadarColor = ini.Get_RGBClass(Name(), "RadarColor", RadarColor);
		IsAnimated = ini.Get_Bool(Name(), "IsAnimated", IsAnimated);
		AnimationRate = ini.Get_Int(Name(), "AnimationRate", AnimationRate);
		AnimationProbability = ini.Get_Float(Name(), "AnimationProbability", AnimationProbability);
		TemperateOccupationBits = ini.Get_Int(Name(), "TemperateOccupationBits", TemperateOccupationBits);
		SnowOccupationBits = ini.Get_Int(Name(), "SnowOccupationBits", SnowOccupationBits);

		return(true);
	}
	return(false);
}

// FUNCTION: 0x0071E0D0
Coord const TerrainTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord tmp = coord;
	if (tmp.Z < Map.Get_Height_GL(tmp)) {
		tmp.Z = Map.Get_Height_GL(tmp);
	}
	return(tmp);
}

// FUNCTION: 0x0071E240
HRESULT STDMETHODCALLTYPE TerrainTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0071E260
HRESULT STDMETHODCALLTYPE TerrainTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TerrainTypeClass;
	return(S_OK);
}

// FUNCTION: 0x0071E140
void TerrainTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(HeapID);
	crc(IsWaterBased);
	crc(IsTiberiumSpawn);
	crc(IsFlammable);
	crc(Foundation);
	crc(IsAnimated);
	crc(AnimationRate);
	crc(AnimationProbability);
}

// FUNCTION: 0x0071E1D0
HRESULT STDMETHODCALLTYPE TerrainTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TerrainTypeClass(NoInitClass());

		Fetch_Voxel_Image();
		Fetch_Normal_Image();
		Occupy = _OccupyLists[Foundation];

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0071E2A0
TerrainTypeClass * TerrainTypeClass::Find_Or_Make(const char * name)
{
	return(TFind_Or_Make<TerrainTypeClass>(name, TerrainTypes));
}

// FUNCTION: 0x0071E330
RTTIType TerrainTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_TERRAINTYPE);
}

// FUNCTION: 0x0071E340
int TerrainTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x0071E350
int TerrainTypeClass::Fetch_Heap_ID(void) const
{
	return(HeapID);
}

// FUNCTION: 0x0071E360 TerrainTypeClass::~TerrainTypeClass

// FUNCTION: 0x0071E410 $E120
double DAT_00B0F170 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0071E440 $E123
double DAT_00B0F1D8 = DAT_00B0F170 / DAT_007F54F8;

// FUNCTION: 0x0071E460 $E126
double DAT_00B0F1C0 = DAT_007E1718 / DAT_00B0F1D8;

// FUNCTION: 0x0071E480 $E129
double DAT_00B0F1B8 = DAT_007E1720 * DAT_007F5500;

// FUNCTION: 0x0071E4A0 $E132
double DAT_00B0F178 = DAT_007E1728 * DAT_007F5500;
