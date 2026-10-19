// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
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

// FUNCTION: 0x006E4D60 $E62
// FUNCTION: 0x006E4DA0 $E60
DynamicVectorClass<TagClass *> Tags;

// FUNCTION: 0x006E4DE0
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

// FUNCTION: 0x006E4F60
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

// FUNCTION: 0x006E5230
void TagClass::Mark_To_Delete(void)
{
	IsToDie = true;
	ObjectsToDelete.Add(this);
}

// FUNCTION: 0x006E5290
bool TagClass::Is_Marked_To_Delete(void) const
{
	return(IsToDie);
}

// FUNCTION: 0x006E52A0
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

// FUNCTION: 0x006E52F0
Cell TagClass::Get_Position(void) const
{
	return(CellID);
}

// FUNCTION: 0x006E5300
bool TagClass::Is_Cross_Vertical(void) const
{
	return((Class != NULL && Class->Is_Cross_Vertical()) ? 1 : 0);
}

// FUNCTION: 0x006E5320
bool TagClass::Is_Cross_Horizontal(void) const
{
	return((Class != NULL && Class->Is_Cross_Horizontal()) ? 1 : 0);
}

// FUNCTION: 0x006E5340
bool TagClass::Is_Enters_Zone(void) const
{
	return((Class != NULL && Class->Is_Enters_Zone()) ? 1 : 0);
}

// FUNCTION: 0x006E5360
bool TagClass::Is_Allow_Win(void) const
{
	return((Class != NULL && Class->Is_Allow_Win()) ? 1 : 0);
}

// FUNCTION: 0x006E5570
void TagClass::Init(void)
{
	while (Tags.Count() > 0) {
		delete Tags[0];
	}
}

// FUNCTION: 0x006E56A0
HRESULT STDMETHODCALLTYPE TagClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TagClass;
	return(S_OK);
}

// FUNCTION: 0x006E57A0
HRESULT STDMETHODCALLTYPE TagClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
