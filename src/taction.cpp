// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "taction.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_tactica.h"
#include "_weapon.h"
#include "_xmouse.h"
#include "anim.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "empulse.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "ion.h"
#include "ionblast.h"
#include "map.h"
#include "movie.h"
#include "stringtable.h"
#include "partsys.h"
#include "reinf.h"
#include "revent.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "team.h"
#include "teamtype.h"
#include "theme.h"
#include "tracker.h"
#include "trigger.h"
#include "trigtype.h"
#include "vanim.h"
#include "vector.h"
#include "velocity.h"
#include "voc.h"
#include "vox.h"
#include "warhead.h"
#include "waypoint.h"
#include "weapon.h"
#include "draw.hh"
#include "need.hh"
#include <cstdio>

void __fastcall FUN_00489280(Coord const & coord, int damage, TechnoClass * source, WarheadTypeClass const * warhead, bool affecttiberium, HouseClass * house);
void __fastcall FUN_0048A620(Coord coord, int damage, WarheadTypeClass const * warhead, bool forced, int flags);
char const * __fastcall FUN_00751590(int index);
int __fastcall FUN_00753250(char const * name);
void __fastcall FUN_0070F120(FootClass * unit, TalkType bubble);
void __fastcall Hold_Scenario(int seconds);

void __fastcall FUN_0053AD00(int, int, int, int);
bool __fastcall FUN_0053A100(void);
void __fastcall FUN_00588AC0(MapClass * map);
extern int DAT_00842AFC[];
bool __fastcall FUN_0065D8E0(TeamTypeClass * team, int waypoint);
void __fastcall FUN_00539EB0(int duration, int deferment, Cell target, HouseClass * house);
bool __fastcall FUN_0065EC30(TeamTypeClass * team, int waypoint);
BulletClass * __fastcall FUN_0046B050(BulletTypeClass const * type, AbstractClass * target, TechnoClass * owner, int damage, WarheadTypeClass const * warhead, int speed, bool bright);
HouseClass * __fastcall FUN_00726910(TriggerClass * trig);
bool __fastcall FUN_00510F60(int index);
void __fastcall FUN_0053B460(int duration);
HouseClass * __fastcall FUN_00510ED0(int index);
HouseClass * __fastcall FUN_00502D30(int index);
void __fastcall FUN_004549B0(BuildingClass * building);

extern DynamicVectorClass<BuildingClass *> DAT_00A8EB40;
#define Buildings DAT_00A8EB40
extern DynamicVectorClass<InfantryClass *> DAT_00A83DE8;
#define Infantry DAT_00A83DE8

inline HouseClass * Find_House(TriggerClass * trig, int index)
{
	if (trig == NULL) return(NULL);
	if (index == 0x2325) return(FUN_00726910(trig));
	if (index == -1) return(NULL);
	if (FUN_00510F60(index)) return(FUN_00510ED0(index));
	return(FUN_00502D30(index));
}

DynamicVectorClass<TActionClass *> Actions;

void __fastcall FUN_0072DDB0(void);
void __fastcall FUN_0072DEF0(void);
void __fastcall FUN_006842F0(bool enable);
void __fastcall FUN_005BF260(VQType movie, ThemeType theme, bool a, bool b, bool c);
void __fastcall FUN_00752480(int index, int a, int b);
void __fastcall FUN_00578100(MapClass * map);
void __fastcall FUN_00750F40(Coord const * coord, int mask);
void __fastcall FUN_005BF390(VQType movie, bool pause);

extern Cell DAT_00B0E680;
extern struct PendingStruct { int Ptr; int Type; int House; } DAT_0088098C;
extern bool DAT_0088099A;
extern bool DAT_0088099B;
extern bool DAT_0088099C;
void __fastcall FUN_00684240(void);
extern DynamicVectorClass<TriggerClass *> DAT_00A8EAE8;
#define Triggers DAT_00A8EAE8
extern DynamicVectorClass<TagClass *> DAT_00B0E720;
#define Tags DAT_00B0E720
extern bool UserInputLocked;
class InputManagerClass {
	public:
		void DoSomething(void);
		static InputManagerClass * Instance;
};
void __fastcall FUN_007315A0(void);
extern wchar_t DAT_00887734[];
extern char DAT_00889F64[];

extern DynamicVectorClass<TechnoClass *> DAT_00A8EC78;
#define Technos DAT_00A8EC78

// FUNCTION: 0x006DCD70 $E2
// FUNCTION: 0x006DCDA0 $E5
// FUNCTION: 0x006DCDC0 $E8
// FUNCTION: 0x006DCDE0 $E11
// FUNCTION: 0x006DCE00 $E14
// FUNCTION: 0x006DCE20 $E17
// FUNCTION: 0x006DCE40 $E20
// FUNCTION: 0x006DCE80 $E23
// FUNCTION: 0x006DCEB0 $E26
// FUNCTION: 0x006DCEE0 $E29
// FUNCTION: 0x006DCF10 $E32
// FUNCTION: 0x006DCF20 $E35
// FUNCTION: 0x006DCF40 $E38
// FUNCTION: 0x006DCF60 $E47
// FUNCTION: 0x006DCF80 $E174
// FUNCTION: 0x006DCFC0 $E172

// FUNCTION: 0x006DD000
TActionClass::TActionClass(void) :
	HeapID(-1),
	Next(NULL),
	Action(TACTION_NONE),
	Team(NULL),
	TriggerRect(0,0,0,0),
	EffectLocation(0),
	Value2(0),
	Tag(NULL),
	Trigger(NULL),
	TechnoID()
{
	Actions.Add(this);
	HeapID = Actions.ID(this);

	AbstractTypePtrTracker.Add(this);
	EventActionPtrTracker.Add(this);
	Text[0] = 0;
	Data.Value = 0;
}

// FUNCTION: 0x006DD180
TActionClass::TActionClass(NoInitClass const & x) :
	BASECLASS(x),
	TechnoID(x)
{
}

// FUNCTION: 0x006DD1B0
TActionClass::~TActionClass(void)
{
	Detach_This_From_All(this, true);

	AbstractTypePtrTracker.Delete(this);
	EventActionPtrTracker.Delete(this);
	Actions.Delete(this);
	Text[0] = 0;
}

// FUNCTION: 0x006DD2C0
void TActionClass::Detach(AbstractClass const * target, bool all)
{
	if (Next == target) {
		Next = Next->Next;
	}
	if (Team == target) {
		Team = NULL;
	}
	if (Trigger == target) {
		Trigger = NULL;
	}
	if (Tag == target) {
		Tag = NULL;
	}
}

// FUNCTION: 0x006DD300
void TActionClass::Build_INI_Entry(char * ptr) const
{
	int mode = GetMode(Action);
	int val = Data.Value;
	int type = 0;
	char const * str = NULL;
	int extra2 = 0;
	int extra1 = 0;

	if (mode == 0x2f) {
		type = 9;
		str = TechnoID;
	} else if (mode == 0x2e) {
		type = 0xb;
		extra2 = val;
		extra1 = Value2;
	} else if (mode == 0x2b) {
		type = 9;
		str = TechnoID;
	} else if (mode == 0x2d) {
		type = 0xa;
		str = TechnoID;
	} else if (Team != NULL && mode == 0x28) {
		val = Team->HeapID;
		type = 5;
		str = Team->IniName;
	} else if (Team != NULL && (mode == 0xc || mode == 0x22)) {
		val = Team->HeapID;
		type = 1;
		str = Team->IniName;
	} else if (Trigger != NULL && mode == 0xb) {
		val = Trigger->HeapID;
		type = 2;
		str = Trigger->IniName;
	} else if (Tag != NULL && mode == 0x1a) {
		val = Tag->HeapID;
		type = 3;
		str = Tag->IniName;
	} else if (mode == 0x27) {
		type = 4;
		str = Text;
		if (!*str || !str) {
			str = "-1";
		}
	} else if (mode == 4) {
		type = 6;
		str = Speech_Name((VoxType)val);
	} else if (mode == 3 || mode == 0x26) {
		type = 7;
		str = FUN_00751590(val);
	} else if (mode == 1) {
		type = 8;
		str = Theme.GetID((ThemeType)val);
	}

	ptr += strlen(ptr);
	if (type == 0xb) {
		sprintf(ptr, "%d,%d,%d,%d,%d,%d,%d,%d", Action, type, extra2, TriggerRect.X, TriggerRect.Y, TriggerRect.Width, TriggerRect.Height, extra1);
	} else if (type == 0xa) {
		sprintf(ptr, "%d,%d,%s,%d,%d,%d,%d,%s", Action, type, str, TriggerRect.X, TriggerRect.Y, TriggerRect.Width, TriggerRect.Height, Waypoint_To_Name(EffectLocation));
	} else if (type == 5 || type == 9) {
		sprintf(ptr, "%d,%d,%s,%d,%d,%d,%d,%d", Action, type, str, TriggerRect.X, TriggerRect.Y, TriggerRect.Width, TriggerRect.Height, Data.Value);
	} else if (type != 0) {
		sprintf(ptr, "%d,%d,%s,%d,%d,%d,%d,%s", Action, type, str, TriggerRect.X, TriggerRect.Y, TriggerRect.Width, TriggerRect.Height, Waypoint_To_Name(EffectLocation));
	} else {
		sprintf(ptr, "%d,%d,%d,%d,%d,%d,%d,%s", Action, 0, val, TriggerRect.X, TriggerRect.Y, TriggerRect.Width, TriggerRect.Height, Waypoint_To_Name(EffectLocation));
	}
}

