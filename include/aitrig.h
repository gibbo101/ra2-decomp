// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"
#include "ccini.h"
#include "noinit.h"

enum AITriggerEnum {
	AIT_ENEMY_OWNS_X_COND_N = 0,
	AIT_HOUSE_OWNS_X_COND_N,
	AIT_POWER_YELLOW,
	AIT_POWER_RED,
	AIT_ENEMY_MONEY_COND_N,

	AIT_COUNT,
	AIT_NONE = -1,
};

enum ConditionEnum {
	COND_LT = 0,
	COND_LE,
	COND_EQ,
	COND_GE,
	COND_GT,
	COND_NE,
	COND_COUNT
};

enum AITriggerHouseType
{
	AITRIG_HOUSE_NONE,
	AITRIG_HOUSE_INDEX,
	AITRIG_HOUSE_ALL,
};

class HouseClass;
class TechnoTypeClass;
class TeamTypeClass;
class CCINIClass;
class CRCEngine;

class AITriggerTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		AITriggerTypeClass(const char *name = NULL);
		AITriggerTypeClass(NoInitClass const & x) : BASECLASS(x) {};
		~AITriggerTypeClass(void);

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static AITriggerTypeClass * Find_Or_Make(char const * ininame);

		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_AITRIGGERTYPE);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		virtual bool Read_INI(CCINIClass & ini) override;
		virtual bool Write_INI(CCINIClass & ini) const override;
		static void Read_All(CCINIClass const & ini, INIScopeType scope);
		static void Write_All(CCINIClass & ini, INIScopeType scope);

		bool ConditionMet(HouseClass *house, HouseClass *enemy, bool skip_base_defense) const;

		double Get_Current_Weight(void) const { return(CurWeight); }

		void Set_Condition_Object(TechnoTypeClass *obj);

		TeamTypeClass * Get_First_TeamType(void){return(TeamTypeOne);}
		TeamTypeClass * Get_Second_TeamType(void){return(TeamTypeTwo);}

		void Set_First_TeamType(TeamTypeClass *team);
		void Set_Second_TeamType(TeamTypeClass *team);

		void Record_Success(void);
		void Record_Failure(void);

	private:
		bool EnemyHouseOwns(HouseClass *house, HouseClass *enemy) const;
		bool OwnerHouseOwns(HouseClass *house, HouseClass *enemy) const;
		bool CivilianHouseOwns(HouseClass *house, HouseClass *enemy) const;
		bool Check_Enemy_Yellow_Power(HouseClass *house, HouseClass *enemy) const;
		bool Check_Enemy_Red_Power(HouseClass *house, HouseClass *enemy) const;
		bool IronCurtainCharged(HouseClass *house, HouseClass *enemy) const;
		bool ChronoSphereCharged(HouseClass *house, HouseClass *enemy) const;
		bool HouseCredits(HouseClass *house, HouseClass *enemy) const;
		bool FUN_0041FEE0(TeamTypeClass *team, HouseClass *house, HouseClass *enemy) const;

	private:

		AITriggerEnum Type;

		INIScopeType Scope;

		AITriggerHouseType TrigHouse;

		bool IsEnabled;

		int House;

		int MultiSide;

		int TechLevelNeeded;
		int unknown_B4;

		double CurWeight;

		double MinWeight;
		double MaxWeight;

		bool IsAvailableInSkirmish;

		bool IsForBaseDefense;

		bool IsEnabledInEasy;
		bool IsEnabledInMedium;
		bool IsEnabledInHard;

		TechnoTypeClass *ConditionObject;

		TeamTypeClass *TeamTypeOne;
		TeamTypeClass *TeamTypeTwo;

		union {
			struct {
				int Number;
				ConditionEnum Condition;
			};
			char Data[32];
		} Params;

		int TimesSucceded;
		int TimesExecuted;
		int unknown_10C;

		static char const * const INI_NAME;
		static char const * const INI_NAME_ENABLE;
};
