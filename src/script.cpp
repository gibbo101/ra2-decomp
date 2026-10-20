// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"

#include "script.h"

#include "_script.h"
#include "ccini.h"
#include "crc.h"
#include "rect.h"
#include "findmake.h"
#include "globals.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "trim.h"
#include "vector.h"

#include <cstdio>

// FUNCTION: 0x006913C0
ScriptClass::ScriptClass(ScriptTypeClass *type):
	Unused1(0),
	Class(type),
	CurrentLineNumber(-1)
{
	Scripts.Add(this);
}

// FUNCTION: 0x00691460
ScriptClass::~ScriptClass(void)
{
	Detach_This_From_All(this, true);
	Scripts.Delete(this);
}

// FUNCTION: 0x006914E0
void ScriptClass::Compute_CRC(CRCEngine &crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(CurrentLineNumber);
}

// FUNCTION: 0x00691500
TeamMissionClass ScriptClass::Get_Current_Mission(void)
{
	if (CurrentLineNumber == -1) {
		return(TeamMissionClass(TMISSION_NONE, 0));
	}
	return(Class->MissionList[CurrentLineNumber]);
}

// FUNCTION: 0x00691540
TeamMissionClass ScriptClass::Get_Next_Mission(void)
{
	if (CurrentLineNumber + 1 >= Class->MissionCount) {
		return(TeamMissionClass(TMISSION_NONE, 0));
	}
	return(Class->MissionList[CurrentLineNumber + 1]);
}

// FUNCTION: 0x00691590
bool ScriptClass::Stop_Script(void)
{
	CurrentLineNumber = -1;
	return(true);
}

// FUNCTION: 0x006915A0
bool ScriptClass::Set_Line(int linenum)
{
	CurrentLineNumber = linenum;
	return(true);
}

// FUNCTION: 0x006915B0
bool ScriptClass::Next_Mission(void)
{
	CurrentLineNumber++;
	return(Has_Missions_Remaining());
}

// FUNCTION: 0x006915D0
bool ScriptClass::Has_Missions_Remaining(void)
{
	return((unsigned)CurrentLineNumber < (unsigned)Class->MissionCount);
}

// FUNCTION: 0x006915F0
HRESULT STDMETHODCALLTYPE ScriptClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ScriptClass;
	return(S_OK);
}

// FUNCTION: 0x00691630
HRESULT STDMETHODCALLTYPE ScriptClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) ScriptClass(NoInitClass());

		Swizzle_Pointer(&Class);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00691690
HRESULT STDMETHODCALLTYPE ScriptClass::Save(IStream *stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006916B0
ScriptTypeClass::ScriptTypeClass(char const *name) :
	BASECLASS(name),
	Scope(SCOPE_LOCAL),
	MissionCount(0)
{
	ScriptTypes.Add(this);
	HeapID = ScriptTypes.ID(this);
}

// FUNCTION: 0x00691770
ScriptTypeClass::~ScriptTypeClass(void)
{
	Detach_This_From_All(this, true);
	ScriptTypes.Delete(this);
}

// FUNCTION: 0x006917F0
bool ScriptTypeClass::Write_INI(CCINIClass & ini) const
{
	if (BASECLASS::Write_INI(ini)) {
		char buf[128];
		char entry[16];

		for (int index = 0; index < MAX_TEAM_MISSIONS; index++) {
			sprintf(entry, "%d", index);
			strtrim(entry);
			if (index < MissionCount) {
				MissionList[index].Build_INI_Entry(buf);
				ini.Put_String(Name(), entry, buf);
			} else {
				ini.Clear(Name(), entry);
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006918A0
bool ScriptTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		char buf[128];
		char entry[16];

		MissionCount = 0;

		for (int index = 0; index < MAX_TEAM_MISSIONS; index++) {
			TeamMissionClass tmission;

			sprintf(entry, "%d", index);
			strtrim(entry);

			if (ini.Get_String(Name(), entry, "", buf, sizeof(buf)) > 0) {
				tmission.Fill_In(buf);
				if (MissionCount < MAX_TEAM_MISSIONS) {
					MissionList[MissionCount] = tmission;
					MissionCount++;
				}
			}
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00691970
void ScriptTypeClass::Read_All(CCINIClass & ini, INIScopeType scope)
{
	int len = ini.Entry_Count("ScriptTypes");
	for (int index = 0; index < len; index++) {
		char const * entry = ini.Get_Entry("ScriptTypes", index);
		char name[24];
		if (ini.Get_String("ScriptTypes", entry, "", name, sizeof(name)) <= 0) {
			continue;
		}

		ScriptTypeClass * script = Find_Or_Make(name);
		script->Read_INI(ini);
		script->Scope = scope;
	}
}

// FUNCTION: 0x00691B30
void ScriptTypeClass::Write_All(CCINIClass & ini, INIScopeType scope)
{
}

// FUNCTION: 0x00691B40
ScriptTypeClass * ScriptTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int index = 0; index < ScriptTypes.Count(); index++) {
			if (stricmp(name, ScriptTypes[index]->IniName) == 0 || stricmp(name, ScriptTypes[index]->GivenName) == 0) {
				return(ScriptTypes[index]);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x00691BB0
ScriptTypeClass * ScriptTypeClass::From_Given_Name(char const * name)
{
	if (name != NULL) {
		for (int index = 0; index < ScriptTypes.Count(); index++) {
			if (stricmp(name, ScriptTypes[index]->GivenName) == 0) {
				return(ScriptTypes[index]);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x00691C00
ScriptTypeClass * ScriptTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<ScriptTypeClass>(name, ScriptTypes));
}

// FUNCTION: 0x00691D50
HRESULT STDMETHODCALLTYPE ScriptTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ScriptTypeClass;
	return(S_OK);
}

// FUNCTION: 0x00691D90
HRESULT STDMETHODCALLTYPE ScriptTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) ScriptTypeClass(NoInitClass());

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00691DE0
HRESULT STDMETHODCALLTYPE ScriptTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00691E00
void ScriptTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(MissionCount);
}

// FUNCTION: 0x00691E30
void ScriptTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (target != NULL && target->Fetch_RTTI() == RTTI_TEAMTYPE) {
		int id = -1;
		for (int index = 0; index < MissionCount; index++) {
			if (MissionList[index].Mission == TMISSION_TEAMCHANGE) {
				if (id == -1) {
					id = TeamTypes.ID((TeamTypeClass *)target);
					if (id == -1) {
						return;
					}
				}
				if (MissionList[index].Data.Value > id) {
					MissionList[index].Data.Value--;
				} else if (MissionList[index].Data.Value == id) {
					MissionList[index].Data.Value = 0;
				}
			}
		}
	}
}

// FUNCTION: 0x00691EC0 ScriptClass::Fetch_RTTI

// FUNCTION: 0x00691ED0 ScriptClass::Fetch_Object_Size

// FUNCTION: 0x00691EE0 ScriptClass::~ScriptClass

// FUNCTION: 0x00691F70 ScriptTypeClass::Fetch_RTTI

// FUNCTION: 0x00691F80 ScriptTypeClass::Fetch_Object_Size

// FUNCTION: 0x00691F90 ScriptTypeClass::Fetch_Heap_ID

// FUNCTION: 0x00691FA0 ScriptTypeClass::~ScriptTypeClass

// FUNCTION: 0x00692030 $E11
