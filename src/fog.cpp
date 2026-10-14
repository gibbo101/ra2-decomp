// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "fog.h"
#include "_convert.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "convert.h"
#include "coord.h"
#include "crc.h"
#include "draw.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "inline.h"
#include "lightcon.h"
#include "objtype.h"
#include "rect.h"
#include "scheme.h"
#include "shapeset.h"
#include "smudtype.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "terrain.h"
#include "vector.h"

// FUNCTION: 0x004D0560 $E2
// FUNCTION: 0x004D0590 $E5
// FUNCTION: 0x004D05B0 $E8
// FUNCTION: 0x004D05D0 $E11
// FUNCTION: 0x004D05F0 $E14
// FUNCTION: 0x004D0610 $E17
// FUNCTION: 0x004D0630 $E20
// FUNCTION: 0x004D0670 $E23
// FUNCTION: 0x004D06A0 $E26
// FUNCTION: 0x004D06D0 $E29
// FUNCTION: 0x004D0700 $E32
// FUNCTION: 0x004D0710 $E35

// FUNCTION: 0x004D0730 $E38
// FUNCTION: 0x004D0750 $E134
// FUNCTION: 0x004D0790 $E131
DynamicVectorClass<FoggedObjectClass *> FoggedObjectClass::FoggyObjects;

// FUNCTION: 0x004D07F0 $E140
// FUNCTION: 0x004D0820 $E137
IndexClass<int, FoggedObjectClass *> FoggedObjectClass::FoggedObjectIndex;

// FUNCTION: 0x004D0860
FoggedObjectClass::FoggedObjectClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x004D08B0
FoggedObjectClass::FoggedObjectClass(void) :
	BASECLASS(),
	Overlay(OVERLAY_NONE),
	House(NULL),
	OverlayData(0),
	RTTI(RTTI_NONE),
	Position(0,0,0),
	BoundingRect(0,0,0,0),
	CellHeight(0),
	Smudge(SMUDGE_NONE),
	SmudgeData(0),
	Records(),
	CanDraw(true)
{
	FoggyObjects.Add(this);
}

// FUNCTION: 0x004D0980
FoggedObjectClass::FoggedObjectClass(Coord const & coord, OverlayType type, int data) :
	BASECLASS(),
	Overlay(type),
	House(NULL),
	OverlayData(data),
	RTTI(RTTI_OVERLAY),
	Position(coord),
	CellHeight(Map[coord].Height),
	Smudge(SMUDGE_NONE),
	SmudgeData(0),
	Records(),
	CanDraw(true)
{
	CellClass * cellptr = &Map[coord];
	BoundingRect = Union(cellptr->Overlay_Render_Rect(), cellptr->Overlay_Shadow_Render_Rect());
	BoundingRect.X += TacticalMap->TacPixelX;
	BoundingRect.Y += TacticalMap->TacPixelY;
	BoundingRect -= TacticalRect.Top_Left();

	FoggyObjects.Add(this);

	Cell cell = Position.As_Cell();
	int id = cell.As_Int() - RTTI + INT_MAX;
	FoggedObjectIndex.Add_Index(id, this);
}

// FUNCTION: 0x004D0C40
FoggedObjectClass::FoggedObjectClass(Coord const & coord, SmudgeType type, int data) :
	BASECLASS(),
	Overlay(OVERLAY_NONE),
	House(NULL),
	OverlayData(0),
	RTTI(RTTI_SMUDGE),
	CellHeight(Map[coord].Height),
	Smudge(type),
	SmudgeData(data),
	Records(),
	CanDraw(true)
{
	Coord crd;
	Point2D point;
	CellClass * cellptr = &Map[coord];
	crd = Coord(coord);
	crd.Z = LEVEL_LEPTON_H * cellptr->Height;
	Position = crd;

	TacticalMap->Coord_To_Pixel(crd, point);
	point += Point2D(ISO_TILE_PIXEL_W / -2, ISO_TILE_PIXEL_H / -2);
	BoundingRect = Rect(point.X, point.Y, ISO_TILE_PIXEL_W, ISO_TILE_PIXEL_H);
	BoundingRect.X += TacticalMap->TacPixelX;
	BoundingRect.Y += TacticalMap->TacPixelY;

	FoggyObjects.Add(this);

	Cell cell = Position.As_Cell();
	int id = cell.As_Int() - RTTI + INT_MAX;
	FoggedObjectIndex.Add_Index(id, this);
}

// FUNCTION: 0x004D0EF0
FoggedObjectClass::FoggedObjectClass(BuildingClass * object, bool fade) :
	BASECLASS(),
	Overlay(OVERLAY_NONE),
	House(object->House),
	OverlayData(0),
	RTTI(RTTI_BUILDING),
	Position(object->PositionCoord),
	CellHeight(Map[(Coord const &)object->PositionCoord].Height),
	Smudge(SMUDGE_NONE),
	SmudgeData(0),
	Records(),
	CanDraw(fade)
{
	BoundingRect = object->Get_Render_Rect();

	FoggyObjects.Add(this);

	Cell cell = Position.As_Cell();
	int id = cell.As_Int() - RTTI + INT_MAX;
	FoggedObjectIndex.Add_Index(id, this);

	object->IsFogged = true;
	int shapenum = object->Shape_Number();

	DrawRecord record(object->Class, shapenum, 0, 0);
	if (object->Class->IsLaserFence && (object->LaserFenceFrame == 12 || object->LaserFenceFrame == 8)) {
		record.HeightAdjust = 1;
	} else {
		record.HeightAdjust = 0;
		if (object->Class->IsFirestormWall) {
			record.HeightAdjust = 1;
		}
	}
	Records.Add(record);

	for (int i = 0; i < BANIM_COUNT; i++) {
		if (object->Anims[i] != NULL) {
			object->Anims[i]->IsFogged = true;
			AnimTypeClass *atype = object->Anims[i]->Type;
			BoundingRect = Union(BoundingRect, object->Anims[i]->Get_Visual_Rect());
			record.TypeClass = atype;
			record.FrameNumber = atype->Start + object->Anims[i]->Animation.Fetch_Stage();
			record.HeightAdjust = 0;
			record.ZAdjust = object->Anims[i]->ZAdjust;
			Records.Add(record);
		}
	}

	TacticalMap->Register_Dirty_Area(BoundingRect);
	BoundingRect.X += TacticalMap->TacPixelX;
	BoundingRect.Y += TacticalMap->TacPixelY;
}

// FUNCTION: 0x004D1370
FoggedObjectClass::FoggedObjectClass(TerrainClass * object) :
	BASECLASS(),
	Overlay(OVERLAY_NONE),
	House(NULL),
	OverlayData(0),
	RTTI(RTTI_TERRAIN),
	Position(object->PositionCoord),
	CellHeight(Map[(Coord const &)object->PositionCoord].Height),
	Smudge(SMUDGE_NONE),
	SmudgeData(0),
	Records(),
	CanDraw(true)
{
	BoundingRect = object->Get_Render_Rect();
	BoundingRect.X += TacticalMap->TacPixelX;
	BoundingRect.Y += TacticalMap->TacPixelY;

	FoggyObjects.Add(this);

	Records.Add(DrawRecord(object->Class, 0, 0, 0));

	Cell cell = Position.As_Cell();
	int id = cell.As_Int() - RTTI + INT_MAX;
	FoggedObjectIndex.Add_Index(id, this);
}

// FUNCTION: 0x004D27D0
HRESULT STDMETHODCALLTYPE FoggedObjectClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_FoggedObjectClass;
	return(S_OK);
}
