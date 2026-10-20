// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tagtype.h"
#include "ahtoi.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "noinit.h"
#include "sun.h"
#include "swizzle.h"
#include "tag.h"
#include "tracker.h"
#include "trigtype.h"
#include "vector.h"

// FUNCTION: 0x006E5AE0 $E16
// FUNCTION: 0x006E5B20 $E14
DynamicVectorClass<TagTypeClass *> TagTypes;

// FUNCTION: 0x006E5B60
TagTypeClass::TagTypeClass(char const *name) :
	BASECLASS(name),
	HeapID(-1),
	Persistence(VOLATILE),
	FirstTrigger(NULL)
{
	GivenName = TStringID<48>(IniName);
	HeapID = TagTypes.Count();
	TagTypes.Add(this);
	AbstractTypePtrTracker.Add(this);
}

// FUNCTION: 0x006E5CA0
TagTypeClass::~TagTypeClass(void)
{
	Detach_This_From_All(this, true);
	TriggerTypeClass *trigtype = FirstTrigger;
	FirstTrigger = NULL;
	while (trigtype != NULL) {
		TriggerTypeClass *linked = trigtype->LinkedTo;
		trigtype->LinkedTo = NULL;
		delete trigtype;
		trigtype = linked;
	}
	for (int i = 0; i < Tags.Count(); i++) {
		if (Tags[i]->Class == this) {
			delete Tags[i];
			i--;
		}
	}
	AbstractTypePtrTracker.Delete(this);
	TagTypes.Delete(this);
}

