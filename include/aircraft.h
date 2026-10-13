// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "foot.h"
#include "iflyctrl.h"

class AircraftTypeClass;
class BuildingClass;

class AircraftClass : public FootClass, public IFlyControl
{
		typedef FootClass BASECLASS;

	public:

		AircraftTypeClass * Class;

		AircraftClass(AircraftTypeClass const * type = NULL, HouseClass * house = NULL);
		AircraftClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~AircraftClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
		virtual ULONG STDMETHODCALLTYPE Release(void) override;

		virtual LONG STDMETHODCALLTYPE Landing_Altitude(void) override;
		virtual LONG STDMETHODCALLTYPE Landing_Direction(void) override;
		virtual BOOL STDMETHODCALLTYPE Is_Loaded(void) override;
		virtual LONG STDMETHODCALLTYPE Is_Strafe(void) override;
		virtual LONG STDMETHODCALLTYPE Is_Fighter(void) override;
		virtual LONG STDMETHODCALLTYPE Is_Locked(void) override;

		virtual void Init(void) override;
		virtual bool On_Ground(void) const override;
		virtual bool In_Air(void) const override;
		virtual bool IsStrange(void) const override;
		virtual void QueueMission(MissionType mission, bool start) override;
		virtual void Set_Mission(MissionType mission) override;
		virtual void Override_Mission(MissionType mission, AbstractClass * tarcom, AbstractClass * navcom) override;
		virtual bool Ready_To_Next_Mission(void) const override;
		virtual int Do_MISSION_PARADROP_APPROACH(void) override;
		virtual int Do_MISSION_PARADROP_OVERFLY(void) override;
		virtual int Do_MISSION_SPYPLANE_APPROACH(void) override;
		virtual int Do_MISSION_SPYPLANE_OVERFLY(void) override;
		virtual bool IsNotWarpingIn(void) const override;
		virtual BuildingClass * TryNearestDockBuilding(TypeList<BuildingTypeClass *> * list, unsigned unk2, unsigned unk3) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;

		virtual bool Commence(void) override;

		virtual int Do_MISSION_ATTACK(void) override;
		virtual int Do_MISSION_UNLOAD(void) override;
		virtual int Do_MISSION_HUNT(void) override;
		virtual int Do_MISSION_RETREAT(void) override;
		virtual int Do_MISSION_MOVE(void) override;
		virtual int Do_MISSION_ENTER(void) override;
		virtual int Do_MISSION_GUARD(void) override;
		virtual int Do_MISSION_GUARD_AREA(void) override;
		virtual int Do_MISSION_PATROL(void) override;

		virtual void Assign_Destination(AbstractClass * target, bool = true) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual LayerType In_Which_Layer(void) const override;
		virtual DirType Turret_Facing(void) const override;
		virtual MoveType Can_Enter_Cell(CellClass const * cell, FacingType facing = FACING_NONE, int cell_height = -1, CellClass const * = 0, bool = true) const override;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual wchar_t const * Full_Name(void) const override;
		virtual ActionType What_Action(ObjectClass const *, bool disallow_force = false) const override;
		virtual ActionType What_Action(Cell const &, bool check_fog = false, bool disallow_force = false) const override;
		virtual FacingType Desired_Load_Dir(ObjectClass * passenger, Cell & moveto) const override;
		AbstractClass * Good_Fire_Location(AbstractClass * target) const;
		bool Cell_Seems_Ok(Cell const & cell, bool landing=false) const;
		Dir256 Pose_Dir(void) const;
		AbstractClass * Good_LZ(void) const;
		virtual DirType Fire_Direction(void) const override;
		virtual FireErrorType Can_Fire(AbstractClass * target, int which, bool ignoreRange) const override;
		virtual bool Should_Delete_Off_Map(void) override;

		AbstractClass * New_LZ(AbstractClass * oldlz) const;

		virtual bool Unlimbo(Coord const & , Dir256 facing = DIR_N) override;

		virtual void Look(bool incremental=false, bool=false) override;
		void Draw_Rotors(Point2D const & xy, Rect const & cliprect) const;
		bool FUN_00414A20(MissionType mission) const;
		virtual int Exit_Object(TechnoClass * techno, Cell cell) override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;

		virtual bool Active_Click_With(ActionType action, ObjectClass * object, bool) override;
		virtual bool Active_Click_With(ActionType action, Cell const & cell, Cell const & cell2, bool) override;
		virtual bool Player_Assign_Mission(MissionType mission, AbstractClass * target=NULL, AbstractClass * destination=NULL, AbstractClass * nearby=NULL) override;

		virtual ResultType Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source, bool forced=false, bool=false, HouseClass * = 0) override;
		virtual BulletClass * Fire_At(AbstractClass * target, int which) override;
		virtual void Reduce_Ammunition(void) override;

		int Paradrop_Cargo(void);
		void Drop_Off_Cargo(void);
		virtual void AI(void) override;
		virtual bool Enter_Idle_Mode(bool initial = false, bool = true) override;
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, int & param) override;
		virtual void Scatter(Coord const & threat, bool forced=false, bool nokidding=false) override;

#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const override;
#endif

		static void Read_INI(CCINIClass & ini);
		static void Write_INI(CCINIClass & ini);

	public:

		bool IsToSpendAmmo;
		bool Passenger;
		bool IsKamikaze;
		BuildingClass * DockNowHeadingTo;
		bool unknown_bool_6D0;
		bool unknown_bool_6D1;
		bool IsLockedStraight;
		char NumParadropsLeft;
		bool IsCarryallNotLanding;
		bool IsReturningFromAttackRun;

	private:
		int Do_MISSION_MOVE_Carryall(void);
		int Do_MISSION_MOVE_Normal(void);

		static char const * const INI_NAME;
};

inline AircraftClass * AbstractClass::As_AircraftClass(void)
{
	return(dynamic_cast<AircraftClass *>(this));
}

inline AircraftClass const * AbstractClass::As_AircraftClass(void) const
{
	return(dynamic_cast<AircraftClass const *>(this));
}

bool Counts_As_Civ_Evac(ObjectClass const * candidate);
