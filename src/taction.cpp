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
void __fastcall FUN_0048A620(int damage, WarheadTypeClass const * warhead, Coord coord, int a, int b);
char const * __fastcall FUN_00751590(int index);
int __fastcall FUN_00753250(char const * name);

HouseClass * __fastcall FUN_00726910(TriggerClass * trig);
bool __fastcall FUN_00510F60(int index);
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

Coord TActionClass::Waypoint_As_Coord(void)
{
	return(Scen->Get_Waypoint_Coord(Data.Value));
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
	FUN_0048A620(50, warhead, Waypoint_As_Coord(), 0, 0);
	return(true);
}

// FUNCTION: 0x006E07F0
bool TActionClass::TAction_LIGHT_MEDIUM(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	WarheadTypeClass const * warhead = Rule->C4Warhead;
	FUN_0048A620(100, warhead, Waypoint_As_Coord(), 0, 0);
	return(true);
}

// FUNCTION: 0x006E0850
bool TActionClass::TAction_LIGHT_LARGE(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	WarheadTypeClass const * warhead = Rule->C4Warhead;
	FUN_0048A620(300, warhead, Waypoint_As_Coord(), 0, 0);
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

// FUNCTION: 0x006E2640
bool TActionClass::TAction_ION_STORM_STOP(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (!IonStormClass::Is_Ion_Storm_Active()) {
		return(false);
	}

	IonStormClass::Ion_Storm_End();
	return(true);
}

// FUNCTION: 0x006E3D70
HRESULT STDMETHODCALLTYPE TActionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ActionClass;
	return(S_OK);
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