// FUNCTION: 0x006DD5B0
void TActionClass::Read_INI(void)
{
	Data.Value = 0;
	Action = TActionType(atoi(strtok(NULL, ",")));
	int code = atoi(strtok(NULL, ","));
	char * text = strtok(NULL, ",");
	int val = atoi(text);

	switch (code) {
		case 0:
		case 11:
			Data.Value = val;
			break;

		case 1:
		case 5:
			if (val == -1) {
				Team = NULL;
			} else if (strlen(text) < 3) {
				Team = TeamTypes[val];
			} else {
				Team = TeamTypeClass::Find_Or_Make(text);
			}
			break;

		case 2:
			if (val == -1) {
				Trigger = NULL;
			} else if (strlen(text) < 3) {
				Trigger = TriggerTypes[val];
			} else {
				Trigger = TriggerTypeClass::Find_Or_Make(text);
			}
			break;

		case 3:
			if (val == -1) {
				Tag = NULL;
			} else if (strlen(text) < 3) {
				Tag = TagTypes[val];
			} else {
				Tag = TagTypeClass::Find_Or_Make(text);
			}
			break;

		case 4:
			if (val == -1) {
				Text[0] = 0;
			} else {
				char * src = text;
				Text[0] = 0;
				if (src != NULL) {
					while (*src && (unsigned char)*src <= ' ') {
						src++;
					}
					if (*src) {
						strncpy(Text, src, 0x1f);
						Text[0x1f] = 0;
						int len = strlen(Text);
						while (len != 0 && (unsigned char)Text[len - 1] <= ' ') {
							len--;
						}
						Text[len] = 0;
					}
				}
			}
			break;

		case 6:
			Data.Value = FUN_00753250(text);
			break;

		case 7:
			Data.Value = VocClass::From_Name(text);
			break;

		case 8:
			Data.Value = Theme.From_Name(text);
			break;

		case 9:
		case 10:
			TechnoID = TStringID<24>(text);
			break;
	}

	TriggerRect.X = atoi(strtok(NULL, ","));
	TriggerRect.Y = atoi(strtok(NULL, ","));
	TriggerRect.Width = atoi(strtok(NULL, ","));
	TriggerRect.Height = atoi(strtok(NULL, ","));
	char * temp = strtok(NULL, ",");
	if (temp != NULL) {
		if (code == 5) {
			Data.Value = atoi(temp);
		} else if (code == 9) {
			Data.Value = atoi(temp);
		} else if (code == 0xb) {
			Value2 = atoi(temp);
		} else {
			EffectLocation = Waypoint_From_Name(temp);
		}
	}
}

// FUNCTION: 0x006E0030
bool TActionClass::TAction_FUN_006E0030(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	return(true);
}

// FUNCTION: 0x006E0040
bool TActionClass::TAction_CHANGE_ZOOM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	return(true);
}

// FUNCTION: 0x006E0050
bool TActionClass::TAction_ION_LIGHTNING_STRIKE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	IonStormClass::Lightning_Bolt(Scen->Get_Waypoint_Cell(EffectLocation));
	return(true);
}

// FUNCTION: 0x006E0080
bool TActionClass::TAction_REMOVE_PARTICLE_ANIM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell cell2 = Scen->Get_Waypoint_Cell(EffectLocation);

	for (int index = 0; index < ParticleSystems.Count(); index++) {
		ParticleSystemClass * psys = ParticleSystems[index];

		if (psys->IsActive && psys->PositionCell == cell2) {
			psys->Delete_Me();
		}
	}
	return(true);
}

// FUNCTION: 0x006E0110
bool TActionClass::TAction_PARTICLE_ANIM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell cell2 = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord(cell2);
	coord.Z = Map.Get_Height_GL(cell2);
	new ParticleSystemClass(ParticleSystemTypes[Data.PAnim], coord, NULL);
	return(true);
}

// FUNCTION: 0x006E01C0
bool TActionClass::TAction_WAKEUP_SELF(HouseClass * , ObjectClass * , TriggerClass * trigger, Cell const & )
{
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->RTTI != RTTI_BUILDING && techno->IsActive && techno->IsDown && techno->Tag != NULL && techno->Tag->Is_Trigger_Attached(trigger)) {
			if (techno->Mission == MISSION_SLEEP || techno->Mission == MISSION_HARMLESS) {
				techno->QueueMission(MISSION_GUARD, false);
			}
		}
	}
	return(true);
}

// FUNCTION: 0x006E0250
bool TActionClass::TAction_VEIN_GROWTH(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->IsVeinGrowth = Data.Bool;
	return(true);
}

// FUNCTION: 0x006E0270
bool TActionClass::TAction_TIB_GROWTH(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->IsTibGrowth = Data.Bool;
	return(true);
}

// FUNCTION: 0x006E0290
bool TActionClass::TAction_ICE_GROWTH(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->IsIceGrowth = Data.Bool;
	return(true);
}

// FUNCTION: 0x006E02B0
bool TActionClass::TAction_WAKEUP_ALL_SLEEP(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	for (int index = 0; index < Feet.Count(); index++) {
		FootClass * techno = Feet[index];
		if (techno->Mission == MISSION_SLEEP && techno->Strength > 0 && techno->IsActive && techno->IsDown && !techno->IsInLimbo && house != techno->House ) {
			techno->QueueMission(MISSION_GUARD, false);
		}
	}
	return(true);
}

// FUNCTION: 0x006E0330
bool TActionClass::TAction_WAKEUP_ALL_HARMLESS(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	for (int index = 0; index < Feet.Count(); index++) {
		FootClass * techno = Feet[index];
		if (techno->Mission == MISSION_HARMLESS && techno->Strength > 0 && techno->IsActive && techno->IsDown && !techno->IsInLimbo && techno->House == house) {
			techno->QueueMission(MISSION_GUARD, false);
		}
	}
	return(true);
}

// FUNCTION: 0x006E03B0
bool TActionClass::TAction_WAKEUP_GROUP(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * foot = Technos[index];
		if (foot->Group == Data.Value &&
				(foot->Mission == MISSION_SLEEP || foot->Mission == MISSION_HARMLESS) &&
				foot->Strength > 0 &&
				foot->IsActive &&
				foot->IsDown &&
				!foot->IsInLimbo) {
			foot->QueueMission(MISSION_GUARD, false);
		}
	}
	return(true);
}

// FUNCTION: 0x006E0440
bool TActionClass::TAction_ANNOUNCE_WIN(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	PlayerPtr->Flag_To_Win(true);
	return(true);
}

// FUNCTION: 0x006E0460
bool TActionClass::TAction_ANNOUNCE_LOSE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	PlayerPtr->Flag_To_Lose(true);
	return(true);
}

// FUNCTION: 0x006E0480
bool TActionClass::TAction_FORCE_END(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	PlayerPtr->Flag_To_End();
	return(true);
}

