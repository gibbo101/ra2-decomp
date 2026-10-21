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
#include "techtype.h"
#include "msanim.h"

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

// SYNTHETIC: GAMEMD 0x005F6FF0 SYMBOL
// _$E55
// SYNTHETIC: GAMEMD 0x005F7030 SYMBOL
// _$E52
DynamicVectorClass<ObjectTypeClass *> ObjectTypeClass::ObjectTypes;

static inline bool _Add_Type(DynamicVectorClass<ObjectTypeClass *> & v, ObjectTypeClass * p) { return v.Add(p); }

// FUNCTION: GAMEMD 0x005F7090
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

// FUNCTION: GAMEMD 0x005F7320
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

// FUNCTION: GAMEMD 0x005F7400
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

// FUNCTION: GAMEMD 0x005F75B0
int ObjectTypeClass::Max_Pips(void) const
{
	return(0);
}

// FUNCTION: GAMEMD 0x005F75C0
Point3D ObjectTypeClass::Pixel_Dimensions(void) const
{
	return(Point3D(10, 10, 10));
}

// FUNCTION: GAMEMD 0x005F75E0
Point3D ObjectTypeClass::Lepton_Dimensions(void) const
{
	return(Point3D(CELL_LEPTON_W, CELL_LEPTON_H, 2 * DAT_00AC1474));
}

// FUNCTION: GAMEMD 0x005F7610
int ObjectTypeClass::Cost_Of(HouseClass * house) const
{
	return(0);
}

// FUNCTION: GAMEMD 0x005F7620
int ObjectTypeClass::Time_To_Build(void) const
{
	return(0);
}

// FUNCTION: GAMEMD 0x005F7630
void const * ObjectTypeClass::Get_Cameo_Data(void) const
{
	return(NULL);
}

// FUNCTION: GAMEMD 0x005F7640
Cell const * ObjectTypeClass::Occupy_List(bool) const
{
	static Cell const _list[] = {Cell(0, 0), REFRESH_EOL};
	return(_list);
}

// SYNTHETIC: GAMEMD 0x005F76A0 SYMBOL
// _$E58

// FUNCTION: GAMEMD 0x005F76B0
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

// FUNCTION: GAMEMD 0x005F77F0
void FUN_005F77F0(void)
{
	if (DAT_00AC1488) {
		delete [] (SHPReference *)FileSystem::PIPBRD_SHP;
		DAT_00AC1488 = false;
	}
	if (DAT_00AC1489) {
		delete [] (SHPReference *)FileSystem::PIPS_SHP;
		DAT_00AC1489 = false;
	}
	if (DAT_00AC148A) {
		delete [] (SHPReference *)FileSystem::PIPS2_SHP;
		DAT_00AC148A = false;
	}
	if (DAT_00AC148B) {
		delete [] (SHPReference *)FileSystem::TALKBUBL_SHP;
		DAT_00AC148B = false;
	}
}

