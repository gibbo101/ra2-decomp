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

// FUNCTION: 0x0047DD70
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

// FUNCTION: 0x0047E8A0
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

// FUNCTION: 0x0047EA90
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

// FUNCTION: 0x0047EBA0
UnitClass * CellClass::Cell_Unit(bool bridge) const
{
	return((UnitClass*)Cell_Find_Object(RTTI_UNIT, bridge));
}

// FUNCTION: 0x0047EBF0
AircraftClass * CellClass::Cell_Aircraft(bool bridge) const
{
	return((AircraftClass*)Cell_Find_Object(RTTI_AIRCRAFT, bridge));
}

// FUNCTION: 0x0047EC40
InfantryClass * CellClass::Cell_Infantry(bool bridge) const
{
	return((InfantryClass*)Cell_Find_Object(RTTI_INFANTRY, bridge));
}

struct FileSystem {
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
		void * GetPixels(int frame) const;
};

// FUNCTION: 0x0047F250
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

	unsigned char * shapeptr = (unsigned char *)shapes->GetPixels(shapenum);
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

// FUNCTION: 0x0047F510
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

// FUNCTION: 0x0047FB90
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

// FUNCTION: 0x0047FDE0
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

// FUNCTION: 0x0047FF80
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

// FUNCTION: 0x00480110
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

// FUNCTION: 0x00480180
void CellClass::Wipe_Depth(Point2D const & point, Rect const & cliprect)
{
	IsometricTileTypes[ISOTILE_CLEAR]->Draw_Tile(TileDrawers[0], 0, *DSurface::Temp, point.X, point.Y, cliprect, Height, TileBrightness, 1, 0, false, true, false, false);
}

// FUNCTION: 0x004801F0
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

// FUNCTION: 0x004802A0
void CellClass::Draw_Shadow_Cast(Point2D const & point, Rect const & cliprect)
{
	int subtile;
	IsometricTileTypeClass * isotype;

	Fetch_Icon(isotype, subtile, NULL, false);
	if (isotype->IsShadowCaster) {
		isotype->Draw_Shadow_Caster(subtile, DSurface::Temp, Point2D(point.X, point.Y - 15 * Height), cliprect, -2 - 15 * (Height - 4));
	}
}

// FUNCTION: 0x00480510
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

// FUNCTION: 0x00480A30
Coord CellClass::Cell_Coord(void) const
{
	Coord coord(Fetch_CellID());
	coord.Z = Get_Height();
	return(coord);
}

// FUNCTION: 0x00480CB0
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

// FUNCTION: 0x00481130
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

// FUNCTION: 0x004814A0
BuildingClass * CellClass::Get_Gate(void) const
{
	BuildingClass * building = Cell_Building();
	if (building != NULL && !building->Class->IsGate) {
		building = NULL;
	}
	return(building);
}

// FUNCTION: 0x004814F0
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

// FUNCTION: 0x00481670
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

// FUNCTION: 0x00481810
CellClass const & CellClass::Adjacent_Cell(FacingType face) const
{
	if ((unsigned)face >= FACING_COUNT) {
		return(*this);
	}

	Cell newcell = ::Adjacent_Cell(Fetch_CellID(), face);
	return(Map[newcell]);
}

// FUNCTION: 0x00481870
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

// FUNCTION: 0x004818E0
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

