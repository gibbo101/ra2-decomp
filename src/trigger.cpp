// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "trigger.h"
#include "ccrand.h"
#include "crc.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "taction.h"
#include "tevent.h"
#include "tracker.h"
#include "trigtype.h"
#include "vector.h"

// FUNCTION: 0x00725FA0
TriggerClass::TriggerClass(TriggerTypeClass * trigtype) :
	Class(trigtype),
	LinkedTo(NULL),
	House(NULL),
	IsToDelete(false),
	Timer(0),
	IsTripped(0),
	IsActive(true)
{
	AbstractTypePtrTracker.Add(this);
	TriggerPtrTracker.Add(this);
	Triggers.Add(this);

	if (Class != NULL)
	{
		Reset_All_Timed_Events();
		if (!Class->Is_Enabled()) {
			IsActive = false;
		} else if (Scen->Difficulty == DIFF_EASY && !Class->Difficulty.IsEnabledOnEasy) {
			IsActive = false;
		} else if (Scen->Difficulty == DIFF_NORMAL && !Class->Difficulty.IsEnabledOnMedium) {
			IsActive = false;
		} else if (Scen->Difficulty == DIFF_HARD && !Class->Difficulty.IsEnabledOnHard) {
			IsActive = false;
		}
	}
}

// FUNCTION: 0x00726140
TriggerClass::~TriggerClass(void)
{
	Detach_This_From_All(this, true);
	Triggers.Delete(this);
	AbstractTypePtrTracker.Delete(this);
	TriggerPtrTracker.Delete(this);
}

// FUNCTION: 0x00726250
bool TriggerClass::Is_Cross_Horizontal(void) const
{
	if (LinkedTo != NULL) {
		return(Class->Is_Cross_Horizontal() || LinkedTo->Is_Cross_Horizontal());
	}
	return(Class->Is_Cross_Horizontal());
}

// FUNCTION: 0x00726290
bool TriggerClass::Is_Cross_Vertical(void) const
{
	if (LinkedTo != NULL) {
		return(Class->Is_Cross_Vertical() || LinkedTo->Is_Cross_Vertical());
	}
	return(Class->Is_Cross_Vertical());
}

// FUNCTION: 0x007262D0
bool TriggerClass::Is_Enters_Zone(void) const
{
	if (LinkedTo != NULL) {
		return(Class->Is_Enters_Zone() || LinkedTo->Class->Is_Enters_Zone());
	}
	return(Class->Is_Enters_Zone());
}

// FUNCTION: 0x00726310
bool TriggerClass::Is_Allow_Win(void) const
{
	if (LinkedTo != NULL) {
		return(Class->Is_Allow_Win() || LinkedTo->Class->Is_Allow_Win());
	}
	return(Class->Is_Allow_Win());
}

// FUNCTION: 0x00726350
bool TriggerClass::Is_Linked_To_Global(int global) const
{
	if (LinkedTo != NULL) {
		return(Class->Is_Linked_To_Global(global) || LinkedTo->Class->Is_Linked_To_Global(global));
	}
	return(Class->Is_Linked_To_Global(global));
}

// FUNCTION: 0x007263A0
void TriggerClass::Reset_Global_Linked_Timed_Events(int global)
{
	TriggerClass * tp = this;
	while (tp != NULL) {
		if (tp->Class->Is_Linked_To_Global(global)) {
			tp->Reset_All_Timed_Events();
		}
		tp = tp->LinkedTo;
	}
}

// FUNCTION: 0x007263D0
void TriggerClass::Reset_Local_Linked_Timed_Events(int local)
{
	TriggerClass * tp = this;
	while (tp != NULL) {
		if (tp->Class->Is_Linked_To_Local(local)) {
			tp->Reset_All_Timed_Events();
		}
		tp = tp->LinkedTo;
	}
}

// FUNCTION: 0x00726400
void TriggerClass::Reset_All_Timed_Events(void)
{
	int index = 0;
	TEventClass * event = Class->FirstEvent;
	while (event != NULL) {
		if (event->Event == TEVENT_TIME) {
			Timer = event->Data.Value * TICKS_PER_SECOND;
			Flag_Event_Untripped(index);
		}
		if (event->Event == TEVENT_RANDOM_TIME) {
			Timer = ((event->Data.Value/2) + Random_Pick(0, int(event->Data.Value))) * TICKS_PER_SECOND;
			Flag_Event_Untripped(index);
		}
		index++;
		event = event->Next;
	}
}

// FUNCTION: 0x007264C0
bool TriggerClass::Should_Spring(TEventType event, ObjectClass * object, bool forced, bool persistent, TechnoClass * source)
{
	if (!IsEnabled || Is_Marked_To_Delete()) {
		return(false);
	}

	bool all_sprung = true;
	if (!forced) {
		TEventClass * tevent = Class->FirstEvent;
		int index = 0;
		while (tevent != NULL) {
			if (Is_Event_Tripped(index) || tevent->operator()(event, House_From_HousesType((HousesType)Class->House->HeapID), object, Timer, persistent, source)) {
				if (tevent->Get_Event_House() != NULL) {
					House = tevent->Get_Event_House();
				}
				if (persistent) {
					if (tevent->Is_Time_Based() && tevent->Is_To_Flag_As_Tripped()) {
						Flag_Event_Tripped(index);
					}
				}
			} else {
				all_sprung = false;
			}
			index++;
			tevent = tevent->Next;
		}
	}

	if (all_sprung && persistent) {
		Reset_All_Timed_Events();
	}

	return(all_sprung);
}

