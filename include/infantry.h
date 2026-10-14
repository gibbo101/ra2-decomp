// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_infantr.h"
#include "foot.h"

#include "do.hh"
#include "fear.hh"

class InfantryClass : public FootClass
{
		typedef FootClass BASECLASS;

	public:
		InfantryTypeClass * Class;

		DoType Doing;

		CDTimerClass<FrameTimerClass> Comment;

		int Fear;

		bool IsBerzerk;

		bool IsTechnician;

		bool IsStoked;

		bool IsProne;

		bool IsZoneCheat;

		bool WasSelected;

		unsigned char : 8;
		unsigned char : 8;
		int unknown_6E0;
		bool ShouldDeploy;
		unsigned char : 8;
		unsigned char : 8;
		unsigned char : 8;
		int unknown_int_6E8;
		int unused_6EC;

		InfantryClass(InfantryTypeClass const * type = NULL, HouseClass * house = NULL);
		InfantryClass(NoInitClass const & x) : BASECLASS(x), Comment(x) {};
		virtual ~InfantryClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual void Init(void) override;

		virtual void Assign_Destination(AbstractClass *, bool=true) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual bool Is_Ready_To_Random_Animate(void) const override;
		virtual int What_Weapon_Should_I_Use(AbstractClass * target) const override;
		virtual bool Ready_To_Next_Mission(void) const override;
		virtual void vt_entry_54C(void) override;
		virtual void const * Get_Image_Data(void) const override;
		int Shape_Number(void) const;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual wchar_t const * Full_Name(void) const override;

		virtual bool Unlimbo(Coord const & coord, Dir256 facing) override;
		virtual bool Paradrop(Coord const & coord) override;
		virtual bool Limbo(void) override;
		virtual void Detach(AbstractClass const * target, bool all) override;

		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;

		virtual bool Active_Click_With(ActionType action, ObjectClass * object, bool) override;
		virtual bool Active_Click_With(ActionType action, Cell const & cell, Cell const & cell2, bool) override;

		virtual ActionType What_Action(ObjectClass const *, bool disallow_force = false) const override;
		virtual ActionType What_Action(Cell const &, bool check_fog = false, bool disallow_force = false) const override;
		virtual BulletClass * Fire_At(AbstractClass * target, int which) override;
		virtual ResultType Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source=0, bool forced=false, bool=false, HouseClass * = 0) override;
		virtual FireErrorType Can_Fire(AbstractClass * target, int which, bool ignoreRange) const override;
		virtual void Assign_Target(AbstractClass *) override;
		virtual void Set_Occupy_Bit(Coord const & coord) override;
		virtual void Clear_Occupy_Bit(Coord const & coord) override;
		virtual bool Is_Renovator(void) const override;
		virtual int IronCurtain(int duration, HouseClass * source, bool force_shield) override;
		virtual bool CanUseWaypoint(void) const override;
		virtual bool CanAttackOnTheMove(void) const override;
		virtual bool CanCheer(void) const override;
		virtual void Cheer(bool force) override;
		virtual bool INoticeSink_Unknown(DWORD unknown, DWORD unknown2) override;
		virtual bool IsDisguisedAs(HouseClass * house) const override;
		virtual ObjectTypeClass * GetDisguise(bool against_allies) const override;
		virtual HouseClass * GetDisguiseHouse(bool against_allies) const override;
		virtual unsigned DisguiseAs(AbstractClass * target) override;
		virtual void ClearDisguise(void) override;

		virtual void On_Movement_Blocked(void) override;
		virtual bool JumpJet_To_Walk(void) override;

		virtual bool Stop_Driver(void) override;
		virtual bool Start_Driver(Coord & coord) override;

		virtual void AI(void) override;
		void Fear_AI(void);
		virtual AbstractClass * Greatest_Threat(ThreatType threat, Coord const & coord, bool) const override;
		bool Ready_To_Commence(void);
		virtual int Do_MISSION_ATTACK(void) override;
		virtual int Do_MISSION_GUARD(void) override;
		virtual int Do_MISSION_HUNT(void) override;
		virtual int Do_MISSION_GUARD_AREA(void) override;
		virtual int Do_MISSION_MOVE(void) override;
		virtual int Do_MISSION_UNLOAD(void) override;
		virtual void Rocking_AI(void) override;
		int FUN_00521320(void);
		void FUN_00522910(BuildingClass * building);
		bool FUN_00522CB0(void);
		void FUN_00522A50(int);
		bool FUN_00522C30(DoType) const;
		bool FUN_00522C60(DoType) const;
		void FUN_00522D00(void);
		virtual void Berzerk(void) override;
		virtual void Start_Fear(void) override;
		virtual void Stop_Fear(void) override;
		virtual void Do_Idle(int which) override;
		bool Edge_Of_World_AI(void);
		void Firing_AI(void);
		void Doing_AI(void);
		bool FUN_005224F0(void) const;
		bool FUN_00522510(void) const;
		bool FUN_005224E0(void) const;
		bool FUN_00522540(void) const;
		int FUN_00522550(void);
		void Movement_AI(void);
		void Tunnel_AI(void);
		bool Theft_AI(void);

#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const override;
#endif

		static void Read_INI(CCINIClass & ini);
		static void Write_INI(CCINIClass & ini);

		virtual bool IsDeployed(void) const;
		virtual bool Do_Action(DoType todo, bool force=false, bool randomize=false);
		virtual bool Random_Animate(void) override;
		virtual MoveType Can_Enter_Cell(CellClass const * cell, FacingType dir = FACING_NONE, int cell_height = -1, CellClass const * = 0, bool = true) const override;
		virtual void Per_Cell_Process(PCPType why) override;
		virtual bool Enter_Idle_Mode(bool initial=false, bool=true) override;
		virtual void Scatter(Coord const & threat, bool forced=false, bool nokidding=false) override;
		virtual Coord Turret_Coord(int which=0) const override;
		virtual int Current_Speed(void) override;
		virtual AbstractClass * Approach_Target(bool = false) override;
		virtual void Stop_Movement_Animation(void) override;
		bool Is_JumpJet(void) const;
		bool Should_JumpJet_Fly(Cell const & from, Cell const & to);

		static int const HumanShape[32];

	private:

		static DoStruct const MasterDoControls[DO_COUNT];

		static char const * const INI_Name;
};

inline InfantryClass * AbstractClass::As_InfantryClass(void)
{
	return(dynamic_cast<InfantryClass *>(this));
}

inline InfantryClass const * AbstractClass::As_InfantryClass(void) const
{
	return(dynamic_cast<InfantryClass const *>(this));
}
