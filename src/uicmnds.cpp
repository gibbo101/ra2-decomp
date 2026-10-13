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
#include "_map.h"
#include "_tactica.h"
#include "display.h"
#include "mouse.h"
#include "tactical.h"
#include "scenario.h"
#include <mmsystem.h>
#include <string.h>
#include "stringtable.h"

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
extern int DAT_00845550;
extern int DAT_00845554;
void __fastcall FUN_007313A0(int group);
void __fastcall FUN_006DA740(Tactical * tactical);
void FUN_0070D150(void);
namespace Unsorted { extern bool MoveFeedback; }
extern Coord DAT_00B0FE10;
extern Coord DAT_00B0FE38;
extern Cell DAT_00B0FDF0;
extern int DAT_00880990;
extern DynamicVectorClass<UnitClass *> DAT_008B4108;
class PlanningNode;
PlanningNode * FUN_0063AC60(void);
void __fastcall FUN_00637D00(PlanningNode * node);
void FUN_006379C0(void);
void FUN_00637A10(void);
void __fastcall FUN_0063A120(bool a, bool b);
class BeaconManagerClass
{
	public:
		void Remove_Beacon(int x, int y);
};
extern BeaconManagerClass BeaconManager;


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

// FUNCTION: 0x007310D0
void __fastcall FUN_007310D0(int group)
{
	int count = DAT_00A8EC78.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * techno = DAT_00A8EC78[i];
		if (techno && Player_Owned(techno) && techno->Group == group - 1) {
			techno->Group = -1;
		}
	}
}

// FUNCTION: 0x00731130
bool __fastcall FUN_00731130(int group)
{
	for (int i = DAT_00A8EC78.Count() - 1; i >= 0; i--) {
		TechnoClass * techno = DAT_00A8EC78[i];
		if (techno && !techno->IsInLimbo && Player_Owned(techno)) {
			if (techno->Group == group - 1 && !techno->IsSelected) {
				return false;
			}
			if (techno->Group != group - 1 && techno->IsSelected) {
				return false;
			}
		}
	}
	return true;
}

static inline void Cancel_Modes(void)
{
	Map.Power_Mode_Control(0);
	Map.Waypoint_Mode_Control(0, false);
	Map.Repair_Mode_Control(0);
	Map.Sell_Mode_Control(0);
	Map.Place_Beacon_Mode_Control(0);
}

// FUNCTION: 0x007311C0
void __fastcall FUN_007311C0(int group)
{
	Cancel_Modes();
	if (timeGetTime() - DAT_00845550 < 800 && group == DAT_00845554 && FUN_00731130(group)) {
		FUN_007313A0(group);
		return;
	}
	DAT_00845550 = timeGetTime();
	DAT_00845554 = group;
	FUN_006DA740(TacticalMap);
	Unsorted::MoveFeedback = true;
	for (int i = 0; i < DAT_00A8EC78.Count(); i++) {
		TechnoClass * techno = DAT_00A8EC78[i];
		if (techno && !techno->IsInLimbo && techno->Group == group - 1 && Player_Owned(techno) && !techno->IsSelected) {
			techno->Select();
			techno->RadarFlashTimer = Rule->RadarCombatFlashTime;
			Unsorted::MoveFeedback = false;
		}
	}
	Unsorted::MoveFeedback = true;
	FUN_0070D150();
}

// FUNCTION: 0x007313A0
void __fastcall FUN_007313A0(int group)
{
	Cancel_Modes();
	if (DAT_00A8ECB8.Count()) {
		if (!DAT_00A8ECB8[0]->Is_Foot() || ((FootClass *)DAT_00A8ECB8[0])->Group != group - 1) {
			FUN_006DA740(TacticalMap);
		}
	}
	for (int i = 0; i < DAT_00A8EC78.Count(); i++) {
		TechnoClass * techno = DAT_00A8EC78[i];
		if (techno && !techno->IsInLimbo && techno->Group == group - 1 && Player_Owned(techno) && !techno->IsSelected) {
			techno->Select();
			Unsorted::MoveFeedback = false;
		}
	}
	Map.Center_Map();
	Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
	Unsorted::MoveFeedback = true;
}

