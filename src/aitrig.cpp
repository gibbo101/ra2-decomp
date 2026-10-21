// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "aitrig.h"
#include "_rules.h"
#include "airctype.h"
#include "builtype.h"
#include "crc.h"
#include "findmake.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "infatype.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "taskforc.h"
#include "techtype.h"
#include "trim.h"
#include "unittype.h"
#include "super.h"
#include "suprtype.h"
#include "vector.h"
#include "map.h"
#include "_map.h"
#include "teamtype.h"

#include <math.h>

extern char const * DAT_00818114;
extern char const * DAT_00818118;
extern SideType FUN_006A46D0(char const * name);
extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E2A08;
extern const double DAT_007E2A10;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x0041E160 SYMBOL
// _$E104
double DAT_0088A020 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0041E190 SYMBOL
// _$E107
double DAT_0088A070 = DAT_0088A020 / DAT_007E2A08;

// SYNTHETIC: GAMEMD 0x0041E1B0 SYMBOL
// _$E110
double DAT_0088A058 = DAT_007E1718 / DAT_0088A070;

// SYNTHETIC: GAMEMD 0x0041E1D0 SYMBOL
// _$E113
double DAT_0088A050 = DAT_007E1720 * DAT_007E2A10;

// SYNTHETIC: GAMEMD 0x0041E1F0 SYMBOL
// _$E116
double DAT_0088A028 = DAT_007E1728 * DAT_007E2A10;

// SYNTHETIC: GAMEMD 0x0041E210 SYMBOL
// _$E119
double DAT_0088A000 = DAT_007E1730 * DAT_007E2A10;

// SYNTHETIC: GAMEMD 0x0041E230 SYMBOL
// _$E122
int DAT_0088A078 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0088A000 - DAT_0088A028) * DAT_0088A020));

// SYNTHETIC: GAMEMD 0x0041E270 SYMBOL
// _$E125
double DAT_0088A030 = FUN_004CADE0(DAT_0088A078 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0041E2A0 SYMBOL
// _$E128
double DAT_0088A048 = FUN_004CADE0((DAT_0088A078 * 2) / DAT_0088A020);

// SYNTHETIC: GAMEMD 0x0041E2D0 SYMBOL
// _$E131
int DAT_0088A06C = (int)(DAT_0088A078 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0041E300 SYMBOL
// _$E134
ShortPair DAT_0088A018;

// SYNTHETIC: GAMEMD 0x0041E310 SYMBOL
// _$E137
IntTriple DAT_0088A038;

struct IntTriple2 {
	int A, B, C;
	IntTriple2() { C = 0; A = 128; B = 128; }
};

// SYNTHETIC: GAMEMD 0x0041E330 SYMBOL
// _$E140
IntTriple2 DAT_0088A060;

// FUNCTION: GAMEMD 0x0041E350
AITriggerTypeClass::AITriggerTypeClass(const char *name) :
	BASECLASS(name),
	Type(AIT_NONE),
	House(-1),
	CurWeight(1),
	MinWeight(1),
	MaxWeight(1),
	IsEnabledInEasy(true),
	IsEnabledInMedium(true),
	IsEnabledInHard(true),
	Scope(SCOPE_LOCAL),
	TrigHouse(AITRIG_HOUSE_NONE),
	IsEnabled(false),
	MultiSide(0),
	TechLevelNeeded(0),
	IsAvailableInSkirmish(false),
	IsForBaseDefense(false),
	ConditionObject(NULL),
	TeamTypeOne(NULL),
	TeamTypeTwo(NULL),
	TimesSucceded(0),
	TimesExecuted(0)
{
	memset(Params.Data, 0, sizeof(Params.Data));

	AITriggerTypes.Add(this);
}

// FUNCTION: GAMEMD 0x0041E480
AITriggerTypeClass::~AITriggerTypeClass(void)
{
	AITriggerTypes.Delete(this);
}

// FUNCTION: GAMEMD 0x0041E500
HRESULT STDMETHODCALLTYPE AITriggerTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AITriggerTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0041E540
HRESULT STDMETHODCALLTYPE AITriggerTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) AITriggerTypeClass(NoInitClass());

		Swizzle_Pointer(&ConditionObject);
		Swizzle_Pointer(&TeamTypeOne);
		Swizzle_Pointer(&TeamTypeTwo);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0041E5C0
HRESULT STDMETHODCALLTYPE AITriggerTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0041E5E0
void AITriggerTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(Type);
	crc(Scope);
	crc(TrigHouse);
	crc(House);
	crc(TechLevelNeeded);

	switch (Type) {
		case AIT_ENEMY_OWNS_X_COND_N:
		case AIT_HOUSE_OWNS_X_COND_N:
		case AIT_ENEMY_MONEY_COND_N:
		case 7:
			crc(Params.Number);
			crc(Params.Condition);
			break;
		case 2:
		case 3:
		case 5:
		case 6:
			goto common;
	}
common:

	crc(CurWeight);
	crc(MinWeight);
	crc(MaxWeight);
	crc(IsAvailableInSkirmish);
	crc(IsEnabledInEasy);
	crc(IsEnabledInMedium);
	crc(IsEnabledInHard);
	crc(MultiSide);
	crc(IsForBaseDefense);
}

