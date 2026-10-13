// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "face.h"
#include "gadget.h"
#include "layer.h"
#include "map.h"
#include "rgb.h"
#include "types.h"

#include "action.hh"
#include "layer.hh"
#include "source.hh"
#include "super.hh"

class ObjectClass;
class ObjectTypeClass;
class CCINIClass;
class TechnoClass;
class WaypointClass;

#define	SIDE_BAR_TAC_WIDTH	10
#define  SIDE_BAR_TAC_HEIGHT	8

class DisplayClass: public MapClass
{
		typedef MapClass BASECLASS;

		friend class TabClass;
		friend class CellClass;
		friend class Tactical;

	public:
		virtual HRESULT Load(IStream * stream);
		virtual HRESULT Save(IStream * stream);

	public:

		static LayerClass Layer[LAYER_COUNT];

		Cell ZoneCell;
		Cell ZoneOffset;
		Cell const *CursorSize;
		bool ProximityCheck;
		bool ShroudCheck;

		Cell CurrentFoundationCopy_CenterCell;
		Cell CurrentFoundationCopy_TopLeftOffset;
		Cell const * CurrentFoundationCopy_Data;
		ObjectClass * CurrentBuildingCopy;
		ObjectTypeClass const * CurrentBuildingTypeCopy;
		HousesType CurrentBuildingOwnerArrayIndexCopy;

		bool FollowingObject;
		ObjectClass * FollowingObjectPtr;

		ObjectClass * PendingObjectPtr;
		ObjectTypeClass const * PendingObject;
		HousesType PendingHouse;

		DisplayClass(void);
		DisplayClass(NoInitClass const & x) : BASECLASS(x) {};

		virtual void Read_INI(CCINIClass const & ini);
		void Write_INI(CCINIClass & ini);

		virtual void One_Time(void) override;
		virtual void Init_Clear(void) override;
		virtual void Init_IO(void) override;

		int Stash_Map_State(void * stash, int);
		void Restore_Map_State(void * stash);

		virtual void AI(KeyNumType &input, Point2D const & xy) override;

		void All_To_Look(bool units_only=false, bool=false);
		void FUN_004ADEE0(bool units_only, bool is_fog);
		void FUN_004ADCD0(bool units_only, bool is_fog);
		void FUN_004A8D50(Cell const * list);
		bool ProcessClickCoords(Point2D * src, Cell * cell, Coord * coord, ObjectClass ** target, unsigned char * a5, unsigned char * a6);
		void Constrained_Look(Coord const & coord, LEPTON distance);
		void Shroud_Cell(Cell const & cell);
		void Encroach_Shadow(void);
		void Fog_Cell(Cell const & cell);
		void Encroach_Fog(void);
		void Center_Map(void);
		virtual char const * Help_Text(int id);
		virtual void Reposition_Sidebar(void);
		virtual void Abort_Drag_Select(void);
		virtual bool Map_Cell(Cell const & cell, HouseClass * house);
		virtual bool RevealFogShroud(Cell const & cell, HouseClass * house, bool increase);
		virtual bool Fog_Map_Cell(Cell const & cell, HouseClass * house);
		virtual bool Shadow_Map_Cell(Cell const & cell, HouseClass * house);
		virtual MouseType Get_Mouse_Shape(void) const = 0;
		virtual bool Scroll_Map(FacingType facing, int & distance, bool really);
		virtual void Set_View_Dimensions(Rect const & dimensions);

		virtual void Put_Place_Back(TechnoClass * ) {};
		void Cursor_Mark(Cell const & pos, bool on);
		void Set_Cursor_Shape(Cell const * list);
		void SetActiveFoundation(Cell const * list);
		Cell Set_Cursor_Pos(Cell const & pos = CELL_NONE);
		Cell Get_Occupy_Dimensions(Cell const *list) const;

		void Remove(ObjectClass const * object);
		void Submit(ObjectClass const * object);
		Cell Calculated_Cell(SourceType dir, Cell const & waypoint=CELL_NONE, Cell const & cell=CELL_NONE, SpeedType loco=SPEED_FOOT, bool zonecheck=true, MZoneType mzone=MZONE_NORMAL) const;
		bool Passes_Proximity_Check(ObjectTypeClass const * object, HousesType house, Cell const * list, Cell const & trycell) const;
		bool Passes_Shroud_Check(ObjectTypeClass const * object, HousesType house, Cell const * list, Cell const & trycell) const;
		ObjectClass * Cell_Object(Cell const & cell, Point2D const & point) const;
		ObjectClass * Next_Object(ObjectClass * object) const;
		ObjectClass * Prev_Object(ObjectClass * object) const;
		bool Is_Spot_Free(Coord const & coord, bool bridge=false) const;
		Coord Closest_Free_Spot(Coord const & coord, bool any=false) const;
		void Sell_Mode_Control(int control);
		void Waypoint_Mode_Control(int control, bool edit_selected_path=false);
		void Power_Mode_Control(int control);
		void Repair_Mode_Control(int control);
		ObjectClass * Object_To_Follow(void) const;
		void Set_To_Follow(ObjectClass * object);
		void Break_Follow_Mode(void) { Set_To_Follow(NULL); }
		void Reinit_Cell_Drawers(void);
		void Update_Cell_Colors(void);

		LRESULT Windows_Message_Proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
		void Active_Click(ObjectClass * object, Cell cell, ActionType action);
		ActionType Action_To_Waypoint_Action(ActionType action, Cell const & cell);
		void Update_Waypoint_Color(int index);

		void Compute_Start_Pos(void);

	protected:
		virtual void Mouse_Right_Press(Point2D const & point = Point2D());
		virtual void Mouse_Left_Press(Point2D const & point);
		virtual void Mouse_Left_Up(Cell const & cell, bool shadow, ObjectClass * object, ActionType action, bool wsmall = false);
		virtual void Mouse_Left_Held(Point2D const & point);
		virtual void Mouse_Left_Release(Coord const & coord, Cell const & cell, ObjectClass * object, ActionType action, bool wsmall = false);
		virtual void Mouse_Right_Release(Point2D const & point = Point2D());

	public:

		bool IsRepairMode;

		bool IsSellMode;

		bool IsPowerMode;

		bool IsWaypointMode;

		bool PlaceBeaconMode;

		SuperWeaponType IsTargettingMode;

		WaypointClass * DraggedWaypoint;

		Coord DraggedWaypointCoord;

		RGBClass WaypointColor;

	protected:

		bool IsRubberBand;

		bool IsTentative;

		class TacticalClass : public GadgetClass {
			public:
				TacticalClass(void) : GadgetClass(0,0,0,0,LEFTPRESS|LEFTRELEASE|LEFTHELD|LEFTUP|RIGHTPRESS|RIGHTRELEASE|RIGHTHELD,true) {};

			protected:
				virtual int Action(unsigned flags, KeyNumType & key, int modifier);
		};
		friend class TacticalClass;

		static TacticalClass TacButton;

	private:

		bool IsShadowPresent;

		struct BandStruct {
			int BandX;
			int BandY;
			BandStruct(void) : BandX(0), BandY(0) {}
		};

		BandStruct Band;
		int NewX,NewY;

		static void const *ShadowShapes;

		static void const *PlacementShapes;


		bool Good_Reinforcement_Cell(Cell const & outcell, Cell const & incell, SpeedType loco, int zone, MZoneType mzone) const;

};
