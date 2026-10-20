// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "trigtype.h"
#include "_map.h"
#include "_rules.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "sun.h"
#include "swizzle.h"
#include "taction.h"
#include "teamtype.h"
#include "tevent.h"
#include "tracker.h"
#include "persist.hh"
#include <cstdio>

static char const * PersistentName[3] = {
	"Volatile",
	"Semi-persistant",
	"Persistent"
};

// FUNCTION: 0x00726E00
TriggerTypeClass::~TriggerTypeClass(void)
{
	Detach_This_From_All(this, true);
	TriggerTypes.Delete(this);

	TActionClass * action = FirstAction;
	while (action != NULL) {
		TActionClass * next = action->Next;
		delete action;
		action = next;
	}

	TEventClass * event = FirstEvent;
	while (event != NULL) {
		TEventClass * next = event->Next;
		delete event;
		event = next;
	}

	AbstractTypePtrTracker.Delete(this);
	EventActionPtrTracker.Delete(this);
}

// FUNCTION: 0x00726F50
bool TriggerTypeClass::Is_Cross_Vertical(void) const
{
	bool found = false;
	TEventClass * tevent = FirstEvent;
	while (tevent != NULL) {
		if (tevent->Event == TEVENT_CROSS_VERTICAL) {
			found = true;
			break;
		}
		tevent = tevent->Next;
	}
	return(found);
}

// FUNCTION: 0x00726F80
bool TriggerTypeClass::Is_Cross_Horizontal(void) const
{
	bool found = false;
	TEventClass * tevent = FirstEvent;
	while (tevent != NULL) {
		if (tevent->Event == TEVENT_CROSS_HORIZONTAL) {
			found = true;
			break;
		}
		tevent = tevent->Next;
	}
	return(found);
}

// FUNCTION: 0x00726FB0
bool TriggerTypeClass::Is_Enters_Zone(void) const
{
	bool found = false;
	TEventClass * tevent = FirstEvent;
	while (tevent != NULL) {
		if (tevent->Event == TEVENT_ENTERS_ZONE) {
			found = true;
			break;
		}
		tevent = tevent->Next;
	}
	return(found);
}

// FUNCTION: 0x00726FE0
bool TriggerTypeClass::Is_Allow_Win(void) const
{
	bool found = false;
	TActionClass * taction = FirstAction;
	while (taction != NULL) {
		if (taction->Action == TACTION_ALLOWWIN) {
			found = true;
			break;
		}
		taction = taction->Next;
	}
	return(found);
}

// FUNCTION: 0x00727010
bool TriggerTypeClass::Is_Linked_To_Global(int global) const
{
	bool found = false;
	TEventClass * event = FirstEvent;
	while (event != NULL) {
		if ((event->Event == TEVENT_GLOBAL_SET || event->Event == TEVENT_GLOBAL_CLEAR) && event->Data.Value == global) {
			found = true;
			break;
		}
		event = event->Next;
	}
	return(found);
}

// FUNCTION: 0x00727050
bool TriggerTypeClass::Is_Linked_To_Local(int local) const
{
	bool found = false;
	TEventClass * event = FirstEvent;
	while (event != NULL) {
		if ((event->Event == TEVENT_LOCAL_SET || event->Event == TEVENT_LOCAL_CLEAR) && event->Data.Value == local) {
			found = true;
			break;
		}
		event = event->Next;
	}
	return(found);
}

// FUNCTION: 0x00727090
void TriggerTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (LinkedTo != NULL && LinkedTo == target) {
		LinkedTo = LinkedTo->LinkedTo;
	}

	TActionClass * taction = FirstAction;
	while (taction != NULL) {
		taction->Detach(target);
		taction = taction->Next;
	}
	if (FirstAction == target) {
		FirstAction = FirstAction->Next;
	}

	TEventClass * tevent = FirstEvent;
	while (tevent != NULL) {
		tevent->Detach(target);
		tevent = tevent->Next;
	}
	if (FirstEvent == target) {
		FirstEvent = FirstEvent->Next;
	}
}

// FUNCTION: 0x00727120
TriggerTypeClass * TriggerTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int index = 0; index < TriggerTypes.Count(); index++) {
			if (stricmp(TriggerTypes[index]->IniName, name) == 0 || stricmp(TriggerTypes[index]->GivenName, name) == 0) {
				return(TriggerTypes[index]);
			}
		}
	}
	return(NULL);
}


