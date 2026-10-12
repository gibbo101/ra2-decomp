// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
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
#include "warhead.h"
#include "bench.hh"
#include "ramp.hh"
#include "tube.hh"

void __cdecl FUN_004068E0(char const * format, ...);

inline IsometricTileTypeClass * Next_Tile_In_Set(IsometricTileTypeClass * type, int index) { return type->Next_Tile_From_Set(index); }
inline unsigned short * Preview_Tile_Of(IsometricTileTypeClass * type, int tile, int level) { return type->Preview_Tile(tile, level); }
inline VisualType Building_Visual(BuildingClass * building) { return building->Visual_Character(false, NULL); }

class PixelFXClass
{
	public:
		virtual ~PixelFXClass(void);
};

// FUNCTION: 0x0047B360
CellClass::CellClass(NoInitClass const & x) :
	BASECLASS(x),
	Drawer(NULL),
	Passability(PASSABLE_LAND)
{

}

inline void CellClass::Dec_Drawer_Ref_Count(LightConvertClass * convert) const
{
	if (GameActive) {
		convert->Remove_Reference();
	}
}

// FUNCTION: 0x0047BB60
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

// FUNCTION: 0x0047BBF0
CellClass::CellClass(void) :
	BASECLASS(),
	CellID(CELL_NONE),
	IsPlot(false),
	IsCursorHere(false),
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
	IsMapped(false),
	IsVisible(false),
	IsFogMapped(false),
	IsFogVisible(false),
	IsAltUnknown(false),
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

// FUNCTION: 0x0047BDB0
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

// FUNCTION: 0x0047C060
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

// FUNCTION: 0x0047C3D0
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

// FUNCTION: 0x0047C4D0
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

// FUNCTION: 0x0047C520
BuildingClass * CellClass::Cell_Building(void) const
{
	return((BuildingClass *)Cell_Find_Object(RTTI_BUILDING));
}

// FUNCTION: 0x0047C550
TerrainClass * CellClass::Cell_Terrain(bool bridge) const
{
	return((TerrainClass *)Cell_Find_Object(RTTI_TERRAIN, bridge));
}

// FUNCTION: 0x0047C5A0
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

// FUNCTION: 0x0047D210
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

// FUNCTION: 0x004810A0
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

// FUNCTION: 0x00485200
HRESULT STDMETHODCALLTYPE CellClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_CellClass;
	return(S_OK);
}

// FUNCTION: 0x00487E50
int STDMETHODCALLTYPE CellClass::What_Am_I(void) const
{
	return(RTTI_CELL);
}
