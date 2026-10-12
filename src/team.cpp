// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "team.h"

#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_tactica.h"
#include "aircraft.h"
#include "airctype.h"
#include "aitrig.h"
#include "anim.h"
#include "building.h"
#include "cell.h"
#include "crc.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "map.h"
#include "mission.h"
#include "mono.h"
#include "movie.h"
#include "rules.h"
#include "scenario.h"
#include "script.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "target.h"
#include "taskforc.h"
#include "teamtype.h"
#include "theme.h"
#include "tmission.h"
#include "tracker.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "voc.h"
#include "vox.h"

#include "target.hh"
#include "tube.hh"

static inline CellClass * _As_Cell(AbstractClass * target)
{
	return((target == NULL) ? NULL : ((target->RTTI == RTTI_CELL) ? (CellClass *)target : NULL));
}

static inline int _Coord_Distance(Coord const & a, Coord const & b)
{
	Coord t = a - b;
	return(t.Length());
}

static inline bool _Is_It_Breathing(FootClass const * object)
{
	if (object == NULL || !object->IsActive || object->Strength == 0) return(false);
	if (!ScenarioInit && object->IsInLimbo) return(false);
	return(true);
}

static inline bool _Is_It_Playing(FootClass const * object)
{
	if (!_Is_It_Breathing(object)) return(false);
	if (!object->IsInitiated && object->RTTI != RTTI_AIRCRAFT) return(false);
	return(true);
}

bool TeamClass::Is_A_Member(void const * who) const
{
	FootClass * unit = Member;
	while (unit != NULL) {
		if (unit == who) {
			return(true);
		}
		unit = unit->Member;
	}
	return(false);
}

FootClass * TeamClass::Fetch_A_Leader(void) const
{
	FootClass * m = Member;
	FootClass * leader = m;
	int best = -1;
	for (; m != NULL; m = m->Member) {
		int rating = m->TClass->LeadershipRating;
		if (_Is_It_Playing(m) && rating > best) {
			leader = m;
			best = rating;
		}
	}
	return(leader);
}

static inline bool _Conscript(TeamClass * team, FootClass * unit)
{
	if (_Is_It_Breathing(unit) && !unit->IsInitiated) {
		TeamMissionType action = team->Script->Get_Current_Mission().Mission;
		int stray = (action == (TeamMissionType)0x36 || action == (TeamMissionType)0x35) ? Rule->RelaxedStray : Rule->StrayDistance;
		if (unit->Distance(team->Zone) > stray) {
			if (unit->NavCom == NULL) {
				unit->QueueMission(MISSION_MOVE, false);
				unit->Assign_Target(NULL);
				unit->Assign_Destination(team->Zone);
			}
			return(true);
		} else {
			unit->IsInitiated = true;
		}
	}
	return(false);
}

bool TeamClass::Coordinate_Conscript(FootClass * unit)
{
	return(_Conscript(this, unit));
}

// FUNCTION: 0x006E8A90
TeamClass::TeamClass(TeamTypeClass const * type, HouseClass * owner, void * unknown) :
	BASECLASS(),
	Class((TeamTypeClass *)type),
	House(owner),
	IsForcedActive(false),
	IsHasBeen(false),
	IsFullStrength(false),
	IsUnderStrength(true),
	IsReforming(false),
	IsLagging(false),
	IsAltered(true),
	JustAltered(false),
	IsMoving(false),
	IsNextMission(true),
	TargetNotAssigned(false),
	IsLeaveMap(false),
	Suspended(false),
	Succeeded(false),
	Zone(NULL),
	ClosestMember(NULL),
	MissionTarget(NULL),
	Target(NULL),
	Total(0),
	Risk(0),
	SuspendTimer(0),
	TimeOut(0),
	Member(0),
	Script(NULL),
	HouseToScout(NULL),
	UnusedPtr1(unknown),
	CreationFrame(Frame),
	Tag(NULL),
	IsDeleteTypeWhenDone(false),
	IsRegrouping(false),
	NeedsReinforcement(false)
{
	Teams.Add(this);
	AbstractTypePtrTracker.Add(this);
	TagPtrTracker.Add(this);
	ObjectPtrTracker.Add(this);
	NeuronPtrTracker.Add(this);

	memset(Quantity, 0, sizeof(Quantity));
	if (Class != NULL) {
		if (House != NULL && Class->IsBaseDefense) {
			House->BaseDefenseTeamCount++;
		}
		if (Class->Get_Origin() != CELL_NONE) {
			Zone = &Map[Class->Get_Origin()];
		}
		Class->Number++;
		if (Class->Tag) {
			Tag = new TagClass(Class->Tag);
		}
		Script = new ScriptClass(Class->Script);
	}
}

