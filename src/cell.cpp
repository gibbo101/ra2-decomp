// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "cell.h"
#include "_alpha.h"
#include "_bench.h"
#include "_convert.h"
#include "_logic.h"
#include "_mixfile.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "abuffer.h"
#include "anim.h"
#include "astar.h"
#include "bench.h"
#include "building.h"
#include "builtype.h"
#include "combat.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsurface.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "incdec.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "isotype.h"
#include "light.h"
#include "lightcalc.h"
#include "lightcon.h"
#include "logic.h"
#include "mixfile.h"
#include "overlay.h"
#include "overtype.h"
#include "pixelfx.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "smudtype.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "terrain.h"
#include "tiberium.h"
#include "tracker.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vein.h"
#include "vox.h"
#include "voc.h"
#include "warhead.h"
#include "bench.hh"
#include "ramp.hh"
#include "tube.hh"

// SYNTHETIC: GAMEMD 0x0047B150 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0047B180 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0047B1A0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0047B1C0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0047B1E0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0047B200 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0047B220 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0047B260 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0047B290 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0047B2C0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0047B2F0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0047B300 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0047B320 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0047B340 SYMBOL
// _$E41

void __cdecl FUN_004068E0(char const * format, ...);

inline IsometricTileTypeClass * Next_Tile_In_Set(IsometricTileTypeClass * type, int index) { return type->Next_Tile_From_Set(index); }
inline unsigned short * Preview_Tile_Of(IsometricTileTypeClass * type, int tile, int level) { return type->Preview_Tile(tile, level); }
inline VisualType Building_Visual(BuildingClass * building) { return building->Visual_Character(false, NULL); }

// FUNCTION: GAMEMD 0x0047B360
CellClass::CellClass(NoInitClass const & x) :
	BASECLASS(x),
	Drawer(NULL),
	Passability(PASSABLE_LAND)
{

}

// FUNCTION: GAMEMD 0x0047B3A0
LEPTON CellClass::Get_Height(Point2D const & point) const
{
	static double _level_height = LEVEL_LEPTON_H;
	static double _level_slope = _level_height / CELL_LEPTON_W;

	static struct {
		double XChange;
		double YChange;
		double Base;
		double Max;
		double Extra;
	} _ramp_control[RAMP_COUNT-1] = {
		{  1.0,	 0.0,	 0.0,							_level_height,					0.0 },
		{  0.0,	 1.0,	 0.0,							_level_height,					0.0 },
		{ -1.0,	 0.0,	 _level_height,					_level_height,					0.0 },
		{  0.0,	-1.0,	 _level_height,					_level_height,					0.0 },
		{  1.0,	 1.0,	-_level_height,					_level_height,					0.0 },
		{ -1.0,	 1.0,	 0.0,							_level_height,					0.0 },
		{ -1.0,	-1.0,	 _level_height,					_level_height,					0.0 },
		{  1.0,	-1.0,	 0.0,							_level_height,					0.0 },
		{  1.0,	 1.0,	 0.0,							_level_height,					0.0 },
		{ -1.0,	 1.0,	 _level_height,					_level_height,					0.0 },
		{ -1.0,	-1.0,	 _level_height + _level_height, _level_height,					0.0 },
		{  1.0,	-1.0,	 _level_height,					_level_height,					0.0 },
		{  1.0,	 1.0,	 0.0,							_level_height + _level_height,	0.0 },
		{ -1.0,	 1.0,	 _level_height,					_level_height + _level_height,	0.0 },
		{ -1.0,	-1.0,	 _level_height + _level_height, _level_height + _level_height,	0.0 },
		{  1.0,	-1.0,	 _level_height,					_level_height + _level_height,	0.0 },
		{  0.0,	 0.0,	 0.0,							_level_height * 0.5,			_level_height * 0.5	 },
		{  0.0,	 0.0,	 _level_height,					_level_height * 0.5,			_level_height * -0.5 },
		{  0.0,	 0.0,	 0.0,							_level_height * 0.5,			_level_height * 0.5	 },
		{  0.0,	 0.0,	 _level_height,					_level_height * 0.5,			_level_height * -0.5 }
	};

	LEPTON height = LEPTON(LEVEL_LEPTON_H * Height + 0.5);

	if (Ramp > 0) {
		int ramp = Ramp - 1;
		double rampheight = ((point.X & (CELL_LEPTON_W - 1)) * _ramp_control[ramp].XChange * _level_slope) +
							((point.Y & (CELL_LEPTON_H - 1)) * _ramp_control[ramp].YChange * _level_slope) +
							_ramp_control[ramp].Base +
							_ramp_control[ramp].Extra;

		if (rampheight < 0.0) {
			rampheight = 0.0;
		}
		if (rampheight > _ramp_control[ramp].Max) {
			rampheight = _ramp_control[ramp].Max;
		}

		height += rampheight;
	}

	return(height);
}

// FUNCTION: GAMEMD 0x0047BB60
CellClass::~CellClass(void)
{
	if (FoggedObjects) {
		delete FoggedObjects;
		FoggedObjects = NULL;
	}

	if (PixelFX) {
		delete PixelFX;
		PixelFX = NULL;
	}

	OccupierPtr = 0;

	if (CellID != CELL_NONE && Drawer) {
		Dec_Drawer_Ref_Count(Drawer);
		Drawer = NULL;
	}
}

// FUNCTION: GAMEMD 0x0047BBF0
CellClass::CellClass(void) :
	BASECLASS(),
	CellID(CELL_NONE),
	IsFogVisible(false),
	IsFogMapped(false),
	IsWaypoint(false),
	IsRadarCursor(false),
	IsFlagged(false),
	IsToShroud(false),
	IsToFog(false),
	IsBridgeDeck(false),
	IsUnderBridge(false),
	IsBridgeTraversable(false),
	WasUnderBridge(false),
	IsBridgeEastWest(false),
	IsBridgeSurface(false),
	IsBridgeDamaged(false),
	IsToGrowIce(false),
	IsToGrowVeins(false),
	IsOvershadowed(false),
	IsAnimAttached(false),
	IsPredictedPath(false),
	IsAffectedByEMP(false),
	IsHorizontalLine(false),
	IsVerticalLine(false),
	IsFogged(false),
	IsPlot(false),
	IsCursorHere(false),
	IsFoundationCopyHere(false),
	IsMapped(false),
	IsVisible(false),
	ITType(TILE_NONE),
	Overlay(OVERLAY_NONE),
	OverlayData(0),
	Smudge(SMUDGE_NONE),
	SmudgeData(0),
	Owner(HOUSE_NONE),
	InfType(HOUSE_NONE),
	BridgeInfType(HOUSE_NONE),
	OccupierPtr(NULL),
	BridgeOccupierPtr(NULL),
	Land(LAND_CLEAR),
	FoggedObjects(NULL),
	BridgeDeckCell(NULL),
	UnusedCell(NULL),
	Drawer(NULL),
	Tag(NULL),
	Rubble(NULL),
	Jumpjet(NULL),
	RadLevel(0),
	RadSite(NULL),
	PixelFX(NULL),
	OccupyHeightsCoveringMe(0),
	ShroudCounter(1),
	GapsCoveringThisCell(0),
	VisibilityChanged(false),
	unknown_13C(0),
	Passability(PASSABLE_LAND),
	LastRedrawFrame(-1),
	LastUnknownDrawFrame(-1),
	LastBridgeDrawFrame(-1),
	LastBridgeDrawRect(Rect(0,0,0,0)),
	Tube(-1),
	ShadowFrame(-2),
	FogFrame(-2),
	CloakedBy(0),
	OccupiedBy(0),
	Intensity(0x10000),
	Ambient(0),
	Brightness(1000),
	TileBrightness(1000),
	AltBrightness(1000),
	RedTint(1000),
	GreenTint(1000),
	BlueTint(1000),
	LastBridgeDrawRedraws(-1),
	IsIceGrowthAllowed(false),
	SubTile(0),
	Height(0),
	Ramp(0),
	Elevation(0),
	AdjacentObjectCount(0)
{
	for (int i = 0; i < 24; i++) {
		SensorsOfHouses[i] = 0;
	}
	for (int j = 0; j < 24; j++) {
		DisguiseSensorsOfHouses[j] = 0;
	}
	Create_ID();
	Flag.Composite = 0;
	BridgeFlag.Composite = 0;
}

// FUNCTION: GAMEMD 0x0047BDB0
int CellClass::Preview_Cell_Color(unsigned char & unknown, bool terrainonly) const
{
	unknown = 0;
	BuildingClass * obj = Cell_Building();

	if (!terrainonly && obj != NULL && !obj->Class->IsInvisible && !obj->Class->IsInvisibleInGame && Building_Visual(obj) != VISUAL_HIDDEN) {
		ColorScheme * owner_scheme = ColorSchemes[obj->House->Scheme];
		unsigned int color = owner_scheme->Converter->Convert_Pixel(owner_scheme->Color);
		if (color == 0) {
			FUN_004068E0("We just got a black pixel for the preview image for the building %s\n", obj->Class->IniName);
		}
		return((color << 16) | color);
	}

	if (IsUnderBridge) {
		RGBClass bridge_color = OverlayTypes[OVERLAY_BRIDGE1]->Get_Radar_Color(0);
		unsigned int color = DSurface::Build_Hicolor_Pixel(bridge_color);
		if (color == 0) {
			bridge_color = OverlayTypes[OVERLAY_BRIDGE1]->RadarColor;
			color = DSurface::Build_Hicolor_Pixel(bridge_color);
			if (color == 0) {
				FUN_004068E0("We just got a black pixel for the preview image for a bridge\n");
			}
		}
		return((color << 16) | color);
	}

	IsometricTileTypeClass * ittype;

	int icon;
	if (ITType != ISOTILE_NONE) {
		ittype = IsometricTileTypes[ITType];
	} else {
		ittype = IsometricTileTypes[TILE_CLEAR];
	}

	icon = 0;
	if (ITType != ISOTILE_NONE) {
		if (ittype->NumTileTypesInSet > 1) {
			if (ittype->Is_Randomized(SubTile)) {
				icon = IsBridgeDamaged ? 1 : 0;
			} else {
				int numtiles = ittype->NumTileTypesInSet;
				icon = Clear_Icon(ITType, numtiles);
			}
		}
	} else {
		icon = Clear_Icon(TILE_CLEAR, ittype->NumTileTypesInSet);
	}

	IsometricTileTypeClass * next_isotype = Next_Tile_In_Set(ittype, icon);
	unsigned short * tile = Preview_Tile_Of(next_isotype, SubTile, Height);
	return(((tile[0] << 16) | tile[1]));
}

struct DAT_007E1BD0_Entry {
	float Scale;
	char Rest[0x6C];
};
extern DAT_007E1BD0_Entry DAT_007E1BD0[];

// FUNCTION: GAMEMD 0x0047C060
void CellClass::Cell_Color(RGBClass & lowcolor, RGBClass & highcolor) const
{
	TerrainClass * tptr = Cell_Terrain();
	if (tptr != NULL) {
		lowcolor = RGBClass(200, 200, 160);
		highcolor = RGBClass(200, 200, 160);
		return;
	}

	if (IsUnderBridge) {
		highcolor = lowcolor = OverlayTypes[OVERLAY_BRIDGE1]->Get_Radar_Color(0);
		return;
	}

	IsometricTileTypeClass * itype = (ITType != ISOTILE_NONE) ? IsometricTileTypes[ITType] : IsometricTileTypes[TILE_CLEAR];

	OverlayType overlay = Overlay;
	if (overlay == OVERLAY_NONE) goto tile;
	if ((overlay == OVERLAY_LOWBRIDGE_27 || overlay == OVERLAY_LOWBRIDGE_28) && (Overlay == OVERLAY_NONE || Overlay == (OverlayType)0xE7 || Overlay == (OverlayType)0xE8)) goto tile;
	if (overlay != (OverlayType)0xEF) {
		OverlayTypeClass * otype = OverlayTypes[overlay];

		if (!otype->IsTiberium) {
			if ((overlay >= OVERLAY_LOWBRIDGE_01 && overlay <= OVERLAY_LOWBRIDGE_26) || (overlay >= 0xCD && overlay <= 0xE6)) {
				lowcolor = OverlayTypes[overlay]->Get_Radar_Color(1);
			} else {
				lowcolor = OverlayTypes[overlay]->Get_Radar_Color(OverlayData);
			}
			highcolor = lowcolor;
			return;
		}

		if (otype->CellAnim != NULL) {
			ShapeSet * shape = (ShapeSet *)otype->CellAnim->Get_Image_Data();
			highcolor = lowcolor = shape->Get_Color(OverlayData);
			return;
		}

		if (Which_Tiberium_Type(overlay) != TIBERIUM_NONE) {
			lowcolor = RGBClass(170, 170, 130);
			highcolor = RGBClass(170, 170, 130);
			return;
		}
	}

	tile:
	int tile = 0;
	if (ITType != ISOTILE_NONE) {
		if (itype->NumTileTypesInSet > 1) {
			if (itype->Is_Randomized(SubTile)) {
				tile = IsBridgeDamaged ? 1 : 0;
			} else {
				int numtiles = itype->NumTileTypesInSet;
				tile = Clear_Icon(ITType, numtiles);
			}
		}
	} else {
		int numtiles = itype->NumTileTypesInSet;
		tile = Clear_Icon(TILE_CLEAR, numtiles);
	}

	IsometricTileTypeClass * next = itype->Next_Tile_From_Set(tile);
	if (next->ImageData == NULL && next->IsFileLoaded) {
		next->Load_Tile_Image();
	}

	IsoTileSet * set = (IsoTileSet *)next->ImageData;
	if (set->Fetch_Record_Pointer_Unsafe(SubTile) != NULL) {
		IsoTileRecord const * record = set->Fetch_Record_Pointer_Unsafe(SubTile);
		RGBClass color(record->LowColor.Red, record->LowColor.Green, record->LowColor.Blue);

		color = RGBClass(NoInitClass()).Set(color, DAT_007E1BD0[Scen->Theater].Scale);

		int red = color.Get_Red() >> 1;
		int green = color.Get_Green() >> 1;
		int blue = color.Get_Blue() >> 1;
		lowcolor = RGBClass(red, green, blue);
		highcolor = RGBClass(red, green, blue);
	} else {
		lowcolor = RGBClass(60, 60, 60);
		highcolor = RGBClass(60, 60, 60);
	}
}

// FUNCTION: GAMEMD 0x0047C3D0
TechnoClass * CellClass::Cell_Techno(Point2D const & xy, bool bridge, TechnoClass const * notthis) const
{
	ObjectClass * object;
	Coord		click;
	TechnoClass * close = NULL;
	int		distance = 0;

	click = Coord(PIXEL_TO_LEPTON(xy.X),PIXEL_TO_LEPTON(xy.Y));

	ObjectClass * occupier = Cell_Occupier(bridge);
	if (occupier != NULL) {
		object = occupier;
		while (object != NULL) {
			if (object->Is_Techno() && object != (ObjectClass *) notthis) {
				Coord coord = object->Center_Coord();
				int dist = Point2D(Coord_Fraction(coord) - click).Length();
				if (!close || dist < distance) {
					close = (TechnoClass *)object;
					distance = dist;
				}
			}
			object = object->Next;
		}
	}
	return(close);
}

