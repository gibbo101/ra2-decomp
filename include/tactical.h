// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "matrix3d.h"
#include "rect.h"
#include "stimer.h"
#include "timer.h"

template<class T> class DynamicVectorClass;
class CellClass;
class Cell;
class ShadowControlClass;
class Surface;
class ColorScheme;

enum DrawPassType {
	DRAW_PASS_PAN = 0,
	DRAW_PASS_BACKGROUND = 1,
	DRAW_PASS_FOREGROUND = 2,
	DRAW_PASS_FULL = 3
};

class ShadowControlClass
{
	public:
		ShadowControlClass(void) {}
		ShadowControlClass(ShadowControlClass const & that)
		{
			Unknown1 = that.Unknown1;
			Unknown2 = that.Unknown2;
			Unknown3 = that.Unknown3;
			Unknown4 = that.Unknown4;
			Unknown5 = that.Unknown5;
			Unknown6 = that.Unknown6;
			Unknown7 = that.Unknown7;
		}

		int Unknown1;
		int Unknown2;
		int Unknown3;
		int Unknown4;
		int Unknown5;
		Rect Unknown6;
		int Unknown7;
};

struct DirtyAreaStruct
{

	Rect Area;

	bool IsToRefreshShroud;

	DirtyAreaStruct(void) : Area(0,0,0,0), IsToRefreshShroud(false) {}
	DirtyAreaStruct(Rect const & area, bool refresh_shroud) : Area(area), IsToRefreshShroud(refresh_shroud) {}

	DirtyAreaStruct(DirtyAreaStruct const & that) : Area(that.Area), IsToRefreshShroud(that.IsToRefreshShroud) {}

	bool operator == (DirtyAreaStruct const &that) const { return(IsToRefreshShroud == that.IsToRefreshShroud && Area == that.Area); }
	bool operator != (DirtyAreaStruct const &that) const { return(IsToRefreshShroud != that.IsToRefreshShroud || Area != that.Area); }

};

class Tactical : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:

		Tactical(void);
		Tactical(const NoInitClass & noinit);
		virtual ~Tactical(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_TACTICALMAP);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		virtual void AI(void) override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;

		static int Z_Lepton_To_Pixel(LEPTON lepton);
		static LEPTON Pixel_To_Z_Lepton(int pixel);

		bool Coord_To_Pixel(Coord const & coord, Point2D & pixel);
		Coord Pixel_To_Coord(Point2D const & pixel);
		Coord Pixel_To_Coord_Absolute(Point2D const & pixel);
		Cell Pixel_To_Cell(Point2D const & pixel);

		static Point2D Rectangular_To_Isometric(Point2D point);
		Point2D Coord_To_Pixel_Absolute(Coord const & coord);
		Point2D Coord_To_Pixel_Absolute(Point2D const & point);
		Point2D Pixel_To_Lepton(Point2D const & pixel);

		static void Rectangular_To_Isometric(int xin, int yin, int & xout, int & yout);
		static void Isometric_To_Rectangular(int xin, int yin, int & xout, int & yout);
		void Cell_To_Rectangular(int xin, int yin, int & xout, int & yout);
		void Cell_To_Rectangular_Centered(int xin, int yin, int & xout, int & yout);
		void Lepton_To_Map_Pixel(int xin, int yin, int & xout, int & yout);

		Cell Coord_To_Cell(Coord const & coord);

		void Set_View_Dimensions(Rect const & dimensions);

		void Set_Tactical_Position(Point2D const & point);
		void Set_Tactical_Position(Coord const & coord);
		Point2D Get_Tactical_Position(void);
		Point2D Get_Relative_Tactical_Position(void);

		bool Clamp_To_Tactical_Rect(Point2D & pixel);
		Point2D Clamp_Pixel_To_Tactical(Point2D const & pixel);

		void Scroll_Map(FacingType facing, int distance);
		FacingType Scroll_Dir(FacingType dir);
		void Setup_Trigger_Scroll(Coord const & coord, int speed);

		void Register_Dirty_Area(Rect area, bool refresh_shroud = false);
		void Reset_Dirty_Rectangles(void);
		void Register_Dirty_Buildings(Rect area);
		void Flag_Cell(CellClass & cell);

		void Select_These(Rect const & rect, void (*select_callback)(ObjectClass * object) = NULL);
		ObjectClass * Get_Selectable_Object(Point2D const & point);

		bool Contains_Selectable_Buildings(Rect rect);
		void Add_Buildings_To_Selectable(Rect rect);

		bool Add_To_Selectables(ObjectClass * object, Point2D point);

		void Start_Rubber_Band(Point2D const & point);
		void Modify_Rubber_Band(Point2D const & point);
		void Select_Rubber_Band(void (*select_callback)(ObjectClass * object));
		int Rubber_Band_Contains_Selectable(void);
		void End_Rubber_Band(void);
		void Draw_Rubber_Band(void);

		void Wipe_Depth(bool fullredraw, int xoff, int yoff, Rect const & cliprect);
		void Render_Tiles(Rect const & xpanrect, Rect const & ypanrect, bool fullredraw);
		void Render_Tile_Shadows(Rect const & xpanrect, Rect const & ypanrect, bool fullredraw);
		void Render_Overlays(Rect const & xpanrect, Rect const & ypanrect, bool fullredraw);
		void Render_Fogged_Objects(Rect const & xpanrect, Rect const & ypanrect, bool fullredraw);
		void Render_Shroud(Rect const & xpanrect, Rect const & ypanrect, Rect const & cliprect, bool fullredraw);
		void Render_Buildings(Rect const & xpanrect, Rect const & ypanrect, bool fullredraw);
		void Render_Terrain(Rect const & xpanrect, Rect const & ypanrect, bool fullredraw);
		void Render(Surface & surface, bool fullredraw, int drawpass);
		bool Blit_Tiles(void);

		virtual bool Draw_3D_Line(Coord const & coord1, Coord const & coord2, int color, bool write_depth);
		void Draw_Overlays(Rect const & area);
		void Draw_Shroud(Rect const & area);
		void Draw_Tiles(Rect const & area, Rect const & cliprect);
		void Draw_Tile_Shadows(Rect const & area, Rect const & cliprect);
		void Draw_Sparkles(Rect const & area, Rect const & cliprect);
		void Draw_Tiles(Cell const & cell, Rect const & cliprect);
		void Draw_Tile_Shadows(int, Rect const & area);

		static void Draw_Render_Hooks(ObjectClass * object, Rect const & cliprect);
		void Draw_Objects(bool forced);
		void Draw_Terrain(bool forced, Rect const & area, Rect const & cliprect);
		void Draw_Fogged_Objects(Rect const & area);
		void Draw_Buildings(bool forced, Rect const & area, Rect const & cliprect);

		void Draw_Placement(bool drawtrans);
		void Draw_Fence_Placement(bool drawtrans, Cell cell);
		void Draw_Firestorm_Wall_Placement(bool drawtrans, Cell cell);
		void Draw_Wall_Placement(bool drawtrans, Cell cell);

		int Get_Cell_Height(Coord const & coord);
		int Get_Cell_Ramp(Coord const & coord);
		CellClass *Find_Bridge_Owner_Cell(Cell const & cell);
		void Add_Shadow_Control(ShadowControlClass control);

		int Cell_Shadow(Cell const & cell, bool fog);
		bool Is_Cell_Visible (Cell const & cell);
		void Update_Visible_Cells(void);

		void Set_Caption_Text(wchar_t const * text, int frame);
		void Clear_Caption_Text(void);
		void Draw_Screen_Text(wchar_t const * text);

		void Draw_Rally_Points(bool inshroud);
		void Draw_Waypoints(bool inshroud);
		void Draw_Radial_Indicators(void);
		static int DrawTimer(int index, ColorScheme * scheme, int time, wchar_t const * text, unsigned __int64 * flashtime, bool * flashstate);

		void Debug_Draw_Occupier_Links(void);
		void Debug_Draw_Occupation_Flags(void);