// FUNCTION: 0x006E8DE0
TeamClass::~TeamClass(void)
{
	for (int i = 0; i < AITriggerTypes.Count(); i++) {
		AITriggerTypeClass *ptr = AITriggerTypes[i];
		if (ptr->Get_First_TeamType() == Class) {
			if (Succeeded) {
				ptr->Record_Success();
			} else {
				ptr->Record_Failure();
			}
		}
	}

	if (House != NULL && Class != NULL && Class->IsBaseDefense) {
		House->BaseDefenseTeamCount--;
	}

	if (GameActive && Class != NULL) {
		while (Member != NULL) {
			Remove(Member);
		}
		Class->Number--;
		if (Tag != NULL) {
			if (Tag->AttachCount == 0) {
				Tag->Mark_To_Delete();
			}
			Tag = NULL;
		}
	}
	Detach_This_From_All(this);

	NeuronPtrTracker.Delete(this);
	ObjectPtrTracker.Delete(this);
	AbstractTypePtrTracker.Delete(this);
	TagPtrTracker.Delete(this);
	Teams.Delete(this);

	if (IsDeleteTypeWhenDone) {
		delete Class;
	}
}

// FUNCTION: 0x006E9050
void TeamClass::Assign_Mission_Target(AbstractClass * new_target)
{
	if (new_target != MissionTarget) {
		FootClass * unit = Member;
		if (MissionTarget != NULL) {
			while (unit != NULL) {
				bool tar = (unit->TarCom == MissionTarget);
				bool nav = (unit->NavCom == MissionTarget);
				if (tar || nav) {
					unit->QueueMission(MISSION_GUARD, false);
					if (nav) {
						unit->Assign_Destination(NULL);
					}
					if (tar) {
						unit->Assign_Target(NULL);
					}
				}
				unit = unit->Member;
			}
		}
	}

	if (Target == MissionTarget || Target == NULL) {
		MissionTarget = Target = new_target;
	} else {
		MissionTarget = new_target;
	}

	if (Is_Target_Cell(MissionTarget)) {
		if (!Map.In_Local_Radar((CellClass *)new_target)) {
			FootClass * unit = Member;
			IsLeaveMap = true;
			while (unit != NULL) {
				unit->Assign_Destination(NULL);
				unit = unit->Member;
			}
		} else {
			IsLeaveMap = false;
		}
	}
}

// FUNCTION: 0x006EA080
void TeamClass::Flag_Into_Action(void)
{
	IsMoving = true;
	IsHasBeen = true;
	IsUnderStrength = false;

	FootClass * member = Member;
	while (member != NULL) {
		if (IsReforming || IsForcedActive) {
			member->IsInitiated = true;
		}
		member = member->Member;
	}

	Script->Stop_Script();
	IsNextMission = true;
}