// FUNCTION: GAMEMD 0x0041E720
bool AITriggerTypeClass::ConditionMet(HouseClass *house, HouseClass *enemy, bool skip_base_defense) const
{
	bool basedefense;
	if ((TeamTypeOne != NULL && TeamTypeOne->IsBaseDefense) || (TeamTypeTwo != NULL && TeamTypeTwo->IsBaseDefense)) {
		basedefense = true;
	} else {
		basedefense = false;
	}

	if (TeamTypeOne == NULL) {
		return(false);
	}

	if (enemy == NULL || (Rule->UseMinDefenseRule && house->BaseDefenseTeamCount < Rule->MinimumAIDefensiveTeams[house->Difficulty])) {
		if (!basedefense) {
			return(false);
		}
	}

	if (basedefense && skip_base_defense) {
		return(false);
	}

	if (Scope == SCOPE_GLOBAL && Scen->IsIgnoreGlobalAITriggers == true) {
		return(false);
	}

	if (!IsEnabled) {
		return(false);
	}

	if (Session.Type != GAME_NORMAL && !IsAvailableInSkirmish) {
		return(false);
	}

	int diff;
	if (Session.Type == GAME_NORMAL) {

		diff = Scen->Difficulty;
		if (diff == DIFF_EASY) {
			if (!IsEnabledInEasy) {
				return(false);
			}
		} else if (diff == DIFF_NORMAL) {
			if (!IsEnabledInMedium) {
				return(false);
			}
		} else if (diff == DIFF_HARD) {
			if (!IsEnabledInHard) {
				return(false);
			}
		}

	} else {

		diff = house->Difficulty;
		if (diff == DIFF_EASY) {
			if (!IsEnabledInHard) {
				return(false);
			}
		} else if (diff == DIFF_NORMAL) {
			if (!IsEnabledInMedium) {
				return(false);
			}
		} else if (diff == DIFF_HARD) {
			if (!IsEnabledInEasy) {
				return(false);
			}
		}
	}

	if (TrigHouse == AITRIG_HOUSE_NONE) {
		return(false);
	}
	if (TrigHouse != AITRIG_HOUSE_ALL && TrigHouse == AITRIG_HOUSE_INDEX && house->Class->House != House) {
		return(false);
	}

	if (MultiSide == 1) {
		if (house->ActLike != HOUSE_GOOD) {
			return(false);
		}
	} else if (MultiSide == 2) {
		if (house->ActLike != HOUSE_BAD) {
			return(false);
		}
	} else if (MultiSide == 3) {
		if (house->ActLike != (HousesType)2) {
			return(false);
		}
	}

	if (TechLevelNeeded > house->Control.TechLevel) {
		return(false);
	}

	bool res = false;
	if (enemy != NULL) {
		switch (Type) {
			case AIT_NONE:
				res = true;
				break;
			case AIT_ENEMY_OWNS_X_COND_N:
				res = EnemyHouseOwns(house, enemy);
				break;
			case AIT_HOUSE_OWNS_X_COND_N:
				res = OwnerHouseOwns(house, enemy);
				break;
			case AIT_POWER_YELLOW:
				res = Check_Enemy_Yellow_Power(house, enemy);
				break;
			case AIT_POWER_RED:
				res = Check_Enemy_Red_Power(house, enemy);
				break;
			case AIT_ENEMY_MONEY_COND_N:
				res = HouseCredits(house, enemy);
				break;
			case 5:
				res = IronCurtainCharged(house, enemy);
				break;
			case 6:
				res = ChronoSphereCharged(house, enemy);
				break;
			case 7:
				res = CivilianHouseOwns(house, enemy);
				break;
		}
	} else {
		if (Type == AIT_HOUSE_OWNS_X_COND_N) {
			res = OwnerHouseOwns(house, enemy);
		} else {
			res = Type == AIT_NONE;
		}
	}

	if (!res) {
		return(false);
	}

	if (!FUN_0041FEE0(TeamTypeOne, house, enemy)) {
		return(false);
	}
	if (TeamTypeTwo != NULL && !FUN_0041FEE0(TeamTypeTwo, house, enemy)) {
		return(false);
	}

	if (!house->Can_Create_Team(TeamTypeOne)) {
		return(false);
	}
	if (TeamTypeTwo != NULL && !house->Can_Create_Team(TeamTypeTwo)) {
		return(false);
	}

	if (TeamTypeOne && TeamTypeOne->MaxAllowed >= 0 && house->Owned_Team_Count(TeamTypeOne) >= TeamTypeOne->MaxAllowed) {
		return(false);
	}
	if (TeamTypeTwo && TeamTypeTwo->MaxAllowed >= 0 && house->Owned_Team_Count(TeamTypeTwo) >= TeamTypeTwo->MaxAllowed) {
		return(false);
	}

	return(true);
}

