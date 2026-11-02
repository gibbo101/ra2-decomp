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

struct FileSystem {
	static void const * LoadFile(char const * name, bool as_shape);
};

// SYNTHETIC: GAMEMD 0x00543C40 SYMBOL
// _$E41
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
// _$E197
// SYNTHETIC: GAMEMD 0x00543E70 SYMBOL
// _$E203
// SYNTHETIC: GAMEMD 0x00543EB0 SYMBOL
// _$E200
// SYNTHETIC: GAMEMD 0x00543F10 SYMBOL
// _$E206
// SYNTHETIC: GAMEMD 0x00544690 SYMBOL
// _$E209

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

ShadowCasterInfo ShadowCasterCliffInfo[40] = {
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(0,0,30,30),
	ShadowCasterInfo(0,0,30,30),
	ShadowCasterInfo(0,0,30,30),
	ShadowCasterInfo(1,0,30,30),
	ShadowCasterInfo(1,0,30,30),
	ShadowCasterInfo(2,1,60,15),
	ShadowCasterInfo(3,1,60,15),
	ShadowCasterInfo(4,1,60,15),
	ShadowCasterInfo(5,0,90,30),
	ShadowCasterInfo(6,0,60,15),
	ShadowCasterInfo(7,1,30,0),
	ShadowCasterInfo(8,0,60,15),
	ShadowCasterInfo(9,0,60,15),
	ShadowCasterInfo(10,1,0,-15),
	ShadowCasterInfo(11,1,0,-15),
	ShadowCasterInfo(12,0,60,15),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
	ShadowCasterInfo(),
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
// _$E212

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

// FUNCTION: GAMEMD 0x00545150
void IsometricTileTypeClass::Read_Control_File(TheaterType theater, bool from_ccfile)
{
	int i;
	int j;

	CCINIClass ini;
	IsometricTileTypeClass * tile = NULL;
	CDTimerClass<SystemTimerClass> callback_timer;
	int shadow_casters = 0;

	memset(ShadowCasterTiles, 0, sizeof(ShadowCasterTiles));

	CCFileClass cshadow("C_SHADOW.SHP");
	if (CellShadowShapes) {
		delete [] ((char *)CellShadowShapes);
		CellShadowShapes = NULL;
	}
	CellShadowShapes = (ShapeSet *)new char[cshadow.Size()];
	cshadow.Read(CellShadowShapes, cshadow.Size());

	if (TileInsertTypes.Count() != 0) {
		for (i = TileInsertTypes.Count() - 1; i >= 0; --i) {
			delete TileInsertTypes[i];
			TileInsertTypes.Delete_Index(i);
		}
	}

	for (i = 0; i < ARRAY_SIZE(SlopeZShapes); i++) {
		if (SlopeZShapes[i]) {
			delete [] ((char *)SlopeZShapes[i]);
			SlopeZShapes[i] = NULL;
		}
	}

	char slopzname[32];
	sprintf(slopzname, "SLOP01Z.%s", Theaters[theater].Suffix);
	CCFileClass slopz(slopzname);

	SlopeZShapes[0] = (ShapeSet *)new char[slopz.Size()];
	slopz.Read(SlopeZShapes[0], slopz.Size());

	slopzname[5] = '2';
	slopz.Set_Name(slopzname);
	SlopeZShapes[1] = (ShapeSet *)new char[slopz.Size()];
	slopz.Read(SlopeZShapes[1], slopz.Size());

	slopzname[5] = '3';
	slopz.Set_Name(slopzname);
	SlopeZShapes[2] = (ShapeSet *)new char[slopz.Size()];
	slopz.Read(SlopeZShapes[2], slopz.Size());

	slopzname[5] = '4';
	slopz.Set_Name(slopzname);
	SlopeZShapes[3] = (ShapeSet *)new char[slopz.Size()];
	slopz.Read(SlopeZShapes[3], slopz.Size());

	while (IsometricTileTypes.Count() != 0) {
		delete IsometricTileTypes[0];
	}

	char palname[32];
	sprintf(palname, "ISO%s.PAL", Theaters[theater].Suffix);
	CCFileClass palette(palname);
	if (palette.Is_Available()) {
		palette.Read(&IsoTilePalette, sizeof(IsoTilePalette));
		unsigned char * color = (unsigned char *)IsoTilePalette;
		for (i = 0; i < sizeof(PaletteClass); i++) {
			*color <<= 2;
			color++;
		}
	}
	IsometricTileTypeClass::Init_Drawers();

	char ininame[20];
	sprintf(ininame, "%sMD.INI", Theaters[theater].Root);
	CCFileClass inifile(ininame);
	ini.Load(inifile, false, false);

	int setid = 0;
	int heapid = 0;
	int tile_count = 0;

	IsometricTileType _RampStart = (IsometricTileType)ini.Get_Int("General", "RampBase", ISOTILE_INVALID);
	IsometricTileType _RampSmooth = (IsometricTileType)ini.Get_Int("General", "RampSmooth", ISOTILE_INVALID);
	IsometricTileType _MMRampBase = (IsometricTileType)ini.Get_Int("General", "MMRampBase", ISOTILE_INVALID);
	IsometricTileType _ClearTile = (IsometricTileType)ini.Get_Int("General", "ClearTile", ISOTILE_INVALID);
	IsometricTileType _RoughTile = (IsometricTileType)ini.Get_Int("General", "RoughTile", ISOTILE_INVALID);
	IsometricTileType _SandTile = (IsometricTileType)ini.Get_Int("General", "SandTile", ISOTILE_INVALID);
	IsometricTileType _GreenTile = (IsometricTileType)ini.Get_Int("General", "GreenTile", ISOTILE_INVALID);
	IsometricTileType _PaveTile = (IsometricTileType)ini.Get_Int("General", "PaveTile", ISOTILE_INVALID);
	IsometricTileType _MiscPaveTile = (IsometricTileType)ini.Get_Int("General", "MiscPaveTile", ISOTILE_INVALID);
	IsometricTileType _ClearToRoughLat = (IsometricTileType)ini.Get_Int("General", "ClearToRoughLat", ISOTILE_INVALID);
	IsometricTileType _ClearToSandLat = (IsometricTileType)ini.Get_Int("General", "ClearToSandLat", ISOTILE_INVALID);
	IsometricTileType _ClearToGreenLat = (IsometricTileType)ini.Get_Int("General", "ClearToGreenLat", ISOTILE_INVALID);
	IsometricTileType _ClearToPaveLat = (IsometricTileType)ini.Get_Int("General", "ClearToPaveLat", ISOTILE_INVALID);
	IsometricTileType _HeightBase = (IsometricTileType)ini.Get_Int("General", "HeightBase", ISOTILE_INVALID);
	IsometricTileType _BlackTile = (IsometricTileType)ini.Get_Int("General", "BlackTile", ISOTILE_INVALID);
	IsometricTileType _BridgeSet = (IsometricTileType)ini.Get_Int("General", "BridgeSet", ISOTILE_INVALID);
	IsometricTileType _TrainBridgeSet = (IsometricTileType)ini.Get_Int("General", "WoodBridgeSet", ISOTILE_INVALID);
	IsometricTileType _CliffSet = (IsometricTileType)ini.Get_Int("General", "CliffSet", ISOTILE_INVALID);
	IsometricTileType _ShorePieces = (IsometricTileType)ini.Get_Int("General", "ShorePieces", ISOTILE_INVALID);
	IsometricTileType _WaterSet = (IsometricTileType)ini.Get_Int("General", "WaterSet", ISOTILE_INVALID);
	IsometricTileType _SlopeSetPieces = (IsometricTileType)ini.Get_Int("General", "SlopeSetPieces", ISOTILE_INVALID);
	IsometricTileType _SlopeSetPieces2 = (IsometricTileType)ini.Get_Int("General", "SlopeSetPieces2", ISOTILE_INVALID);
	IsometricTileType _MonorailSlopes = (IsometricTileType)ini.Get_Int("General", "MonorailSlopes", ISOTILE_INVALID);
	IsometricTileType _Tunnels = (IsometricTileType)ini.Get_Int("General", "Tunnels", ISOTILE_INVALID);
	IsometricTileType _TrackTunnels = (IsometricTileType)ini.Get_Int("General", "TrackTunnels", ISOTILE_INVALID);
	IsometricTileType _DirtTunnels = (IsometricTileType)ini.Get_Int("General", "DirtTunnels", ISOTILE_INVALID);
	IsometricTileType _DirtTrackTunnels = (IsometricTileType)ini.Get_Int("General", "DirtTrackTunnels", ISOTILE_INVALID);
	IsometricTileType _WaterfallEast = (IsometricTileType)ini.Get_Int("General", "WaterfallEast", ISOTILE_INVALID);
	IsometricTileType _WaterfallWest = (IsometricTileType)ini.Get_Int("General", "WaterfallWest", ISOTILE_INVALID);
	IsometricTileType _WaterfallNorth = (IsometricTileType)ini.Get_Int("General", "WaterfallNorth", ISOTILE_INVALID);
	IsometricTileType _WaterfallSouth = (IsometricTileType)ini.Get_Int("General", "WaterfallSouth", ISOTILE_INVALID);
	IsometricTileType _CliffRamps = (IsometricTileType)ini.Get_Int("General", "CliffRamps", ISOTILE_INVALID);
	IsometricTileType _PavedRoads = (IsometricTileType)ini.Get_Int("General", "PavedRoads", ISOTILE_INVALID);
	IsometricTileType _PavedRoadEnds = (IsometricTileType)ini.Get_Int("General", "PavedRoadEnds", ISOTILE_INVALID);
	IsometricTileType _Medians = (IsometricTileType)ini.Get_Int("General", "Medians", ISOTILE_INVALID);
	IsometricTileType _RoughGround = (IsometricTileType)ini.Get_Int("General", "RoughGround", ISOTILE_INVALID);
	IsometricTileType _DirtRoadJunction = (IsometricTileType)ini.Get_Int("General", "DirtRoadJunction", ISOTILE_INVALID);
	IsometricTileType _DirtRoadCurve = (IsometricTileType)ini.Get_Int("General", "DirtRoadCurve", ISOTILE_INVALID);
	IsometricTileType _DirtRoadStraight = (IsometricTileType)ini.Get_Int("General", "DirtRoadStraight", ISOTILE_INVALID);
	IsometricTileType _DestroyableCliffs = (IsometricTileType)ini.Get_Int("General", "DestroyableCliffs", ISOTILE_INVALID_CLIFF);
	IsometricTileType _WaterCaves = (IsometricTileType)ini.Get_Int("General", "WaterCaves", ISOTILE_INVALID);
	IsometricTileType _WaterCliffs = (IsometricTileType)ini.Get_Int("General", "WaterCliffs", ISOTILE_INVALID);
	IsometricTileType _PavedRoadSlopes = (IsometricTileType)ini.Get_Int("General", "PavedRoadSlopes", ISOTILE_INVALID);
	IsometricTileType _DirtRoadSlopes = (IsometricTileType)ini.Get_Int("General", "DirtRoadSlopes", ISOTILE_INVALID);
	IsometricTileType _Rocks = (IsometricTileType)ini.Get_Int("General", "Rocks", ISOTILE_INVALID);
	IsometricTileType _WaterBridge = (IsometricTileType)ini.Get_Int("General", "WaterBridge", ISOTILE_INVALID);

	RampStart = ISOTILE_INVALID;
	RampSmooth = ISOTILE_INVALID;
	MMRampBase = ISOTILE_INVALID;
	ClearTile = ISOTILE_INVALID;
	RoughTile = ISOTILE_INVALID;
	SandTile = ISOTILE_INVALID;
	GreenTile = ISOTILE_INVALID;
	PaveTile = ISOTILE_INVALID;
	MiscPaveTile = ISOTILE_INVALID;
	ClearToRoughLat = ISOTILE_INVALID;
	ClearToSandLat = ISOTILE_INVALID;
	ClearToGreenLat = ISOTILE_INVALID;
	ClearToPaveLat = ISOTILE_INVALID;
	HeightBase = ISOTILE_INVALID;
	BlackTile = ISOTILE_INVALID;
	BridgeSet = ISOTILE_INVALID;
	TrainBridgeSet = ISOTILE_INVALID;
	CliffSet = ISOTILE_INVALID;
	ShorePieces = ISOTILE_INVALID;
	WaterSet = ISOTILE_INVALID;
	SlopeSetPieces = ISOTILE_INVALID;
	SlopeSetPieces2 = ISOTILE_INVALID;
	MonorailSlopes = ISOTILE_INVALID;
	Tunnels = ISOTILE_INVALID;
	TrackTunnels = ISOTILE_INVALID;
	DirtTunnels = ISOTILE_INVALID;
	DirtTrackTunnels = ISOTILE_INVALID;
	WaterfallEast = ISOTILE_INVALID;
	WaterfallWest = ISOTILE_INVALID;
	WaterfallNorth = ISOTILE_INVALID;
	WaterfallSouth = ISOTILE_INVALID;
	CliffRamps = ISOTILE_INVALID;
	PavedRoads = ISOTILE_INVALID;
	PavedRoadEnds = ISOTILE_INVALID;
	Medians = ISOTILE_INVALID;
	RoughGround = ISOTILE_INVALID;
	DirtRoadJunction = ISOTILE_INVALID;
	DirtRoadCurve = ISOTILE_INVALID;
	DirtRoadStraight = ISOTILE_INVALID;
	DestroyableCliffs = ISOTILE_INVALID_CLIFF;
	WaterCaves = ISOTILE_INVALID;
	WaterCliffs = ISOTILE_INVALID;
	PavedRoadSlopes = ISOTILE_INVALID;
	DirtRoadSlopes = ISOTILE_INVALID;
	Rocks = ISOTILE_INVALID;
	WaterBridge = ISOTILE_INVALID;

	BridgeTopLeft1 = (IsometricTileType)ini.Get_Int("General", "BridgeTopLeft1", ISOTILE_INVALID);
	BridgeTopLeft2 = (IsometricTileType)ini.Get_Int("General", "BridgeTopLeft2", ISOTILE_INVALID);
	BridgeBottomRight1 = (IsometricTileType)ini.Get_Int("General", "BridgeBottomRight1", ISOTILE_INVALID);
	BridgeBottomRight2 = (IsometricTileType)ini.Get_Int("General", "BridgeBottomRight2", ISOTILE_INVALID);
	BridgeTopRight1 = (IsometricTileType)ini.Get_Int("General", "BridgeTopRight1", ISOTILE_INVALID);
	BridgeTopRight2 = (IsometricTileType)ini.Get_Int("General", "BridgeTopRight2", ISOTILE_INVALID);
	BridgeBottomLeft1 = (IsometricTileType)ini.Get_Int("General", "BridgeBottomLeft1", ISOTILE_INVALID);
	BridgeBottomLeft2 = (IsometricTileType)ini.Get_Int("General", "BridgeBottomLeft2", ISOTILE_INVALID);
	BridgeMiddle1 = (IsometricTileType)ini.Get_Int("General", "BridgeMiddle1", ISOTILE_INVALID);
	BridgeMiddle2 = (IsometricTileType)ini.Get_Int("General", "BridgeMiddle2", ISOTILE_INVALID);

	TileSetLookupCount = 0;
	callback_timer = 4;

	while (true) {
		if (callback_timer == 0) {
			Call_Back();
			callback_timer = 4;
		}

		TileSetIDLookup[TileSetLookupCount++] = (IsometricTileType)heapid;

		if (setid == _RampStart) RampStart = (IsometricTileType)heapid;
		if (setid == _RampSmooth) RampSmooth = (IsometricTileType)heapid;
		if (setid == _MMRampBase) MMRampBase = (IsometricTileType)heapid;
		if (setid == _ClearTile) ClearTile = (IsometricTileType)heapid;
		if (setid == _RoughTile) RoughTile = (IsometricTileType)heapid;
		if (setid == _SandTile) SandTile = (IsometricTileType)heapid;
		if (setid == _GreenTile) GreenTile = (IsometricTileType)heapid;
		if (setid == _PaveTile) PaveTile = (IsometricTileType)heapid;
		if (setid == _MiscPaveTile) MiscPaveTile = (IsometricTileType)heapid;
		if (setid == _ClearToRoughLat) ClearToRoughLat = (IsometricTileType)heapid;
		if (setid == _ClearToSandLat) ClearToSandLat = (IsometricTileType)heapid;
		if (setid == _ClearToGreenLat) ClearToGreenLat = (IsometricTileType)heapid;
		if (setid == _ClearToPaveLat) ClearToPaveLat = (IsometricTileType)heapid;
		if (setid == _HeightBase) HeightBase = (IsometricTileType)heapid;
		if (setid == _BlackTile) BlackTile = (IsometricTileType)heapid;
		if (setid == _BridgeSet) BridgeSet = (IsometricTileType)heapid;
		if (setid == _TrainBridgeSet) TrainBridgeSet = (IsometricTileType)heapid;
		if (setid == _CliffSet) CliffSet = (IsometricTileType)heapid;
		if (setid == _ShorePieces) ShorePieces = (IsometricTileType)heapid;
		if (setid == _WaterSet) WaterSet = (IsometricTileType)heapid;
		if (setid == _SlopeSetPieces) SlopeSetPieces = (IsometricTileType)heapid;
		if (setid == _SlopeSetPieces2) SlopeSetPieces2 = (IsometricTileType)heapid;
		if (setid == _MonorailSlopes) MonorailSlopes = (IsometricTileType)heapid;
		if (setid == _DirtTunnels) DirtTunnels = (IsometricTileType)heapid;
		if (setid == _DirtTrackTunnels) DirtTrackTunnels = (IsometricTileType)heapid;
		if (setid == _Tunnels) Tunnels = (IsometricTileType)heapid;
		if (setid == _TrackTunnels) TrackTunnels = (IsometricTileType)heapid;
		if (setid == _WaterfallEast) WaterfallEast = (IsometricTileType)heapid;
		if (setid == _WaterfallWest) WaterfallWest = (IsometricTileType)heapid;
		if (setid == _WaterfallNorth) WaterfallNorth = (IsometricTileType)heapid;
		if (setid == _WaterfallSouth) WaterfallSouth = (IsometricTileType)heapid;
		if (setid == _CliffRamps) CliffRamps = (IsometricTileType)heapid;
		if (setid == _PavedRoads) PavedRoads = (IsometricTileType)heapid;
		if (setid == _PavedRoadEnds) PavedRoadEnds = (IsometricTileType)heapid;
		if (setid == _Medians) Medians = (IsometricTileType)heapid;
		if (setid == _RoughGround) RoughGround = (IsometricTileType)heapid;
		if (setid == _DirtRoadJunction) DirtRoadJunction = (IsometricTileType)heapid;
		if (setid == _DirtRoadCurve) DirtRoadCurve = (IsometricTileType)heapid;
		if (setid == _DirtRoadStraight) DirtRoadStraight = (IsometricTileType)heapid;
		if (setid == _DestroyableCliffs) DestroyableCliffs = (IsometricTileType)heapid;
		if (setid == _WaterCliffs) WaterCliffs = (IsometricTileType)heapid;
		if (setid == _WaterCaves) WaterCaves = (IsometricTileType)heapid;
		if (setid == _PavedRoadSlopes) PavedRoadSlopes = (IsometricTileType)heapid;
		if (setid == _DirtRoadSlopes) DirtRoadSlopes = (IsometricTileType)heapid;
		if (setid == _Rocks) Rocks = (IsometricTileType)heapid;
		if (setid == _WaterBridge) WaterBridge = (IsometricTileType)heapid;

		char section[64];
		sprintf(section, "TileSet%04d", setid);
		ini.Reset();
		int tiles_in_set = ini.Get_Int(section, "TilesInSet", -1);
		if (tiles_in_set == -1) {
			break;
		}

		int last_tiles_in_set = ini.Get_Int(section, "LastTilesInSet", -1);
		if (last_tiles_in_set == -1 || last_tiles_in_set == tiles_in_set) {
			tile_count += tiles_in_set;
		} else {
			TileInsertType * insert = new TileInsertType;
			insert->Count = tile_count + last_tiles_in_set;
			insert->Offset = tiles_in_set - last_tiles_in_set;
			TileInsertTypes.Add(insert);
			tile_count += last_tiles_in_set;
		}

		char set_name[64];
		char file_name[64];
		ini.Get_String(section, "SetName", "No Name", set_name, sizeof(set_name));
		ini.Get_String(section, "FileName", "TILE", file_name, sizeof(file_name));
		IsometricTileType _MarbleMadness = (IsometricTileType)ini.Get_Int(section, "MarbleMadness", ISOTILE_NONE);
		IsometricTileType _NonMarbleMadness = (IsometricTileType)ini.Get_Int(section, "NonMarbleMadness", ISOTILE_NONE);
		bool _Morphable = ini.Get_Bool(section, "Morphable", false);
		bool _AllowToPlace = ini.Get_Bool(section, "AllowToPlace", true);
		bool _AllowBurrowing = ini.Get_Bool(section, "AllowBurrowing", true);
		bool _AllowTiberium = ini.Get_Bool(section, "AllowTiberium", false);
		bool _RequiredForRMG = ini.Get_Bool(section, "RequiredForRMG", false);
		IsometricTileType _ToSnowTheater = (IsometricTileType)ini.Get_Int(section, "ToSnowTheater", ISOTILE_INVALID);
		IsometricTileType _ToTemperateTheater = (IsometricTileType)ini.Get_Int(section, "ToTemperateTheater", ISOTILE_INVALID);

		bool is_shadow_caster = ini.Get_Bool(section, "ShadowCaster", false);
		if (is_shadow_caster) {
			ShadowCasterTiles[shadow_casters++] = (IsometricTileType)heapid;
		}
		int shadow_tiles = 0;
		if (is_shadow_caster) {
			shadow_tiles = ini.Get_Int(section, "ShadowTiles", shadow_tiles);
		}

		bool has_section = ini.Is_Present(set_name);

		for (i = 0; i < tiles_in_set; i++) {
			int tile_count = 0;

			IsometricTileTypeClass * head_tile = NULL;
			IsometricTileTypeClass * tail_tile = NULL;

			IsoTileSet * mixfile_set = NULL;
			bool found_image = false;

			do {
				char letter_suffix[2];
				char suffix[10];
				char tile_name[128];
				char given_name[128];

				if (tile_count != 0) {
					letter_suffix[0] = tile_count + 'a' - 1;
					letter_suffix[1] = 0;
				} else {
					letter_suffix[0] = 0;
				}

				sprintf(suffix, "%02d", i + 1);
				strcat(suffix, letter_suffix);
				strcpy(tile_name, file_name);
				strcat(tile_name, suffix);
				sprintf(given_name, "%.28s %02d", set_name, i + 1);

				int uninitialized_value = -65;

				if (tile_count == 0) {
					tile = new IsometricTileTypeClass((IsometricTileType)heapid++, uninitialized_value, 0, tile_name, false);

					tile->GivenName = given_name;
					tile->MarbleMadness = _MarbleMadness;
					tile->NonMarbleMadness = _NonMarbleMadness;
					tile->IsMorphable = _Morphable;
					tile->IsAllowToPlace = _AllowToPlace;
					tile->IsAllowBurrowing = _AllowBurrowing;
					tile->IsAllowTiberium = _AllowTiberium;
					tile->IsRequiredForRMG = _RequiredForRMG;
					tile->ImageData = NULL;
					tile->ImageAllocated = false;
					tile->ToSnowTheater = _ToSnowTheater;
					tile->ToTemperateTheater = _ToTemperateTheater;
					if (is_shadow_caster && shadow_tiles) {
						tile->IsShadowCaster = is_shadow_caster;
					}
					if (from_ccfile) {
						tile->IsFileLoaded = true;
					}
					tile->TileSetBaseID = TileSetIDLookup[TileSetLookupCount - 1];
					tile->PreviewTiles.Clear();
					if (has_section) {

						char entry[32];
						sprintf(entry, "Tile%02dAnim", i + 1);

						char anim_name[128];
						if (ini.Get_String(set_name, entry, (const char *)"", anim_name, sizeof(anim_name))) {

							AnimTypeClass * animtype = AnimTypeClass::Find_Or_Make(anim_name);
							if (animtype != NULL) {
								tile->Anim = (AnimType)animtype->Fetch_Heap_ID();
								sprintf(entry, "Tile%02dXOffset", i + 1);
								tile->Offset.X = ini.Get_Int(set_name, entry, tile->Offset.X);
								sprintf(entry, "Tile%02dYOffset", i + 1);
								tile->Offset.Y = ini.Get_Int(set_name, entry, tile->Offset.Y);
								sprintf(entry, "Tile%02dAttachesTo", i + 1);
								tile->AttachesTo = ini.Get_Int(set_name, entry, tile->AttachesTo);
								sprintf(entry, "Tile%02dZAdjust", i + 1);
								tile->ZAdjust = ini.Get_Int(set_name, entry, tile->ZAdjust);
							}
						}
					}
					head_tile = tile;
					tail_tile = tile;
				}

				char file_path[512];
				_makepath(file_path, NULL, NULL, tile_name, Theaters[theater].Suffix);

				found_image = false;
				mixfile_set = NULL;

				if (from_ccfile) {
					CCFileClass file(file_path);
					found_image = file.Is_Available();
					if (found_image && tile_count == 0) {
						strncpy(tile->Filename, file_path, sizeof(tile->Filename)-2);
					}
				} else {
					mixfile_set = (IsoTileSet *)FileSystem::LoadFile(file_path, false);
					if (mixfile_set) {
						found_image = true;
					}
				}
				if (!found_image && _NonMarbleMadness) {
					if (theater != THEATER_NONE) {
						_makepath(file_path, NULL, NULL, tile_name, Theaters[theater].MMExtension);
					} else {
						_makepath(file_path, NULL, NULL, tile_name, ".MMT");
					}
					if (from_ccfile) {
						CCFileClass file(file_path);
						found_image = file.Is_Available();
						if (found_image && tile_count == 0) {
							strncpy(tile->Filename, file_path, sizeof(tile->Filename)-2);
						}
					} else {
						mixfile_set = (IsoTileSet *)FileSystem::LoadFile(file_path, false);
						if (mixfile_set) {
							found_image = true;
						}
					}
					if (!found_image) {
						_makepath(file_path, NULL, NULL, tile_name, ".TEM");
						if (from_ccfile) {
							CCFileClass file(file_path);
							found_image = file.Is_Available();
							if (found_image && tile_count == 0) {
								strncpy(tile->Filename, file_path, sizeof(tile->Filename)-2);
							}
						} else {
							mixfile_set = (IsoTileSet *)FileSystem::LoadFile(file_path, false);
							if (mixfile_set) {
								found_image = true;
							}
						}
						if (!found_image) {
							_makepath(file_path, NULL, NULL, tile_name, ".URB");
							if (from_ccfile) {
								CCFileClass file(file_path);
								found_image = file.Is_Available();
								if (found_image && tile_count == 0) {
									strncpy(tile->Filename, file_path, sizeof(tile->Filename)-2);
								}
							} else {
								mixfile_set = (IsoTileSet *)FileSystem::LoadFile(file_path, false);
								if (mixfile_set) {
									found_image = true;
								}
							}
						}
					}
				}

				if (tile_count != 0) {
					if (!mixfile_set && !found_image) {
						break;
					}
					tile = new IsometricTileTypeClass((IsometricTileType)(heapid - 1), uninitialized_value, 0, tile_name, true);

					tile->GivenName = given_name;
					tile->MarbleMadness = _MarbleMadness;
					tile->NonMarbleMadness = _NonMarbleMadness;
					tile->IsMorphable = _Morphable;
					tile->IsAllowToPlace = _AllowToPlace;
					tile->IsAllowBurrowing = _AllowBurrowing;
					tile->IsAllowTiberium = _AllowTiberium;
					tile->IsRequiredForRMG = _RequiredForRMG;
					if (from_ccfile) {
						tile->IsFileLoaded = true;
						strncpy(tile->Filename, file_path, sizeof(tile->Filename)-2);
					}
					if (is_shadow_caster && shadow_tiles) {
						tile->IsShadowCaster = is_shadow_caster;
					}
					tile->TileSetBaseID = TileSetIDLookup[TileSetLookupCount - 1];
					tail_tile->NextTileTypeInSet = tile;
					tile->PreviewTiles.Clear();
					tail_tile = tile;
				}

				if (tile_count == 0 || mixfile_set) {
					tile->ImageData = mixfile_set;
				}

				if (mixfile_set != NULL) {
					IsoTileSet const * tileset = mixfile_set;
					tile->Width = (unsigned char)tileset->MapWidth;
					tile->Height = (unsigned char)tileset->MapHeight;
					for (j = 0; j < mixfile_set->Tile_Count(); j++) {
						IsoTileRecord ** record = &mixfile_set->Tiles[j];
						if (*record != 0 && (unsigned int)*record < (unsigned int)mixfile_set) {
							*record = (IsoTileRecord *)((unsigned int)mixfile_set + (unsigned int)*record);
						}
					}
					tile->Build_Preview_Tiles();
				} else {
					if (tile_count == 0) {
						tile->Width = 0;
						tile->Height = 0;
					}
				}
				tile_count++;
			} while (mixfile_set != NULL || found_image);

			if (tile_count > 1) {
				IsometricTileTypeClass * tile = head_tile;
				for (char t = tile_count; t > 1; t--) {
					tile->NumTileTypesInSet = t;
					tile = tile->NextTileTypeInSet;
				}
			}
		}
		setid++;
	}

	for (j = 0; j < IsometricTileTypes.Count(); ++j) {
		IsometricTileTypeClass * isotype = IsometricTileTypes[j];
		int offset = isotype->HeapID - isotype->TileSetBaseID;
		if (isotype->MarbleMadness != ISOTILE_NONE) {
			isotype->MarbleMadness = (IsometricTileType)(TileSetIDLookup[isotype->MarbleMadness] + offset);
		}
		if (isotype->NonMarbleMadness != ISOTILE_NONE) {
			isotype->NonMarbleMadness = (IsometricTileType)(TileSetIDLookup[isotype->NonMarbleMadness] + offset);
		}
	}

	if (theater == THEATER_LUNAR) {
		ShorePieces = ISOTILE_INVALID;
		WaterSet = ISOTILE_INVALID;
		CliffSet = ISOTILE_INVALID;
		WaterCliffs = ISOTILE_INVALID;
		WaterBridge = ISOTILE_INVALID;
		BridgeSet = ISOTILE_INVALID;
		TrainBridgeSet = ISOTILE_INVALID;
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

// FUNCTION: GAMEMD 0x00547370
Cell const * IsometricTileTypeClass::Shadow_Caster_List(void) const
{
	if (IsShadowCaster) {
		static const Cell _list[CLIFF_COUNT][6] = {
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(0,-1), Cell(0,-1), Cell(-1,-2), CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(0,-1), Cell(0,-1), Cell(-1,-2), CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(-1,-3), Cell(0,-3), CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(-1,-3), Cell(0,-3), CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(-1,-3), Cell(0,-3), CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(0,-3), CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(0,-3), Cell(1,-3), Cell(-1,-3), Cell(-2,-3), CELL_NONE, REFRESH_EOL },
			{ Cell(0,-3), Cell(-1,-3), Cell(-2,-3), Cell(-3,-2), CELL_NONE, REFRESH_EOL },
			{ Cell(0,-3), Cell(-1,-3), Cell(-2,-3), CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(0,-3), Cell(-1,-3), Cell(-2,-3), CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(-2,-3), CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(-2,-3), CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ Cell(0,-3), CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
			{ CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, CELL_NONE, REFRESH_EOL },
		};

		for (int i = 0; i < ARRAY_SIZE(ShadowCasterTiles); i++) {
			int idx = HeapID - ShadowCasterTiles[i];
			if (idx >= 0 && HeapID - ShadowCasterTiles[i] < CLIFF_COUNT) {
				return(_list[idx]);
			}
		}
	}
	return(NULL);
}

// SYNTHETIC: GAMEMD 0x00547CE0 SYMBOL
// _$E214

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

// FUNCTION: GAMEMD 0x00547230
void IsometricTileTypeClass::Draw_Shadow_Caster(int index, Surface * surf, Point2D point, Rect cliprect, int offset)
{
	int x;
	int y;
	int frame;
	int caster_index;

	int tiletype = HeapID;

	bool flat = false;

	int pieces;
	if (tiletype >= SlopeSetPieces && tiletype < SlopeSetPieces + ARRAY_SIZE(ShadowCasterSlopeInfo)) {
		pieces = SlopeSetPieces;
		flat = true;
	} else {
		if (tiletype >= SlopeSetPieces2 && tiletype < SlopeSetPieces2 + ARRAY_SIZE(ShadowCasterSlopeInfo)) {
			pieces = SlopeSetPieces2;
			flat = true;
		}
	}

	bool can_draw = false;

	if (flat && tiletype - pieces != -1) {
		caster_index = tiletype - pieces;

		if (index == ShadowCasterSlopeInfo[caster_index].Index) {

			frame = ShadowCasterSlopeInfo[caster_index].Frame;

			if (frame != 0) {
				x = (ISO_TILE_PIXEL_W / 2) + ShadowCasterSlopeInfo[caster_index].XOffset;
				y = (ISO_TILE_PIXEL_H / 2) + ShadowCasterSlopeInfo[caster_index].YOffset;
				can_draw = true;
			}
		}
	} else if (IsShadowCaster) {

		for (int i = 0; i < ARRAY_SIZE(ShadowCasterTiles); i++) {
			caster_index = tiletype - ShadowCasterTiles[i];
			if (caster_index >= 0 && caster_index < ARRAY_SIZE(ShadowCasterCliffInfo)) break;
			if (i == 4) return;
		}
		if (index == ShadowCasterCliffInfo[caster_index].Index) {
			frame = ShadowCasterCliffInfo[caster_index].Frame;
			if (frame != 0) {
				x = ISO_TILE_PIXEL_W / 2 + ShadowCasterCliffInfo[caster_index].XOffset;
				y = ISO_TILE_PIXEL_H / 2 + ShadowCasterCliffInfo[caster_index].YOffset;
				can_draw = true;
			}
		}
	}

	if (can_draw) {
		point.X += x;
		point.Y += y;
		point.X += DSurface::ViewBounds.X;
		point.Y += DSurface::ViewBounds.Y;
		point.X -= cliprect.X;
		point.Y -= cliprect.Y;
		Draw_Shape(
			*surf,
			*TileDrawers[0],
			(const ShapeSet *)CellShadowShapes,
			frame - 1,
			point,
			cliprect,
			ShapeFlags_Type(SHAPE_ZWRITE|SHAPE_WIN_REL|SHAPE_CENTER|SHAPE_DARKEN),
			0,
			offset);
	}
}
