// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "teamtype.h"
#include "_rules.h"
#include "aitrig.h"
#include "ccini.h"
#include "ccrand.h"
#include "dbgprint.h"
#include "ddist.h"
#include "findmake.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "notify.h"
#include "houstype.h"
#include "incdec.h"
#include "rules.h"
#include "scenario.h"
#include "script.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "tagtype.h"
#include "taskforc.h"
#include "team.h"
#include "tracker.h"
#include "vector.h"
#include "waypoint.h"

DynamicVectorClass<TeamTypeClass *> TeamTypeClass::Array;

// FUNCTION: 0x006F0680
int _ahtoi(const char *string)
{
	char *s = (char *)string;
	int integer = 0;
	while (isxdigit(s[0]))
	{
		integer *= 16;

		if (isdigit(s[0])) {
			integer += s[0] - '0';
		} else {
			integer += toupper(s[0]) - ('A' - 10);
		}
		s++;
	}

	return(integer);
}

// FUNCTION: 0x006F06E0
TeamTypeClass::TeamTypeClass(char const * name) :
	BASECLASS(name),
	HeapID(-1),
	Group(-1),
	VeteranLevel(1),
	IsLoadable(false),
	IsFull(false),
	IsAnnoyance(false),
	IsGuardSlower(false),
	IsRecruiter(false),
	IsAutocreate(false),
	IsPrebuilt(false),
	IsReinforcable(false),
	IsWhiner(false),
	IsAggressive(false),
	IsLooseRecruit(false),
	IsSuicide(false),
	IsDroppod(false),
	IsUseTransportOrigin(false),
	IsDropship(false),
	OnTransOnly(false),
	RecruitPriority(7),
	MaxAllowed(-1),
	Fear(0),
	MindControlDecision(0),
	House(NULL),
	idxHouse(-1),
	TechLevel(0),
	Tag(NULL),
	Origin(-1),
	TransportOrigin(-1),
	Number(0),
	Script(NULL),
	TaskForce(NULL),
	Scope(SCOPE_LOCAL),
	field_EC(9),
	field_F0(true),
	field_F1(false),
	AvoidThreats(false),
	IsIonImmune(false),
	TransportsReturnOnUnload(false),
	AreTeamMembersRecruitable(true),
	IsBaseDefense(false),
	OnlyTargetHouseEnemy(false)
{
	Array.Add(this);

	HeapID = Array.ID(this);
	PointerExpiredNotification::NotifyInvalidType.Add(this);
}

// FUNCTION: 0x006F08F0
TeamTypeClass::~TeamTypeClass(void)
{
	Detach_This_From_All(this, true);
	Array.Delete(this);
	PointerExpiredNotification::NotifyInvalidType.Delete(this);
}

void __cdecl FUN_004068E0(char const * format, ...);
bool FUN_00510F60(int index);
HouseClass * FUN_00510ED0(int index);

// FUNCTION: 0x006F09C0
TeamClass * TeamTypeClass::Create_One_Of(HouseClass * house) const
{
	FUN_004068E0("Create_One_Of...\n");
	if (house == NULL) {
		house = House;
		if (house == NULL) {
			if (!FUN_00510F60(idxHouse)) return(NULL);
			house = FUN_00510ED0(idxHouse);
			if (house == NULL) return(NULL);
		}
	}
	if (ScenarioInit || MaxAllowed < 0 ||
		((Session.Type == GAME_NORMAL && Number < MaxAllowed) ||
		 (Session.Type != GAME_NORMAL && house->Owned_Team_Count(this) < MaxAllowed))) {

		FUN_004068E0("Creating a new team named '%s'.\n", (char const *)GivenName);
		return(new TeamClass(this, house));
	}
	return(NULL);
}

// FUNCTION: 0x006F0A70
void TeamTypeClass::Destroy_All_Of(void) const
{
	for (int index = 0; index < Teams.Count(); index++) {
		TeamClass * team = Teams[index];

		if (this == team->Class) {
			delete team;
			index--;
		}
	}
}

// FUNCTION: 0x006F0FC0
TeamTypeClass * TeamTypeClass::From_Name(char const * name)
{
	if (name) {
		for (int index = 0; index < Array.Count(); index++) {
			if (stricmp(name, Array[index]->IniName) == 0 || stricmp(name, Array[index]->GivenName) == 0) {
				return(Array[index]);
			}
		}
	}
	return(0);
}