// FUNCTION: 0x006E0490
bool TActionClass::TAction_DAMAGE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	int damage = 100;
	FUN_00489280(Waypoint_As_Coord(), damage, NULL, Rule->C4Warhead, true, NULL);
	damage = 100;
	FUN_00489280(Waypoint_As_Coord() + Coord(CELL_LEPTON_W/3, CELL_LEPTON_H/3, 0), damage, NULL, Rule->C4Warhead, true, NULL);
	damage = 100;
	FUN_00489280(Waypoint_As_Coord() + Coord(-CELL_LEPTON_W/3, CELL_LEPTON_H/3, 0), damage, NULL, Rule->C4Warhead, true, NULL);
	damage = 100;
	FUN_00489280(Waypoint_As_Coord() + Coord(CELL_LEPTON_W/3, -CELL_LEPTON_H/3, 0), damage, NULL, Rule->C4Warhead, true, NULL);
	damage = 100;
	FUN_00489280(Waypoint_As_Coord() + Coord(-CELL_LEPTON_W/3, -CELL_LEPTON_H/3, 0), damage, NULL, Rule->C4Warhead, true, NULL);

	if (Scen->Is_Valid_Waypoint(Data.Value)) {
		Cell cell = Scen->Get_Waypoint_Cell(Data.Value);
		int tries = 3;
		while (Map.Damage_Bridge(cell) == false && tries-- > 0) {};
		Point2D point;
		Coord coord = Coord(cell);
		coord.Z += BRIDGE_LEPTON_HEIGHT;
		TacticalMap->Coord_To_Pixel(coord, point);
		Rect dirty (point.X-128, point.Y-128, 256, 256);
		TacticalMap->Register_Dirty_Area(dirty);
	}

	return(true);
}

// FUNCTION: 0x006E0790
bool TActionClass::TAction_LIGHT_SMALL(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	WarheadTypeClass const * warhead = Rule->C4Warhead;
	FUN_0048A620(Waypoint_As_Coord(), 50, warhead, false, 0);
	return(true);
}

// FUNCTION: 0x006E07F0
bool TActionClass::TAction_LIGHT_MEDIUM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	WarheadTypeClass const * warhead = Rule->C4Warhead;
	FUN_0048A620(Waypoint_As_Coord(), 100, warhead, false, 0);
	return(true);
}

// FUNCTION: 0x006E0850
bool TActionClass::TAction_LIGHT_LARGE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	WarheadTypeClass const * warhead = Rule->C4Warhead;
	FUN_0048A620(Waypoint_As_Coord(), 300, warhead, false, 0);
	return(true);
}

// FUNCTION: 0x006E08B0
bool TActionClass::TAction_SELL_ATTACHED(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	bool success = false;

	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		if (building->IsActive &&
				building->IsDown &&
				!building->IsInLimbo &&
				building->Tag != NULL &&
				building->Tag->Is_Trigger_Attached(trig)) {

			building->Sell_Back(1);
			success = true;
		}
	}
	return(success);
}

// FUNCTION: 0x006E0930
bool TActionClass::TAction_GO_BERZERK(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	bool success = false;

	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * infantry = Infantry[index];
		if (infantry->IsActive &&
				infantry->IsDown &&
				!infantry->IsInLimbo &&
				infantry->Tag != NULL &&
				infantry->Tag->Is_Trigger_Attached(trig)) {

			infantry->Berzerk();
			success = true;
		}
	}
	return(success);
}

// FUNCTION: 0x006E09A0
bool TActionClass::TAction_TURN_OFF_ATTACHED(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	bool success = false;
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		if (building->IsActive &&
				building->IsDown &&
				!building->IsInLimbo &&
				building->Tag != NULL &&
				building->Tag->Is_Trigger_Attached(trig) &&
				building->IsOn) {

			building->Turn_Off();
			success = true;
		}
	}
	return(success);
}

// FUNCTION: 0x006E0A20
bool TActionClass::TAction_TURN_ON_ATTACHED(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	bool success = false;
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		if (building->IsActive && building->IsDown && !building->IsInLimbo && building->Tag != NULL && building->Tag->Is_Trigger_Attached(trig) && !building->IsOn) {
			building->Turn_On();
			success = true;
		}
	}
	return(success);
}

// FUNCTION: 0x006E0AA0
bool TActionClass::TAction_CHANGE_HOUSE(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) {
		return(false);
	}
	bool success = false;

	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->IsActive &&
				techno->IsDown &&
				!techno->IsInLimbo &&
				techno->Tag != NULL &&
				techno->Tag->Is_Trigger_Attached(trig)) {

			techno->Captured(hptr, false);
			success = true;
		}
	}
	return(success);
}

// FUNCTION: 0x006E0B60
bool TActionClass::TAction_ALL_CHANGE_HOUSE(HouseClass * house, ObjectClass * , TriggerClass * trig, Cell const & )
{
	bool retval = false;

	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) {
		return(false);
	}

	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->House == house) {
			BuildingClass * building = (BuildingClass *)techno;
			if (techno == NULL || techno->RTTI != RTTI_BUILDING || building->Class == NULL || (!building->Class->IsPowered && !building->Class->PoweredSpecial)) {
				Technos[index]->Captured(hptr, false);
				retval = true;
			}
		}
	}

	for (int index2 = 0; index2 < Technos.Count(); index2++) {
		TechnoClass * techno = Technos[index2];
		if (techno->House == house) {
			BuildingClass * building = (BuildingClass *)techno;
			if (techno != NULL && techno->RTTI == RTTI_BUILDING && building->Class != NULL && (building->Class->IsPowered || building->Class->PoweredSpecial)) {
				Technos[index2]->Captured(hptr, false);
				FUN_004549B0(building);
				retval = true;
			}
		}
	}

	return(retval);
}

// FUNCTION: 0x006E0CA0
bool TActionClass::TAction_MIND_CONTROL_BUILDINGS(HouseClass * house, ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) {
		return(false);
	}
	house->MindControlBuildings(hptr);
	return(true);
}

// FUNCTION: 0x006E0D00
bool TActionClass::TAction_RELINQUISH_BUILDINGS(HouseClass * house, ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) {
		return(false);
	}
	house->RelinquishBuildings(hptr);
	return(true);
}

// FUNCTION: 0x006E0D60
bool TActionClass::TAction_TEXT_TRIGGER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	char * label = Text[0] ? Text : NULL;
	int side = Scen->PlayerSideIndex;
	int scheme = Session.Color_Index_To_Scheme(side == 0 ? 2 : (side == 1 ? 1 : 5));
	if (label != NULL) {
		Session.Messages.Add_Message(NULL, 0, StringTable::LoadString(label, NULL, "D:\\ra2mdpost\\TAction.CPP", 1766), scheme, TextPrintType(0x4046), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
	}
	return(true);
}

// FUNCTION: 0x006E0DF0
bool TActionClass::TAction_MAKE_ALLY(HouseClass * house, ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * house2 = Find_House(trig, Data.Value);
	if (house2 == NULL) {
		return(false);
	}
	house->Make_Ally(house2, false);
	house2->Make_Ally(house, false);
	return(true);
}

// FUNCTION: 0x006E0E60
bool TActionClass::TAction_MAKE_ENEMY(HouseClass * house, ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * house2 = Find_House(trig, Data.Value);
	if (house2 == NULL) {
		return(false);
	}
	house->Make_Enemy(house2, false);
	house2->Make_Enemy(house, false);
	return(true);
}

// FUNCTION: 0x006E0ED0
bool TActionClass::TAction_PREFERRED_TARGET(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL) {
		house->PreferredTarget = Data.Quarry;
	}
	return(true);
}

// FUNCTION: 0x006E0EF0
bool TActionClass::TAction_BASE_BUILDING(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (Data.Bool) {
		house->IsBaseBuilding = true;
		if (house->ConYards.Count() > 0) {
			BuildingClass *building = house->ConYards[0];
			Cell base_cell = building->Get_Coord().As_Cell();
			house->Set_Base_Spawn_Cell(base_cell);
			house->Base.PlacementCenter = base_cell;
		}
		if (house->Base.Nodes.Count() == 0) {
			house->FUN_0050C210();
		}
	} else {
		house->IsBaseBuilding = false;
	}
	return(true);
}

// FUNCTION: 0x006E0F90
bool TActionClass::TAction_CREEP_SHADOW(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Map.Encroach_Shadow();
	return(true);
}

// FUNCTION: 0x006E0FA0
bool TActionClass::TAction_SET_GLOBAL(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->Set_Global_To(Data.Value, true);
	return(true);
}

// FUNCTION: 0x006E0FC0
bool TActionClass::TAction_CLEAR_GLOBAL(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->Set_Global_To(Data.Value, false);
	return(true);
}

