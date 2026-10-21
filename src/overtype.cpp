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


// SYNTHETIC: GAMEMD 0x005FE040 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005FE060 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005FE090 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005FE0B0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005FE0D0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005FE0F0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005FE110 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005FE130 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005FE170 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005FE1A0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005FE1D0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005FE200 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005FE210 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x005FE230 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x005FE250
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

// FUNCTION: GAMEMD 0x005FE3C0
OverlayTypeClass::OverlayTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	RadarColor(x)
{
}

// FUNCTION: GAMEMD 0x005FE3F0
OverlayTypeClass::~OverlayTypeClass(void)
{
	Detach_This_From_All(this, true);
	OverlayTypes.Delete(this);
}

// FUNCTION: GAMEMD 0x005FE470
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

// FUNCTION: GAMEMD 0x005FE4C0
Cell const * OverlayTypeClass::Occupy_List(bool) const
{
	static Cell const _simple[] = {Cell(0, 0), REFRESH_EOL};

	return(_simple);
}

// SYNTHETIC: GAMEMD 0x005FE520 SYMBOL
// _$E128

extern char DAT_00889F64[];

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
	static ConvertClass * x_PAL;
};

void FUN_005F9710(char * name);
void FUN_005F96B0(char * name, TheaterType theater);

// FUNCTION: GAMEMD 0x005FE530
bool OverlayTypeClass::Create_And_Place(Cell const & cell, HouseClass *) const
{
	if (new OverlayClass((OverlayTypeClass *)this, cell)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005FE570
ObjectClass * OverlayTypeClass::Create_One_Of(HouseClass *) const
{
	return(new OverlayClass((OverlayTypeClass *)this, CELL_NONE));
}

// FUNCTION: GAMEMD 0x005FE5A0
void OverlayTypeClass::Draw_It(Point2D const & point, Rect const & cliprect, int data) const
{
	Draw_Shape(*DSurface::Temp, *FileSystem::x_PAL, (ShapeSet const *)Get_Image_Data(), data, DSurface::ViewBounds.Top_Left() + point + Point2D(ISO_TILE_PIXEL_W / 2, ISO_TILE_PIXEL_H / 2), cliprect, (ShapeFlags_Type)(SHAPE_CENTER|SHAPE_WIN_REL));
}

// FUNCTION: GAMEMD 0x005FE620
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

// FUNCTION: GAMEMD 0x005FE770
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

// FUNCTION: GAMEMD 0x005FEA30
Coord const OverlayTypeClass::Coord_Fixup(Coord const & coord) const
{
	return(coord);
}

// FUNCTION: GAMEMD 0x005FEA50
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

// FUNCTION: GAMEMD 0x005FEC10
HRESULT STDMETHODCALLTYPE OverlayTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005FEC30
HRESULT STDMETHODCALLTYPE OverlayTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_OverlayTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x005FEAF0
HRESULT STDMETHODCALLTYPE OverlayTypeClass::Load(IStream * stream)
{
	char fullname[_MAX_FNAME+_MAX_EXT];
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) OverlayTypeClass(NoInitClass());

		Fetch_Voxel_Image();
		Fetch_Normal_Image();

		Swizzle_Pointer(&CellAnim);

		if (!DemandLoad) {
			if (IsTheater) {
				_makepath(fullname, NULL, NULL, GraphicName, Theaters[Scen->Theater].Suffix);
			} else {
				_makepath(fullname, NULL, NULL, GraphicName, ".SHP");
				FUN_005F96B0(fullname, Scen->Theater);
			}

			if (ImageData == NULL) {
				ImageData = FileSystem::LoadFile(fullname, true);
			}
		}
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005FEC70
OverlayTypeClass * OverlayTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<OverlayTypeClass>(name, OverlayTypes));
}

// FUNCTION: GAMEMD 0x005FED00
RGBClass OverlayTypeClass::Get_Radar_Color(int shape) const
{
	ShapeSet const *image = (const ShapeSet *) Get_Image_Data();

	if (image == NULL && CellAnim != NULL) {
		image = (const ShapeSet *)CellAnim->Get_Image_Data();
	}

	if (image != NULL) {
		if ((HeapID >= OVERLAY_TIBERIUM2_01 && HeapID <= OVERLAY_TIBERIUM2_12) ||
			(HeapID >= OVERLAY_TIBERIUM3_01 && HeapID <= OVERLAY_TIBERIUM3_12)) {

			RGBClass color = image->Get_Color(shape);
			return(RGBClass (color.Get_Red(), color.Get_Blue(), color.Get_Green()));
		}
		return(image->Get_Color(shape));
	}

	return(RGBClass(0, 0, 0));
}