// FUNCTION: 0x007314C0
void __fastcall FUN_007314C0(int group)
{
	Cancel_Modes();
	Unsorted::MoveFeedback = true;
	for (int i = 0; i < DAT_00A8EC78.Count(); i++) {
		TechnoClass * techno = DAT_00A8EC78[i];
		if (techno && !techno->IsInLimbo && techno->Group == group - 1 && Player_Owned(techno) && !techno->IsSelected) {
			techno->Select();
			Unsorted::MoveFeedback = false;
		}
	}
	Unsorted::MoveFeedback = true;
}

static inline bool Is_Base_Unit(UnitTypeClass const * type)
{
	for (int k = 0; k < Rule->BaseUnit.Count(); k++) {
		if (Rule->BaseUnit[k] == type) {
			return true;
		}
	}
	return false;
}

// FUNCTION: 0x007315A0
void FUN_007315A0(void)
{
	Coord yard = DAT_00B0FE10;
	Coord other = DAT_00B0FE10;
	int i = PlayerPtr->Buildings.Count() - 1;
	if (i >= 0) {
		for (; i >= 0; i--) {
			BuildingClass * building = PlayerPtr->Buildings[i];
			if (building && !building->IsInLimbo) {
				if (building->Class->IsConstructionYard) {
					yard = building->Center_Coord();
					if (building->IsLeader) {
						break;
					}
				} else if (other == DAT_00B0FE10) {
					other = building->Center_Coord();
				}
			}
		}
		if (!(other == DAT_00B0FE10 && yard == DAT_00B0FE10)) {
			goto place;
		}
	}
	if (PlayerPtr->CurUnits) {
		for (int j = DAT_008B4108.Count() - 1; j >= 0; j--) {
			UnitClass * unit = DAT_008B4108[j];
			if (unit && !unit->IsInLimbo && unit->House->Is_Player_Control()) {
				if (Is_Base_Unit(unit->Class)) {
					yard = unit->Center_Coord();
					break;
				}
			}
		}
	}
place:
	Coord base = PlayerPtr->Get_Base_Coord();
	if (!(base == DAT_00B0FE10) && !(base == DAT_00B0FE38)) {
		other = base;
	}
	if (!(yard == DAT_00B0FE10)) {
		TacticalMap->Set_Tactical_Position(yard);
	} else if (!(other == DAT_00B0FE10)) {
		TacticalMap->Set_Tactical_Position(other);
	}
	if (DAT_00880990) {
		Map.Set_Cursor_Pos(DAT_00B0FDF0);
	}
	Map.Set_To_Follow(NULL);
	Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
}

// FUNCTION: 0x00731840
void __fastcall FUN_00731840(int house)
{
	if (Session.Type != GAME_NORMAL && !Scen->Special.IsAllianceFixed && Rule->AlliesAllowed && Rule->AllyChangeAllowed) {
		OutList.Add(EventClass(PlayerPtr->HeapID, (unsigned char)EventClass::ALLY, house));
	}
}

// FUNCTION: 0x007318F0
void FUN_007318F0(void)
{
	if (DAT_00A8ECB8.Count()) {
		if (!PlayerPtr->IsDefeated && DAT_00A8ECB8[0]->Owner_HouseClass() != PlayerPtr && DAT_00A8ECB8[0]->Owner_HouseClass()->IsHuman) {
			FUN_00731840(DAT_00A8ECB8[0]->Owner());
		}
	}
}

// FUNCTION: 0x00731A10
void FUN_00731A10(void)
{
	PlanningNode * node = FUN_0063AC60();
	if (node) {
		FUN_00637D00(node);
	} else {
		BeaconManager.Remove_Beacon(-1, -1);
	}
}

// FUNCTION: 0x00731A30
void FUN_00731A30(void)
{
	if (!Unsorted::MuteSWLaunches) {
		Map.Place_Beacon_Mode_Control(-1);
	}
}

// FUNCTION: 0x00731A50
void __fastcall FUN_00731A50(bool flag)
{
	if (!Unsorted::MuteSWLaunches) {
		FUN_006379C0();
		FUN_0063A120(flag, true);
	}
}