// FUNCTION: 0x006EA0D0
void TeamClass::Regroup(void)
{
	IsMoving = false;
	Script->Stop_Script();
	if (Total > 0) {
		Calc_Center(Zone, ClosestMember);
		if (Zone == NULL) {
			if (ClosestMember == NULL) {
				return;
			}
			Zone = ClosestMember;
		}

		Cell dest = Zone->Center_Coord().As_Cell();
		int max = 0x7FFFFFFF;

		int count = House->Buildings.Count();
		for (int index = 0; index < count; index++) {
			BuildingClass * b = House->Buildings[index];

			if (b != NULL && !b->IsInLimbo && b->PrimaryWeapon == NULL) {
				Cell cell = b->Center_Coord().As_Cell();
				int dist = b->Distance(Zone->Center_Coord()) * (Map.Cell_Threat(cell, *House) + 1);

				if (b->Class->IsCanUnitRepair) {
					dist /= 2;
				}
				if (dist < max) {
					Cell zone_cell = Zone->Center_Coord().As_Cell();
					FootClass * leader = Fetch_A_Leader();
					cell = leader->Safety_Point(zone_cell, cell, 2, 4);
					if (cell != CELL_NONE) {
						max = dist;
						dest = cell;
					}
				}
			}
		}

		Target = &Map[dest];
		Coordinate_Move();
	} else {
		Zone = NULL;
	}
}

// FUNCTION: 0x006EA3E0
bool TeamClass::Recalc_Strength(void)
{
	bool old_under = IsUnderStrength;
	int desired = Class->TaskForce->Required_Object_Count();

	if (Total > 0) {
		IsFullStrength = (Total == desired);
		if (IsFullStrength) {
			IsHasBeen = true;
		}

		if (Class->IsReinforcable) {
			if (desired > 2) {
				IsUnderStrength = (Total <= desired / 3);
			} else {
				IsUnderStrength = (Total < desired);
			}
		} else {
			IsUnderStrength = !IsHasBeen;
		}

		if (Class->IsGuardSlower) {
			if (IsUnderStrength) {
				NeedsReinforcement = false;
			} else {
				NeedsReinforcement = true;
			}
		}

		IsAltered = JustAltered = false;
	} else {
		NeedsReinforcement = false;
		IsUnderStrength = true;
		IsFullStrength = false;
		Zone = NULL;

		if (IsHasBeen) {
			if (IsLeaveMap) {
				for (int index = LogicTags.Count() - 1; index >= 0; index--) {
					TagClass * tag = LogicTags[index];
					if (tag->Spring(TEVENT_LEAVES_MAP)) {
						if (LogicTags.Count() == 0) break;
					}
				}
			}
			delete this;
			return(false);
		}
	}

	if (old_under != IsUnderStrength) {
		IsReforming = true;
	}
	return(true);
}

// FUNCTION: 0x006EA4F0
bool TeamClass::Add(FootClass * obj)
{
	return(AddMember(obj, true));
}

// FUNCTION: 0x006EA500
bool TeamClass::AddMember(FootClass * obj, bool flag)
{
	if (!obj) return(false);

	int typeindex;
	if (!Can_Add(obj, typeindex, flag)) return(false);

	if (obj->Team != NULL) {
		obj->Team->Remove(obj);
	}

	if (!flag) {
		Quantity[typeindex]++;
	}
	obj->IsInitiated = (Member == NULL);
	obj->Member = Member;
	Member = obj;
	obj->Team = this;
	obj->Group = Class->Get_Group();

	if (Tag != NULL && (!Class->OnTransOnly || obj->TClass->MaxPassengers > 0)) {
		obj->Attach_Tag(Tag);
	}

	Total++;
	Risk += obj->Risk();
	if (Zone == NULL) {
		Calc_Center(Zone, ClosestMember);
	}

	IsAltered = JustAltered = true;
	obj->IsAutocreateRecruitable = Class->AreTeamMembersRecruitable;
	return(true);
}

