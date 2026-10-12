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
