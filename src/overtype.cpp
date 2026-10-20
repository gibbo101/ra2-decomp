// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "overtype.h"
#include "_convert.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_theater.h"
#include "animtype.h"
#include "ccfile.h"
#include "ccini.h"
#include "data.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsurface.h"
#include "findmake.h"
#include "init.h"
#include "mixfile.h"
#include "overlay.h"
#include "resource.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"


// FUNCTION: 0x005FE040 $E2
// FUNCTION: 0x005FE060 $E5
// FUNCTION: 0x005FE090 $E8
// FUNCTION: 0x005FE0B0 $E11
// FUNCTION: 0x005FE0D0 $E14
// FUNCTION: 0x005FE0F0 $E17
// FUNCTION: 0x005FE110 $E20
// FUNCTION: 0x005FE130 $E23
// FUNCTION: 0x005FE170 $E26
// FUNCTION: 0x005FE1A0 $E29
// FUNCTION: 0x005FE1D0 $E32
// FUNCTION: 0x005FE200 $E35
// FUNCTION: 0x005FE210 $E38

// FUNCTION: 0x005FE230 $E41

// FUNCTION: 0x005FE250
OverlayTypeClass::OverlayTypeClass(char const * ininame) :
	BASECLASS(ininame),
	HeapID(OVERLAY_NONE),
	Land(LAND_CLEAR),
	CellAnim(NULL),
	DamageLevels(1),
	DamagePoints(1),
	IsWall(false),
	IsTiberium(false),
	IsCrate(false),
	IsCrateTrigger(false),
	IsNoUseTileLandType(true),
	IsVeinholeMonster(false),
	IsVeins(false),
	DemandLoad(false),
	IsExplosive(false),
	IsChainReaction(false),
	IsOverrides(false),
	IsDrawFlat(true),
	IsRubble(false),
	IsARock(false)
{
	Create_ID();

	IsStealthy = true;
	IsSelectable = false;
	IsInsignificant = true;
	IsFootprint = false;

	OverlayTypes.Add(this);
	HeapID = OverlayType(OverlayTypes.ID(this));
}

// FUNCTION: 0x005FE3C0
OverlayTypeClass::OverlayTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	RadarColor(x)
{
}

// FUNCTION: 0x005FE3F0
OverlayTypeClass::~OverlayTypeClass(void)
{
	Detach_This_From_All(this, true);
	OverlayTypes.Delete(this);
}

// FUNCTION: 0x005FE470
OverlayType OverlayTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int index = 0; index < OverlayTypes.Count(); index++) {
			if (stricmp(OverlayTypes[index]->IniName, name) == 0) {
				return(OverlayType(index));
			}
		}
	}
	return(OVERLAY_NONE);
}

// FUNCTION: 0x005FE4C0
Cell const * OverlayTypeClass::Occupy_List(bool) const
{
	static Cell const _simple[] = {Cell(0, 0), REFRESH_EOL};

	return(_simple);
}

// FUNCTION: 0x005FE520 $E128

extern char DAT_00889F64[];

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
	static ConvertClass * x_PAL;
};

void FUN_005F9710(char * name);
void FUN_005F96B0(char * name, TheaterType theater);

