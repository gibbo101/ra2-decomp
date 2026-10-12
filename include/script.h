// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"
#include "ccini.h"
#include "tmission.h"

class ScriptTypeClass;

class ScriptClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

		friend class ScriptTypeClass;

	public:
		ScriptClass(ScriptTypeClass *type = NULL);
		ScriptClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~ScriptClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override { return(RTTI_SCRIPT); }
		virtual int Fetch_Object_Size(void) const override { return(sizeof(ScriptClass)); }

		virtual void Compute_CRC(CRCEngine &) const override;

		TeamMissionClass Get_Current_Mission(void);
		TeamMissionClass Get_Next_Mission(void);

		bool Stop_Script(void);
		bool Set_Line(int linenum);

		bool Next_Mission(void);
		bool Has_Missions_Remaining(void);

	private:

		ScriptTypeClass *Class;

		int Unused1;

		int CurrentLineNumber;
};

class ScriptTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

		friend class ScriptClass;

	public:
		ScriptTypeClass(char const *name = NULL);
		ScriptTypeClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~ScriptTypeClass(void) override;

		static ScriptTypeClass * Find_Or_Make(char const * ininame = NULL);

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static void Read_All(CCINIClass const & ini, INIScopeType scope);
		static void Write_All(CCINIClass & ini, INIScopeType scope);

		virtual bool Read_INI(CCINIClass const & ini) override;
		virtual bool Write_INI(CCINIClass & ini) const override;

		virtual void Compute_CRC(CRCEngine & crc) const override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_SCRIPTTYPE);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);};

		static ScriptTypeClass * From_Name(char const * name);
		static ScriptTypeClass * From_Given_Name(char const * name);

	public:

		int HeapID;

		INIScopeType Scope;

		int MissionCount;
		TeamMissionClass MissionList[MAX_TEAM_MISSIONS];

		static char const * const INI_NAME;
};
