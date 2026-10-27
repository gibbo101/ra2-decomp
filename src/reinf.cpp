// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"

#include "_map.h"
#include "aircraft.h"
#include "airctype.h"
#include "building.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "airstrike.h"
#include "house.h"
#include "infatype.h"
#include "infantry.h"
#include "scenario.h"
#include "script.h"
#include "taskforc.h"
#include "team.h"
#include "teamtype.h"
#include "techno.h"
#include "techtype.h"
#include "unit.h"
#include "unittype.h"

// SYNTHETIC: GAMEMD 0x0065D6C0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0065D6F0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0065D710 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0065D730 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0065D750 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0065D770 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0065D790 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0065D7D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0065D800 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0065D830 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0065D860 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0065D870 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0065D890 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0065D8B0 SYMBOL
// _$E41

class FootClass;
class TechnoClass;

// FUNCTION: GAMEMD 0x0065D8D0
bool FUN_0065D8D0(void)
{
	return false;
}

// FUNCTION: GAMEMD 0x0065DD30
FootClass * _Create_Group(TeamTypeClass const * teamtype)
{
	TeamClass * team = new TeamClass(teamtype, teamtype->Get_House());
	if (team != NULL) {
		team->Force_Active();
	}

	bool hasunload = false;
	ScriptTypeClass * script = teamtype->Script;
	int count = script->MissionCount;
	for (int tm = 0; tm < count; tm++) {
		TeamMissionClass mission = script->MissionList[tm];
		if (mission.Mission == TMISSION_UNLOAD) {
			hasunload = true;
			break;
		}
	}

	bool has_air_transport = team->Has_Air_Transport();

	bool anynaval = false;
	for (int i = 0; i < teamtype->TaskForce->ClassCount; i++) {
		if (teamtype->TaskForce->Members[i].Class->Naval) {
			anynaval = true;
			break;
		}
	}

	FootClass * transport = NULL;
	FootClass * object = NULL;
	for (int index = 0; index < teamtype->TaskForce->ClassCount; index++) {
		TechnoTypeClass const * tclass = teamtype->TaskForce->Members[index].Class;

		for (int sub = 0; sub < teamtype->TaskForce->Members[index].Quantity; sub++) {
			ScenarioInit++;
			FootClass * temp = (FootClass *)tclass->Create_One_Of(team->House);
			ScenarioInit--;

			if (temp != NULL) {
				if (tclass->Capacity > 0 && teamtype->IsFull) {
					temp->Storage.Increase_Amount(tclass->Capacity, 0);
				}

				switch (teamtype->VeteranLevel) {
					case 0:
						temp->Veterancy.Set_Dumbass(true);
						break;
					case 2:
						temp->Veterancy.Set_Veteran(true);
						break;
					case 3:
						temp->Veterancy.Set_Elite(true);
						break;
				}

				if (team != NULL) {
					ScenarioInit++;
					team->AddMember(temp, false);
					ScenarioInit--;
					temp->IsInitiated = true;
				}

				if (tclass->Max_Passengers() > 0 && teamtype->IsFull) {
					if (has_air_transport) {
						if (temp->RTTI == RTTI_AIRCRAFT) goto transporter;
					} else {
						if (temp->RTTI == RTTI_UNIT) {
transporter:
							if (tclass->Naval || !anynaval) {
								temp->Next = transport;
								transport = temp;
								continue;
							}
						}
					}
				}

				temp->Next = object;
				object = temp;
			}
		}
	}

	if (transport != NULL && object != NULL && teamtype->IsFull) {
		transport->Cargo.Attach(object);
		if (hasunload && transport->RTTI == RTTI_AIRCRAFT) {
			transport->IsALoaner = true;
		}
	}

	if (transport == NULL && object == NULL) {
		if (team != NULL) delete team;
		return NULL;
	}

	if (transport == NULL) {
		return object;
	}
	return transport;
}

// FUNCTION: GAMEMD 0x0065E650
bool Create_Special_Reinforcement(HouseClass * house, TechnoTypeClass const * type, TechnoTypeClass const * another, TeamMissionType mission, int argument)
{
	return false;
}

int __fastcall FUN_0050DA80(HouseClass * house);