// FUNCTION: GAMEMD 0x005FEDE0
void const * OverlayTypeClass::Get_Image_Data(void) const
{
	char fullname[_MAX_FNAME+_MAX_EXT];

	if (ImageData != NULL || DemandLoad == false) {
		return(ImageData);
	}

	OverlayTypeClass * _this = (OverlayTypeClass *)this;

	DebugString("Demand loading image for %s\n", (char const *)GivenName);
	if (IsTheater) {
		_makepath(fullname, NULL, NULL, GraphicName, Theaters[Scen->Theater].Suffix);

	} else {
		if (IsNewTheater) {
			_makepath(fullname, NULL, NULL, GraphicName, ".SHP");
			FUN_005F96B0(fullname, Scen->Theater);
		}
	}

	CCFileClass file (fullname);

	_this->ImageData = Load_Alloc_Data(file);
	if (ImageData != NULL) {
		_this->ImageAllocated = true;
	}

	return(ImageData);
}

// FUNCTION: GAMEMD 0x005FEF00
// OverlayTypeClass::Fetch_RTTI

// FUNCTION: GAMEMD 0x005FEF10
// OverlayTypeClass::Fetch_Object_Size

// FUNCTION: GAMEMD 0x005FEF20
// OverlayTypeClass::Fetch_Heap_ID

// SYNTHETIC: GAMEMD 0x005FEF30
// ??_GOverlayTypeClass@@UAEPAXI@Z

extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007EF6A8;
extern const double DAT_007EF6B0;

// SYNTHETIC: GAMEMD 0x005FEFC0 SYMBOL
// _$E131
double DAT_00AC17D8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x005FEFF0 SYMBOL
// _$E134
double DAT_00AC1828 = DAT_00AC17D8 / DAT_007EF6A8;

// SYNTHETIC: GAMEMD 0x005FF010 SYMBOL
// _$E137
double DAT_00AC1810 = DAT_007E1718 / DAT_00AC1828;

// SYNTHETIC: GAMEMD 0x005FF030 SYMBOL
// _$E140
double DAT_00AC1808 = DAT_007E1720 * DAT_007EF6B0;

extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;

// SYNTHETIC: GAMEMD 0x005FF050 SYMBOL
// _$E143
double DAT_00AC17E0 = DAT_007E1728 * DAT_007EF6B0;

// SYNTHETIC: GAMEMD 0x005FF070 SYMBOL
// _$E146
double DAT_00AC1690 = DAT_007E1730 * DAT_007EF6B0;

// SYNTHETIC: GAMEMD 0x005FF090 SYMBOL
// _$E149
int DAT_00AC1830 = (int)(fastmath::tan(DAT_00AC1690 - DAT_00AC17E0) * DAT_007E1738 * DAT_00AC17D8);

// SYNTHETIC: GAMEMD 0x005FF0D0 SYMBOL
// _$E152
double DAT_00AC17E8 = fastmath::atan(DAT_00AC1830 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005FF100 SYMBOL
// _$E155
double DAT_00AC1800 = fastmath::atan((DAT_00AC1830 * 2) / DAT_00AC17D8);

// SYNTHETIC: GAMEMD 0x005FF130 SYMBOL
// _$E158
int DAT_00AC1824 = (int)((DAT_00AC1830 * 4) + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x005FF160 SYMBOL
// _$E161
Cell DAT_00AC17D0(0, 0);

// SYNTHETIC: GAMEMD 0x005FF170 SYMBOL
// _$E164
Coord DAT_00AC17F0(0, 0, 0);

// SYNTHETIC: GAMEMD 0x005FF190 SYMBOL
// _$E167
Coord DAT_00AC1818(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0);

// SYNTHETIC: GAMEMD 0x005FF1B0 SYMBOL
// _$E170
Rect DAT_00AC17C0(0, 0, 0, 0);
