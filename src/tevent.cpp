// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tevent.h"
#include "building.h"
#include "builtype.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "team.h"
#include "teamtype.h"
#include "noinit.h"
#include "techno.h"
#include "tracker.h"
#include "vector.h"

// SYNTHETIC: GAMEMD 0x0071E620 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x0071E660 SYMBOL
// _$E38
DynamicVectorClass<TEventClass *> Events;

// FUNCTION: GAMEMD 0x0071E6A0
TEventClass::TEventClass(void) :
	HeapID(-1),
	Next(NULL),
	Event(TEVENT_NONE),
	Team(NULL),
	EventHouse(NULL)
{
	Events.Add(this);
	HeapID = Events.ID(this);

	EventActionPtrTracker.Add(this);
	AbstractTypePtrTracker.Add(this);

	Data.Value = 0;
}

// FUNCTION: GAMEMD 0x0071E800
TEventClass::TEventClass(NoInitClass const & x) :
	BASECLASS(x),
	String(x)
{
}

// FUNCTION: GAMEMD 0x0071E830
TEventClass::~TEventClass(void)
{
	Detach_This_From_All(this, true);
	EventActionPtrTracker.Delete(this);
	AbstractTypePtrTracker.Delete(this);
	Events.Delete(this);
}

// FUNCTION: GAMEMD 0x0071F390
void TEventClass::Build_INI_Entry(char * ptr) const
{
	int code = 0;
	int val = Data.Value;
	NeedType need = Event_Needs(Event);
	if (Team != NULL) {
		code = 1;
		ptr += strlen(ptr);
		wsprintf(ptr, "%d,%d,%s", Event, code, (char const *)Team->IniName);
	} else if (need == NeedType(0x2b)) {
		code = 2;
		ptr += strlen(ptr);
		wsprintf(ptr, "%d,%d,%d,%s", Event, code, val, (char const *)String);
	} else {
		ptr += strlen(ptr);
		wsprintf(ptr, "%d,%d,%d", Event, code, val);
	}
}

// FUNCTION: GAMEMD 0x0071F4E0
void TEventClass::Read_INI(void)
{
	Data.Value = 0;
	Event = TEventType(atoi(strtok(NULL, ",")));
	int code = atoi(strtok(NULL, ","));
	char * text = strtok(NULL, ",");
	int val = atoi(text);
	char * extra = NULL;
	if (code == 2) {
		extra = strtok(NULL, ",");
	}

	switch (code) {
		case 0:
			Data.Value = val;
			break;

		case 1:
			Team = TeamTypeClass::From_Name(text);
			break;

		case 2:
			Data.Value = val;
			String = TStringID<24>(extra);
			break;
	}
}

// FUNCTION: GAMEMD 0x0071F5B0
NeedType Event_Needs(TEventType event)
{
	switch (event) {
		case TEVENT_THIEVED:
		case TEVENT_PLAYER_ENTERED:
		case TEVENT_CROSS_HORIZONTAL:
		case TEVENT_CROSS_VERTICAL:
		case TEVENT_ENTERS_ZONE:
		case TEVENT_HOUSE_DISCOVERED:
		case TEVENT_BUILDINGS_DESTROYED:
		case TEVENT_UNITS_DESTROYED:
		case TEVENT_ALL_DESTROYED:
		case TEVENT_LOW_POWER:
		case TEVENT_ATTACKED_BY:
		case 53:
		case 55:
		case 56:
		case 58:
		case 59:
			return(NEED_HOUSE);

		case TEVENT_NUNITS_DESTROYED:
		case TEVENT_NBUILDINGS_DESTROYED:
		case TEVENT_CREDITS:
		case TEVENT_CREDITS_BELOW:
		case TEVENT_TIME:
		case TEVENT_RANDOM_TIME:
		case TEVENT_GAME_TIME:
		case TEVENT_AMBIENT_LESS_THAN:
		case TEVENT_AMBIENT_GREATER_THAN:
			return(NEED_NUMBER);

		case TEVENT_GLOBAL_SET:
		case TEVENT_GLOBAL_CLEAR:
			return(NEED_GLOBAL);

		case TEVENT_LOCAL_SET:
		case TEVENT_LOCAL_CLEAR:
			return(NEED_LOCAL);

		case TEVENT_BUILDING_EXISTS:
		case TEVENT_BUILD:
		case 57:
			return(NEED_STRUCTURE);

		case TEVENT_BUILD_UNIT:
			return(NEED_UNIT);

		case TEVENT_BUILD_INFANTRY:
		case 54:
			return(NEED_INFANTRY);

		case TEVENT_BUILD_AIRCRAFT:
			return(NEED_AIRCRAFT);

		case TEVENT_LEAVES_MAP:
			return(NEED_TEAM);

		case TEVENT_NEAR_WAYPOINT:
			return(NEED_WAYPOINT);

		case 60:
		case 61:
			return(NeedType(0x2b));

		default:
			break;
	}
	return(NEED_NONE);
}