// FUNCTION: 0x006EA610
bool TeamClass::Can_Add(FootClass * obj, int & typeindex, bool flag) const
{
	if (this == obj->Team) {
		return(false);
	}

	if (!_Is_It_Breathing(obj) || obj->House != House) {
		return(false);
	}

	if (obj->HasAnyLink()) {
		if (obj->RTTI != RTTI_AIRCRAFT || !((AircraftClass *)obj)->Class->IsAirportBound) {
			return(false);
		}
	}

	for (typeindex = 0; typeindex < Class->TaskForce->ClassCount; typeindex++) {
		if (Class->TaskForce->Members[typeindex].Class == obj->Class_Of()) {
			break;
		}
	}

	if (typeindex == Class->TaskForce->ClassCount && !flag) {
		return(false);
	}

	if (obj->Mission != MISSION_NONE && !MissionClass::Is_Recruitable_Mission(obj->Mission)) {
		return(false);
	}

	if (!obj->IsTeamRecruitable && !Class->IsAutocreate) {
		return(false);
	}

	bool forced = false;
	if (Script != NULL && Script->Get_Current_Mission().Mission == TMISSION_UNLOAD) {
		forced = true;
	}
	if (obj->RTTI == RTTI_AIRCRAFT) {
		forced = true;
	}

	if (!obj->IsAutocreateRecruitable && Class->IsAutocreate && !forced) {
		return(false);
	}

	if (obj->Is_Entering_Structure()) {
		return(false);
	}

	if (obj->Is_Draining()) {
		return(false);
	}

	if (obj->BunkerLinkedItem != NULL) {
		return(false);
	}

	if (obj->Team != NULL && (obj->Team->Class->RecruitPriority >= Class->RecruitPriority)) {
		return(false);
	}

	if (obj->RTTI == RTTI_AIRCRAFT && obj->PrimaryWeapon != NULL && !obj->Ammo) {
		return(false);
	}

	if (Quantity[typeindex] >= Class->TaskForce->Members[typeindex].Quantity && !flag) {
		return(false);
	}

	if (obj->BunkerLinkedItem != NULL) {
		return(false);
	}
	if (obj->LocomotorSource != NULL) {
		return(false);
	}
	return(true);
}

// FUNCTION: 0x006EA870
bool TeamClass::Remove(FootClass * obj, int typeindex, bool flag)
{
	obj->IsDroppedFromTeam = true;

	if (this != obj->Team) {
		return(true);
	}

	if (obj->Tag == Tag) {
		HouseClass *hptr = obj->House;
		if (hptr != NULL && !hptr->Is_Human_Player()) {
			obj->Attach_Tag(NULL);
		}
	}

	if (typeindex == -1 && Class != NULL && Class->TaskForce != NULL) {
		for (typeindex = 0; typeindex < Class->TaskForce->ClassCount; typeindex++) {
			if (Class->TaskForce->Members[typeindex].Class == obj->Class_Of()) {
				break;
			}
		}
	}

	if (Class != NULL && Class->TaskForce != NULL && (unsigned)typeindex < (unsigned)Class->TaskForce->ClassCount) {
		Quantity[typeindex]--;
	}

	bool initiated = false;
	FootClass * prev = 0;
	FootClass * curr = Member;
	bool found = false;
	while (curr != NULL && (!found || !initiated)) {
		if (curr == obj) {
			if (prev != NULL) {
				prev->Member = curr->Member;
			} else {
				Member = curr->Member;
			}
			FootClass * temp = curr->Member;
			curr->Member = 0;
			curr->Team = 0;
			curr->SuspendedMission = MISSION_NONE;
			curr->SuspendedNavCom = NULL;
			curr->SuspendedTarCom = NULL;
			curr = temp;
			Total--;
			found = true;
			Risk -= obj->Risk();
			continue;
		}

		if (initiated) {
			initiated = true;
		} else {
			initiated = false;
			if (curr->IsInitiated) {
				initiated = true;
			}
		}

		prev = curr;
		curr = curr->Member;
	}

	if (obj->Team != NULL) {
		obj->Team = NULL;
	}

	if (!ScenarioInit && obj->IsActive && !obj->IsCrashing && !obj->IsInLimbo && !flag) {
		obj->Enter_Idle_Mode();
	}

	if (!initiated && Member != NULL) {
		Member->IsInitiated = true;
		Zone = NULL;
	}

	IsAltered = JustAltered = true;
	return(true);
}