#ifdef _DEBUG
		void Debug_Draw_Waypoint_Markers(void);
		void Debug_Draw_Current_Mouse_Cell(void);
#endif

		Cell Clamp_Cursor_To_Tactical(Cell const & cell, Cell const * list);

		void Noop(int);

		int Selectable_Count(void);
		static ObjectClass * __stdcall Selectable_Object(int index);

	public:
		enum {
			MAX_SELECTABLE_OBJECTS = 500
		};

		wchar_t ScreenText[64];

		int EndGameGraphicsFrame;

		int LastAIFrame;

		struct FlagPair {
			FlagPair(void) {}
			FlagPair(bool a, bool b) : field_58(a), field_59(b) {}

			bool field_58;
			bool field_59;
		} Flags;
		char gap_AE[2];

		int TacPixelX;
		int TacPixelY;
		int LastTacPixelX;
		int LastTacPixelY;

		double ZoomFactor;

		struct Selectable {

			ObjectClass * Object;

			Point2D Position;
		};

		static Selectable SelectableObjects[MAX_SELECTABLE_OBJECTS];

		Point2D MoveFrom;
		Point2D MoveTo;
		float MoveSpeed;
		float MoveFactor;

		int CellRedrawCount;
		CellClass * CellRedraw[800];

		Point2D TacticalCoord;

		Point2D LastTacticalCoord;

		Point2D DesiredTacticalCoord;

		bool IsFirstRender;

		bool IsToRedraw;

		bool UnusedBool;

		Rect VisibleCellRect;

		Point2D RubberBandStart;
		Point2D RubberBandEnd;

		int WaypointAnimCounter;
		CDTimerClass<SystemTimerClass> WaypointAnimTimer;

		int SelectableCount;

		Matrix3D CoordToPixelMatrix;

		Matrix3D PixelToCoordMatrix;

		int field_E14;

		static DynamicVectorClass<DirtyAreaStruct> DirtyAreas;

		static DynamicVectorClass<ShadowControlClass *> ShadowControls;
};

#ifdef _DEBUG
extern bool DrawOccupierLinks;
extern bool OccupationBitPrint;
extern bool PassabilityPrint;
#endif
