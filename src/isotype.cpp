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

// SYNTHETIC: GAMEMD 0x00543C60 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00543C90 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00543CB0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00543CD0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00543CF0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00543D10 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00543D30 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00543D70 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00543DA0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00543DD0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00543E00 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00543E10 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00543E30 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00543E50 SYMBOL
// _$E196
// SYNTHETIC: GAMEMD 0x00543E70 SYMBOL
// _$E202
// SYNTHETIC: GAMEMD 0x00543EB0 SYMBOL
// _$E199
// SYNTHETIC: GAMEMD 0x00544690 SYMBOL
// _$E205

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

	ShadowCasterInfo(int frame = 0, int index = 0, int x = 0, int y = 0);
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

// FUNCTION: GAMEMD 0x005447C0
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

// FUNCTION: GAMEMD 0x00544A00
IsometricTileTypeClass::IsometricTileTypeClass(NoInitClass const & x) :
	BASECLASS(x)
{
}

// FUNCTION: GAMEMD 0x00544A70
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

// FUNCTION: GAMEMD 0x00544BE0
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

// FUNCTION: GAMEMD 0x00544C20
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

// FUNCTION: GAMEMD 0x00544C80
bool IsometricTileTypeClass::Load_Tile_Image(void)
{
	if (ImageData == NULL && IsFileLoaded) {
		Load_Tile_Data();
	}
	return(ImageData != NULL);
}

// FUNCTION: GAMEMD 0x00544CB0
void const * IsometricTileTypeClass::Get_Image_Data(void) const
{
	void const * data = BASECLASS::Get_Image_Data();
	if (data == NULL && IsFileLoaded) {
		((IsometricTileTypeClass *)this)->Load_Tile_Data();
		data = BASECLASS::Get_Image_Data();
	}
	return(data);
}

// FUNCTION: GAMEMD 0x00544CE0
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

// FUNCTION: GAMEMD 0x00544D30
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

// SYNTHETIC: GAMEMD 0x00544DF0 SYMBOL
// _$E208

// FUNCTION: GAMEMD 0x00544E00
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

// FUNCTION: GAMEMD 0x00544E30
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

// FUNCTION: GAMEMD 0x00544E70
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

// FUNCTION: GAMEMD 0x00544FF0
bool IsometricTileTypeClass::Free_Unused_Drawers(int time_budget_ms, bool force)
{
	return(false);
}

// FUNCTION: GAMEMD 0x00545000
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

// FUNCTION: GAMEMD 0x005471F0
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

// FUNCTION: GAMEMD 0x00546DA0
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

// FUNCTION: GAMEMD 0x00547020
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

// FUNCTION: GAMEMD 0x00547110
void IsometricTileTypeClass::Clear_Use_Counts(void)
{
	IsometricTileTypeClass * t;
	for (int i = 0; i < IsometricTileTypes.Count(); i++) {
		for (t = IsometricTileTypes[i]; t != NULL; t = t->NextTileTypeInSet) {
			t->UseCount = 0;
		}
	}
}

// FUNCTION: GAMEMD 0x00547150
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

// FUNCTION: GAMEMD 0x005471B0
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