// FUNCTION: 0x006E0FE0
bool TActionClass::TAction_REVEAL_SOME(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (!PlayerPtr->IsVisionary) {
		Cell waypoint = Scen->Get_Waypoint_Cell(Data.Value);
		int height = Map[waypoint].Height + ((Map[waypoint].IsUnderBridge || Map[waypoint].WasUnderBridge) ? BRIDGE_CELL_HEIGHT : 0);
		{
			Coord c1 = Coord(waypoint - Cell(height/2, height/2)) + Coord(0, 0, height * LEVEL_LEPTON_H);
			Map.RevealArea2(&c1, Rule->RevealTriggerRadius, PlayerPtr, false, false, false, true, false);
		}
		{
			Coord c2 = Coord(waypoint - Cell(height/2, height/2)) + Coord(0, 0, height * LEVEL_LEPTON_H);
			Map.RevealArea2(&c2, Rule->RevealTriggerRadius, PlayerPtr, false, false, false, true, true);
		}
	}
	return(true);
}

// FUNCTION: 0x006E1180
bool TActionClass::TAction_REDUCE_TIBERIUM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell cell1 = Scen->Get_Waypoint_Cell(Data.Value);
	Map.Area_Reduce_Tiberium(cell1);
	return(true);
}

// FUNCTION: 0x006E11C0
bool TActionClass::TAction_REVEAL_ZONE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (!PlayerPtr->IsVisionary) {
		int zone = Map.Get_Cell_Zone(Scen->Get_Waypoint_Cell(Data.Value), MZONE_CRUSHER);

		Map.Reset_Local_Iterator();
		CellClass *cellptr = Map.Local_Iterate();

		while (cellptr) {
			if (Map.Get_Cell_Zone(cellptr->CellID, MZONE_CRUSHER) == zone) {
				int height = cellptr->Height/2;
				Coord c1 = Coord(cellptr->CellID - Cell(height/2, height/2)) + Coord(0, 0, height * LEVEL_LEPTON_H);
				Map.RevealArea2(&c1, 2, PlayerPtr, false, false, false, true, false);
				Coord c2 = Coord(cellptr->CellID - Cell(height/2, height/2)) + Coord(0, 0, height * LEVEL_LEPTON_H);
				Map.RevealArea2(&c2, 2, PlayerPtr, false, false, false, true, true);
			}
			cellptr = Map.Local_Iterate();
		}

	}
	return(true);
}

// FUNCTION: 0x006E1330
bool TActionClass::TAction_REVEAL_ALL(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (!PlayerPtr->IsVisionary) {
		PlayerPtr->IsVisionary = true;

		Map.Reset_Iterator();
		CellClass *cellptr = Map.Iterate();

		while (cellptr) {
			Map.Map_Cell(cellptr->CellID, PlayerPtr);
			cellptr = Map.Iterate();
		}
		Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
	}
	return(true);
}

// FUNCTION: 0x006E13A0
bool TActionClass::TAction_START_TIMER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (!Scen->MissionTimer.Is_Active()) {
		Scen->MissionTimer.Start();
		Map.Redraw_Tab();
	}
	return(true);
}

// FUNCTION: 0x006E13E0
bool TActionClass::TAction_STOP_TIMER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Scen->MissionTimer.Is_Active()) {
		Scen->MissionTimer.Stop();
		Map.Redraw_Tab();
	}
	return(true);
}

// FUNCTION: 0x006E1440
bool TActionClass::TAction_ADD_TIMER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->MissionTimer = Scen->MissionTimer + Data.Value * TICKS_PER_SECOND;
	Map.Redraw_Tab();
	return(true);
}

// FUNCTION: 0x006E14A0
bool TActionClass::TAction_SUB_TIMER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Scen->MissionTimer <= Data.Value * TICKS_PER_SECOND) {
		Scen->MissionTimer = 0;
	} else {
		Scen->MissionTimer = Scen->MissionTimer - Data.Value * TICKS_PER_SECOND;
	}
	Map.Redraw_Tab();
	return(true);
}

// FUNCTION: 0x006E1530
bool TActionClass::TAction_SET_TIMER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->MissionTimer = Data.Value * TICKS_PER_SECOND;
	Scen->MissionTimer.Start();
	Scen->MissionTimerTextCSF = DAT_00887734;
	Scen->MissionTimerText = MissionTimerString(DAT_00889F64);
	Map.Redraw_Tab();
	return(true);
}

// FUNCTION: 0x006E15F0
bool TActionClass::TAction_SET_TIMER_TEXT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	char const * text = Text[0] ? Text : NULL;
	MissionTimerString label(text);
	if (text) {
		Scen->MissionTimerTextCSF = StringTable::LoadString(text, NULL, "D:\\ra2mdpost\\TAction.CPP", 2242);
		Scen->MissionTimerText = label;
	} else {
		Scen->MissionTimerTextCSF = DAT_00887734;
		Scen->MissionTimerText = MissionTimerString(DAT_00889F64);
	}
	Map.Redraw_Tab();
	return(true);
}

// FUNCTION: 0x006E16D0
bool TActionClass::TAction_PLAY_MOVIE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_0072DDB0();
	MouseCursor->Release_Mouse();
	FUN_006842F0(false);
	FUN_005BF260(Data.Movie, THEME_NONE, true, true, true);
	FUN_006842F0(true);
	MouseCursor->Capture_Mouse();
	FUN_0072DEF0();
	return(true);
}

// FUNCTION: 0x006E1720
bool TActionClass::TAction_PLAY_INGAME_MOVIE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_005BF390(Data.Movie, false);
	return(true);
}

// FUNCTION: 0x006E1740
bool TActionClass::TAction_PLAY_INGAME_MOVIE_PAUSE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_005BF390(Data.Movie, true);
	return(true);
}

// FUNCTION: 0x006E1760
bool TActionClass::TAction_PLAY_SOUND(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	VocClass::PlayGlobal(Data.Sound, 1.0f, 0x2000);
	return(true);
}

// FUNCTION: 0x006E1780
bool TActionClass::TAction_PLAY_SOUND_RANDOM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell list[100];
	int count = 0;

	for (int index = 0; index < WAYPT_COUNT; index++) {
		if (Scen->Is_Valid_Waypoint(index)) {
			list[count++] = Scen->Get_Waypoint_Cell(index);
			if (count == sizeof(list)) break;
		}
	}

	VocClass::PlayIndexAtPos(Data.Sound, Coord(list[Random_Pick(0, count-1)]), 0);
	return(true);
}

// FUNCTION: 0x006E1830
ObjectClass * __fastcall Coord_Object(Coord const & coord)
{
	Cell cell = Cell(coord);
	ObjectClass * object = NULL;
	if (cell != DAT_00B0E680) {
		CellClass * cellptr = &Map[cell];
		object = cellptr->Cell_Building();
		if (object == NULL) {
			object = (ObjectClass *)cellptr->Cell_Terrain(false);
		}
	}
	return(object);
}

// FUNCTION: 0x006E18B0
bool TActionClass::TAction_PLAY_SOUND_AT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Coord coord = Scen->Get_Waypoint_Coord(EffectLocation);
	ObjectClass * object = Coord_Object(coord);
	if (object != NULL) {
		object->FUN_005F6CB0(Data.Sound);
	} else {
		VocClass::PlayIndexAtPos(Data.Sound, coord, 1);
	}
	return(true);
}

// FUNCTION: 0x006E1980
bool TActionClass::TAction_STOP_SOUND_AT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Coord coord = Scen->Get_Waypoint_Coord(EffectLocation);
	ObjectClass * object = Coord_Object(coord);
	if (object != NULL) {
		object->FUN_005F6CB0(-1);
	} else {
		FUN_00750F40(&coord, 1);
	}
	return(true);
}

// FUNCTION: 0x006E1A40
bool TActionClass::TAction_TELEPORT_HOUSE(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	Cell cell = Scen->Get_Waypoint_Cell(EffectLocation);
	house->FUN_0050D6D0(cell);
	return(true);
}

// FUNCTION: 0x006E1A70
bool TActionClass::TAction_REVEAL_AROUND_WAYPOINT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (!PlayerPtr->IsVisionary) {
		Cell waypoint = Scen->Get_Waypoint_Cell(Data.Value);
		int height = Map[waypoint].Height + ((Map[waypoint].IsUnderBridge || Map[waypoint].WasUnderBridge) ? BRIDGE_CELL_HEIGHT : 0);
		Map.Shroud_Gap_Area(Coord(waypoint - Cell(height/2, height/2)) + Coord(0, 0, height * LEVEL_LEPTON_H), Rule->RevealTriggerRadius);
		FUN_00578100(&Map);
	}
	return(true);
}