// FUNCTION: 0x006E5DD0
bool TagTypeClass::Link(TriggerTypeClass *trigger)
{
	if (trigger != NULL) {
		trigger->LinkedTo = FirstTrigger;
		FirstTrigger = trigger;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E5E00
bool TagTypeClass::Unlink(TriggerTypeClass *trigger)
{
	if (FirstTrigger == trigger) {
		FirstTrigger = trigger->LinkedTo;
		return(true);
	}
	TriggerTypeClass *trigtype = FirstTrigger;
	while (trigtype != NULL) {
		if (trigtype->LinkedTo == trigger) {
			trigtype->LinkedTo = trigger->LinkedTo;
			return(true);
		}
		trigtype = trigtype->LinkedTo;
	}
	return(false);
}

// FUNCTION: 0x006E5E50
void TagTypeClass::Detach(AbstractClass const * target, bool all)
{
	if (FirstTrigger == target) {
		FirstTrigger = FirstTrigger->LinkedTo;
	}
}

// FUNCTION: 0x006E5E70
TagTypeClass * TagTypeClass::From_Name(char const * name)
{
	if (name != NULL) {
		for (int index = 0; index < TagTypes.Count(); index++) {
			TagTypeClass *ttptr = TagTypes[index];
			if (stricmp(ttptr->IniName, name) == 0 || stricmp(ttptr->GivenName, name) == 0) {
				return(ttptr);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x006E5ED0
void TagTypeClass::Read_All(CCINIClass & ini)
{
	int len = ini.Entry_Count(INI_NAME);
	for (int index = 0; index < len; index++) {
		char const * entry = ini.Get_Entry(INI_NAME, index);
		char name[24];
		if (ini.Get_String(INI_NAME, entry, "", name, sizeof(name)) <= 0) {
			continue;
		}
		TagTypeClass * tag = TFind_Or_Make<TagTypeClass>(entry, TagTypes);
		Swizzler.Here_I_Am(ahtoi(entry), (void*)tag);
		tag->Read_INI(ini);
	}
}

// FUNCTION: 0x006E5FE0
void TagTypeClass::Write_All(CCINIClass & ini)
{
	int numtypes = ini.Entry_Count(INI_NAME);
	int index;
	for (index = 0; index < numtypes; index++) {
		char const * entry = ini.Get_Entry(INI_NAME, index);
		char buffer[32];
		ini.Get_String(INI_NAME, entry, "", buffer, sizeof(buffer));
		ini.Clear(buffer);
	}
	ini.Clear(INI_NAME);
	for (index = 0; index < TagTypes.Count(); index++) {
		TagTypes[index]->Write_INI(ini);
	}
}

// FUNCTION: 0x006E6080
bool TagTypeClass::Read_INI(CCINIClass & ini)
{
	char buffer[128];
	ini.Reset();
	if (ini.Get_String(INI_NAME, IniName, "", buffer, sizeof(buffer))) {
		char * token = strtok(buffer, ",");
		Persistence = (PersistentType)atoi(buffer);
		token = strtok(NULL, ",");
		GivenName = TStringID<48>(token);
		token = strtok(NULL, ",");
		FirstTrigger = TriggerTypeClass::Find_Or_Make(token);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E6160
bool TagTypeClass::Write_INI(CCINIClass & ini) const
{
	char buffer[128];
	if (FirstTrigger == NULL) {
		wsprintf(buffer, "%s,<none>", (char const *)GivenName);
		ini.Put_String(INI_NAME, IniName, buffer);
	} else {
		wsprintf(buffer, "%d,%s,%s", Persistence, (char const *)GivenName, (char const *)FirstTrigger->IniName);
		ini.Put_String(INI_NAME, IniName, buffer);
	}
	return(true);
}

// FUNCTION: 0x006E61F0
AttachType TagTypeClass::Attaches_To(void) const
{
	AttachType attach = ATTACH_NONE;
	TriggerTypeClass * trigger = FirstTrigger;
	while (trigger != NULL) {
		attach = AttachType(attach | trigger->Attaches_To());
		trigger = trigger->LinkedTo;
	}
	return(attach);
}

// FUNCTION: 0x006E6220
bool TagTypeClass::Is_Allow_Win(void) const
{
	TriggerTypeClass * trigger = FirstTrigger;
	while (trigger != NULL) {
		if (trigger->Is_Allow_Win()) {
			return(true);
		}
		trigger = trigger->LinkedTo;
	}
	return(false);
}

// FUNCTION: 0x006E6250
bool TagTypeClass::Is_Cross_Horizontal(void) const
{
	TriggerTypeClass * trigger = FirstTrigger;
	while (trigger != NULL) {
		if (trigger->Is_Cross_Horizontal()) {
			return(true);
		}
		trigger = trigger->LinkedTo;
	}
	return(false);
}

// FUNCTION: 0x006E6280
bool TagTypeClass::Is_Cross_Vertical(void) const
{
	TriggerTypeClass * trigger = FirstTrigger;
	while (trigger != NULL) {
		if (trigger->Is_Cross_Vertical()) {
			return(true);
		}
		trigger = trigger->LinkedTo;
	}
	return(false);
}

// FUNCTION: 0x006E62B0
bool TagTypeClass::Is_Enters_Zone(void) const
{
	TriggerTypeClass * trigger = FirstTrigger;
	while (trigger != NULL) {
		if (trigger->Is_Enters_Zone()) {
			return(true);
		}
		trigger = trigger->LinkedTo;
	}
	return(false);
}

// FUNCTION: 0x006E62E0
bool TagTypeClass::Is_Linked(TriggerTypeClass *trigger) const
{
	TriggerTypeClass * trigtype = FirstTrigger;
	while (trigtype != NULL) {
		if (trigtype == trigger) {
			return(true);
		}
		trigtype = trigtype->LinkedTo;
	}
	return(false);
}

// FUNCTION: 0x006E6310
TagTypeClass * TagTypeClass::Find_Or_Make(char const * name)
{
	return(TFind_Or_Make<TagTypeClass>(name, TagTypes));
}

// FUNCTION: 0x006E63A0
HRESULT STDMETHODCALLTYPE TagTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TagTypeClass;
	return(S_OK);
}

// FUNCTION: 0x006E63E0
void TagTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Persistence);
}

// FUNCTION: 0x006E6410
HRESULT STDMETHODCALLTYPE TagTypeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TagTypeClass(NoInitClass());
		Swizzle_Pointer(&FirstTrigger);
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006E6470
HRESULT STDMETHODCALLTYPE TagTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006E6490 TagTypeClass::Fetch_RTTI

// FUNCTION: 0x006E64A0 TagTypeClass::Fetch_Object_Size

// FUNCTION: 0x006E64B0 TagTypeClass::Fetch_Heap_ID

// FUNCTION: 0x006E64C0 TagTypeClass::~TagTypeClass

// FUNCTION: 0x006E64E0 ?$DynamicVectorClass@PAVTagTypeClass@@::Clear
template class DynamicVectorClass<TagTypeClass *>;

// FUNCTION: 0x006E6510 ?$DynamicVectorClass@PAVTagTypeClass@@::ID
// FUNCTION: 0x006E6530 ?$VectorClass@PAVTagTypeClass@@::GetItem
// FUNCTION: 0x006E6540 ?$VectorClass@PAVTagTypeClass@@::operator==
// FUNCTION: 0x006E6580 ?$VectorClass@PAVTagTypeClass@@::Resize
// FUNCTION: 0x006E6630 ?$VectorClass@PAVTagTypeClass@@::Clear
// FUNCTION: 0x006E6660 ?$VectorClass@PAVTagTypeClass@@::ID
// FUNCTION: 0x006E66A0 ?$VectorClass@PAVTagTypeClass@@::ID
// FUNCTION: 0x006E66C0 ?$DynamicVectorClass@PAVTagTypeClass@@::Resize
// FUNCTION: 0x006E6770 ?$DynamicVectorClass@PAVTagTypeClass@@::ID
// FUNCTION: 0x006E67A0 ?$VectorClass@PAVTagTypeClass@@::~?$VectorClass@PAVTagTypeClass@@
template class VectorClass<TagTypeClass *>;