// FUNCTION: GAMEMD 0x0071F680
AttachType Attaches_To(TEventType event)
{
	AttachType attach = ATTACH_NONE;

	switch (event) {
		case TEVENT_CROSS_HORIZONTAL:
		case TEVENT_CROSS_VERTICAL:
		case TEVENT_ENTERS_ZONE:
		case TEVENT_PLAYER_ENTERED:
		case TEVENT_ANY:
		case TEVENT_DISCOVERED:
		case TEVENT_NONE:
		case TEVENT_BRIDGE_DESTROYED:
		case 53:
		case 54:
		case 59:
			attach = AttachType(attach | ATTACH_CELL);
			break;

		default:
			break;
	}

	switch (event) {
		case TEVENT_FIRST_DAMAGED_ANY:
		case TEVENT_ENTER_YELLOW_ANY:
		case TEVENT_ENTER_RED_ANY:
		case TEVENT_FIRST_DAMAGED:
		case TEVENT_ENTER_YELLOW:
		case TEVENT_ENTER_RED:
		case TEVENT_SPIED:
		case TEVENT_PLAYER_ENTERED:
		case TEVENT_DISCOVERED:
		case TEVENT_DESTROYED:
		case TEVENT_DESTROYED_ANY:
		case TEVENT_DESTROYED_ANY_X:
		case TEVENT_ATTACKED:
		case TEVENT_ATTACKED_BY:
		case TEVENT_ANY:
		case TEVENT_NONE:
		case TEVENT_SELECTED:
		case TEVENT_NEAR_WAYPOINT:
		case TEVENT_ENEMY_IN_SPOTLIGHT:
		case TEVENT_PICKUP_CRATE:
			attach = AttachType(attach | ATTACH_OBJECT);
			break;

		default:
			break;
	}

	switch (event) {
		case TEVENT_ENTERS_ZONE:
		case TEVENT_ANY:
			attach = AttachType(attach | ATTACH_MAP);
			break;

		default:
			break;
	}

	switch (event) {
		case TEVENT_LOW_POWER:
		case TEVENT_EVAC_CIVILIAN:
		case TEVENT_BUILDING_EXISTS:
		case TEVENT_BUILD:
		case TEVENT_BUILD_UNIT:
		case TEVENT_BUILD_INFANTRY:
		case TEVENT_BUILD_AIRCRAFT:
		case TEVENT_NOFACTORIES:
		case TEVENT_BUILDINGS_DESTROYED:
		case TEVENT_NBUILDINGS_DESTROYED:
		case TEVENT_UNITS_DESTROYED:
		case TEVENT_NUNITS_DESTROYED:
		case TEVENT_ALL_DESTROYED:
		case TEVENT_HOUSE_DISCOVERED:
		case TEVENT_CREDITS:
		case TEVENT_CREDITS_BELOW:
		case TEVENT_THIEVED:
		case TEVENT_ANY:
		case 55:
		case 56:
		case 57:
		case 58:
			attach = AttachType(attach | ATTACH_HOUSE);
			break;

		default:
			break;
	}

	switch (event) {
		case TEVENT_GAME_TIME:
		case TEVENT_TIME:
		case TEVENT_RANDOM_TIME:
		case TEVENT_GLOBAL_SET:
		case TEVENT_GLOBAL_CLEAR:
		case TEVENT_LOCAL_SET:
		case TEVENT_LOCAL_CLEAR:
		case TEVENT_MISSION_TIMER_EXPIRED:
		case TEVENT_ANY:
		case TEVENT_AMBIENT_LESS_THAN:
		case TEVENT_AMBIENT_GREATER_THAN:
		case TEVENT_LEAVES_MAP:
		case TEVENT_PICKUP_CRATE_ANY:
		case 60:
		case 61:
			attach = AttachType(attach | ATTACH_GENERAL);
			break;

		default:
			break;
	}

	return(attach);
}

