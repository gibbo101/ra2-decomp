// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_map.h"
#include "abstract.h"
#include "globals.h"
#include "map.h"
#include "rect.h"

#include "isotype.hh"
#include "land.hh"
#include "mzone.hh"
#include "overlay.hh"
#include "smudge.hh"
#include "tiberium.hh"

class FoggedObjectClass;
class BuildingTypeClass;
class FootClass;
class PixelFXClass;
class RadSiteClass;
class LightConvertClass;
class TagClass;
class TiberiumClass;
class BuildingClass;
class UnitClass;
class TechnoClass;
class TerrainClass;
class IsometricTileTypeClass;
template<class T> class DynamicVectorClass;

typedef DynamicVectorClass<FoggedObjectClass *> FOGGED_OBJECT_LIST;

class CellClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:

		Cell CellID;

		FOGGED_OBJECT_LIST * FoggedObjects;

		CellClass * BridgeDeckCell;

		CellClass * UnusedCell;

		LightConvertClass * Drawer;

		IsometricTileType ITType;

		TagClass * Tag;

		BuildingTypeClass * Rubble;

		OverlayType Overlay;

		SmudgeType Smudge;

		PassabilityType Passability;

		HousesType Owner;

		HousesType InfType;
		HousesType BridgeInfType;

		int LastRedrawFrame;

		int LastUnknownDrawFrame;

		int LastBridgeDrawFrame;
		Rect LastBridgeDrawRect;

		unsigned CloakedBy;
		unsigned short SensorsOfHouses[24];
		unsigned short DisguiseSensorsOfHouses[24];
		unsigned OccupiedBy;

		FootClass * Jumpjet;

	private:

		ObjectClass * OccupierPtr;
		ObjectClass * BridgeOccupierPtr;

		LandType Land;

	public:

		double RadLevel;
		RadSiteClass * RadSite;
		PixelFXClass * PixelFX;
		int OccupyHeightsCoveringMe;

		int Intensity;
		short Ambient;
		short Brightness;
		short TileBrightness;
		short AltBrightness;
		short RedTint;
		short GreenTint;
		short BlueTint;

		short Tube;

		unsigned char LastBridgeDrawRedraws;

		unsigned char IsIceGrowthAllowed;

		unsigned char SubTile;

		char Height;

		unsigned char Ramp;

		char Elevation;

		unsigned char OverlayData;

		unsigned char SmudgeData;

		char ShadowFrame;
		char FogFrame;

		unsigned char AdjacentObjectCount;

		union {
			struct {
				unsigned Center:1;
				unsigned NW:1;
				unsigned NE:1;
				unsigned SW:1;
				unsigned SE:1;
				unsigned Vehicle:1;
				unsigned Monolith:1;
				unsigned Building:1;
			} Occupy;
			unsigned char Composite;
		} Flag;

		union {
			struct {
				unsigned Center:1;
				unsigned NW:1;
				unsigned NE:1;
				unsigned SW:1;
				unsigned SE:1;
				unsigned Vehicle:1;
				unsigned Monolith:1;
				unsigned Building:1;
			} Occupy;
			unsigned char Composite;
		} BridgeFlag;

		unsigned IsMapped:1;
		unsigned IsVisible:1;
		unsigned IsFogMapped:1;
		unsigned IsFogVisible:1;
		unsigned IsAltUnknown:1;
		unsigned : 27;

		int ShroudCounter;

		int GapsCoveringThisCell;

		bool VisibilityChanged;

		unsigned unknown_13C;

		unsigned IsPlot:1;

		unsigned IsCursorHere:1;

		unsigned IsWaypoint:1;

		unsigned IsRadarCursor:1;

		unsigned IsFlagged:1;

		unsigned IsToShroud:1;

		unsigned IsToFog:1;

		unsigned IsBridgeDeck:1;

		unsigned IsUnderBridge:1;

		unsigned IsBridgeTraversable:1;

		unsigned WasUnderBridge:1;

		unsigned IsBridgeEastWest:1;

		unsigned IsBridgeSurface:1;

		unsigned IsBridgeDamaged:1;

		unsigned IsToGrowIce:1;

		unsigned IsToGrowVeins:1;

		unsigned IsOvershadowed:1;

		unsigned IsAnimAttached:1;

		unsigned IsPredictedPath:1;

		unsigned IsAffectedByEMP:1;

		unsigned IsHorizontalLine:1;

		unsigned IsVerticalLine:1;

		unsigned IsFogged:1;

		CellClass(void);
		CellClass(NoInitClass const & x);
		virtual ~CellClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual int STDMETHODCALLTYPE What_Am_I(void) const override;

		void Detach(AbstractClass const * target);

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;

		LEPTON Get_Height(Point2D const & point = Point2D(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2)) const;
		bool Can_Tiberium_Germinate(TiberiumClass const * tiberium) const;
		bool Can_Tiberium_Grow(void) const;
		bool Can_Tiberium_Spread(void) const;
		bool Is_Bridge_Here(void) const {return(IsUnderBridge);}
		int GetLevel(void) const;
		BuildingClass * Cell_Building(void) const;
		Cell Fetch_CellID(void) const {return(CellID);}
		void Set_CellID(Cell const & cell);
		Coord Cell_Coord(void) const;
		Coord Closest_Free_Spot(Coord const & coord, bool any=false, bool bridge=false, bool use_cell_coord=false) const;
		Coord Free_Spot(void) const {return(Closest_Free_Spot(Cell_Coord()));}
		CellClass & Adjacent_Cell(FacingType face) {return((CellClass &)((*((CellClass const *)this)).Adjacent_Cell(face)));}
		CellClass const & Adjacent_Cell(FacingType face) const;
		AircraftClass * Cell_Aircraft(bool bridge=false) const;
		InfantryClass * Cell_Infantry(bool bridge=false) const;
		LandType Land_Type(void) const {return(Land);}
		ObjectClass * Cell_Find_Object(RTTIType rtti, bool bridge=false) const;
		ObjectClass * Cell_Object(Point2D const & xy=Point2D(0, 0), bool bridge=false) const;
		ObjectClass * Cell_Occupier(bool bridge=false) const {return(bridge ? BridgeOccupierPtr : OccupierPtr);}
		ObjectClass * Cell_Bridge_Occupier(void) const { return(BridgeOccupierPtr); }
		TechnoClass * Cell_Techno(Point2D const & xy=Point2D(0, 0), bool bridge=false, TechnoClass const * notthis=NULL) const;
		TerrainClass * Cell_Terrain(bool bridge=false) const;
		UnitClass * Cell_Unit(bool bridge=false) const;
		bool Goodie_Check(FootClass * object);
		bool Is_Clear_To_Build(SpeedType loco = SPEED_TRACK, BuildingTypeClass * what = NULL, HouseClass * who = NULL) const;
		bool Is_Clear_To_Move(SpeedType loco, bool ignoreinfantry, bool ignorevehicles, int zone=-1, MZoneType check=MZONE_NORMAL, int cell_height=-1, bool checkbridge=true) const;
		bool Is_Spot_Free(int spot_index, bool bridge=false) const;
		void Cell_Color(RGBClass & lowcolor, RGBClass & highcolor) const;
		int Preview_Cell_Color(unsigned char & unknown, bool terrainonly) const;
		int Clear_Icon(IsometricTileType type, int seq_length) const;
		static int Spot_Index(Coord const & coord);
		RadSiteClass * FUN_00487C80(void);
		bool Has_Tunnel(void) const;
		bool Is_Near_Tunnel_NW(void) const;
		bool Is_Near_Tunnel_ES(void) const;
		bool Can_Enter_Tunnel(FootClass const * foot) const;
		TubeClass * Get_Tunnel(void) const;
		char const * Tiberium_Name(void) const;
		TiberiumType Tiberium_Type_Here(void) const;
		bool Place_Tiberium(TiberiumType tib, int data);
		int Tiberium_Value(void) const;
		LEPTON Occupier_Height(void) const;
		FacingType Bounce_Direction(Coord const & target_coord) const;

		virtual bool On_Ground(void) const override;
		virtual Coord Center_Coord(void) const override;
		virtual Coord As_Coord(void) const override;

		bool Is_Cloaked(HousesType house) const;
		bool Is_Sensed(HousesType house) const;
		void Cloaked_By(HousesType house);
		void Uncloaked_By(HousesType house);
		void Sensed_By(HousesType house);
		void Unsensed_By(HousesType house);
		bool Should_Draw_As_Cloaked(HousesType house) const;

		int Get_Vein_Frame(void) const;
		bool Can_Place_Veins(void);
		void Redraw_Veins(void);
		void Place_Veins(void);
		void Trigger_Veins(void);

		bool Is_Tile_Water(void) const;
		bool Is_Tile_Clear(void) const;
		bool Is_Tile_Ramp(void) const;
		bool Is_Tile_Cliff(void) const;
		bool Is_Tile_Shore(void) const;
		bool Is_Tile_With_Water(void) const;
		bool Is_Tile_Swamp(void) const;
		bool Is_Tile_Misc_Pavement(void) const;
		bool Is_Tile_Pavement(void) const;
		bool Is_Tile_Dirt_Road(void) const;
		bool Is_Tile_Paved_Road(void) const;
		bool Is_Tile_Paved_Road_End(void) const;
		bool Is_Tile_Paved_Road_Slope(void) const;
		bool Is_Tile_Road_Median(void) const;
		bool Is_Tile_Ice(void) const;
		bool Is_Tile_Bridge(void) const;
		bool Is_Tile_Train_Bridge(void) const;
		bool Is_Tile_Clear_To_Sand_LAT(void) const;
		bool Is_Tile_Clear_To_Green_LAT(void) const;
		bool Is_Tile_Destroyable_Cliff(void) const;

		bool Fixup_LAT(void);

		bool Is_Fogged(void) const;
		bool Is_Shrouded(void) const;
		bool Can_Build_Here(void) const;
		int Occupation_Mask(HousesType house) const;

		void Occupy_Down(ObjectClass * object, bool bridge=false);
		void Occupy_Up(ObjectClass * object, bool bridge=false);
		bool Flag_Place(HousesType house);
		bool Flag_Remove(void);

		void Wipe_Depth(Point2D const & point, Rect const & cliprect);
		void Draw_Shroud_And_Fog(Point2D const & point, Rect const & cliprect);
		void Draw_Shadow_Cast(Point2D const & point, Rect const & cliprect);
		void Draw_It(Point2D const & xy, Rect const & cliprect, bool objects=false) const;
		void Shimmer(void);
		void Init_Drawer(LightConvertClass * drawer = NULL, int intensity = 0x10000, int ambient = 0, int brightness = 1000, int tile_brightness = 1000, int alt_brightness = 1000);
		void Pick_Drawer(LightConvertClass * & drawer, int & intensity, int & ambient, int & brightness, int & tile_brightness, int & alt_brightness) const;
		void Inc_Drawer_Ref_Count(LightConvertClass * convert) const;
		void Dec_Drawer_Ref_Count(LightConvertClass * convert) const;
		Rect Overlay_Render_Rect(void) const;
		Rect Overlay_Shadow_Render_Rect(void) const;
		Rect Cell_Render_Rect(void) const;
		bool Draw_Placement_Cursor(Point2D const & xpoint, Rect const & cliprect, bool zeroalpha);
		void Draw_Shroud_Or_Fog_Shape(Point2D const & xpoint, Rect const & cliprect, int index);
		void Draw_Fog_Shape(Point2D const & xpoint, Rect const & cliprect, int index);
		void Draw_Overlay_Shadow(Point2D const & xpoint, Rect const & cliprect);
		void Draw_Overlay(Point2D const & point, Rect const & cliprect);
		Point2D Overlay_Draw_Offset(void) const;
		void Register_As_Dirty(void);
		void Fetch_Icon(IsometricTileTypeClass *& ittype, int& subtile, int * icon, bool doicon) const;

		bool Grow_Tiberium(void);
		bool Spread_Tiberium(bool forced=false);
		int Tiberium_Adjust(bool pregame=false);
		void Set_Wall_Owner(void);
		void Wall_Update(bool justneighbors=false);
		bool Has_Wall_Or_Gate(OverlayType type=OVERLAY_NONE, FacingType facing=FACING_NONE) const;
		BuildingClass * Get_Gate(void) const;
		void Recalc_Attributes(int cell_height=-1);
		void Recalc_Passability(void);
		void Destroy_Bridge(void);
		void On_Bridge_Collapse(void);
		void Set_Under_Bridge(FacingType facing, bool state = true);
		void Set_Under_Rail_Bridge(FacingType facing, bool state = true);
		int  Reduce_Tiberium(int levels);
		int  Reduce_Weed(void);
		int  Reduce_Wall(int damage);
		void Incoming(Coord const & threat=COORD_NONE, bool forced=false, bool nokidding=false, bool bridge=false);
		void Adjust_Threat(HousesType house, int threat_value);
		void Force_New_Slope_For_Occupiers(void) const;
		void Kill_Illegal_Occupiers(void);

		Cell Get_Bridge_Deck_Cell(void) const
		{
			if (IsBridgeDeck) {
				return(Fetch_CellID());
			} else {
				return(BridgeDeckCell->Fetch_CellID());
			}
		}

		CellClass * Get_Bridge_Deck_CellClass(void) const
		{
			CellClass * bridge_deck = NULL;
			if (IsUnderBridge) {
				if (IsBridgeDeck) {
					bridge_deck = &Map[Fetch_CellID()];
				} else {
					bridge_deck = &Map[BridgeDeckCell->Fetch_CellID()];
				}
			}
			return(bridge_deck);
		}

		bool Is_Overlay_Low_Bridge(void) const { return(Overlay >= OVERLAY_LOWBRIDGE_01 && Overlay <= OVERLAY_LOWBRIDGE_26); }
		bool Is_Overlay_Bridge(void) const { return(Overlay == OVERLAY_BRIDGE1 || Overlay == OVERLAY_BRIDGE2); }
		bool Is_Overlay_Rail_Bridge(void) const { return(Overlay == OVERLAY_RAIL_BRIDGE1 || Overlay == OVERLAY_RAIL_BRIDGE2); }

		void Attach_Tag(TagClass *tag);

		void Fog_Cell(void);
		void Unfog_Cell(void);
		void Remove_Fogged_Objects(void);

		void Register_For_Redraw(void);

		bool Can_Burrow_Here(void) const;

		void Init_Light(int & intensity, int & ambient, int & brightness, int & tile_brightness, int & alt_brightness, int & red_tint, int & green_tint, int & blue_tint) const;
		void Recalc_Light(void);

	private:
		CellClass (CellClass const &);
};

inline CellClass * AbstractClass::As_CellClass(void)
{
	return(dynamic_cast<CellClass *>(this));
}

inline CellClass const * AbstractClass::As_CellClass(void) const
{
	return(dynamic_cast<CellClass const *>(this));
}
