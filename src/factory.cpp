// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "factory.h"
#include "_rules.h"
#include "bound.h"
#include "building.h"
#include "crc.h"
#include "dbgprint.h"
#include "globals.h"
#include "house.h"
#include "rules.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "voc.h"
#include "super.hh"

// FUNCTION: 0x004C98B0
FactoryClass::FactoryClass(void) :
	BASECLASS(),
	Production(),
	QueuedObjects(),
	Object(0),
	OnHold(false),
	IsDifferent(false),
	Balance(0),
	OriginalBalance(0),
	SpecialItem(SUPER_NONE),
	House(0),
	IsSuspended(false),
	IsOnHold(true)
{
	Create_ID();
	Factories.Add(this);
	Production.Set_Rate(0);
	Production.Set_Stage(0);
	ObjectPtrTracker.Add(this);
}

// FUNCTION: 0x004C9A10
FactoryClass::~FactoryClass(void)
{
	Detach_This_From_All(this, true);
	Factories.Delete(this);
	if (GameActive) {
		Abandon();
	}
	ObjectPtrTracker.Delete(this);
}

// FUNCTION: 0x004C9B20
void FactoryClass::AI(void)
{
	if (!IsSuspended && (Object != NULL || SpecialItem)) {
		if (!Is_Finished() && Production.Graphic_Logic()) {
			IsDifferent = true;
			int cost = Cost_Per_Tick();
			cost = MIN(cost, Balance);
			if (cost > House->Available_Money()) {
				Production.Set_Stage(Production.Fetch_Stage() - 1);
				OnHold = true;
			} else {
				House->Spend_Money(cost);
				OnHold = false;
				Balance -= cost;
			}
			if (Production.Fetch_Stage() == STEP_COUNT) {
				IsSuspended = true;
				Production.Set_Rate(0);
				House->Spend_Money(Balance);
				Balance = 0;
			}
		}
	}
}

// FUNCTION: 0x004C9C50
bool FactoryClass::Is_On_Hold(void) const
{
	return(OnHold);
}

// FUNCTION: 0x004C9C60
bool FactoryClass::Has_Changed(void)
{
	bool changed = IsDifferent;
	IsDifferent = false;
	return(changed);
}

// FUNCTION: 0x004C9C70
bool FactoryClass::Set(TechnoTypeClass const & object, HouseClass & house, bool resume)
{
	if (object.Fetch_RTTI() == RTTI_BUILDINGTYPE) {
		Abandon();
	}
	if (object.Fetch_RTTI() != RTTI_BUILDINGTYPE && (Is_Building() || QueuedObjects.Count() > 0 || Is_Suspended()) && !resume) {
		if (QueuedObjects.Count() < Rule->MaximumQueuedObjects && !House->Is_Build_Limited(&object)) {
			QueuedObjects.Add(&object);
			return(true);
		}
		if (house.Is_Player_Control()) {
			VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
		}
		return(false);
	}
	IsDifferent = true;
	IsSuspended = true;
	Production.Set_Rate(0);
	Production.Set_Stage(0);
	Object = (TechnoClass *)object.Create_One_Of(&house);
	if (!house.Is_Human_Player() && Object != NULL && Object->Fetch_RTTI() == RTTI_BUILDING) {
		((BuildingClass *)Object)->IsToRebuild = true;
	}
	if (Object) {
		House = Object->House;
		Balance = object.Cost_Of(House);
		Object->PurchasePrice = Balance;
	}
	return(Object != NULL);
}

// FUNCTION: 0x004C9E10
void FactoryClass::Set(TechnoClass & object)
{
	Abandon();
	Object = &object;
	House = Object->House;
	Balance = 0;
	Production.Set_Rate(0);
	Production.Set_Stage(STEP_COUNT);
	IsDifferent = true;
	IsSuspended = true;
}

// FUNCTION: 0x004C9E60
bool FactoryClass::Suspend(bool onhold)
{
	if (!IsSuspended) {
		IsOnHold = onhold;
		IsSuspended = true;
		Production.Set_Rate(0);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004C9EA0
bool FactoryClass::Start(bool onhold)
{
	if ((Object || SpecialItem) && IsSuspended && !Is_Finished()) {
		IsSuspended = false;
		Production.Set_Rate(Build_Rate());
		if (House->Available_Money() >= Cost_Per_Tick()) {
			IsOnHold = true;
			if (onhold) {
				Suspend(true);
			}
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x004C9FB0
int FactoryClass::Build_Rate(void) const
{
	int time = 0;
	if (Object) {
		time = Object->Time_To_Build();
	}
	time /= STEP_COUNT;
	time = Bound(time, 1, 255);
	return(time);
}

// FUNCTION: 0x004C9FF0
bool FactoryClass::Abandon(void)
{
	if (Object) {
		DebugString("Abandoning production of %s\n", (char const *)Object->Class_Of()->GivenName);
		int money = Object->Class_Of()->Cost_Of(Object->House);
		House->Refund_Money(money - Balance);
		Balance = 0;
		if (SpecialItem) {
			SpecialItem = SUPER_NONE;
		}
		Production.Set_Rate(0);
		Production.Set_Stage(0);
		IsSuspended = true;
		IsDifferent = true;
		if (!House->Is_Human_Player()) {
			if (Object->Fetch_RTTI() == RTTI_INFANTRY) {
				House->BuildInfantry = INFANTRY_NONE;
			}
			if (Object->Fetch_RTTI() == RTTI_UNIT) {
				House->BuildUnit = UNIT_NONE;
			}
			if (Object->Fetch_RTTI() == RTTI_AIRCRAFT) {
				House->BuildAircraft = AIRCRAFT_NONE;
			}
			if (Object->Fetch_RTTI() == RTTI_BUILDING) {
				House->BuildStructure = STRUCT_NONE;
			}
		}
		ScenarioInit++;
		delete Object;
		Object = NULL;
		ScenarioInit--;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004CA120
int FactoryClass::Completion(void)
{
	return(Production.Fetch_Stage());
}

// FUNCTION: 0x004CA230
HRESULT STDMETHODCALLTYPE FactoryClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_FactoryClass;
	return(S_OK);
}