// FUNCTION: 0x006E1B90
bool TActionClass::TAction_PLAY_MUSIC(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Theme.Queue_Song(Data.Theme);
	return(true);
}

// FUNCTION: 0x006E1BB0
bool TActionClass::TAction_PLAY_SPEECH(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_00752480(Data.Speech, 2, -1);
	return(true);
}

// FUNCTION: 0x006E1BD0
bool TActionClass::TAction_1_SPECIAL(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	house->SuperWeapon[Data.Special]->Grant(true, false, false);

	if (PlayerPtr == house) {
		Map.AddCameo(RTTI_SPECIAL, Data.Special);
		Map.RepaintSidebar(SidebarClass::GetObjectTabIdx(RTTI_SPECIAL, Data.Special, false));
	}
	return(true);
}

// FUNCTION: 0x006E1C40
bool TActionClass::TAction_FULL_SPECIAL(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	house->SuperWeapon[Data.Special]->Grant(false, false, false);
	house->SuperWeapon[Data.Special]->NeedsBuilding = false;

	if (PlayerPtr == house) {
		Map.AddCameo(RTTI_SPECIAL, Data.Special);
		Map.RepaintSidebar(SidebarClass::GetObjectTabIdx(RTTI_SPECIAL, Data.Special, false));
	}
	return(true);
}

// FUNCTION: 0x006E1CC0
bool TActionClass::TAction_DZ(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord(waypoint);
	coord.Z = Map.Get_Height_GL(coord);
	if ( Map[waypoint].IsUnderBridge || Map[waypoint].WasUnderBridge ) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	AnimClass * anim = new AnimClass(Rule->FlareAnim, coord);
	anim->IsInert = true;
	return(true);
}

// FUNCTION: 0x006E1DC0
bool TActionClass::TAction_WIN(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Data.House == PlayerPtr->Class->House) {
		PlayerPtr->Flag_To_Win();
	} else {
		PlayerPtr->Flag_To_Lose();
	}
	return(true);
}

// FUNCTION: 0x006E1E00
bool TActionClass::TAction_LOSE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Data.House != PlayerPtr->Class->House) {
		PlayerPtr->Flag_To_Win();
	} else {
		PlayerPtr->Flag_To_Lose();
	}
	return(true);
}

// FUNCTION: 0x006E1E40
bool TActionClass::TAction_BEGIN_PRODUCTION(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * h = Find_House(trig, Data.House);
	if (h == NULL) return(false);
	h->Begin_Production();
	return(true);
}

// FUNCTION: 0x006E1EA0
bool TActionClass::TAction_FIRE_SALE(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * h = Find_House(trig, Data.House);
	if (h == NULL) return(false);
	h->State = STATE_ENDGAME;
	return(true);
}

// FUNCTION: 0x006E1F00
bool TActionClass::TAction_AUTOCREATE(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * h = Find_House(trig, Data.House);
	if (h == NULL) return(false);
	h->IsAlerted = true;
	return(true);
}

// FUNCTION: 0x006E1F60
bool TActionClass::TAction_CREATE_TEAM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	ScenarioInit++;
	if (Team != NULL) {
		Team->Create_One_Of(NULL);
	}
	ScenarioInit--;
	return(true);
}

// FUNCTION: 0x006E1F90
bool TActionClass::TAction_DESTROY_TEAM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Team != NULL) {
		Team->Destroy_All_Of();
	}
	return(true);
}

// FUNCTION: 0x006E1FB0
bool TActionClass::TAction_SPAWN_TEAM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Team != NULL) {
		return(FUN_0065D8E0(Team, -1));
	}
	return(false);
}

// FUNCTION: 0x006E1FD0
bool TActionClass::TAction_SPAWN_TEAM_AT_WP(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Team == NULL || EffectLocation == -1) return(false);
	return(FUN_0065D8E0(Team, EffectLocation));
}

// FUNCTION: 0x006E1FF0
bool TActionClass::TAction_ALL_HUNT(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * h = Find_House(trig, Data.House);
	if (h == NULL) return(false);
	h->All_To_Hunt();
	return(true);
}

// FUNCTION: 0x006E2050
bool TActionClass::TAction_DESTROY_OBJECT(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & cell)
{
	bool any = false;
	bool process = true;
	while (process) {
		process = false;
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->Strength > 0 && techno->IsActive && techno->IsDown && !techno->IsInLimbo && techno->Tag != NULL && techno->Tag->Is_Trigger_Attached(trig)) {
				int damage = techno->Strength;
				techno->Take_Damage(damage, 0, Rule->C4Warhead, 0, true);
				any = true;
				process = true;
				index--;
			}
		}
	}

	if (cell != DAT_00B0E680) {
		int tries = 3;
		while (Map.Damage_Bridge(cell) == false && tries-- > 0) {};
		Point2D point;
		Coord coord = Coord(cell);
		coord.Z += BRIDGE_LEPTON_HEIGHT;
		TacticalMap->Coord_To_Pixel(coord, point);
		Rect dirty (point.X-128, point.Y-128, 256, 256);
		TacticalMap->Register_Dirty_Area(dirty);
	}
	return(any);
}

// FUNCTION: 0x006E21D0
bool TActionClass::TAction_NOP(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	return(true);
}

// FUNCTION: 0x006E21E0
bool TActionClass::TAction_RESIZE_PLAYER_VIEW(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Map.LocalRect = TriggerRect;
	Map.Reset_Radar();

	Map.Reset_Iterator();
	CellClass *cptr = Map.Iterate();

	while (cptr) {
		cptr->Recalc_Attributes();
		cptr = Map.Iterate();
	}

	Map.Zone_Reset();
	Map.Reset_All_Subzones();

	Map.Complete_Radar_Refresh();

	for (int i = Buildings.Count() - 1; i >= 0; i--) {
		Buildings[i]->Update_Radar_Position(true);
	}

	return(true);
}

// FUNCTION: 0x006E2290
bool TActionClass::TAction_PLAY_ANIM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord (waypoint);
	coord.Z = Map.Get_Height_GL(coord);
	if ( Map[waypoint].IsUnderBridge || Map[waypoint].WasUnderBridge ) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	AnimClass * anim = new AnimClass(AnimTypes[Data.Anim], coord);
	anim->IsInert = true;
	return(true);
}

// FUNCTION: 0x006E2390
bool TActionClass::TAction_DO_EXPLOSION(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord (waypoint);
	coord.Z = Map.Get_Height_GL(coord);
	if ( Map[waypoint].IsUnderBridge || Map[waypoint].WasUnderBridge ) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	WeaponType weapon = Data.Weapon;
	WeaponTypeClass * ww = Weapons[weapon];
	int damage = ww->Attack;
	AnimTypeClass const * aptr = Combat_Anim(damage, ww->WarheadPtr, Map[coord].Land_Type(), coord);

	if (aptr) {
		new AnimClass(aptr, coord, 0, 1, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ZGRAD), Get_Explosion_Z(coord));
	}

	FUN_0048A620(coord, damage, Weapons[weapon]->WarheadPtr, false, 0);
	FUN_00489280(coord, damage, NULL, Weapons[weapon]->WarheadPtr, true, NULL);
	return(true);
}

// FUNCTION: 0x006E2520
bool TActionClass::TAction_METEOR_IMPACT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord (waypoint);
	coord.Z = Map.Get_Height_GL(coord);
	if ( Map[waypoint].IsUnderBridge || Map[waypoint].WasUnderBridge ) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	new VoxelAnimClass(VoxelAnimTypes[Data.VAnim], coord, NULL);
	return(true);
}

// FUNCTION: 0x006E2600
bool TActionClass::TAction_ION_STORM_START(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (IonStormClass::Is_Ion_Storm_Active()) {
		return(false);
	}
	FUN_00539EB0(Data.Value, Rule->LightningDeferment, DAT_00B0E680, NULL);
	return(true);
}

// FUNCTION: 0x006E2640
bool TActionClass::TAction_ION_STORM_STOP(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (!IonStormClass::Is_Ion_Storm_Active()) {
		return(false);
	}

	IonStormClass::Ion_Storm_End();
	return(true);
}