// FUNCTION: 0x006EAE60
void TeamClass::Detach(AbstractClass const * target, bool all)
{
	if (target == Tag) {
		Tag = NULL;
	}

	if (target == Member) {
		if (all) {
			Member = Member->Member;
		}
	}

	if (target == House) {
		House = NULL;
	}

	if (target == (AbstractClass *)Script) {
		Script = NULL;
	}

	if (Target == target) {
		Target = NULL;
	}
	if (MissionTarget == target) {
		MissionTarget = NULL;
	}

	if (Zone == target) {
		Zone = NULL;
	}

	if (Class == (TeamTypeClass *)target) {
		Class = NULL;
	}

	if (ClosestMember == (TeamTypeClass *)target) {
		ClosestMember = NULL;
	}

	if ((AbstractClass *)UnusedPtr1 == target) {
		UnusedPtr1 = NULL;
	}

	if (HouseToScout == target) {
		HouseToScout = NULL;
	}
}

// FUNCTION: 0x006EAEE0
void TeamClass::Calc_Center(AbstractClass *& center, AbstractClass *& close_member) const
{
	close_member = NULL;
	center = NULL;

	FootClass * team_member = Member;
	if (team_member == NULL) return;

	if (Script->Get_Current_Mission().Mission == TMISSION_HOUND_DOG) {
		if (!team_member) return;

		FootClass * closest = NULL;
		int distance = -1;

		for (int unit_index = 0; unit_index < Units.Count(); unit_index++) {
			FootClass * trial_unit = Units[unit_index];

			if (_Is_It_Breathing(trial_unit) && trial_unit->House->Is_Ally(House) && this != trial_unit->Team) {
				int trial_distance = team_member->Distance_To(trial_unit);

				if (distance == -1 || trial_distance < distance) {
					distance = trial_distance;
					closest = trial_unit;
				}
			}
		}

		for (int infantry_index = 0; infantry_index < Infantry.Count(); infantry_index++) {
			FootClass * trial_infantry = Infantry[infantry_index];

			if (_Is_It_Breathing(trial_infantry) && trial_infantry->House->Is_Ally(House) && this != trial_infantry->Team) {
				int trial_distance = team_member->Distance_To(trial_infantry);

				if (distance == -1 || trial_distance < distance) {
					distance = trial_distance;
					closest = trial_infantry;
				}
			}
		}

		if (closest) {
			if (closest->CurrentTube >= TUBE_FIRST) {
				Cell exit = Tubes[closest->CurrentTube]->Exit;
				center = &Map[exit];
			} else {
				center = closest;
			}
			close_member = Member;
		}

	} else {

		int x = 0;
		int y = 0;
		int dist = 0;
		int quantity = 0;
		FootClass * closest = 0;

		while (team_member != NULL) {
			if (_Is_It_Playing(team_member) && team_member->IsInPlayfield) {
				x += team_member->PositionCoord.X;
				y += team_member->PositionCoord.Y;
				quantity++;

				if (Class->IsGuardSlower && team_member->Is_Considered_Slow()) {
					x += team_member->PositionCoord.X;
					y += team_member->PositionCoord.Y;
					quantity++;
				}

				if (!(team_member->TClass->MaxPassengers > 0 && team_member->TClass->Naval)) {
					int try_dist = team_member->Relative_Distance(Target);
					if (!dist || try_dist < dist) {
						dist = try_dist;
						closest = team_member;
					}
				}
			}

			team_member = team_member->Member;
		}

		if (quantity) {
			x /= quantity;
			y /= quantity;
			Coord coord = Coord((int)x, (int)y);
			center = &Map[coord];

			if (closest == NULL) {
				closest = Member;
			}
			if (closest->Can_Enter_Cell((CellClass *)center) != MOVE_OK) {
				center = closest;
			}
		}

		if (closest != NULL) {
			close_member = closest;
		}
	}
}

