// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "tag.h"
#include "_map.h"
#include "cell.h"
#include "crc.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "noinit.h"
#include "sun.h"
#include "swizzle.h"
#include "tagtype.h"
#include "tracker.h"
#include "trigger.h"
#include "trigtype.h"
#include "vector.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x006E4B50 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006E4B80 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006E4BA0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006E4BC0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006E4BE0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006E4C00 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006E4C20 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006E4C60 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006E4C90 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006E4CC0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006E4CF0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006E4D00 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006E4D20 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006E4D40 SYMBOL
// _$E47

// SYNTHETIC: GAMEMD 0x006E4D60 SYMBOL
// _$E107

// SYNTHETIC: GAMEMD 0x006E4DA0 SYMBOL
// _$E105
DynamicVectorClass<TagClass *> Tags;

// FUNCTION: GAMEMD 0x006E4DE0
TagClass::TagClass(TagTypeClass *type) :
	BASECLASS(),
	Class(type),
	Trigger(NULL),
	AttachCount(0),
	CellID(CELL_NONE),
	IsToDie(false),
	IsCurrentlySprung(false)
{
	Tags.Add(this);
	AbstractTypePtrTracker.Add(this);
	TriggerPtrTracker.Add(this);

	if (type != NULL) {
		TriggerTypeClass *tt = type->FirstTrigger;
		while (tt != NULL) {

			TriggerClass *trigger = new TriggerClass(tt);

			trigger->LinkedTo = Trigger;
			Trigger = trigger;

			tt = tt->LinkedTo;
		}
	}
}

// FUNCTION: GAMEMD 0x006E4F60
TagClass::~TagClass(void)
{
	Detach_This_From_All(this, true);

	if (GameActive && ScenarioActive &&
		Class != NULL && (Class->Attaches_To() & ATTACH_GENERAL) != 0) {
		if (LogicTriggerID >= LogicTags.ID(this)) {
			LogicTriggerID--;
			if (LogicTriggerID < 0 && LogicTags.Count() == 0) {
				LogicTriggerID = 0;
			}
		}
	}

	if (GameActive && ScenarioActive &&
		Class != NULL && (Class->Attaches_To() & ATTACH_MAP) != 0) {
		if (MapTriggerID >= MapTags.ID(this)) {
			MapTriggerID--;
			if (MapTriggerID < 0 && MapTags.Count() == 0) {
				MapTriggerID = 0;
			}
		}
	}

	if (GameActive && ScenarioActive &&
		Trigger != NULL && Trigger->Class != NULL && Trigger->Class->House != NULL && Class->Is_Allow_Win()) {
		HouseClass *hptr = House_From_HousesType(Trigger->Class->House->House);

		if (hptr->Blockage) hptr->Blockage--;
		hptr->BorrowedTime = TICKS_PER_SECOND*4;
	}

	TriggerClass *trigger = Trigger;
	while (trigger != NULL) {

		ObjectsToDelete.Add(trigger);

		trigger = trigger->LinkedTo;
	}

	AbstractTypePtrTracker.Delete(this);
	TriggerPtrTracker.Delete(this);
	Tags.Delete(this);

	Detach_This_From_All(this, true);
}

// FUNCTION: GAMEMD 0x006E5230
void TagClass::Mark_To_Delete(void)
{
	IsToDie = true;
	ObjectsToDelete.Add(this);
}

// FUNCTION: GAMEMD 0x006E5290
bool TagClass::Is_Marked_To_Delete(void) const
{
	return(IsToDie);
}

// FUNCTION: GAMEMD 0x006E52A0
TagClass * Find_Or_Make(TagTypeClass *type)
{
	for (int index = 0; index < Tags.Count(); index++) {
		TagClass *tag = Tags[index];
		if (tag->Class == type) {
			return(tag);
		}
	}

	return(new TagClass(type));
}

// FUNCTION: GAMEMD 0x006E52F0
Cell TagClass::Get_Position(void) const
{
	return(CellID);
}

// FUNCTION: GAMEMD 0x006E5300
bool TagClass::Is_Cross_Vertical(void) const
{
	return((Class != NULL && Class->Is_Cross_Vertical()) ? 1 : 0);
}

// FUNCTION: GAMEMD 0x006E5320
bool TagClass::Is_Cross_Horizontal(void) const
{
	return((Class != NULL && Class->Is_Cross_Horizontal()) ? 1 : 0);
}

// FUNCTION: GAMEMD 0x006E5340
bool TagClass::Is_Enters_Zone(void) const
{
	return((Class != NULL && Class->Is_Enters_Zone()) ? 1 : 0);
}

// FUNCTION: GAMEMD 0x006E5360
bool TagClass::Is_Allow_Win(void) const
{
	return((Class != NULL && Class->Is_Allow_Win()) ? 1 : 0);
}

namespace Unsorted { extern bool ArmageddonMode; }

