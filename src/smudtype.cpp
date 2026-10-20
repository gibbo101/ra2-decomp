// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "smudtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "ccfile.h"
#include "ccini.h"
#include "ccrand.h"
#include "cell.h"
#include "data.h"
#include "draw.h"
#include "findmake.h"
#include "globals.h"
#include "isotype.h"
#include "lightcon.h"
#include "mixfile.h"
#include "mouse.h"
#include "scenario.h"
#include "smudge.h"
#include "sun.h"
#include "tactical.h"
#include "tracker.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F34E8;
extern const double DAT_007F34F0;

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x006B5070 $E137
double DAT_00B0B790 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x006B50A0 $E140
double DAT_00B0B7E0 = DAT_00B0B790 / DAT_007F34E8;

// FUNCTION: 0x006B50C0 $E143
double DAT_00B0B7C8 = DAT_007E1718 / DAT_00B0B7E0;

// FUNCTION: 0x006B50E0 $E146
double DAT_00B0B7C0 = DAT_007E1720 * DAT_007F34F0;

// FUNCTION: 0x006B5100 $E149
double DAT_00B0B798 = DAT_007E1728 * DAT_007F34F0;

// FUNCTION: 0x006B5120 $E152
double DAT_00B0B770 = DAT_007E1730 * DAT_007F34F0;

// FUNCTION: 0x006B5140 $E155
int DAT_00B0B7E8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0B770 - DAT_00B0B798) * DAT_00B0B790));

// FUNCTION: 0x006B5180 $E158
double DAT_00B0B7A0 = FUN_004CADE0(DAT_00B0B7E8 * DAT_007E1740);

// FUNCTION: 0x006B51B0 $E161
double DAT_00B0B7B8 = FUN_004CADE0((DAT_00B0B7E8 * 2) / DAT_00B0B790);

// FUNCTION: 0x006B51E0 $E164
int DAT_00B0B7DC = (int)(DAT_00B0B7E8 * 4 + DAT_007E1738);

// FUNCTION: 0x006B5210 $E167
ShortPair DAT_00B0B788;

// FUNCTION: 0x006B5220 $E170
IntTriple DAT_00B0B7A8;

struct SmudgeTypeInit {
	int A, B, C;
	SmudgeTypeInit() { C = 0; A = 128; B = 128; }
};

// FUNCTION: 0x006B5240 $E173
SmudgeTypeInit DAT_00B0B7D0;

// FUNCTION: 0x006B5260
SmudgeTypeClass::SmudgeTypeClass(char const * ini) :
	BASECLASS(ini),
	HeapID(SMUDGE_NONE),
	Width(1),
	Height(1),
	IsCrater(false),
	IsScorch(false)
{
	Create_ID();

	IsStealthy = true;
	IsSelectable = false;
	IsLegalTarget = false;
	IsInsignificant = true;
	IsImmune = true;
	IsFootprint = false;

	SmudgeTypes.Add(this);
	HeapID = (SmudgeType)SmudgeTypes.ID(this);
}

// FUNCTION: 0x006B5370
SmudgeTypeClass::SmudgeTypeClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x006B53A0
SmudgeTypeClass::~SmudgeTypeClass(void)
{
	if (ImageData != NULL) {
		delete (void *)ImageData;
		ImageData = NULL;
	}
	Detach_This_From_All(this, true);
	SmudgeTypes.Delete(this);
}

// FUNCTION: 0x006B5440
SmudgeType SmudgeTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int index = SMUDGE_FIRST; index < SmudgeTypes.Count(); index++) {
			if (stricmp(SmudgeTypes[index]->Name(), name) == 0) {
				return(SmudgeType(index));
			}
		}
	}
	return(SMUDGE_NONE);
}

// FUNCTION: 0x006B5490
void SmudgeTypeClass::Init(TheaterType theater)
{
	for (int index = SMUDGE_FIRST; index < SmudgeTypes.Count(); index++) {
		SmudgeTypeClass * smudge = SmudgeTypes[index];
		char fullname[_MAX_FNAME+_MAX_EXT];
		if (smudge->IsTheater) {
			_makepath(fullname, NULL, NULL, smudge->Name(), Theaters[theater].Suffix);
			CCFileClass file(fullname);
			smudge->ImageData = Load_Alloc_Data(file);
		}
	}
}

