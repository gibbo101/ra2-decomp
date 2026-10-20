// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"
#include "ccini.h"
#include "emember.h"

class TaskForceClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		TaskForceClass(char const *name=NULL);
		TaskForceClass(NoInitClass const & x);
		virtual ~TaskForceClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static TaskForceClass * Find_Or_Make(char const * name);

		static void Read_All(CCINIClass const & ini, INIScopeType scope);
		static void Write_All(CCINIClass & ini, INIScopeType scope);
		virtual bool Read_INI(CCINIClass & ini) override;
		virtual bool Write_INI(CCINIClass & ini) const override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_TASKFORCE);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual void Compute_CRC(CRCEngine & crc) const override;

		static TaskForceClass * From_Name(char const * name);
		static TaskForceClass * From_Given_Name(char const * name);

		int Required_Object_Count(void) const;
		int Needed_Tech_Level(void) const;
		bool Has_Only_Infantry(void) const;

		TaskForceClass(TaskForceClass const &that);
		TaskForceClass operator=(TaskForceClass const &that);

	public:

		int Group;

		int ClassCount;
		INIScopeType Scope;
		EnlistedMemberClass Members[MAX_TEAM_CLASSCOUNT];
};