// FUNCTION: GAMEMD 0x006E5380
bool TagClass::Is_Trigger_Attached(TriggerClass *trigger) const
{
	TriggerClass * trigptr = Trigger;
	while (trigptr != NULL) {
		if (trigptr == trigger) {
			return(true);
		}
		trigptr = trigptr->LinkedTo;
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006E53A0
bool TagClass::Spring(TEventType event, ObjectClass * object, Cell cell, bool forced, TechnoClass *source)
{
	bool res = false; /// was handled
	bool die = false; /// to delete
	bool det = false; /// to detach

	if (Unsorted::ArmageddonMode || IsCurrentlySprung || IsToDie || Class == NULL) {
		return(false);
	}

	TriggerClass *trigger = Trigger;

	IsCurrentlySprung = true;

	while (trigger != NULL) {
		if (trigger->Should_Spring(event, object, forced, Class->Persistence == PERSISTENT, source)) {
			switch (Class->Persistence) {

				case VOLATILE:
					trigger->Spring(object, cell);
					trigger->Mark_To_Delete();
					res = true;
					die = true;
					det = true;
					break;

				case SEMIPERSISTENT:
					if (AttachCount == 1) {
						trigger->Spring(object, cell);
						trigger->Mark_To_Delete();
						die = true;
						res = true;
						break;
					}
					det = true;
					break;

				case PERSISTENT:
					trigger->Spring(object, cell);
					res = true;
					break;

				default:
					break;
			}
		}
		trigger = trigger->LinkedTo;
	}

	IsCurrentlySprung = false;

	if (det) {
		if (object != NULL && object->Tag == this) {
			object->Attach_Tag(NULL);
		}
		if (cell != CELL_NONE) {
			Map[cell].Attach_Tag(NULL);
		}
	}

	if (die) {
		Detach_This_From_All(this, true);
		ObjectsToDelete.Add(this);
	}

	return(res);
}

// FUNCTION: GAMEMD 0x006E5560
void TagClass::Set_Position(Cell cell)
{
	CellID = cell;
}

// FUNCTION: GAMEMD 0x006E5570
void TagClass::Init(void)
{
	while (Tags.Count() > 0) {
		delete Tags[0];
	}
}

// FUNCTION: GAMEMD 0x006E55A0
void TagClass::Timer_Global_Reset(int global)
{
	Trigger->Reset_Global_Linked_Timed_Events(global);
}

// FUNCTION: GAMEMD 0x006E55B0
void TagClass::Timer_Local_Reset(int local)
{
	Trigger->Reset_Local_Linked_Timed_Events(local);
}

// FUNCTION: GAMEMD 0x006E55C0
void TagClass::Link(TriggerClass *trigger)
{
	trigger->LinkedTo = Trigger;
	Trigger = trigger;
}

// FUNCTION: GAMEMD 0x006E55D0
bool TagClass::Unlink(TriggerClass *trigger)
{
	if (trigger != NULL) {
		if (Trigger == trigger) {
			Trigger = trigger->LinkedTo;
			return(true);
		}
		TriggerClass *trigptr = Trigger;
		while (trigptr != NULL) {
			if (trigptr->LinkedTo == trigger) {
				trigptr->LinkedTo = trigger->LinkedTo;
				return(true);
			}
			trigptr = trigptr->LinkedTo;
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006E5610
void TagClass::Detach(AbstractClass const * target, bool all)
{

	if (Class == target) {
		Class = NULL;
	}

	if (Trigger == target) {
		Trigger = Trigger->LinkedTo;
	}

	if (Class == NULL) {
		Mark_To_Delete();
	}
}

// FUNCTION: GAMEMD 0x006E56A0
HRESULT STDMETHODCALLTYPE TagClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TagClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006E56E0
void TagClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(Class->Fetch_ID());
	if (Trigger != NULL) crc(Trigger->Fetch_ID());

	crc(IsToDie);
}

// FUNCTION: GAMEMD 0x006E5730
HRESULT STDMETHODCALLTYPE TagClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TagClass(NoInitClass());

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&Trigger);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006E57A0
HRESULT STDMETHODCALLTYPE TagClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006E57C0
bool TagClass::Is_To_Inherit(void) const
{
	TriggerClass *trigger = Trigger;
	while (trigger != NULL) {
		if (trigger->Class->Is_To_Inherit()) {
			return(true);
		}
		trigger = trigger->LinkedTo;
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006E57F0
void TagClass::All_Timer_Global_Reset(int global)
{
	for (int index = 0; index < Tags.Count(); index++) {
		Tags[index]->Timer_Global_Reset(global);
	}
}

// FUNCTION: GAMEMD 0x006E5820
void TagClass::All_Timer_Local_Reset(int local)
{
	for (int index = 0; index < Tags.Count(); index++) {
		Tags[index]->Timer_Local_Reset(local);
	}
}

// FUNCTION: GAMEMD 0x006E5850
bool TagClass::Is_One_Of_A_Kind(void)
{
	for (int i = 0; i < Tags.Count(); i++) {
		TagClass *tag = Tags[i];
		if (tag != this && tag->Class == Class) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x006E5890
// TagClass::Fetch_Object_Size
// FUNCTION: GAMEMD 0x006E58A0
// TagClass::Fetch_RTTI
// SYNTHETIC: GAMEMD 0x006E58B0
// ??_GTagClass@@UAEPAXI@Z