// FUNCTION: GAMEMD 0x00549830
bool IsometricTileTypeClass::Get_Tile_Image(int tilenum, unsigned char **buffer, int width, int height)
{
	#define __ "\x20"
	#define XX "\xDB"

	static unsigned char _tilemask[] = {
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __
		__ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __
		__ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __
		__ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __
		XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
		__ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __
		__ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __
		__ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __
		__ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ XX XX XX XX __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
		__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
	};

	#undef __
	#undef XX

	int tilew = 0;
	int tileh = 0;
	memset(*buffer, 0, width * height);
	IsoTileSet *isotile = (IsoTileSet *)Get_Image_Data();

	if (isotile != NULL) {
		tilenum = tilenum % isotile->Tile_Count();
		const IsoTileRecord *record = ((const IsoTileRecord *)isotile->Fetch_Record_Pointer_Unsafe(tilenum));

		if (record != NULL) {
			Get_Tile_Pixel_Dimensions(tilenum, tilew, tileh);

			if (tilew <= width && tileh <= height) {

				unsigned char *buf = *buffer;
				unsigned char *end = &(*buffer)[width * height];

				if (record->IsHasExtraData) {
					buf += width * (record->Y - record->ExtraY);
					if (buf >= end) {
						return(false);
					}
				}

				unsigned char *mask = _tilemask;
				unsigned char *image = (unsigned char *)(record + 1);

				for (int py = 0; py < isotile->Pixel_Height(); py++) {
					for (int px = 0; px < isotile->Pixel_Width(); px++) {

						if (*mask++ != '\x20') {
							*buf = *image;
							image++;
						}

						buf++;
						if (buf >= end) {
							return(false);
						}
					}

					buf += width - isotile->Pixel_Width();
					if (buf >= end) {
						return(false);
					}
				}

				if (record->IsHasExtraData) {

					buf = &(*buffer)[record->ExtraX - record->X];
					if (buf >= end) {
						return(false);
					}

					unsigned char *extraimage = (unsigned char *)record + record->ExtraOffset;

					for (int ey = 0; ey < record->ExtraHeight; ey++) {
						for (int ex = 0; ex < record->ExtraWidth; ex++) {
							if (*extraimage != 0) {
								*buf = *extraimage;
							}
							buf++;
							extraimage++;
							if (buf >= end) {
								return(false);
							}

						}

						buf += width - record->ExtraWidth;
						if (buf >= end) {
							return(false);
						}

					}
				}
				return(true);
			}
		}
		return(false);
	}

	return(true);
}

// FUNCTION: GAMEMD 0x00549A00
void World_To_Screen(int x, int y, int & xx, int & yy)
{
	xx = x * 60 / 2;
	yy = x * 30 / 2;
	xx += y * 60 / -2;
	yy += y * 30 / 2;
}

// FUNCTION: GAMEMD 0x00549A60
int IsometricTileTypeClass::Get_Y_Offset(int tile)
{
	IsoTileSet const * tileset = (IsoTileSet const *)Get_Image_Data();
	if (tileset != NULL && tile < tileset->Tile_Count()) {
		IsoTileRecord const * record = tileset->Tiles[tile];
		if (record != NULL) {
			if ((record->IsHasExtraData) != 0) {
				return(record->ExtraY - record->Y);
			}
		}
	}

	return(0);
}