// FUNCTION: 0x006EB380
void TeamClass::Took_Damage(FootClass *, ResultType result, TechnoClass * source)
{
	if ((source != NULL) && (result != RESULT_NONE) && (!Class->IsSuicide)) {
		if (!IsMoving) {
			Zone = NULL;
			IsRegrouping = true;
			IsReforming = true;
		} else {
			if (source && !Is_A_Member(source) && Member && Member->RTTI != RTTI_AIRCRAFT && Member->PrimaryWeapon != NULL) {
				if (Target != source) {
					if (Class->IsAnnoyance) {
						IsRegrouping = true;
						IsReforming = true;
						Zone = NULL;
					}
					if (Target != NULL) {
						TechnoClass * techno = Dynamic_Cast<TechnoClass *>(Target);
						if (techno != NULL && techno->PrimaryWeapon != NULL) {
							if (Zone == NULL || techno->IsCloseEnoughToAttackCoords(Zone->Center_Coord())) {
								return;
							}
						}
					}
					source->Fetch_RTTI();
				}
			}
		}
	}
}

// FUNCTION: 0x006EB490
void TeamClass::Coordinate_Attack(void)
{
	if (Target == NULL) {
		Target = MissionTarget;
	}

	FootClass * leader = Fetch_A_Leader();
	if (Is_Target_Cell(Target) && Member != NULL && leader->RTTI != RTTI_AIRCRAFT) {
		CellClass * cellptr = _As_Cell(Target);
		if (cellptr->Cell_Object()) {
			Target = cellptr->Cell_Object();
		}
	}

	if (Frame % 8 == 4) {
		if (leader->Can_Fire(Target, leader->What_Weapon_Should_I_Use(Target), true) == (FireErrorType)5) {
			TargetNotAssigned = true;
		}
	}

	if (Target == NULL) {
		IsNextMission = true;
	} else {
		TeamMissionClass mission = Script->Get_Current_Mission();
		bool has_attacker = false;
		FootClass * unit = Member;
		while (unit != NULL) {
			Coordinate_Conscript(unit);

			if (_Is_It_Playing(unit)) {
				if (mission.Mission == TMISSION_SPY && unit->RTTI == RTTI_INFANTRY && ((InfantryClass *)unit)->Class->IsCapture) {
					unit->QueueMission(MISSION_CAPTURE, false);
					unit->Assign_Target(Target);
				} else {
					if (unit->Mission != MISSION_ATTACK && unit->Mission != MISSION_ENTER && unit->Mission != MISSION_CAPTURE && unit->Mission != MISSION_SABOTAGE && (unit->Mission != MISSION_UNLOAD || !unit->Deploy_To_Fire())) {
						unit->Transmit_Message(RADIO_OVER_OUT);
						unit->QueueMission(MISSION_ATTACK, false);
						unit->Assign_Target(NULL);
						unit->Assign_Destination(NULL);
					}
				}

				if (unit->TarCom != Target && unit->TarCom == NULL) {
					unit->Assign_Target(Target);
				}

				if (unit->RTTI != RTTI_AIRCRAFT || unit->PrimaryWeapon == NULL || unit->Ammo > 0) {
					has_attacker = true;
				}
			} else if (Class->IsDroppod && unit->IsInLimbo) {
				has_attacker = true;
			}

			unit = unit->Member;
		}
		if (!has_attacker) {
			IsNextMission = true;
		}
	}
}

// FUNCTION: 0x006EB870
bool TeamClass::Coordinate_Regroup(void)
{
	FootClass * unit = Member;
	bool retval = true;

	while (unit != NULL) {
		Coordinate_Conscript(unit);

		if (_Is_It_Playing(unit)) {
			TeamMissionType action = Script->Get_Current_Mission().Mission;
			int stray = (action == (TeamMissionType)0x36 || action == (TeamMissionType)0x35) ? Rule->RelaxedStray : Rule->StrayDistance;
			if (unit->Distance(Zone) > stray) {
				if (unit->Mission == MISSION_GUARD_AREA && unit->TarCom != NULL) {
					goto hold;
				}
				if (unit->NavCom == NULL) {
					retval = false;
					unit->QueueMission(MISSION_MOVE, false);
					Cell dest = unit->Adjust_Dest(Zone->Center_Coord().As_Cell());
					unit->Assign_Destination(&Map[dest]);
				}
			} else {
hold:
				if (unit->Mission != MISSION_GUARD_AREA) {
					unit->QueueMission(MISSION_GUARD, false);
					unit->Assign_Destination(NULL);
				}
			}
		}

		unit = unit->Member;
	}
	if (retval == true) {
		IsRegrouping = false;
	}
	return(retval);
}