// FUNCTION: GAMEMD 0x0041EAF0
bool AITriggerTypeClass::EnemyHouseOwns(HouseClass *house, HouseClass *enemy) const
{
	bool res = false;

	if (enemy != NULL) {
		int count;
		TechnoTypeClass *obj = ConditionObject;

		if (obj != NULL) {
			int id = obj->Fetch_Heap_ID();
			int hcount = 0;
			RTTIType rtti = obj->RTTI;
			switch (rtti) {
				case RTTI_BUILDINGTYPE:
					hcount = enemy->ABQuantity.Value(id);
					break;

				case RTTI_UNITTYPE:
					hcount = enemy->AUQuantity.Value(id);
					break;

				case RTTI_INFANTRYTYPE:
					hcount = enemy->AIQuantity.Value(id);
					break;

				case RTTI_AIRCRAFTTYPE:
					hcount = enemy->AAQuantity.Value(id);
					break;
			}
			count = hcount;
		} else {
			count = 0;
		}

		switch (Params.Condition) {
			case COND_LT:
				res = count < Params.Number;
				break;

			case COND_LE:
				res = count <= Params.Number;
				break;

			case COND_EQ:
				res = count == Params.Number;
				break;

			case COND_GE:
				res = count >= Params.Number;
				break;

			case COND_GT:
				res = count > Params.Number;
				break;

			case COND_NE:
				res = count != Params.Number;
				break;
		}
	}

	return(res);
}

// FUNCTION: GAMEMD 0x0041EC90
bool AITriggerTypeClass::CivilianHouseOwns(HouseClass *house, HouseClass *enemy) const
{
	bool res = false;

	for (int i = 0; i < Houses.Count(); i++) {
		if (Houses[i]->Class->Side == FUN_006A46D0("Civilian")) {
			HouseClass *civ = Houses[i];
			if (civ != NULL) {
				int count;
				TechnoTypeClass *obj = ConditionObject;

				if (obj != NULL) {
					int id = obj->Fetch_Heap_ID();
					int hcount = 0;
					RTTIType rtti = obj->RTTI;
					switch (rtti) {
						case RTTI_BUILDINGTYPE:
							hcount = civ->ABQuantity.Value(id);
							break;

						case RTTI_UNITTYPE:
							hcount = civ->AUQuantity.Value(id);
							break;

						case RTTI_INFANTRYTYPE:
							hcount = civ->AIQuantity.Value(id);
							break;

						case RTTI_AIRCRAFTTYPE:
							hcount = civ->AAQuantity.Value(id);
							break;
					}
					count = hcount;
				} else {
					count = 0;
				}

				switch (Params.Condition) {
					case COND_LT:
						res = count < Params.Number;
						break;

					case COND_LE:
						res = count <= Params.Number;
						break;

					case COND_EQ:
						res = count == Params.Number;
						break;

					case COND_GE:
						res = count >= Params.Number;
						break;

					case COND_GT:
						res = count > Params.Number;
						break;

					case COND_NE:
						res = count != Params.Number;
						break;
				}
			}
			break;
		}
	}

	return(res);
}

