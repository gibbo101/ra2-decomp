// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"

// FUNCTION: 0x00730760 $E2
// FUNCTION: 0x00730780 $E5
// FUNCTION: 0x007307B0 $E8
// FUNCTION: 0x007307D0 $E11
// FUNCTION: 0x007307F0 $E14
// FUNCTION: 0x00730810 $E17
// FUNCTION: 0x00730830 $E20
// FUNCTION: 0x00730850 $E23
// FUNCTION: 0x00730890 $E26
// FUNCTION: 0x007308C0 $E29
// FUNCTION: 0x007308F0 $E32
// FUNCTION: 0x00730920 $E35
// FUNCTION: 0x00730930 $E38
// FUNCTION: 0x00730950 $E41


#include "_rules.h"
#include "building.h"
#include "event.h"
#include "house.h"
#include "infantry.h"
#include "msglist.h"
#include "object.h"
#include "queue.h"
#include "rules.h"
#include "session.h"
#include "techno.h"
#include "unit.h"
#include "unittype.h"
#include "vector.h"
#include "voc.h"

class UICommandHolder
{
	public:
		virtual ~UICommandHolder(void);
};

extern UICommandHolder * DAT_00B0FE6C;
extern DynamicVectorClass<TechnoClass *> DAT_00A8EC78;
extern DynamicVectorClass<ObjectClass *> DAT_00A8ECB8;
extern HouseClass * PlayerPtr;
extern QueueClass<EventClass, 128> OutList;
extern int DAT_0084555C;
namespace Unsorted { extern int MuteSWLaunches; extern int CurrentFrame; }
bool FUN_00637AA0(void);
void FUN_0063A0D0(void);


// FUNCTION: 0x00730970
void FUN_00730970(void)
{
	if (DAT_00B0FE6C) {
		delete DAT_00B0FE6C;
		DAT_00B0FE6C = NULL;
	}
}

static inline bool Player_Owned(TechnoClass * techno)
{
	if (Session.Type == GAME_NORMAL) {
		return techno->House->IsPlayerControl;
	}
	return techno->House->Is_Player_Control();
}

// FUNCTION: 0x00730990
bool __fastcall FUN_00730990(int group)
{
	int count = DAT_00A8EC78.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * techno = DAT_00A8EC78[i];
		if (techno && !techno->IsInLimbo && Player_Owned(techno) && !techno->IsSelected && techno->Group == group - 1) {
			return false;
		}
	}
	return true;
}

// FUNCTION: 0x00730A10
int __fastcall FUN_00730A10(int group)
{
	int total = 0;
	int count = DAT_00A8EC78.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * techno = DAT_00A8EC78[i];
		if (techno && !techno->IsInLimbo && Player_Owned(techno) && techno->Group == group - 1) {
			total++;
		}
	}
	return total;
}

// FUNCTION: 0x00730A90
void __fastcall FUN_00730A90(wchar_t const * text, int timeout)
{
	if (timeout == -1) {
		timeout = 240;
	}
	int color = 3;
	if (PlayerPtr) {
		color = PlayerPtr->Scheme;
	}
	Session.Messages.Add_Message(NULL, 0, text, color, TextPrintType(TPF_USE_GRAD_PAL | TPF_FULLSHADOW | TPF_6PT_GRAD), timeout, true);
}

// FUNCTION: 0x00730AD0
int FUN_00730AD0(void)
{
	return TPF_USE_GRAD_PAL | TPF_FULLSHADOW | TPF_6PT_GRAD;
}

// FUNCTION: 0x00730AE0
int FUN_00730AE0(void)
{
	return 240;
}


static inline UnitClass * Unit_Of(ObjectClass * object)
{
	return (object->RTTI == RTTI_UNIT) ? (UnitClass *)object : NULL;
}

// FUNCTION: 0x00730D60
void FUN_00730D60(void)
{
	if (FUN_00637AA0()) {
		FUN_0063A0D0();
		return;
	}
	if (DAT_00A8ECB8.Count() > 0) {
		bool ok = true;
		int count = DAT_00A8ECB8.Count();
		for (int i = 0; i < count; i++) {
			ObjectClass * object = DAT_00A8ECB8[i];
			if (object && (object->AbstractFlags & 1)) {
				if (object->Can_Player_Move() && object->Can_Player_Fire() && object->RTTI != RTTI_BUILDING) {
					ok = ((TechnoClass *)object)->Player_Assign_Mission(MISSION_GUARD_AREA, (AbstractClass *)object->Get_Target_Cell_Ptr(), NULL, NULL);
					if (!ok) {
						return;
					}
				} else if (object->RTTI == RTTI_UNIT) {
					if (Unit_Of(object)->SlaveManager || Unit_Of(object)->Class->Harvester) {
						ok = ((TechnoClass *)object)->Player_Assign_Mission(MISSION_GUARD_AREA, (AbstractClass *)object->Get_Target_Cell_Ptr(), NULL, NULL);
						if (!ok) {
							return;
						}
					}
				}
			}
		}
		if (ok) {
			VocClass::PlayGlobal(Rule->GuardSound, 1.0f, 0x2000);
		}
	}
}

// FUNCTION: 0x00730EA0
void FUN_00730EA0(void)
{
	if (DAT_00A8ECB8.Count() > 0) {
		int count = DAT_00A8ECB8.Count();
		bool ok = true;
		for (int i = 0; i < count; i++) {
			ObjectClass * object = DAT_00A8ECB8[i];
			if (object && (object->AbstractFlags & 1) && (object->Can_Player_Move() || object->Can_Player_Fire())) {
				ok = ((TechnoClass *)object)->ClickedEvent(6);
				if (!ok) {
					return;
				}
			}
		}
		if (ok) {
			VocClass::PlayGlobal(Rule->StopSound, 1.0f, 0x2000);
		}
	}
}

// FUNCTION: 0x00730F30
void FUN_00730F30(void)
{
	if (!Unsorted::MuteSWLaunches && Rule->MaximumCheerRate + DAT_0084555C < Unsorted::CurrentFrame) {
		OutList.Add(EventClass(PlayerPtr->HeapID, (unsigned char)EventClass::ALL_CHEER));
		DAT_0084555C = Unsorted::CurrentFrame;
	}
}

// FUNCTION: 0x00730FE0
void FUN_00730FE0(void)
{
	if (DAT_00A8ECB8.Count() > 0) {
		int count = DAT_00A8ECB8.Count();
		bool ok = true;
		for (int i = 0; i < count; i++) {
			ObjectClass * object = DAT_00A8ECB8[i];
			if (object && (object->AbstractFlags & 1) && object->Can_Player_Move()) {
				ok = ((TechnoClass *)object)->ClickedEvent(7);
				if (!ok) {
					return;
				}
			}
		}
		if (ok) {
			VocClass::PlayGlobal(Rule->ScatterSound, 1.0f, 0x2000);
		}
	}
}

// FUNCTION: 0x00731060
void __fastcall FUN_00731060(int group)
{
	int count = DAT_00A8EC78.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * techno = DAT_00A8EC78[i];
		if (techno && Player_Owned(techno)) {
			if (techno->Group == group - 1) {
				techno->Group = -1;
			}
			if (techno->IsSelected) {
				techno->Group = group - 1;
			}
		}
	}
}
