// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x004C96A0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004C96D0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004C96F0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004C9710 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004C9730 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004C9750 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004C9770 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004C97B0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004C97E0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004C9810 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004C9840 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004C9850 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004C9870 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004C9890 SYMBOL
// _$E47

// FUNCTION: GAMEMD 0x004C98B0
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

// FUNCTION: GAMEMD 0x004C9A10
FactoryClass::~FactoryClass(void)
{
	Detach_This_From_All(this, true);
	Factories.Delete(this);
	if (GameActive) {
		Abandon();
	}
	ObjectPtrTracker.Delete(this);
}

// FUNCTION: GAMEMD 0x004C9B20
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

// FUNCTION: GAMEMD 0x004C9C50
bool FactoryClass::Is_On_Hold(void) const
{
	return(OnHold);
}

// FUNCTION: GAMEMD 0x004C9C60
bool FactoryClass::Has_Changed(void)
{
	bool changed = IsDifferent;
	IsDifferent = false;
	return(changed);
}

// FUNCTION: GAMEMD 0x004C9C70
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

// FUNCTION: GAMEMD 0x004C9E10
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

// FUNCTION: GAMEMD 0x004C9E60
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

// FUNCTION: GAMEMD 0x004C9EA0
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

// FUNCTION: GAMEMD 0x004C9FB0
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

// FUNCTION: GAMEMD 0x004C9FF0
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

// FUNCTION: GAMEMD 0x004CA120
int FactoryClass::Completion(void)
{
	return(Production.Fetch_Stage());
}

// FUNCTION: GAMEMD 0x004CA130
bool FactoryClass::Has_Completed(void)
{
	if (Object && Production.Fetch_Stage() == STEP_COUNT) {
		return(true);
	}
	if (SpecialItem != SUPER_NONE && Production.Fetch_Stage() == STEP_COUNT) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004CA160
TechnoClass * FactoryClass::Get_Object(void) const
{
	return(Object);
}

// FUNCTION: GAMEMD 0x004CA170
int FactoryClass::Get_Special_Item(void) const
{
	return(SpecialItem);
}

// FUNCTION: GAMEMD 0x004CA180
int FactoryClass::Cost_Per_Tick(void)
{
	if (Object) {
		int steps = STEP_COUNT - Production.Fetch_Stage();
		if (steps) {
			return(Balance / steps);
		}
		return(Balance);
	}
	return(0);
}

// FUNCTION: GAMEMD 0x004CA1A0
bool FactoryClass::Completed(void)
{
	if (Object && Production.Fetch_Stage() == STEP_COUNT) {
		Object = NULL;
		IsSuspended = true;
		IsDifferent = true;
		Production.Set_Stage(0);
		Production.Set_Rate(0);
		return(true);
	}
	if (SpecialItem && Production.Fetch_Stage() == STEP_COUNT) {
		SpecialItem = SUPER_NONE;
		IsSuspended = true;
		IsDifferent = true;
		Production.Set_Stage(0);
		Production.Set_Rate(0);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004CA230
HRESULT STDMETHODCALLTYPE FactoryClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_FactoryClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004CA270
HRESULT STDMETHODCALLTYPE FactoryClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) FactoryClass(NoInitClass());

		int index;
		int count = 0;

		result = stream->Read(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (index = 0; index < count; index++) {
			TechnoTypeClass const * obj = NULL;
			result = stream->Read(&obj, sizeof(obj), NULL);
			if (FAILED(result)) {
				return(result);
			}
			QueuedObjects.Add(obj);
		}

		for (index = 0; index < count; index++) {
			Swizzle_Pointer(&QueuedObjects[index]);
		}

		Swizzle_Pointer(&House);
		Swizzle_Pointer(&Object);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004CA3C0
HRESULT STDMETHODCALLTYPE FactoryClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int index;

		int count = QueuedObjects.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (FAILED(result)) {
			return(result);
		}

		for (index = 0; index < count; index++) {
			result = stream->Write(&QueuedObjects[index], sizeof(QueuedObjects[index]), NULL);
			if (FAILED(result)) {
				return(S_OK);
			}
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004CA430
void FactoryClass::Compute_CRC(CRCEngine &crc) const
{
	DebugString("Frame == %d\n", Frame);
	DebugString("QueuedObjects.Count() == %d\n", QueuedObjects.Count());
	DebugString("Object->RTTI == %d\n", Object->RTTI);
	DebugString("Object->HeapID == %d\n", Object->Fetch_Heap_ID());
	DebugString("IsSuspended\t= %d\n", IsSuspended);
	DebugString("IsDifferent\t= %d\n", IsDifferent);
	DebugString("Balance\t= %d\n", Balance);
	DebugString("OriginalBalance = %d\n", OriginalBalance);
	DebugString("SpecialItem\t= %d\n", SpecialItem);
	DebugString("House->Fetch_ID()\t= %d\n", House->Fetch_ID());
	DebugString("ID\t= %d\n", Fetch_ID());

	BASECLASS::Compute_CRC(crc);

	crc(IsSuspended);
	crc(IsDifferent);
	crc(Balance);
	crc(OriginalBalance);

	if (Object != NULL) crc(Object->Fetch_ID());
	crc(SpecialItem);

	crc(House->Fetch_ID());
}

// FUNCTION: GAMEMD 0x004CA580
void FactoryClass::Detach(AbstractClass const * target, bool all)
{
	if (target == (AbstractClass *)Object) {
		Object = NULL;
	}
}

// FUNCTION: GAMEMD 0x004CA5A0
void FactoryClass::Resume_Queue(void)
{
	if (QueuedObjects.Count()) {
		if (Object == NULL && (!Production.Fetch_Rate() || IsSuspended)) {
			const TechnoTypeClass *object = QueuedObjects[0];
			QueuedObjects.Delete_Index(0);
			int id = object->Fetch_Heap_ID();
			if (id >= 0) {
				House->Begin_Production(object->Fetch_RTTI(), id, object->Naval, true);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x004CA620
bool FactoryClass::Remove_From_Queue(const TechnoTypeClass *type)
{
	return(QueuedObjects.Delete(type));
}

// FUNCTION: GAMEMD 0x004CA670
int FactoryClass::Total(const TechnoTypeClass *type)
{
	int total = 0;
	if (Object != NULL) {
		if (Object->TClass == type) {
			total++;
		}
	}

	for (int i = 0; i < QueuedObjects.Count(); i++) {
		if (QueuedObjects[i] == type) {
			total++;
		}
	}
	return(total);
}

// FUNCTION: GAMEMD 0x004CA6B0
bool FactoryClass::Is_Queued(const TechnoTypeClass *type)
{
	for (int i = 0; i < QueuedObjects.Count(); i++) {
		if (type == QueuedObjects[i]) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004CA6E0
void Recalc_House_Factories(HouseClass *house)
{
	for (int i = 0; i < Factories.Count(); i++) {
		FactoryClass *fptr = Factories[i];
		if (fptr->Get_House() == house) {
			int rate = fptr->Build_Rate();
			fptr->Production.Adjust_Rate(rate);
		}
	}
}

// FUNCTION: GAMEMD 0x004CA750
// FactoryClass::Fetch_RTTI

// FUNCTION: GAMEMD 0x004CA760
// FactoryClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x004CA770
// ??_GFactoryClass@@UAEPAXI@Z
