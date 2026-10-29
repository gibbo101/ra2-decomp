// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x006F0470 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006F04A0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006F04C0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006F04E0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006F0500 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006F0520 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006F0540 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006F0580 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006F05B0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006F05E0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006F0610 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006F0620 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006F0640 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006F0660 SYMBOL
// _$E59

DynamicVectorClass<TeamTypeClass *> TeamTypeClass::Array;

// FUNCTION: GAMEMD 0x006F0680
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

// FUNCTION: GAMEMD 0x006F06E0
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

// FUNCTION: GAMEMD 0x006F08F0
TeamTypeClass::~TeamTypeClass(void)
{
	Detach_This_From_All(this, true);
	Array.Delete(this);
	PointerExpiredNotification::NotifyInvalidType.Delete(this);
}

void __cdecl FUN_004068E0(char const * format, ...);
bool FUN_00510F60(int index);
HouseClass * FUN_00510ED0(int index);
int __fastcall FUN_005889F0(int zone, int other);

// FUNCTION: GAMEMD 0x006F09C0
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

// FUNCTION: GAMEMD 0x006F0A70
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

// FUNCTION: GAMEMD 0x006F0FC0
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

// FUNCTION: GAMEMD 0x006F1030
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

// FUNCTION: GAMEMD 0x006F1090
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

// FUNCTION: GAMEMD 0x006F1550
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

// FUNCTION: GAMEMD 0x006F1870
int TeamTypeClass::Get_Group(void) const
{
	if (Group != -2 && Group == -1) {
		if (TaskForce != NULL) {
			return(TaskForce->Group);
		}
	}
	return(Group);
}

// FUNCTION: GAMEMD 0x006F18A0
Cell TeamTypeClass::Get_Origin(void) const
{
	if (Origin == -1) {
		return(CELL_NONE);
	}
	return(Scen->Get_Waypoint_Cell(Origin));
}

// FUNCTION: GAMEMD 0x006F18E0
Cell TeamTypeClass::Get_Transport_Origin(void) const
{
	if (TransportOrigin == -1) {
		return(CELL_NONE);
	}
	return(Scen->Get_Waypoint_Cell(TransportOrigin));
}

// FUNCTION: GAMEMD 0x006F1920
TeamTypeClass * TeamTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<TeamTypeClass>(name, Array));
}

// FUNCTION: GAMEMD 0x006F19B0
void TeamTypeClass::Read_All(CCINIClass const & ini, INIScopeType scope)
{
	char buffer[32];

	int len = ini.Entry_Count("TeamTypes");
	for (int index = 0; index < len; index++) {
		char const * entry = ini.Get_Entry("TeamTypes", index);
		ini.Get_String("TeamTypes", entry, "", buffer, sizeof(buffer));
		TeamTypeClass * ttptr = Find_Or_Make(buffer);
		ttptr->Read_INI((CCINIClass &)ini);
		ttptr->Scope = scope;
	}
}

// FUNCTION: GAMEMD 0x006F1AB0
void TeamTypeClass::Write_All(CCINIClass & ini, INIScopeType scope)
{
	char buffer[32];
	int index;

	int numtypes = ini.Entry_Count("TeamTypes");
	for (index = 0; index < numtypes; index++) {
		char const * entry = ini.Get_Entry("TeamTypes", index);
		ini.Get_String("TeamTypes", entry, "", buffer, sizeof(buffer));
		ini.Clear(buffer);
	}
	ini.Clear("TeamTypes");

	int i = 0;
	for (index = 0; index < Array.Count(); index++) {
		if (Array[index]->Scope == scope) {
			sprintf(buffer, "%d", i++);
			ini.Put_String("TeamTypes", buffer, (char const *)Array[index]->IniName);
			Array[index]->Write_INI(ini);
		}
	}
}

// FUNCTION: GAMEMD 0x006F1B90
HRESULT STDMETHODCALLTYPE TeamTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006F1BB0
HRESULT STDMETHODCALLTYPE TeamTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TeamTypeClass(NoInitClass());

		Swizzle_Pointer(&House);
		Swizzle_Pointer(&Tag);
		Swizzle_Pointer(&Script);
		Swizzle_Pointer(&TaskForce);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006F1C40
HRESULT STDMETHODCALLTYPE TeamTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TeamTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006F1C80
void TeamTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Group);
	crc(IsRecruiter);
	crc(IsAutocreate);
	crc(IsPrebuilt);
	crc(IsReinforcable);
	crc(IsWhiner);
	crc(IsAggressive);
	crc(IsLooseRecruit);
	crc(IsSuicide);
	crc(IsDroppod);
	crc(IsUseTransportOrigin);
	crc(RecruitPriority);
	crc(MaxAllowed);
	crc(Fear);

	if (House != NULL) {
		crc((RTTIType)House->Fetch_RTTI());
	}
	if (House != NULL) {
		crc(House->Fetch_ID());
	}

	crc(TechLevel);

	if (Tag != NULL) {
		crc(Tag->Fetch_ID());
	}

	crc(Origin);
	crc(TransportOrigin);
	crc(Number);

	if (Script != NULL) {
		crc(Script->Fetch_ID());
	}

	if (TaskForce != NULL) {
		crc(TaskForce->Fetch_ID());
	}
}