// FUNCTION: 0x006E2660
bool TActionClass::TAction_CLEAR_MODES(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	DAT_0088098C.Ptr = 0;
	DAT_0088098C.Type = 0;
	DAT_0088098C.House = -1;
	Map.SetActiveFoundation(NULL);
	Map.Repair_Mode_Control(0);
	Map.Sell_Mode_Control(0);
	DAT_0088099A = false;
	DAT_0088099B = false;
	DAT_0088099C = false;
	FUN_00684240();
	return(true);
}

// FUNCTION: 0x006E26C0
bool TActionClass::TAction_UNLOCK_INPUT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Unlock_Scenario_Input();
	return(true);
}

// FUNCTION: 0x006E26D0
bool TActionClass::TAction_CENTER_VIEWPOINT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord (waypoint);
	coord.Z = Map.Get_Height_GL(coord);

	if (Map[waypoint].IsUnderBridge || Map[waypoint].WasUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	TacticalMap->Setup_Trigger_Scroll(coord, Data.Speed);
	return(true);
}

// FUNCTION: 0x006E2790
bool TActionClass::TAction_JUMP_VIEWPOINT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord (waypoint);
	coord.Z = Map.Get_Height_GL(coord);

	if (Map[waypoint].IsUnderBridge || Map[waypoint].WasUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	TacticalMap->Set_Tactical_Position(coord);
	return(true);
}

// FUNCTION: 0x006E2850
bool TActionClass::TAction_CENTER_ON_BASE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_007315A0();
	return(true);
}

// FUNCTION: 0x006E2860
bool TActionClass::TAction_ZOOM_IN(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	TacticalMap->ZoomFactor = Rule->ZoomInFactor;
	UserInputLocked = true;
	InputManagerClass::Instance->DoSomething();

	Hide_Mouse();
	Map.Abort_Drag_Select();
	Map.Flag_To_Redraw();
	Map.Render();

	Sleep(1000);
	return(true);
}

// FUNCTION: 0x006E28E0
bool TActionClass::TAction_ZOOM_OUT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	TacticalMap->ZoomFactor = 1;
	InputManagerClass::Instance->DoSomething();
	UserInputLocked = Scen->IsInputLocked;

	Show_Mouse();
	Map.Flag_To_Redraw();
	Map.Render();

	return(true);
}

// FUNCTION: 0x006E2950
bool TActionClass::TAction_RESHROUD(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	Map.FUN_00577C90(house);
	return(true);
}

// FUNCTION: 0x006E2970
bool TActionClass::TAction_CHANGE_SPOTLIGHT_BEHAVIOR(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	bool success = false;
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * ptr = Buildings[index];
		if (ptr->Strength > 0 &&
				ptr->IsActive &&
				ptr->IsDown &&
				!ptr->IsInLimbo &&
				ptr->Class->HasSpotlight &&
				ptr->BuildingLight != NULL &&
				ptr->Tag != NULL &&
				ptr->Tag->Is_Trigger_Attached(trig)) {
			ptr->BuildingLight->Set_Behavior_Type(Data.LightBehavior);
			success = true;
		}
	}
	return(success);
}

// FUNCTION: 0x006E2A10
bool TActionClass::TAction_DESTROY_TRIGGER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Trigger != NULL) {
		for (int index = Triggers.Count() - 1; index >= 0; index--) {
			if (Triggers[index]->Class == Trigger) {
				Triggers[index]->Mark_To_Delete();
			}
		}
	}
	return(true);
}

// FUNCTION: 0x006E2A50
bool TActionClass::TAction_DESTROY_TAG(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Tag != NULL) {
		int count = Tags.Count();
		for (int index = 0; index < count; index++) {
			if (Tags[index]->Class == Tag) {
				delete Tags[index];
				index--;
			}
		}
	}
	return(true);
}

// FUNCTION: 0x006E2AA0
bool TActionClass::TAction_FORCE_TRIGGER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Trigger != NULL) {
		for (int index = 0; index < Triggers.Count(); index++) {
			if (Triggers[index]->Class == Trigger) {
				Triggers[index]->Spring(NULL, DAT_00B0E680);
			}
		}
	}
	return(true);
}

// FUNCTION: 0x006E2AF0
bool TActionClass::TAction_ENABLE_TRIGGER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Trigger != NULL) {
		for (int index = 0; index < Triggers.Count(); index++) {
			TriggerClass * trig = Triggers[index];
			TriggerTypeClass * type = trig->Class;
			if (type == Trigger) {
				if ((Scen->Difficulty != 0 || type->Difficulty.IsEnabledOnEasy) &&
						(Scen->Difficulty != 1 || type->Difficulty.IsEnabledOnMedium) &&
						(Scen->Difficulty != 2 || type->Difficulty.IsEnabledOnHard)) {
					trig->Enable();
				}
			}
		}
	}
	return(true);
}

// FUNCTION: 0x006E2B70
bool TActionClass::TAction_DISABLE_TRIGGER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Trigger != NULL) {
		for (int index = 0; index < Triggers.Count(); index++) {
			if (Triggers[index]->Class == Trigger) {
				Triggers[index]->Disable();
			}
		}
	}
	return(true);
}

// FUNCTION: 0x006E2BB0
bool TActionClass::TAction_RADAR_EVENT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Submit_Radar_Event(Data.RadarEvent, Scen->Get_Waypoint_Cell(EffectLocation));
	return(true);
}

// FUNCTION: 0x006E2BE0
bool TActionClass::TAction_SET_LOCAL(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->Set_Local_To(Data.Value, true);
	return(true);
}

// FUNCTION: 0x006E2C00
bool TActionClass::TAction_CLEAR_LOCAL(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->Set_Local_To(Data.Value, false);
	return(true);
}

// FUNCTION: 0x006E2C20
bool TActionClass::TAction_CLEAR_SMUDGES(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_00588AC0(&Map);
	Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
	return(true);
}

// FUNCTION: 0x006E2C40
bool TActionClass::TAction_METEOR_SHOWER(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	static const int _scale = 70;

	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord (waypoint);
	coord.Z = Map.Get_Height_GL(coord);
	if ( Map[waypoint].IsUnderBridge || Map[waypoint].WasUnderBridge ) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	int count = DAT_00842AFC[Data.MeteorShower] + abs(Scen->RandomNumber) % 3;
	AnimType met_large = Anim_From_Name("METLARGE");
	AnimType met_small = Anim_From_Name("METSMALL");

	int distance = count * _scale;
	for (int i = 0; i < count; i++) {
		Coord meteor_coord = coord + Coord(Scen->RandomNumber % distance, Scen->RandomNumber % distance);
		meteor_coord.Z = Map.Get_Height_GL(meteor_coord);

		new AnimClass(AnimTypes[abs(Scen->RandomNumber) % 2 ? met_large : met_small], meteor_coord);
	}
	return(true);
}

// FUNCTION: 0x006E2E20
bool TActionClass::TAction_SET_AMBIENT_STEP(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Rule->AmbientLightChangeStep = (double)Data.Float;
	return(true);
}

// FUNCTION: 0x006E2E40
bool TActionClass::TAction_SET_AMBIENT_RATE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Rule->AmbientLightChangeRate = (double)Data.Float;
	return(true);
}

// FUNCTION: 0x006E2E60
bool TActionClass::TAction_SET_TINT_RED(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_0053AD00(Data.Value * 10, Scen->NormalLighting.GreenTint * 10, Scen->NormalLighting.BlueTint * 10, 0);
	Scen->NormalLighting.RedTint = Data.Value;
	return(true);
}

// FUNCTION: 0x006E2EB0
bool TActionClass::TAction_SET_TINT_GREEN(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_0053AD00(Scen->NormalLighting.RedTint * 10, Data.Value * 10, Scen->NormalLighting.BlueTint * 10, 0);
	Scen->NormalLighting.GreenTint = Data.Value;
	return(true);
}

// FUNCTION: 0x006E2F00
bool TActionClass::TAction_SET_TINT_BLUE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_0053AD00(Scen->NormalLighting.RedTint * 10, Scen->NormalLighting.GreenTint * 10, Data.Value * 10, 0);
	Scen->NormalLighting.BlueTint = Data.Value;
	return(true);
}

// FUNCTION: 0x006E2F50
bool TActionClass::TAction_SET_AMBIENT_LIGHT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->AmbientLight = Data.Value;
	if (!FUN_0053A100()) {
		Scen->DesiredAmbientLight = Scen->AmbientLight;
	}
	return(true);
}

// FUNCTION: 0x006E2F90
bool TActionClass::TAction_SET_CHRONO_SCREEN_EFFECT(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	FUN_0053B460(Data.Value);
	return(true);
}