// FUNCTION: GAMEMD 0x0041EE90
bool AITriggerTypeClass::OwnerHouseOwns(HouseClass *house, HouseClass *enemy) const
{
	bool res = false;

	if (house != NULL) {
		int count;
		TechnoTypeClass *obj = ConditionObject;

		if (obj != NULL) {
			int id = obj->Fetch_Heap_ID();
			int hcount = 0;
			RTTIType rtti = obj->RTTI;
			switch (rtti) {
				case RTTI_BUILDINGTYPE:
					hcount = house->ABQuantity.Value(id);
					break;

				case RTTI_UNITTYPE:
					hcount = house->AUQuantity.Value(id);
					break;

				case RTTI_INFANTRYTYPE:
					hcount = house->AIQuantity.Value(id);
					break;

				case RTTI_AIRCRAFTTYPE:
					hcount = house->AAQuantity.Value(id);
					break;
			}
			count = hcount;
		} else {
			count = 0;
		}

		switch (Params.Condition) {
			case COND_LT:
				res = count < Params.Number;
				break;

			case COND_LE:
				res = count <= Params.Number;
				break;

			case COND_EQ:
				res = count == Params.Number;
				break;

			case COND_GE:
				res = count >= Params.Number;
				break;

			case COND_GT:
				res = count > Params.Number;
				break;

			case COND_NE:
				res = count != Params.Number;
				break;
		}
	}

	return(res);
}

// FUNCTION: GAMEMD 0x0041F030
bool AITriggerTypeClass::Check_Enemy_Yellow_Power(HouseClass *house, HouseClass *enemy) const
{
	bool res = false;

	if (enemy != NULL) {
		res = enemy->Power_Output() - enemy->Power_Drain() < 100.0;
	}
	return(res);
}

// FUNCTION: GAMEMD 0x0041F080
bool AITriggerTypeClass::Check_Enemy_Red_Power(HouseClass *house, HouseClass *enemy) const
{
	bool res = false;

	if (enemy != NULL) {
		res = enemy->Power_Output() - enemy->Power_Drain() < 0.0;
	}
	return(res);
}