// FUNCTION: GAMEMD 0x00549AA0
bool IsometricTileTypeClass::Create_And_Place(Cell const & cell, HouseClass * house) const
{
	if (new IsometricTileClass(HeapID, cell)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00549AE0
ObjectClass * IsometricTileTypeClass::Create_One_Of(HouseClass * house) const
{
	((IsometricTileTypeClass *)this)->Load_Tile_Image();
	return(new IsometricTileClass(HeapID, CELL_NONE));
}

// FUNCTION: GAMEMD 0x00549B30
void IsometricTileTypeClass::Clear_Z_Shapes(void)
{
	CellShadowShapes = NULL;
	SlopeZShapes[0] = NULL;
	SlopeZShapes[1] = NULL;
	SlopeZShapes[2] = NULL;
	SlopeZShapes[3] = NULL;
}

// FUNCTION: GAMEMD 0x00549B50
Coord const IsometricTileTypeClass::Coord_Fixup(Coord const & coord) const
{
	return(coord);
}

// FUNCTION: GAMEMD 0x00549B70
void IsometricTileTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(HeapID);
	crc(MarbleMadness);
	crc(NonMarbleMadness);
	crc(TileSetBaseID);
	crc(PreviewTiles.Count());
	if (NextTileTypeInSet != NULL) {
		crc(NextTileTypeInSet->Fetch_ID());
	}
	crc(ToSnowTheater);
	crc(ToTemperateTheater);
	crc(AttachesTo);
	crc(ZAdjust);
	crc(Unused1);
	crc(IsMorphable);
	crc(IsShadowCaster);
	crc(Width);
	crc(Height);
	crc(Unused2);
	crc(NumTileTypesInSet);
}

// FUNCTION: GAMEMD 0x00549C80
HRESULT STDMETHODCALLTYPE IsometricTileTypeClass::Load(IStream * stream)
{
	int i;
	int j;
	for (i = 0; i < TileDrawers.Count(); i++) {
		delete TileDrawers[i];
		TileDrawers[i] = NULL;
	}

	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) IsometricTileTypeClass(NoInitClass());

		Swizzle_Pointer(&NextTileTypeInSet);
		for (j = 0; j < TileDrawers.Count(); j++) {
			TileDrawers[j] = NULL;
		}
		Build_Preview_Tiles();
		return(S_OK);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00549D70
HRESULT STDMETHODCALLTYPE IsometricTileTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00549D90
HRESULT STDMETHODCALLTYPE IsometricTileTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_IsometricTileTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00549DD0
void IsometricTileTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (NextTileTypeInSet == target) {
		NextTileTypeInSet = NULL;
	}

	if (Anim != ANIM_NONE && AnimTypes[Anim] == target) {
		Anim = ANIM_NONE;
	}
}

// FUNCTION: GAMEMD 0x00549E20
unsigned short IsometricTileTypeClass::Preview_Tile_Color(int tile, int level, bool use_high_color)
{
	return(PreviewTiles[tile][(use_high_color ? 1 : 0) + level * 2]);
}

// FUNCTION: GAMEMD 0x00549E90
void IsometricTileTypeClass::Build_Preview_Tiles(void)
{
	int i;
	int j;

	IsoTileSet const * tileset = (IsoTileSet const *)ImageData;

	for (i = 0; i < PreviewTiles.Count(); i++) {
		if (PreviewTiles[i] != NULL) {
			delete [] PreviewTiles[i];
		}
	}

	PreviewTiles.Clear();

	for (i = 0; i < tileset->Tile_Count(); i++) {
		if (tileset->Tiles[i] != NULL) {
			unsigned short * buffer = new unsigned short[24 + 2];
			IsoTileRecord const * record = tileset->Tiles[i];

			PreviewTiles.Add(buffer);

			RGBClass low(record->LowColor.Red, record->LowColor.Green, record->LowColor.Blue);
			RGBClass high(record->HighColor.Red, record->HighColor.Green, record->HighColor.Blue);

			static float const _brightness_factor = 1.4f;

			RGBClass alow((NoInitClass()));
			alow.Set(low, _brightness_factor);

			RGBClass ahigh((NoInitClass()));
			ahigh.Set(high, _brightness_factor);

			unsigned short * ptr = buffer;

			for (j = 0; j <= 12; j++) {
				RGBClass tmp = RGBClass(NoInitClass()).Lerp(low, alow, float((double)j / 12.0f));
				ptr[0] = DSurface::Build_Hicolor_Pixel(tmp.Get_Red(), tmp.Get_Green(), tmp.Get_Blue());

				tmp = RGBClass(NoInitClass()).Lerp(high, ahigh, float((double)j / 12.0f));
				ptr[1] = DSurface::Build_Hicolor_Pixel(tmp.Get_Red(), tmp.Get_Green(), tmp.Get_Blue());

				ptr += 2;
			}
		} else {
			PreviewTiles.Add(NULL);
		}
	}
}

// FUNCTION: GAMEMD 0x0054A120
ShadowCasterInfo::ShadowCasterInfo(int frame, int index, int x, int y) : Frame(frame), Index(index), XOffset(x), YOffset(y)
{
}