// FUNCTION: 0x006EBAD0
void TeamClass::Coordinate_Move(void)
{
	FootClass * unit = Member;
	bool finished = true;
	bool found = false;

	if (unit == NULL) return;

	if (Target == NULL) {
		Target = MissionTarget;
	}

	if (Target != NULL) {

		if (!Lagging_Units()) {

			while (unit != NULL) {

				if (Coordinate_Conscript(unit)) {
					finished = false;
				}

				if (unit->Mission == MISSION_UNLOAD || unit->MissionQueue == MISSION_UNLOAD) {
					finished = false;
				}

				if (_Is_It_Playing(unit) && unit->Mission != MISSION_UNLOAD && unit->MissionQueue != MISSION_UNLOAD) {
					TeamMissionType action = Script->Get_Current_Mission().Mission;
					int stray = (action == (TeamMissionType)0x36 || action == (TeamMissionType)0x35) ? Rule->RelaxedStray : Rule->StrayDistance;
					if (unit->In_Air()) {
						stray *= 2;
					}

					found = true;

					int dist = unit->Distance(Target);

					if (dist > stray ||
						(unit->HeightAGL < 0 && Script->Get_Next_Mission().Mission != TMISSION_MOVE) ||
						(unit->RTTI == RTTI_AIRCRAFT && unit->Height > 0 &&
						&Map[unit->Center_Coord()] != Target &&
						Script->Get_Next_Mission().Mission != TMISSION_MOVE)) {

						if (Class->IsAggressive && unit->TarCom != NULL) {
							if (!unit->unknown_bool_68E) {
								unit = unit->Member;
								continue;
							}
							unit->Assign_Target(NULL);
						}
						if (unit->Mission != MISSION_MOVE) {
							unit->QueueMission(MISSION_MOVE, false);
							if (unit->Ready_To_Next_Mission()) {
								unit->Commence();
							}
						}

						if (unit->NavCom == NULL) {
							unit->Assign_Destination(Target);
						}
						AbstractClass * nav = unit->NavCom;
						if (nav != Target) {
							if (unit->TClass->BalloonHover || (nav != Target && unit->RTTI == RTTI_AIRCRAFT && nav == unit->Get_Cell_Ptr())) {
								unit->Assign_Destination(Target);
							}
						}
						finished = false;
					} else {
						if (unit->Mission == MISSION_MOVE && (unit->NavCom == NULL || (unit->Distance(unit->NavCom) <= Rule->CloseEnoughDistance && !unit->Locomotion->Is_Moving()))) {
							if (unit->TarCom == NULL) {
								unit->Assign_Destination(NULL);
								unit->Enter_Idle_Mode();
							}
						}
					}

					bool at_dest = (unit->RTTI == RTTI_AIRCRAFT && unit->NavCom == unit->Get_Cell_Ptr());
					bool hovering = ((unit->AbstractFlags & 1) && unit->TClass->BalloonHover && dist < stray);
					if (unit->NavCom != NULL && !at_dest && !hovering) {
						finished = hovering;
					}
				}

				unit = unit->Member;
			}
		} else {
			finished = false;
		}
	}

	if (!found) {
		finished = false;
	}

	if (finished && IsMoving) {
		IsNextMission = true;
	}
}