// FUNCTION: 0x006F1030
void TeamTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (Tag == target) {
		Tag = NULL;
	}
	if (House == target) {
		House = NULL;
	}
	if (TaskForce == target) {
		TaskForce = NULL;
	}
	if (Script == target) {
		Script = NULL;
	}
	if (Script != NULL) {
		Script->Detach(target, all);
	}
}

// FUNCTION: 0x006F1090
bool TeamTypeClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		HousesType house = HOUSE_NONE;
		if (House != NULL) {
			house = House->Class->House;
		}
		house = ini.Get_HousesType(IniName, "House", house);
		if (FUN_00510F60(house)) {
			idxHouse = house;
			House = NULL;
		} else if (house != HOUSE_NONE) {
			House = House_From_HousesType(house);
			idxHouse = -1;
		}

		VeteranLevel = ini.Get_Int(IniName, "VeteranLevel", VeteranLevel);
		MindControlDecision = ini.Get_Int(IniName, "MindControlDecision", MindControlDecision);
		IsLoadable = ini.Get_Bool(IniName, "Loadable", IsLoadable);
		IsFull = ini.Get_Bool(IniName, "Full", IsFull);
		IsAnnoyance = ini.Get_Bool(IniName, "Annoyance", IsAnnoyance);
		IsGuardSlower = ini.Get_Bool(IniName, "GuardSlower", IsGuardSlower);
		IsAutocreate = ini.Get_Bool(IniName, "Autocreate", IsAutocreate);
		IsPrebuilt = ini.Get_Bool(IniName, "Prebuild", IsPrebuilt);
		IsReinforcable = ini.Get_Bool(IniName, "Reinforce", IsReinforcable);
		IsDroppod = ini.Get_Bool(IniName, "Droppod", IsDroppod);
		IsUseTransportOrigin = ini.Get_Bool(IniName, "UseTransportOrigin", IsUseTransportOrigin);
		IsRecruiter = ini.Get_Bool(IniName, "Recruiter", IsRecruiter);
		IsWhiner = ini.Get_Bool(IniName, "Whiner", IsWhiner);
		IsSuicide = ini.Get_Bool(IniName, "Suicide", IsSuicide);
		IsLooseRecruit = ini.Get_Bool(IniName, "LooseRecruit", IsLooseRecruit);
		IsAggressive = ini.Get_Bool(IniName, "Aggressive", IsAggressive);
		OnTransOnly = ini.Get_Bool(IniName, "OnTransOnly", OnTransOnly);
		RecruitPriority = ini.Get_Int(IniName, "Priority", RecruitPriority);
		MaxAllowed = ini.Get_Int(IniName, "Max", MaxAllowed);
		TechLevel = ini.Get_Int(IniName, "TechLevel", TechLevel);

		Tag = TGet_Class(ini, IniName, "Tag", Tag);

		Group = ini.Get_Int(IniName, "Group", Group);
		AvoidThreats = ini.Get_Bool(IniName, "AvoidThreats", AvoidThreats);
		IsIonImmune = ini.Get_Bool(IniName, "IonImmune", IsIonImmune);
		IsBaseDefense = ini.Get_Bool(IniName, "IsBaseDefense", IsBaseDefense);
		OnlyTargetHouseEnemy = ini.Get_Bool(IniName, "OnlyTargetHouseEnemy", OnlyTargetHouseEnemy);
		TransportsReturnOnUnload = ini.Get_Bool(IniName, "TransportsReturnOnUnload", TransportsReturnOnUnload);
		AreTeamMembersRecruitable = ini.Get_Bool(IniName, "AreTeamMembersRecruitable", AreTeamMembersRecruitable);

		char waypt[64];
		if (ini.Get_String(IniName, "Waypoint", Waypoint_To_Name(Origin), waypt, sizeof(waypt)) > 0) {
			Origin = Waypoint_From_Name(waypt);
		}
		if (ini.Get_String(IniName, "TransportWaypoint", Waypoint_To_Name(TransportOrigin), waypt, sizeof(waypt)) > 0) {
			TransportOrigin = Waypoint_From_Name(waypt);
		}

		Script = TGet_Class(ini, IniName, "Script", Script);
		TaskForce = TGet_Class(ini, IniName, "TaskForce", TaskForce);

		if (TaskForce == NULL) {
			if (TaskForces.Count() <= 0) return(false);
			TaskForce = TaskForces[0];
		}
		if (Script == NULL) {
			if (ScriptTypes.Count() <= 0) return(false);
			Script = ScriptTypes[0];
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006F1550
bool TeamTypeClass::Write_INI(CCINIClass & ini) const
{
	if (BASECLASS::Write_INI(ini)) {
		HousesType house = HOUSE_NONE;
		if (House != NULL) {
			house = House->Class->House;
		} else if (FUN_00510F60(idxHouse)) {
			house = (HousesType)idxHouse;
		}

		ini.Put_Int(IniName, "VeteranLevel", VeteranLevel);
		ini.Put_Int(IniName, "MindControlDecision", MindControlDecision);
		ini.Put_Bool(IniName, "Loadable", IsLoadable);
		ini.Put_Bool(IniName, "Full", IsFull);
		ini.Put_Bool(IniName, "Annoyance", IsAnnoyance);
		ini.Put_Bool(IniName, "GuardSlower", IsGuardSlower);
		ini.Put_HousesType(IniName, "House", house);
		ini.Put_Bool(IniName, "Recruiter", IsRecruiter);
		ini.Put_Bool(IniName, "Autocreate", IsAutocreate);
		ini.Put_Bool(IniName, "Prebuild", IsPrebuilt);
		ini.Put_Bool(IniName, "Reinforce", IsReinforcable);
		ini.Put_Bool(IniName, "Droppod", IsDroppod);
		ini.Put_Bool(IniName, "UseTransportOrigin", IsUseTransportOrigin);
		ini.Put_Bool(IniName, "Whiner", IsWhiner);
		ini.Put_Bool(IniName, "LooseRecruit", IsLooseRecruit);
		ini.Put_Bool(IniName, "Aggressive", IsAggressive);
		ini.Put_Bool(IniName, "Suicide", IsSuicide);
		ini.Put_Int(IniName, "Priority", RecruitPriority);
		ini.Put_Int(IniName, "Max", MaxAllowed);
		ini.Put_Int(IniName, "TechLevel", TechLevel);
		ini.Put_Int(IniName, "Group", Group);
		ini.Put_Bool(IniName, "OnTransOnly", OnTransOnly);
		ini.Put_Bool(IniName, "AvoidThreats", AvoidThreats);
		ini.Put_Bool(IniName, "IonImmune", IsIonImmune);
		ini.Put_Bool(IniName, "TransportsReturnOnUnload", TransportsReturnOnUnload);
		ini.Put_Bool(IniName, "AreTeamMembersRecruitable", AreTeamMembersRecruitable);
		ini.Put_Bool(IniName, "IsBaseDefense", IsBaseDefense);
		ini.Put_Bool(IniName, "OnlyTargetHouseEnemy", OnlyTargetHouseEnemy);

		if (Tag != NULL) {
			ini.Put_String(IniName, "Tag", Tag->IniName);
		}

		ini.Put_String(IniName, "Waypoint", Waypoint_To_Name(Origin));
		ini.Put_String(IniName, "TransportWaypoint", Waypoint_To_Name(TransportOrigin));

		if (Script != NULL) {
			ini.Put_String(IniName, "Script", Script->IniName);
		}

		if (TaskForce != NULL) {
			ini.Put_String(IniName, "TaskForce", TaskForce->IniName);
		}

		return(true);
	}
	return(false);
}

// FUNCTION: 0x006F1870
int TeamTypeClass::Get_Group(void) const
{
	if (Group != -2 && Group == -1) {
		if (TaskForce != NULL) {
			return(TaskForce->Group);
		}
	}
	return(Group);
}

// FUNCTION: 0x006F18A0
Cell TeamTypeClass::Get_Origin(void) const
{
	if (Origin == -1) {
		return(CELL_NONE);
	}
	return(Scen->Get_Waypoint_Cell(Origin));
}

// FUNCTION: 0x006F1B90
HRESULT STDMETHODCALLTYPE TeamTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006F1C40
HRESULT STDMETHODCALLTYPE TeamTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TeamTypeClass;
	return(S_OK);
}