// FUNCTION: GAMEMD 0x0054A140
// IsometricTileTypeClass::Fetch_RTTI

// FUNCTION: GAMEMD 0x0054A150
// IsometricTileTypeClass::Fetch_Object_Size

// FUNCTION: GAMEMD 0x0054A160
// IsometricTileTypeClass::Fetch_Heap_ID

// SYNTHETIC: GAMEMD 0x0054A170
// ??_GIsometricTileTypeClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0054A190
// ?Clear@?$DynamicVectorClass@PAG@@UAEXXZ

// TEMPLATE: GAMEMD 0x0054A1C0
// ?ID@?$DynamicVectorClass@PAG@@UAEHPBQAG@Z

// TEMPLATE: GAMEMD 0x0054A1E0
// ?GetItem@?$VectorClass@PAG@@UBEPAGH@Z

// TEMPLATE: GAMEMD 0x0054A1F0
// ?Clear@?$DynamicVectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0054A220
// ?ID@?$DynamicVectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAEHPBQAUTileInsertType@IsometricTileTypeClass@@@Z

// TEMPLATE: GAMEMD 0x0054A240
// ?GetItem@?$VectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UBEPAUTileInsertType@IsometricTileTypeClass@@H@Z

// TEMPLATE: GAMEMD 0x0054A250
// ??8?$VectorClass@PAG@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0054A290
// ?Resize@?$VectorClass@PAG@@UAE_NHPBQAG@Z

// TEMPLATE: GAMEMD 0x0054A340
// ?Clear@?$VectorClass@PAG@@UAEXXZ

// TEMPLATE: GAMEMD 0x0054A370
// ?ID@?$VectorClass@PAG@@UAEHABQAG@Z

// TEMPLATE: GAMEMD 0x0054A3B0
// ?ID@?$VectorClass@PAG@@UAEHPBQAG@Z

// TEMPLATE: GAMEMD 0x0054A3D0
// ?Resize@?$DynamicVectorClass@PAG@@UAE_NHPBQAG@Z

// TEMPLATE: GAMEMD 0x0054A480
// ?ID@?$DynamicVectorClass@PAG@@UAEHABQAG@Z

// TEMPLATE: GAMEMD 0x0054A4B0
// ??8?$VectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0054A4F0
// ?Resize@?$VectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAE_NHPBQAUTileInsertType@IsometricTileTypeClass@@@Z

// TEMPLATE: GAMEMD 0x0054A5A0
// ?Clear@?$VectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0054A5D0
// ?ID@?$VectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAEHABQAUTileInsertType@IsometricTileTypeClass@@@Z

// TEMPLATE: GAMEMD 0x0054A610
// ?ID@?$VectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAEHPBQAUTileInsertType@IsometricTileTypeClass@@@Z

// TEMPLATE: GAMEMD 0x0054A630
// ?Resize@?$DynamicVectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAE_NHPBQAUTileInsertType@IsometricTileTypeClass@@@Z

// TEMPLATE: GAMEMD 0x0054A6E0
// ?ID@?$DynamicVectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAEHABQAUTileInsertType@IsometricTileTypeClass@@@Z

// SYNTHETIC: GAMEMD 0x0054A710
// ??_G?$VectorClass@PAG@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0054A760
// ??_G?$DynamicVectorClass@PAG@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0054A7B0
// ??_G?$VectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0054A800
// ??_G?$DynamicVectorClass@PAUTileInsertType@IsometricTileTypeClass@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0054A850
// ??0?$VectorClass@PAG@@QAE@HPBQAG@Z

struct IsoZeroQuad {
	int A;
	int B;
	int C;
	int D;

	IsoZeroQuad() : A(0), B(0), C(0), D(0) {}
};

IsoZeroQuad IsoZeroQuadInstance;

// SYNTHETIC: GAMEMD 0x0054A8A0 SYMBOL
// _$E210