// FUNCTION: 0x006EBF50
bool TeamClass::Lagging_Units(void)
{
	FootClass * unit = Member;
	bool lag = false;

	if (!IsLagging) return(false);

	while (unit != NULL) {

		if (_Is_It_Playing(unit)) {
			TeamMissionType action = Script->Get_Current_Mission().Mission;
			int stray = (action == (TeamMissionType)0x36 || action == (TeamMissionType)0x35) ? Rule->RelaxedStray : Rule->StrayDistance;
			if (unit->RTTI == RTTI_AIRCRAFT) {
				stray *= 3;
			}
			if (Class->IsGuardSlower && !unit->Is_Considered_Slow()) {
				stray /= 3;
			}

			if (_Coord_Distance(unit->Center_Coord(), ClosestMember->Center_Coord()) > stray) {
				if (unit->NavCom == NULL) {
					unit->QueueMission(MISSION_MOVE, false);
					unit->Assign_Destination(ClosestMember);
				}
				lag = true;

			} else {
				if (unit->Mission != MISSION_GUARD) {
					unit->QueueMission(MISSION_GUARD, false);
					unit->Assign_Destination(NULL);
				}
			}
		}
		unit = unit->Member;
	}

	IsLagging = lag;
	return(lag);
}

// FUNCTION: 0x006EC130 TeamClass::Coordinate_Conscript

// FUNCTION: 0x006EC220 TeamClass::Is_A_Member

// FUNCTION: 0x006EC250
void TeamClass::Suspend_Teams(int priority, HouseClass const * house)
{
	for (int index = 0; index < Teams.Count(); index++) {
		TeamClass * team = Teams[index];

		if (team != NULL && house == team->House && team->Class->RecruitPriority < priority) {
			while (team->Member) {
				team->Remove(team->Member);
			}
			team->IsAltered = team->JustAltered = true;
			team->SuspendTimer = Rule->SuspendDelay * TICKS_PER_MINUTE;
			team->Suspended = true;
		}
	}
}

// FUNCTION: 0x006EC300
bool TeamClass::Is_Leaving_Map(void) const
{
	if (IsMoving && Script->Has_Missions_Remaining()) {
		TeamMissionClass const mission = Script->Get_Current_Mission();

		if (mission.Mission == TMISSION_MOVE && !Map.In_Local_Radar(Scen->Get_Waypoint_Cell(mission.Data.Value))) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x006EC370
bool TeamClass::Has_Entered_Map(void) const
{
	bool ok = true;
	FootClass * foot = Member;
	while (foot != NULL) {
		if (!foot->IsInPlayfield) {
			ok = false;
			break;
		}
		foot = (FootClass *)foot->Member;
	}
	return(ok);
}

// FUNCTION: 0x006EC3A0
void TeamClass::Scan_Limit(void)
{
	Assign_Mission_Target(NULL);
	FootClass * foot = Member;
	while (foot != NULL) {
		foot->Assign_Target(NULL);
		foot->IsScanLimited = true;
		foot = foot->Member;
	}
}

// FUNCTION: 0x006EC3D0 TeamClass::Fetch_A_Leader

// FUNCTION: 0x006EC450
HRESULT STDMETHODCALLTYPE TeamClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TeamClass(NoInitClass());

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&Script);
		Swizzle_Pointer(&House);
		Swizzle_Pointer(&Tag);
		Swizzle_Pointer(&Member);
		Swizzle_Pointer(&ClosestMember);
		Swizzle_Pointer(&Target);
		Swizzle_Pointer(&Zone);
		Swizzle_Pointer(&MissionTarget);
		Swizzle_Pointer(&HouseToScout);
		Swizzle_Pointer(&UnusedPtr1);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006EC540
HRESULT STDMETHODCALLTYPE TeamClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006EC770
void TeamClass::TMission_MOVECELL(TeamMissionClass * mission, bool first_time)
{
	if (first_time) {
		Cell cell;
		cell.X = mission->Data.Value % 128;
		cell.Y = mission->Data.Value / 128;
		Assign_Mission_Target(&Map[cell]);
	}
	Coordinate_Move();
}

// FUNCTION: 0x006EDEF0
void TeamClass::TMission_PLAY_MUSIC(TeamMissionClass * mission, bool)
{
	Theme.Queue_Song(mission->Data.Theme);
	IsNextMission = true;
}
