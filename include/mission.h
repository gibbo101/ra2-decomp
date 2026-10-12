// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ccini.h"
#include "ftimer.h"
#include "object.h"
#include "timer.h"

class MissionControlClass;

class MissionClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:

		MissionType CurrentMission;
		MissionType SuspendedMission;

		MissionType MissionQueue;

		bool IsMissionUnloadStandby;

		int Status;

		int CurrentMissionStartTime;

		int MissionAccumulateTime;

		MissionClass(void);
		MissionClass(NoInitClass const & x) : BASECLASS(x), Timer(x) {};
		virtual ~MissionClass(void) override {};

#ifdef _DEBUG
		void  Debug_Dump(MonoClass *mono) const;
#endif

		void Shorten_Mission_Timer(void) {Timer = 0;}
		virtual MissionType Get_Mission(void) const override;
		virtual void AI(void) override;
		virtual void Compute_CRC(CRCEngine &) const override;

		virtual void QueueMission(MissionType mission, bool start);
		virtual bool Commence(void);
		virtual void Set_Mission(MissionType mission);
		virtual void  Override_Mission(MissionType mission, AbstractClass *, AbstractClass *);
		virtual bool Restore_Mission(void);
		virtual bool Has_Suspended_Mission(void) const;
		virtual bool Ready_To_Next_Mission(void) const;

		virtual int Do_MISSION_SLEEP(void);
		virtual int Do_MISSION_HARMLESS(void);
		virtual int Do_MISSION_AMBUSH(void);
		virtual int Do_MISSION_ATTACK(void);
		virtual int Do_MISSION_CAPTURE(void);
		virtual int Do_MISSION_EATEN(void);
		virtual int Do_MISSION_GUARD(void);
		virtual int Do_MISSION_GUARD_AREA(void);
		virtual int Do_MISSION_HARVEST(void);
		virtual int Do_MISSION_HUNT(void);
		virtual int Do_MISSION_MOVE(void);
		virtual int Do_MISSION_RETREAT(void);
		virtual int Do_MISSION_RETURN(void);
		virtual int Do_MISSION_STOP(void);
		virtual int Do_MISSION_UNLOAD(void);
		virtual int Do_MISSION_ENTER(void);
		virtual int Do_MISSION_CONSTRUCTION(void);
		virtual int Do_MISSION_DECONSTRUCTION(void);
		virtual int Do_MISSION_REPAIR(void);
		virtual int Do_MISSION_MISSILE(void);
		virtual int Do_MISSION_OPEN(void);
		virtual int Do_MISSION_RESCUE(void);
		virtual int Do_MISSION_PATROL(void);
		virtual int Do_MISSION_PARADROP_APPROACH(void);
		virtual int Do_MISSION_PARADROP_OVERFLY(void);
		virtual int Do_MISSION_WAIT(void);
		virtual int Do_MISSION_SPYPLANE_APPROACH(void);
		virtual int Do_MISSION_SPYPLANE_OVERFLY(void);

		static bool Is_Recruitable_Mission(MissionType mission);

		static char const *  Mission_Name(MissionType order);
		static MissionType  Mission_From_Name(char const *name);
		MissionControlClass const & Current_Mission_Control(void) const;

	private:

		CDTimerClass<FrameTimerClass> Timer;
};

class MissionControlClass
{
	public:
		MissionControlClass(void);

		bool Read_INI(CCINIClass const & ini);
		int Normal_Delay(void) const {return(int(TICKS_PER_MINUTE * Rate));}
		int AA_Delay(void) const {return(int(TICKS_PER_MINUTE * AARate));}

		MissionType Mission;

		char const * Name(void) const;

		bool IsNoThreat;

		bool IsZombie;

		bool IsRecruitable;

		bool IsParalyzed;

		bool IsRetaliate;

		bool IsScatter;

		double Rate;

		double AARate;
};
