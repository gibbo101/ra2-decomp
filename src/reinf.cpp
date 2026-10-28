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

#include "ilocos.h"
#include "incdec.h"
#include "inline.h"

// Can every object in the chain travel by tunnel?
inline bool _Can_Burrow(FootClass * object)
{
	while (object != NULL) {
		TechnoTypeClass const * tclass = object->Techno_Type_Class();
		if (tclass->Locomotor != CLSID_TunnelLocomotion) {
			return(false);
		}
		object = (FootClass *)object->Next;
	}
	return(true);
}

// FUNCTION: GAMEMD 0x0065E010
bool Create_Reinforcement(TeamTypeClass const * teamtype, FootClass * object, Cell const & xcell, bool atwaypoint)
{
	bool drop_pod = false;
	if (teamtype->IsDroppod) {
		drop_pod = true;
	}

	bool burrow = _Can_Burrow(object);

	FacingType eface = FACING_N;

	Cell cell = xcell;

	if (!drop_pod && !burrow && !atwaypoint) {
		SourceType source = teamtype->Get_House() ? teamtype->Get_House()->Control.Edge : SOURCE_NONE;
		if (source == SOURCE_NONE) {
			source = SOURCE_NORTH;
		}

		eface = (FacingType)(source << 1);
	} else {
		cell = Map.Nearby_Location(cell, object->Techno_Type_Class()->Speed);
	}

	Cell newcell = cell;

	FootClass * o = (FootClass *)object->Next;
	object->Next = NULL;
	bool okvoice = false;
	if (newcell != CELL_NONE) {
		while (newcell != CELL_NONE && object != NULL) {
			DirType desiredfacing(eface);
			if (object->RTTI == RTTI_AIRCRAFT) {
				desiredfacing = DirType(DirType(eface).As_Int() - DirType(FACING_135).As_Int() - 1).Snap_To_8();
			}

			ScenarioInit++;

			bool placed = false;

			if (drop_pod) {
				object->PositionCoord = newcell;
				object->Assign_Destination(&Map[newcell]);
				object->Locomotion->Move_To(newcell.As_Coord());
				object->UpdateSight(false, false, false, NULL, 0);
				Coord c = object->Position;
				Map.RevealArea3(c, 0, object->LastSightRange + 3, 0);
				placed = true;
			} else if (burrow) {
				Coord coord = Map[newcell].Cell_Coord() - Coord(0, 0, 400);
				placed = object->Unlimbo(coord, desiredfacing.As_Dir256());
				if (placed) {
					object->PositionCoord += Coord(0, 0, -CELL_LEPTON - object->Height);
					object->Assign_Destination(&Map[newcell]);
					object->Set_Speed(1);
					object->Locomotion->Move_To(newcell.As_Coord());
				}
			} else {
				placed = object->Unlimbo(Map[newcell].Cell_Coord(), desiredfacing.As_Dir256());
			}

			if (placed) {
				okvoice = true;

				if (object->RTTI != RTTI_AIRCRAFT) {
					object->QueueMission(MISSION_GUARD, false);
					object->Commence();
				}

				if (drop_pod) {
					FacingType adj;
					for (adj = FACING_N; adj < FACING_COUNT; adj++) {
						Cell trycell = Cell(newcell + AdjacentCell[(int)adj & 7]);
						if (Map.In_Radar(trycell)) {
							newcell = trycell;
							break;
						}
					}
					if (adj >= FACING_COUNT) {
						newcell = CELL_NONE;
					}
				}

			} else {
				FacingType adj;
				for (adj = FACING_N; adj < FACING_COUNT; adj++) {
					Cell trycell = Cell(newcell + AdjacentCell[(int)adj & 7]);
					if (!Map.In_Radar(trycell) && object->Can_Enter_Cell(&Map[trycell], adj) == MOVE_OK) {
						newcell = trycell;
						break;
					}
				}

				if (adj >= FACING_COUNT) {
					newcell = CELL_NONE;
				} else if (newcell != CELL_NONE) {
					ScenarioInit--;
					continue;
				}

				delete object;
			}

			ScenarioInit--;

			object = o;
			if (object != NULL) {
				o = (FootClass *)(ObjectClass *)object->Next;
				object->Next = NULL;
			}
		}

	} else if (object != NULL) {
		delete object;
	}

	if (o != NULL) {
		while (o != NULL) {
			FootClass * old = o;
			o = (FootClass *)(ObjectClass *)o->Next;
			old->Next = 0;

			delete old;
		}
	}

	return(okvoice);
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

#include "ipiggy.h"
#include "_rules.h"
#include "rules.h"

namespace LocomotionClass { namespace CLSIDs { extern const CLSID Teleport; } }
void __fastcall FUN_00660B80(Cell const & cell);

// FUNCTION: GAMEMD 0x0065EC30
bool __fastcall Do_Chrono_Reinforcements(TeamTypeClass const * teamtype, WAYPOINT wp)
{
	if (!teamtype || !teamtype->TaskForce->ClassCount) return(false);

	ScriptTypeClass * script = teamtype->Script;
	if (!script || script->MissionCount == 0) {
		TeamMissionClass tmission(TMISSION_GUARD, 0);
		if (!script) {
			((TeamTypeClass *)teamtype)->Script = new ScriptTypeClass(NULL);
		}
		script = teamtype->Script;
		int count = script->MissionCount;
		if (count < MAX_TEAM_MISSIONS) {
			script->MissionList[count] = tmission;
			script->MissionCount++;
		}
	}

	FootClass * object = _Create_Group(teamtype);
	object->Team->IsDeleteTypeWhenDone = false;
	if (!object) {
		return(false);
	}

	Cell origin = teamtype->Get_Origin();
	if (wp != -1) {
		origin = Scen->Get_Waypoint_Cell(wp);
	}

	FootClass * next = (FootClass *)(ObjectClass *)object->Next;
	object->Next = NULL;
	FootClass * placed = NULL;

	while (object != NULL) {
		bool isunit = false;
		bool isair = false;
		if (object != NULL) {
			if (object->Fetch_RTTI() == RTTI_UNIT) isunit = true;
			if (object->Fetch_RTTI() == RTTI_AIRCRAFT) isair = true;
		}

		Coord coord = Map[origin].Center_Coord();
		if (!isunit && !isair) {
			coord.X += ((object->Ammo & 3) - 2) << 8;
			coord.Y += ((object->Ammo & 0xC) - 4) << 6;
		}
		if (!isair) {
			object->Ammo--;
		}

		Coord dest = COORD_NONE;
		CellClass * cellptr = &Map[coord];
		int tries = 0;
		while (dest == COORD_NONE && tries < 255) {
			Cell nearcell;
			if (isair) {
				nearcell = Map.Nearby_Location(Cell(cellptr->Center_Coord()), SPEED_TRACK);
			} else {
				nearcell = Map.Nearby_Location(Cell(cellptr->Center_Coord()), object->Techno_Type_Class()->Speed);
			}
			cellptr = &Map[nearcell];
			if (!isunit && !isair) {
				dest = cellptr->Closest_Free_Spot(cellptr->Center_Coord());
			} else {
				dest = cellptr->Center_Coord();
			}
			tries++;
		}

		coord.X = dest.X;
		coord.Y = dest.Y;
		ScenarioInit++;
		int ok = object->Unlimbo(coord, DIR_N);
		ScenarioInit--;
		if (ok) {
			IPiggybackPtr piggy(object->Locomotion);
			ILocomotionPtr teleport(LocomotionClass::CLSIDs::Teleport);
			teleport->Link_To_Object(object);
			piggy = IPiggybackPtr(teleport);
			if (piggy != NULL) {
				piggy->Begin_Piggyback(object->Locomotion);
				object->Locomotion = teleport;
			}
			object->ChronoDestCoords = coord;
			object->Locomotion->Move_To(coord);
			object->WarpingOut = true;
			object->ChronoLockRemaining = Rule->ChronoReinfDelay;
			object->Next = placed;
			placed = object;
			object = next;
			if (object != NULL) {
				next = (FootClass *)(ObjectClass *)object->Next;
				object->Next = NULL;
			}
		} else {
			delete object;
			object = next;
			if (object != NULL) {
				next = (FootClass *)(ObjectClass *)object->Next;
				object->Next = NULL;
			}
		}
	}

	object = placed;
	placed = (FootClass *)(ObjectClass *)object->Next;
	object->Next = NULL;
	while (object != NULL) {
		object->Mark(MARK_UP);
		object->unknown_280 = 3;
		object->Commence();
		object = placed;
		if (object != NULL) {
			placed = (FootClass *)(ObjectClass *)object->Next;
			object->Next = NULL;
		}
	}

	if (teamtype->Get_House() && teamtype->Get_House()->Is_Ally(PlayerPtr)) {
		FUN_00660B80(origin);
	}
	return(true);
}