// FUNCTION: 0x00483410
bool CellClass::Flag_Place(HousesType house)
{
	if (!IsFlagged && Is_Clear_To_Move(SPEED_TRACK, false, false)) {
		IsFlagged = true;
		Owner = house;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00483460
bool CellClass::Flag_Remove(void)
{
	if (IsFlagged) {
		IsFlagged = false;
		Owner = HOUSE_NONE;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00483480
void CellClass::Shimmer(void)
{
	ObjectClass * object = Cell_Occupier();

	while (object) {
		object->Do_Shimmer();
		object = object->Next;
	}
}

// FUNCTION: 0x004834A0
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

// FUNCTION: 0x00483610
bool CellClass::Is_Bridge_Here(void) const
{
	return(IsUnderBridge);
}

// FUNCTION: 0x00483620
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

// FUNCTION: 0x00483690
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

// FUNCTION: 0x00483710
bool CellClass::Grow_Tiberium(void)
{
	if (Can_Tiberium_Grow()) {
		return(Place_Tiberium(Overlay_Tiberium(this), 1));
	}
	return(false);
}

// FUNCTION: 0x00483780
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

// FUNCTION: 0x004838E0
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

// FUNCTION: 0x004839F0
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

// FUNCTION: 0x00483C10
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

// FUNCTION: 0x00483C80
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

// FUNCTION: 0x00483E30
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

// FUNCTION: 0x00484050
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

// FUNCTION: 0x00484180
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

// FUNCTION: 0x00484680
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

// FUNCTION: 0x00484A80
void CellClass::Inc_Drawer_Ref_Count(LightConvertClass * convert) const
{
	convert->Add_Reference();
}

// FUNCTION: 0x00484A90
void CellClass::Dec_Drawer_Ref_Count(LightConvertClass * convert) const
{
	if (GameActive) {
		convert->Remove_Reference();
	}
}

// FUNCTION: 0x00484AB0
bool CellClass::Has_Tunnel(void) const
{
	return(Tube >= TUBE_FIRST && Tube < Tubes.Count() && Land == LAND_TUNNEL);
}

// FUNCTION: 0x00484D60
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

// FUNCTION: 0x00484F10
bool CellClass::Can_Enter_Tunnel(FootClass const * foot) const
{
	return(true);
}

// FUNCTION: 0x00484F20
TubeClass * CellClass::Get_Tunnel(void) const
{
	if (Tube >= TUBE_FIRST && Tube < Tubes.Count()) {
		return(Tubes[Tube]);
	}
	return(NULL);
}

// FUNCTION: 0x00484F50
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

// FUNCTION: 0x00484FF0
wchar_t const * CellClass::Tiberium_Name(void) const
{
	TiberiumType tib = Which_Tiberium_Type(Overlay);
	if (tib != TIBERIUM_NONE) {
		return(Tiberiums[tib]->UIName);
	}
	return(NULL);
}

// FUNCTION: 0x00485010
TiberiumType CellClass::Tiberium_Type_Here(void) const
{
	return(Which_Tiberium_Type(Overlay));
}

// FUNCTION: 0x00485020
int CellClass::Tiberium_Value(void) const
{
	TiberiumType tib = Which_Tiberium_Type(Overlay);
	if (tib == TIBERIUM_NONE) {
		return(0);
	}
	return(Tiberiums[tib]->CreditValue * (OverlayData + 1));
}

// FUNCTION: 0x00485060
bool CellClass::Is_Tile_Water(void) const
{
	return(ITType >= IsometricTileTypeClass::WaterSet && ITType < IsometricTileTypeClass::WaterSet + 14);
}

// FUNCTION: 0x00485080
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

// FUNCTION: 0x00485130
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

// FUNCTION: 0x00485200
HRESULT STDMETHODCALLTYPE CellClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_CellClass;
	return(S_OK);
}

// FUNCTION: 0x00485240
void CellClass::Set_CellID(Cell const & cell)
{
	CellID = cell;
}

// FUNCTION: 0x00485250
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

// FUNCTION: 0x004852D0
void CellClass::Register_As_Dirty(void)
{
	Point2D point;
	TacticalMap->Coord_To_Pixel(Coord_Whole(CellID), point);
	point.Y += TacticalRect.Y - 15 * Height;
	point -= Point2D(30, 15);
	TacticalMap->Register_Dirty_Area(Rect(point, 60, 30));
}

// FUNCTION: 0x00485390
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

// FUNCTION: 0x00485460
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

// FUNCTION: 0x00485590
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

// FUNCTION: 0x00486380
bool CellClass::Is_Tile_Clear(void) const
{
	if (ITType == ISOTILE_NONE || ITType == ISOTILE_CLEAR) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x004863A0
bool CellClass::Is_Tile_Ramp(void) const
{
	if (IS_TILE_IN_SET(RampStart, RAMP_BASE_COUNT) ||
		IS_TILE_IN_SET(RampSmooth, RAMP_SMOOTH_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x004863D0
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

// FUNCTION: 0x004865B0
bool CellClass::Is_Tile_Shore(void) const
{
	if (IS_TILE_IN_SET(ShorePieces, SHORE_PIECES_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x004865D0
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

// FUNCTION: 0x00486650
bool CellClass::Is_Tile_Misc_Pavement(void) const
{
	if (IS_TILE_IN_SET(MiscPaveTile, MISC_PAVE_TILE_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x00486670
bool CellClass::Is_Tile_Pavement(void) const
{
	if (IS_TILE_IN_SET(PaveTile, PAVE_TILE_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x00486690
bool CellClass::Is_Tile_Dirt_Road(void) const
{
	if (IS_TILE_IN_SET(DirtRoadJunction, DIRT_ROAD_JUNCTION_COUNT) ||
		IS_TILE_IN_SET(DirtRoadCurve, DIRT_ROAD_CURVE_COUNT) ||
		IS_TILE_IN_SET(DirtRoadStraight, DIRT_ROAD_STRAIGHT_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x004866D0
bool CellClass::Is_Tile_Paved_Road(void) const
{
	if (IS_TILE_IN_SET(PavedRoads, PAVED_ROAD_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x004866F0
bool CellClass::Is_Tile_Paved_Road_End(void) const
{
	if (IS_TILE_IN_SET(PavedRoadEnds, PAVED_ROAD_ENDS_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x00486710
bool CellClass::Is_Tile_Paved_Road_Slope(void) const
{
	if (IS_TILE_IN_SET(PavedRoadSlopes, PAVED_ROAD_SLOPES_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x00486730
bool CellClass::Is_Tile_Road_Median(void) const
{
	if (IS_TILE_IN_SET(Medians, 14)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x00486750
bool CellClass::Is_Tile_Bridge(void) const
{
	if (IS_TILE_IN_VALID_SET(BridgeSet, BRIDGE_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x00486770
bool CellClass::Is_Tile_Train_Bridge(void) const
{
	if (IS_TILE_IN_VALID_SET(TrainBridgeSet, TRAIN_BRIDGE_COUNT)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x00486790
bool CellClass::Is_Tile_Clear_To_Sand_LAT(void) const
{
	if (IS_TILE_IN_SET(ClearToSandLat, 16)) {
		return(true);
	}

	return(false);
}

// FUNCTION: 0x004867B0
bool CellClass::Is_Tile_Clear_To_Green_LAT(void) const
{
	if (ITType == IsometricTileTypeClass::GreenTile || (ITType >= IsometricTileTypeClass::ClearToGreenLat && ITType < IsometricTileTypeClass::ClearToGreenLat + 16)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004867E0
bool CellClass::Is_Tile_Not_Water(void) const
{
	if (ITType >= IsometricTileTypeClass::WaterSet && ITType < IsometricTileTypeClass::WaterSet + 14) {
		return(false);
	}

	return(true);
}

inline bool CellClass::Is_Cloaked(HousesType house) const
{
	return((CloakedBy & (1 << house)) != 0);
}

inline bool CellClass::Is_Sensed(HousesType house) const
{
	return((short)SensorsOfHouses[house] > 0);
}

// FUNCTION: 0x00486800
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

// FUNCTION: 0x00486840
Coord CellClass::Center_Coord(void) const
{
	Point2D pt = Point2D((CellID.X * CELL_LEPTON_W) + CELL_LEPTON_W / 2, (CellID.Y * CELL_LEPTON_H) + CELL_LEPTON_H / 2);
	Coord coord;
	coord.X = pt.X;
	coord.Y = pt.Y;
	coord.Z = Get_Height(pt);
	return(coord);
}

// FUNCTION: 0x00486890
Coord CellClass::As_Coord(void) const
{
	if (IsUnderBridge) {
		return(Center_Coord() + Coord(0, 0, BRIDGE_LEPTON_HEIGHT));
	}

	return(Center_Coord());
}

// FUNCTION: 0x00486900
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

// FUNCTION: 0x00486920
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

// FUNCTION: 0x00486A70
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

// FUNCTION: 0x00486BF0
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

// FUNCTION: 0x00486C50
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

// FUNCTION: 0x00487D50
int CellClass::GetLevel(void) const
{
	return(Height + (IsBridgeDeck ? BRIDGE_CELL_HEIGHT : 0));
}

// FUNCTION: 0x00487E50
int STDMETHODCALLTYPE CellClass::What_Am_I(void) const
{
	return(RTTI_CELL);
}
