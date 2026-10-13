// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"
#include "ccini.h"
#include "vector.h"
#include "typelist.h"
#include "types.h"

#include "dialog.hh"
#include "tmission.hh"

class TechnoTypeClass;
class TriggerTypeClass;
class TeamClass;
class HouseClass;
class ScriptTypeClass;
class TaskForceClass;
class TagTypeClass;
class TeamTypeClass;

typedef TypeList<TeamTypeClass const *> SUGGESTED_TEAM_LIST;

class TeamTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:

		TeamTypeClass(char const * name = NULL);
		TeamTypeClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~TeamTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static DynamicVectorClass<TeamTypeClass *> Array;

		static TeamTypeClass * Find_Or_Make(char const * ininame = NULL);

		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_TEAMTYPE);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);};

		static void Read_All(CCINIClass const & ini, INIScopeType scope);
		static void Write_All(CCINIClass & ini, INIScopeType scope);
		virtual bool Read_INI(CCINIClass & ini) override;
		virtual bool Write_INI(CCINIClass & ini) const override;

		TeamClass * Create_One_Of(HouseClass *house=NULL) const;
		void Destroy_All_Of(void) const;
		void Detach(AbstractClass const * target, bool all=true);

		static char const * Name_From_Mission(TeamMissionType order);
		static TeamMissionType Mission_From_Name(char const *name);
		static SUGGESTED_TEAM_LIST Suggested_New_Team(HouseClass * house, bool);
		static TeamTypeClass * From_Name(char const * name);
#ifdef _DEBUG
		char const * Member_Description(void) const;
		char const * Description(void) const;
		operator const char * (void) const {return(Description());};
#endif
		TeamClass *Find_First_Of_Type(void);

		int Get_Group(void) const;
		Cell Get_Origin(void) const;
		Cell Get_Transport_Origin(void) const;
		void Process_Task_Force(void);
		static void Process_All_Task_Forces(void);
		HouseClass * Get_House(void) const;

		bool Can_Recruit(FootClass * foot, HouseClass * house) const;
		void Flash(int);

		int HeapID;

		int Group;

		int VeteranLevel;

		bool IsLoadable;

		bool IsFull;

		bool IsAnnoyance;

		bool IsGuardSlower;

		bool IsRecruiter;

		bool IsAutocreate;

		bool IsPrebuilt;

		bool IsReinforcable;

		bool IsWhiner;

		bool IsAggressive;

		bool IsLooseRecruit;

		bool IsSuicide;

		bool IsDroppod;
		bool IsUseTransportOrigin;

		bool IsDropship;

		bool OnTransOnly;

		int RecruitPriority;

		int MaxAllowed;

		int Fear;
		int MindControlDecision;

		HouseClass * House;

		int idxHouse;
		int TechLevel;

		TagTypeClass * Tag;

		WAYPOINT Origin;
		WAYPOINT TransportOrigin;

		int Number;

		ScriptTypeClass * Script;

		TaskForceClass * TaskForce;

		INIScopeType Scope;
		int field_EC;
		bool field_F0;
		bool field_F1;

		bool AvoidThreats;

		bool IsIonImmune;

		bool TransportsReturnOnUnload;

		bool AreTeamMembersRecruitable;

		bool IsBaseDefense;

		bool OnlyTargetHouseEnemy;
};