// FUNCTION: 0x005FE530
bool OverlayTypeClass::Create_And_Place(Cell const & cell, HouseClass *) const
{
	if (new OverlayClass((OverlayTypeClass *)this, cell)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005FE570
ObjectClass * OverlayTypeClass::Create_One_Of(HouseClass *) const
{
	return(new OverlayClass((OverlayTypeClass *)this, CELL_NONE));
}

// FUNCTION: 0x005FE5A0
void OverlayTypeClass::Draw_It(Point2D const & point, Rect const & cliprect, int data) const
{
	Draw_Shape(*DSurface::Temp, *FileSystem::x_PAL, (ShapeSet const *)Get_Image_Data(), data, DSurface::ViewBounds.Top_Left() + point + Point2D(ISO_TILE_PIXEL_W / 2, ISO_TILE_PIXEL_H / 2), cliprect, (ShapeFlags_Type)(SHAPE_CENTER|SHAPE_WIN_REL));
}

// FUNCTION: 0x005FE620
void OverlayTypeClass::Init(TheaterType theater)
{
	for (int index = 0; index < OverlayTypes.Count(); index++) {
		OverlayTypeClass & overlay = *OverlayTypes[index];

		char fullname[_MAX_FNAME+_MAX_EXT];
		if (!overlay.DemandLoad) {
			if (overlay.IsTheater) {
				_makepath(fullname, NULL, NULL, overlay.GraphicName, Theaters[theater].Suffix);
				overlay.ImageData = FileSystem::LoadFile(fullname, true);
				if (overlay.ImageData == NULL) {
					FUN_005F9710(fullname);
					overlay.ImageData = FileSystem::LoadFile(fullname, true);
				}
			} else if (overlay.IsNewTheater) {
				_makepath(fullname, NULL, NULL, overlay.GraphicName, ".SHP");
				FUN_005F96B0(fullname, theater);
				overlay.ImageData = FileSystem::LoadFile(fullname, true);
				if (overlay.ImageData == NULL) {
					FUN_005F9710(fullname);
					overlay.ImageData = FileSystem::LoadFile(fullname, true);
				}
			}
		} else {
			if (overlay.IsTheater || overlay.IsNewTheater) {
				if (overlay.ImageData != NULL && overlay.ImageAllocated) {
					delete (ShapeSet *)overlay.ImageData;
					overlay.ImageData = NULL;
					overlay.ImageAllocated = false;
				}
			}
		}
	}
}

// FUNCTION: 0x005FE770
bool OverlayTypeClass::Read_INI(CCINIClass & ini)
{
	char fullname[_MAX_FNAME+_MAX_EXT];

	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		Land = ini.Get_LandType(IniName, "Land", Land);
		DamagePoints = ini.Get_Int(IniName, "Strength", DamagePoints);
		IsWall = ini.Get_Bool(IniName, "Wall", IsWall);
		IsTiberium = ini.Get_Bool(IniName, "Tiberium", IsTiberium);
		IsCrate = ini.Get_Bool(IniName, "Crate", IsCrate);
		IsCrateTrigger = ini.Get_Bool(IniName, "CrateTrigger", IsCrateTrigger);
		IsExplosive = ini.Get_Bool(IniName, "Explodes", IsExplosive);
		IsOverrides = ini.Get_Bool(IniName, "Overrides", IsOverrides);

		char buffer[128];
		AnimTypeClass * old = CellAnim;
		CellAnim = ini.Get_String(IniName, "CellAnim", DAT_00889F64, buffer, sizeof(buffer)) ? AnimTypeClass::Find_Or_Make(buffer) : old;

		DamageLevels = ArtINI.Get_Int(GraphicName, "DamageLevels", DamageLevels);

		if (IsTiberium) {
			Armor = ARMOR_WOOD;
			if (Land == LAND_CLEAR) {
				Land = (LandType)5;
			}
		}

		if (!IsTheater && !DemandLoad) {
			_makepath(fullname, NULL, NULL, GraphicName, ".SHP");
			ImageData = FileSystem::LoadFile(fullname, true);
		}

		RadarColor = ini.Get_RGBClass(IniName, "RadarColor", RadarColor);
		IsNoUseTileLandType = ini.Get_Bool(IniName, "NoUseTileLandType", IsNoUseTileLandType);
		IsVeinholeMonster = ini.Get_Bool(IniName, "IsVeinholeMonster", IsVeinholeMonster);
		IsVeins = ini.Get_Bool(IniName, "IsVeins", IsVeins);
		IsChainReaction = ini.Get_Bool(IniName, "ChainReaction", IsChainReaction);
		IsDrawFlat = ini.Get_Bool(IniName, "DrawFlat", IsDrawFlat);
		IsARock = ini.Get_Bool(IniName, "IsARock", IsARock);
		IsRubble = ini.Get_Bool(IniName, "IsRubble", IsRubble);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005FEA30
Coord const OverlayTypeClass::Coord_Fixup(Coord const & coord) const
{
	return(coord);
}

// FUNCTION: 0x005FEA50
void OverlayTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(HeapID);
	crc(Land);
	crc(DamageLevels);
	crc(DamagePoints);
	crc(IsWall);
	crc(IsTiberium);
	crc(IsCrate);
	crc(IsCrateTrigger);
	crc(IsExplosive);
}

// FUNCTION: 0x005FEC10
HRESULT STDMETHODCALLTYPE OverlayTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x005FEC30
HRESULT STDMETHODCALLTYPE OverlayTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_OverlayTypeClass;
	return(S_OK);
}
