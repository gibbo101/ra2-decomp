// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "display.h"
#include "hashtable.h"
#include "stimer.h"
#include "timer.h"

#include "bsize.hh"

class DSurface;
class BSurface;
template<class T> class DynamicVectorClass;

typedef DynamicVectorClass<Point2D> FOUNDATION_LIST;

struct RadarTrackingStruct {

	RadarTrackingStruct(TechnoClass * object = NULL, int x = 0, int y = 0) : Object(object), Position(x, y) {}
	RadarTrackingStruct(RadarTrackingStruct const & that) : Object(that.Object), Position(that.Position) {}

	TechnoClass * Object;

	Point2D Position;

	bool operator==(const RadarTrackingStruct & that) const { return(Object == that.Object && Position == that.Position); }
	bool operator!=(const RadarTrackingStruct & that) const { return(Object != that.Object || Position != that.Position); }

	bool operator==(RadarTrackingStruct & that) { return(Position == that.Position); }
	bool operator!=(RadarTrackingStruct & that) { return(Position != that.Position); }

	int Hash(void) const { return((Position.X - 5 * Position.Y) & 0xFF); }

	bool Use_Head(void) const;

	static int Hash_Old(RadarTrackingStruct const & s);
	static int Hash2(RadarTrackingStruct const & s);
};

typedef HashTableClass<RadarTrackingStruct, TechnoClass *> RADAR_HASH_TABLE;

class RadarClass: public DisplayClass
{
		typedef DisplayClass BASECLASS;
		friend class RadarEventClass;

	public:
		virtual HRESULT Load(IStream * stream) override;
		virtual HRESULT Save(IStream * stream) override;

	public:
		RadarClass(void);
		RadarClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~RadarClass(void) override;

		int RadX;
		int RadY;
		int RadWidth;
		int RadHeight;
		int RadOffX;
		int RadOffY;
		int RadIWidth;
		int RadIHeight;
		int RadPWidth;
		int RadPHeight;

		virtual void One_Time(void) override;
		virtual void Init_Clear(void) override;

		void Post_Load_Radar_Fixup(void);

		virtual bool Map_Cell(Cell const & cell, HouseClass * house) override;
		virtual Cell Click_Cell_Calc(Point2D const & point) const;
		virtual void AI(KeyNumType & input, Point2D const & xy) override;
		virtual void Draw_It(bool complete=false) override;
		virtual void Reposition_Sidebar(void) override;
		virtual void Set_Map_Dimensions(Rect const & size, bool, int cell_height, bool) override;
		virtual void Set_Local_Dimensions(Rect const & size) override;
		virtual void Set_Tactical_Position(Coord const & coord);
		virtual void Init_For_House(void);

		void Radar_Activate(int control);
		void Radar_Background(Cell const & cell);
		Point2D Coord_To_Radar_Pixel(Coord const & coord, bool clip);
		bool Cell_On_Radar(Cell const & cell);
		bool Is_Radar_Active(void);
		bool Is_Radar_Tactical(void);
		bool Is_Radar_Existing(void);

		void Flag_Background_Update(Cell const & cell) {Radar_Background(cell); }

		bool Is_Player_Names(void);
		void Draw_Names(void);
		void Noop(int, int);
		void Compute_Radar_Image(void);
		Rect Compute_Background(Rect const & cell_rect, Rect & update_rect, bool fill_in);
		void Fill_In_Background(Rect const & cell_rect, Rect const & update_rect);
		Rect Cell_Radar_Rect(Cell const & cell);
		Rect Cell_To_Radar_Pixel(Cell const & cell);
		Cell Radar_Pixel_To_Cell(Point2D const & point);
		void Plot_Radar_Background(void);
		void Radar_Track(TechnoClass * techno, Point2D point);
		void Radar_Untrack(TechnoClass * techno, Point2D point);
		void Init_Radar(void);
		void Reset_Radar(void);
		void Clear_Radar(void);
		void Plot_Radar_Pixel(Point2D const & point);
		void Render_Tracked_Objects(void);
		void Radar_Pixel(Point2D const & point);

		void Compute_Foundations(void);
		FOUNDATION_LIST const & Get_Foundation(BuildingTypeClass * btype);

		void Radar_Cell(Cell const & cell);
		void Resolve_Radar_Point(Point2D const & point, Cell & cell, ObjectClass *& object);
		void Set_Radar_State(bool on);

		void Toggle_Radar(bool on);
		bool Is_Playing_Movie(void);
		void Redraw_Radar(bool complete);

		void Render_Radar(void);
		void Play_Movie(void);
		void Complete_Radar_Refresh(void);

		void Queue_Next_Movie(void) {RadarState = RSTATE_NEXT_MOVIE; RadarAnimFrame = RADAR_ACTIVATED_FRAME;}

		void Clear_Background_Stack(void) { BackgroundStack.Clear(); }

		void Set_Radar_Scale(int scale) { RadarScale = scale; }

	public:

		enum RadarClassEnums {
			RADAR_ACTIVATED_FRAME = 25,
			MAX_RADAR_FRAMES = 40
		};

		Rect LastDrawRect;

		DSurface * RadarSurface;

	protected:

		BSurface * BackgroundSurface;

		DynamicVectorClass<Cell> BackgroundStack;

		RGBClass * BackgroundColors;

		class RTacticalClass : public GadgetClass {
			public:
				RTacticalClass(void) : GadgetClass(0,0,0,0,LEFTPRESS|LEFTRELEASE|LEFTHELD|LEFTUP|RIGHTPRESS|RIGHTUP,true) {};

			protected:
				virtual int Action(unsigned flags, KeyNumType & key, int modifier) override;
			friend class RadarClass;
		};
		friend class RTacticalClass;

		static RTacticalClass RadarButton;

	private:

		int RadarCellWidth;
		int RadarCellHeight;
		Rect CellRedrawRect;

		RADAR_HASH_TABLE * RadarTrackingTable;

		DynamicVectorClass<Point2D> PixelStack;
		unsigned char * PixelFlags;

		FOUNDATION_LIST Foundation[BSIZE_COUNT];

		float ZoomFactor;

		int RadarScale;
		int RadarX;

		int field_149C;

		int RadarY;

		Rect RadarRect;

		enum RadarStateType {
			RSTATE_INACTIVE,
			RSTATE_ACTIVE,
			RSTATE_DEACTIVATING,
			RSTATE_ACTIVATING,
			RSTATE_NEXT_MOVIE,
			RSTATE_MOVIE_DONE,
		};

		int RadarState;

		enum RadarModeType {
			RMODE_UNAVAILABLE,
			RMODE_TACTICAL,
			RMODE_PLAYER_NAMES,
			RMODE_MOVIE,
			RMODE_4,
			RMODE_5,
		};

		int RadarMode;

		int SuspendedRadarMode;

		bool DoesRadarExist;

		bool IsToRedraw;
		bool FullRedraw;

		Rect RadarViewRect;

		Rect OldRadarViewRect;

		int RadarAnimFrame;

		CDTimerClass<SystemTimerClass> RadarAnimTimer;

		static void const * RadarAnim;
};