// FUNCTION: 0x00731A70
void __fastcall FUN_00731A70(bool flag)
{
	FUN_00637A10();
	if (!Unsorted::MuteSWLaunches) {
		FUN_0063A120(flag, false);
	}
}

class InputManagerClass
{
	public:
		static InputManagerClass * Instance;
		bool IsKeyPressed(int key);
};
extern int DAT_00A8EC00;
extern int DAT_00A8EC04;
extern int DAT_00A8EC08;
extern int DAT_00A8EC0C;
extern bool DAT_00B0FE58;
extern bool DAT_00B0FE64;
namespace Unsorted { extern int NavCycleMode; }

// FUNCTION: 0x00731A90
bool FUN_00731A90(void)
{
	return FUN_00637AA0();
}

static inline bool All_Selected_Can_Attack_Move(void)
{
	int count = DAT_00A8ECB8.Count();
	for (int i = 0; i < count; i++) {
		ObjectClass * object = DAT_00A8ECB8[i];
		if (object && object->Is_Techno()) {
			if (!((TechnoClass *)object)->CanAttackOnTheMove()) {
				return false;
			}
		}
	}
	return true;
}

// FUNCTION: 0x00731AA0
bool FUN_00731AA0(void)
{
	return All_Selected_Can_Attack_Move();
}

// FUNCTION: 0x00731AF0
void FUN_00731AF0(void)
{
	if (DAT_00A8ECB8.Count() == 0) {
		FUN_00730A90(StringTable::LoadString("MSG:NothingSelected", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x358), -1);
		DAT_00B0FE58 = true;
		return;
	}
	if (!All_Selected_Can_Attack_Move()) {
		FUN_00730A90(StringTable::LoadString("MSG:AttackMoveUnsupported", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x35e), -1);
		return;
	}
	DAT_00B0FE58 = true;
}

// FUNCTION: 0x00731BE0
void FUN_00731BE0(void)
{
	DAT_00B0FE58 = false;
}

static inline bool Key_Down(int key)
{
	return InputManagerClass::Instance->IsKeyPressed(key);
}

// FUNCTION: 0x00731BF0
bool FUN_00731BF0(void)
{
	if (DAT_00B0FE58) {
		return true;
	}
	bool a = Key_Down(DAT_00A8EC00) || Key_Down(DAT_00A8EC04);
	bool b = Key_Down(DAT_00A8EC08) || Key_Down(DAT_00A8EC0C);
	if (a && b) {
		int count = DAT_00A8ECB8.Count();
		for (int i = 0; i < count; i++) {
			ObjectClass * object = DAT_00A8ECB8[i];
			if (object && object->Is_Techno()) {
				if (!((TechnoClass *)object)->CanAttackOnTheMove()) {
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

// FUNCTION: 0x00731CB0
int FUN_00731CB0(void)
{
	return 0x47;
}

// FUNCTION: 0x00731CC0
int FUN_00731CC0(void)
{
	return 0x16;
}

// FUNCTION: 0x00731CD0
int FUN_00731CD0(void)
{
	return Map.Get_Mouse_Current_Frame((MouseType)0x4d, false);
}

// FUNCTION: 0x00731CE0
int FUN_00731CE0(void)
{
	return Map.Get_Mouse_Current_Frame((MouseType)0x4c, false);
}

// FUNCTION: 0x00731CF0
void FUN_00731CF0(void)
{
	DAT_00B0FE58 = false;
}

// FUNCTION: 0x00731D00
void __fastcall FUN_00731D00(int mode)
{
	Unsorted::NavCycleMode = mode;
	DAT_00B0FE58 = false;
}

// FUNCTION: 0x00731D10
wchar_t * __fastcall FUN_00731D10(wchar_t * buffer)
{
	if (DAT_00A8ECB8.Count() == 0) {
		return wcscpy(buffer, StringTable::LoadString("MSG:NothingSelected", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x3f1));
	}
	if (DAT_00B0FE64) {
		return wcscpy(buffer, StringTable::LoadString("MSG:SelAcrossMap", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x3f3));
	}
	return wcscpy(buffer, StringTable::LoadString("MSG:SelAcrossScreen", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x3f5));
}