// FUNCTION: GAMEMD 0x0041F0D0
bool AITriggerTypeClass::IronCurtainCharged(HouseClass *house, HouseClass *enemy) const
{
	if (house != NULL) {
		for (int i = 0; i < house->SuperWeapon.Count(); i++) {
			if (house->SuperWeapon[i]->Class->Type == SUPER_IRON_CURTAIN) {
				SuperClass *super = house->SuperWeapon[i];
				if (super != NULL && super->IsPresent) {
					int remaining = super->Control.Value();
					double total = super->GetRechargeTime();
					if (remaining / total <= 1.0f - Rule->AIMinorSuperReadyPercent) {
						return(true);
					}
				}
				break;
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0041F180
bool AITriggerTypeClass::ChronoSphereCharged(HouseClass *house, HouseClass *enemy) const
{
	if (house != NULL) {
		for (int i = 0; i < house->SuperWeapon.Count(); i++) {
			if (house->SuperWeapon[i]->Class->Type == SUPER_CHRONO_SPHERE) {
				SuperClass *super = house->SuperWeapon[i];
				if (super != NULL && super->IsPresent) {
					int remaining = super->Control.Value();
					double total = super->GetRechargeTime();
					if (remaining / total <= 1.0f - Rule->AIMinorSuperReadyPercent) {
						return(true);
					}
				}
				break;
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0041F230
bool AITriggerTypeClass::HouseCredits(HouseClass *house, HouseClass *enemy) const
{
	bool res = false;

	if (enemy != NULL) {
		int money = enemy->Available_Money();
		switch (Params.Condition) {
			case COND_LT:
				res = money < Params.Number;
				break;

			case COND_LE:
				res = money <= Params.Number;
				break;

			case COND_EQ:
				res = money == Params.Number;
				break;

			case COND_GE:
				res = money >= Params.Number;
				break;

			case COND_GT:
				res = money > Params.Number;
				break;

			case COND_NE:
				res = money != Params.Number;
				break;
		}
	}
	return(res);
}

// FUNCTION: GAMEMD 0x0041F2E0
void AITriggerTypeClass::Read_All(CCINIClass const & ini, INIScopeType scope)
{
	int i;
	int count;

	count = ini.Entry_Count(INI_NAME);

	for (i = 0; i < count; i++) {
		const char *entry = ini.Get_Entry(INI_NAME, i);

		AITriggerTypeClass *ptr = TFind_Or_Make<AITriggerTypeClass>(entry, AITriggerTypes);
		if (ptr != NULL) {
			ptr->Read_INI((CCINIClass &)ini);
			ptr->Scope = scope;
		}

		if (scope == SCOPE_GLOBAL) {
			ptr->IsEnabled = true;
		}
	}

	if (scope == SCOPE_LOCAL) {
		count = ini.Entry_Count(INI_NAME_ENABLE);
		for (i = 0; i < count; i++) {
			const char *entry = ini.Get_Entry(INI_NAME_ENABLE, i);

			for (int k = 0; k < AITriggerTypes.Count(); k++) {
				if (strcmpi((const char *)AITriggerTypes[k]->IniName, entry) == 0) {
					AITriggerTypeClass *ptr = AITriggerTypes[k];
					if (ptr != NULL) {
						if (ini.Get_Bool(INI_NAME_ENABLE, entry) || Session.Type != GAME_NORMAL) {
							ptr->IsEnabled = true;
						} else {
							ptr->IsEnabled = false;
						}
					}
					break;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0041F490
void AITriggerTypeClass::Write_All(CCINIClass & ini, INIScopeType scope)
{
	int i;
	char buf[32];

	int count = ini.Entry_Count(INI_NAME);

	for (i = 0; i < count; i++) {
		const char *entry = ini.Get_Entry(INI_NAME, i);
		ini.Get_String(INI_NAME, entry, "", buf, sizeof(buf));
		ini.Clear(buf);
	}

	ini.Clear(INI_NAME);

	for (i = 0; i < AITriggerTypes.Count(); i++) {
		AITriggerTypeClass *ptr = AITriggerTypes[i];
		if (ptr->Scope == scope) {
			ptr->Write_INI(ini);
		}
	}

	if (scope == SCOPE_LOCAL) {
		ini.Clear(INI_NAME_ENABLE);
		for (i = 0; i < AITriggerTypes.Count(); i++) {
			ini.Put_Bool(INI_NAME_ENABLE, (const char *)AITriggerTypes[i]->IniName, AITriggerTypes[i]->IsEnabled);
		}
	}
}

// FUNCTION: GAMEMD 0x0041F580
bool AITriggerTypeClass::Read_INI(CCINIClass & ini)
{
	unsigned int i;
	char *paramstr;
	char *tok;
	char objname[24];
	char teamname[24];
	char buf[INIClass::MAX_LINE_LENGTH];

	ini.Reset();
	if (ini.Get_String(INI_NAME, (const char *)IniName, "", buf, sizeof(buf))) {

		tok = strtok(buf, ",");
		if (tok == NULL) {
			return(false);
		}

		GivenName = TStringID<48>(tok);

		tok = strtok(NULL, ",");
		if (tok == NULL) {
			return(false);
		}
		strncpy(objname, tok, sizeof(objname));
		objname[sizeof(objname)-1] = 0;
		strtrim(objname);
		TeamTypeOne = NULL;
		if (strcmpi(objname, DAT_00818114) != 0) {
			TeamTypeOne = TeamTypeClass::From_Name(objname);
		}

		tok = strtok(NULL, ",");
		if (tok == NULL) {
			return(false);
		}
		strncpy(objname, tok, sizeof(objname));
		objname[sizeof(objname)-1] = 0;
		strtrim(objname);
		TrigHouse = AITRIG_HOUSE_NONE;
		House = HOUSE_NONE;

		if (strcmpi(objname, DAT_00818118) == 0) {
			TrigHouse = AITRIG_HOUSE_ALL;
		} else if (strcmpi(objname, DAT_00818114) != 0) {
			House = HouseTypeClass::From_Name(objname);
			if (House != HOUSE_NONE) {
				TrigHouse = AITRIG_HOUSE_INDEX;
			}
		}

		tok = strtok(NULL, ",");
		if (tok == NULL) {
			return(false);
		}

		TechLevelNeeded = 0;
		tok = strtok(NULL, ",");
		if (tok == NULL) {
			return(false);
		}

		Type = (AITriggerEnum)atoi(tok);
		tok = strtok(NULL, ",");
		if (tok == NULL) {
			return(false);
		}

		strncpy(objname, tok, sizeof(objname));
		objname[sizeof(objname)-1] = 0;
		strtrim(objname);

		int type = -1;
		TechnoTypeClass * conobj = NULL;

		if (type == -1) {
			type = (int)InfantryTypeClass::From_Name(objname);
			if (type != -1) {
				conobj = InfantryTypes[type];
			}
		}

		if (type == -1) {
			type = (int)UnitTypeClass::From_Name(objname);
			if (type != -1) {
				conobj = UnitTypes[type];
			}
		}

		if (type == -1) {
			type = (int)AircraftTypeClass::From_Name(objname);
			if (type != -1) {
				conobj = AircraftTypes[type];
			}
		}

		if (type == -1) {
			type = (int)BuildingTypeClass::From_Name(objname);
			if (type != -1) {
				conobj = BuildingTypes[type];
			}
		}

		ConditionObject = conobj;

		tok = strtok(NULL, ",");
		if (tok == NULL) {
			return(false);
		}
		paramstr = tok;
		{
			char tmp;
			char *endptr;
			char nptr[] = "00";
			endptr = 0;
			i = 0;
			while (*paramstr != '\0') {
				while (isspace(*paramstr)) {
					paramstr++;
				}
				nptr[0] = *paramstr;
				paramstr++;
				tmp = *paramstr;
				if (tmp) {
					nptr[1] = tmp;
					paramstr++;
				} else {
					nptr[1] = '\0';
				}
				if (i >= ARRAY_SIZE(Params.Data)) {
					break;
				}

				Params.Data[i] = strtol(nptr, &endptr, 16);
				i++;
			}
		}

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			CurWeight = (unsigned int)atof(tok);
		}

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			MinWeight = (unsigned int)atof(tok);
		}

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			MaxWeight = (unsigned int)atof(tok);
		}

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			IsAvailableInSkirmish = atoi(tok) != 0;
		}

		tok = strtok(NULL, ",");

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			MultiSide = atoi(tok);
		}

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			IsForBaseDefense = atoi(tok) != 0;
		}

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			strncpy(teamname, tok, sizeof(teamname));
			teamname[sizeof(teamname)-1] = 0;
			strtrim(teamname);
			TeamTypeTwo = NULL;
			if (strcmpi(teamname, DAT_00818114) != 0) {
				TeamTypeTwo = TeamTypeClass::From_Name(teamname);
			}
		}

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			IsEnabledInEasy = atoi(tok) != 0;
		}

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			IsEnabledInMedium = atoi(tok) != 0;
		}

		tok = strtok(NULL, ",");
		if (tok != NULL) {
			IsEnabledInHard = atoi(tok) != 0;
		}

		if (TeamTypeOne != NULL) {
			TechLevelNeeded = MAX(TechLevelNeeded, TeamTypeOne->TaskForce->Needed_Tech_Level());
		}

		if (TeamTypeTwo != NULL) {
			TechLevelNeeded = MAX(TechLevelNeeded, TeamTypeTwo->TaskForce->Needed_Tech_Level());
		}

		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0041FB10
bool AITriggerTypeClass::Write_INI(CCINIClass & ini) const
{
	char paramstr[INIClass::MAX_LINE_LENGTH];
	char buf[INIClass::MAX_LINE_LENGTH];

	const char *ininame1 = DAT_00818114;
	const char *ininame2 = DAT_00818114;
	const char *housname = DAT_00818114;
	const char *condname = DAT_00818114;

	if (TeamTypeOne != NULL) {
		ininame1 = (const char *)TeamTypeOne->IniName;
	}

	if (TeamTypeTwo != NULL) {
		ininame2 = (const char *)TeamTypeTwo->IniName;
	}

	switch (TrigHouse) {
		case AITRIG_HOUSE_INDEX:
			if (House != HOUSE_NONE) {
				housname = (const char *)HouseTypes[House]->IniName;
			}
			break;

		case AITRIG_HOUSE_ALL: {
			housname = DAT_00818118;
			break;
		}
	}

	if (ConditionObject != NULL) {
		condname = (const char *)ConditionObject->IniName;
	}

	unsigned int i = 0;
	char *ptr = paramstr;
	while (i < ARRAY_SIZE(Params.Data)) {
		sprintf(ptr, "%02x", (unsigned char)Params.Data[i]);
		i++;
		ptr += 2;
	}
	*ptr = 0;

	sprintf(
		buf,
		"%s,%s,%s,%d,%d,%s,%s,%lf,%lf,%lf,%d,%d,%d,%d,%s,%d,%d,%d",
		(const char *)GivenName,
		ininame1,
		housname,
		TechLevelNeeded,
		Type,
		condname,
		paramstr,
		CurWeight,
		MinWeight,
		MaxWeight,
		IsAvailableInSkirmish != 0,
		0,
		MultiSide,
		IsForBaseDefense != 0,
		ininame2,
		IsEnabledInEasy != 0,
		IsEnabledInMedium != 0,
		IsEnabledInHard != 0);

	return(ini.Put_String(INI_NAME, IniName, buf));
}

// FUNCTION: GAMEMD 0x0041FCA0
AITriggerTypeClass * AITriggerTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<AITriggerTypeClass>(name, AITriggerTypes));
}

// FUNCTION: GAMEMD 0x0041FD30
void AITriggerTypeClass::Set_Condition_Object(TechnoTypeClass *obj)
{
	ConditionObject = obj;
}

// FUNCTION: GAMEMD 0x0041FD40
void AITriggerTypeClass::Set_First_TeamType(TeamTypeClass *team)
{
	TeamTypeOne = team;
}

// FUNCTION: GAMEMD 0x0041FD50
void AITriggerTypeClass::Set_Second_TeamType(TeamTypeClass *team)
{
	TeamTypeTwo = team;
}

// FUNCTION: GAMEMD 0x0041FD60
void AITriggerTypeClass::Record_Success(void)
{
	double weight = 0;
	if (TimesExecuted > 0) {
		weight = (double)TimesExecuted * ((double)TimesSucceded / (double)TimesExecuted - 0.5);
		if (weight < 0.0) {
			weight = 0.0;
		}
	}
	CurWeight = CurWeight + weight + Rule->AITriggerSuccessWeightDelta;

	if (CurWeight < MinWeight) {
		CurWeight = MinWeight;
	}

	if (CurWeight > MaxWeight) {
		CurWeight = MaxWeight;
	}

	TimesSucceded++;
	TimesExecuted++;
}

// FUNCTION: GAMEMD 0x0041FE20
void AITriggerTypeClass::Record_Failure(void)
{
	double weight = 0;
	if (TimesExecuted > 0) {
		weight = (double)TimesExecuted * ((double)TimesSucceded / (double)TimesExecuted - 0.5) * Rule->AITriggerTrackRecordCoefficient;
		if (weight > 0.0) {
			weight = 0.0;
		}
	}
	CurWeight = CurWeight + weight + Rule->AITriggerFailureWeightDelta;

	if (CurWeight < MinWeight) {
		CurWeight = MinWeight;
	}

	if (CurWeight > MaxWeight) {
		CurWeight = MaxWeight;
	}

	TimesExecuted++;
}

// FUNCTION: GAMEMD 0x0041FEE0
bool AITriggerTypeClass::FUN_0041FEE0(TeamTypeClass *team, HouseClass *house, HouseClass *enemy) const
{
	if (!team->field_F0 || enemy == NULL) {
		return(true);
	}
	Cell a = house->Get_Base_Center();
	Cell b = enemy->Get_Base_Center();
	MZoneType mz = (MZoneType)team->field_EC;
	if (team->field_F1) {
		int zb = Map.Get_Cell_Zone(b, mz, false);
		int za = Map.Get_Cell_Zone(a, mz, false);
		if (za != zb) {
			zb = Map.Get_Cell_Zone(b, MZONE_AMPHIBIOUS, false);
			za = Map.Get_Cell_Zone(a, MZONE_AMPHIBIOUS, false);
			if (za == zb) {
				return(true);
			}
		}
		return(false);
	}
	int zb = Map.Get_Cell_Zone(b, mz, false);
	int za = Map.Get_Cell_Zone(a, mz, false);
	return((za == zb) ? true : false);
}

// FUNCTION: GAMEMD 0x0041FFD0
// AITriggerTypeClass::Fetch_RTTI

// FUNCTION: GAMEMD 0x0041FFE0
// AITriggerTypeClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x0041FFF0
// ??_GAITriggerTypeClass@@UAEPAXI@Z
