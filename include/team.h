// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "timer.h"
#include "tmission.h"
#include "vector.h"

#include "result.hh"

class TeamTypeClass;
class ScriptClass;
class TechnoClass;
class TeamTypeClass;
class HouseClass;
class FootClass;
class TagClass;
class TechnoTypeClass;
class MonoClass;

typedef DynamicVectorClass<TechnoTypeClass const *> TEAM_MEMBER_LIST;

#define STRAY_DISTANCE		2

class TeamClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:

		TeamTypeClass * Class;

		ScriptClass * Script;

		HouseClass * House;

		HouseClass * HouseToScout;

		AbstractClass * Zone;

		AbstractClass * ClosestMember;

		AbstractClass * MissionTarget;
		AbstractClass * Target;

		void * UnusedPtr1;

		int Total;

		int Risk;

		int CreationFrame;

		FootClass * Member;

		CDTimerClass<FrameTimerClass> TimeOut;

		CDTimerClass<FrameTimerClass> SuspendTimer;

		TagClass * Tag;

		bool IsDeleteTypeWhenDone;

		bool IsRegrouping;

		bool NeedsReinforcement;

		bool IsForcedActive;

		bool IsHasBeen;

		bool IsFullStrength;

		bool IsUnderStrength;

		bool IsReforming;

		bool IsLagging;

		bool IsAltered;
		bool JustAltered;

		bool IsMoving;

		bool IsNextMission;
		bool TargetNotAssigned;

		bool IsLeaveMap;

		bool Suspended;

		bool Succeeded;

		TeamClass(TeamTypeClass const * team=0, HouseClass * owner=0, void * = NULL);
		TeamClass(NoInitClass const & x) : BASECLASS(x), TimeOut(x), SuspendTimer(x) {};
		virtual ~TeamClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(bool oldsave) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;

		bool Add(FootClass *);
		bool Remove(FootClass *, int typeindex=-1);
		bool Can_Add(FootClass * obj, int & typeindex) const;
		bool Is_Empty(void) const {return(Member == NULL);}
		FootClass * Get_Member(void) {return(Member);}
		void Team_Members(TEAM_MEMBER_LIST & list);
		bool Recalc_Strength(void);

		void AI(void);
		void Detach(AbstractClass const * target, bool all);
		void Took_Damage(FootClass * obj, ResultType result, TechnoClass * source);

		void Force_Active(void) {IsForcedActive = true;IsUnderStrength=false;};
		void Assign_Mission_Target(AbstractClass * new_target);
		void Flag_Into_Action(void);
		void Regroup(void);
		void Scan_Limit(void);
		void Flash(int count);
		bool Has_Entered_Map(void) const;
		bool Is_Leaving_Map(void) const;
		bool Has_Air_Transport(void) const;

		static void Suspend_Teams(int priority, HouseClass const * house);
#ifdef _DEBUG
		void Debug_Dump(MonoClass * mono) const;
#endif

	private:

		void TMission_ATTACK(TeamMissionClass * mission, bool);
		void TMission_ATT_WAYPT(TeamMissionClass * mission, bool);

		void TMission_BERZERK(TeamMissionClass * mission, bool);
		void TMission_MOVE(TeamMissionClass * mission, bool);
		void TMission_MOVECELL(TeamMissionClass * mission, bool);
		void TMission_GUARD(TeamMissionClass * mission, bool);
		void TMission_LOOP(TeamMissionClass * mission, bool);

		void TMission_WIN(TeamMissionClass * mission, bool);
		void TMission_UNLOAD(TeamMissionClass * mission, bool);
		void TMission_DEPLOY(TeamMissionClass * mission, bool);
		void TMission_HOUND_DOG(TeamMissionClass * mission, bool);
		void TMission_DO(TeamMissionClass * mission, bool);
		void TMission_SET_GLOBAL(TeamMissionClass * mission, bool);

		void TMission_IDLE_ANIM(TeamMissionClass * mission, bool);
		void TMission_LOAD(TeamMissionClass * mission, bool);
		void TMission_SPY(TeamMissionClass * mission, bool);
		void TMission_PATROL(TeamMissionClass * mission, bool);
		void TMission_SCRIPT(TeamMissionClass * mission, bool);
		void TMission_TEAMCHANGE(TeamMissionClass * mission, bool);
		void TMission_PANIC(TeamMissionClass * mission, bool);
		void TMission_CHANGE_HOUSE(TeamMissionClass * mission, bool);
		void TMission_SCATTER(TeamMissionClass * mission, bool);
		void TMission_GOTO_SHROUD(TeamMissionClass * mission, bool);
		void TMission_LOSE(TeamMissionClass * mission, bool);
		void TMission_PLAY_SPEECH(TeamMissionClass * mission, bool);
		void TMission_PLAY_SOUND(TeamMissionClass * mission, bool);
		void TMission_PLAY_MOVIE(TeamMissionClass * mission, bool);
		void TMission_PLAY_MUSIC(TeamMissionClass * mission, bool);
		void TMission_REDUCE_TIBERIUM(TeamMissionClass * mission, bool);
		void TMission_BEGIN_PRODUCTION(TeamMissionClass * mission, bool);
		void TMission_FIRE_SALE(TeamMissionClass * mission, bool);
		void TMission_SELF_DESTRUCT(TeamMissionClass * mission, bool);
		void TMission_ION_STORM_START(TeamMissionClass * mission, bool);
		void TMission_ION_STORM_END(TeamMissionClass * mission, bool);
		void TMission_CENTER_VIEWPOINT(TeamMissionClass * mission, bool);
		void TMission_RESHROUD(TeamMissionClass * mission, bool);
		void TMission_REVEAL(TeamMissionClass * mission, bool);
		void TMission_DESTROY_MEMBERS(TeamMissionClass * mission, bool);
		void TMission_CLEAR_GLOBAL(TeamMissionClass * mission, bool);
		void TMission_SET_LOCAL(TeamMissionClass * mission, bool);
		void TMission_CLEAR_LOCAL(TeamMissionClass * mission, bool);
		void TMission_UNPANIC(TeamMissionClass * mission, bool);
		void TMission_FORCE_FACING(TeamMissionClass * mission, bool);
		void TMission_FULLY_LOADED(TeamMissionClass * mission, bool);
		void TMission_UNLOAD_TRUCK(TeamMissionClass * mission, bool);
		void TMission_LOAD_TRUCK(TeamMissionClass * mission, bool);
		void TMission_ATTACK_BUILDING_WITH_PROPERTY(TeamMissionClass * mission, bool);
		void TMission_MOVETO_BUILDING_WITH_PROPERTY(TeamMissionClass * mission, bool);
		void TMission_SCOUT(TeamMissionClass * mission, bool);
		void TMission_SUCCESS(TeamMissionClass * mission, bool);
		void TMission_FLASH(TeamMissionClass * mission, bool);
		void TMission_PLAY_ANIM(TeamMissionClass * mission, bool);
		void TMission_TALK_BUBBLE(TeamMissionClass * mission, bool);

		int Recruit(int typeindex);
		bool Is_A_Member(void const * who) const;
		bool Coordinate_Regroup(void);
		void Coordinate_Attack(void);
		void Coordinate_Move(void);
		bool Coordinate_Conscript(FootClass * unit);
		void Calc_Center(AbstractClass *& center, AbstractClass *& obj_center) const;
		bool Lagging_Units(void);
		FootClass * Fetch_A_Leader(void) const;
		bool Ammo_Check(void) const;

	public:
		int Quantity[MAX_TEAM_CLASSCOUNT];
};
