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

#include <math.h>

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

// FUNCTION: 0x0041E160 $E65
double DAT_0088A020 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0041E190 $E68
double DAT_0088A070 = DAT_0088A020 / DAT_007E2A08;

// FUNCTION: 0x0041E1B0 $E71
double DAT_0088A058 = DAT_007E1718 / DAT_0088A070;

// FUNCTION: 0x0041E1D0 $E74
double DAT_0088A050 = DAT_007E1720 * DAT_007E2A10;

// FUNCTION: 0x0041E1F0 $E77
double DAT_0088A028 = DAT_007E1728 * DAT_007E2A10;

// FUNCTION: 0x0041E210 $E80
double DAT_0088A000 = DAT_007E1730 * DAT_007E2A10;

// FUNCTION: 0x0041E230 $E83
int DAT_0088A078 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0088A000 - DAT_0088A028) * DAT_0088A020));

// FUNCTION: 0x0041E270 $E86
double DAT_0088A030 = FUN_004CADE0(DAT_0088A078 * DAT_007E1740);

// FUNCTION: 0x0041E2A0 $E89
double DAT_0088A048 = FUN_004CADE0((DAT_0088A078 * 2) / DAT_0088A020);

// FUNCTION: 0x0041E2D0 $E92
int DAT_0088A06C = (int)(DAT_0088A078 * 4 + DAT_007E1738);

// FUNCTION: 0x0041E300 $E95
ShortPair DAT_0088A018;

// FUNCTION: 0x0041E310 $E98
IntTriple DAT_0088A038;

struct IntTriple2 {
	int A, B, C;
	IntTriple2() { C = 0; A = 128; B = 128; }
};

// FUNCTION: 0x0041E330 $E101
IntTriple2 DAT_0088A060;

// FUNCTION: 0x0041E350
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

// FUNCTION: 0x0041E480
AITriggerTypeClass::~AITriggerTypeClass(void)
{
	AITriggerTypes.Delete(this);
}

// FUNCTION: 0x0041E500
HRESULT STDMETHODCALLTYPE AITriggerTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AITriggerTypeClass;
	return(S_OK);
}

// FUNCTION: 0x0041E540
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

// FUNCTION: 0x0041E5C0
HRESULT STDMETHODCALLTYPE AITriggerTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0041E5E0
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

// FUNCTION: 0x0041E720
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

// FUNCTION: 0x0041EAF0
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

// FUNCTION: 0x0041EC90
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

// FUNCTION: 0x0041EE90
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

// FUNCTION: 0x0041F030
bool AITriggerTypeClass::Check_Enemy_Yellow_Power(HouseClass *house, HouseClass *enemy) const
{
	bool res = false;

	if (enemy != NULL) {
		res = enemy->Power_Output() - enemy->Power_Drain() < 100.0;
	}
	return(res);
}

// FUNCTION: 0x0041F080
bool AITriggerTypeClass::Check_Enemy_Red_Power(HouseClass *house, HouseClass *enemy) const
{
	bool res = false;

	if (enemy != NULL) {
		res = enemy->Power_Output() - enemy->Power_Drain() < 0.0;
	}
	return(res);
}

// FUNCTION: 0x0041F0D0
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