// FUNCTION: GAMEMD 0x0047C4D0
ObjectClass * CellClass::Cell_Find_Object(RTTIType rtti, bool bridge) const
{
	if (GameActive) {
		ObjectClass * object = Cell_Occupier(bridge);

		while (object != NULL) {
			if (object->RTTI == rtti) {
				return(object);
			}
			object = object->Next;
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0047C520
BuildingClass * CellClass::Cell_Building(void) const
{
	return((BuildingClass *)Cell_Find_Object(RTTI_BUILDING));
}

// FUNCTION: GAMEMD 0x0047C550
TerrainClass * CellClass::Cell_Terrain(bool bridge) const
{
	return((TerrainClass *)Cell_Find_Object(RTTI_TERRAIN, bridge));
}

// FUNCTION: GAMEMD 0x0047C5A0
ObjectClass * CellClass::Cell_Object(Point2D const & xy, bool bridge) const
{
	ObjectClass * ptr;

	ptr = (ObjectClass *)Cell_Find_Object(RTTI_AIRCRAFT);
	if (ptr) {
		return(ptr);
	}

	ptr = Cell_Techno(xy, bridge);
	if (ptr) {
		return(ptr);
	}
	ptr = Cell_Terrain();
	return(ptr);
}

static inline Cell Ramp_Neighbor(Cell const & cell, FacingType dir)
{
	return Cell(cell.X + AdjacentCell[dir].X, cell.Y + AdjacentCell[dir].Y);
}

static inline CellClass const & LAT_Neighbor(CellClass const & cell, FacingType face)
{
	if ((unsigned)face >= FACING_COUNT) {
		return cell;
	}
	Cell adjacent = AdjacentCell[(int)face & 7] + cell.Fetch_CellID();
	return Map[adjacent];
}

// FUNCTION: GAMEMD 0x0047CA80
bool CellClass::Fixup_LAT(void)
{
	IsometricTileType waterbridge1 = IsometricTileTypeClass::WaterBridge;
	IsometricTileType waterbridge2 = IsometricTileType(waterbridge1 + 1);
	if (waterbridge1 == ISOTILE_INVALID) waterbridge2 = ISOTILE_INVALID;
	IsometricTileType shore1 = IsometricTileTypeClass::ShorePieces;
	IsometricTileType shore2 = IsometricTileType(shore1 + 41);
	if (shore1 == ISOTILE_INVALID) shore2 = ISOTILE_INVALID;
	IsometricTileType cleartorough1 = IsometricTileTypeClass::ClearToRoughLat;
	IsometricTileType cleartorough2 = IsometricTileType(cleartorough1 + 15);
	if (cleartorough1 == ISOTILE_INVALID) {
		cleartorough2 = ISOTILE_INVALID;
	}
	IsometricTileType cleartosand1 = IsometricTileTypeClass::ClearToSandLat;
	IsometricTileType cleartosand2 = IsometricTileType(cleartosand1 + 15);
	if (cleartosand1 == ISOTILE_INVALID) {
		cleartosand2 = ISOTILE_INVALID;
	}
	IsometricTileType cleartogreen1 = IsometricTileTypeClass::ClearToGreenLat;
	IsometricTileType cleartogreen2 = IsometricTileType(cleartogreen1 + 15);
	if (cleartogreen1 == ISOTILE_INVALID) {
		cleartogreen2 = ISOTILE_INVALID;
	}
	IsometricTileType cleartopave1 = IsometricTileTypeClass::ClearToPaveLat;
	IsometricTileType cleartopave2 = IsometricTileType(cleartopave1 + 15);
	if (cleartopave1 == ISOTILE_INVALID) {
		cleartopave2 = ISOTILE_INVALID;
	}
	IsometricTileType miscpave2 = IsometricTileTypeClass::MiscPaveTile;
	IsometricTileType miscpave1 = miscpave2;
	miscpave2 = IsometricTileType(miscpave2 + 13);
	if (miscpave1 == ISOTILE_INVALID) {
		miscpave2 = ISOTILE_INVALID;
	}
	IsometricTileType pavedroad2 = IsometricTileTypeClass::PavedRoads;
	IsometricTileType pavedroad1 = pavedroad2;
	pavedroad2 = IsometricTileType(pavedroad2 + 20);
	if (pavedroad1 == ISOTILE_INVALID) {
		pavedroad2 = ISOTILE_INVALID;
	}
	IsometricTileType median2 = IsometricTileTypeClass::Medians;
	IsometricTileType median1 = median2;
	median2 = IsometricTileType(median2 + 13);
	if (median1 == ISOTILE_INVALID) {
		median2 = ISOTILE_INVALID;
	}
	IsometricTileType rampsmooth1 = IsometricTileTypeClass::RampSmooth;
	IsometricTileType rampsmooth2 = IsometricTileType(rampsmooth1 + 11);
	if (rampsmooth1 == ISOTILE_INVALID) {
		rampsmooth2 = ISOTILE_INVALID;
	}
	IsometricTileType ramp1 = IsometricTileTypeClass::RampStart;
	IsometricTileType ramp2 = IsometricTileType(ramp1 + 19);
	if (ramp1 == ISOTILE_INVALID) {
		ramp2 = ISOTILE_INVALID;
	}
	IsometricTileType old_ittype = ITType;

	if (old_ittype == IsometricTileTypeClass::RoughTile || old_ittype >= cleartorough1 && old_ittype <= cleartorough2) {
		int offset = 0;
		unsigned int facing = 0;
		for (int i = 0; i < FACING_COUNT / 2; i++) {
			CellClass const & cptr = Adjacent_Cell((FacingType)facing);
			IsometricTileType ittype = cptr.ITType;
			if (ittype != IsometricTileTypeClass::RoughTile && (ittype < cleartorough1 || ittype > cleartorough2)) {
				offset |= 1 << i;
			}
			facing = Facing_Add(facing, FACING_90);
		}
		if (offset != 0) {
			ITType = IsometricTileType(cleartorough1 + offset);
		} else {
			ITType = IsometricTileTypeClass::RoughTile;
		}
	}

	IsometricTileType type;
	if (cleartosand1 != ISOTILE_INVALID) {
		type = ITType;
		if (type == IsometricTileTypeClass::SandTile || type >= cleartosand1 && type <= cleartosand2) {
			int offset = 0;
			unsigned int facing = 0;
			for (int i = 0; i < FACING_COUNT / 2; i++) {
				CellClass const & cptr = LAT_Neighbor(*this, (FacingType)facing);
				IsometricTileType ittype = cptr.ITType;
				if (ittype != IsometricTileTypeClass::SandTile && (ittype < cleartosand1 || ittype > cleartosand2)) {
					offset |= 1 << i;
				}
				facing = Facing_Add(facing, FACING_90);
			}
			if (offset != 0) {
				ITType = IsometricTileType(cleartosand1 + offset);
			} else {
				ITType = IsometricTileTypeClass::SandTile;
			}
		}
	}

	if (cleartogreen1 != ISOTILE_INVALID) {
		type = ITType;
		if (type == IsometricTileTypeClass::GreenTile || type >= cleartogreen1 && type <= cleartogreen2) {
			int offset = 0;
			unsigned int facing = 0;
			for (int i = 0; i < FACING_COUNT / 2; i++) {
				CellClass const & cptr = LAT_Neighbor(*this, (FacingType)facing);
				IsometricTileType ittype = cptr.ITType;
				if (ittype != IsometricTileTypeClass::GreenTile && (ittype < cleartogreen1 || ittype > cleartogreen2) && (ittype < shore1 || ittype > shore2) && (ittype < waterbridge1 || ittype > waterbridge2)) {
					offset |= 1 << i;
				}
				facing = Facing_Add(facing, FACING_90);
			}
			if (offset != 0) {
				ITType = IsometricTileType(cleartogreen1 + offset);
			} else {
				ITType = IsometricTileTypeClass::GreenTile;
			}
		}
	}

	if (cleartopave1 != ISOTILE_INVALID) {
		type = ITType;
		if (type == IsometricTileTypeClass::PaveTile || type >= cleartopave1 && type <= cleartopave2) {
			int offset = 0;
			unsigned int facing = 0;
			for (int i = 0; i < FACING_COUNT / 2; i++) {
				CellClass const & cptr = LAT_Neighbor(*this, (FacingType)facing);
				IsometricTileType ittype = cptr.ITType;
				if (ittype != IsometricTileTypeClass::PaveTile
				&& (ittype < cleartopave1 || ittype > cleartopave2)
				&& (ittype < miscpave1 || ittype > miscpave2)
				&& (ittype < median1 || ittype > median2)
				&& (ittype < pavedroad1 || ittype > pavedroad2)) {
					offset |= 1 << i;
				}
				facing = Facing_Add(facing, FACING_90);
			}
			if (offset != 0) {
				ITType = IsometricTileType(cleartopave1 + offset);
			} else {
				ITType = IsometricTileTypeClass::PaveTile;
			}
		}
	}

	type = ITType;
	if (type >= ramp1 && type <= ramp2 || type >= rampsmooth1 && type <= rampsmooth2) {
		int offset = 0;
		if (Ramp == RAMP_WEST) {
			if (Map[Ramp_Neighbor(Fetch_CellID(), FACING_W)].Ramp == RAMP_NONE) {
				offset |= 1;
			}
			if (Map[Ramp_Neighbor(Fetch_CellID(), FACING_E)].Ramp == RAMP_NONE) {
				offset |= 2;
			}
			if (offset != 0) {
				ITType = IsometricTileType(offset + rampsmooth1 - 1);
			}
		}
		if (Ramp == RAMP_NORTH) {
			if (Map[Ramp_Neighbor(Fetch_CellID(), FACING_N)].Ramp == RAMP_NONE) {
				offset |= 1;
			}
			if (Map[Ramp_Neighbor(Fetch_CellID(), FACING_S)].Ramp == RAMP_NONE) {
				offset |= 2;
			}
			if (offset != 0) {
				ITType = IsometricTileType(offset + rampsmooth1 + 2);
			}
		}
		if (Ramp == RAMP_EAST) {
			if (Map[Ramp_Neighbor(Fetch_CellID(), FACING_E)].Ramp == RAMP_NONE) {
				offset |= 1;
			}
			if (Map[Ramp_Neighbor(Fetch_CellID(), FACING_W)].Ramp == RAMP_NONE) {
				offset |= 2;
			}
			if (offset != 0) {
				ITType = IsometricTileType(offset + rampsmooth1 + 5);
			}
		}
		if (Ramp == RAMP_SOUTH) {
			if (Map[Ramp_Neighbor(Fetch_CellID(), FACING_S)].Ramp == RAMP_NONE) {
				offset |= 1;
			}
			if (Map[Ramp_Neighbor(Fetch_CellID(), FACING_N)].Ramp == RAMP_NONE) {
				offset |= 2;
			}
			if (offset != 0) {
				ITType = IsometricTileType(offset + rampsmooth1 + 8);
			}
		}
		if (offset == 0) {
			ITType = IsometricTileType(Ramp + ramp1 - 1);
		}
	}

	if (old_ittype != ITType) {
		IsometricTileTypeClass * ttype = IsometricTileTypes[ITType];
		if (ttype->Get_Image_Data() == NULL) {
			ttype->Load_Tile_Image();
		}
	}
	return(old_ittype != ITType);
}



// FUNCTION: GAMEMD 0x0047D210
void CellClass::Set_Wall_Owner(void)
{
	if (Overlay != OVERLAY_NONE && OverlayTypes[Overlay]->IsWall) {
		HousesType owner = HOUSE_NONE;
		int dist = 0x7FFFFFFF;
		for (int i = 0; i < Buildings.Count(); i++) {
			BuildingClass * bld = Buildings[i];
			if (bld->IsActive && bld->IsDown && bld->House->Class->IsWallOwner) {
				int d = bld->Distance(this);
				if (d < dist) {
					dist = d;
					owner = bld->Owner();
				}
			}
		}
		Owner = owner;
	}
}

// FUNCTION: GAMEMD 0x0047D2B0
void CellClass::Recalc_Attributes(int cell_height)
{
	if (this == &InvalidCell) {
		return;
	}

	CellZoneStruct * zone = &Map.CellZones[Map.Get_Cell_Zone_Index(CellID)];
	CellSubzoneStruct * subzone = &Map.CellSubzones[Map.Get_Cell_Zone_Index(CellID)];

	OverlayType overlay = Overlay;

	// Check for wall effects.
	if (overlay != OVERLAY_NONE) {
		OverlayTypeClass const * otype = OverlayTypes[overlay];
		Land = otype->Land;
		if (Land == LAND_WALL || Land == LAND_RAILROAD || otype->IsNoUseTileLandType) {
			if (ITType != ISOTILE_NONE && ITType < IsometricTileTypes.Count()) {
				IsometricTileTypeClass * itype = IsometricTileTypes[ITType];
				Ramp = itype->Ramp_Type(SubTile);
			}
			if (Ramp != RAMP_NONE && otype->IsTiberium) {
				Overlay = OVERLAY_NONE;
				OverlayData = 0;
			}
			if (Rule->CliffBackImpassability != 0) {
				if (Height + 4 <= Map[CellID + Cell(+0, -1)].Height
				|| (Height + 4 <= Map[CellID + Cell(-1, +0)].Height)
				|| (Height + 4 <= Map[CellID + Cell(+2, +2)].Height)
				|| (Height + 4 <= Map[CellID + Cell(+1, +1)].Height)
				|| (Height + 4 <= Map[CellID + Cell(-1, +1)].Height)
				|| (Height + 4 <= Map[CellID + Cell(+1, -1)].Height)) {
					if (Rule->CliffBackImpassability == 2) {
						Land = LAND_ROCK;
					}
				}
			}
			Fixup_LAT();
			Recalc_Passability();
			zone->Height = Height;
			subzone->Height = Height;
			zone->Passability = Passability;
			return;
		}
	}

	if (ITType >= IsometricTileTypes.Count()) {
		ITType = ISOTILE_NONE;
	}

	// Determine the land type from the template and its icon number.
	if (ITType != ISOTILE_NONE) {
		IsometricTileTypeClass * itype = IsometricTileTypes[ITType];
		bool valid = itype->Is_Tile_Index_Valid(SubTile, false);

		if (Ramp != RAMP_NONE && !valid) {
			valid = itype->Is_Tile_Index_Valid(SubTile, true);
		}

		if (!valid) {
			ITType = ISOTILE_NONE;
			SubTile = 0;
			Land = LAND_CLEAR;
			Ramp = RAMP_NONE;
			if (Rule->CliffBackImpassability != 0) {
				if (Height + 4 <= Map[CellID + Cell(+0, -1)].Height
				|| (Height + 4 <= Map[CellID + Cell(-1, +0)].Height)
				|| (Height + 4 <= Map[CellID + Cell(+2, +2)].Height)
				|| (Height + 4 <= Map[CellID + Cell(+1, +1)].Height)
				|| (Height + 4 <= Map[CellID + Cell(-1, +1)].Height)
				|| (Height + 4 <= Map[CellID + Cell(+1, -1)].Height)) {
					if (Rule->CliffBackImpassability == 2 && Land == LAND_CLEAR) {
						Land = LAND_ROCK;
					}
				}
			}
			Recalc_Passability();
			zone->Height = Height;
			subzone->Height = Height;
			zone->Passability = Passability;
			return;
		}

		Ramp = itype->Ramp_Type(SubTile);
		Fixup_LAT();
		if (Overlay != OVERLAY_NONE) {
			if (Which_Tiberium_Type(Overlay) != TIBERIUM_NONE) {
				if (Ramp > RAMP_SOUTH) {
					Land = itype->Land_Type(SubTile);
					Overlay = OVERLAY_NONE;
					OverlayData = 0;
				} else if (!OverlayTypes[Overlay]->Land) {
					Land = LAND_TIBERIUM;
				}
			} else if (!OverlayTypes[Overlay]->IsNoUseTileLandType) {
				Land = itype->Land_Type(SubTile);
			}
		} else {
			Land = itype->Land_Type(SubTile);
		}

		if (Land == LAND_TUNNEL) {
			if (Tube < TUBE_FIRST || Tube >= Tubes.Count()) {
				int tunnel_dir = -1;

				if (ITType >= IsometricTileTypeClass::Tunnels && ITType <= IsometricTileTypeClass::Tunnels + 3) {
					tunnel_dir = ITType - IsometricTileTypeClass::Tunnels;
				} else if (ITType >= IsometricTileTypeClass::TrackTunnels && ITType <= IsometricTileTypeClass::TrackTunnels + 3) {
					tunnel_dir = ITType - IsometricTileTypeClass::TrackTunnels;
				} else if (ITType >= IsometricTileTypeClass::DirtTunnels && ITType <= IsometricTileTypeClass::DirtTunnels + 3) {
					tunnel_dir = ITType - IsometricTileTypeClass::DirtTunnels;
				} else if (ITType >= IsometricTileTypeClass::DirtTrackTunnels && ITType <= IsometricTileTypeClass::DirtTrackTunnels + 3) {
					tunnel_dir = ITType - IsometricTileTypeClass::DirtTrackTunnels;
				}

				if (tunnel_dir != -1) {
					static FacingType const _facings[] = { FACING_E, FACING_S, FACING_W, FACING_N };
					new TubeClass(CellID, _facings[tunnel_dir]);
				}
			}
		}

		if (cell_height != -1) {
			Height = cell_height;
		}

		int width;
		int height;
		itype->Get_Tile_Pixel_Dimensions(SubTile, width, height);
		Elevation = (height - ISO_TILE_PIXEL_H) / (ISO_TILE_PIXEL_H / 2);
		if (!IsAnimAttached && itype->Anim != ANIM_NONE && itype->AttachesTo == SubTile) {
			Coord anim_coord = Coord(CellID, LEVEL_LEPTON_H * Height) + TacticalMap->Pixel_To_Coord_Absolute(itype->Offset);
			AnimClass * anim = new AnimClass(AnimTypes[itype->Anim], anim_coord, 0, -1, ShapeFlags_Type(SHAPE_ZREAD | SHAPE_WIN_REL | SHAPE_CENTER), 0);
			anim->ZAdjust = itype->ZAdjust;
			anim->UseCellLightConvert = true;
			anim->DeleteOnMapCleanup = true;
			IsAnimAttached = true;
		}

		if (itype->IsShadowCaster) {
			const Cell * list = itype->Shadow_Caster_List();
			if (list) {
				while (*list != REFRESH_EOL && *list != CELL_NONE) {
					Cell pos = *list + CellID;
					Map[pos].IsOvershadowed = true;
					list++;
				}
			}
		}
	} else {
		// Treat a missing template as clear terrain.
		if (overlay == OVERLAY_NONE || OverlayTypes[overlay]->IsNoUseTileLandType) {
			Land = LAND_CLEAR;
		} else {
			Land = OverlayTypes[overlay]->Land;
		}
		Ramp = RAMP_NONE;
	}

	if (Rule->CliffBackImpassability != 0) {
		if (Height + 4 <= Map[CellID + Cell(+0, -1)].Height
		|| (Height + 4 <= Map[CellID + Cell(-1, +0)].Height)
		|| (Height + 4 <= Map[CellID + Cell(+2, +2)].Height)
		|| (Height + 4 <= Map[CellID + Cell(+1, +1)].Height)
		|| (Height + 4 <= Map[CellID + Cell(-1, +1)].Height)
		|| (Height + 4 <= Map[CellID + Cell(+1, -1)].Height)) {
			if (Rule->CliffBackImpassability == 2) {
				if (Land == LAND_CLEAR || Land == LAND_WATER || Land == LAND_BEACH || Land == LAND_ICE) {
					Land = LAND_ROCK;
				}
			}
		}
	}

	Recalc_Passability();
	zone->Height = Height;
	zone->Passability = Passability;
	subzone->Height = Height;
}

// FUNCTION: GAMEMD 0x0047DD70
void CellClass::Destroy_Bridge(void)
{
	if (!Debug_Map) {
		ObjectClass * occupier = Cell_Occupier();
		while (occupier != NULL) {
			ObjectClass * next = occupier->Next;
			occupier->Take_Damage(occupier->Strength, 0, Rule->C4Warhead, NULL, true, true);
			occupier = next;
		}

		occupier = Cell_Occupier(true);
		while (occupier != NULL) {
			ObjectClass * next = occupier->Next;
			occupier->Fall_From_Height();
			occupier = next;
		}

		Map.PendingBridgeCells.Add(CellID);

		if (Rule->BridgeExplosions.Count() > 0 && Random_Double(0.0, 1.0) < 0.95) {
			Coord coord = CellID;
			coord.Z = LEVEL_LEPTON_H * Height + BRIDGE_LEPTON_HEIGHT;
			coord.X += Random_Double(-0.5, 0.5) * 50;
			coord.Y += Random_Double(-0.5, 0.5) * 50;

			if (Random_Double(0.0, 1.0) < 0.5) {
				new AnimClass(Rule->MetallicDebris[Random_Pick(0, Rule->MetallicDebris.Count() - 1)], coord);
			}

			new AnimClass(Rule->BridgeExplosions[Random_Pick(0, Rule->BridgeExplosions.Count() - 1)], coord, Random_Pick(1, 5));
		}
	}
}

static inline void Set_Bridge_Deck_Flags(CellClass & cell, bool state, bool east_west)
{
	cell.IsBridgeDeck = state;
	cell.IsUnderBridge = state;
	cell.IsBridgeTraversable = state;
	cell.IsBridgeSurface = state;
	cell.IsOvershadowed = state;
	cell.WasUnderBridge = !state;
	cell.IsBridgeEastWest = east_west;
}

// FUNCTION: GAMEMD 0x0047E040
void CellClass::Set_Under_Bridge(FacingType facing, bool state)
{
	if (facing == FACING_N) {
		OverlayData = OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		OverlayData = OVERLAYDATA_BRIDGE_NS_FULL1;
	}
	bool east_west = facing == FACING_N;
	Set_Bridge_Deck_Flags(*this, state, facing == FACING_N);
	if (state) {
		OverlayData = (facing != FACING_N) ? OVERLAYDATA_BRIDGE_NS_FULL1 : OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		OverlayData = 0;
		Destroy_Bridge();
	}
	Map.Radar_Background(CellID);
	CellClass * across = &Adjacent_Cell(facing);
	across->BridgeDeckCell = state ? this : NULL;
	across->IsUnderBridge = state;
	across->IsBridgeTraversable = state;
	across->IsBridgeSurface = state;
	across->IsOvershadowed = state;
	across->WasUnderBridge = !state;
	across->IsBridgeEastWest = east_west;
	if (state) {
		across->OverlayData = (facing != FACING_N) ? OVERLAYDATA_BRIDGE_NS_FULL1 : OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		across->OverlayData = 0;
		across->Destroy_Bridge();
	}
	Map.Radar_Background(across->CellID);
	across = &across->Adjacent_Cell(facing);
	across->BridgeDeckCell = state ? this : NULL;
	across->IsUnderBridge = state;
	across->IsBridgeTraversable = false;
	across->IsBridgeSurface = state;
	across->IsOvershadowed = state;
	across->WasUnderBridge = !state;
	across->IsBridgeEastWest = east_west;
	if (state) {
		across->OverlayData = (facing != FACING_N) ? OVERLAYDATA_BRIDGE_NS_FULL1 : OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		across->OverlayData = 0;
		across->Destroy_Bridge();
	}
	Map.Radar_Background(across->CellID);
	across->Adjacent_Cell(facing).IsBridgeSurface = state;
	CellClass * back = &Adjacent_Cell(Facing_Sub(facing, FACING_180));
	back->BridgeDeckCell = state ? this : NULL;
	back->IsUnderBridge = state;
	back->IsBridgeTraversable = state;
	back->WasUnderBridge = !state;
	back->IsBridgeSurface = false;
	back->IsOvershadowed = state;
	back->IsBridgeEastWest = east_west;
	if (state) {
		back->OverlayData = (facing != FACING_N) ? OVERLAYDATA_BRIDGE_NS_FULL1 : OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		back->OverlayData = 0;
		back->Destroy_Bridge();
	}
	Map.Radar_Background(back->CellID);
	if (facing == FACING_W) {
		CellClass & far_back = back->Adjacent_Cell(FACING_E);
		far_back.BridgeDeckCell = state ? this : NULL;
		far_back.IsOvershadowed = state;
	}
}

// FUNCTION: GAMEMD 0x0047E470
void CellClass::Set_Under_Rail_Bridge(FacingType facing, bool state)
{
	if (facing == FACING_N) {
		OverlayData = OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		OverlayData = OVERLAYDATA_BRIDGE_NS_FULL1;
	}
	bool east_west = facing == FACING_N;
	Set_Bridge_Deck_Flags(*this, state, facing == FACING_N);
	if (state) {
		OverlayData = (facing != FACING_N) ? OVERLAYDATA_BRIDGE_NS_FULL1 : OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		OverlayData = 0;
		Destroy_Bridge();
	}
	Map.Radar_Background(CellID);
	CellClass * across = &Adjacent_Cell(facing);
	across->BridgeDeckCell = state ? this : NULL;
	across->IsUnderBridge = state;
	across->IsBridgeTraversable = state;
	across->IsBridgeSurface = state;
	across->IsOvershadowed = state;
	across->WasUnderBridge = !state;
	across->IsBridgeEastWest = east_west;
	if (state) {
		across->OverlayData = (facing != FACING_N) ? OVERLAYDATA_BRIDGE_NS_FULL1 : OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		across->OverlayData = 0;
		across->Destroy_Bridge();
	}
	Map.Radar_Background(across->CellID);
	across = &across->Adjacent_Cell(facing);
	across->BridgeDeckCell = state ? this : NULL;
	across->IsUnderBridge = state;
	across->IsBridgeTraversable = false;
	across->IsBridgeSurface = state;
	across->IsOvershadowed = state;
	across->WasUnderBridge = !state;
	across->IsBridgeEastWest = east_west;
	if (state) {
		across->OverlayData = (facing != FACING_N) ? OVERLAYDATA_BRIDGE_NS_FULL1 : OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		across->OverlayData = 0;
		across->Destroy_Bridge();
	}
	Map.Radar_Background(across->CellID);
	across->Adjacent_Cell(facing).IsBridgeSurface = state;
	CellClass * back = &Adjacent_Cell(Facing_Sub(facing, FACING_180));
	back->BridgeDeckCell = state ? this : NULL;
	back->IsUnderBridge = state;
	back->IsBridgeTraversable = state;
	back->WasUnderBridge = !state;
	back->IsBridgeSurface = false;
	back->IsOvershadowed = state;
	back->IsBridgeEastWest = east_west;
	if (state) {
		back->OverlayData = (facing != FACING_N) ? OVERLAYDATA_BRIDGE_NS_FULL1 : OVERLAYDATA_BRIDGE_EW_FULL1;
	} else {
		back->OverlayData = 0;
		back->Destroy_Bridge();
	}
	Map.Radar_Background(back->CellID);
	if (facing == FACING_W) {
		CellClass & far_back = back->Adjacent_Cell(FACING_E);
		far_back.BridgeDeckCell = state ? this : NULL;
		far_back.IsOvershadowed = state;
	}
}

// FUNCTION: GAMEMD 0x0047E8A0
void CellClass::Occupy_Down(ObjectClass * object, bool bridge)
{
	ObjectClass * optr = Cell_Occupier(bridge);

	if (object == NULL) return;

	if (object->RTTI == RTTI_BUILDING && optr) {
		while (optr->Next != NULL) {
			optr = optr->Next;
		}
		optr->Next = object;
		object->Next = NULL;
	} else if (bridge) {
		if (optr == NULL || optr->Next != object) {
			object->Next = optr;
			BridgeOccupierPtr = object;
		}
	} else {
		if (optr == NULL || optr->Next != object) {
			object->Next = optr;
			OccupierPtr = object;
		}
	}

	if (!Map.Is_Shrouded(Cell_Coord()) && !Map.Is_Fogged(Cell_Coord()) || Session.Type != GAME_NORMAL) {
		object->Revealed(PlayerPtr);
	}

	if (object->RTTI != RTTI_INFANTRY && object->Occupies_Cells()) {
		if (object->RTTI == RTTI_BUILDING) {
			object->Set_Occupy_Bit(CellID);
		} else {
			object->Set_Occupy_Bit(object->Get_Coord());
		}
	}
}

// FUNCTION: GAMEMD 0x0047EA90
void CellClass::Occupy_Up(ObjectClass * object, bool bridge)
{
	if (object == NULL) return;

	ObjectClass * optr = Cell_Occupier(bridge);

	if (optr == object) {
		if (bridge) {
			BridgeOccupierPtr = object->Next;
		} else {
			OccupierPtr = object->Next;
		}
		object->Next = NULL;
	} else {
		while (optr != NULL) {
			if (optr->Next == object) {
				optr->Next = object->Next;
				object->Next = NULL;
				break;
			}
			optr = optr->Next;
		}
	}

	if (object->RTTI != RTTI_INFANTRY && object->Occupies_Cells()) {
		if (object->RTTI == RTTI_BUILDING) {
			object->Clear_Occupy_Bit(CellID);
		} else {
			object->Clear_Occupy_Bit(object->Get_Coord());
		}
	}
}

// FUNCTION: GAMEMD 0x0047EBA0
UnitClass * CellClass::Cell_Unit(bool bridge) const
{
	return((UnitClass*)Cell_Find_Object(RTTI_UNIT, bridge));
}

// FUNCTION: GAMEMD 0x0047EBF0
AircraftClass * CellClass::Cell_Aircraft(bool bridge) const
{
	return((AircraftClass*)Cell_Find_Object(RTTI_AIRCRAFT, bridge));
}

// FUNCTION: GAMEMD 0x0047EC40
InfantryClass * CellClass::Cell_Infantry(bool bridge) const
{
	return((InfantryClass*)Cell_Find_Object(RTTI_INFANTRY, bridge));
}

struct FileSystem {
	static ConvertClass * PALETTE_PAL;
	static ConvertClass * x_PAL;
	static void const * LoadFile(char const * name, bool flag);
};

class SHPStruct
{
	public:
		short Type;
		short Width;
		short Height;
		short Count;
		void Load(void);
		Rect GetFrameBounds(int frame) const;
		unsigned char * GetPixels(int frame) const;
};

// FUNCTION: GAMEMD 0x0047EC90
bool CellClass::Draw_Placement_Cursor(Point2D const & xpoint, Rect const & cliprect, bool zeroalpha)
{
	Point2D point = Point2D(30, -1);

	bool ok = false;
	if (IsCursorHere || IsFoundationCopyHere) {
		ObjectTypeClass const * pending = IsCursorHere ? Map.PendingObject : Map.CurrentBuildingTypeCopy;
		SpeedType loco = SPEED_NONE;
		if (pending != NULL && pending->RTTI == RTTI_BUILDINGTYPE) {
			loco = ((BuildingTypeClass const *)pending)->Speed;
		}

		int lift = 15 * Height;
		point -= Point2D(0, lift);
		int height_offset = -2 - lift;
		if (Ramp) {
			height_offset -= 10;
		}

		if (IsCursorHere && Map.PendingObject->RTTI == RTTI_BUILDINGTYPE) {
			Coord coord = Coord(CellID);
			coord.Z = Map.Get_Height_GL(coord);

			if (Map.Is_Shrouded(coord)) {
				if (Map.ProximityCheck && Map.PendingObject != NULL
						&& Map.PendingObject->RTTI == RTTI_BUILDINGTYPE
						&& ((BuildingTypeClass *)Map.PendingObject)->ToTile != NULL) {
					ok = true;
				} else {
					ok = false;
				}
			} else if (((BuildingTypeClass *)Map.PendingObject)->PowersUpBuilding[0] != '\0') {
				BuildingClass * building = (BuildingClass *)Cell_Object();
				if (building != NULL && building->RTTI == RTTI_BUILDING) {
					BuildingTypeClass * ptype = (BuildingTypeClass *)Map.PendingObject;
					if (building->Can_Upgrade(ptype, PlayerPtr)) {
						ok = true;
					}
				}
			} else {
				if (Map.ProximityCheck && Map.PendingObject != NULL
						&& Map.PendingObject->RTTI == RTTI_BUILDINGTYPE
						&& Is_Clear_To_Build(loco, (BuildingTypeClass *)Map.PendingObject, PlayerPtr)
						&& !(((BuildingTypeClass *)Map.PendingObject)->ToTile != NULL && Land == LAND_ROAD)) {
					ok = true;
				}
			}
		}

		ShapeFlags_Type flags = ShapeFlags_Type(zeroalpha ? 0x10600 : 0x20600);

		if (IsFoundationCopyHere) {
			Draw_Shape(*DSurface::Temp, *FileSystem::PALETTE_PAL, (ShapeSet *)DisplayClass::PlacementShapes,
				1, point + xpoint, cliprect, flags, 0, height_offset);
		} else if (ok) {
			Draw_Shape(*DSurface::Temp, *FileSystem::PALETTE_PAL, (ShapeSet *)DisplayClass::PlacementShapes,
				0, point + xpoint, cliprect, flags, 0, height_offset);
		} else {
			Draw_Shape(*DSurface::Temp, *FileSystem::PALETTE_PAL, (ShapeSet *)DisplayClass::PlacementShapes,
				Ramp + 2, point + xpoint, cliprect, flags, 0, height_offset);
		}
	}

	return(ok);
}

// FUNCTION: GAMEMD 0x0047F250
void CellClass::Draw_Fog_Shape(Point2D const & drawpoint, Rect const & cliprect, int shapenum)
{
	static bool shapes_loaded = false;
	static SHPStruct const * shapes = NULL;

	if (!shapes_loaded) {
		shapes_loaded = true;
		shapes = (SHPStruct const *)FileSystem::LoadFile("FOG.SHP", false);
	}

	Rect shaperect = shapes->GetFrameBounds(shapenum);

	if (drawpoint.Y + shaperect.Y + shaperect.Height < cliprect.Y + 1) return;
	if (drawpoint.Y >= cliprect.Height + cliprect.Y) return;
	if (drawpoint.X + shaperect.X + shaperect.Width < cliprect.X + 1) return;
	if (drawpoint.X >= cliprect.X + cliprect.Width) return;

	int inter_top = MAX(cliprect.Y, drawpoint.Y + shaperect.Y);
	int src_y = inter_top - drawpoint.Y - shaperect.Y;
	int inter_bottom = MIN(cliprect.Y + cliprect.Height, drawpoint.Y + shaperect.Y + shaperect.Height);

	int inter_left = MAX(cliprect.X, drawpoint.X + shaperect.X);
	int src_x = inter_left - drawpoint.X - shaperect.X;
	int inter_right = MIN(cliprect.X + cliprect.Width, drawpoint.X + shaperect.X + shaperect.Width);

	int shape_skip = drawpoint.X - inter_right + shaperect.X + shaperect.Width + src_x;
	int alpha_skip = inter_left - inter_right + AlphaBuffer->Get_Buffer_Width();

	unsigned char * shapeptr = shapes->GetPixels(shapenum);
	unsigned short * alphaptr = (unsigned short *)AlphaBuffer->Get_Buffer_Offset(Point2D(inter_left, inter_top - TacticalRect.Y));

	shapeptr += src_x + src_y * shaperect.Width;
	if (&alphaptr[inter_right - inter_left + (inter_bottom - inter_top) * AlphaBuffer->Get_Buffer_Width() + 2] >= (unsigned short *)AlphaBuffer->Get_Buffer_End()) {
		for (int i = inter_top; i < inter_bottom; i++) {
			for (int j = inter_left; j < inter_right; j++) {
				unsigned char pixel = *shapeptr++;
				if (pixel <= 0x7F) {
					if (*alphaptr == 127) {
						*alphaptr = pixel;
					} else {
						*alphaptr = MAX(*alphaptr + pixel - 127, 0);
					}
				}
				alphaptr++;
				alphaptr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned)alphaptr);
			}
			shapeptr += shape_skip;
			alphaptr += alpha_skip;
			alphaptr = (unsigned short *)AlphaBuffer->Wrap_Overflow((unsigned)alphaptr);
		}
	} else {
		for (int i = inter_top; i < inter_bottom; i++) {
			for (int j = inter_left; j < inter_right; j++) {
				unsigned char pixel = *shapeptr++;
				if (pixel <= 0x7F) {
					if (*alphaptr == 127) {
						*alphaptr = pixel;
					} else {
						*alphaptr = MAX(*alphaptr + pixel - 127, 0);
					}
				}
				++alphaptr;
			}
			shapeptr += shape_skip;
			alphaptr += alpha_skip;
		}
	}
}

// FUNCTION: GAMEMD 0x0047F510
void CellClass::Draw_Overlay_Shadow(Point2D const & xpoint, Rect const & cliprect)
{
	int yoffset = 15 * Height;
	OverlayTypeClass * otype = OverlayTypes[Overlay];
	ShapeSet const * shape = (ShapeSet const *)otype->Get_Image_Data();

	Point2D point = xpoint + Overlay_Draw_Offset();
	point -= cliprect.Top_Left();

	if (IsBridgeDeck) {
		if (OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1 && OverlayData <= OVERLAYDATA_BRIDGE_NS_END2) {
			point += Point2D(-15, 7);
		}
	}
	if (Drawer == NULL) {
		Init_Drawer();
	}

	unsigned char frame = OverlayData;
	if (otype->IsCrate) {
		frame = 0;
	} else if (otype->IsRubble) {
		ShapeSet const * rshape;
		int rframe;
		if (Rubble == NULL) return;
		if (!Rubble->FUN_0045F1D0(&rshape, &rframe)) return;
		Draw_Shape(*LogicalSurface, *Drawer, rshape, rframe, point, cliprect,
			ShapeFlags_Type(0x4601), NULL, -2 - yoffset);
		return;
	}
	Draw_Shape(*LogicalSurface, *Drawer, shape, frame + shape->Get_Count() / 2, point, cliprect,
		ShapeFlags_Type(0x4601), NULL, -2 - yoffset);
}

// FUNCTION: GAMEMD 0x0047F6A0
void CellClass::Draw_Overlay(Point2D const & xpoint, Rect const & cliprect)
{
	ShapeFlags_Type const flags = ShapeFlags_Type(SHAPE_CENTER | SHAPE_WIN_REL | SHAPE_ALPHA | SHAPE_ZWRITE);
	if (Overlay == OVERLAY_VEINHOLE || Overlay == OVERLAY_VEINHOLE_DUMMY) {
		return;
	}

	OverlayTypeClass * otype = OverlayTypes[Overlay];

	Point2D point = Overlay_Draw_Offset() + xpoint;
	int yoffset = 15 * (Height + (IsBridgeDeck ? BRIDGE_CELL_HEIGHT : 0));
	point -= cliprect.Top_Left();

	if (Drawer == NULL) {
		Init_Drawer();
	}

	ShapeSet const * shape = (ShapeSet const *)otype->Get_Image_Data();

	if (IsBridgeDeck) {
		if (LastBridgeDrawFrame != Frame || LastBridgeDrawRedraws != Map.Redraws || LastBridgeDrawRect != cliprect) {
			int shapenum = OverlayData;
			if (shapenum == 0 || shapenum == 9) {
				// Vary intact bridge frames over a four-by-four cell pattern.
				static int _bridge_variation[16] = {
					0, 1, 2, 3,
					3, 2, 1, 0,
					2, 3, 0, 1,
					1, 0, 3, 2
				};
				shapenum += _bridge_variation[(CellID.X & 3) | (4 * (CellID.Y & 3))];
			}

			Draw_Shape(*DSurface::Temp, *Drawer, shape, shapenum, point, cliprect, flags, 0, -2 - yoffset, ZGRAD_GROUND, AltBrightness);
			LastBridgeDrawFrame = Frame;
			LastBridgeDrawRect = TacticalRect;
			LastBridgeDrawRedraws = Map.Redraws;
		}
		return;
	}

	if (otype->IsTiberium) {
		TiberiumType tibtype = Which_Tiberium_Type(Overlay);
		if (tibtype == TIBERIUM_NONE) {
			return;
		}

		TiberiumClass * tiberium = Tiberiums[tibtype];

		OverlayTypeClass * overlaytodisplay = NULL;
		if (Ramp != RAMP_NONE) {
			overlaytodisplay = OverlayTypes[tiberium->Overlay->HeapID + tiberium->Variety + tiberium->RampVariety / 4 * (Ramp - 1) + CellID.X * CellID.Y % (tiberium->RampVariety / 4)];
		} else {
			overlaytodisplay = OverlayTypes[tiberium->Overlay->HeapID + ((CellID.Y * CellID.X) % tiberium->Variety)];
		}

		ShapeSet const * tibshape = (ShapeSet const *)overlaytodisplay->Get_Image_Data();

		if (Ramp != RAMP_NONE) {
			Draw_Shape(*DSurface::Temp, *FileSystem::x_PAL, tibshape, OverlayData, point, cliprect, flags, 0, -2 - yoffset, ZGRAD_GROUND, NORMAL_LIGHT, 0, SlopeZShapes[Ramp - 1]);
		} else {
			Draw_Shape(*DSurface::Temp, *FileSystem::x_PAL, tibshape, OverlayData, point, cliprect, flags, 0, -2 - yoffset, ZGRAD_GROUND, NORMAL_LIGHT);
		}
	} else if (otype->IsWall) {
		Draw_Shape(*DSurface::Temp, *ColorSchemes[PlayerPtr->Scheme]->Converter, shape, OverlayData, point, cliprect, flags, 0, -2 - yoffset, ZGRAD_90DEG, Brightness);
	} else if (otype->HeapID == OVERLAY_VEINS) {
		if (Ramp != RAMP_NONE) {
			Draw_Shape(*DSurface::Temp, *FileSystem::x_PAL, shape, OverlayData, point, cliprect, flags, 0, -2 - yoffset, ZGRAD_GROUND, TileBrightness, 0, SlopeZShapes[Ramp - 1], 0, Point2D(30, -2));
		} else {
			Draw_Shape(*DSurface::Temp, *FileSystem::x_PAL, shape, OverlayData, point, cliprect, flags, 0, -2 - yoffset, ZGRAD_GROUND, TileBrightness);
		}
	} else {
		bool flat = otype->IsDrawFlat;
		int offsetY = flat ? 0 : -15;
		if (otype->IsARock) {
			offsetY = 0;
		}
		if (otype->IsCrate) {
			Draw_Shape(*DSurface::Temp, *Drawer, shape, 0, point, cliprect, flags, 0, offsetY - yoffset - 2, flat ? ZGRAD_GROUND : ZGRAD_90DEG, TileBrightness);
		} else if (otype->IsRubble) {
			ShapeSet const * rubble_shape;
			int frame;
			if (!Rubble || !Rubble->FUN_0045F160(&rubble_shape, &frame)) return;
			Draw_Shape(*DSurface::Temp, *Drawer, rubble_shape, frame, point, cliprect, flags, 0, offsetY - yoffset - 2, flat ? ZGRAD_GROUND : ZGRAD_90DEG, TileBrightness);
		} else {
			Draw_Shape(*DSurface::Temp, *Drawer, shape, OverlayData, point, cliprect, flags, 0, offsetY - yoffset - 2, flat ? ZGRAD_GROUND : ZGRAD_90DEG, TileBrightness);
		}
	}
}

// FUNCTION: GAMEMD 0x0047FB90
Rect CellClass::Overlay_Render_Rect(void) const
{
	if (Overlay == OVERLAY_NONE) {
		return(Rect(0, 0, 0, 0));
	}

	int frame = OverlayData;
	OverlayTypeClass * overlay = OverlayTypes[Overlay];
	ShapeSet const * image = NULL;

	if (overlay->IsTiberium) {
		TiberiumType tib = Which_Tiberium_Type(Overlay);
		if (tib != TIBERIUM_NONE) {
			TiberiumClass * tiberium = Tiberiums[tib];
			if (Ramp != 0) {
				OverlayTypeClass * tib_overlay = OverlayTypes[tiberium->Variety + tiberium->Overlay->HeapID + tiberium->RampVariety / 4 * (Ramp - 1) + CellID.X * CellID.Y % (tiberium->RampVariety / 4)];
				image = (ShapeSet const *)tib_overlay->Get_Image_Data();
			} else {
				OverlayTypeClass * tib_overlay = OverlayTypes[tiberium->Overlay->HeapID + CellID.X * CellID.Y % tiberium->Variety];
				image = (ShapeSet const *)tib_overlay->Get_Image_Data();
			}
		}
	} else if (overlay->IsRubble) {
		image = NULL;
		if (Rubble) {
			Rubble->FUN_0045F160(&image, &frame);
		}
	} else {
		image = (ShapeSet const *)overlay->Get_Image_Data();
	}

	Point2D point;
	TacticalMap->Coord_To_Pixel(Coord_Whole(CellID), point);

	point += Overlay_Draw_Offset();
	point.X -= 30;

	if (image == NULL) {
		return(Rect(0, 0, 0, 0));
	}

	((SHPStruct *)image)->Load();
	if (overlay->IsCrate) {
		frame = 0;
	}

	SHPStruct const * shp = (SHPStruct const *)image;
	return(shp->GetFrameBounds(frame) + (point - Point2D(shp->Width / 2, shp->Height / 2)));
}

// FUNCTION: GAMEMD 0x0047FDE0
Rect CellClass::Overlay_Shadow_Render_Rect(void) const
{
	SHPStruct const * image = NULL;
	OverlayTypeClass * overlay = NULL;

	if (Overlay != OVERLAY_NONE) {
		overlay = OverlayTypes[Overlay];
		image = (SHPStruct const *)overlay->Get_Image_Data();
	}

	if (!image) {
		return(Rect(0, 0, 0, 0));
	}

	unsigned char frame = OverlayData;
	if (overlay->IsCrate) {
		frame = 0;
	}
	int shadowframe = image->Count / 2 + frame;

	Point2D point;
	TacticalMap->Coord_To_Pixel(Coord_Whole(CellID), point);

	point += Overlay_Draw_Offset();
	point.X -= 30;

	if (IsBridgeDeck) {
		if (OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1 && OverlayData <= OVERLAYDATA_BRIDGE_NS_END2) {
			point += Point2D(-15, 7);
		}
	}

	Point2D d0 = point + point;
	Point2D d1 = point + point;
	return(image->GetFrameBounds(shadowframe) + (point - Point2D(image->Width / 2, image->Height / 2)));
}

// FUNCTION: GAMEMD 0x0047FF80
Rect CellClass::Cell_Render_Rect(void) const
{
	Point2D point;
	TacticalMap->Coord_To_Pixel(Coord_Whole(CellID), point);
	point.X += -30;
	point.Y += DSurface::ViewBounds.Y - 15 * Height;

	Rect rect;
	rect.X = point.X;
	rect.Y = point.Y;
	rect.Width = 60;
	rect.Height = 30;

	if (ITType == ISOTILE_NONE) {
		return(rect);
	}

	if (ITType >= ISOTILE_FIRST && ITType < IsometricTileTypes.Count()) {
		IsometricTileTypeClass * itype = IsometricTileTypes[ITType];
		int subtile = SubTile;
		IsoTileSet const * image = (IsoTileSet const *)itype->Get_Image_Data();
		if (image == NULL) {
			return(Rect());
		}
		subtile = MIN(subtile, image->Tile_Count() - 1);
		IsoTileRecord const * tile = image->Fetch_Record_Pointer_Unsafe(subtile);
		if (tile->IsHasExtraData) {
			int dy = MAX(0, tile->Y - tile->ExtraY);
			rect.Y -= dy;
			rect.Height += dy;
		}
		return(rect);
	} else {
		FUN_004068E0("Invalid tile at (%d, %d)\n", CellID.X, CellID.Y);
		return(rect);
	}
}

// FUNCTION: GAMEMD 0x00480110
Point2D CellClass::Overlay_Draw_Offset(void) const
{
	Point2D offset = ::Overlay_Draw_Offset(Overlay);
	if (IsBridgeDeck) {
		offset.Y -= 16;
		if (OverlayData >= OVERLAYDATA_BRIDGE_NS_FULL1 && OverlayData <= OVERLAYDATA_BRIDGE_NS_END2) {
			offset.Y -= 15;
		}
	} else if (Overlay == 0xEF) {
		offset.Y -= 15;
	}
	offset.Y += DSurface::ViewBounds.Y - 15 * Height;
	offset += Point2D(30, 15);
	Point2D doubled = offset + offset;
	return(offset);
}

// FUNCTION: GAMEMD 0x00480180
void CellClass::Wipe_Depth(Point2D const & point, Rect const & cliprect)
{
	IsometricTileTypes[ISOTILE_CLEAR]->Draw_Tile(TileDrawers[0], 0, *DSurface::Temp, point.X, point.Y, cliprect, Height, TileBrightness, 1, 0, false, true, false, false);
}

// FUNCTION: GAMEMD 0x004801F0
void CellClass::Draw_Shroud_And_Fog(Point2D const & point, Rect const & cliprect)
{
	ShadowFrame = TacticalMap->Cell_Shadow(CellID, false);

	int shadow_frame = ShadowFrame;
	if (shadow_frame == -2) {
		shadow_frame = 15;
	} else if (shadow_frame == -1) {
		shadow_frame = 0;
	}

	Draw_Shroud_Or_Fog_Shape(point, cliprect, shadow_frame);

	FogFrame = TacticalMap->Cell_Shadow(CellID, true);

	if (!Scen->Special.IsFogOfWar || PlayerPtr->IsDefeated) {
		return;
	}

	int fog_frame = FogFrame;
	if (fog_frame == -2) {
		fog_frame = 15;
	} else if (fog_frame == -1) {
		fog_frame = 0;
	}

	Draw_Fog_Shape(point, cliprect, fog_frame);
}

inline void CellClass::Fetch_Icon(IsometricTileTypeClass *& ittype, int & subtile, int * icon, bool doicon) const
{
	if (ITType != ISOTILE_NONE) {
		ittype = IsometricTileTypes[ITType];
		subtile = SubTile;
		if (ittype->NumTileTypesInSet > 1) {
			if (ittype->Is_Randomized(SubTile)) {
				if (doicon) *icon = IsBridgeDamaged;
			} else {
				if (doicon) *icon = Clear_Icon(ITType, ittype->NumTileTypesInSet);
			}
		}
	} else {
		subtile = 0;
		ittype = IsometricTileTypes[TILE_CLEAR];
		if (doicon) *icon = Clear_Icon(TILE_CLEAR, ittype->NumTileTypesInSet);
	}
}

// FUNCTION: GAMEMD 0x004802A0
void CellClass::Draw_Shadow_Cast(Point2D const & point, Rect const & cliprect)
{
	int subtile;
	IsometricTileTypeClass * isotype;

	Fetch_Icon(isotype, subtile, NULL, false);
	if (isotype->IsShadowCaster) {
		isotype->Draw_Shadow_Caster(subtile, DSurface::Temp, Point2D(point.X, point.Y - 15 * Height), cliprect, -2 - 15 * (Height - 4));
	}
}

// FUNCTION: GAMEMD 0x00480510
bool CellClass::Has_Wall_Or_Gate(OverlayType type, FacingType facing) const
{
	if (Overlay == type && Overlay != OVERLAY_NONE) {
		return(true);
	}
	if (type == OVERLAY_NONE) {
		if (Overlay == OVERLAY_GDI_WALL || Overlay == OVERLAY_NOD_WALL || (int)Overlay == 0xF3) {
			return(true);
		}
		return(false);
	}

	if (type == OVERLAY_GDI_WALL || type == OVERLAY_SANDBAG_WALL) {
		BuildingClass * obj = (BuildingClass *)OccupierPtr;
		while (obj != NULL) {
			if (obj->Fetch_RTTI() == RTTI_BUILDING) {
				BuildingTypeClass * btype = obj->Class;
				if (obj->Strength > 0) {
					if ((btype == Rule->GDIGateOne && (facing == FACING_E || facing == FACING_W)) ||
						(btype == Rule->GDIGateTwo && (facing == FACING_N || facing == FACING_S)) ||
						(btype == Rule->WallTower)) {
						return(true);
					}
				}
			}
			obj = (BuildingClass *)obj->Next;
		}
	}

	if (type == OVERLAY_NOD_WALL) {
		BuildingClass * obj = (BuildingClass *)OccupierPtr;
		while (obj != NULL) {
			if (obj->Fetch_RTTI() == RTTI_BUILDING) {
				BuildingTypeClass * btype = obj->Class;
				if (obj->Strength > 0) {
					if ((btype == Rule->NodGateOne && (facing == FACING_E || facing == FACING_W)) ||
						(btype == Rule->NodGateTwo && (facing == FACING_N || facing == FACING_S))) {
						return(true);
					}
				}
			}
			obj = (BuildingClass *)obj->Next;
		}
	}

	return(false);
}

static inline int Wall_Frame(CellClass & cell, FacingType const * offsets)
{
	int icon = 0;
	for (unsigned i = 0; i < FACING_COUNT / 2; i++) {
		CellClass & adjacent = cell.Adjacent_Cell(offsets[i]);
		if (adjacent.Has_Wall_Or_Gate(cell.Overlay, offsets[i])) {
			icon |= 1 << i;
		}
	}
	return(icon);
}

// FUNCTION: GAMEMD 0x00480630
void CellClass::Wall_Update(bool justneighbors)
{
	static FacingType _offsets[FACING_COUNT / 2 + 1] = {FACING_N, FACING_E, FACING_S, FACING_W, FACING_NONE};

	for (unsigned index = 0; index < ARRAY_SIZE(_offsets); index++) {
		CellClass & newcell = Adjacent_Cell(_offsets[index]);
		Rect rect;
		if (!justneighbors || _offsets[index] != FACING_NONE) {
			rect = Union(newcell.Overlay_Render_Rect(), newcell.Overlay_Shadow_Render_Rect()) - TacticalRect.TopLeft;
		}
		newcell.Register_For_Redraw();

		if (newcell.Overlay != OVERLAY_NONE && OverlayTypes[newcell.Overlay]->IsWall) {
			int icon = Wall_Frame(newcell, _offsets);
			newcell.OverlayData = (newcell.OverlayData & 0xFFF0) | icon;

			bool recalc_around = false;
			PassabilityType old_passability = newcell.Passability;

			if ((newcell.Overlay == OVERLAY_BRICK_WALL || newcell.Overlay == OVERLAY_NOD_WALL) &&
				(newcell.OverlayData == 3 * OVERLAYDATA_WALL_DAMAGE_STAGE || newcell.OverlayData == 2 * OVERLAYDATA_WALL_DAMAGE_STAGE)) {
				newcell.Overlay = OVERLAY_NONE;
				newcell.OverlayData = 0;
				Detach_This_From_All(&newcell, true);
				recalc_around = true;
			}
			if (newcell.Overlay == OVERLAY_SANDBAG_WALL &&
				(newcell.OverlayData == OVERLAYDATA_WALL_DAMAGE_STAGE || newcell.OverlayData == 2 * OVERLAYDATA_WALL_DAMAGE_STAGE)) {
				newcell.Overlay = OVERLAY_NONE;
				newcell.OverlayData = 0;
				newcell.Owner = HOUSE_NONE;
				Detach_This_From_All(&newcell, true);
				recalc_around = true;
			}
			if (newcell.Overlay == OVERLAY_CYCLONE_WALL && newcell.OverlayData == 2 * OVERLAYDATA_WALL_DAMAGE_STAGE) {
				newcell.Overlay = OVERLAY_NONE;
				newcell.OverlayData = 0;
				newcell.Owner = HOUSE_NONE;
				Detach_This_From_All(&newcell, true);
				recalc_around = true;
			}
			if (newcell.Overlay == OVERLAY_FENCE &&
				(newcell.OverlayData == OVERLAYDATA_WALL_DAMAGE_STAGE || newcell.OverlayData == 2 * OVERLAYDATA_WALL_DAMAGE_STAGE)) {
				newcell.Overlay = OVERLAY_NONE;
				newcell.OverlayData = 0;
				newcell.Owner = HOUSE_NONE;
				Detach_This_From_All(&newcell, true);
				recalc_around = true;
			}
			if (newcell.Overlay == OVERLAY_BARBWIRE_WALL && newcell.OverlayData == OVERLAYDATA_WALL_DAMAGE_STAGE) {
				newcell.Overlay = OVERLAY_NONE;
				newcell.OverlayData = 0;
				newcell.Owner = HOUSE_NONE;
				Detach_This_From_All(&newcell, true);
				recalc_around = true;
			}

			newcell.Recalc_Attributes();

			if (newcell.Passability != old_passability) {
				if (recalc_around) {
					Map.ResetZones(newcell.CellID);
					Map.RecalculateSubZones(newcell.CellID);
					for (int dir = FACING_FIRST; (unsigned)dir < FACING_COUNT; dir++) {
						CellClass & adjacent = Map[::Adjacent_Cell(newcell.CellID, (FacingType)dir)];
						adjacent.AdjacentObjectCount--;
					}
				} else {
					Map.Update_Cell_Zone_Constructively(newcell.CellID);
					Map.RecalculateSubZones(newcell.CellID);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00480A30
Coord CellClass::Cell_Coord(void) const
{
	Coord coord(Fetch_CellID());
	coord.Z = Get_Height();
	return(coord);
}

// FUNCTION: GAMEMD 0x00480CB0
int CellClass::Reduce_Wall(int damage)
{
	if (Overlay != OVERLAY_NONE) {
		bool destroyed = false;
		OverlayTypeClass const * wall = OverlayTypes[Overlay];

		if (wall->IsWall) {
			if (damage == -1 || damage >= wall->DamagePoints || ScenarioInit) {
				destroyed = true;
			} else {
				destroyed = Random_Pick(0, wall->DamagePoints) < damage;
			}

			if (destroyed) {
				Rect rect = Union(Overlay_Render_Rect(), Overlay_Shadow_Render_Rect());
				rect.Y -= TacticalRect.Y;
				TacticalMap->Register_Dirty_Area(rect);
				OverlayData+=OVERLAYDATA_WALL_DAMAGE_STAGE;

				if ((OverlayData>>OVERLAYDATA_WALL_DAMAGE_SHIFT) == wall->DamageLevels-1 && wall->DamageLevels > 2) {
					for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir = FacingType(dir + FACING_90)) {
						CellClass * adjacent = (CellClass *)&((CellClass const *)this)->Adjacent_Cell(dir);
						if (adjacent->Overlay != OVERLAY_NONE && OverlayTypes[adjacent->Overlay]->IsWall && adjacent->Overlay == Overlay && adjacent->OverlayData < OVERLAYDATA_WALL_DAMAGE_STAGE) {
							adjacent->Reduce_Wall(200);
						}
					}
				}

				if (damage == -1 ||
					(OverlayData>>OVERLAYDATA_WALL_DAMAGE_SHIFT) >= wall->DamageLevels ||
					((OverlayData>>OVERLAYDATA_WALL_DAMAGE_SHIFT) == wall->DamageLevels-1 && (OverlayData & OVERLAYDATA_WALL_FRAME_MASK)==0)	) {

					Owner = HOUSE_NONE;
					Overlay = OVERLAY_NONE;
					OverlayData = 0;
					Recalc_Attributes();
					Map.ResetZones(CellID);
					Map.RecalculateSubZones(CellID);
					Map.Flag_Background_Update(CellID);
					Adjacent_Cell(FACING_N).Wall_Update();
					Adjacent_Cell(FACING_W).Wall_Update();
					Adjacent_Cell(FACING_S).Wall_Update();
					((CellClass &)((CellClass const *)this)->Adjacent_Cell(FACING_E)).Wall_Update();
					Detach_This_From_All(this);

					for (int dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
						((CellClass &)((CellClass const *)this)->Adjacent_Cell((FacingType)dir)).AdjacentObjectCount--;
					}
					return(true);
				}
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004810A0
int CellClass::Spot_Index(Coord const & coord)
{
	Point2D rel = Coord_Fraction(coord);

	if (Distance(rel, Point2D(CELL_LEPTON/2,CELL_LEPTON/2)) < 60) {
		return(0);
	}

	int index = 0;
	if (rel.X > CELL_LEPTON/2) index |= 0x01;
	if (rel.Y > CELL_LEPTON/2) index |= 0x02;
	if (index == 0) return(0);
	return(index+1);
}

// FUNCTION: GAMEMD 0x00481130
bool CellClass::Is_Spot_Free(int spot_index, bool bridge) const
{
	if (spot_index == 0 || spot_index == 1) {
		return(false);
	}

	if (bridge) {
		return(! (BridgeFlag.Composite & (1 << spot_index)) );
	}
	else {
		return(! (Flag.Composite & (1 << spot_index)) );
	}
}

// FUNCTION: GAMEMD 0x004814A0
BuildingClass * CellClass::Get_Gate(void) const
{
	BuildingClass * building = Cell_Building();
	if (building != NULL && !building->Class->IsGate) {
		building = NULL;
	}
	return(building);
}

// FUNCTION: GAMEMD 0x004814F0
int CellClass::Clear_Icon(IsometricTileType type, int seq_length) const
{
	static bool _init = false;
	static int _small_pattern[16] = { 0, 1, 2, 3, 3, 2, 1, 0, 2, 3, 0, 1, 1, 0, 3, 2 };
	static int _large_pattern[64];
	static Point2DStruct _adjacent_icon[8] = {
		{-1,-1}, {0,-1}, {1,-1}, {-1,0}, {1,0}, {-1,1}, {0,1}, {1,1}
	};

	if (!_init) {
		_init = true;
		memset(_large_pattern, 0xFF, sizeof(_large_pattern));

		for (int i = 0; i < ARRAY_SIZE(_large_pattern); ++i) {
			int random = -1;
			for (int j = 0; j < ARRAY_SIZE(_large_pattern); j++) {
				random = NonCriticalRandomNumber() & 7;
				bool duplicate = false;

				for (int k = 0; k < ARRAY_SIZE(_adjacent_icon); ++k) {
					int num = i + _adjacent_icon[k].X + _adjacent_icon[k].Y * 8;

					if (num < 0) num += 64;
					if (num >= 64) num -= 64;

					if (_large_pattern[num] == random) {
						duplicate = true;
						break;
					}
				}

				if (!duplicate) {
					break;
				}
			}
			_large_pattern[i] = random;
		}
	}

	Cell cell = Fetch_CellID();
	if (SubTile != 0) {
		IsometricTileTypeClass const * isotype = IsometricTileTypes[type];
		if (isotype->Get_Image_Data() == NULL) {
			return(0);
		}
		Point2D p = IsoTile_Dimensions(isotype);
		cell = cell - Cell(SubTile % p.X, SubTile / p.X);
		cell.X /= p.X;
		cell.Y /= p.Y;
	}
	if (seq_length <= 4) {
		return(_small_pattern[(cell.X & 0x03) | ((cell.Y & 0x03) << 2)]);
	} else {
		return(_large_pattern[(cell.X & 0x07) + ((cell.Y & 0x07) << 3)]);
	}
}

// FUNCTION: GAMEMD 0x00481670
void CellClass::Incoming(Coord const & threat, bool forced, bool nokidding, bool bridge)
{
	ObjectClass * object = NULL;
	TechnoClass * techno = NULL;

	object = Cell_Occupier(bridge);
	bool elite = false;
	if (!nokidding) {
		while (object != NULL) {
			techno = Dynamic_Cast<TechnoClass *>(object);
			if (techno != NULL && techno->Veterancy.Is_Elite()) {
				elite = true;
				break;
			}
			object = object->Next;
		}
	}

	object = Cell_Occupier(bridge);

	DynamicVectorClass<ObjectClass *> objs;
	while (object != NULL) {
		objs.Add(object);
		object = object->Next;
	}

	for (int i = 0; i < objs.Count(); i++) {
		techno = Dynamic_Cast<TechnoClass *>(objs[i]);
		if (elite || nokidding || Rule->IsScatter ||
			(techno != NULL && ((techno->Has_Ability(ABILITY_SCATTER) || techno->House->IQ >= Rule->IQScatter)))) {
			objs[i]->Scatter(threat, forced, nokidding);
		}
	}
}

// FUNCTION: GAMEMD 0x00481810
CellClass const & CellClass::Adjacent_Cell(FacingType face) const
{
	if ((unsigned)face >= FACING_COUNT) {
		return(*this);
	}

	Cell newcell = ::Adjacent_Cell(Fetch_CellID(), face);
	return(Map[newcell]);
}

// FUNCTION: GAMEMD 0x00481870
void CellClass::Adjust_Threat(HousesType house, int threat_value)
{
	int region = Map.Cell_Region(Fetch_CellID());

	for (HousesType lp = HOUSE_FIRST; lp < Houses.Count(); lp ++) {
		HouseClass * house_ptr = Houses[lp];
		if (house_ptr && house_ptr->HeapID != house && !house_ptr->Is_Ally(house)) {
			house_ptr->Adjust_Threat(region, threat_value);
		}
	}
}

// FUNCTION: GAMEMD 0x004818E0
int CellClass::Tiberium_Adjust(bool pregame)
{
	if (Overlay != OVERLAY_NONE) {
		static int _adj[13] = {0,1,3,4,6,7,8,10,11,7,0,1,6};

		TiberiumType tibtype = Which_Tiberium_Type(Overlay);
		if (tibtype != TIBERIUM_NONE) {
			TiberiumClass * tiberium = Tiberiums[tibtype];
			int	count = 0;

			int value = tiberium->CreditValue;
			if (pregame) {
				Overlay = (OverlayType)Random_Pick((int)tiberium->Overlay, (int)&tiberium->Overlay[tiberium->Variety - 1]);
			}

			for (FacingType face = FACING_FIRST; face < FACING_COUNT; face++) {
				CellClass & adjc = Adjacent_Cell(face);

				if (Which_Tiberium_Type(adjc.Overlay) == tibtype) {
					count++;
				}
			}

			OverlayData = _adj[count % Tiberiums[tibtype]->FrameCount];
			return((OverlayData+1) * value);
		}
	}
	return(0);
}

static inline Coord Crate_Sound_Coord(CellClass const & cell)
{
	return(cell.Cell_Coord());
}

namespace Powerups {
	extern int Weights[CRATE_COUNT];
	extern AnimType Anims[CRATE_COUNT];
	extern bool Naval[CRATE_COUNT];
	extern double Arguments[CRATE_COUNT];
}

// FUNCTION: GAMEMD 0x00481A00
bool CellClass::Goodie_Check(FootClass * object)
{
	if (object != NULL && Overlay != OVERLAY_NONE && OverlayTypes[Overlay]->IsCrate) {
		bool force_mcv = false;
		int force_money = 0;
		int damage;
		Coord coord;

		if (Session.Type != GAME_NORMAL && object->House->Class->IsMultiplayPassive) {
			return(true);
		}

		if (OverlayTypes[Overlay]->IsCrateTrigger) {
			if (object->Tag != NULL) {
				FUN_004068E0("Springing trigger on crate at %d,%d\n", CellID.X, CellID.Y);
				object->Tag->Spring(TEVENT_PICKUP_CRATE, object);
				if (!object->IsActive) {
					return(false);
				}
			}
			Scen->IsCrateBeenPickedUp = true;
		}

		int index;
		CrateType powerup;
		if (OverlayData >= CRATE_COUNT) {
			int total_shares = 0;
			for (index = 0; index < CRATE_COUNT; ++index) {
				total_shares += Powerups::Weights[index];
			}
			int pick = Random_Pick(1, total_shares);
			int count = 0;
			for (index = CRATE_FIRST; index < CRATE_COUNT; ++index) {
				count += Powerups::Weights[index];
				if (pick <= count) break;
			}
			powerup = (CrateType)index;
		} else {
			powerup = (CrateType)OverlayData;
		}
		if (Session.Type == GAME_NORMAL) {
			if (OverlayData == 0) {
				force_money = Rule->SoloCrateMoney;
				if (OverlayTypes[Overlay] == Rule->CrateImg) powerup = Rule->SilverCrate;
				if (OverlayTypes[Overlay] == Rule->WoodCrateImg) powerup = Rule->WoodCrate;
				if (OverlayTypes[Overlay] == Rule->WaterCrateImg) powerup = Rule->WaterCrate;
			}
		} else {
			HouseClass * house = object->House;
			UnitTypeClass const * base_unit = house->Get_First_Ownable(Rule->BaseUnit);
			if (house->CurBuildings == 0 &&
					house->Available_Money() > 1500 &&
					house->UQuantity.Value(base_unit->HeapID) == 0 &&
					Session.Options.Bases) {
				powerup = CRATE_UNIT;
				force_mcv = true;
			}

			switch (powerup) {
				case CRATE_UNIT:
					if (object->House->CurUnits > 50) powerup = CRATE_MONEY;
					if (Land == LAND_WATER || Land == LAND_BEACH) powerup = CRATE_MONEY;
					break;
				case CRATE_SQUAD:
					if (object->House->CurInfantry > 100) powerup = CRATE_MONEY;
					if (Land == LAND_WATER || Land == LAND_BEACH) powerup = CRATE_MONEY;
					break;
				case CRATE_ARMOR:
					if (object->ArmorBias != 1) powerup = CRATE_MONEY;
					break;
				case CRATE_VETERAN:
					if ((object->AbstractFlags & 1) && !object->Techno_Type_Class()->IsTrainable) {
						powerup = CRATE_MONEY;
						break;
					}
					if ((object->AbstractFlags & 1) && object->Veterancy.Is_Elite()) powerup = CRATE_MONEY;
					break;

				case CRATE_SPEED:
					if (object->SpeedBias != 1 || object->Fetch_RTTI() == RTTI_AIRCRAFT) powerup = CRATE_MONEY;
					break;
				case CRATE_FIREPOWER:
					if (object->FirepowerBias != 1 || !object->Is_Weapon_Equipped()) powerup = CRATE_MONEY;
					break;
				case CRATE_CLOAK:
					if (object->IsCloakable) powerup = CRATE_MONEY;
					break;
			}

			if (Land == LAND_WATER && !Powerups::Naval[powerup]) powerup = CRATE_MONEY;
			if (Session.Type == GAME_INTERNET) {
				object->House->TotalCrates.Increment_Unit_Total(powerup);
			}
		}

		Map.Remove_Crate(Fetch_CellID());

		if (Session.Type != GAME_NORMAL && Session.Options.Goodies) {
			Map.Place_Random_Crate();
		}

		if (powerup == CRATE_SQUAD) {
			powerup = CRATE_MONEY;
		}

		double data = Powerups::Arguments[powerup];
		bool tospeak = false;
		switch (powerup) {

			case CRATE_GAS: {
				FUN_004068E0("Crate at %d,%d contains poison gas\n", CellID.X, CellID.Y);
				WarheadTypeClass * gas = WarheadTypeClass::From_Name("GAS");
				damage = data;
				Explosion_Damage(Center_Coord(), damage, NULL, gas, false);
				for (FacingType facing = FACING_N; facing < FACING_COUNT; facing++) {
					Explosion_Damage(Adjacent_Cell(facing).Center_Coord(), damage, NULL, gas, false);
				}
				break;
			}

			case CRATE_TIBERIUM: {
				FUN_004068E0("Crate at %d,%d contains tiberium\n", CellID.X, CellID.Y);
				TiberiumType tibtype = (TiberiumType)Random_Pick(0, Tiberiums.Count() - 1);
				if (tibtype == TIBERIUM_CRUENTUS) {
					tibtype = TIBERIUM_RIPARIUS;
				}
				Place_Tiberium(tibtype, 1);
				int count = Random_Pick(10, 20);
				while (count--) {
					Coord scatter = Coord_Scatter(Center_Coord(), Random_Pick(0, 3 * CELL_LEPTON), true);
					Map[scatter].Place_Tiberium(tibtype, 1);
				}
				break;
			}

			case CRATE_DARKNESS:
				FUN_004068E0("Crate at %d,%d contains 'shroud'\n", CellID.X, CellID.Y);
				Map.Reshroud(object->House);
				break;

			case CRATE_REVEAL:
				FUN_004068E0("Crate at %d,%d contains 'reveal'\n", CellID.X, CellID.Y);
				Map.Reveal(object->House);
				VocClass::PlayAt(Rule->CrateRevealSound, Crate_Sound_Coord(*this));
				break;

			case CRATE_UNIT: {
				FUN_004068E0("Crate at %d,%d contains a unit\n", CellID.X, CellID.Y);
				UnitTypeClass const * utp = NULL;

				if (force_mcv) {
					utp = object->House->Get_First_Ownable(Rule->BaseUnit);
				}

				if (utp == NULL && (object->House->BQuantity.Value(Rule->BuildRefinery[0]->HeapID) > 0 ||
						object->House->BQuantity.Value(Rule->BuildRefinery[1]->HeapID) > 0) &&
						object->House->UQuantity.Value(Rule->HarvesterUnit[0]->HeapID) == 0 &&
						object->House->UQuantity.Value(Rule->HarvesterUnit[1]->HeapID) == 0) {
					utp = object->House->Get_First_Ownable(Rule->HarvesterUnit);
				}

				if (Rule->UnitCrateType != NULL) {
					utp = Rule->UnitCrateType;
				}

				while (utp == NULL) {
					utp = UnitTypes[Random_Pick(UNIT_FIRST, (UnitType)(UnitTypes.Count() - 1))];
					bool is_base = Rule->BaseUnit.Is_In_List(utp);
					bool human = object->House->FUN_0050B730();
					if (utp->CrateGoodie && (Session.Options.Bases || !is_base) &&
							(!Session.Options.Bases || !is_base || human || force_mcv)) {
						break;
					}
					utp = NULL;
				}

				if (utp != NULL) {
					UnitClass * goodie_unit = (UnitClass *)utp->Create_One_Of(object->House);
					if (goodie_unit != NULL) {
						if (goodie_unit->Unlimbo(Cell_Coord())) {
							if (object->House->Is_Player_Control()) {
								VocClass::PlayAt(Rule->CrateUnitSound, Cell_Coord());
							}
							return(false);
						}

						Cell cell = Map.Nearby_Location(Fetch_CellID(), goodie_unit->Class->Speed);
						if (cell != CELL_NONE && goodie_unit->Unlimbo(cell)) {
							if (object->House->Is_Player_Control()) {
								VocClass::PlayAt(Rule->CrateUnitSound, Cell_Coord());
							}
							return(false);
						}
						delete goodie_unit;
						powerup = CRATE_MONEY;
						goto crate_money;
					}
				}
			}
			break;

			case CRATE_SQUAD:
				break;

			case CRATE_MONEY:
crate_money:
				FUN_004068E0("Crate at %d,%d contains money\n", CellID.X, CellID.Y);
				if (!force_money) {
					force_money = Random_Pick((int)data, (int)data + 900);
				}
				if (!object->House->Is_Player_Control() || Session.Type != GAME_NORMAL) {
					object->House->Refund_Money(force_money);
				} else {
					PlayerPtr->Refund_Money(force_money);
				}
				if (object->House->Is_Player_Control()) {
					VocClass::PlayAt(Rule->CrateMoneySound, Crate_Sound_Coord(*this));
				}
				break;

			case CRATE_EXPLOSION:
				FUN_004068E0("Crate at %d,%d contains explosives\n", CellID.X, CellID.Y);
				damage = data;
				if (object != NULL) {
					int d = damage;
					object->Take_Damage(d, 0, Rule->C4Warhead, 0, true);
				}
				for (index = 0; index < 5; index++) {
					Coord frag_coord = Coord_Scatter(Cell_Coord(), Random_Pick(0, 2 * CELL_LEPTON));
					Explosion_Damage(frag_coord, damage, NULL, Rule->C4Warhead);
					AnimTypeClass const * anim = Combat_Anim(damage, Rule->C4Warhead, LAND_CLEAR, frag_coord);
					new AnimClass(anim, frag_coord, 0, 1,
						ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ZGRAD), Get_Explosion_Z(frag_coord));
					Combat_Lighting(frag_coord, damage, Rule->C4Warhead, false);
				}
				break;

			case CRATE_NAPALM:
				FUN_004068E0("Crate at %d,%d contains napalm\n", CellID.X, CellID.Y);
				coord = (object->Center_Coord() + Cell_Coord()) / 2;
				new AnimClass(AnimTypes[0], coord);
				damage = data;
				if (object != NULL) {
					int d = damage;
					object->Take_Damage(d, 0, Rule->FlameDamage, NULL, true);
				}
				Explosion_Damage(coord, damage, NULL, Rule->FlameDamage);
				break;

			case CRATE_CLOAK: {
				FUN_004068E0("Crate at %d,%d contains cloaking device\n", CellID.X, CellID.Y);
				int i;
				for (i = 0; i < DisplayClass::Layer[LAYER_GROUND].Count(); i++) {
					ObjectClass * obj = DisplayClass::Layer[LAYER_GROUND][i];

					if (obj != NULL && obj->IsDown && obj->Is_Techno() &&
							Distance(Cell_Coord(), obj->Center_Coord()) < Rule->CrateRadius) {
						((TechnoClass *)obj)->IsCloakable = true;
					}
				}
				break;
			}

			case CRATE_VETERAN: {
				FUN_004068E0("Crate at %d,%d contains veterancy(TM)\n", CellID.X, CellID.Y);
				int i;
				for (i = 0; i < DisplayClass::Layer[LAYER_GROUND].Count(); i++) {
					ObjectClass * obj = DisplayClass::Layer[LAYER_GROUND][i];

					if (obj != NULL && obj->IsDown && obj->Is_Techno() &&
							Distance(Cell_Coord(), obj->Center_Coord()) < Rule->CrateRadius &&
							((TechnoClass *)obj)->Techno_Type_Class()->IsTrainable) {
						for (int count = 0; count < data; count++) {
							VeterancyClass * vet = &((TechnoClass *)obj)->Veterancy;
							if (vet->Is_Veteran()) vet->Set_Elite(true);
							if (vet->Is_Rookie()) vet->Set_Veteran(true);
							if (vet->Is_Dumbass()) vet->Set_Rookie(true);
						}
					}
				}
				if (object->House->Is_Player_Control()) {
					VocClass::PlayAt(Rule->CratePromoteSound, Cell_Coord());
				}
				break;
			}

			case CRATE_HEAL_BASE:
				FUN_004068E0("Crate at %d,%d contains base healing\n", CellID.X, CellID.Y);
				if (object->House->Is_Player_Control()) {
					VocClass::PlayAt(Rule->HealCrateSound, Cell_Coord());
				}
				for (index = 0; index < Logic.Count(); index++) {
					ObjectClass * obj = Logic[index];

					if (obj && object->Is_Techno() && object->House == obj->Owner_HouseClass()) {
						int damage = obj->Strength - obj->Class_Of()->MaxStrength;
						obj->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true);
					}
				}
				break;

			case CRATE_ICBM: {
				FUN_004068E0("Crate at %d,%d contains ICBM\n", CellID.X, CellID.Y);
				SuperWeaponTypeClass * super = SuperWeaponTypeClass::From_Action(ACTION_NUKE_BOMB);
				int sindex = -1;
				for (index = 0; index < object->House->SuperWeapon.Count(); index++) {
					if (object->House->SuperWeapon[index]->Class->Type == SUPER_FIRST) {
						sindex = index;
						break;
					}
				}
				if (sindex != -1 && object->House->SuperWeapon[super->HeapID]->Grant(true, false, false) &&
						object->IsOwnedByPlayer) {
					Map.AddCameo(RTTI_SPECIAL, super->HeapID);
				}
				break;
			}

			case CRATE_ARMOR:
				FUN_004068E0("Crate at %d,%d contains armor\n", CellID.X, CellID.Y);
				for (index = 0; index < DisplayClass::Layer[LAYER_GROUND].Count(); index++) {
					ObjectClass * obj = DisplayClass::Layer[LAYER_GROUND][index];

					if (obj != NULL && obj->Is_Techno() &&
							Distance(Cell_Coord(), obj->Center_Coord()) < Rule->CrateRadius &&
							((TechnoClass *)obj)->ArmorBias == 1) {
						double val = ((TechnoClass *)obj)->ArmorBias * data;
						((TechnoClass *)obj)->ArmorBias = val;
						if (obj->Owner_HouseClass()->Is_Player_Control()) tospeak = true;
					}
				}
				if (tospeak) VoxClass::Play("EVA_UnitArmorUpgraded");
				if (object->House->Is_Player_Control()) {
					VocClass::PlayAt(Rule->CrateArmourSound, Cell_Coord());
				}
				break;

			case CRATE_SPEED:
				FUN_004068E0("Crate at %d,%d contains speed\n", CellID.X, CellID.Y);
				for (index = 0; index < DisplayClass::Layer[LAYER_GROUND].Count(); index++) {
					ObjectClass * obj = DisplayClass::Layer[LAYER_GROUND][index];

					if (obj && obj->Is_Foot() &&
							Distance(Cell_Coord(), obj->Center_Coord()) < Rule->CrateRadius &&
							((FootClass *)obj)->SpeedBias == 1 &&
							obj->Fetch_RTTI() != RTTI_AIRCRAFT) {
						double val = ((FootClass *)obj)->SpeedBias * data;
						((FootClass *)obj)->SpeedBias = val;
						if (((FootClass *)obj)->House->IsPlayerControl) tospeak = true;
					}
				}
				if (tospeak) VoxClass::Play("EVA_UnitSpeedUpgraded");
				if (object->House->Is_Player_Control()) {
					VocClass::PlayAt(Rule->CrateSpeedSound, Cell_Coord());
				}
				break;

			case CRATE_FIREPOWER:
				FUN_004068E0("Crate at %d,%d contains firepower\n", CellID.X, CellID.Y);
				for (index = 0; index < DisplayClass::Layer[LAYER_GROUND].Count(); index++) {
					ObjectClass * obj = DisplayClass::Layer[LAYER_GROUND][index];

					if (obj && obj->Is_Techno() &&
							Distance(Cell_Coord(), obj->Center_Coord()) < Rule->CrateRadius &&
							((TechnoClass *)obj)->FirepowerBias == 1) {
						double val = ((TechnoClass *)obj)->FirepowerBias * data;
						((TechnoClass *)obj)->FirepowerBias = val;
						if (obj->Owner_HouseClass()->Is_Player_Control()) tospeak = true;
					}
				}
				if (tospeak) VoxClass::Play("EVA_UnitFirePowerUpgraded");
				if (object->House->Is_Player_Control()) {
					VocClass::PlayAt(Rule->CrateFireSound, Cell_Coord());
				}
				break;

			case CRATE_INVULN:
				break;

			case CRATE_ION_STORM:
				break;

			default:
				break;
		}

		if (Powerups::Anims[powerup] != ANIM_NONE) {
			Coord const animation_coord = Cell_Coord() + Coord(0, 0, 200);
			new AnimClass(AnimTypes[Powerups::Anims[powerup]], animation_coord);
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x00483410
bool CellClass::Flag_Place(HousesType house)
{
	if (!IsFlagged && Is_Clear_To_Move(SPEED_TRACK, false, false)) {
		IsFlagged = true;
		Owner = house;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00483460
bool CellClass::Flag_Remove(void)
{
	if (IsFlagged) {
		IsFlagged = false;
		Owner = HOUSE_NONE;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00483480
void CellClass::Shimmer(void)
{
	ObjectClass * object = Cell_Occupier();

	while (object) {
		object->Do_Shimmer();
		object = object->Next;
	}
}

// FUNCTION: GAMEMD 0x004834A0
bool CellClass::Is_Clear_To_Move(SpeedType loco, bool ignoreinfantry, bool ignorevehicles, int zone, MZoneType check, int cell_height, bool checkbridge) const
{
	if (loco == SPEED_WINGED) {
		return(true);
	}

	if (zone != -1) {
		if (zone != Map.Get_Cell_Zone(CellID, check, checkbridge)) {
			return(false);
		}
	}

	if (cell_height != -1) {
		if (cell_height != Height) {
			if (!IsUnderBridge || cell_height != Height + BRIDGE_CELL_HEIGHT) {
				return(false);
			}
		} else if (IsUnderBridge && !checkbridge) {
			return(false);
		}
	}

	bool bridge = false;
	int composite;
	if ((cell_height == -1 || cell_height == Height + BRIDGE_CELL_HEIGHT) && IsUnderBridge) {
		bridge = true;
		composite = BridgeFlag.Composite;
	} else {
		composite = Flag.Composite;
	}

	if (ignoreinfantry) {
		composite &= 0xE0;
	}
	if (ignorevehicles) {
		composite &= 0x5F;
	}
	if (composite != 0) {
		return(false);
	}

	LandType land = Land_Type();

	OverlayTypeClass const * overlay = NULL;
	if (Overlay != OVERLAY_NONE) {
	 	overlay = OverlayTypes[Overlay];
	}
	if (overlay != NULL && overlay->IsWall) {
		if (check != MZONE_DESTROYER && check != MZONE_AMPHIBIOUS_DESTROYER && check != MZONE_INFANTRY_DESTROYER && (check != MZONE_CRUSHER && check != MZONE_AMPHIBIOUS_CRUSHER || !overlay->IsCrushable) && check != 12) {
			return(false);
		}

		land = LAND_CLEAR;
	}

	if (::Ground[land].Cost[loco] == 0 && !bridge) {
		return(false);
	}

	return(true);
}

// FUNCTION: GAMEMD 0x00483610
bool CellClass::Is_Bridge_Here(void) const
{
	return(IsUnderBridge);
}

// FUNCTION: GAMEMD 0x00483620
bool CellClass::Can_Tiberium_Grow(void) const
{
	if (!Scen->IsTibGrowth) return(false);

	TiberiumType tiberium = Which_Tiberium_Type(Overlay);

	if (tiberium == TIBERIUM_NONE) return(false);

	TiberiumClass *tptr = Tiberiums[tiberium];

	if (Ramp != RAMP_NONE) return(false);

	if (OverlayData >= tptr->FrameCount - 1) return(false);

	if (tptr->GrowthPercentage < 0.00001) return(false);

	return(true);
}

// FUNCTION: GAMEMD 0x00483690
bool CellClass::Can_Tiberium_Spread(void) const
{
	if (!Scen->Special.IsTSpread) return(false);

	TiberiumType tiberium = Which_Tiberium_Type(Overlay);

	if (tiberium == TIBERIUM_NONE) return(false);

	if (OverlayData <= tiberium / 2) return(false);

	if (Ramp != RAMP_NONE) return(false);

	if (Tiberiums[tiberium]->SpreadPercentage < 0.00001) return(false);

	if (Cell_Occupier() != NULL) return(false);

	return(true);
}

inline TiberiumType Overlay_Tiberium(CellClass const * cell) { return Which_Tiberium_Type(cell->Overlay); }

// FUNCTION: GAMEMD 0x00483710
bool CellClass::Grow_Tiberium(void)
{
	if (Can_Tiberium_Grow()) {
		return(Place_Tiberium(Overlay_Tiberium(this), 1));
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00483780
bool CellClass::Spread_Tiberium(bool forced)
{
	if (!forced) {
		if (!Can_Tiberium_Spread()) return(false);
	}

	TiberiumType tibtype = Which_Tiberium_Type(Overlay);

	if (!forced) {
		if (tibtype == TIBERIUM_NONE) return(false);
	}

	if (tibtype == TIBERIUM_NONE) {
		tibtype = TIBERIUM_FIRST;
	}

	TiberiumClass * tiberium = Tiberiums[tibtype];
	FacingType offset = Random_Pick(FACING_N, FACING_NW);
	for (FacingType index = FACING_N; index < FACING_COUNT; index++) {
		FacingType facing = Facing_Add(index, offset);
		CellClass * newcell;
		if ((unsigned)facing >= FACING_COUNT) {
			newcell = this;
		} else {
			Cell here = Fetch_CellID();
			Cell next = here + AdjacentCell[(int)facing & 7];
			newcell = &Map[next];
		}

		if (newcell != NULL && newcell->Can_Tiberium_Germinate(tiberium)) {
			return(newcell->Place_Tiberium(tibtype, 3));
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004838E0
bool CellClass::Can_Tiberium_Germinate(TiberiumClass const * tiberium) const
{
	if (!Map.In_Local_Radar(CellID)) return(false);

	if (IsUnderBridge || WasUnderBridge) return(false);

	BuildingClass const * building = Cell_Building();
	if (building != NULL && building->Strength > 0 && !building->Class->IsInvisible && !building->Class->IsInvisibleInGame) return(false);

	TerrainClass * terrain = Cell_Terrain();
	if (terrain != NULL && terrain->Class->IsTiberiumSpawn) return(false);

	if (!Ground[Land_Type()].Build) return(false);

	if (Overlay != OVERLAY_NONE) return(false);

	if (Ramp != RAMP_NONE) return(false);

	if (ITType >= ISOTILE_FIRST && ITType < IsometricTileTypes.Count() && !IsometricTileTypes[ITType]->IsAllowTiberium) return(false);

	return(true);
}

// FUNCTION: GAMEMD 0x004839F0
HRESULT STDMETHODCALLTYPE CellClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) CellClass(NoInitClass());

		if (FoggedObjects != NULL) {
			FoggedObjects = new FOGGED_OBJECT_LIST;

			int count;
			result = stream->Read(&count, sizeof(count), NULL);
			if (FAILED(result)) {
				return(result);
			}

			for (int i = 0; i < count; i++) {
				FoggedObjectClass * fogged = NULL;
				if (FAILED(stream->Read(&fogged, sizeof(fogged), NULL))) {
					return(S_OK);
				}
				FoggedObjects->Add(fogged);
			}

			for (int j = 0; j < FoggedObjects->Count(); j++) {
				Swizzle_Pointer(&(*FoggedObjects)[j]);
			}
		}

		PixelFX = NULL;

		int id = CellID.X + CellID.Y * MAP_CELL_W;
		if (Map.Array[id] != NULL) {
			delete Map.Array[id];
			Map.Array[id] = NULL;
		}
		Map.Array[id] = this;

		Swizzle_Pointer(&BridgeDeckCell);
		Swizzle_Pointer(&UnusedCell);
		Swizzle_Pointer(&Tag);
		Swizzle_Pointer(&Jumpjet);
		Swizzle_Pointer(&OccupierPtr);
		Swizzle_Pointer(&BridgeOccupierPtr);
		Swizzle_Pointer(&RadSite);
		Swizzle_Pointer(&Rubble);

		if (Overlay == 0xb3 || Overlay == 0xf2) {
			Overlay = OVERLAY_NONE;
			OverlayData = 0;
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00483C10
HRESULT STDMETHODCALLTYPE CellClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		if (FoggedObjects != NULL) {
			int count = FoggedObjects->Count();
			result = stream->Write(&count, sizeof(count), NULL);
			if (FAILED(result)) {
				return(result);
			}

			for (int i = 0; i < count; i++) {
				result = stream->Write(&(*FoggedObjects)[i], sizeof((*FoggedObjects)[i]), NULL);
				if (FAILED(result)) {
					return(S_OK);
				}
			}
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00483C80
void CellClass::Recalc_Passability(void)
{
	if (!Map.In_Local_Radar(CellID)) {
		Passability = PASSABLE_OUTSIDE;
		return;
	}

	if (Overlay != OVERLAY_NONE) {
		OverlayTypeClass const * overlay = OverlayTypes[Overlay];
		if (overlay->IsCrushable) {
			Passability = PASSABLE_CRUSH;
			return;
		}
		if (overlay->IsWall) {
			Passability = PASSABLE_BLOCKED;
			return;
		}
		if (Ground[overlay->Land].Cost[SPEED_WHEEL] == 0) {
			Passability = PASSABLE_NO;
			return;
		}
		if (overlay->IsARock) {
			Passability = PASSABLE_NO;
			return;
		}
		if (overlay->IsRubble) {
			Passability = PASSABLE_LAND;
			return;
		}
	}

	if (Land == LAND_WATER) {
		Passability = PASSABLE_WATER;
		return;
	}

	if (Land == LAND_BEACH) {
		Passability = PASSABLE_BEACH;
		return;
	}

	if (Ground[Land].Cost[SPEED_WHEEL] <= 0.01) {
		Passability = PASSABLE_NO;
		return;
	}

	ObjectClass * occupier = OccupierPtr;
	while (occupier != NULL) {
		switch (occupier->Fetch_RTTI()) {
			case RTTI_BUILDING:
				if (((BuildingClass *)occupier)->Class->IsFirestormWall) {
					if (((BuildingClass *)occupier)->House->FirestormDefenseActivated) {
						Passability = PASSABLE_NO;
						return;
					}
				} else if (((BuildingClass *)occupier)->Class->IsLaserFence) {
					if (((BuildingClass *)occupier)->LaserFenceFrame != 12 && ((BuildingClass *)occupier)->LaserFenceFrame != 8) {
						Passability = PASSABLE_NO;
					}
				}
				break;

			case RTTI_TERRAIN:
				if ((Scen->Theater != THEATER_SNOW && ((TerrainClass *)occupier)->Class->TemperateOccupationBits != 7) ||
					(Scen->Theater == THEATER_SNOW && ((TerrainClass *)occupier)->Class->SnowOccupationBits != 7)) {

					Passability = PASSABLE_PARTIALLY_BLOCKED;
					return;
				}

				Passability = PASSABLE_BLOCKED;
				return;
		}
		occupier = occupier->Next;
	}

	Passability = PASSABLE_LAND;
}

// FUNCTION: GAMEMD 0x00483E30
void CellClass::Init_Drawer(LightConvertClass * drawer, int intensity, int ambient, int brightness, int tile_brightness, int alt_brightness)
{
	bool got_drawer = false;
	int red_tint, green_tint, blue_tint;

	if (drawer != NULL) {
		if (Drawer != NULL) {
			Dec_Drawer_Ref_Count(Drawer);
		}
		Drawer = drawer;
		Inc_Drawer_Ref_Count(Drawer);

		red_tint = Drawer->NormalRedTint;
		green_tint = Drawer->NormalGreenTint;
		blue_tint = Drawer->NormalBlueTint;

		got_drawer = true;

	} else if (CellID != Cell(0, 0) && CellID != Cell(-1, -1)) {
		Init_Light(intensity, ambient, brightness, tile_brightness, alt_brightness, red_tint, green_tint, blue_tint);

		if (Drawer != NULL) {
			int r_clamped = red_tint, g_clamped = green_tint, b_clamped = blue_tint;
			Clamp_Tile_RGB(r_clamped, g_clamped, b_clamped);
			if (Drawer->NormalRedTint != r_clamped ||
				Drawer->NormalGreenTint != g_clamped ||
				Drawer->NormalBlueTint != b_clamped) {
				Dec_Drawer_Ref_Count(Drawer);
				Drawer = NULL;
			}
		}

		if (Drawer == NULL) {
			Drawer = IsometricTileTypeClass::Find_Or_Make_Drawer(red_tint, green_tint, blue_tint);
			if (Drawer != NULL) {
				Inc_Drawer_Ref_Count(Drawer);
			}
		}

		got_drawer = true;
	}

	if (got_drawer) {
		Intensity = intensity;
		Ambient = ambient;
		Brightness = brightness;
		TileBrightness = tile_brightness;
		AltBrightness = alt_brightness;
		RedTint = red_tint;
		GreenTint = green_tint;
		BlueTint = blue_tint;
		return;
	}

	Drawer = IsometricTileTypeClass::Find_Or_Make_Drawer(NORMAL_LIGHT, NORMAL_LIGHT, NORMAL_LIGHT);
	if (Drawer != NULL) {
		Inc_Drawer_Ref_Count(Drawer);
	}
	Brightness = NORMAL_LIGHT;
	TileBrightness = NORMAL_LIGHT;
	AltBrightness = NORMAL_LIGHT;
	RedTint = NORMAL_LIGHT;
	GreenTint = NORMAL_LIGHT;
	BlueTint = NORMAL_LIGHT;
	Intensity = 0x10000;
	Ambient = 0;
}

class PsyDom
{
	public:
		static bool Active(void);
};

class NukeFlash
{
	public:
		static bool IsFadingIn(void);
};

// FUNCTION: GAMEMD 0x00484050
void CellClass::Pick_Drawer(LightConvertClass * & drawer, int & intensity, int & ambient, int & brightness, int & tile_brightness, int & alt_brightness) const
{
	intensity = 0x10000;
	ambient = 0;
	brightness = NORMAL_LIGHT;
	tile_brightness = NORMAL_LIGHT;
	alt_brightness = NORMAL_LIGHT;

	int red_tint, green_tint, blue_tint;

	if (CellID != Cell(0, 0) && CellID != Cell(-1, -1)) {
		Init_Light(intensity, ambient, brightness, tile_brightness, alt_brightness, red_tint, green_tint, blue_tint);

		bool new_drawer = false;
		if (Drawer != NULL) {
			int r_clamped = red_tint, g_clamped = green_tint, b_clamped = blue_tint;
			Clamp_Tile_RGB(r_clamped, g_clamped, b_clamped);
			if (Drawer->NormalRedTint == r_clamped &&
				Drawer->NormalGreenTint == g_clamped &&
				Drawer->NormalBlueTint == b_clamped) {
				drawer = Drawer;
			} else {
				new_drawer = true;
			}
		} else {
			new_drawer = true;
		}

		if (new_drawer) {
			drawer = IsometricTileTypeClass::Find_Or_Make_Drawer(red_tint, green_tint, blue_tint);
		}

	} else {
		drawer = IsometricTileTypeClass::Find_Or_Make_Drawer(NORMAL_LIGHT, NORMAL_LIGHT, NORMAL_LIGHT);
	}
}

// FUNCTION: GAMEMD 0x00484180
void CellClass::Init_Light(int & intensity, int & ambient, int & brightness, int & tile_brightness, int & alt_brightness, int & red_tint, int & green_tint, int & blue_tint) const
{
	if (CellID != Cell(0, 0) && CellID != Cell(-1, -1)) {

		brightness = NORMAL_LIGHT * Scen->CurrentAmbientLight / 100;
		red_tint = NORMAL_LIGHT * Scen->NormalLighting.RedTint / 100;
		green_tint = NORMAL_LIGHT * Scen->NormalLighting.GreenTint / 100;
		blue_tint = NORMAL_LIGHT * Scen->NormalLighting.BlueTint / 100;
		ambient = 0;

		for (int i = 0; i < LightSources.Count(); i++) {
			LightSourceClass * light = LightSources[i];
			if (light->Is_Enabled() && Options.DetailLevel >= light->DetailLevel) {
				Coord cc = Cell_Coord();
				unsigned int dx = cc.X - ((Coord)light->Position).X;
				unsigned int dy = cc.Y - ((Coord)light->Position).Y;
				unsigned int visibility = light->Visibility;
				if (dx * dx + dy * dy <= visibility * visibility) {
					dx = cc.X - ((Coord)light->Position).X;
					double fdx = (int)dx;
					dy = cc.Y - ((Coord)light->Position).Y;
					double fdy = (int)dy;
					unsigned int dist = (unsigned int)fastmath::sqrt(fdx * fdx + fdy * fdy);
					if (dist <= visibility) {
						int num = (NORMAL_LIGHT * visibility - NORMAL_LIGHT * dist) / visibility;
						ambient += (num * light->Intensity) / NORMAL_LIGHT;
						red_tint += (num * light->RedTint) / NORMAL_LIGHT;
						green_tint += (num * light->GreenTint) / NORMAL_LIGHT;
						blue_tint += (num * light->BlueTint) / NORMAL_LIGHT;
					}
				}
			}
		}

		brightness += ambient;
		alt_brightness = brightness;

		if (IonStormClass::Is_Ion_Storm_Active()) {
			brightness += Height * Scen->IonLighting.LevelLight - Scen->IonLighting.GroundLight;
			alt_brightness += (Height + BRIDGE_CELL_HEIGHT) * Scen->IonLighting.LevelLight - Scen->IonLighting.GroundLight;
		} else if (PsyDom::Active()) {
			brightness += Height * Scen->NukeLighting.LevelLight - Scen->DominatorLighting.GroundLight;
			alt_brightness += (Height + BRIDGE_CELL_HEIGHT) * Scen->DominatorLighting.LevelLight - Scen->DominatorLighting.GroundLight;
		} else if (NukeFlash::IsFadingIn()) {
			brightness += Height * Scen->NukeLighting.LevelLight - Scen->NukeLighting.GroundLight;
			alt_brightness += (Height + BRIDGE_CELL_HEIGHT) * Scen->NukeLighting.LevelLight - Scen->NukeLighting.GroundLight;
		} else {
			brightness += Height * Scen->NormalLighting.LevelLight - Scen->NormalLighting.GroundLight;
			alt_brightness += (Height + BRIDGE_CELL_HEIGHT) * Scen->NormalLighting.LevelLight - Scen->NormalLighting.GroundLight;
		}

		brightness = MIN(brightness, 2000);
		tile_brightness = brightness;

		Adjust_Tile_RGB(intensity, tile_brightness, red_tint, green_tint, blue_tint);

		alt_brightness = (alt_brightness * intensity) >> 16;
		alt_brightness = MIN(alt_brightness, 2000);

		brightness = MAX(brightness, 0);
		tile_brightness = MAX(tile_brightness, 0);
		alt_brightness = MAX(alt_brightness, 0);

	} else {
		intensity = 0x10000;
		ambient = 0;
		brightness = NORMAL_LIGHT;
		tile_brightness = NORMAL_LIGHT;
		alt_brightness = NORMAL_LIGHT;
		red_tint = NORMAL_LIGHT;
		green_tint = NORMAL_LIGHT;
		blue_tint = NORMAL_LIGHT;
	}
}

// FUNCTION: GAMEMD 0x00484680
void CellClass::Recalc_Light(void)
{
	Brightness = Ambient + (NORMAL_LIGHT * Scen->CurrentAmbientLight) / 100;
	AltBrightness = Brightness;

	if (IonStormClass::Is_Ion_Storm_Active()) {
		Brightness += Height * Scen->IonLighting.LevelLight - Scen->IonLighting.GroundLight;
		AltBrightness += (Height + BRIDGE_CELL_HEIGHT) * Scen->IonLighting.LevelLight - Scen->IonLighting.GroundLight;
	} else if (PsyDom::Active()) {
		Brightness += Height * Scen->DominatorLighting.LevelLight - Scen->DominatorLighting.GroundLight;
		AltBrightness += (Height + BRIDGE_CELL_HEIGHT) * Scen->DominatorLighting.LevelLight - Scen->DominatorLighting.GroundLight;
	} else if (NukeFlash::IsFadingIn()) {
		Brightness += Height * Scen->NukeLighting.LevelLight - Scen->NukeLighting.GroundLight;
		AltBrightness += (Height + BRIDGE_CELL_HEIGHT) * Scen->NukeLighting.LevelLight - Scen->NukeLighting.GroundLight;
	} else {
		Brightness += Height * Scen->NormalLighting.LevelLight - Scen->NormalLighting.GroundLight;
		AltBrightness += (Height + BRIDGE_CELL_HEIGHT) * Scen->NormalLighting.LevelLight - Scen->NormalLighting.GroundLight;
	}

	TileBrightness = (Brightness * Intensity) >> 16;
	AltBrightness = (AltBrightness * Intensity) >> 16;

	Brightness = MIN(Brightness, 2000);
	TileBrightness = MIN(TileBrightness, 2000);
	AltBrightness = MIN(AltBrightness, 2000);

	Brightness = MAX(Brightness, 0);
	TileBrightness = MAX(TileBrightness, 0);
	AltBrightness = MAX(AltBrightness, 0);
}

// FUNCTION: GAMEMD 0x004848B0
FacingType CellClass::Bounce_Direction(Coord const & xorigin) const
{
	Cell origin = Map[xorigin].CellID;

	int dx = CellID.X - origin.X;
	int dy = CellID.Y - origin.Y;

	static FacingType const facing_lookup[9] = {
		FACING_NW, FACING_N,    FACING_NE,
		FACING_W,  FACING_NONE, FACING_E,
		FACING_SW, FACING_S,    FACING_SE
	};
	FacingType base_direction = facing_lookup[(CellID.X - origin.X) + 3 * (CellID.Y - origin.Y) + 4];

	if (dx != 0 && dy != 0) {
		bool bounce_left;
		bool bounce_right;

		if (dx == dy) {
			bounce_left = Map[origin + Cell(dx, 0)].Height >= Map[xorigin].Height + 2;
			bounce_right = Map[origin + Cell(0, dy)].Height >= Map[xorigin].Height + 2;
		} else {
			bounce_left = Map[origin + Cell(dy, 0)].Height >= Map[xorigin].Height + 2;
			bounce_right = Map[origin + Cell(0, dx)].Height >= Map[xorigin].Height + 2;
		}

		if (bounce_left) {
			if (!bounce_right) {
				base_direction = Facing_Sub(base_direction, FACING_45);
			}
		} else if (bounce_right) {
			++base_direction;
		}
	}
	return base_direction;
}

// FUNCTION: GAMEMD 0x00484A80
void CellClass::Inc_Drawer_Ref_Count(LightConvertClass * convert) const
{
	convert->Add_Reference();
}

// FUNCTION: GAMEMD 0x00484A90
void CellClass::Dec_Drawer_Ref_Count(LightConvertClass * convert) const
{
	if (GameActive) {
		convert->Remove_Reference();
	}
}

// FUNCTION: GAMEMD 0x00484AB0
bool CellClass::Has_Tunnel(void) const
{
	int count = Tubes.Count();
	return(Tube >= TUBE_FIRST && Tube < count && Land == LAND_TUNNEL);
}

// FUNCTION: GAMEMD 0x00484AE0
bool CellClass::Is_Near_Tunnel_NW(void) const
{
	if (Has_Tunnel()) {
		return(true);
	}

	CellClass const * north1 = &Adjacent_Cell(FACING_N);
	if (north1 != NULL) {
		CellClass const * north2 = &north1->Adjacent_Cell(FACING_N);
		if (north2 != NULL) {
			if (north1->Has_Tunnel() && !north2->Has_Tunnel()) {
				return(true);
			}
			CellClass const * north3 = &north2->Adjacent_Cell(FACING_N);
			if (north3 != NULL) {
				if (north2->Has_Tunnel() && !north3->Has_Tunnel()) {
					return(true);
				}
			}
		}
	}

	CellClass const * west1 = &Adjacent_Cell(FACING_W);
	if (west1 != NULL) {
		CellClass const * west2 = &west1->Adjacent_Cell(FACING_W);
		if (west2 != NULL) {
			if (west1->Has_Tunnel() && !west2->Has_Tunnel()) {
				return(true);
			}
			CellClass const * west3 = &west2->Adjacent_Cell(FACING_W);
			if (west3 != NULL) {
				if (west2->Has_Tunnel() && !west3->Has_Tunnel()) {
					return(true);
				}
			}
		}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00484D60
bool CellClass::Is_Near_Tunnel_ES(void) const
{
	if (Has_Tunnel()) {
		return(true);
	}

	CellClass const * south_east = &Adjacent_Cell(FACING_SE);
	if (south_east != NULL) {
		CellClass const * south_east2 = &south_east->Adjacent_Cell(FACING_SE);
		if (south_east2 != NULL) {
			if (south_east->Has_Tunnel() || south_east2->Has_Tunnel()) {
				return(true);
			}

			CellClass const * south = &south_east->Adjacent_Cell(FACING_S);
			CellClass const * east = &south_east->Adjacent_Cell(FACING_E);
			if (south != NULL && east != NULL) {
				if (south->Has_Tunnel() || east->Has_Tunnel()) {
					return(true);
				}
			}
		}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00484F10
bool CellClass::Can_Enter_Tunnel(FootClass const * foot) const
{
	return(true);
}

// FUNCTION: GAMEMD 0x00484F20
TubeClass * CellClass::Get_Tunnel(void) const
{
	if (Tube >= TUBE_FIRST && Tube < Tubes.Count()) {
		return(Tubes[Tube]);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x00484F50
void CellClass::Force_New_Slope_For_Occupiers(void) const
{
	int count = 0;
	ObjectClass * occupier = Cell_Occupier();
	ObjectClass * occupiers[10];

	while (occupier) {
		if (count >= 10) break;
		occupiers[count] = occupier;
		occupier = occupier->Next;
		count++;
	}

	for (int i = 0; i < count; i++) {
		occupier = occupiers[i];
		occupier->HeightAGL = 0;
		if (occupier->RTTI == RTTI_UNIT || occupier->RTTI == RTTI_AIRCRAFT) {
			((FootClass *)occupier)->Locomotion->Force_New_Slope(Ramp);
		}
	}
}

// FUNCTION: GAMEMD 0x00484FF0
wchar_t const * CellClass::Tiberium_Name(void) const
{
	TiberiumType tib = Which_Tiberium_Type(Overlay);
	if (tib != TIBERIUM_NONE) {
		return(Tiberiums[tib]->UIName);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x00485010
TiberiumType CellClass::Tiberium_Type_Here(void) const
{
	return(Which_Tiberium_Type(Overlay));
}

// FUNCTION: GAMEMD 0x00485020
int CellClass::Tiberium_Value(void) const
{
	TiberiumType tib = Which_Tiberium_Type(Overlay);
	if (tib == TIBERIUM_NONE) {
		return(0);
	}
	return(Tiberiums[tib]->CreditValue * (OverlayData + 1));
}

// FUNCTION: GAMEMD 0x00485060
bool CellClass::Is_Tile_Water(void) const
{
	return(ITType >= IsometricTileTypeClass::WaterSet && ITType < IsometricTileTypeClass::WaterSet + 14);
}

// FUNCTION: GAMEMD 0x00485080
LEPTON CellClass::Occupier_Height(void) const
{
	Coord coord(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0);
	coord.Z = Get_Height(coord);

	BuildingClass * building = NULL;
	if (GameActive) {
		ObjectClass * object = Cell_Occupier();
		while (object != NULL) {
			if (object->Fetch_RTTI() == RTTI_BUILDING) {
				building = (BuildingClass *)object;
				break;
			}
			object = object->Next;
		}
	}
	if (building) {
		Point3D dimensions = building->Class->Lepton_Dimensions();
		coord.Z += dimensions.Z;
	} else {
		TechnoClass * techno = Cell_Techno();
		if (techno != NULL) {
			coord.Z += CELL_LEPTON / 3;
		}
	}

	return(coord.Z);
}

// FUNCTION: GAMEMD 0x00485130
void CellClass::Detach(AbstractClass const * target)
{
	if (Tag == target) {
		if (Tag != NULL) {
			Tag->AttachCount--;
		}
		Tag = NULL;
		Map.TaggedCells.Delete(CellID);
	}
	if (BridgeDeckCell == target) {
		BridgeDeckCell = NULL;
	}
	if (UnusedCell == target) {
		UnusedCell = NULL;
	}
	if (FoggedObjects != NULL) {
		if (target->Fetch_RTTI() == RTTI_FOGGEDOBJECT) {
			FoggedObjects->Delete((FoggedObjectClass *)target);
		}
	}
}

// FUNCTION: GAMEMD 0x00485200
HRESULT STDMETHODCALLTYPE CellClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_CellClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00485240
void CellClass::Set_CellID(Cell const & cell)
{
	CellID = cell;
}

// FUNCTION: GAMEMD 0x00485250
void CellClass::Attach_Tag(TagClass *tag)
{
	if (Tag != NULL) {
		Tag->AttachCount--;
	}

	Tag = tag;
	if (Tag != NULL) {
		Map.TaggedCells.Add(CellID);
		Tag->AttachCount++;
	}
}

// FUNCTION: GAMEMD 0x004852D0
void CellClass::Register_As_Dirty(void)
{
	Point2D point;
	TacticalMap->Coord_To_Pixel(Coord_Whole(CellID), point);
	point.Y += TacticalRect.Y - 15 * Height;
	point -= Point2D(30, 15);
	TacticalMap->Register_Dirty_Area(Rect(point, 60, 30));
}

// FUNCTION: GAMEMD 0x00485390
int CellClass::Get_Vein_Frame(void) const
{
	int frame = 0;
	for (int i = 0; i < FACING_COUNT / 2; i++) {
		CellClass const & adjacent = Adjacent_Cell(FacingType(i << FACING_45));
		if (adjacent.Overlay != OVERLAY_NONE && (adjacent.Ramp <= RAMP_NONE || OverlayTypes[adjacent.Overlay]->IsVeins)) {
			if ((adjacent.Overlay != OVERLAY_VEINS && OverlayTypes[adjacent.Overlay]->IsVeins) ||
				(adjacent.Overlay == OVERLAY_VEINS && adjacent.OverlayData >= 0x30)) {
				frame |= 1 << i;
			}
		}
	}
	return(frame);
}

// FUNCTION: GAMEMD 0x00485460
bool CellClass::Can_Place_Veins(void)
{
	if (Ramp <= RAMP_SOUTH) {
		if (Land != LAND_WATER && Land != LAND_ROCK && Land != LAND_ICE && Land != LAND_BEACH) {
			if (Overlay == OVERLAY_NONE || OverlayTypes[Overlay]->IsVeins) {
				for (FacingType dir = FACING_N; dir < FACING_COUNT; dir = FacingType(dir + FACING_90)) {
					CellClass const & adjacent = ((CellClass const *)this)->Adjacent_Cell(dir);
					if (adjacent.Ramp > RAMP_SOUTH && Ramp == RAMP_NONE) {
						if (adjacent.Overlay == OVERLAY_NONE || !OverlayTypes[adjacent.Overlay]->IsVeins) {
							return(false);
						}
					}
					if (adjacent.Land == LAND_WATER || adjacent.Land == LAND_ROCK || adjacent.Land == LAND_ICE || adjacent.Land == LAND_BEACH) {
						return(false);
					}
					if (adjacent.Overlay != OVERLAY_NONE && !OverlayTypes[adjacent.Overlay]->IsVeins) {
						return(false);
					}
				}
				return(true);
			}
		}
	}
	return(false);
}

static inline int _Vein_Frame_B(CellClass const * cell) { return cell->Get_Vein_Frame(); }
static inline int _Vein_Frame_A(CellClass const * cell) { return _Vein_Frame_B(cell); }

// FUNCTION: GAMEMD 0x00485590
void CellClass::Redraw_Veins(void)
{
	Rect dirty(0, 0, 0, 0);
	dirty = Union(dirty, Union(Overlay_Render_Rect(), Overlay_Shadow_Render_Rect()));

	OverlayData = 0;
	int frame = _Vein_Frame_A(this);
	if (frame == 0 || Ramp != 0) {
		Overlay = OVERLAY_NONE;
	} else {
		OverlayData = 3 * frame + abs(Scen->RandomNumber() % 3);
	}
	Map.Radar_Background(CellID);

	for (FacingType dir = FACING_N; dir < FACING_COUNT; dir = FacingType(dir + FACING_90)) {
		CellClass & adjacent = Adjacent_Cell(dir);

		if (adjacent.Ramp == 0 && adjacent.Overlay == OVERLAY_VEINS && adjacent.OverlayData < 0x30) {
			int adjframe = _Vein_Frame_A(&adjacent);

			Rect rect(0, 0, 0, 0);
			rect = Union(rect, Union(adjacent.Overlay_Render_Rect(), adjacent.Overlay_Shadow_Render_Rect()));

			if (adjframe == 0) {
				adjacent.Overlay = OVERLAY_NONE;
			} else {
				adjacent.OverlayData = 3 * adjframe + abs(Scen->RandomNumber() % 3);
			}
			adjacent.Recalc_Attributes();

			rect = Union(rect, Union(adjacent.Overlay_Render_Rect(), adjacent.Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
			TacticalMap->Register_Dirty_Area(rect);
			Map.Radar_Background(adjacent.CellID);
		}
	}

	dirty = Union(dirty, Union(Overlay_Render_Rect(), Overlay_Shadow_Render_Rect())) - TacticalRect.TopLeft;
	TacticalMap->Register_Dirty_Area(dirty);
	Recalc_Attributes();
}

#define IS_SET_VALID(setname) (IsometricTileTypeClass::setname != ISOTILE_INVALID)
#define IS_TILE_IN_SET(setname, setsize) (ITType >= IsometricTileTypeClass::setname && ITType < IsometricTileTypeClass::setname + setsize)
#define IS_TILE_IN_VALID_SET(setname, setsize) (IS_SET_VALID(setname) && IS_TILE_IN_SET(setname, setsize))

// FUNCTION: GAMEMD 0x00486380
bool CellClass::Is_Tile_Clear(void) const
{
	if (ITType == ISOTILE_NONE || ITType == ISOTILE_CLEAR) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004863A0
bool CellClass::Is_Tile_Ramp(void) const
{
	if (IS_TILE_IN_SET(RampStart, RAMP_BASE_COUNT) ||
		IS_TILE_IN_SET(RampSmooth, RAMP_SMOOTH_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004863D0
bool CellClass::Is_Tile_Cliff(void) const
{
	if (IS_TILE_IN_VALID_SET(CliffSet, CLIFF_COUNT)) return(true);

	if (IS_TILE_IN_VALID_SET(WaterfallEast, WATERFALL_EAST_COUNT)) {
		if (ITType == IsometricTileTypeClass::WaterfallEast || ITType == IsometricTileTypeClass::WaterfallEast + WATERFALL_EAST_COUNT-1) {
			if (SubTile == 0 || SubTile == 4) {
				return(false);
			}
		}
		return(true);
	}

	if (IS_TILE_IN_VALID_SET(WaterfallWest, WATERFALL_WEST_COUNT)) {
		if (ITType == IsometricTileTypeClass::WaterfallWest || ITType == IsometricTileTypeClass::WaterfallWest + WATERFALL_WEST_COUNT-1) {
			if (SubTile == 1 || SubTile == 3) {
				return(false);
			}
		}
		return(true);
	}

	if (IS_TILE_IN_VALID_SET(WaterfallSouth, WATERFALL_SOUTH_COUNT)) {
		if (ITType == IsometricTileTypeClass::WaterfallSouth || ITType == IsometricTileTypeClass::WaterfallSouth + WATERFALL_SOUTH_COUNT-1) {
			if (SubTile == 0 || SubTile == 1) {
				return(false);
			}
		}
		return(true);
	}

	if (IS_TILE_IN_VALID_SET(WaterfallNorth, WATERFALL_NORTH_COUNT)) {
		if (ITType == IsometricTileTypeClass::WaterfallNorth || ITType == IsometricTileTypeClass::WaterfallNorth + WATERFALL_NORTH_COUNT-1) {
			if (SubTile == 2 || SubTile == 3) {
				return(false);
			}
		}
		return(true);
	}

	if (IS_TILE_IN_VALID_SET(CliffRamps, CLIFF_RAMPS_COUNT)) return(true);
	if (IS_TILE_IN_VALID_SET(WaterCaves, 4)) return(true);
	if (IS_TILE_IN_VALID_SET(BridgeSet, BRIDGE_COUNT)) return(true);
	if (IS_TILE_IN_VALID_SET(TrainBridgeSet, TRAIN_BRIDGE_COUNT)) return(true);
	if (IS_TILE_IN_VALID_SET(DestroyableCliffs, DESTROYABLE_CLIFFS_COUNT)) return(true);
	if (IS_TILE_IN_VALID_SET(WaterCliffs, WATER_CLIFFS_COUNT)) return(true);

	return(false);
}

// FUNCTION: GAMEMD 0x004865B0
bool CellClass::Is_Tile_Shore(void) const
{
	if (IS_TILE_IN_SET(ShorePieces, SHORE_PIECES_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004865D0
bool CellClass::Is_Tile_With_Water(void) const
{
	if (Is_Tile_Shore()) {
		return(true);
	}

	if (IS_TILE_IN_SET(WaterSet, WATER_COUNT) ||
		IS_TILE_IN_SET(WaterfallEast, WATERFALL_EAST_COUNT) ||
		IS_TILE_IN_SET(WaterfallWest, WATERFALL_WEST_COUNT) ||
		IS_TILE_IN_SET(WaterfallSouth, WATERFALL_SOUTH_COUNT) ||
		IS_TILE_IN_SET(WaterfallNorth, WATERFALL_NORTH_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00486650
bool CellClass::Is_Tile_Misc_Pavement(void) const
{
	if (IS_TILE_IN_SET(MiscPaveTile, MISC_PAVE_TILE_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00486670
bool CellClass::Is_Tile_Pavement(void) const
{
	if (IS_TILE_IN_SET(PaveTile, PAVE_TILE_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00486690
bool CellClass::Is_Tile_Dirt_Road(void) const
{
	if (IS_TILE_IN_SET(DirtRoadJunction, DIRT_ROAD_JUNCTION_COUNT) ||
		IS_TILE_IN_SET(DirtRoadCurve, DIRT_ROAD_CURVE_COUNT) ||
		IS_TILE_IN_SET(DirtRoadStraight, DIRT_ROAD_STRAIGHT_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004866D0
bool CellClass::Is_Tile_Paved_Road(void) const
{
	if (IS_TILE_IN_SET(PavedRoads, PAVED_ROAD_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004866F0
bool CellClass::Is_Tile_Paved_Road_End(void) const
{
	if (IS_TILE_IN_SET(PavedRoadEnds, PAVED_ROAD_ENDS_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00486710
bool CellClass::Is_Tile_Paved_Road_Slope(void) const
{
	if (IS_TILE_IN_SET(PavedRoadSlopes, PAVED_ROAD_SLOPES_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00486730
bool CellClass::Is_Tile_Road_Median(void) const
{
	if (IS_TILE_IN_SET(Medians, 14)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00486750
bool CellClass::Is_Tile_Bridge(void) const
{
	if (IS_TILE_IN_VALID_SET(BridgeSet, BRIDGE_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00486770
bool CellClass::Is_Tile_Train_Bridge(void) const
{
	if (IS_TILE_IN_VALID_SET(TrainBridgeSet, TRAIN_BRIDGE_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00486790
bool CellClass::Is_Tile_Clear_To_Sand_LAT(void) const
{
	if (IS_TILE_IN_SET(ClearToSandLat, 16)) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004867B0
bool CellClass::Is_Tile_Clear_To_Green_LAT(void) const
{
	if (ITType == IsometricTileTypeClass::GreenTile || (ITType >= IsometricTileTypeClass::ClearToGreenLat && ITType < IsometricTileTypeClass::ClearToGreenLat + 16)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004867E0
bool CellClass::Is_Tile_Not_Water(void) const
{
	if (ITType >= IsometricTileTypeClass::WaterSet && ITType < IsometricTileTypeClass::WaterSet + 14) {
		return(false);
	}

	return(true);
}

// FUNCTION: GAMEMD 0x00486800
bool CellClass::Should_Draw_As_Cloaked(HousesType house) const
{
	if (PlayerPtr != NULL) {
		if (Is_Cloaked(house)) {
			if (house == PlayerPtr->HeapID) {
				return(true);
			}
			if (!Is_Sensed(PlayerPtr->HeapID)) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00486840
Coord CellClass::Center_Coord(void) const
{
	Point2D pt = Point2D((CellID.X * CELL_LEPTON_W) + CELL_LEPTON_W / 2, (CellID.Y * CELL_LEPTON_H) + CELL_LEPTON_H / 2);
	Coord coord;
	coord.X = pt.X;
	coord.Y = pt.Y;
	coord.Z = Get_Height(pt);
	return(coord);
}

// FUNCTION: GAMEMD 0x00486890
Coord CellClass::As_Coord(void) const
{
	if (IsUnderBridge) {
		return(Center_Coord() + Coord(0, 0, BRIDGE_LEPTON_HEIGHT));
	}

	return(Center_Coord());
}

// FUNCTION: GAMEMD 0x00486900
bool CellClass::Is_Tile_Destroyable_Cliff(void) const
{
	if (ITType == IsometricTileTypeClass::DestroyableCliffs + DESTROYABLE_CLIFFS_COUNT-2) {
		return(true);
	}

	if (ITType == IsometricTileTypeClass::DestroyableCliffs + DESTROYABLE_CLIFFS_COUNT-1) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00486920
void CellClass::Trigger_Veins(void)
{
	if (Overlay == OVERLAY_VEINS && OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN && Ramp == RAMP_NONE && !IsAnimAttached) {
		ObjectClass * occupier = Cell_Occupier();
		while (occupier != NULL) {
			if (occupier->HeightAGL <= 5 && occupier->Is_Techno() &&
				!occupier->TClass->IsImmuneToVeins && !((TechnoClass *)occupier)->Has_Ability(ABILITY_VEIN_PROOF)) {

				new AnimClass(Rule->VeinAttack, Cell_Coord() - Coord(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2));
				IsAnimAttached = true;
			}
			occupier = occupier->Next;
		}
	}
}

// FUNCTION: GAMEMD 0x00486A70
void CellClass::Fog_Cell(void)
{
	if (Scen->Special.IsFogOfWar) {
		Cell cell = CellID;

		for (int height = 1; height <= 15; height += 2) {
			CellClass *cptr = &Map[cell];

			if (cptr->Height >= height - 2 && cptr->Height <= height) {
				if (!cptr->IsFogged) {
					FOGGED_OBJECT_LIST * fogged_objects = new FOGGED_OBJECT_LIST;
					fogged_objects->Resize(1);
					fogged_objects->Set_Growth_Step(1);

					cptr->IsFogged = true;
					ObjectClass * occupier = cptr->Cell_Occupier();

					while (occupier != NULL) {
						int rtti = occupier->What_Am_I();
						if (rtti == RTTI_BUILDING) {
							BuildingClass * building = (BuildingClass *)occupier;
							building->PositionCell;
							if (building->Should_Fog()) {
								bool fade;
								if (building->IsStrange() || building->TranslucencyLevel == 15) {
									fade = false;
								} else {
									fade = true;
								}
								building->Make_Fogged(fogged_objects, cptr, fade);
							}
						} else if (rtti == RTTI_UNIT || rtti == RTTI_AIRCRAFT || rtti == RTTI_INFANTRY) {
							occupier->Unselect();
						}
						occupier = occupier->Next;
					}

					if (fogged_objects->Count() > 0) {
						cptr->FoggedObjects = fogged_objects;
					} else {
						delete fogged_objects;
					}
				}
			}
			cell += Cell(1,1);
		}
	}
}

// FUNCTION: GAMEMD 0x00486BF0
void CellClass::Unfog_Cell(void)
{
	Cell cell = CellID;
	for (int i = 1; i < 15; i += 2) {
		CellClass *cptr = &Map[cell];
		int height = cptr->Height;
		if (height >= i - 2 && height <= i) {
			cptr->IsFogged = false;
			cptr->Remove_Fogged_Objects();
		}
		cell += Cell(1,1);
	}
}

// FUNCTION: GAMEMD 0x00486C50
void CellClass::Remove_Fogged_Objects(void)
{
	if (FoggedObjects != NULL) {
		for (int i = FoggedObjects->Count() - 1; i >= 0; i--) {
			FoggedObjectClass * obj = (*FoggedObjects)[i];
			if (obj->Get_Head_Record_Occupy_List() != NULL) {
				Cell const * list = obj->Get_Head_Record_Occupy_List();
				while (*list != REFRESH_EOL) {
					Coord coord = obj->Position;
					Cell cell = *list + coord.As_Cell();
					CellClass * cptr = &Map[cell];
					if (cptr != this && cptr->FoggedObjects != NULL) {
						cptr->FoggedObjects->Delete(obj);
					}
					list++;
				}
			}
			delete obj;
		}
		FoggedObjects->Clear();
		delete FoggedObjects;
		FoggedObjects = NULL;
	}
}

// FUNCTION: GAMEMD 0x00486D90
int CellClass::Occupation_Mask(HousesType house) const
{
	if (!(OccupiedBy & (1 << house))) {
		return(-1);
	}

	int mask = 0;
	for (int dir = 0; dir < FACING_COUNT; dir++) {
		CellClass const & adjacent = Adjacent_Cell(FacingType(dir));
		if (adjacent.OccupiedBy & (1 << house)) {
			mask |= (1 << dir);
		}
	}
	return(mask);
}

// FUNCTION: GAMEMD 0x00486E30
int CellClass::Reduce_Weed(void)
{
	VeinholeMonsterClass * veinhole = VeinholeMonsterClass::Get_Vein_Owner_At(CellID);
	if (veinhole != NULL) {
		veinhole->Reduce_Veins_At(this);
	} else {
		Map[CellID].Redraw_Veins();
	}
	return(0);
}

// FUNCTION: GAMEMD 0x00486E70
void CellClass::Register_For_Redraw(void)
{
	Rect rect = Union(Union(Cell_Render_Rect(), Overlay_Render_Rect()), Overlay_Shadow_Render_Rect());
	TacticalMap->Register_Dirty_Area(rect - TacticalRect.TopLeft);
	Map.Radar_Background(CellID);
}

// FUNCTION: GAMEMD 0x00486FF0
bool CellClass::Can_Burrow_Here(void) const
{
	if (!Map.In_Local_Radar(this)) {
		return(true);
	}

	if (ITType < ISOTILE_FIRST || ITType >= IsometricTileTypes.Count() || IsometricTileTypes[ITType]->IsAllowBurrowing) {
		if (Ramp == 0 && !IsUnderBridge && !WasUnderBridge) {
			if (Cell_Building() != NULL) {
				return(false);
			}
			if (Cell_Terrain() != NULL) {
				return(false);
			}
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004870B0
bool CellClass::Is_Cloaked(HousesType house) const
{
	return((CloakedBy & (1 << house)) != 0);
}

// FUNCTION: GAMEMD 0x004870D0
bool CellClass::Is_Sensed(HousesType house) const
{
	return((short)SensorsOfHouses[house] > 0);
}

// FUNCTION: GAMEMD 0x004870F0
bool CellClass::Is_Disguise_Sensed(HousesType house) const
{
	return((short)DisguiseSensorsOfHouses[house] > 0);
}

// FUNCTION: GAMEMD 0x00487110
void CellClass::Cloaked_By(HousesType house)
{
	CloakedBy |= (1 << house);
}

// FUNCTION: GAMEMD 0x00487130
void CellClass::Uncloaked_By(HousesType house)
{
	CloakedBy &= ~(1 << house);
}

// FUNCTION: GAMEMD 0x00487150
void CellClass::Sensed_By(HousesType house)
{
	SensorsOfHouses[house]++;
}

// FUNCTION: GAMEMD 0x00487160
void CellClass::Unsensed_By(HousesType house)
{
	SensorsOfHouses[house]--;
}

// FUNCTION: GAMEMD 0x00487170
void CellClass::Disguise_Sensed_By(HousesType house)
{
	DisguiseSensorsOfHouses[house]++;
}

// FUNCTION: GAMEMD 0x00487180
void CellClass::Disguise_Unsensed_By(HousesType house)
{
	DisguiseSensorsOfHouses[house]--;
}

// FUNCTION: GAMEMD 0x00487190
bool CellClass::Place_Tiberium(TiberiumType tib, int data)
{
	TiberiumClass * tiberium = Tiberiums[tib];
	if (data < tiberium->FrameCount) {
		if (Can_Tiberium_Germinate(tiberium)) {
			if (Ramp != RAMP_NONE) {
				new OverlayClass(OverlayTypes[tiberium->Overlay->HeapID + tiberium->Variety + 2 * Ramp + (Random_Pick(0, 1) - 2)], Fetch_CellID());
			} else {
				new OverlayClass(OverlayTypes[tiberium->Overlay->HeapID + Random_Pick(0, 11)], Fetch_CellID());
			}
			tiberium->Queue_Growth(CellID);
			OverlayData = data;
			Register_For_Redraw();
			return(true);
		}

		if (Can_Tiberium_Grow()) {
			if (Tiberium_Type_Here() == tib) {
				OverlayData += data;
				OverlayData = MIN(OverlayData, tiberium->FrameCount - 1);
				Rect rect = Union(Union(Cell_Render_Rect(), Overlay_Render_Rect()), Overlay_Shadow_Render_Rect());
				TacticalMap->Register_Dirty_Area(rect - TacticalRect.TopLeft);
				tiberium->Queue_Spread(CellID);
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00487630
void CellClass::ReduceShroudCounter(void)
{
	if (ShroudCounter == 1) {
		ShroudCounter = 0;
	}
	ShroudCounter--;
	if (!IsMapped) {
		goto reveal;
	}
	if (!IsVisible) {
reveal:
		if (ShroudCounter <= 0) {
			IsMapped = true;
			IsVisible = true;
		}
	} else {
		if (ShroudCounter <= 0) {
			if (IsToShroud == 1) {
				IsToShroud = false;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00487690
void CellClass::IncreaseShroudCounter(void)
{
	int old = ShroudCounter;
	if (ShroudCounter == -1) {
		ShroudCounter = 0;
	}
	ShroudCounter++;
	if (ShroudCounter > GapsCoveringThisCell) {
		ShroudCounter = GapsCoveringThisCell;
	}
	if (old <= 0 && ShroudCounter > 0) {
		IsToShroud = true;
	}
}

// FUNCTION: GAMEMD 0x004876F0
void CellClass::Unshroud(void)
{
	IsMapped = true;
	IsVisible = true;
	if (ShroudCounter > 0) {
		IsToShroud = true;
	}
}

// FUNCTION: GAMEMD 0x00487720
void CellClass::On_Bridge_Collapse(void)
{
	ObjectClass * next;
	if (BridgeFlag.Composite) {
		Point2D pt(CellID.X * CELL_LEPTON_W + CELL_LEPTON_W / 2, CellID.Y * CELL_LEPTON_H + CELL_LEPTON_H / 2);
		Coord coord;
		coord = Coord(pt, 0);
		coord.Z = Map.Get_Height_GL(coord) + BRIDGE_LEPTON_HEIGHT;
		for (int x = -2; x < 3; x++) {
			for (int y = -2; y < 3; y++) {
				CellClass * cellptr = &Map[CellID + Cell(x, y)];
				if (cellptr != this) {
					ObjectClass * object = cellptr->Cell_Occupier(true);
					while (object) {
						next = object->Next;
						if (object->Is_Foot()) {
							FootClass * foot = (FootClass *)object;
							if (foot->Locomotion->Is_Moving_Here(coord)) {
								int damage = foot->Strength;
								foot->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true);
							}
						}
						object = next;
					}
					object = cellptr->Cell_Occupier();
					while (object) {
						next = object->Next;
						if (object->Is_Foot()) {
							FootClass * foot = (FootClass *)object;
							if (foot->Locomotion->Is_Moving_Here(coord)) {
								int damage = foot->Strength;
								foot->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true);
							}
						}
						object = next;
					}
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00487950
bool CellClass::Is_Shrouded(void) const
{
	return(Map.Is_Shrouded(Cell_Coord()));
}

// FUNCTION: GAMEMD 0x004879B0
bool CellClass::Is_Fogged(void) const
{
	return(Map.Is_Fogged(Cell_Coord()));
}

// FUNCTION: GAMEMD 0x00487A10
void CellClass::Kill_Illegal_Occupiers(bool kill_jumpjets)
{
	ObjectClass * object = Cell_Occupier();
	while (object) {
		ObjectClass * next = object->Next;
		if (object->Can_Enter_Cell(this) == MOVE_NO ||
			(kill_jumpjets && object && (object->AbstractFlags & 1) && ((TechnoClass *)object)->Techno_Type_Class()->JumpJet) ||
			(object && object->Fetch_RTTI() == RTTI_AIRCRAFT)) {
			int damage = object->Strength;
			object->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true);
		}
		object = next;
	}

	Coord coord = Cell_Coord();
	for (int x = -2; x < 3; x++) {
		for (int y = -2; y < 3; y++) {
			CellClass * cellptr = &Map[Cell(x, y) + CellID];
			if (cellptr != this) {
				object = cellptr->Cell_Occupier();
				while (object) {
					if (object->Is_Foot()) {
						FootClass * foot = (FootClass *)object;
						if (foot->Locomotion->Is_Moving_Here(coord) && foot->Can_Enter_Cell(this) == MOVE_NO) {
							int damage = foot->Strength;
							foot->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, true);
						}
					}
					object = object->Next;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00487C10
bool CellClass::Can_Build_Here(void) const
{
	if (Ramp) {
		return(false);
	}

	if (GameActive) {
		BuildingClass * building = Cell_Building();
		if (building) {
			return(false);
		}
	}

	return(Ground[Land].Build);
}

// FUNCTION: GAMEMD 0x00487C70
void CellClass::FUN_00487C70(RadSiteClass * site)
{
	RadSite = site;
}

// FUNCTION: GAMEMD 0x00487C80
RadSiteClass * CellClass::FUN_00487C80(void)
{
	return(RadSite);
}

// FUNCTION: GAMEMD 0x00487C90
bool CellClass::IsRadiated(void) const
{
	return(RadLevel > 1.0);
}

// FUNCTION: GAMEMD 0x00487CB0
int CellClass::GetRadLevel(void) const
{
	return((int)MIN(RadLevel, (double)Rule->RadLevelMax));
}

// FUNCTION: GAMEMD 0x00487CE0
void CellClass::RadLevel_Increase(double amount)
{
	RadLevel += amount;
}

// FUNCTION: GAMEMD 0x00487D00
void CellClass::RadLevel_Decrease(double amount)
{
	double level = RadLevel - amount;
	RadLevel = level;
	if (0.0 > level) {
		level = 0.0;
	}
	RadLevel = level;
}

// FUNCTION: GAMEMD 0x00487D50
int CellClass::GetLevel(void) const
{
	return(Height + (IsBridgeDeck ? BRIDGE_CELL_HEIGHT : 0));
}

inline bool Is_Foot_Object(AbstractClass * const & p)
{
	return(p != NULL && (p->AbstractFlags & 4) != 0);
}

inline FootClass * As_Foot_Object(AbstractClass * const & p)
{
	return(!p ? NULL : ((p->AbstractFlags & 4) ? (FootClass *)p : NULL));
}

// FUNCTION: GAMEMD 0x00487D70
bool CellClass::TryAssignJumpjet(FootClass * foot)
{
	if (foot == NULL) {
		if (Is_Foot_Object(Jumpjet)) {
			As_Foot_Object(Jumpjet)->Jumpjet_OccupyCell(CELL_NONE);
		}
		Jumpjet = NULL;
		return(true);
	}
	if (Jumpjet != NULL && Jumpjet != foot) {
		return(false);
	}
	Jumpjet = foot;
	if (FootClass * f = As_Foot_Object(foot)) {
		f->Jumpjet_OccupyCell(CellID);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00487DF0
bool CellClass::FUN_00487DF0(void) const
{
	if (Land == LAND_TIBERIUM) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00487E00
bool CellClass::FUN_00487E00(void) const
{
	if (GameActive) {
		ObjectClass * object = Cell_Occupier();
		while (object != NULL) {
			if (object->Fetch_RTTI() == RTTI_BUILDING) {
				break;
			}
			object = object->Next;
		}
		if (object != NULL && OccupyHeightsCoveringMe == 1) {
			return(false);
		}
	}
	return(OccupyHeightsCoveringMe != 0);
}

// FUNCTION: GAMEMD 0x00487E50
int STDMETHODCALLTYPE CellClass::What_Am_I(void) const
{
	return(RTTI_CELL);
}

// FUNCTION: GAMEMD 0x00487E60
RTTIType CellClass::Fetch_RTTI(void) const
{
	return(RTTI_CELL);
}

// FUNCTION: GAMEMD 0x00487E70
int CellClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// SYNTHETIC: GAMEMD 0x00487E80
// ??_GCellClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x00487EA0
// Adjacent_Cell

// TEMPLATE: GAMEMD 0x00487EE0
// ??G?$TRect@H@@QBE?BV0@ABV?$TPoint2D@H@@@Z

// TEMPLATE: GAMEMD 0x00487F20
// ??G?$TPoint2D@H@@QBE?BV0@ABV0@@Z

// TEMPLATE: GAMEMD 0x00487F40
// ?Union@@YI?BV?$TRect@H@@ABV1@0@Z
template TRect<int> const Union<int>(TRect<int> const &, TRect<int> const &);

// SYNTHETIC: GAMEMD 0x00488030
// ??_G?$DynamicVectorClass@PAVObjectClass@@@@UAEPAXI@Z
template class VectorClass<ObjectClass *>;

// TEMPLATE: GAMEMD 0x00488080
// ??0?$VectorClass@PAVFoggedObjectClass@@@@QAE@HPBQAVFoggedObjectClass@@@Z
template class VectorClass<FoggedObjectClass *>;
