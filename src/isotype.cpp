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
#include "lightcalc.h"
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

// FUNCTION: 0x00544CE0
IsometricTileType IsometricTileTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int index = ISOTILE_FIRST; index < IsometricTileTypes.Count(); index++) {
			if (stricmp(IsometricTileTypes[index]->IniName, name) == 0) {
				return(IsometricTileType(index));
			}
		}
	}
	return(ISOTILE_NONE);
}

// FUNCTION: 0x00544D30
Cell const * IsometricTileTypeClass::Occupy_List(bool placement) const
{
	static Cell _occupy[13*8+5+11];
	Cell	* ptr;

	IsoTileSet const * tileset = (IsoTileSet const *)Get_Image_Data();
	unsigned int const * map = (unsigned int const *)tileset->Tiles;

	ptr = &_occupy[0];
	for (int index = 0; index < Width * Height; index++) {
		if (*map++ != NULL) {
			*ptr++ = Cell(index % Width, index / Width);
		}
	}
	*ptr = REFRESH_EOL;

	return((Cell const *)&_occupy[0]);
}

// FUNCTION: 0x00544DF0 $E208

// FUNCTION: 0x00544E00
IsometricTileTypeClass * IsometricTileTypeClass::Next_Tile_From_Set(int index)
{
	int num = NumTileTypesInSet;
	int i = index;
	if (i > num - 1) {
		i = i % num;
	}

	if (i == 0) {
		return(this);
	}

	IsometricTileTypeClass * next = this;
	do {
		next = next->NextTileTypeInSet;
	} while (--i != 0);
	return(next);
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

// FUNCTION: 0x00544E70
LightConvertClass * IsometricTileTypeClass::Find_Or_Make_Drawer(int r, int g, int b)
{
	LightConvertClass * drawer;

	if (VisibleSurface == NULL) {
		return(NULL);
	}

	if (r == NORMAL_LIGHT && g == NORMAL_LIGHT && b == NORMAL_LIGHT && TileDrawers.Count() != 0) {
		return(TileDrawers[0]);
	}

	Clamp_Tile_RGB(r,g,b);

	for (int i = 1; i < TileDrawers.Count(); i++) {
		drawer = TileDrawers[i];
		if (drawer->NormalRedTint == r && drawer->NormalGreenTint == g && drawer->NormalBlueTint == b) {
			return(drawer);
		}
	}

	int levels = 53;
	if (r + g + b < 2000) {
		levels = 27;
	}
	drawer = new LightConvertClass(IsoTilePalette, GamePalette, *VisibleSurface, r, g, b, TileDrawers.Count() != 0, NULL, levels);
	TileDrawers.Add(drawer);

	return(drawer);
}

// FUNCTION: 0x00544FF0
bool IsometricTileTypeClass::Free_Unused_Drawers(int time_budget_ms, bool force)
{
	return(false);
}

// FUNCTION: 0x00545000
void IsometricTileTypeClass::Init_Drawers(void)
{
	for (int i = 0; i < TileDrawers.Count(); i++) {
		LightConvertClass *drawer = TileDrawers[i];
		delete drawer;
		TileDrawers.Delete_Index(i);
		i--;
	}

	TileDrawers.Add(new LightConvertClass(IsoTilePalette, GamePalette, *VisibleSurface, 1000, 1000, 1000, false, NULL, 53));

	BlubCell.Drawer = NULL;
	BlubCell.Init_Drawer();

	Map.Reset_Iterator();
	CellClass * cptr = Map.Iterate();
	while (cptr != NULL) {
		cptr->Drawer = NULL;
		cptr = Map.Iterate();
	}
}

// FUNCTION: 0x005471F0
bool IsometricTileTypeClass::Is_Randomized(int tile) const
{
	IsoTileSet const * tileset = (IsoTileSet const *)Get_Image_Data();

	if (tileset != NULL) {
		IsoTileRecord const * record = tileset->Fetch_Record_Pointer(tile);
		if (record != NULL) {
			return(record->IsRandomized);
		}
	}
	return(0);
}

// FUNCTION: 0x00546DA0
void IsometricTileTypeClass::Load_Tiles(bool skipiteration, bool isrand)
{
	CDTimerClass<SystemTimerClass> timer = 0;
	IsometricTileTypeClass *ptr;

	if (!skipiteration) {
		Map.Reset_Iterator();
		CellClass *cptr = Map.Iterate();
		while (cptr != NULL) {
			IsometricTileType type = cptr->ITType;
			int index = 0;
			int num;

			if (type != ISOTILE_NONE && type < (IsometricTileType)IsometricTileTypes.Count()) {
				ptr = IsometricTileTypes[type];

				if (ptr->NumTileTypesInSet > 1) {
					num = ptr->NumTileTypesInSet;
					if (ptr->Is_Randomized(cptr->SubTile)) {
						index = cptr->IsBridgeDamaged;
					} else {
						num = ptr->NumTileTypesInSet;
						index = cptr->Clear_Icon(cptr->ITType, num);
					}
				}
			} else {
				ptr = IsometricTileTypes[TILE_CLEAR];
				num = ptr->NumTileTypesInSet;
				index = cptr->Clear_Icon(TILE_CLEAR, num);
			}
			ptr = ptr->Next_Tile_From_Set(index);
			ptr->UseCount++;
			cptr = Map.Iterate();
		}
	}

	int tile_count = 0;
	int bytes = 0;
	timer = 1;

	for (int i = 0; i < IsometricTileTypes.Count(); i++) {
		if (!timer) {
			Call_Back();
			timer = 4;
		}


		ptr = IsometricTileTypes[i];
		while (ptr != NULL) {
			if (ptr->IsFileLoaded && strlen(ptr->Filename)) {
				if (!skipiteration && ptr->UseCount == 0 && (!isrand || !ptr->IsRequiredForRMG)) {
					if (ptr->ImageData != NULL) {
						delete [] (unsigned char *)ptr->ImageData;
						ptr->ImageData = NULL;
						ptr->ImageAllocated = false;
					}
				} else {
					if (ptr->ImageData == NULL) {
						bytes += ptr->Load_Tile_Data();
						tile_count++;
					}
				}
			}
			ptr = ptr->NextTileTypeInSet;
		}
	}

	DebugString("Loaded %d isometric tiles consuming %d Kb\n", tile_count, bytes / 1024);
}

// FUNCTION: 0x00547020
int IsometricTileTypeClass::Load_Tile_Data(void)
{
	CCFileClass file(Filename);
	int size = file.Size();
	if (ImageData != NULL) {
		delete [] (unsigned char *)ImageData;
	}
	unsigned char * data = new unsigned char[size];
	ImageData = data;
	file.Read(data, size);

	ImageAllocated = true;
	IsoTileSet * tileset = (IsoTileSet *)ImageData;

	Width = *((unsigned char *)ImageData);
	Height = *((unsigned char *)ImageData + 4);

	/// Fixup pointers to point to actual memory
	for (int i = 0; i < (Width * Height); i++) {

		if (tileset->Tiles[i] != NULL) {
			unsigned char * ptr = (unsigned char *)ImageData;
			// Only fix up pointers that have not been converted already. The file
			// has just been read fresh, so none of them ever have been.
			if ((void *)tileset->Tiles[i] < ptr) {
				tileset->Tiles[i] = (IsoTileRecord *)(ptr + (unsigned int)tileset->Tiles[i]);
			}
		}

	}

	Build_Preview_Tiles();

	return(size);
}

// FUNCTION: 0x00547110
void IsometricTileTypeClass::Clear_Use_Counts(void)
{
	IsometricTileTypeClass * t;
	for (int i = 0; i < IsometricTileTypes.Count(); i++) {
		for (t = IsometricTileTypes[i]; t != NULL; t = t->NextTileTypeInSet) {
			t->UseCount = 0;
		}
	}
}

// FUNCTION: 0x00547150
bool IsometricTileTypeClass::Get_Tile_Pixel_Dimensions(int tile, int & width, int & height)
{
	IsoTileSet const * tileset = (IsoTileSet const *)Get_Image_Data();
	if (tileset != NULL) {
		IsoTileRecord const * record = tileset->Fetch_Record_Pointer(tile);
		width = tileset->Pixel_Width();
		height = tileset->Pixel_Height();
		if (record != NULL) {
			if ((record->IsHasExtraData) != 0) {
				height += (record->Y - record->ExtraY);
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005471B0
int IsometricTileTypeClass::Ramp_Type(int tile) const
{
	IsoTileSet const * tileset = (IsoTileSet const *)Get_Image_Data();

	if (tileset != NULL) {
		IsoTileRecord const * record = tileset->Fetch_Record_Pointer(tile);
		if (record != NULL) {
			return(record->RampType);
		}
	}
	return(0);
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
