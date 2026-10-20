// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "taskforc.h"
#include "ahtoi.h"
#include "ccini.h"
#include "crc.h"
#include "findmake.h"
#include "globals.h"
#include "sun.h"
#include "session.h"
#include "swizzle.h"
#include "techtype.h"
#include "tracker.h"
#include "trim.h"
#include "vector.h"
#include <cstdio>
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007F46F0;
extern const double DAT_007F46F8;

// FUNCTION: 0x006E7E80
TaskForceClass::TaskForceClass(char const *name) :
	BASECLASS(name),
	Group(-1),
	ClassCount(0),
	Scope(SCOPE_LOCAL)
{
	TaskForces.Add(this);

	for (int i = 0; i < MAX_TEAM_CLASSCOUNT; i++) {
		Members[i] = EnlistedMemberClass(0,NULL);
	}
}

// FUNCTION: 0x006E7F50
TaskForceClass::TaskForceClass(NoInitClass const & x) : BASECLASS(x) {}

// FUNCTION: 0x006E7F80
TaskForceClass::~TaskForceClass(void)
{
	Detach_This_From_All(this, true);
	TaskForces.Delete(this);
}

// FUNCTION: 0x006E8000
TaskForceClass::TaskForceClass(TaskForceClass const & that) :
	BASECLASS(that)
{
	Group = that.Group;
	ClassCount = that.ClassCount;
	Scope = that.Scope;

	for (int i = 0; i < ClassCount; i++) {
		Members[i] = that.Members[i];
	}
}

// FUNCTION: 0x006E80F0
TaskForceClass TaskForceClass::operator=(TaskForceClass const & that)
{
	if (this != &that) {
		Group = that.Group;
		ClassCount = that.ClassCount;
		Scope = that.Scope;

		for (int i = 0; i < ClassCount; i++) {
			Members[i] = that.Members[i];
		}
	}
	return(*this);
}

// FUNCTION: 0x006E8160
int TaskForceClass::Required_Object_Count(void) const
{
	int desired = 0;
	for (int index = 0; index < ClassCount; index++) {
		desired += Members[index].Quantity;
	}
	return(desired);
}

// FUNCTION: 0x006E8180
TaskForceClass * TaskForceClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int index = 0; index < TaskForces.Count(); index++) {
			if (stricmp(name, TaskForces[index]->IniName) == 0) {
				return(TaskForces[index]);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x006E81D0
TaskForceClass * TaskForceClass::From_Given_Name(char const * name)
{
	if (name != NULL) {
		for (int index = 0; index < TaskForces.Count(); index++) {
			if (stricmp(name, TaskForces[index]->GivenName) == 0) {
				return(TaskForces[index]);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x006E8220
void TaskForceClass::Read_All(CCINIClass const & ini, INIScopeType scope)
{
	int len = ini.Entry_Count("TaskForces");
	for (int index = 0; index < len; index++) {
		char const * entry = ini.Get_Entry("TaskForces", index);
		char name[24];
		if (ini.Get_String("TaskForces", entry, "", name, sizeof(name)) <= 0) {
			continue;
		}

		TaskForceClass * tforce = TFind_Or_Make<TaskForceClass>(name, TaskForces);
		Swizzler.Here_I_Am(ahtoi(name), (void*)tforce);

		tforce->Read_INI((CCINIClass &)ini);
		tforce->Scope = scope;
	}
}

// FUNCTION: 0x006E8340
void TaskForceClass::Write_All(CCINIClass & ini, INIScopeType scope)
{
	char buffer[32];
	int index;

	int numtypes = ini.Entry_Count("TaskForces");
	for (index = 0; index < numtypes; index++) {
		char const * entry = ini.Get_Entry("TaskForces", index);
		ini.Get_String("TaskForces", entry, "", buffer, sizeof(buffer));
		ini.Clear(buffer);
	}
	ini.Clear("TaskForces");

	int i = 0;
	for (index = 0; index < TaskForces.Count(); index++) {
		TaskForceClass *tforce = TaskForces[index];
		if (tforce->Scope == scope) {
			sprintf(buffer, "%d", i++);
			strtrim(buffer);
			ini.Put_String("TaskForces", buffer, (char const *)tforce->IniName);
			tforce->Write_INI(ini);
		}
	}
}

// FUNCTION: 0x006E8420
bool TaskForceClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		char buf[128];
		char entry[32];

		ClassCount = 0;

		for (int index = 0; index < MAX_TEAM_CLASSCOUNT; index++) {
			sprintf(entry, "%d", index);

			if (ini.Get_String(Name(), entry, "", buf, sizeof(buf)) > 0) {
				Members[ClassCount] = EnlistedMemberClass(buf);
				if (Members[ClassCount].Class != NULL) {
					ClassCount++;
				}
			}
		}
		Group = ini.Get_Int(IniName, "Group", Group);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E8510
bool TaskForceClass::Write_INI(CCINIClass & ini) const
{
	if (BASECLASS::Write_INI(ini)) {
		char entry[32];

		for (int index = 0; index < MAX_TEAM_CLASSCOUNT; index++) {
			sprintf(entry, "%d", index);
			if (index < ClassCount) {
				ini.Put_String(Name(), entry, Members[index].Build_INI_Entry());
			} else {
				ini.Clear(Name(), entry);
			}
		}
		ini.Put_Int(IniName, "Group", Group, 0);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E85B0
bool TaskForceClass::Has_Only_Infantry(void) const
{
	for (int i = 0; i < ClassCount; i++) {
		if (Members[i].Class->Fetch_RTTI() != RTTI_INFANTRYTYPE) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: 0x006E85F0
TaskForceClass * TaskForceClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<TaskForceClass>(name, TaskForces));
}

// FUNCTION: 0x006E8680
HRESULT STDMETHODCALLTYPE TaskForceClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006E86A0
HRESULT STDMETHODCALLTYPE TaskForceClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TaskForceClass(NoInitClass());

		for (int i = 0; i < MAX_TEAM_CLASSCOUNT; i++) {
			Swizzle_Pointer(&Members[i].Class);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006E8710
HRESULT STDMETHODCALLTYPE TaskForceClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TaskForceClass;
	return(S_OK);
}

// FUNCTION: 0x006E8750
void TaskForceClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(Group);
	crc(ClassCount);
}

// FUNCTION: 0x006E8780
int TaskForceClass::Needed_Tech_Level(void) const
{
	int needed = 0;

	for (int i = 0; i < ClassCount; i++) {
		int level = Members[i].Class->Level;
		if (level > needed) {
			needed = level;
		} else {
			if (level == -1 && Session.Type != GAME_NORMAL) {
				needed = 11;
			}
		}
	}
	return(needed);
}

// FUNCTION: 0x006E87D0 TaskForceClass::Fetch_RTTI

// FUNCTION: 0x006E87E0 TaskForceClass::Fetch_Object_Size

// FUNCTION: 0x006E87F0 TaskForceClass::~TaskForceClass

// FUNCTION: 0x006E8880 $E37
double DAT_00B0E950 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x006E88B0 $E40
double DAT_00B0E9A0 = DAT_00B0E950 / DAT_007F46F0;

// FUNCTION: 0x006E88D0 $E43
double DAT_00B0E988 = DAT_007E1718 / DAT_00B0E9A0;

// FUNCTION: 0x006E88F0 $E46
double DAT_00B0E980 = DAT_007E1720 * DAT_007F46F8;