// FUNCTION: 0x006B5550
bool SmudgeTypeClass::Create_And_Place(Cell const & cell, HouseClass * house) const
{
	if (new SmudgeClass(this, cell)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006B55C0
ObjectClass * SmudgeTypeClass::Create_One_Of(HouseClass *) const
{
	return(new SmudgeClass(this));
}

// FUNCTION: 0x006B55F0
void SmudgeTypeClass::Draw_It(Point2D const & point, Rect const & cliprect, int size, int z, Cell const & cell) const
{
	ShapeSet const * ptr = (ShapeSet const *)Get_Image_Data();
	if (ptr != NULL) {
		Point2D drawpoint = point;
		if (size != 0) {
			int width = size % Width;
			int height = size / Width;
			drawpoint.Y -= 15 * (height + width);
			drawpoint.X += 60 * (height - width) / 2;
		}
		CellClass * cellptr = &Map[cell];
		int brightness = cellptr->TileBrightness;
		Draw_Shape(*LogicalSurface, *cellptr->Drawer, ptr, 0, drawpoint, cliprect, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA), NULL, -1 - TacticalMap->Z_Lepton_To_Pixel(z), ZGRAD_GROUND, brightness);
	}
}

// FUNCTION: 0x006B56D0
bool SmudgeTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		IsCrater = ini.Get_Bool(Name(), "Crater", IsCrater);
		IsScorch = ini.Get_Bool(Name(), "Burn", IsScorch);
		Width = ini.Get_Int(Name(), "Width", Width);
		Height = ini.Get_Int(Name(), "Height", Height);

		char fullname[_MAX_FNAME+_MAX_EXT];
		if (!IsTheater) {
			_makepath(fullname, NULL, NULL, (char const *)Graphic_Name(), ".SHP");
			ImageData = FileSystem::LoadFile(fullname, false);
		} else {
			_makepath(fullname, NULL, NULL, (char const *)Graphic_Name(), Theaters[Scen->Theater].Suffix);
			ImageData = FileSystem::LoadFile(fullname, false);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006B57F0
void SmudgeTypeClass::Compute_CRC(CRCEngine &crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(HeapID);
	crc(IsCrater);
	crc(IsScorch);
	crc(Width);
	crc(Height);
}

// FUNCTION: 0x006B5850
HRESULT STDMETHODCALLTYPE SmudgeTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) SmudgeTypeClass(NoInitClass());

		Fetch_Voxel_Image();
		Fetch_Normal_Image();

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006B58B0
HRESULT STDMETHODCALLTYPE SmudgeTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006B58D0
HRESULT STDMETHODCALLTYPE SmudgeTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SmudgeTypeClass;
	return(S_OK);
}

// FUNCTION: 0x006B5910
SmudgeTypeClass * SmudgeTypeClass::Find_Or_Make(const char *name)
{
	return(TFind_Or_Make<SmudgeTypeClass>(name, SmudgeTypes));
}