// FUNCTION: 0x006E2FA0
bool TActionClass::TAction_BEGIN_AI_TRIGGERS(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) return(false);
	hptr->IsAITriggersOn = true;
	return(true);
}

// FUNCTION: 0x006E3000
bool TActionClass::TAction_STOP_AI_TRIGGERS(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) return(false);
	hptr->IsAITriggersOn = false;
	return(true);
}

// FUNCTION: 0x006E3060
bool TActionClass::TAction_CHEER(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) return(false);
	hptr->Cheer();
	return(true);
}

// FUNCTION: 0x006E30C0
bool TActionClass::TAction_RESTORE_STARTING_UNITS(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) return(false);
	hptr->RespawnStartingForces();
	return(true);
}

// FUNCTION: 0x006E3120
bool TActionClass::TAction_RESTORE_STARTING_BUILDINGS(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) return(false);
	hptr->RespawnStartingBuildings();
	return(true);
}

// FUNCTION: 0x006E3180
bool TActionClass::TAction_DESTROY_ALL_OBJECTS(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) return(false);
	hptr->Destroy_All();
	return(true);
}

// FUNCTION: 0x006E31E0
bool TActionClass::TAction_DESTROY_ALL_BUILDINGS(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) return(false);
	hptr->Destroy_All_Buildings();
	return(true);
}

// FUNCTION: 0x006E3240
bool TActionClass::TAction_DESTROY_ALL_NON_NAVAL_UNITS(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) return(false);
	hptr->Destroy_All_Non_Buildings_Non_Naval();
	return(true);
}

// FUNCTION: 0x006E32A0
bool TActionClass::TAction_DESTROY_ALL_NAVAL_UNITS(HouseClass * , ObjectClass * , TriggerClass * trig, Cell const & )
{
	HouseClass * hptr = Find_House(trig, Data.Value);
	if (hptr == NULL) return(false);
	hptr->Destroy_All_Non_Buildings_Naval();
	return(true);
}

// FUNCTION: 0x006E3300
bool TActionClass::TAction_SET_AI_TRIGGER_TEAM_RATIO(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL) {
		house->RatioAITriggerTeam = Data.Value;
	}
	return(true);
}

// FUNCTION: 0x006E3320
bool TActionClass::TAction_SET_TEAM_AIRCRAFT_RATIO(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL) {
		house->RatioTeamAircraft = Data.Value;
	}
	return(true);
}

// FUNCTION: 0x006E3340
bool TActionClass::TAction_SET_TEAM_INFANTRY_RATIO(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL) {
		house->RatioTeamInfantry = Data.Value;
	}
	return(true);
}

// FUNCTION: 0x006E3360
bool TActionClass::TAction_SET_TEAM_UNIT_RATIO(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL) {
		house->RatioTeamUnits = Data.Value;
	}
	return(true);
}

// FUNCTION: 0x006E3380
bool TActionClass::TAction_ION_CANNON(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord (waypoint);
	coord.Z = Map.Get_Height_GL(coord);

	new IonBlastClass(coord);
	return(true);
}

// FUNCTION: 0x006E35F0
bool TActionClass::TAction_LIGHTNING_STORM(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord (waypoint);
	coord.Z = Map.Get_Height_GL(coord);
	if (Map[waypoint].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	Cell cell = coord.As_Cell();
	if (!FUN_0053A100()) {
		FUN_00539EB0(Rule->LightningStormDuration, Rule->LightningDeferment, cell, house);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E37F0
bool TActionClass::TAction_SET_TECH_LEVEL(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	for (int index = TechnoTypes.Count() - 1; index >= 0; index--) {
		TechnoTypeClass * type = TechnoTypes[index];
		if (TechnoID == type->IniName) {
			type->Level = Data.Value;
			break;
		}
	}
	for (int house_index = Houses.Count() - 1; house_index >= 0; house_index--) {
		Houses[house_index]->IsRecalcNeeded = true;
	}
	return(true);
}

// FUNCTION: 0x006E3890
bool TActionClass::TAction_CHRONO_IN_TEAM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Team != NULL && EffectLocation != -1) {
		return(FUN_0065EC30(Team, EffectLocation));
	}
	return(false);
}

// FUNCTION: 0x006E38B0
bool TActionClass::TAction_CREATE_CRATE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell cell = Scen->Get_Waypoint_Cell(EffectLocation);
	return(Map.PlacePowerupCrate(cell, Data.Value));
}

// FUNCTION: 0x006E38F0
bool TActionClass::TAction_CHEM_MISSILE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Cell waypoint = Scen->Get_Waypoint_Cell(EffectLocation);
	Coord coord = Coord (waypoint);
	coord.Z = Map.Get_Height_GL(coord);
	if (Map[waypoint].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	Cell closest = Map.Closest_Edge_Cell(coord.As_Cell());

	WeaponTypeClass * weapon_type = Weapons[WeaponTypeClass::From_Name("ChemLauncher")];
	BulletTypeClass const * bullet_type = weapon_type->Bullet;
	BulletClass * bullet = FUN_0046B050(bullet_type, &Map[coord], NULL, weapon_type->Attack, weapon_type->WarheadPtr, 20, false);

	if (bullet != NULL) {
		TVelocity3D<double> velocity(DirType(DIR_N), DirType(DIR_E), 100);
		bullet->Unlimbo(closest, velocity);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E3B20
bool TActionClass::TAction_TOGGLE_TRAIN_CARGO(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Scen->IsTrainCargo = (Scen->IsTrainCargo == false);
	return(true);
}

// FUNCTION: 0x006E3B40
bool TActionClass::TAction_RADAR_BLACKOUT(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house == NULL) {
		return(false);
	}
	house->Create_Radar_Outage(Data.Value);
	return(true);
}

// FUNCTION: 0x006E3B60
int TActionClass::GetMode(TActionType action)
{
	switch (action) {
		case 11: case 103:
			return(0x27);

		case 33: case 34: case 134:
			return(0x11);

		case 1: case 2: case 3: case 6: case 9: case 13: case 14: case 36: case 37: case 38: case 74: case 75: case 113: case 119: case 120: case 121: case 122: case 123: case 124: case 126: case 130:
			return(0xd);

		case 30: case 85: case 86: case 87:
			return(0x10);

		case 4: case 5: case 7: case 105:
			return(0xc);

		case 104:
			return(0x28);

		case 80: case 107:
			return(0x22);

		case 88:
			return(0x23);

		case 12: case 22: case 53: case 54:
			return(0xb);

		case 20:
			return(0x1);

		case 10: case 100: case 117:
			return(0x2);

		case 19: case 98:
			return(0x3);

		case 21:
			return(0x4);

		case 70:
			return(0x1a);

		case 106: case 115:
			return(0x2b);

		case 129: case 132: case 133:
			return(0x2e);

		case 28: case 29:
			return(0x16);

		case 56: case 57:
			return(0x1d);

		case 35:
			return(0xf);

		case 40:
			return(0x15);

		case 41:
			return(0x17);

		case 99:
			return(0x26);

		case 8: case 17: case 18: case 59: case 63: case 64: case 65: case 66: case 89: case 90: case 94: case 95: case 96: case 101: case 102: case 109: case 112: case 116: case 128: case 135: case 137: case 140:
			return(0x9);

		case 42:
			return(0x18);

		case 43:
			return(0x1b);

		case 48:
			return(0x1e);

		case 25: case 26: case 27: case 39: case 44: case 73: case 76: case 77: case 78: case 79: case 84: case 110: case 114: case 127: case 139: case 142: case 143: case 144:
			return(0xa);

		case 52:
			return(0x19);

		case 55:
			return(0x1c);

		case 58:
			return(0x1f);

		case 71: case 72:
			return(0x21);

		case 108:
			return(0x2c);

		case 125:
			return(0x2d);

		case 131:
			return(0x2f);

		default:
			break;
	}
	return(0);
}

// FUNCTION: 0x006E3D70
HRESULT STDMETHODCALLTYPE TActionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ActionClass;
	return(S_OK);
}

// FUNCTION: 0x006E3DB0
HRESULT STDMETHODCALLTYPE TActionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TActionClass(NoInitClass());

		Swizzle_Pointer(&Next);
		Swizzle_Pointer(&Team);
		Swizzle_Pointer(&Tag);
		Swizzle_Pointer(&Trigger);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006E3E30