// FUNCTION: GAMEMD 0x006F1E20
bool TeamTypeClass::Can_Recruit(FootClass * foot, HouseClass * house) const
{
	if (foot->Team != NULL && this == foot->Team->Class) {
		return(false);
	}

	if (foot->Strength <= 0 || !foot->IsActive || foot->IsInLimbo) {
		return(false);
	}

	if (foot->HasAnyLink()) {
		return(false);
	}

	if (foot->House != house) {
		return(false);
	}

	if (foot->Get_Mission() != MISSION_NONE && !MissionClass::Is_Recruitable_Mission(foot->Get_Mission())) {
		return(false);
	}

	if ((!foot->IsTeamRecruitable && !IsAutocreate) || (!foot->IsAutocreateRecruitable && IsAutocreate)) {
		return(false);
	}

	if (foot->Team != NULL && foot->Team->Class->RecruitPriority >= RecruitPriority) {
		return(false);
	}

	if (foot->Fetch_RTTI() == RTTI_AIRCRAFT && foot->Get_Primary_Weapon() != NULL && foot->Ammo == 0) {
		return(false);
	}

	return(true);
}

// FUNCTION: GAMEMD 0x006F1F30
void TeamTypeClass::Flash(int frames)
{
	for (int index = 0; index < Teams.Count(); index++) {
		TeamClass * team = Teams[index];
		if (team->Class == this) {
			team->Flash(frames);
		}
	}
}

// FUNCTION: GAMEMD 0x006F1F70
TeamClass * TeamTypeClass::Find_First_Of_Type(void)
{
	for (int index = 0; index < Teams.Count(); index++) {
		TeamClass *team = Teams[index];
		if (team->Class == this) {
			return(team);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x006F1FA0
void TeamTypeClass::Process_Task_Force(void)
{
	field_F0 = true;
	field_F1 = false;
	field_EC = 9;
	for (int index = 0; index < TaskForce->ClassCount; index++) {
		TechnoTypeClass const * type = TaskForce->Members[index].Class;
		if (type->Naval) {
			if (type->MaxPassengers == 0) {
				field_F0 = false;
			} else {
				field_F1 = true;
			}
		}
		field_EC = FUN_005889F0(type->MZone, field_EC);
	}
	if (IsBaseDefense) {
		field_F0 = false;
	}
}

// FUNCTION: GAMEMD 0x006F2040
void TeamTypeClass::Process_All_Task_Forces(void)
{
	for (int index = 0; index < Array.Count(); index++) {
		Array[index]->Process_Task_Force();
	}
}

// FUNCTION: GAMEMD 0x006F2070
HouseClass * TeamTypeClass::Get_House(void) const
{
	if (House != NULL) {
		return(House);
	}
	if (FUN_00510F60(idxHouse)) {
		return(FUN_00510ED0(idxHouse));
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x006F20A0
// TeamTypeClass::Fetch_RTTI
// FUNCTION: GAMEMD 0x006F20B0
// TeamTypeClass::Fetch_Object_Size
// FUNCTION: GAMEMD 0x006F20C0
// TeamTypeClass::Fetch_Heap_ID

// SYNTHETIC: GAMEMD 0x006F20D0
// ??_GTeamTypeClass@@UAEPAXI@Z

typedef DiscreteDistributionClass<AITriggerTypeClass>::DIST_OBJ TRIGGER_DIST_OBJ;

// TEMPLATE: GAMEMD 0x006F21B0
// ?Clear@?$DynamicVectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAEXXZ
template class DynamicVectorClass<TRIGGER_DIST_OBJ>;

// TEMPLATE: GAMEMD 0x006F21F0
// ?ID@?$DynamicVectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAEHPBV?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@Z
// TEMPLATE: GAMEMD 0x006F2210
// ?GetItem@?$VectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UBE?AV?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@H@Z

// TEMPLATE: GAMEMD 0x006F2230
// ??0?$DynamicVectorClass@PBVTeamTypeClass@@@@QAE@IPBQBVTeamTypeClass@@@Z
template class DynamicVectorClass<TeamTypeClass const *>;

// TEMPLATE: GAMEMD 0x006F2290
// ??8?$VectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x006F22F0
// ?Resize@?$VectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAE_NHPBV?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@Z
// TEMPLATE: GAMEMD 0x006F2400
// ?Clear@?$VectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAEXXZ
template class VectorClass<TRIGGER_DIST_OBJ>;

// TEMPLATE: GAMEMD 0x006F2440
// ?ID@?$VectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAEHABV?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@Z

// TEMPLATE: GAMEMD 0x006F2490
// ?ID@?$VectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAEHPBV?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@Z
// TEMPLATE: GAMEMD 0x006F24B0
// ?Resize@?$DynamicVectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAE_NHPBV?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@Z
// TEMPLATE: GAMEMD 0x006F25E0
// ?Add@?$DynamicVectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@QAE_NABV?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@Z
// TEMPLATE: GAMEMD 0x006F2640
// ?ID@?$DynamicVectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAEHABV?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@Z
// TEMPLATE: GAMEMD 0x006F25D0
// ??1?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@QAE@XZ
// SYNTHETIC: GAMEMD 0x006F2680
// ??_G?$VectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x006F26E0
// ??_E?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@QAEPAXI@Z
// SYNTHETIC: GAMEMD 0x006F2720
// ??_G?$DynamicVectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x006F2780
// ??4?$VectorClass@PBVTeamTypeClass@@@@QAEAAV0@ABV0@@Z
template class VectorClass<TeamTypeClass const *>;
// TEMPLATE: GAMEMD 0x006F27F0
// ??0?$VectorClass@V?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@@QAE@HPBV?$DistributionObject@PAVAITriggerTypeClass@@@?$DiscreteDistributionClass@VAITriggerTypeClass@@@@@Z
