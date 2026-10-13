// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "isotype.h"
#include "_alpha.h"
#include "_convert.h"
#include "_map.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_rules.h"
#include "_surface.h"
#include "_theater.h"
#include "_zbuffer.h"
#include "abuffer.h"
#include "alphalighting.h"
#include "animtype.h"
#include "ccfile.h"
#include "ccini.h"
#include "cell.h"
#include "conquer.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsurface.h"
#include "globals.h"
#include "isotile.h"
#include "lightcon.h"
#include "mixfile.h"
#include "palette.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"
#include "zbuffer.h"

// FUNCTION: 0x00543C60 $E2
// FUNCTION: 0x00543C90 $E5
// FUNCTION: 0x00543CB0 $E8
// FUNCTION: 0x00543CD0 $E11
// FUNCTION: 0x00543CF0 $E14
// FUNCTION: 0x00543D10 $E17
// FUNCTION: 0x00543D30 $E20
// FUNCTION: 0x00543D70 $E23
// FUNCTION: 0x00543DA0 $E26
// FUNCTION: 0x00543DD0 $E29
// FUNCTION: 0x00543E00 $E32
// FUNCTION: 0x00543E10 $E35
// FUNCTION: 0x00543E30 $E38
// FUNCTION: 0x00543E50 $E196
// FUNCTION: 0x00543E70 $E202
// FUNCTION: 0x00543EB0 $E199
// FUNCTION: 0x00544690 $E205

enum {
	ISO_WIDTH = 48,
	ISO_HEIGHT = 24,
};

ShapeSet * SlopeZShapes[4];

void * IsometricTileTypeClass::CellShadowShapes;

int TileSetLookupCount;

int TileSetIDLookup[255];

PaletteClass IsoTilePalette;

DynamicVectorClass<IsometricTileTypeClass::TileInsertType *> IsometricTileTypeClass::TileInsertTypes;

struct ShadowCasterInfo {
	int Frame;
	int Index;
	int XOffset;
	int YOffset;

	ShadowCasterInfo(int frame = 0, int index = 0, int x = 0, int y = 0) : Frame(frame), Index(index), XOffset(x), YOffset(y) {}
};

ShadowCasterInfo ShadowCasterSlopeInfo[10] = {
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(13,6,ISO_WIDTH,ISO_HEIGHT/2),
	ShadowCasterInfo(),
	ShadowCasterInfo(14,1,ISO_WIDTH,ISO_HEIGHT/2),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
};

// FUNCTION: 0x005447C0
IsometricTileTypeClass::IsometricTileTypeClass(IsometricTileType type, int unknown1, unsigned char unknown2, char const * ininame, bool skip_registration) :
	BASECLASS(ininame),
	HeapID(type),
	MarbleMadness(ISOTILE_NONE),
	NonMarbleMadness(ISOTILE_NONE),
	TileSetBaseID(0),
	PreviewTiles(),
	NextTileTypeInSet(NULL),
	ToSnowTheater(-1),
	ToTemperateTheater(-1),
	Anim(ANIM_NONE),
	Offset(Point2D(0,0)),
	AttachesTo(-1),
	ZAdjust(0),
	Unused1((unsigned char)unknown1),
	IsMorphable(false),
	IsShadowCaster(false),
	IsAllowToPlace(true),
	IsRequiredForRMG(false),
	Width(0),
	Height(0),
	Unused2(unknown2),
	NumTileTypesInSet(1),
	IsFileLoaded(false),
	IsAllowBurrowing(true),
	IsAllowTiberium(false),
	UseCount(0)
{
	AbstractTypePtrTracker.Add(this);
	if (!skip_registration) {
		IsometricTileTypes.Add(this);
	}

	GivenName = ininame;

	Filename[0] = '\0';

	IsStealthy = true;
	IsSelectable = false;
	IsLegalTarget = false;
	IsInsignificant = true;
	IsImmune = true;
	IsFootprint = false;
}

