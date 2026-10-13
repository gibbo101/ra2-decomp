// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "objtype.h"
#include "rgb.h"
#include "vector.h"

#include "anim.hh"
#include "isotype.hh"
#include "land.hh"

class LightConvertClass;
class Surface;
class ShapeSet;

#pragma pack(4)
struct IsoTileRecord
{

	int X;
	int Y;

	int ExtraOffset;

	int ZDataOffset;

	int ExtraZOffset;

	int ExtraX;
	int ExtraY;
	int ExtraWidth;
	int ExtraHeight;

	unsigned int IsHasExtraData:1;

	unsigned int IsHasZData:1;

	unsigned int IsRandomized:1;

	unsigned char Height;

	signed char TileType;

	signed char RampType;

	RGBStruct LowColor;
	RGBStruct HighColor;
};
#pragma pack()

#pragma pack(4)
class IsoTileSet
{
	friend class IsometricTileClass;
	friend class IsometricTileTypeClass;

	public:
		operator void *() const { return(*this); }

		IsoTileRecord const * Fetch_Record_Pointer(int index) const
		{
			return(Tiles[index % Tile_Count()]);
		}
		IsoTileRecord const * Fetch_Record_Pointer_Unsafe(int index) const
		{
			return(Tiles[index]);
		}

		int Map_Width(void) const { return(MapWidth); }
		int Map_Height(void) const { return(MapHeight); }
		int Tile_Count(void) const { return(MapWidth * MapHeight); }
		int Pixel_Width(void) const { return(Width); }
		int Pixel_Height(void) const { return(Height); }

		int Get_Highest_Height(void) const
		{
			int height = 0;
			for (int index = 0; index < Tile_Count(); ++index) {
				IsoTileRecord const * record = Fetch_Record_Pointer(index);
				if (record != NULL) {
					if (record->Height > height)
						height = record->Height;
				}
			}
			return(height);
		}

	protected:

		int MapWidth;
		int MapHeight;

		int Width;
		int Height;

		IsoTileRecord *Tiles[1];

	private:
		IsoTileSet(void) {};
		IsoTileSet(IsoTileSet const & rvalue);
		IsoTileSet const & operator = (IsoTileSet const & rvalue);
};
#pragma pack()