// FUNCTION: GAMEMD 0x0071F800
void TEventClass::Detach(AbstractClass const * target, bool all)
{
	if (Next == target) {
		Next = Next->Next;
	}
	if (Team == target) {
		Team = NULL;
	}
}

// FUNCTION: GAMEMD 0x0071F820
void TEventClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	if (Next != NULL) crc(Next->Fetch_ID());
	crc(Event);
	if (Team != NULL) crc(Team->Fetch_ID());
	crc((int)Data.Value);
}

// FUNCTION: GAMEMD 0x0071F880
HRESULT STDMETHODCALLTYPE TEventClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_EventClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0071F8C0
HRESULT STDMETHODCALLTYPE TEventClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TEventClass(NoInitClass());

		Swizzle_Pointer(&Next);
		Swizzle_Pointer(&Team);
		Swizzle_Pointer(&EventHouse);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0071F930
HRESULT STDMETHODCALLTYPE TEventClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0071F950
bool TEventClass::Is_Time_Based(void) const
{
	switch (Event) {
		case 1: case 2: case 3: case 4: case 6: case 7:
		case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
		case 29: case 31: case 33: case 34: case 35:
		case 38: case 39: case 40: case 41: case 42: case 43: case 44:
		case 48: case 49: case 50: case 53: case 54: case 59:
			return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0071F9C0
bool TEventClass::Is_To_Flag_As_Tripped(void) const
{
	switch (Event) {
		case 1: case 6: case 31: case 44: case 59:
			return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x0071FA30
HouseClass * TEventClass::Get_Event_House(void) const
{
	return(EventHouse);
}

// FUNCTION: GAMEMD 0x0071FA40
void TEventClass::Set_Event_House(HouseClass * house)
{
	EventHouse = house;
}

// FUNCTION: GAMEMD 0x0071FA50
// TEventClass::Fetch_Object_Size
// FUNCTION: GAMEMD 0x0071FA60
// TEventClass::Fetch_RTTI
// FUNCTION: GAMEMD 0x0071FA70
// TEventClass::Fetch_Heap_ID
// SYNTHETIC: GAMEMD 0x0071FA80
// ??_GTEventClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0071FF00 SYMBOL
// _$E43
const Rect RECT_NONE_2(0, 0, 0, 0);

// TEMPLATE: GAMEMD 0x0071FBA0
// ?Clear@?$DynamicVectorClass@PAVTEventClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0071FBD0
// ?ID@?$DynamicVectorClass@PAVTEventClass@@@@UAEHPBQAVTEventClass@@@Z
// TEMPLATE: GAMEMD 0x0071FBF0
// ?GetItem@?$VectorClass@PAVTEventClass@@@@UBEPAVTEventClass@@H@Z
// TEMPLATE: GAMEMD 0x0071FC00
// ??8?$VectorClass@PAVTEventClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0071FC40
// ?Resize@?$VectorClass@PAVTEventClass@@@@UAE_NHPBQAVTEventClass@@@Z
// TEMPLATE: GAMEMD 0x0071FCF0
// ?Clear@?$VectorClass@PAVTEventClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0071FD20
// ?ID@?$VectorClass@PAVTEventClass@@@@UAEHABQAVTEventClass@@@Z
// TEMPLATE: GAMEMD 0x0071FD60
// ?ID@?$VectorClass@PAVTEventClass@@@@UAEHPBQAVTEventClass@@@Z
// TEMPLATE: GAMEMD 0x0071FD80
// ?Resize@?$DynamicVectorClass@PAVTEventClass@@@@UAE_NHPBQAVTEventClass@@@Z
// TEMPLATE: GAMEMD 0x0071FE30
// ?ID@?$DynamicVectorClass@PAVTEventClass@@@@UAEHABQAVTEventClass@@@Z
// SYNTHETIC: GAMEMD 0x0071FE60
// ??_G?$VectorClass@PAVTEventClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0071FEB0
// ??_G?$DynamicVectorClass@PAVTEventClass@@@@UAEPAXI@Z
template class DynamicVectorClass<TEventClass *>;