// FUNCTION: 0x00544A00
IsometricTileTypeClass::IsometricTileTypeClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: 0x00544A70
IsometricTileTypeClass::~IsometricTileTypeClass(void)
{
	Detach_This_From_All(this, true);

	IsometricTileTypeClass * next = NextTileTypeInSet;

	if (next != NULL) {
		NextTileTypeInSet = NULL;
		delete next;
	}

	IsometricTileTypes.Delete(this);
	AbstractTypePtrTracker.Delete(this);

	for (int i = PreviewTiles.Count() - 1; i >= 0; i--) {
		if (PreviewTiles[i] != NULL) {
			delete [] PreviewTiles[i];
		}
		PreviewTiles.Delete_Index(i);
	}
}

// FUNCTION: 0x00544BE0
LandType IsometricTileTypeClass::Land_Type(int tile) const
{
	IsoTileSet const * tileset = (IsoTileSet const *)Get_Image_Data();

	if (tileset != NULL) {
		IsoTileRecord const * record = tileset->Fetch_Record_Pointer(tile);
		if (record != NULL) {
			static LandType _land[16] = {
				LAND_CLEAR,
				LAND_ICE,
				LAND_ICE,
				LAND_ICE,
				LAND_ICE,
				LAND_TUNNEL,
				LAND_RAILROAD,
				LAND_ROCK,
				LAND_ROCK,
				LAND_WATER,
				LAND_BEACH,
				LAND_ROAD,
				LAND_ROAD,
				LAND_CLEAR,
				LAND_ROUGH,
				LAND_ROCK,
			};

			return(_land[record->TileType]);
		}
	}
	return(LAND_CLEAR);
}

// FUNCTION: 0x00544C20
bool IsometricTileTypeClass::Is_Tile_Index_Valid(int tile, bool load)
{
	if (load) {
		Load_Tile_Image();
	}

	IsoTileSet const * tileset = (IsoTileSet const *)Get_Image_Data();
	if (tileset != NULL && tile < (tileset->Tile_Count()) && tileset->Tiles[tile] != NULL) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00544C80
bool IsometricTileTypeClass::Load_Tile_Image(void)
{
	if (ImageData == NULL && IsFileLoaded) {
		Load_Tile_Data();
	}
	return(ImageData != NULL);
}

// FUNCTION: 0x00544CB0
void const * IsometricTileTypeClass::Get_Image_Data(void) const
{
	void const * data = BASECLASS::Get_Image_Data();
	if (data == NULL && IsFileLoaded) {
		((IsometricTileTypeClass *)this)->Load_Tile_Data();
		data = BASECLASS::Get_Image_Data();
	}
	return(data);
}

// FUNCTION: 0x00544E30
IsometricTileType IsometricTileTypeClass::Fixup_Tile_Type(IsometricTileType current_type)
{
	IsometricTileType new_type = current_type;
	if (current_type == ISOTILE_NONE) {
		return(new_type);
	}

	for (int i = 0; i < TileInsertTypes.Count(); i++) {
		TileInsertType *tileinsert = TileInsertTypes[i];
		if (tileinsert->Count > current_type) {
			break;
		}
		new_type += tileinsert->Offset;
	}
	return(new_type);
}

// FUNCTION: 0x00549B30
void IsometricTileTypeClass::Clear_Z_Shapes(void)
{
	CellShadowShapes = NULL;
	SlopeZShapes[0] = NULL;
	SlopeZShapes[1] = NULL;
	SlopeZShapes[2] = NULL;
	SlopeZShapes[3] = NULL;
}

// FUNCTION: 0x00549B50
Coord const IsometricTileTypeClass::Coord_Fixup(Coord const & coord) const
{
	return(coord);
}

// FUNCTION: 0x00549D70
HRESULT STDMETHODCALLTYPE IsometricTileTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00549D90
HRESULT STDMETHODCALLTYPE IsometricTileTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_IsometricTileTypeClass;
	return(S_OK);
}
