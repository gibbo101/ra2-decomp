// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x0071E410 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0071E440 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0071E460 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0071E480 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0071E4A0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0071E4C0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0071E4E0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0071E520 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0071E550 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0071E580 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0071E5B0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0071E5C0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0071E5E0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0071E600 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x0071E620 SYMBOL
// _$E86
// SYNTHETIC: GAMEMD 0x0071E660 SYMBOL
// _$E84
DynamicVectorClass<TEventClass *> Events;

namespace Unsorted { extern bool ArmageddonMode; }

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

// FUNCTION: GAMEMD 0x0071E940
bool TEventClass::operator () (TEventType event, HouseClass const * house, ObjectClass const * object, CDTimerClass<FrameTimerClass> & timer, bool & tripped, TechnoClass * source)
{
	switch (Event) {
		case 60: {
			int found = -1;
			for (int index = TechnoTypes.Count() - 1; index >= 0; index--) {
				if (String == TechnoTypes[index]->IniName) {
					found = index;
					break;
				}
			}
			if (found < TechnoTypes.Count() && found >= 0) {
				TechnoTypeClass * type = TechnoTypes[found];
				int count = 0;
				int needed = Data.Value;
				for (int i = Technos.Count() - 1; i >= 0; i--) {
					if (Technos[i]->Techno_Type_Class() == type) {
						count++;
					}
					if (count >= needed) {
						return(true);
					}
				}
			}
			return(false);
		}

		case 61: {
			int found = -1;
			for (int index = TechnoTypes.Count() - 1; index >= 0; index--) {
				if (String == TechnoTypes[index]->IniName) {
					found = index;
					break;
				}
			}
			if (found < TechnoTypes.Count() && found >= 0) {
				TechnoTypeClass * type = TechnoTypes[found];
				for (int i = Technos.Count() - 1; i >= 0; i--) {
					if (Technos[i]->Techno_Type_Class() == type) {
						return(false);
					}
				}
				return(true);
			}
			return(false);
		}

		case TEVENT_AMBIENT_LESS_THAN:
			return(Scen->CurrentAmbientLight <= Data.Value);

		case TEVENT_AMBIENT_GREATER_THAN:
			return(Scen->CurrentAmbientLight >= Data.Value);

		case TEVENT_GAME_TIME:
			if (Data.Value > (Frame / TICKS_PER_SECOND)) {
				return(false);
			} else {
				return(true);
			}

		case TEVENT_GLOBAL_SET: {
			bool value;
			Scen->Fetch_Global_Value(Data.Value, value);
			return(value);
		}

		case TEVENT_GLOBAL_CLEAR: {
			bool value;
			Scen->Fetch_Global_Value(Data.Value, value);
			return(!value);
		}

		case TEVENT_LOCAL_SET: {
			bool value;
			Scen->Fetch_Local_Value(Data.Value, value);
			return(value);
		}

		case TEVENT_LOCAL_CLEAR: {
			bool value;
			Scen->Fetch_Local_Value(Data.Value, value);
			return(!value);
		}

		case TEVENT_MISSION_TIMER_EXPIRED:
			if (!Scen->MissionTimer.Is_Active() || Scen->MissionTimer != 0) return(false);
			return(true);

		case TEVENT_TIME:
		case TEVENT_RANDOM_TIME:
			if (timer != 0) return(false);
			return(true);
	}

	if (Event == TEVENT_NONE) {
		return(false);
	}

	if (Event != TEVENT_BUILD &&
		Event != TEVENT_BUILD_UNIT &&
		Event != TEVENT_BUILD_INFANTRY &&
		Event != TEVENT_BUILD_AIRCRAFT &&
		Is_Time_Based()) {
		if ((event != Event) || Unsorted::ArmageddonMode) {
			return(false);
		}
	}

	if (Event == TEVENT_PLAYER_ENTERED || Event == TEVENT_CROSS_HORIZONTAL || Event == TEVENT_CROSS_VERTICAL || Event == TEVENT_ENTERS_ZONE || Event == 59) {
		if (event != Event || (Data.House != HOUSE_NONE && HouseClass::FindByCountryIndex(Data.House) == NULL) || !object || (Data.House != HOUSE_NONE && object->Owner() != HouseClass::FindByCountryIndex(Data.House)->HeapID)) return(false);
		tripped = true;
		EventHouse = object->Owner_HouseClass();
		return(true);
	}
	else if (Event == 53) {
		if (event != Event || object == NULL || Data.House == HOUSE_NONE || HouseClass::FindByCountryIndex(Data.House) == NULL || object->GetDisguiseHouse(true) == NULL || object->GetDisguiseHouse(true)->HeapID != HouseClass::FindByCountryIndex(Data.House)->HeapID) return(false);
		tripped = true;
		return(true);
	}
	else if (Event == 54) {
		if (event != Event || object == NULL) return(false);
		ObjectTypeClass * disguise = object->GetDisguise(true);
		if (disguise->Fetch_RTTI() != RTTI_INFANTRYTYPE || Data.Value == -1 || disguise->Fetch_Heap_ID() != Data.Value) return(false);
		tripped = true;
		return(true);
	}
	else if (Event == TEVENT_NEAR_WAYPOINT) {
		if (event != Event) return(false);
		Coord waypoint_coord(Scen->Get_Waypoint_Coord(Data.Value));
		if (object->Distance(waypoint_coord) > (CELL_LEPTON_W*5)) {
			return(false);
		}
		return(true);
	}
	else if (Event == TEVENT_ATTACKED_BY) {
		if (event != Event || source == NULL || Data.House != source->House->HeapID) {
			return(false);
		}
	}

	int index;
	if (house != NULL) {
		switch (Event) {
			case TEVENT_LEAVES_MAP:
				for (index = 0; index < Teams.Count(); index++) {
					TeamClass * ptr = Teams[index];
					if (ptr->Class == Team && ptr->Is_Empty() && ptr->IsLeaveMap) {
						tripped = true;
						break;
					}
				}
				if (index == Teams.Count()) return(false);
				break;

			case TEVENT_CREDITS:
				if (((HouseClass *)house)->Available_Money() < Data.Value) return(false);
				break;

			case TEVENT_CREDITS_BELOW:
				if (((HouseClass *)house)->Available_Money() > Data.Value) return(false);
				break;

			case TEVENT_NOFACTORIES:
				if (house->CurBuildings > 0) {
					for (int index = 0; index < house->Buildings.Count(); index++) {
						BuildingClass * ptr = house->Buildings[index];
						if (ptr != NULL && !ptr->IsInLimbo && ptr->Class->ToBuild != RTTI_NONE) {
							return(false);
						}
					}
				}
				break;

			case TEVENT_EVAC_CIVILIAN:
				if (!house->IsCivEvacuated) return(false);
				break;

			case TEVENT_BUILDING_EXISTS:
				if (house->ABQuantity.Value(Data.Structure) == 0) return(false);
				tripped = true;
				break;

			case 57:
				if (house->ABQuantity.Value(Data.Structure) != 0) return(false);
				tripped = true;
				break;

			case TEVENT_BUILD:
				if (house->JustBuiltStructure != Data.Structure) return(false);
				tripped = true;
				break;

			case TEVENT_BUILD_UNIT:
				if (house->JustBuiltUnit != Data.Unit) return(false);
				tripped = true;
				break;

			case TEVENT_BUILD_INFANTRY:
				if (house->JustBuiltInfantry != Data.Infantry) return(false);
				tripped = true;
				break;

			case TEVENT_BUILD_AIRCRAFT:
				if (house->JustBuiltAircraft != Data.Aircraft) return(false);
				tripped = true;
				break;

			case TEVENT_NBUILDINGS_DESTROYED:
				if (house->BuildingsLost < Data.Value) return(false);
				break;

			case TEVENT_NUNITS_DESTROYED:
				if (house->UnitsLost < Data.Value) return(false);
				break;

			default:
				break;
		}
	}

	house = HouseClass::FindByCountryIndex(Data.House);
	if (house != NULL) {
		switch (Event) {
			case TEVENT_LOW_POWER:
				if (house->Power_Fraction() >= 1) return(false);
				break;

			case 58:
				if (house->Power_Fraction() < 1) return(false);
				break;

			case TEVENT_THIEVED:
				if (!house->IsThieved) return(false);
				break;

			case TEVENT_HOUSE_DISCOVERED:
				if (!house->IsDiscovered) return(false);
				break;

			case TEVENT_BUILDINGS_DESTROYED:
				if (house->CurBuildings > 0) return(false);
				break;

			case TEVENT_UNITS_DESTROYED:
				if (house->AUQuantity.Total() > 0 || house->AIQuantity.Total() > 0) return(false);
				break;

			case 55:
				if (house->OwnedNavy > 0) return(false);
				break;

			case 56:
				if (house->CurUnits - house->OwnedNavy > 0) return(false);
				if (house->CurInfantry > 0) return(false);
				break;

			case TEVENT_ALL_DESTROYED:
				if (house->CurBuildings > 0 || house->AUQuantity.Total() > 0 || house->AIQuantity.Total() > 0) return(false);
				break;

			default:
				break;
		}
	}

	return(true);
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
	} else if (need == NEED_NUMBER_AND_TECHNO) {
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
			return(NEED_NUMBER_AND_TECHNO);

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