// FUNCTION: GAMEMD 0x0065E660
int __fastcall Create_Paradrop_Planes(HouseClass * house, int air, int number, MissionType mission, AbstractClass * tarcom, AbstractClass * navcom, int passenger, int passengers)
{
	int sub;
	TechnoTypeClass const * type = (TechnoTypeClass *)AircraftTypes[air];

	for (sub = 0; sub < number; sub++) {
		ScenarioInit++;
		TechnoClass * obj = (TechnoClass *)type->Create_One_Of(house);
		ScenarioInit--;
		if (!obj) return sub;

		obj->IsALoaner = true;

		int edge = house->Control.Edge;
		if (edge < 0 || edge > 3) edge = FUN_0050DA80(house);
		Cell newcell = Map.Calculated_Cell((SourceType)edge, CELL_NONE, CELL_NONE, SPEED_WINGED, true, MZONE_NORMAL);

		obj->QueueMission(mission, false);
		if (navcom != NULL) {
			obj->Assign_Destination(navcom, true);
		}
		if (tarcom != NULL) {
			obj->Assign_Target(tarcom);
		}

		ScenarioInit++;
		int placed = obj->Unlimbo(Coord(newcell), DIR_N);
		ScenarioInit--;
		if (!placed) {
			if (obj) delete obj;
			return sub - 1;
		}

		if (obj->RTTI == RTTI_AIRCRAFT && passenger != -1 && passengers != 0) {
			((AircraftClass *)obj)->Passenger = true;
			InfantryTypeClass * itype = InfantryTypes[passenger];
			if (itype != NULL) {
				int remaining = passengers;
				do {
					ObjectClass * inf = itype->Create_One_Of(house);
					if (inf) {
						inf->Limbo();
						obj->Cargo.Attach((FootClass *)inf);
					}
				} while (--remaining != 0);
			}
		}
		obj->Commence();
	}
	return sub;
}

// FUNCTION: GAMEMD 0x0065E850
int __fastcall Create_Airstrike_Planes(HouseClass * house, int air, int number, MissionType mission, AbstractClass * tarcom, AbstractClass * navcom, int passenger, int passengers, AirstrikeClass * sender)
{
	TechnoTypeClass const * type = (TechnoTypeClass *)AircraftTypes[air];

	int edge = house->Control.Edge;
	if (edge < 0 || edge > 3) edge = FUN_0050DA80(house);
	Cell edgecell = Map.Calculated_Cell((SourceType)edge, CELL_NONE, CELL_NONE, SPEED_WINGED, true, MZONE_NORMAL);

	int sub;
	for (sub = 0; sub < number; sub++) {
		ScenarioInit++;
		TechnoClass * obj = (TechnoClass *)type->Create_One_Of(house);
		ScenarioInit--;
		if (!obj) break;

		obj->Airstrike = sender;
		obj->IsALoaner = true;
		FootClass * foot = (obj->AbstractFlags & 4) ? (FootClass *)obj : NULL;
		if (sender != NULL && foot != NULL) {
			foot->Member = sender->FirstObject;
			sender->FirstObject = foot;
		}

		Cell newcell = Map.Nearby_Location(edgecell, SPEED_WINGED, -1, MZONE_NORMAL, false, Point2D(1, 1), false, false, false, true, Cell(0, 0), false, false);

		obj->QueueMission(mission, false);
		if (navcom != NULL) {
			obj->Assign_Destination(navcom, true);
		}
		if (tarcom != NULL) {
			obj->Assign_Target(tarcom);
		}

		ScenarioInit++;
		int placed = obj->Unlimbo(Coord(newcell), DIR_N);
		ScenarioInit--;
		if (!placed) {
			if (obj) delete obj;
			return sub - 1;
		}

		if (obj->RTTI == RTTI_AIRCRAFT && passenger != -1 && passengers != 0) {
			((AircraftClass *)obj)->Passenger = true;
			InfantryTypeClass * itype = InfantryTypes[passenger];
			if (itype != NULL) {
				int remaining = passengers;
				do {
					ObjectClass * inf = itype->Create_One_Of(house);
					if (inf) {
						inf->Limbo();
						obj->Cargo.Attach((FootClass *)inf);
					}
				} while (--remaining != 0);
			}
		}
		obj->Commence();
	}
	return sub;
}

// FUNCTION: GAMEMD 0x0065EAB0
int __fastcall Create_Spy_Planes(HouseClass * house, int air, int number, MissionType mission, AbstractClass * tarcom, AbstractClass * navcom)
{
	TechnoTypeClass const * type = (TechnoTypeClass *)AircraftTypes[air];

	int sub;
	for (sub = 0; sub < number; sub++) {
		ScenarioInit++;
		TechnoClass * obj = (TechnoClass *)type->Create_One_Of(house);
		ScenarioInit--;
		if (!obj) return sub;

		obj->IsALoaner = true;

		int edge = house->Control.Edge;
		if (edge < 0 || edge > 3) edge = FUN_0050DA80(house);
		Cell newcell = Map.Calculated_Cell((SourceType)edge, CELL_NONE, CELL_NONE, SPEED_WINGED, true, MZONE_NORMAL);

		obj->QueueMission(mission, false);
		if (navcom != NULL) {
			obj->Assign_Destination(navcom, true);
		}
		if (tarcom != NULL) {
			obj->Assign_Target(tarcom);
		}

		ScenarioInit++;
		int placed = obj->Unlimbo(Coord(newcell), DIR_N);
		ScenarioInit--;
		if (!placed) {
			delete obj;
			return sub - 1;
		}

		obj->Commence();
	}
	return sub;
}