// FUNCTION: 0x00727190
PersistentType Persistence_From_Name(char const * name)
{
	for (int index = 0; index < ARRAY_SIZE(PersistentName); index++) {
		if (stricmp(name, PersistentName[index]) == 0) {
			return(PersistentType(index));
		}
	}
	return(VOLATILE);
}

// FUNCTION: 0x007271D0
char const * Name_From_Persistence(PersistentType pers)
{
	return(PersistentName[pers]);
}

// FUNCTION: 0x007271E0
AttachType TriggerTypeClass::Attaches_To(void) const
{
	AttachType attach = ATTACH_NONE;
	TEventClass * tevent = FirstEvent;
	while (tevent != NULL) {
		attach = AttachType(attach | ::Attaches_To(tevent->Event));
		tevent = tevent->Next;
	}
	TActionClass * taction = FirstAction;
	while (taction != NULL) {
		attach = AttachType(attach | ::Attaches_To(taction->Action));
		taction = taction->Next;
	}

	if (LinkedTo != NULL) {
		return(AttachType(attach | LinkedTo->Attaches_To()));
	}
	return(attach);
}

// FUNCTION: 0x00727240
bool TriggerTypeClass::Read_INI(CCINIClass & ini)
{
	char buffer[INIClass::MAX_LINE_LENGTH];

	ini.Reset();
	if (ini.Get_String(INI_NAME, IniName, "", buffer, sizeof(buffer))) {
		char * token = strtok(buffer, ",");
		if (stricmp(token, "<none>") == 0) {
			House = HouseTypes[HOUSE_FIRST];
		} else {
			House = HouseTypes[HouseTypeClass::From_Name(token)];
		}
		token = strtok(NULL, ",");
		LinkedTo = NULL;
		if (stricmp(token, "<none>") != 0) {
			LinkedTo = TFind_Or_Make<TriggerTypeClass>(token, TriggerTypes);
		}
		GivenName = strtok(NULL, ",");

		token = strtok(NULL, ",");
		if (token != NULL && atoi(token) == 0) {
			IsEnabled = true;
		} else {
			IsEnabled = false;
		}

		token = strtok(NULL, ",");
		if (token != NULL && atoi(token) != 0) {
			Difficulty.IsEnabledOnEasy = true;
		} else {
			Difficulty.IsEnabledOnEasy = false;
		}

		token = strtok(NULL, ",");
		if (token != NULL && atoi(token) != 0) {
			Difficulty.IsEnabledOnMedium = true;
		} else {
			Difficulty.IsEnabledOnMedium = false;
		}

		token = strtok(NULL, ",");
		if (token != NULL && atoi(token) != 0) {
			Difficulty.IsEnabledOnHard = true;
		} else {
			Difficulty.IsEnabledOnHard = false;
		}

		token = strtok(NULL, ",");
		if (token != NULL && atoi(token) != 0) {
			Set_To_Inherit(true);
		}

		if (ini.Get_String(INI_EVENT_NAME, IniName, "", buffer, sizeof(buffer))) {
			int count = atoi(strtok(buffer, ","));
			while (count) {
				TEventClass * tevent = new TEventClass;
				tevent->Read_INI();
				tevent->Next = FirstEvent;
				FirstEvent = tevent;
				count--;
			}
		}

		if (ini.Get_String(INI_ACTION_NAME, IniName, "", buffer, sizeof(buffer))) {
			int count = atoi(strtok(buffer, ","));
			FirstAction = NULL;
			TActionClass * last = NULL;

			while (count) {
				TActionClass * taction = new TActionClass;
				taction->Read_INI();
				if (FirstAction == NULL) {
					FirstAction = taction;
					last = FirstAction;
				} else {
					last->Next = taction;
					last = taction;
				}
				count--;
			}
		}

		return(true);
	}
	return(false);
}

// FUNCTION: 0x00727BB0
HRESULT STDMETHODCALLTYPE TriggerTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TriggerTypeClass;
	return(S_OK);
}

// FUNCTION: 0x00727C80
HRESULT STDMETHODCALLTYPE TriggerTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