// FUNCTION: 0x006B59A0
bool SmudgeTypeClass::Scorch_The_Ground(Coord const & coord, int width, int height, bool large)
{
	int i;

	if (coord.As_Cell() == CELL_NONE) {
		return(false);
	}

	DynamicVectorClass<SmudgeTypeClass const *> list;
	for (i = 0; i < SmudgeTypes.Count(); i++) {
		SmudgeTypeClass const * smudge = SmudgeTypes[i];
		if (smudge->IsScorch) {
			if (smudge->Can_Place_Here(coord.As_Cell(), large)) {
				list.Add(smudge);
			}
		}
	}

	DynamicVectorClass<SmudgeTypeClass const *> largelist;
	for (i = 0; i < list.Count(); i++) {
		SmudgeTypeClass const * scorch = list[i];
		if (!large) {
			if (scorch->Width == 1 && scorch->Height == 1 || width > 60 && height > 50) {
				largelist.Add(scorch);
			}
		} else {
			if (scorch->Width > 1 && scorch->Height > 1) {
				largelist.Add(scorch);
			}
		}
	}

	if (largelist.Count() > 0) {
		new SmudgeClass(largelist[Random_Pick(0, largelist.Count() - 1)], coord);
	} else if (list.Count() > 0) {
		new SmudgeClass(list[Random_Pick(0, list.Count() - 1)], coord);
	}

	return(false);
}
// FUNCTION: 0x006B5C90
bool SmudgeTypeClass::Crater_The_Ground(Coord const & coord, int width, int height, bool large)
{
	int i;

	if (coord.As_Cell() == CELL_NONE) {
		return(false);
	}

	DynamicVectorClass<SmudgeTypeClass const *> list;
	for (i = 0; i < SmudgeTypes.Count(); i++) {
		SmudgeTypeClass const * smudge = SmudgeTypes[i];
		if (smudge->IsCrater) {
			if (smudge->Can_Place_Here(coord.As_Cell(), large)) {
				list.Add(smudge);
			}
		}
	}

	DynamicVectorClass<SmudgeTypeClass const *> largelist;
	for (i = 0; i < list.Count(); i++) {
		SmudgeTypeClass const * scorch = list[i];
		if (!large) {
			if (scorch->Width == 1 && scorch->Height == 1 || width > 60 && height > 50) {
				largelist.Add(scorch);
			}
		} else {
			if (scorch->Width > 1 && scorch->Height > 1) {
				largelist.Add(scorch);
			}
		}
	}

	if (largelist.Count() > 0) {
		new SmudgeClass(largelist[Random_Pick(0, largelist.Count() - 1)], coord);
	} else if (list.Count() > 0) {
		new SmudgeClass(list[Random_Pick(0, list.Count() - 1)], coord);
	}

	return(false);
}

// FUNCTION: 0x006B5F80
bool SmudgeTypeClass::Can_Place_Here(Cell const & origin, bool underbuildings) const
{
	for (int h = 0; h < Height; h++) {
		for (int w = 0; w < Width; w++) {
			CellClass * cell = &Map[origin + Cell(w, h)];
			if (!Map.In_Radar(origin)) {
				return(false);
			}
			if (cell->Ramp != 0) {
				return(false);
			}
			if (cell->Smudge != SMUDGE_NONE) {
				return(false);
			}
			if (cell->Overlay != OVERLAY_NONE) {
				return(false);
			}
			if (!underbuildings && cell->Cell_Building() != NULL) {
				return(false);
			}
			IsometricTileType ittype = cell->ITType;
			if (cell->ITType < ISOTILE_FIRST || cell->ITType >= IsometricTileTypes.Count()) {
				ittype = ISOTILE_FIRST;
			}
			if (!IsometricTileTypes[ittype]->IsMorphable) {
				return(false);
			}
		}
	}
	return(true);
}

// FUNCTION: 0x006B6080
void SmudgeTypeClass::Place(Cell const & origin) const
{
	for (int h = 0; h < Height; h++) {
		for (int w = 0; w < Width; w++) {
			Cell newcell = origin + Cell(w, h);
			CellClass * cell = &Map[newcell];
			cell->Smudge = HeapID;
			cell->SmudgeData = w + (h*Width);
			cell->Register_For_Redraw();
		}
	}
}

// FUNCTION: 0x006B6130 SmudgeTypeClass::Fetch_RTTI
// FUNCTION: 0x006B6140 SmudgeTypeClass::Fetch_Object_Size
// FUNCTION: 0x006B6150 SmudgeTypeClass::Fetch_Heap_ID
// FUNCTION: 0x006B6160 SmudgeTypeClass::~SmudgeTypeClass

struct SmudgeTypeZero {
	int A, B, C, D;
	SmudgeTypeZero() { A = 0; B = 0; C = 0; D = 0; }
};

// FUNCTION: 0x006B6210 $E176
SmudgeTypeZero DAT_00B0B7F0;
