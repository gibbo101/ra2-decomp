// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x00730760 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00730780 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x007307B0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x007307D0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x007307F0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00730810 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00730830 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00730850 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00730890 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x007308C0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x007308F0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00730920 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00730930 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00730950 SYMBOL
// _$E41


#include "_rules.h"
#include "building.h"
#include "event.h"
#include "goptions.h"
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
#include "cell.h"
#include <mmsystem.h>
#include <string.h>
#include <math.h>
#include "stringtable.h"

class UICommandHolder
{
	public:
		virtual ~UICommandHolder(void);
		int Reserved[3];
		void * Table;
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


// FUNCTION: GAMEMD 0x00730970
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

// FUNCTION: GAMEMD 0x00730990
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

// FUNCTION: GAMEMD 0x00730A10
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

// FUNCTION: GAMEMD 0x00730A90
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

// FUNCTION: GAMEMD 0x00730AD0
int FUN_00730AD0(void)
{
	return TPF_USE_GRAD_PAL | TPF_FULLSHADOW | TPF_6PT_GRAD;
}

// FUNCTION: GAMEMD 0x00730AE0
int FUN_00730AE0(void)
{
	return 240;
}


static inline UnitClass * Unit_Of(ObjectClass * object)
{
	return (object->RTTI == RTTI_UNIT) ? (UnitClass *)object : NULL;
}

// FUNCTION: GAMEMD 0x00730D60
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

// FUNCTION: GAMEMD 0x00730EA0
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

// FUNCTION: GAMEMD 0x00730F30
void FUN_00730F30(void)
{
	if (!Unsorted::MuteSWLaunches && Rule->MaximumCheerRate + DAT_0084555C < Unsorted::CurrentFrame) {
		OutList.Add(EventClass(PlayerPtr->HeapID, (unsigned char)EventClass::ALL_CHEER));
		DAT_0084555C = Unsorted::CurrentFrame;
	}
}

// FUNCTION: GAMEMD 0x00730FE0
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

// FUNCTION: GAMEMD 0x00731060
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

// FUNCTION: GAMEMD 0x007310D0
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

// FUNCTION: GAMEMD 0x00731130
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

// FUNCTION: GAMEMD 0x007311C0
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

// FUNCTION: GAMEMD 0x007313A0
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

// FUNCTION: GAMEMD 0x007314C0
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

// FUNCTION: GAMEMD 0x007315A0
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

// FUNCTION: GAMEMD 0x00731840
void __fastcall FUN_00731840(int house)
{
	if (Session.Type != GAME_NORMAL && !Scen->Special.IsAllianceFixed && Rule->AlliesAllowed && Rule->AllyChangeAllowed) {
		OutList.Add(EventClass(PlayerPtr->HeapID, (unsigned char)EventClass::ALLY, house));
	}
}

// FUNCTION: GAMEMD 0x007318F0
void FUN_007318F0(void)
{
	if (DAT_00A8ECB8.Count()) {
		if (!PlayerPtr->IsDefeated && DAT_00A8ECB8[0]->Owner_HouseClass() != PlayerPtr && DAT_00A8ECB8[0]->Owner_HouseClass()->IsHuman) {
			FUN_00731840(DAT_00A8ECB8[0]->Owner());
		}
	}
}

// FUNCTION: GAMEMD 0x00731A10
void FUN_00731A10(void)
{
	PlanningNode * node = FUN_0063AC60();
	if (node) {
		FUN_00637D00(node);
	} else {
		BeaconManager.Remove_Beacon(-1, -1);
	}
}

// FUNCTION: GAMEMD 0x00731A30
void FUN_00731A30(void)
{
	if (!Unsorted::MuteSWLaunches) {
		Map.Place_Beacon_Mode_Control(-1);
	}
}

// FUNCTION: GAMEMD 0x00731A50
void __fastcall FUN_00731A50(bool flag)
{
	if (!Unsorted::MuteSWLaunches) {
		FUN_006379C0();
		FUN_0063A120(flag, true);
	}
}

// FUNCTION: GAMEMD 0x00731A70
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
extern bool DAT_00B0FE58;
extern bool DAT_00B0FE64;
namespace Unsorted { extern int NavCycleMode; }

// FUNCTION: GAMEMD 0x00731A90
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

// FUNCTION: GAMEMD 0x00731AA0
bool FUN_00731AA0(void)
{
	return All_Selected_Can_Attack_Move();
}

// FUNCTION: GAMEMD 0x00731AF0
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

// FUNCTION: GAMEMD 0x00731BE0
void FUN_00731BE0(void)
{
	DAT_00B0FE58 = false;
}

static inline bool Key_Down(int key)
{
	return InputManagerClass::Instance->IsKeyPressed(key);
}

// FUNCTION: GAMEMD 0x00731BF0
bool FUN_00731BF0(void)
{
	if (DAT_00B0FE58) {
		return true;
	}
	bool a = Key_Down(Options.KeyForceAttack1) || Key_Down(Options.KeyForceAttack2);
	bool b = Key_Down(Options.KeySelect1) || Key_Down(Options.KeySelect2);
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

// FUNCTION: GAMEMD 0x00731CB0
int FUN_00731CB0(void)
{
	return 0x47;
}

// FUNCTION: GAMEMD 0x00731CC0
int FUN_00731CC0(void)
{
	return 0x16;
}

// FUNCTION: GAMEMD 0x00731CD0
int FUN_00731CD0(void)
{
	return Map.Get_Mouse_Start_Frame((MouseType)0x4d, false);
}

// FUNCTION: GAMEMD 0x00731CE0
int FUN_00731CE0(void)
{
	return Map.Get_Mouse_Start_Frame((MouseType)0x4c, false);
}

// FUNCTION: GAMEMD 0x00731CF0
void FUN_00731CF0(void)
{
	DAT_00B0FE58 = false;
}

// FUNCTION: GAMEMD 0x00731D00
void __fastcall FUN_00731D00(int mode)
{
	Unsorted::NavCycleMode = mode;
	DAT_00B0FE58 = false;
}

// FUNCTION: GAMEMD 0x00731D10
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

typedef DynamicVectorClass<ObjectClass *> ObjectList;
typedef DynamicVectorClass<TechnoClass *> TechnoList;

class TacticalSelection
{
	public:
		int Count(void);
		ObjectClass * Get(int index);
};
extern TacticalSelection * TacticalSelectionInstance;
extern bool DAT_00B0FE64;
extern int DAT_00A8ECC8;
unsigned char __fastcall FUN_007342C0(ObjectClass * object);
bool __fastcall FUN_00732580(TechnoClass * techno, int unused);
int __fastcall FUN_007325C0(TechnoClass * techno, int unused);
void __fastcall FUN_006DA770(Tactical * tactical);
extern Tactical * TacticalMap;

// FUNCTION: GAMEMD 0x00731D90
void __fastcall FUN_00731D90(wchar_t * buffer, wchar_t const * name)
{
	wchar_t upper[100];
	if (DAT_00B0FE6C->Table && name) {
		wcscpy(upper, name);
		_wcsupr(upper);
		int count = DAT_00A8ECB8.Count();
		if (count == 0) {
			swprintf(buffer, StringTable::LoadString("MSG:NoUnitsSel", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x408), upper);
			return;
		}
		int total = 0;
		for (int i = 0; i < count; i++) {
			ObjectClass * object = DAT_00A8ECB8[i];
			if (object && object->Is_Techno()) {
				HouseClass * house = ((TechnoClass *)object)->House;
				total += object->Class_Of()->Cost_Of(house);
			}
		}
		swprintf(buffer, StringTable::LoadString("MSG:UnitsWorth", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x41c), count, upper, total);
		return;
	}
	wcscpy(buffer, StringTable::LoadString("MSG:NavEmpty", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x3fe));
}

// FUNCTION: GAMEMD 0x00731EB0
TechnoList * __fastcall FUN_00731EB0(bool (__fastcall * test)(TechnoClass *, int), int arg)
{
	TechnoList * result = new TechnoList;
	int count = DAT_00A8EC78.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * techno = DAT_00A8EC78[i];
		if (test(techno, arg)) {
			result->Add(techno);
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x00731F70
TechnoList * __fastcall FUN_00731F70(bool (__fastcall * test)(ObjectClass *, int), int arg)
{
	TechnoList * result = new TechnoList;
	int count = TacticalSelectionInstance->Count();
	for (int i = 0; i < count; i++) {
		ObjectClass * object = TacticalSelectionInstance->Get(i);
		if (object && object->Is_Techno() && test(object, arg)) {
			result->Add((TechnoClass *)object);
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x00732050
TechnoList * FUN_00732050(void)
{
	TechnoList * result = new TechnoList;
	int count = DAT_00A8ECC8;
	for (int i = 0; i < count; i++) {
		ObjectClass * object = DAT_00A8ECB8[i];
		if (object && object->Is_Techno()) {
			result->Add((TechnoClass *)object);
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x00732110
void __fastcall FUN_00732110(ObjectList * list, void (__fastcall * fn)(ObjectClass *, int), int arg)
{
	int count = list->Count();
	for (int i = 0; i < count; i++) {
		fn((*list)[i], arg);
	}
}

// FUNCTION: GAMEMD 0x00732140
void __fastcall FUN_00732140(ObjectList * list, bool (__fastcall * test)(ObjectClass *, int), void (__fastcall * fn)(ObjectClass *, int), int test_arg, int fn_arg)
{
	int count = list->Count();
	for (int i = 0; i < count; i++) {
		ObjectClass * object = (*list)[i];
		if (test(object, test_arg)) {
			fn(object, fn_arg);
		}
	}
}

// FUNCTION: GAMEMD 0x00732190
bool __fastcall FUN_00732190(ObjectList * list, bool (__fastcall * test)(ObjectClass *, int), int arg)
{
	int count = list->Count();
	for (int i = 0; i < count; i++) {
		if (!test((*list)[i], arg)) {
			return false;
		}
	}
	return true;
}

// FUNCTION: GAMEMD 0x007321D0
bool __fastcall FUN_007321D0(ObjectList * list, bool (__fastcall * test)(ObjectClass *, int), int arg)
{
	int count = list->Count();
	for (int i = 0; i < count; i++) {
		if (test((*list)[i], arg)) {
			return true;
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x00732210
int __fastcall FUN_00732210(DynamicVectorClass<char const *> * list, char const * name)
{
	for (int i = 0; i < list->Count(); i++) {
		if (strcmp(name, (*list)[i]) == 0) {
			return i;
		}
	}
	return -1;
}

// FUNCTION: GAMEMD 0x00732580
int __fastcall Is_Owned_Active(TechnoClass * techno, int unused)
{
	if (techno && techno->IsActive) {
		bool owned;
		if (Session.Type == GAME_NORMAL) {
			owned = techno->House->IsPlayerControl;
		} else {
			owned = techno->House->Is_Player_Control();
		}
		if (owned) {
			return true;
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x007325C0
int __fastcall Is_Selectable_Combatant(TechnoClass * techno, int unused)
{
	if (techno->Fetch_RTTI() != RTTI_BUILDING && techno->Techno_Type_Class()->IsSelectableCombatant && techno->CanBeSelectedNow()) {
		return true;
	}
	return false;
}

bool __fastcall FUN_00732770(TechnoClass * techno, char const * name);

static inline TechnoList * Visible_Of_Name(char const * name)
{
	TechnoList * list = new TechnoList(0, NULL);
	int count = TacticalSelectionInstance->Count();
	for (int i = 0; i < count; i++) {
		ObjectClass * object = TacticalSelectionInstance->Get(i);
		if (object && object->Is_Techno()) {
			TechnoClass * item = (TechnoClass *)object;
			if (FUN_00732770(item, name)) {
				list->Add(item);
			}
		}
	}
	return list;
}

// FUNCTION: GAMEMD 0x00732770
int __fastcall Is_Player_Alive_Unit_Of(TechnoClass * techno, char const * name)
{
	if (techno && techno->IsActive && Player_Owned(techno) && techno->Is_Type_Named(name)) {
		return true;
	}
	return false;
}

namespace Unsorted { extern bool TypeSelecting; }
extern unsigned DAT_00B0FE68;
void __fastcall FUN_00732950(void);

typedef DynamicVectorClass<char const *> NameList;
NameList * FUN_00732D10(void);
bool __fastcall FUN_00732C30(TechnoClass * techno, NameList * names);

// FUNCTION: GAMEMD 0x00732C30
bool __fastcall Is_Named_Selectable(TechnoClass * techno, DynamicVectorClass<char const *> * names)
{
	int count = names->Count();
	for (int i = 0; i < count; i++) {
		char const * name = (*names)[i];
		if (techno->Is_Type_Named(name)) {
			if (techno->CanBeSelectedNow()) {
				return true;
			}
			if (techno && techno->Fetch_RTTI() == RTTI_BUILDING && techno->Techno_Type_Class()->UndeploysInto) {
				return true;
			}
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x00732CA0
void FUN_00732CA0(void)
{
	if (!Unsorted::TypeSelecting) {
		Unsorted::TypeSelecting = true;
		DAT_00B0FE68 = timeGetTime();
	}
}

// FUNCTION: GAMEMD 0x00732CC0
void FUN_00732CC0(void)
{
	if (Unsorted::TypeSelecting) {
		Unsorted::TypeSelecting = false;
		if (abs((int)(timeGetTime() - DAT_00B0FE68)) <= 500) {
			FUN_00732950();
			Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
		}
	}
}

// FUNCTION: GAMEMD 0x00732D00
bool FUN_00732D00(void)
{
	return Unsorted::TypeSelecting;
}

typedef DynamicVectorClass<char const *> NameList;

static inline bool Add_Name(NameList * list, char const * name)
{
	int index = -1;
	for (int i = 0; i < list->Count(); i++) {
		if (strcmp(name, (*list)[i]) == 0) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		list->Add((char *)name);
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00732D10
NameList * FUN_00732D10(void)
{
	NameList * list = new NameList(0, NULL);
	int count = DAT_00A8ECC8;
	for (int i = 0; i < count; i++) {
		ObjectClass * object = DAT_00A8ECB8[i];
		if (Add_Name(list, object->Class_Of()->IniName)) {
			if (object->Techno_Type_Class()->DeploysInto) {
				Add_Name(list, object->Techno_Type_Class()->DeploysInto->IniName);
			}
			if (object->Techno_Type_Class()->UndeploysInto) {
				Add_Name(list, object->Techno_Type_Class()->UndeploysInto->IniName);
			}
		}
	}
	return list;
}

// FUNCTION: GAMEMD 0x00732FF0
void __fastcall FUN_00732FF0(char const * name, void (__fastcall * fn)(ObjectClass *, int), int arg)
{
	TechnoList * list;
	if (DAT_00B0FE64) {
		list = new TechnoList(0, NULL);
		int count = DAT_00A8EC78.Count();
		for (int i = 0; i < count; i++) {
			TechnoClass * techno = DAT_00A8EC78[i];
			if (FUN_00732770(techno, name)) {
				list->Add(techno);
			}
		}
	} else {
		list = Visible_Of_Name(name);
	}
	int count = list->Count();
	for (int i = 0; i < count; i++) {
		fn((*list)[i], arg);
	}
	if (list) {
		delete list;
	}
}

static inline ObjectList * New_Object_List(void)
{
	return new ObjectList(0, NULL);
}

static inline TechnoList * New_Techno_List(void)
{
	return new TechnoList(0, NULL);
}

// FUNCTION: GAMEMD 0x007331B0
void FUN_007331B0(void)
{
	if (DAT_00B0FE6C) {
		delete DAT_00B0FE6C;
		DAT_00B0FE6C = NULL;
	}
	int i = 0;
	TechnoList * list = new TechnoList(0, NULL);
	int count = DAT_00A8ECC8;
	for (; i < count; i++) {
		ObjectClass * object = DAT_00A8ECB8[i];
		if (object && object->Is_Techno()) {
			list->Add((TechnoClass *)object);
		}
	}
	DAT_00B0FE6C = (UICommandHolder *)list;
	if (list->Count() == 0) {
		if (list) {
			delete list;
		}
		DAT_00B0FE6C = NULL;
		list = New_Techno_List();
		int visible = TacticalSelectionInstance->Count();
		for (i = 0; i < visible; i++) {
			ObjectClass * object = TacticalSelectionInstance->Get(i);
			if (object && object->Is_Techno() && object->IsActive && Player_Owned((TechnoClass *)object) && object->Fetch_RTTI() != RTTI_BUILDING) {
				list->Add((TechnoClass *)object);
			}
		}
		DAT_00B0FE6C = (UICommandHolder *)list;
	}
}

extern wchar_t const DAT_00887734[];
extern int DAT_00845560;
extern int DAT_00845564;
extern int DAT_00B0FE70;
extern char DAT_00B0FD70[];
int __fastcall FUN_007335F0(TechnoClass * techno, int unused);

static inline wchar_t const * Health_Text(int status)
{
	if (status == 0) {
		return StringTable::LoadString("MSG:Critical", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x644);
	}
	if (status == 1) {
		return StringTable::LoadString("MSG:HeavilyDamaged", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x646);
	}
	if (status == 2) {
		return StringTable::LoadString("MSG:Healthy", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x648);
	}
	return DAT_00887734;
}

static inline int Cycle_Step(int value)
{
	if (value == -1) {
		return 0;
	}
	if (value == 0) {
		return 1;
	}
	return value == 1 ? 2 : 0;
}

static inline TechnoList * Wrap_All(void)
{
	return FUN_00732050();
}

static inline TechnoList * Wrap_Filter(bool (__fastcall * test)(ObjectClass *, int), int arg)
{
	return FUN_00731F70(test, arg);
}

static inline void Append_Object(ObjectList * list, ObjectClass * object)
{
	list->Add(object);
}

static inline void Mixed_Check(wchar_t const *& name, int next)
{
	ObjectList * all = New_Object_List();
	int total = DAT_00A8ECC8;
	for (int j = 0; j < total; j++) {
		ObjectClass * object = DAT_00A8ECB8[j];
		if (object && object->Is_Techno()) {
			Append_Object(all, object);
		}
	}
	int shown = all->Count();
	for (int k = 0; k < shown; k++) {
		if ((*all)[k]->GetHealthStatus() != next) {
			name = StringTable::LoadString("MSG:Mixed", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x62f);
			break;
		}
	}
	if (all) {
		delete all;
	}
}

static inline void Show_Message(wchar_t * text, wchar_t const * name)
{
	FUN_00731D90(text, name);
	FUN_00730A90(text, 240);
}

// FUNCTION: GAMEMD 0x00733380
void __fastcall FUN_00733380(bool redraw)
{
	wchar_t text[256];
	int next;
	if (Unsorted::MuteSWLaunches) {
		return;
	}
	if (Unsorted::NavCycleMode != 3) {
		FUN_00730970();
		TechnoList * list = Wrap_All();
		DAT_00B0FE6C = (UICommandHolder *)list;
		if (list->Count() == 0) {
			if (list) {
				delete list;
			}
			DAT_00B0FE6C = NULL;
			DAT_00B0FE6C = (UICommandHolder *)Wrap_Filter((bool (__fastcall *)(ObjectClass *, int))FUN_007335F0, 0);
		}
		FUN_006DA740(TacticalMap);
		next = 0;
	} else {
		next = Cycle_Step(DAT_00845560);
		if (redraw) {
			FUN_006DA740(TacticalMap);
		}
	}
	ObjectList * selection = (ObjectList *)DAT_00B0FE6C;
	int count = selection->Count();
	for (int i = 0; i < count; i++) {
		ObjectClass * object = (*selection)[i];
		if (object->GetHealthStatus() == next) {
			object->Select();
		}
	}
	wchar_t const * name;
	if (next == 0) {
		name = StringTable::LoadString("MSG:Critical", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x644);
	} else if (next == 1) {
		name = StringTable::LoadString("MSG:HeavilyDamaged", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x646);
	} else if (next == 2) {
		name = StringTable::LoadString("MSG:Healthy", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x648);
	} else {
		name = DAT_00887734;
	}
	Mixed_Check(name, next);
	Show_Message(text, name);
	ObjectList * current = (ObjectList *)DAT_00B0FE6C;
	DAT_00845560 = next;
	int remaining = current->Count();
	Unsorted::NavCycleMode = 0;
	if (remaining) {
		Unsorted::NavCycleMode = 3;
	}
	DAT_00B0FE58 = false;
}

// FUNCTION: GAMEMD 0x007335F0
int __fastcall FUN_007335F0(TechnoClass * techno, int unused)
{
	if (techno && techno->IsActive && Player_Owned(techno) && techno->Fetch_RTTI() != RTTI_BUILDING) {
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00733640
wchar_t const * __fastcall FUN_00733640(int status)
{
	return Health_Text(status);
}

// FUNCTION: GAMEMD 0x007336A0
int __fastcall FUN_007336A0(int value)
{
	if (value == -1) {
		return 0;
	}
	if (value == 0) {
		return 1;
	}
	return value == 1 ? 2 : 0;
}

class VeterancyReader
{
	public:
		int FUN_00750030(void);
};

static inline int Cycle_Rank(int value)
{
	if (value == -1) {
		return 0;
	}
	if (value == 0) {
		return 1;
	}
	return value == 1 ? 2 : 0;
}

static inline void Mixed_Rank(wchar_t const *& name, int next)
{
	ObjectList * all = New_Object_List();
	int total = DAT_00A8ECC8;
	for (int j = 0; j < total; j++) {
		ObjectClass * object = DAT_00A8ECB8[j];
		if (object && object->Is_Techno()) {
			Append_Object(all, object);
		}
	}
	int shown = all->Count();
	for (int k = 0; k < shown; k++) {
		TechnoClass * techno = (TechnoClass *)(*all)[k];
		if (((VeterancyReader *)&techno->Veterancy)->FUN_00750030() != next) {
			name = StringTable::LoadString("MSG:Mixed", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x67a);
			break;
		}
	}
	if (all) {
		delete all;
	}
}

// FUNCTION: GAMEMD 0x007336C0
void __fastcall FUN_007336C0(bool redraw)
{
	wchar_t text[256];
	int next;
	if (Unsorted::MuteSWLaunches) {
		return;
	}
	if (Unsorted::NavCycleMode != 4) {
		FUN_00730970();
		TechnoList * list = Wrap_All();
		DAT_00B0FE6C = (UICommandHolder *)list;
		if (list->Count() == 0) {
			if (list) {
				delete list;
			}
			DAT_00B0FE6C = NULL;
			DAT_00B0FE6C = (UICommandHolder *)Wrap_Filter((bool (__fastcall *)(ObjectClass *, int))FUN_007335F0, 0);
		}
		FUN_006DA740(TacticalMap);
		next = 0;
	} else {
		next = Cycle_Rank(DAT_00845564);
		if (redraw) {
			FUN_006DA740(TacticalMap);
		}
	}
	ObjectList * selection = (ObjectList *)DAT_00B0FE6C;
	int count = selection->Count();
	for (int i = 0; i < count; i++) {
		ObjectClass * object = (*selection)[i];
		if (((VeterancyReader *)&((TechnoClass *)object)->Veterancy)->FUN_00750030() == next) {
			object->Select();
		}
	}
	wchar_t const * name;
	if (next == 0) {
		name = StringTable::LoadString("MSG:Elite", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x68e);
	} else if (next == 1) {
		name = StringTable::LoadString("MSG:Veteran", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x690);
	} else if (next == 2) {
		name = StringTable::LoadString("MSG:LittleExperience", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x692);
	} else {
		name = DAT_00887734;
	}
	Mixed_Rank(name, next);
	Show_Message(text, name);
	ObjectList * current = (ObjectList *)DAT_00B0FE6C;
	DAT_00845564 = next;
	int remaining = current->Count();
	Unsorted::NavCycleMode = 0;
	if (remaining) {
		Unsorted::NavCycleMode = 4;
	}
	DAT_00B0FE58 = false;
}

// FUNCTION: GAMEMD 0x00733930
wchar_t const * __fastcall FUN_00733930(int level)
{
	if (level == 0) {
		return StringTable::LoadString("MSG:Elite", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x68e);
	}
	if (level == 1) {
		return StringTable::LoadString("MSG:Veteran", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x690);
	}
	if (level == 2) {
		return StringTable::LoadString("MSG:LittleExperience", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x692);
	}
	return DAT_00887734;
}

// FUNCTION: GAMEMD 0x00733990
int __fastcall FUN_00733990(int value)
{
	if (value == -1) {
		return 0;
	}
	if (value == 0) {
		return 1;
	}
	return value == 1 ? 2 : 0;
}

char * __fastcall FUN_00733E10(ObjectList * items, char const * preferred, int limit, int * best);
int __fastcall FUN_00733C60(TechnoClass * techno, char const * name);
extern char DAT_00B0FD70[];

static inline int Named_Selectable(TechnoClass * techno, char const * name)
{
	char const * id = techno->Class_Of()->IniName;
	if (name && strcmp(id, name) == 0 && techno->CanBeSelectedNow()) {
		return true;
	}
	return false;
}

static inline ObjectList * Collect_Selected(void)
{
	ObjectList * all = New_Object_List();
	int total = DAT_00A8ECC8;
	for (int j = 0; j < total; j++) {
		ObjectClass * object = DAT_00A8ECB8[j];
		if (object && object->Is_Techno()) {
			Append_Object(all, object);
		}
	}
	return all;
}

static inline void Find_Mixed(ObjectList * all, char const * chosen, wchar_t const *& name)
{
	int shown = all->Count();
	for (int k = 0; k < shown; k++) {
		if (!(unsigned char)FUN_00733C60((TechnoClass *)(*all)[k], chosen)) {
			name = StringTable::LoadString("MSG:Mixed", NULL, "D:\\ra2mdpost\\UICmnds.cpp", 0x6c3);
			break;
		}
	}
}

static inline void Remember_Name(char * chosen, int best)
{
	DAT_00B0FE70 = best;
	strcpy(DAT_00B0FD70, chosen);
	if (chosen) {
		delete chosen;
	}
}

static inline void Find_Mixed_Outer(ObjectList * all, char const * chosen, wchar_t const *& name)
{
	Find_Mixed(all, chosen, name);
}

// FUNCTION: GAMEMD 0x007339B0
void __fastcall FUN_007339B0(bool redraw)
{
	wchar_t text[256];
	int best;
	char * chosen;
	if (Unsorted::MuteSWLaunches) {
		return;
	}
	if (Unsorted::NavCycleMode != 5) {
		FUN_00730970();
		TechnoList * list = Wrap_All();
		DAT_00B0FE6C = (UICommandHolder *)list;
		if (list->Count() == 0) {
			if (list) {
				delete list;
			}
			DAT_00B0FE6C = NULL;
			DAT_00B0FE6C = (UICommandHolder *)Wrap_Filter((bool (__fastcall *)(ObjectClass *, int))FUN_007335F0, 0);
		}
		FUN_006DA740(TacticalMap);
		chosen = FUN_00733E10((ObjectList *)DAT_00B0FE6C, NULL, 0x7fffffff, &best);
	} else {
		chosen = FUN_00733E10((ObjectList *)DAT_00B0FE6C, DAT_00B0FD70, DAT_00B0FE70, &best);
		if (redraw) {
			FUN_006DA740(TacticalMap);
		}
	}
	ObjectList * selection = (ObjectList *)DAT_00B0FE6C;
	int count = selection->Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * techno = (TechnoClass *)(*selection)[i];
		char const * id = techno->Class_Of()->IniName;
		if (chosen && strcmp(id, chosen) == 0 && techno->CanBeSelectedNow()) {
			techno->Select();
		}
	}
	wchar_t const * name = NULL;
	ObjectList * all = Collect_Selected();
	Find_Mixed_Outer(all, chosen, name);
	if (!name && DAT_00A8ECC8 > 0) {
		name = DAT_00A8ECB8[0]->Class_Of()->UIName;
	}
	Show_Message(text, name);
	Remember_Name(chosen, best);
	int remaining = all->Count();
	Unsorted::NavCycleMode = 0;
	if (remaining) {
		Unsorted::NavCycleMode = 5;
	}
	DAT_00B0FE58 = false;
	if (all) {
		delete all;
	}
}

// FUNCTION: GAMEMD 0x00733C60
int __fastcall FUN_00733C60(TechnoClass * techno, char const * name)
{
	char const * id = techno->Class_Of()->IniName;
	if (name && strcmp(id, name) == 0 && techno->CanBeSelectedNow()) {
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00733CD0
NameList * __fastcall FUN_00733CD0(ObjectList * objects)
{
	NameList * list = new NameList(0, NULL);
	int count = objects->Count();
	for (int i = 0; i < count; i++) {
		Add_Name(list, (*objects)[i]->Class_Of()->IniName);
	}
	return list;
}

extern int DAT_00845558;

// FUNCTION: GAMEMD 0x00734210
void FUN_00734210(void)
{
	DAT_00845550 = timeGetTime() - 0x641;
	DAT_00845554 = -1;
	DAT_00845558 = timeGetTime() - 0x641;
	DAT_0084555C = -100;
}

extern BuildingClass * DAT_00B0FE5C;
extern BuildingClass * DAT_00B0FE60;

// FUNCTION: GAMEMD 0x00734250
void __fastcall FUN_00734250(BuildingClass * building)
{
	if (building->Class->BuildCat == 5) {
		DAT_00B0FE60 = building;
	} else {
		DAT_00B0FE5C = building;
	}
}

// FUNCTION: GAMEMD 0x00734270
void TechnoClass::ClearSidebarTabObject(void)
{
	if (this == NULL) {
		DAT_00B0FE5C = NULL;
		DAT_00B0FE60 = NULL;
	} else if (this == (TechnoClass *)DAT_00B0FE5C) {
		DAT_00B0FE5C = NULL;
	} else if (this == (TechnoClass *)DAT_00B0FE60) {
		DAT_00B0FE60 = NULL;
	}
}

// FUNCTION: GAMEMD 0x007342A0
BuildingClass * FUN_007342A0(void)
{
	return DAT_00B0FE5C;
}

// FUNCTION: GAMEMD 0x007342B0
BuildingClass * FUN_007342B0(void)
{
	return DAT_00B0FE60;
}

// FUNCTION: GAMEMD 0x007342C0
unsigned char __fastcall FUN_007342C0(ObjectClass * object)
{
	if (!object) {
		return 0;
	}
	unsigned char flag = object->AbstractFlags & 1;
	return flag;
}

// TEMPLATE: GAMEMD 0x007342D0
// ??0?$DynamicVectorClass@PAVTechnoClass@@@@QAE@IPBQAVTechnoClass@@@Z
template class DynamicVectorClass<TechnoClass *>;

// TEMPLATE: GAMEMD 0x00734330
// ??0?$DynamicVectorClass@PBD@@QAE@IPBQBD@Z
// TEMPLATE: GAMEMD 0x00734390
// ?Add@?$DynamicVectorClass@PBD@@QAE_NABQBD@Z
template class DynamicVectorClass<char const *>;

// TEMPLATE: GAMEMD 0x007343E0
// ??0?$VectorClass@PBD@@QAE@HPBQBD@Z
template class VectorClass<char const *>;

// SYNTHETIC: GAMEMD 0x00734430 SYMBOL
// _$E149
// SYNTHETIC: GAMEMD 0x00734460 SYMBOL
// _$E152
// SYNTHETIC: GAMEMD 0x00734480 SYMBOL
// _$E155
extern double __cdecl FUN_004CAC40(double);
double DAT_00B11E98 = FUN_004CAC40(pow(256.0, 2.0) * 2);
extern const double DAT_007F5C18;
double DAT_00B1CF50 = DAT_00B11E98 / DAT_007F5C18;
double DAT_00B1CF38 = 1.0 / DAT_00B1CF50;

extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007F5C20;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntTripleMid {
	int A, B, C;
	IntTripleMid() { C = 0; A = 128; B = 128; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// SYNTHETIC: GAMEMD 0x007344A0 SYMBOL
// _$E158
double DAT_00B1BF30 = DAT_007E1720 * DAT_007F5C20;

// SYNTHETIC: GAMEMD 0x007344C0 SYMBOL
// _$E161
double DAT_00B12EA0 = DAT_007E1728 * DAT_007F5C20;

// SYNTHETIC: GAMEMD 0x007344E0 SYMBOL
// _$E164
double DAT_00B0FE78 = DAT_007E1730 * DAT_007F5C20;

// SYNTHETIC: GAMEMD 0x00734500 SYMBOL
// _$E167
int DAT_00B1CF5C = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0FE78 - DAT_00B12EA0) * DAT_00B11E98));

// SYNTHETIC: GAMEMD 0x00734540 SYMBOL
// _$E170
double DAT_00B13EA8 = FUN_004CADE0(DAT_00B1CF5C * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x00734570 SYMBOL
// _$E173
double DAT_00B1BF28 = FUN_004CADE0((DAT_00B1CF5C * 2) / DAT_00B11E98);

// SYNTHETIC: GAMEMD 0x007345A0 SYMBOL
// _$E176
int DAT_00B1CF4C = (int)(DAT_00B1CF5C * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x007345D0 SYMBOL
// _$E179
ShortPair DAT_00B0FE90;

// SYNTHETIC: GAMEMD 0x007345E0 SYMBOL
// _$E182
IntTriple DAT_00B13F18;

// SYNTHETIC: GAMEMD 0x00734600 SYMBOL
// _$E185
IntTripleMid DAT_00B1CF40;

// SYNTHETIC: GAMEMD 0x00734620 SYMBOL
// _$E188
IntQuad DAT_00B0FE80;
