// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "audioctl.h"
#include "assert.h"
#include "coord.h"
#include "face.h"
#include "globals.h"
#include "point.h"
#include "rect.h"
#include "tag.h"

#include "action.hh"
#include "layer.hh"
#include "mark.hh"
#include "mission.hh"
#include "move.hh"
#include "path.hh"
#include "pcp.hh"
#include "radio.hh"
#include "result.hh"
#include "tevent.hh"
#include "visual.hh"

class ObjectClass;
class TechnoClass;
class TechnoTypeClass;
class ObjectTypeClass;
class HouseClass;
class BuildingClass;
class RadioClass;
class TagClass;
class WarheadTypeClass;
class ShapeSet;
class MonoClass;
class AnimClass;
class BombClass;
class LineTrail;

class ObjectClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		enum {
			PARACHUTE_MAX_FALL_RATE = -3,
			NO_PARACHUTE_MAX_FALL_RATE = -100,
		};

		Coord Riser;

		ObjectClass * Next;

		TagClass * Tag;

		BombClass * AttachedBomb;

		AudioController AmbientSoundController;

		AudioController CustomSoundController;

		int CustomSound;

		bool BombVisible;

		int Strength;

		int EstimatedHealth;

		bool IsDown;

		BOOL IsToDamage;

		DWORD unknown_7C;

		mutable bool IsToDisplay;

		bool IsInLimbo;

		bool InOpenToppedTransport;

		bool IsSelected;

		bool IsAnimAttached;

		AnimClass * Parachute;

		bool IsOnBridge;

		bool IsFalling;

		bool IsABomb;

		bool IsToExplode;

		bool IsActive;

		LayerType Layer;

		bool IsSubmittedToLayer;

		bool IsVisible;

		Coord Position;

		LineTrail * LineTrailer;

		ObjectClass(void);
		ObjectClass(NoInitClass const & x);
		virtual ~ObjectClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;

		bool operator < (ObjectClass const & object) const;
		bool operator > (ObjectClass const & object) const;

		virtual void Compute_CRC(CRCEngine &) const override;
		virtual void AnimPointerExpired(AnimClass * anim);
		virtual bool Is_Players_Army(void) const {return(false);}
		virtual VisualType Visual_Character(bool raw = false, HouseClass const * = 0) const {return(VISUAL_NORMAL);}
		virtual void const * Get_Image_Data(void) const;
		virtual ActionType What_Action(ObjectClass const *, bool disallow_force = false) const;
		virtual ActionType What_Action(Cell const &, bool check_fog = false, bool disallow_force = false) const;
		virtual LayerType In_Which_Layer(void) const;
		bool Not_Underground(void) const;
		virtual bool Considered_Vehicle(void) const {return(false);}
		virtual bool IsStrange(void) const;
		virtual TechnoTypeClass const * Techno_Type_Class(void) const;
		__declspec( property( get=Techno_Type_Class) ) TechnoTypeClass const * TClass;
		virtual ObjectTypeClass const * Class_Of(void) const {return(0);}
		bool Is_Type_Named(char const * name) const;
		void Clear_Line_Trail(void);
		bool Is_Infantry(void) const {return(Fetch_RTTI() == RTTI_INFANTRY);}
		bool Is_Foot(void) const { if (this == NULL) return(false); unsigned char flag = ((unsigned char)AbstractFlags >> 2) & 1; return(flag != 0); }
		virtual int Get_Ownable(void) const;
		virtual wchar_t const * Full_Name(void) const {return(L"No name");}
		virtual bool Can_Repair(void) const;
		virtual bool Can_Demolish(void) const;
		virtual bool Can_Player_Fire(void) const;
		virtual bool Can_Player_Move(void) const;

		virtual Coord Target_Coord(void) const {return(Center_Coord());}
		virtual Coord Docking_Coord(TechnoClass * docker) const;
		virtual Coord Center_Coord(void) const override;
		virtual Coord Render_Coord(void) const {return(Center_Coord());}
		virtual Coord Fire_Coord(int which, Coord base) const {return(Center_Coord() + Coord(0, 0, 50));}
		virtual Coord Exit_Coord(int index) const {return(Center_Coord());}
		virtual int Sort_Y(void) const;
		virtual bool On_Ground(void) const override;
		virtual bool In_Air(void) const override;
		virtual bool Is_Moving_Onto_Bridge(TechnoClass * docker = NULL) const;
		virtual bool Occupies_Cells(void) const {return(true);}
		virtual bool IsDisguised(void) const;
		virtual bool IsDisguisedAs(HouseClass * house) const;
		virtual ObjectTypeClass * GetDisguise(bool against_allies) const;
		virtual HouseClass * GetDisguiseHouse(bool against_allies) const;

		int Distance(AbstractClass const * target) const;
		bool IsCrushable(TechnoClass * crusher) const;
		int Distance(Coord const & coord) const {return(Center_Coord().Distance_To(coord));}
		int Distance_To(AbstractClass const * obj) const {return(Distance(obj->Center_Coord()));}
		int Planar_Distance(AbstractClass const * target) const;
		int Relative_Distance(AbstractClass const * target) const;
		int Relative_Distance(Coord const & coord) const;

		DirType Direction(AbstractClass const * object) const;

		virtual bool Limbo(void);
		virtual bool Unlimbo(Coord const & , Dir256 facing = DIR_N);
		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual void Detach_All(bool all=true);
		virtual void RegisterDestruction(TechnoClass * destroyer);
		virtual void Record_The_Kill(HouseClass * );
		virtual bool Paradrop(Coord const & coord);
		bool Attach_Tag(TagClass * tag);
		virtual bool Is_Inactive(void) const override;

		virtual void Fall_From_Height(void);
		virtual void Set_Occupy_Bit(Coord const & coord);
		virtual void Clear_Occupy_Bit(Coord const & coord);
		virtual void Delete_Me(void);
		virtual void Do_Shimmer(void);
		virtual int Exit_Object(TechnoClass * techno, Cell cell);
		virtual bool Render(Rect &, bool forced, bool extras_only) const;
		virtual Cell const * Occupy_List(bool placement=false) const;

		double Get_Health_Ratio(void) const;
		void Set_Health_Ratio(double health);

		__declspec( property( get=Get_Health_Ratio, put=Set_Health_Ratio ) ) double HealthRatio;

		virtual void Draw_Pre_Render(Point2D const & point, Rect const & cliprect) const { }
		virtual void Draw_Post_Render(Point2D const & point, Rect const & cliprect) const { }
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const { }
		virtual void Editor_Draw_It(Point2D const & point, Rect const & cliprect) const;
		virtual void Hidden(void);
		virtual void Look(bool incremental=false, bool=false);
		virtual bool Mark(MarkType=MARK_CHANGE);

		virtual Rect Get_Visual_Rect(void) const;
		virtual Rect Get_Render_Rect(void);
		virtual void Draw_Radial_Indicator(void) const {};

	private:
		virtual void Mark_For_Redraw(void);

	public:

		virtual bool CanBeSelected(void) const;
		virtual bool CanBeSelectedNow(void) const;

		virtual bool Active_Click_With(ActionType , ObjectClass *, bool);
		virtual bool Active_Click_With(ActionType , Cell const &, Cell const &, bool);
		virtual void Clicked_As_Target(int = 7);
		virtual bool Select(void);
		virtual void Unselect(void);
		virtual int IronCurtain(int duration, HouseClass * source, bool force_shield);
		virtual void StopAirstrikeTimer(void);
		virtual void StartAirstrikeTimer(int duration);
		virtual bool IsIronCurtained(void) const;

		virtual bool In_Range(Coord const & coord, int which=0) const;
		virtual int Weapon_Range(int =0) const;
		virtual ResultType Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source=0, bool forced=false, bool=false, HouseClass * = 0);
		virtual void Destroy(void);
		virtual void Scatter(Coord const &, bool forced=false, bool nokidding=false);
		virtual bool Catch_Fire(void);
		virtual void Fire_Out(void);
		virtual int Value(void) const;
		virtual MissionType Get_Mission(void) const;
		virtual void Assign_Mission(MissionType mission) {}

		__declspec( property( get=Get_Mission, put=Assign_Mission ) ) MissionType Mission;

		virtual void Per_Cell_Process(PCPType) {}
		virtual BuildingClass * Who_Can_Build_Me(bool intheory, bool legal) const;
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, int & param);
		virtual bool Revealed(HouseClass * house);
		virtual void Repair(int);
		virtual void Sell_Back(int);
		virtual void Set_Waypoint_Path(PathType path, char index);
		virtual void AI(void) override;

		virtual void Move(FacingType);
		virtual MoveType Can_Enter_Cell(CellClass const * cell, FacingType dir = FACING_NONE, int cell_height = -1, CellClass const * = 0, bool = true) const {return(MOVE_OK);}
		virtual MoveType Can_Reach(CellClass const * current_cell, FacingType facing, int & cell_height, bool & onto_bridge, CellClass const * adjacent_cell) const {return(MOVE_OK);}

		Coord Get_Coord(void) const {return(Position);}
		virtual void Set_Coord(Coord const & coord);

		__declspec( property( get=Get_Coord, put=Set_Coord ) ) Coord PositionCoord;

		virtual Cell Get_Cell(void) const {return(Position.As_Cell());}

		__declspec( property( get=Get_Cell  ) ) Cell PositionCell;

		virtual CellClass * Get_Cell_Ptr(void) const;
		virtual Cell Get_Target_Cell(void) const;
		virtual CellClass * Get_Target_Cell_Ptr(void) const;

		int Get_Cell_Height(void) const;

		virtual int Get_Height_AGL(void) const;
		virtual void Set_Height_AGL(int);

		__declspec( property( get=Get_Height_AGL, put=Set_Height_AGL ) ) int HeightAGL;

		virtual int Get_Height(void) const;
		virtual bool IsBeingWarpedOut(void) const;
		virtual bool IsWarpingIn(void) const;
		virtual bool IsWarpingSomethingOut(void) const;
		virtual bool IsNotWarping(void) const;
		virtual void * GetRemapColour(void) const;
		void Set_Height(int height);

		__declspec( property( get=Get_Height, put=Set_Height ) ) int Height;

		void Spring_Tag(TEventType event=TEVENT_ANY, ObjectClass * object=NULL, Cell const & cell=CELL_NONE, bool forced=false, TechnoClass *source=NULL)
		{
			if (IsActive && Tag != NULL) {
				Tag->Spring(event, object, cell, forced, source);
			}
		}

#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const override;
#endif
};

inline ObjectClass * AbstractClass::As_ObjectClass(void)
{
	return(dynamic_cast<ObjectClass *>(this));
}

inline ObjectClass const * AbstractClass::As_ObjectClass(void) const
{
	return(dynamic_cast<ObjectClass const *>(this));
}

inline ObjectClass * As_Object(AbstractClass * target)
{
	return(dynamic_cast<ObjectClass *>(target));
}

Coord Vector_Center(DynamicVectorClass<ObjectClass *> const & list);
ObjectClass * Vector_Closest_Object(DynamicVectorClass<ObjectClass *> const & list, Coord const & coord);