HRESULT STDMETHODCALLTYPE TActionClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006E3E50
void TActionClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	if (Next != NULL) crc(Next->Fetch_ID());
	crc(Action);
	if (Team != NULL) crc(Team->Fetch_ID());
	if (Tag != NULL) crc(Tag->Fetch_ID());
	if (Trigger != NULL) crc(Trigger->Fetch_ID());
	crc((int)Data.Value);
}

// FUNCTION: 0x006E3EE0
int __fastcall Action_Flags(int action)
{
	int flags = 0;
	switch (action) {
		case 14: case 32: case 60: case 61: case 62: case 91: case 111:
			flags = 2;
			break;
		default:
			break;
	}
	return(flags);
}

// FUNCTION: 0x006E3F70
Coord TActionClass::Waypoint_As_Coord(void)
{
	return(Scen->Get_Waypoint_Coord(Data.Value));
}

// FUNCTION: 0x006E3FB0
void TActionClass::Set_Text(char const * text)
{
	Text[0] = '\0';
	if (text != NULL) {
		while (*text != '\0' && (unsigned char)*text <= ' ') {
			text++;
		}
		if (*text != '\0') {
			strncpy(Text, text, sizeof(Text) - 1);
			Text[sizeof(Text) - 1] = '\0';
			int length = strlen(Text);
			while (length != 0 && (unsigned char)Text[length - 1] <= ' ') {
				length--;
			}
			Text[length] = '\0';
		}
	}
}

// FUNCTION: 0x006E4010
char * TActionClass::Get_Text(void)
{
	return(Text[0] != '\0' ? Text : NULL);
}

// FUNCTION: 0x006E4020
bool TActionClass::TAction_FLASH_TEAM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Team != NULL) {
		Team->Flash(Data.Value);
	}
	return(true);
}

// FUNCTION: 0x006E4040
bool TActionClass::TAction_TALK_BUBBLE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (Team != NULL) {
		TeamClass * team = Team->Find_First_Of_Type();
		if (team != NULL) {
			FUN_0070F120(team->Member, (TalkType)Data.Value);
		}
	} else {
		FUN_0070F120(NULL, TALK_NONE);
	}
	return(true);
}

// FUNCTION: 0x006E4080
bool TActionClass::TAction_PAUSE_SCENARIO(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	Hold_Scenario(Data.Value);
	return(true);
}

// FUNCTION: 0x006E4090
bool TActionClass::TAction_EJECT_OCCUPANTS(HouseClass * , ObjectClass * , TriggerClass * trigger, Cell const & )
{
	bool result = false;
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		if (building->IsActive && building->IsDown && !building->IsInLimbo && building->Tag != NULL && building->Tag->Is_Trigger_Attached(trigger)) {
			building->FUN_00457DE0(false, false);
			result = true;
			break;
		}
	}
	return(result);
}

// FUNCTION: 0x006E4100
bool TActionClass::TAction_SWITCH_TAB(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	int tab = Data.Value;
	if (tab < 4 && tab >= 0 && Map.Column[tab].FUN_006ABAC0()) {
		Map.SetTab(tab);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E4150
bool TActionClass::TAction_FlashCameo(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	int found = -1;
	for (int index = TechnoTypes.Count() - 1; index >= 0; index--) {
		if (TechnoID == TechnoTypes[index]->IniName) {
			found = index;
			break;
		}
	}
	if (found < TechnoTypes.Count() && found >= 0) {
		Map.FUN_006AC5F0(TechnoTypes[found], Data.Value);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E4200
bool TActionClass::TAction_CreateBuildingAt(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL) {
		Cell cell = Scen->Get_Waypoint_Cell(EffectLocation);
		Coord coord = Map[cell].Center_Coord();
		int type = BuildingTypeClass::From_Name(TechnoID);
		if (type >= 0 && type <= BuildingTypes.Count() && BuildingTypes[type] != NULL) {
			BuildingClass * building = (BuildingClass *)BuildingTypes[type]->Create_One_Of(house);
			if (building != NULL) {
				if (!building->ForceCreate(coord, 0)) {
					building->Delete_Me();
				} else {
					building->Grand_Opening(false);
					return(true);
				}
			}
		}
	}
	return(false);
}

// FUNCTION: 0x006E42D0
bool TActionClass::TAction_SetSWChargeLevel(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL && Data.Value >= 0 && Data.Value <= house->SuperWeapon.Count()) {
		SuperClass * super = house->SuperWeapon[Data.Value];
		if (super->IsPresent && Value2 >= 0 && Value2 <= 100) {
			super->SetCharge(Value2);
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x006E4320
bool TActionClass::TAction_SetSWRechargeTime(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL && Data.Value >= 0 && Data.Value <= house->SuperWeapon.Count()) {
		house->SuperWeapon[Data.Value]->SetRechargeTime(Value2);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E4360
bool TActionClass::TAction_ResetSWRechargeTime(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL && Data.Value >= 0 && Data.Value <= house->SuperWeapon.Count()) {
		house->SuperWeapon[Data.Value]->ResetRechargeTime();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E43A0
bool TActionClass::TAction_ResetSW(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house != NULL && Data.Value >= 0 && Data.Value <= house->SuperWeapon.Count()) {
		house->SuperWeapon[Data.Value]->Reset();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x006E43E0
bool TActionClass::TAction_SetPreferredTargetCell(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house == NULL) {
		return(false);
	}
	Cell cell = Scen->Get_Waypoint_Cell(EffectLocation);
	if (cell == CELL_NONE) {
		return(false);
	}
	house->FUN_0050DA00(cell);
	return(true);
}

// FUNCTION: 0x006E4440
bool TActionClass::TAction_ResetTargetCell(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house == NULL) {
		return(false);
	}
	house->FUN_0050DA10();
	return(true);
}

// FUNCTION: 0x006E4460
bool TActionClass::TAction_SetDefenseCell(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house == NULL) {
		return(false);
	}
	Cell cell = Scen->Get_Waypoint_Cell(EffectLocation);
	if (cell == CELL_NONE) {
		return(false);
	}
	house->FUN_0050DA20(cell);
	return(true);
}

// FUNCTION: 0x006E44C0
bool TActionClass::TAction_ResetDefenseCell(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house == NULL) {
		return(false);
	}
	house->FUN_0050DA50();
	return(true);
}

// FUNCTION: 0x006E44E0
bool TActionClass::TAction_SetBaseCenter(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house == NULL) {
		return(false);
	}
	Cell cell = Scen->Get_Waypoint_Cell(EffectLocation);
	if (cell == CELL_NONE) {
		return(false);
	}
	house->FUN_0050DFE0(cell);
	return(true);
}

// FUNCTION: 0x006E4540
bool TActionClass::TAction_ResetBaseCenter(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house == NULL) {
		return(false);
	}
	house->FUN_0050DFF0();
	return(true);
}

#undef Buildings

// FUNCTION: 0x006E4560
bool TActionClass::TAction_FlashBuildingsOfType(HouseClass * house, ObjectClass * , TriggerClass * , Cell const & )
{
	if (house == NULL) {
		return(false);
	}
	for (int index = house->Buildings.Count() - 1; index >= 0; index--) {
		BuildingClass * building = house->Buildings[index];
		if (TechnoID == building->Class->IniName) {
			building->Flashing.FlashCount = Data.Value;
		}
	}
	return(true);
}

// FUNCTION: 0x006E45E0
HouseClass * TActionClass::FindHouseByIndex(TriggerClass * trig, int index) const
{
	if (trig == NULL) {
		return(NULL);
	}
	return(Find_House(trig, index));
}

// FUNCTION: 0x006E4630 TActionClass::Fetch_Object_Size
// FUNCTION: 0x006E4640 TActionClass::Fetch_RTTI
// FUNCTION: 0x006E4650 TActionClass::Fetch_Heap_ID
// FUNCTION: 0x006E4660 TActionClass::~TActionClass

// FUNCTION: 0x006E4790 ?$TStringID@$0BP@::?$TStringID@$0BP@
// FUNCTION: 0x006E47C0 ?$TStringID@$0BP@::operator=
template struct TStringID<31>;

// FUNCTION: 0x006E47F0 ?$DynamicVectorClass@PAVTActionClass@@::Clear
// FUNCTION: 0x006E4820 ?$DynamicVectorClass@PAVTActionClass@@::ID
// FUNCTION: 0x006E4840 ?$VectorClass@PAVTActionClass@@::GetItem
template class DynamicVectorClass<TActionClass *>;