// FUNCTION: 0x007265C0
bool TriggerClass::Spring(ObjectClass * object, Cell cell)
{
	if (!IsEnabled || Is_Marked_To_Delete()) {
		return(false);
	}

	bool done = false;
	TActionClass * taction = Class->FirstAction;
	while (taction != NULL) {
		if (taction->operator()(House_From_HousesType((HousesType)Class->House->HeapID), object, this, cell)) {
			done = true;
		}
		taction = taction->Next;
	}

	return(done);
}

// FUNCTION: 0x00726630
TriggerClass * Find_Or_Make(TriggerTypeClass * trigtype)
{
	if (trigtype == NULL) return(NULL);

	for (int index = 0; index < Triggers.Count(); index++) {
		if (trigtype == Triggers[index]->Class) {
			return(Triggers[index]);
		}
	}

	TriggerClass * trig = new TriggerClass(trigtype);
	return(trig);
}

// FUNCTION: 0x00726690
void TriggerClass::Detach(AbstractClass const * target, bool all)
{
	if (Class == target) {
		Class = NULL;
	}
	if (LinkedTo == target) {
		LinkedTo = LinkedTo->LinkedTo;
	}
	if (House == target) {
		House = NULL;
	}
}

// FUNCTION: 0x007266C0
void TriggerClass::Flag_Event_Tripped(int event)
{
	IsTripped |= (1 << event);
}

// FUNCTION: 0x007266E0
void TriggerClass::Flag_Event_Untripped(int event)
{
	IsTripped &= ~(1 << event);
}

// FUNCTION: 0x00726700
bool TriggerClass::Is_Event_Tripped(int event)
{
	return((IsTripped & (1 << event)) != 0);
}

// FUNCTION: 0x00726720
void TriggerClass::Mark_To_Delete(void)
{
	IsToDelete = true;
	ObjectsToDelete.Add(this);
}

// FUNCTION: 0x00726780
bool TriggerClass::Is_Marked_To_Delete(void) const
{
	return(IsToDelete);
}

// FUNCTION: 0x00726790
void TriggerClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(Class->Fetch_ID());
	if (LinkedTo != NULL) crc(LinkedTo->Fetch_ID());

	crc(IsToDelete);
	crc(IsActive);

	crc((int)Timer);
	crc(IsTripped);
}

// FUNCTION: 0x00726820
HRESULT STDMETHODCALLTYPE TriggerClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TriggerClass;
	return(S_OK);
}

// FUNCTION: 0x00726860
HRESULT STDMETHODCALLTYPE TriggerClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TriggerClass(NoInitClass());

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&LinkedTo);
		Swizzle_Pointer(&House);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x007268D0
HRESULT STDMETHODCALLTYPE TriggerClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x007268F0
void TriggerClass::Enable(void)
{
	IsActive = true;
	Reset_All_Timed_Events();
}

// FUNCTION: 0x00726900
void TriggerClass::Disable(void)
{
	IsActive = false;
}

// FUNCTION: 0x00726910
HouseClass * TriggerClass::Get_House(void) const
{
	return(House);
}

// FUNCTION: 0x00726920
void TriggerClass::Set_House(HouseClass * house)
{
	House = house;
}

// FUNCTION: 0x00726930 TriggerClass::Fetch_Object_Size

// FUNCTION: 0x00726940 TriggerClass::Fetch_RTTI

// FUNCTION: 0x00726950 TriggerClass::~TriggerClass

// FUNCTION: 0x00726A70 $E2
// FUNCTION: 0x00726AA0 $E5
// FUNCTION: 0x00726AC0 $E8
// FUNCTION: 0x00726AE0 $E11
// FUNCTION: 0x00726B00 $E14
// FUNCTION: 0x00726B20 $E17
// FUNCTION: 0x00726B40 $E20
// FUNCTION: 0x00726B80 $E23

extern double __cdecl FUN_004CADE0(double);
extern int DAT_00B0F850;
extern double DAT_00B0F7F8;
extern const double DAT_007E1738;

// FUNCTION: 0x00726BB0 $E26
double DAT_00B0F820 = FUN_004CADE0((DAT_00B0F850 * 2) / DAT_00B0F7F8);

// FUNCTION: 0x00726BE0 $E29
int DAT_00B0F844 = (int)(DAT_00B0F850 * 4 + DAT_007E1738);

struct TrigZero2S { short a, b; TrigZero2S() : a(0), b(0) {} };
struct TrigZero3I { int a, b, c; TrigZero3I() : a(0), b(0), c(0) {} };
struct TrigInit128 { int a, b, c; TrigInit128() { c = 0; a = 128; b = 128; } };
struct TrigZero4I { int a, b, c, d; TrigZero4I() : a(0), b(0), c(0), d(0) {} };

// FUNCTION: 0x00726C10 $E32
TrigZero2S DAT_00B0F7F0;

// FUNCTION: 0x00726C20 $E35
TrigZero3I DAT_00B0F810;

// FUNCTION: 0x00726C40 $E38
TrigInit128 DAT_00B0F838;

// FUNCTION: 0x00726C60 $E50
TrigZero4I DAT_00B0F7E0;
