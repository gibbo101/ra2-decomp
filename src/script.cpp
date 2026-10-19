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