// FUNCTION: GAMEMD 0x005F7900
BuildingClass * ObjectTypeClass::Who_Can_Build_Me(bool intheory, bool needsnopower, bool legal, HouseClass * house) const
{
	BuildingClass * freebuilding = NULL;
	int ownable = Get_Ownable();

	int count = house->Buildings.Count();
	for (int index = 0; index < count; index++) {
		BuildingClass * building = house->Buildings[index];

		if (!building->IsInLimbo &&
			building->Class->ToBuild == RTTI &&
			(!needsnopower || building->IsOn) &&
			building->Mission != MISSION_DECONSTRUCTION && building->MissionQueue != MISSION_DECONSTRUCTION &&
			(!legal || building->House->Can_Build(this, true, true) > 0) &&
			(building->Class->Get_Ownable() & ownable)) {

			if (!intheory && building->HasAnyLink() && RTTI == RTTI_AIRCRAFTTYPE) {
				if (RTTI == RTTI_AIRCRAFTTYPE && building->HasFreeLink()) {
					if (building->IsLeader) return(building);
					freebuilding = building;
				}
			} else {
				if (RTTI == RTTI_UNITTYPE && ((TechnoTypeClass const *)this)->Naval) {
					if (building->Class->Naval) {
						if (building->IsLeader) return(building);
						goto leader;
					}
				} else {
					if (!building->Class->Naval) {
						leader:
						if (building->IsLeader) return(building);
						freebuilding = building;
					}
				}
			}
		}
	}
	if (freebuilding != NULL) {
		return(freebuilding);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x005F7A90
bool ObjectTypeClass::Fetch_Charger_Turret(char const * graphic, int index)
{
	char buffer[256];
	char name[260];

	if (index == 0) {
		sprintf(buffer, "%sTUR", graphic);
	} else {
		sprintf(buffer, "%sTUR%d", graphic, index);
	}
	_makepath(name, NULL, NULL, buffer, ".VXL");
	CCFileClass vxl(name);
	if (!vxl.Is_Available()) {
		return(false);
	}

	if (ChargerTurrets[index].VoxLib != NULL) {
		delete ChargerTurrets[index].VoxLib;
		ChargerTurrets[index].VoxLib = NULL;
	}
	bool voxok = false;
	ChargerTurrets[index].VoxLib = new VoxelLibrary(vxl);
	if (ChargerTurrets[index].VoxLib != NULL && !ChargerTurrets[index].VoxLib->Load_Failed()) {
		voxok = true;
	}

	if (index == 0) {
		sprintf(buffer, "%sTUR", graphic);
	} else {
		sprintf(buffer, "%sTUR%d", graphic, index);
	}
	_makepath(name, NULL, NULL, buffer, ".HVA");
	CCFileClass hva(name);

	if (ChargerTurrets[index].MotLib != NULL) {
		delete ChargerTurrets[index].MotLib;
		ChargerTurrets[index].MotLib = NULL;
	}
	bool motok = false;
	ChargerTurrets[index].MotLib = new MotionLibrary(hva);
	if (ChargerTurrets[index].MotLib != NULL && !ChargerTurrets[index].MotLib->Load_Failed()) {
		motok = true;
	}

	if (voxok && motok) {
		ChargerTurrets[index].MotLib->Scale(ChargerTurrets[index].VoxLib->Get_Layer_Info(0, 0).Scale);
		return(true);
	}
	if (ChargerTurrets[index].VoxLib != NULL) {
		delete ChargerTurrets[index].VoxLib;
		ChargerTurrets[index].VoxLib = NULL;
	}
	if (ChargerTurrets[index].MotLib != NULL) {
		delete ChargerTurrets[index].MotLib;
		ChargerTurrets[index].MotLib = NULL;
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F7DB0
bool ObjectTypeClass::Fetch_Charger_Barrel(char const * graphic, int index)
{
	char buffer[256];
	char name[260];

	if (index == 0) {
		sprintf(buffer, "%sBARL", graphic);
	} else {
		sprintf(buffer, "%sBARL%d", graphic, index);
	}
	_makepath(name, NULL, NULL, buffer, ".VXL");
	CCFileClass vxl(name);
	if (!vxl.Is_Available()) {
		return(true);
	}

	bool voxok = false;
	ChargerBarrels[index].VoxLib = new VoxelLibrary(vxl);
	if (ChargerBarrels[index].VoxLib != NULL && !ChargerBarrels[index].VoxLib->Load_Failed()) {
		voxok = true;
	}

	if (index == 0) {
		sprintf(buffer, "%sBARL", graphic);
	} else {
		sprintf(buffer, "%sBARL%d", graphic, index);
	}
	_makepath(name, NULL, NULL, buffer, ".HVA");
	CCFileClass hva(name);

	bool motok = false;
	ChargerBarrels[index].MotLib = new MotionLibrary(hva);
	if (ChargerBarrels[index].MotLib != NULL && !ChargerBarrels[index].MotLib->Load_Failed()) {
		motok = true;
	}

	if (voxok && motok) {
		ChargerBarrels[index].MotLib->Scale(ChargerBarrels[index].VoxLib->Get_Layer_Info(0, 0).Scale);
		return(true);
	}
	delete ChargerBarrels[index].VoxLib;
	delete ChargerBarrels[index].MotLib;
	return(false);
}

// FUNCTION: GAMEMD 0x005F8080
void ObjectTypeClass::Free_Voxels(void)
{
	for (int index = 0; index < 18; index++) {
		if (ChargerTurrets[index].VoxLib != NULL) {
			delete ChargerTurrets[index].VoxLib;
			ChargerTurrets[index].VoxLib = NULL;
		}
		if (ChargerTurrets[index].MotLib != NULL) {
			delete ChargerTurrets[index].MotLib;
			ChargerTurrets[index].MotLib = NULL;
		}
		if (ChargerBarrels[index].VoxLib != NULL) {
			delete ChargerBarrels[index].VoxLib;
			ChargerBarrels[index].VoxLib = NULL;
		}
		if (ChargerBarrels[index].MotLib != NULL) {
			delete ChargerBarrels[index].MotLib;
			ChargerBarrels[index].MotLib = NULL;
		}
	}
}

// FUNCTION: GAMEMD 0x005F8CE0
void ObjectTypeClass::Fetch_Aux_Voxel_Image(void)
{
	char buffer[256];
	char name[260];

	if (RTTI == RTTI_UNITTYPE && Get_Image_Data() != NULL && ((TechnoTypeClass *)this)->IsTurretEquipped) {
		sprintf(buffer, "%sTUR", (char const *)GraphicName);
		_makepath(name, NULL, NULL, buffer, ".VXL");
		CCFileClass tvxl(name);

		if (tvxl.Is_Available()) {
			delete AuxVoxel.VoxLib;
			AuxVoxel.VoxLib = new VoxelLibrary(tvxl);

			if (AuxVoxel.VoxLib != NULL && !AuxVoxel.VoxLib->Load_Failed()) {
				sprintf(buffer, "%sTUR", (char const *)GraphicName);
				_makepath(name, NULL, NULL, buffer, ".HVA");
				CCFileClass thva(name);

				delete AuxVoxel.MotLib;
				AuxVoxel.MotLib = new MotionLibrary(thva);
				AuxVoxel.MotLib->Scale(AuxVoxel.VoxLib->Get_Layer_Info(0, 0).Scale);
			}
		}

		sprintf(buffer, "%sBARL", (char const *)GraphicName);
		_makepath(name, NULL, NULL, buffer, ".VXL");
		CCFileClass bvxl(name);

		if (bvxl.Is_Available()) {
			delete AuxVoxel2.VoxLib;
			AuxVoxel2.VoxLib = new VoxelLibrary(bvxl);

			if (AuxVoxel2.VoxLib != NULL && !AuxVoxel2.VoxLib->Load_Failed()) {
				sprintf(buffer, "%sBARL", (char const *)GraphicName);
				_makepath(name, NULL, NULL, buffer, ".HVA");
				CCFileClass bhva(name);

				delete AuxVoxel2.MotLib;
				AuxVoxel2.MotLib = new MotionLibrary(bhva);
				AuxVoxel2.MotLib->Scale(AuxVoxel2.VoxLib->Get_Layer_Info(0, 0).Scale);
			}
		}
	}
}

void FUN_005F96B0(char * name, TheaterType theater);
void FUN_005F9710(char * name);

// FUNCTION: GAMEMD 0x005F9070
void ObjectTypeClass::Fetch_Normal_Image(void)
{
	char fullname[260];

	if (AlternateArcticArt && Scen->Theater == THEATER_SNOW && !ImageAllocated) {
		if (!ArcticArtInUse) {
			char arctic[256];
			sprintf(arctic, "%sA", (char const *)GraphicName);
			GraphicName = arctic;
			ArcticArtInUse = true;
		}
	} else {
		ArcticArtInUse = false;
	}

	_makepath(fullname, NULL, NULL, GraphicName, ".SHP");

	if (IsTheater) {
		_makepath(fullname, NULL, NULL, GraphicName, Theaters[Scen->Theater].Suffix);
	} else if (IsNewTheater) {
		FUN_005F96B0(fullname, Scen->Theater);
	}

	if (ImageAllocated && ImageData != NULL) {
		delete [] (char *)ImageData;
	}
	ImageData = NULL;
	ImageAllocated = false;

	if (RTTI == RTTI_OVERLAYTYPE || RTTI == RTTI_ANIMTYPE) {
		ImageData = FileSystem::LoadFile(fullname, true);
		if (ImageData == NULL) {
			FUN_005F9710(fullname);
			ImageData = FileSystem::LoadFile(fullname, true);
		}
	} else if (RTTI != RTTI_SMUDGETYPE && RTTI != RTTI_TERRAINTYPE) {
		ImageData = FileSystem::LoadFile(fullname, false);
		if (ImageData == NULL) {
			FUN_005F9710(fullname);
			ImageData = FileSystem::LoadFile(fullname, false);
		}
	}

	if (ImageData != NULL) {
		ShapeSet const * image = (ShapeSet const *)ImageData;
		int maxsize = MAX(image->Get_Width(), image->Get_Height());
		maxsize = MAX(maxsize, 8);
		MaxSize = maxsize;
	}
}

// FUNCTION: GAMEMD 0x005F92D0
bool ObjectTypeClass::Read_INI(CCINIClass & ini)
{
	char namebuffer[25];
	char path[_MAX_PATH];

	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		strcpy(namebuffer, GraphicName);
		ini.Get_String(IniName, "Image", namebuffer, GraphicName, sizeof(namebuffer));

		strcpy(namebuffer, AlphaGraphicName);
		ini.Get_String(IniName, "AlphaImage", namebuffer, AlphaGraphicName, sizeof(namebuffer));

		CrushSound = ini.Get_VocType(IniName, "CrushSound", CrushSound);
		AmbientSound = ini.Get_VocType(IniName, "AmbientSound", (VocType)AmbientSound);

		IsCrushable = ini.Get_Bool(IniName, "Crushable", IsCrushable);
		IsBombable = ini.Get_Bool(IniName, "Bombable", IsBombable);
		NoSpawnAlt = ini.Get_Bool(IniName, "NoSpawnAlt", NoSpawnAlt);
		AlternateArcticArt = ini.Get_Bool(IniName, "AlternateArcticArt", AlternateArcticArt);
		IsStealthy = ini.Get_Bool(IniName, "RadarInvisible", IsStealthy);
		IsSelectable = ini.Get_Bool(IniName, "Selectable", IsSelectable);
		IsLegalTarget = ini.Get_Bool(IniName, "LegalTarget", IsLegalTarget);
		Armor = ini.Get_ArmorType(IniName, "Armor", Armor);
		MaxStrength = ini.Get_Int(IniName, "Strength", MaxStrength);
		IsImmune = ini.Get_Bool(IniName, "Immune", IsImmune);
		IsInsignificant = ini.Get_Bool(IniName, "Insignificant", IsInsignificant);
		IsHasRadialIndicator = ini.Get_Bool(IniName, "HasRadialIndicator", IsHasRadialIndicator);
		RadialColor = ini.Get_RGBClass(IniName, "RadialColor", RadialColor);
		IsIgnoresFirestorm = ini.Get_Bool(IniName, "IgnoresFirestorm", IsIgnoresFirestorm);

		UseLineTrail = ArtINI.Get_Bool(GraphicName, "UseLineTrail", UseLineTrail);
		LineTrailColor = ArtINI.Get_RGBClass(GraphicName, "LineTrailColor", LineTrailColor);
		LineTrailColorDecrement = ArtINI.Get_Int(GraphicName, "LineTrailColorDecrement", LineTrailColorDecrement);
		IsTheater = ArtINI.Get_Bool(GraphicName, "Theater", IsTheater);
		IsNewTheater = ArtINI.Get_Bool(GraphicName, "NewTheater", IsNewTheater);
		IsVoxel = ArtINI.Get_Bool(GraphicName, "Voxel", IsVoxel);

		if (!IsVoxel && RTTI != RTTI_ANIMTYPE) {
			Fetch_Normal_Image();
		}

		if (AlphaGraphicName[0] != '\0') {
			_makepath(path, NULL, NULL, AlphaGraphicName, ".SHP");
			AlphaImageData = FileSystem::LoadFile(path, false);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005F96B0
void FUN_005F96B0(char * name, TheaterType theater)
{
	if (theater != THEATER_NONE) {
		char first = name[0];
		char second = name[1];
		if (first >= 'A' && first <= 'Z') first += 32;
		if (second >= 'A' && second <= 'Z') second += 32;
		if (first == 'g' || first == 'n' || first == 'c' || first == 'y') {
			if (second == 'a' || second == 't') {
				name[1] = Theaters[theater].Letter[0];
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005F9710
void FUN_005F9710(char * name)
{
	name[1] = 'G';
}

// FUNCTION: GAMEMD 0x005F9720
HRESULT STDMETHODCALLTYPE ObjectTypeClass::Load(IStream * stream)
{
	char filename[260];

	Clear_Voxel_Index();

	delete Voxel.VoxLib;
	Voxel.VoxLib = NULL;

	delete Voxel.MotLib;
	Voxel.MotLib = NULL;

	delete AuxVoxel.VoxLib;
	AuxVoxel.VoxLib = NULL;

	delete AuxVoxel.MotLib;
	AuxVoxel.MotLib = NULL;

	delete AuxVoxel2.VoxLib;
	AuxVoxel2.VoxLib = NULL;

	delete AuxVoxel2.MotLib;
	AuxVoxel2.MotLib = NULL;

	Free_Voxels();

	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		ImageData = NULL;
		ImageAllocated = false;
		AlphaImageData = NULL;

		new (&VoxelIndex) VoxelIndexClass;
		new (&AuxVoxelIndex) VoxelIndexClass;
		new (&ShadowVoxelIndex) VoxelIndexClass;
		new (&AuxVoxel2Index) VoxelIndexClass;

		Voxel.VoxLib = NULL;
		Voxel.MotLib = NULL;
		AuxVoxel.VoxLib = NULL;
		AuxVoxel.MotLib = NULL;
		AuxVoxel2.VoxLib = NULL;
		AuxVoxel2.MotLib = NULL;

		for (int index = 0; index < 18; index++) {
			ChargerTurrets[index].VoxLib = NULL;
			ChargerTurrets[index].MotLib = NULL;
			ChargerBarrels[index].VoxLib = NULL;
			ChargerBarrels[index].MotLib = NULL;
		}

		if (*AlphaGraphicName != NULL) {
			_makepath(filename, NULL, NULL, AlphaGraphicName, ".SHP");
			AlphaImageData = FileSystem::LoadFile(filename, false);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005F9950
HRESULT STDMETHODCALLTYPE ObjectTypeClass::Save(IStream *stream, int cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005F9970
HRESULT STDMETHODCALLTYPE ObjectTypeClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	HRESULT res = BASECLASS::GetSizeMax(pcbSize);
	if (SUCCEEDED(res)) {
		res = S_OK;
	}
	return(res);
}

// FUNCTION: GAMEMD 0x005F9990
ObjectTypeClass const * ObjectTypeClass::From_Name(char const * name)
{
	for (int classid = 0; classid < ObjectTypes.Count(); classid++) {
		if (stricmp(ObjectTypes[classid]->IniName, name) == 0) {
			return(ObjectTypes[classid]);
		}
	}
	return(NULL);
}
