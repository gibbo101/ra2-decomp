// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "foot.h"

class EBolt;
class Surface;

class UnitClass : public FootClass
{
		typedef FootClass BASECLASS;

	public:
		int FiringSyncDelay;
		UnitTypeClass * Class;
		UnitClass * FollowingMe;
		HousesType Flagged;
		bool IsFollowing;
		bool IsDumping;
		bool IsHarvesting;
		bool TerrainPalette;
		int unknown_int_6D4;
		int DeathCounter;
		EBolt * ElectricBolt;
		bool Deployed;
		bool Deploying;
		bool Undeploying;
		int NonPassengerCount;
		wchar_t ToolTipText[256];

		UnitClass(UnitTypeClass const * type = NULL, HouseClass * house = NULL);
		UnitClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~UnitClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual void Init(void) override;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual wchar_t const * Full_Name(void) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual bool IsStrange(void) const override;
		virtual bool IsDisguisedAs(HouseClass * house) const override;
		virtual ObjectTypeClass * GetDisguise(bool against_allies) const override;
		virtual HouseClass * GetDisguiseHouse(bool against_allies) const override;
		virtual void RegisterDestruction(TechnoClass * destroyer) override;
		virtual void Destroy(void) override;

		bool Goto_Clear_Spot(void);
		bool Try_To_Deploy(void);
		void Deploy(void);
		void Undeploy(void);
		virtual void Scatter(Coord const &, bool forced=false, bool nokidding=false) override;
		bool Flag_Attach(HousesType house);
		bool Flag_Remove(void);
		bool Harvesting(void);
		void APC_Close_Door(void);
		void APC_Open_Door(void);

		virtual bool Is_Ready_To_Move(void) const override;
		bool Should_Crush_It(TechnoClass const * it) const;
		int Credit_Load(void) const;
		virtual DirType Turret_Facing(void) const override;
		virtual int Pip_Count(void) const override;
		virtual InfantryTypeClass const * Crew_Type(void) const override;
		virtual DirType Fire_Direction(void) const override;
		virtual FireErrorType Can_Fire(AbstractClass * target, int which, bool ignoreRange) const override;
		virtual double Tiberium_Load(void) const override;
		virtual int What_Weapon_Should_I_Use(AbstractClass * target) const override;
		virtual bool IsUnderEMP(void) const override;
		virtual void Reload(void) override;
		virtual unsigned DisguiseAs(AbstractClass * target) override;
		virtual void ClearDisguise(void) override;
		virtual bool CanUseWaypoint(void) const override;
		virtual bool CanAttackOnTheMove(void) const override;

		bool Is_Route_Broken(Cell const & from, Cell const & to) const;
		AbstractClass * Plan_Route(AbstractClass const & object) const;

		virtual bool Limbo(void) override;
		virtual bool Unlimbo(Coord const & , Dir256 facing=DIR_N) override;
		virtual void Set_Occupy_Bit(Coord const & coord) override;
		virtual void Clear_Occupy_Bit(Coord const & coord) override;

		virtual bool Render(Rect &, bool forced, bool extras_only) const override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual void Unit_Draw_Voxel(Point2D xyoff, Rect rect, int intensity, int tint);
		virtual void Unit_Draw_Shape(Point2D xyoff, Rect rect, int intensity, int tint);
		virtual void Unit_Blit_Voxel(Surface * surface, Point2D xyoff, Rect rect, int alpha, int tint);

		virtual ActionType What_Action(Cell const &, bool check_fog = false, bool disallow_force = false) const override;
		virtual ActionType What_Action(ObjectClass const *, bool disallow_force = false) const override;
		virtual bool Active_Click_With(ActionType action, ObjectClass * object, bool) override;
		virtual bool Active_Click_With(ActionType , Cell const &, Cell const &, bool) override;

		virtual ResultType Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source=0, bool forced=false, bool=false, HouseClass * = 0) override;
		virtual BulletClass * Fire_At(AbstractClass * target, int which=0) override;
		virtual bool Captured(HouseClass * newowner) override;
		void EMPulse_Blast(void);
		void Explode(void);
		virtual AbstractClass * Greatest_Threat(ThreatType threat, Coord const & coord, bool) const override;

		virtual FacingType Desired_Load_Dir(ObjectClass * passenger, Cell & moveto) const override;
		virtual RadioMessageType Receive_Message(RadioClass * from, RadioMessageType message, int & param) override;

		virtual void AI(void) override;
		virtual bool Ready_To_Next_Mission(void) const override;
		virtual int Do_MISSION_ATTACK(void) override;
		virtual int Do_MISSION_GUARD_AREA(void) override;
		virtual int Do_MISSION_UNLOAD(void) override;
		virtual int Do_MISSION_GUARD(void) override;
		virtual int Do_MISSION_HARVEST(void) override;
		virtual int Do_MISSION_HUNT(void) override;
		virtual int Do_MISSION_REPAIR(void) override;
		virtual int Do_MISSION_MOVE(void) override;
		virtual int Do_MISSION_PATROL(void) override;
		void Rotation_AI(void);
		void Firing_AI(void);
		bool Edge_Of_World_AI(void);
		void Tunnel_AI(void);
		void Visceroid_AI(void);
		void Jellyfish_AI(void);

		virtual void ReceiveGunner(FootClass * gunner) override;
		virtual void RemoveGunner(FootClass * gunner) override;
		virtual bool Deploy_To_Fire(void) const override;
		virtual void Overrun_Square(Cell const & cell, bool threaten=true) override;
		virtual void Approach_Target(bool = false) override;
		virtual void Assign_Destination(AbstractClass * target, bool = true) override;
		virtual bool Enter_Idle_Mode(bool initial=false, bool=true) override;
		virtual MoveType Can_Enter_Cell(CellClass const * cell, FacingType dir = FACING_NONE, int cell_height = -1, CellClass const * = 0, bool = true) const override;
		virtual void Per_Cell_Process(PCPType why) override;

		static void Read_INI(CCINIClass const & ini);
		static void Write_INI(CCINIClass & ini);

	private:
		static int Visceroid_Fire_List[FACING_COUNT];
		static int Harvester_Load_List[FACING_COUNT];
		static char const * const INI_NAME;
};

inline UnitClass * AbstractClass::As_UnitClass(void)
{
	return(dynamic_cast<UnitClass *>(this));
}

inline UnitClass const * AbstractClass::As_UnitClass(void) const
{
	return(dynamic_cast<UnitClass const *>(this));
}

extern Rect UnitCompositeDirtyRect;