class IsometricTileTypeClass : public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:
		IsometricTileTypeClass(IsometricTileType type = ISOTILE_CLEAR, int unknown1 = 0, unsigned char unknown2 = 0, char const *ininame = NULL, bool skip_registration = false);
		IsometricTileTypeClass(NoInitClass const &x);
		virtual ~IsometricTileTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_ISOTILETYPE);}
		virtual int Fetch_Object_Size(bool = false) const override {return(sizeof(*this));}
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);};

		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual Coord const Coord_Fixup(Coord const & coord) const override;
		virtual bool Create_And_Place(Cell const & cell, HouseClass * house) const override;
		virtual ObjectClass * Create_One_Of(HouseClass *house) const override;
		virtual Cell const * Occupy_List(bool placement=false) const override;

		static IsometricTileType From_Name(char const * name);

		virtual void const * Get_Image_Data(void) const override;

		LandType Land_Type(int tile) const;
		bool Is_Tile_Index_Valid(int tile, bool load);
		bool Load_Tile_Image(void);
		IsometricTileTypeClass *Next_Tile_From_Set(int index);
		static IsometricTileType Fixup_Tile_Type(IsometricTileType current_type);
		static LightConvertClass * Find_Or_Make_Drawer(int r, int g, int b);
		static bool Free_Unused_Drawers(int time_budget_ms, bool force);
		static void Init_Drawers(void);
		static void Read_Control_File(TheaterType theater, bool from_ccfile);
		static void Load_Tiles(bool,bool);
		int Load_Tile_Data(void);
		static void Clear_Use_Counts(void);
		bool Get_Tile_Pixel_Dimensions(int tile, int & width, int & height);
		int Ramp_Type(int tile) const;
		bool Is_Randomized(int tile) const;
		void Draw_Shadow_Caster(int index, Surface * surf, Point2D pt, Rect rect, int offset);
		Cell const * Shadow_Caster_List(void) const;
		void Draw_Tile(LightConvertClass * drawer, int subtile, Surface & surface, int x, int y, Rect cliprect, int height, int brightness, bool use_z, int cell_variation, bool fill, bool depth_only, bool fog, signed int fog_color) const;
		bool Get_Tile_Image(int tilenum, unsigned char **buffer, int width, int height);
		int Get_Y_Offset(int index);
		static void Clear_Z_Shapes(void);
		unsigned short Preview_Tile_Color(int tile, int level, bool use_high_color);
		unsigned short * Preview_Tile(int tile, int level);
		void Build_Preview_Tiles(void);

		int SubTile_Index(int x, int y) const { return(x + (Width * y)); }

	public:

		IsometricTileType HeapID;

		IsometricTileType MarbleMadness;

		IsometricTileType NonMarbleMadness;

		int TileSetBaseID;

		DynamicVectorClass<unsigned short *> PreviewTiles;

		IsometricTileTypeClass *NextTileTypeInSet;

		int ToSnowTheater;

		int ToTemperateTheater;

		AnimType Anim;
		Point2D Offset;
		int AttachesTo;
		int ZAdjust;

		int Unused1;

		bool IsMorphable;

		bool IsShadowCaster;

		bool IsAllowToPlace;

		bool IsRequiredForRMG;

		int Width;
		int Height;

		int Unused2;

		int NumTileTypesInSet;

		bool IsFileLoaded;

		char Filename[15+1];

		bool IsAllowBurrowing;

		bool IsAllowTiberium;

		int UseCount;

	public:

		static IsometricTileType RampStart;
		static IsometricTileType RampSmooth;
		static IsometricTileType MMRampBase;

		static IsometricTileType ClearTile;

		static IsometricTileType RoughTile;
		static IsometricTileType SandTile;
		static IsometricTileType GreenTile;
		static IsometricTileType PaveTile;

		static IsometricTileType MiscPaveTile;

		static IsometricTileType ClearToRoughLat;
		static IsometricTileType ClearToSandLat;
		static IsometricTileType ClearToGreenLat;
		static IsometricTileType ClearToPaveLat;

		static IsometricTileType HeightBase;
		static IsometricTileType BlackTile;

		static IsometricTileType BridgeSet;
		static IsometricTileType TrainBridgeSet;

		static IsometricTileType CliffSet;

		static IsometricTileType ShorePieces;

		static IsometricTileType WaterSet;

		static IsometricTileType Ice1Set;
		static IsometricTileType Ice2Set;
		static IsometricTileType Ice3Set;

		static IsometricTileType IceShoreSet;

		static IsometricTileType SlopeSetPieces;
		static IsometricTileType SlopeSetPieces2;

		static IsometricTileType MonorailSlopes;

		static IsometricTileType Tunnels;
		static IsometricTileType TrackTunnels;
		static IsometricTileType DirtTunnels;
		static IsometricTileType DirtTrackTunnels;

		static IsometricTileType WaterfallEast;
		static IsometricTileType WaterfallWest;
		static IsometricTileType WaterfallNorth;
		static IsometricTileType WaterfallSouth;

		static IsometricTileType CliffRamps;

		static IsometricTileType PavedRoads;

		static IsometricTileType PavedRoadEnds;

		static IsometricTileType Medians;

		static IsometricTileType RoughGround;

		static IsometricTileType DirtRoadJunction;

		static IsometricTileType DirtRoadCurve;

		static IsometricTileType DirtRoadStraight;

		static IsometricTileType DestroyableCliffs;

		static IsometricTileType WaterCaves;

		static IsometricTileType WaterCliffs;

		static IsometricTileType PavedRoadSlopes;

		static IsometricTileType DirtRoadSlopes;

		static IsometricTileType Rocks;

		static IsometricTileType CrystalTile;

		static IsometricTileType ClearToCrystalLat;

		static IsometricTileType CrystalCliff;

		static IsometricTileType SwampTile;

		static IsometricTileType WaterToSwampLat;

		static IsometricTileType BlueMoldTile;

		static IsometricTileType ClearToBlueMoldLat;

		static IsometricTileType BridgeTopLeft1;
		static IsometricTileType BridgeTopLeft2;
		static IsometricTileType BridgeBottomRight1;
		static IsometricTileType BridgeBottomRight2;
		static IsometricTileType BridgeTopRight1;
		static IsometricTileType BridgeTopRight2;
		static IsometricTileType BridgeBottomLeft1;
		static IsometricTileType BridgeBottomLeft2;

		static IsometricTileType BridgeMiddle1;

		static IsometricTileType BridgeMiddle2;

		static IsometricTileType ShadowCasterTiles[5];

		static void * CellShadowShapes;

		struct TileInsertType {

			int Count;

			int Offset;
		};

		static DynamicVectorClass<TileInsertType *> TileInsertTypes;
};

#define TILE_CLEAR IsometricTileTypeClass::ClearTile

extern ShapeSet * SlopeZShapes[4];

inline IsometricTileType operator += (IsometricTileType & t1, int t2)
{
	t1 = (IsometricTileType)(((int)t1 + (int)t2));
	return(t1);
}

IsometricTileType Pick_Ice_Tile_Set(void);

inline Point2D IsoTile_Dimensions(const IsometricTileTypeClass * isotype)
{
	int w = ((IsoTileSet const *)isotype->Get_Image_Data())->Map_Width();
	int h = ((IsoTileSet const *)isotype->Get_Image_Data())->Map_Height();
	return(Point2D(w,h));
}

inline Point2D IsoTile_Dimensions2(const IsoTileSet * set)
{
	int w = set->Map_Width();
	int h = set->Map_Height();
	return(Point2D(w,h));
}
