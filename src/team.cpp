// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
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

ThreatType __fastcall FUN_00645BB0(QuarryType quarry);
bool __fastcall FUN_005224E0(FootClass * infantry);
bool __fastcall FUN_0053A100(void);
void __fastcall LightningStorm_Start(int duration, int deferment, int center, bool flag);
void __cdecl LightningStorm_RequestStop(void);
extern int DAT_00B0E948;
void __fastcall FUN_005BF260(VQType movie, ThemeType theme, bool a, bool b, bool c);

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

// FUNCTION: 0x006EC560
HRESULT STDMETHODCALLTYPE TeamClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TeamClass;
	return(S_OK);
}

// FUNCTION: 0x006EC5A0
void TeamClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(Class->Fetch_ID());

	crc(House->Fetch_ID());

	if (ClosestMember != NULL) {
		crc(ClosestMember->Fetch_ID());
	}

	crc(Total);
	crc(Risk);

	if (Member != NULL) {
		crc(Member->Fetch_ID());
	}

	crc((int)TimeOut);
	crc((int)SuspendTimer);

	if (Tag != NULL) {
		crc(Tag->Fetch_ID());
	}

	crc(IsForcedActive);
	crc(IsHasBeen);
	crc(IsFullStrength);
	crc(IsUnderStrength);
	crc(IsReforming);
	crc(IsLagging);
	crc(IsAltered);
	crc(JustAltered);
	crc(IsMoving);
	crc(IsNextMission);
	crc(IsLeaveMap);
	crc(Suspended);

	crc(Quantity, sizeof(Quantity));
}

// FUNCTION: 0x006EC730
void TeamClass::TMission_GOTO_SHROUD(TeamMissionClass * mission, bool first_time)
{
	if (first_time) {
		FootClass *unit = (FootClass *)Zone;
		if (unit == NULL) {
			unit = Member;
		}
		Assign_Mission_Target(Map.Find_Nearby_Shroud(unit));
	}
	Coordinate_Move();
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

// FUNCTION: 0x006EC7D0
void TeamClass::TMission_MOVE(TeamMissionClass * mission, bool first_time)
{
	if (first_time) {
		FootClass * leader = Fetch_A_Leader();
		Cell movecell = Scen->Get_Waypoint_Cell(mission->Data.Value);
		Assign_Mission_Target(&Map[movecell]);
		if (!Is_Leaving_Map()) {
			if (leader->Can_Enter_Cell(&Map[movecell], FACING_NONE, -1, NULL, false) != MOVE_OK) {
				movecell = Map.Nearby_Location(movecell, leader->TClass->Speed);
			}
		}
		if (movecell != CELL_NONE) {
			Assign_Mission_Target(&Map[movecell]);
		} else {
			Assign_Mission_Target(NULL);
		}
	}
	Coordinate_Move();
}

inline bool TeamClass::Ammo_Check(void) const
{
	bool r = false;
	FootClass * unit = Member;
	while (unit != NULL) {
		if (unit->TClass->MaxAmmo <= 0 || unit->Ammo > 0) {
			r = true;
			break;
		}
		unit = unit->Member;
	}
	return(r);
}

// FUNCTION: 0x006EC9A0
void TeamClass::TMission_ATT_WAYPT(TeamMissionClass * mission, bool first_time)
{
	if (first_time) {
		CellClass * trgt = (CellClass *)Scen->Get_Waypoint_Target(mission->Data.Value);
		if (Is_Target_Cell(trgt)) {
			ObjectClass * otrgt = trgt->Cell_Object(Point2D(0, 0), trgt->IsUnderBridge);
			if (otrgt) {
				trgt = (CellClass *)otrgt;
			}
		}
		Assign_Mission_Target(trgt);
	}

	if (MissionTarget && Ammo_Check()) {
		Coordinate_Attack();
	} else {
		Assign_Mission_Target(NULL);
		IsNextMission = true;
	}
}

// FUNCTION: 0x006ECA70
void TeamClass::TMission_ATT_WAYPT_BUILDING(TeamMissionClass * mission, bool first_time)
{
	if (first_time) {
		AbstractClass * trgt = Scen->Get_Waypoint_Target(mission->Data.Value);
		if (Is_Target_Cell(trgt)) {
			CellClass * cell = (CellClass *)trgt;
			trgt = cell->Cell_Object(Point2D(0, 0), cell->IsUnderBridge);
			if (trgt == NULL || trgt->RTTI != RTTI_BUILDING) {
				trgt = NULL;
			}
		} else {
			trgt = NULL;
		}
		Assign_Mission_Target(trgt);
	}

	if (MissionTarget && Ammo_Check()) {
		Coordinate_Attack();
	} else {
		Assign_Mission_Target(NULL);
		IsNextMission = true;
	}
}

// FUNCTION: 0x006ECB50
void TeamClass::TMission_ENTER_GRINDER(TeamMissionClass * mission, bool)
{
	FootClass * unit = Member;
	while (unit != NULL) {
		bool entered = unit->EnterGrinder() != 0;
		FootClass * next = unit->Member;
		if (entered) {
			Remove(unit, -1, true);
		}
		unit = next;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006ECBA0
void TeamClass::TMission_ENTER_TANK_BUNKER(TeamMissionClass * mission, bool)
{
	FootClass * unit = Member;
	while (unit != NULL) {
		bool entered = unit->EnterTankBunker() != 0;
		FootClass * next = unit->Member;
		if (entered) {
			Remove(unit, -1, true);
		}
		unit = next;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006ECBF0
void TeamClass::TMission_ENTER_BIO_REACTOR(TeamMissionClass * mission, bool)
{
	FootClass * unit = Member;
	while (unit != NULL) {
		bool entered = unit->EnterBioReactor() != 0;
		FootClass * next = unit->Member;
		if (entered) {
			Remove(unit, -1, true);
		}
		unit = next;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006ECC40
void TeamClass::TMission_ENTER_BATTLE_BUNKER(TeamMissionClass * mission, bool)
{
	FootClass * unit = Member;
	while (unit != NULL) {
		bool entered = unit->EnterBattleBunker() != 0;
		FootClass * next = unit->Member;
		if (entered) {
			Remove(unit, -1, true);
		}
		unit = next;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006ECC90
void TeamClass::TMission_GARRISON_STRUCTURE(TeamMissionClass * mission, bool)
{
	FootClass * unit = Member;
	while (unit != NULL) {
		bool entered = unit->GarrisonStructure() != 0;
		FootClass * next = unit->Member;
		if (entered) {
			Remove(unit, -1, true);
		}
		unit = next;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006ECE60
void TeamClass::TMission_SPY(TeamMissionClass * mission, bool first_time)
{
	if (first_time) {
		Assign_Mission_Target(Scen->Get_Waypoint_Target(mission->Data.Value));
	}

	if (Is_Target_Cell(MissionTarget)) {
		CellClass * cellptr = _As_Cell(MissionTarget);

		ObjectClass * bldg = cellptr->Cell_Object();
		if (bldg != NULL) {
			Assign_Mission_Target(bldg);
			Coordinate_Attack();
		}
	} else {
		if (MissionTarget == NULL) {
			Assign_Mission_Target(NULL);
			IsNextMission = true;
		} else {
			Coordinate_Attack();
		}
	}
}

// FUNCTION: 0x006ECF10
void TeamClass::TMission_SCATTER(TeamMissionClass * mission, bool)
{
	FootClass * unit = Member;
	while (unit != NULL) {
		unit->Scatter(COORD_NONE, true);
		unit = unit->Member;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006ECF50
void TeamClass::TMission_CHANGE_HOUSE(TeamMissionClass * mission, bool)
{
	FootClass * unit = Member;
	while (unit != NULL) {
		FootClass * next = unit->Member;
		unit->Captured(House_From_HousesType(mission->Data.House));
		unit = next;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006ECFB0
void TeamClass::TMission_TEAMCHANGE(TeamMissionClass * mission, bool)
{
	TeamClass * team = new TeamClass(TeamTypes[mission->Data.Value], House);

	FootClass * unit = Member;
	while (unit != NULL) {
		Remove(unit);
		team->AddMember(unit, false);
		unit = Member;
	}
	delete this;
}

// FUNCTION: 0x006ED030
void TeamClass::TMission_SCRIPT(TeamMissionClass * mission, bool)
{
	if (Script != NULL) {
		delete Script;
	}

	Script = new ScriptClass(ScriptTypes[mission->Data.Value]);
	Script->Stop_Script();
}

// FUNCTION: 0x006ED090
void TeamClass::TMission_ATTACK(TeamMissionClass * mission, bool)
{
	if (MissionTarget == NULL && Member != NULL) {
		FootClass const * candidate = Fetch_A_Leader();

		ThreatType threat = FUN_00645BB0(mission->Data.Quarry);
		AbstractClass * found = candidate->Greatest_Threat(threat, candidate->PositionCoord, Class->OnlyTargetHouseEnemy);
		Assign_Mission_Target(found);
		if (MissionTarget == NULL || !Ammo_Check()) IsNextMission = true;
	}
	if (MissionTarget == NULL || !Ammo_Check()) IsNextMission = true;
	Coordinate_Attack();
}

inline bool TeamClass::Has_Air_Transport(void) const
{
	for (int i = 0; i < Class->TaskForce->ClassCount; i++) {
		TechnoTypeClass const * techtype = Class->TaskForce->Members[i].Class;
		if (techtype->RTTI == RTTI_AIRCRAFTTYPE && techtype->MaxPassengers > 0) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x006ED200
void TeamClass::TMission_LOAD(TeamMissionClass * mission, bool)
{
	FootClass * unit = Member;
	FootClass * trans = NULL;

	bool transportisaircraft = Has_Air_Transport();

	while (unit != NULL) {
		if (unit->Techno_Type_Class()->Max_Passengers() > 0 && unit->Cargo.How_Many() < unit->Techno_Type_Class()->Max_Passengers()) {
			if ((!transportisaircraft && unit->Fetch_RTTI() == RTTI_UNIT) || (transportisaircraft && unit->Fetch_RTTI() == RTTI_AIRCRAFT)) {
				trans = unit;
				break;
			}
		}
		unit = unit->Member;
	}

	if (trans == NULL) {
		IsNextMission = true;
		return;
	}

	if (trans->HasAnyLink()) {
		return;
	}

	bool finished = true;
	unit = Member;
	while (unit != NULL && Total > 1) {
		Coordinate_Conscript(unit);

		if (_Is_It_Playing(unit) && unit != trans) {
			finished = false;
			if (unit->Mission != MISSION_ENTER) {
				unit->QueueMission(MISSION_ENTER, false);
				unit->Assign_Target(NULL);
				unit->Assign_Destination(trans);
				break;
			}
		}

		unit = unit->Member;
	}

	if (finished) {
		if (Class->TransportsReturnOnUnload) {
			Coord pc = trans->PositionCoord;
			trans->ArchiveTarget = &Map[pc];
		}
		IsNextMission = true;
	}
}

// FUNCTION: 0x006ED4D0
void TeamClass::TMission_DEPLOY(TeamMissionClass * mission, bool)
{
	bool finished = true;
	FootClass * obj = Member;

	while (obj != NULL) {
		Coordinate_Conscript(obj);

		if (_Is_It_Playing(obj)) {
			bool deploys = obj->RTTI == RTTI_UNIT && ((UnitClass *)obj)->Class->DeploysInto != NULL;
			bool simple = obj->RTTI == RTTI_UNIT && ((UnitClass *)obj)->Class->IsSimpleDeployer;
			bool infdeploy = obj->RTTI == RTTI_INFANTRY && FUN_005224E0(obj);

			if (deploys) {
				UnitClass * unit = (UnitClass *)obj;
				finished = false;
				if (unit->Mission != MISSION_UNLOAD) {
					unit->Mark(MARK_UP);
					if (!unit->Class->DeploysInto->Legal_Placement(unit->PositionCell, NULL)) {
						unit->Class->DeploysInto->Flush_For_Placement(unit->PositionCell, unit->House);
						unit->Mark(MARK_DOWN);
					} else {
						unit->Mark(MARK_DOWN);
						unit->Assign_Destination(NULL, true);
						unit->Assign_Target(NULL);
						unit->QueueMission(MISSION_UNLOAD, false);
					}
				}
			} else if (simple || infdeploy) {
				if (obj->Mission != MISSION_UNLOAD) {
					obj->QueueMission(MISSION_UNLOAD, false);
				}
			}
		}

		obj = obj->Member;
	}

	if (finished) {
		IsNextMission = true;
	}
}

// FUNCTION: 0x006ED770
void TeamClass::TMission_GUARD(TeamMissionClass * mission, bool first_time)
{
	if (first_time) {
		TimeOut = TICKS_PER_SECOND * mission->Data.Value;
	}

	Coordinate_Regroup();

	if (TimeOut == 0) {
		IsNextMission = true;
	}
}

// FUNCTION: 0x006ED7E0
void TeamClass::TMission_DO(TeamMissionClass * mission, bool)
{
	FootClass * unit = Member;
	MissionType do_mission = mission->Data.Mission;

	while (unit != NULL) {
		Coordinate_Conscript(unit);

		if (_Is_It_Playing(unit)) {
			if (unit->TarCom == NULL && unit->NavCom == NULL) {
				TeamMissionType action = Script->Get_Current_Mission().Mission;
				int stray = (action == (TeamMissionType)0x36 || action == (TeamMissionType)0x35) ? Rule->RelaxedStray : Rule->StrayDistance;
				if (unit->Distance(Zone) > stray * 2 && do_mission != MISSION_GUARD_AREA) {
					unit->QueueMission(MISSION_MOVE, false);
					unit->Assign_Destination(Zone, true);
					unit->QueueMission(MISSION_MOVE, false);
					unit->Assign_Destination(&Map[Cell(unit->Adjust_Dest(Zone->Center_Coord().As_Cell()))], true);
					unit = unit->Member;
					continue;
				}
			}

			if (unit->TarCom == NULL && unit->NavCom == NULL && unit->Get_Mission() != do_mission && (do_mission != MISSION_GUARD || unit->Get_Mission() != MISSION_UNLOAD)) {
				unit->ArchiveTarget = NULL;
				unit->QueueMission(do_mission, false);
				unit->Assign_Target(NULL);
				unit->Assign_Destination(NULL, true);
			}
		}

		unit = unit->Member;
	}
}

// FUNCTION: 0x006EDA90
void TeamClass::TMission_SET_GLOBAL(TeamMissionClass * mission, bool)
{
	Scen->Set_Global_To(mission->Data.Value, true);
	IsNextMission = true;
}

// FUNCTION: 0x006EDAC0
void TeamClass::TMission_CLEAR_GLOBAL(TeamMissionClass * mission, bool)
{
	Scen->Set_Global_To(mission->Data.Value, false);
	IsNextMission = true;
}

// FUNCTION: 0x006EDAF0
void TeamClass::TMission_SET_LOCAL(TeamMissionClass * mission, bool)
{
	Scen->Set_Local_To(mission->Data.Value, true);
	IsNextMission = true;
}

// FUNCTION: 0x006EDB20
void TeamClass::TMission_CLEAR_LOCAL(TeamMissionClass * mission, bool)
{
	Scen->Set_Local_To(mission->Data.Value, false);
	IsNextMission = true;
}

// FUNCTION: 0x006EDB50
void TeamClass::TMission_HOUND_DOG(TeamMissionClass * mission, bool)
{
	Calc_Center(Zone, ClosestMember);
	Target = Zone;
	Coordinate_Move();

	TechnoClass *trgt = Dynamic_Cast<TechnoClass *>(Zone);
	if (trgt != NULL) {
		FootClass *unit = Member;
		while (unit != NULL) {
			if (_Is_It_Playing(unit)) {
				if (unit->TarCom != NULL && unit->TarCom != trgt->TarCom) {
					unit->Assign_Target(NULL);
				}
				if (trgt->TarCom != NULL) {
					if (!unit->Locomotion->Is_Moving() && unit->Is_Weapon_Equipped() && unit->Mission == MISSION_GUARD) {
						unit->QueueMission(MISSION_ATTACK, false);
						unit->Assign_Target(trgt->TarCom);
					}
				}
			}
			unit = unit->Member;
		}
	}
}

// FUNCTION: 0x006EDC70
void TeamClass::TMission_UNPANIC(TeamMissionClass * mission, bool)
{
	FootClass *unit = Member;
	while (unit != NULL) {
		unit->Stop_Fear();
		unit = unit->Member;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006EDCA0
void TeamClass::TMission_FORCE_FACING(TeamMissionClass * mission, bool)
{
	FootClass *unit = Member;
	bool is_complete = true;
	while (unit != NULL) {
		if (unit->Locomotion->Is_Moving()) {
			is_complete = false;
		} else {
			if (unit->PrimaryFacing.Current() != mission->Data.Facing) {
				unit->Locomotion->Do_Turn((DirStruct &)DirType(mission->Data.Facing));
			}
		}
		unit = unit->Member;
	}
	if (is_complete) {
		IsNextMission = true;
	}
}

// FUNCTION: 0x006EDD60
void TeamClass::TMission_PANIC(TeamMissionClass * mission, bool)
{
	FootClass *unit = Member;
	while (unit != NULL) {
		unit->Start_Fear();
		unit = unit->Member;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006EDD90
void TeamClass::TMission_BERZERK(TeamMissionClass * mission, bool)
{
	FootClass *unit = Member;
	while (unit != NULL) {
		unit->Berzerk();
		unit = unit->Member;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006EDDC0
void TeamClass::TMission_IDLE_ANIM(TeamMissionClass * mission, bool)
{
	FootClass *unit = Member;
	while (unit != NULL) {
		unit->Do_Idle(mission->Data.Value);
		unit = unit->Member;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006EDE10
void TeamClass::TMission_LOOP(TeamMissionClass * mission, bool)
{
	Script->Set_Line(mission->Data.Value-2);
	IsNextMission = true;
}

// FUNCTION: 0x006EDE40
void TeamClass::TMission_WIN(TeamMissionClass * mission, bool)
{
	PlayerPtr->Flag_To_Win();
	IsNextMission = true;
}

// FUNCTION: 0x006EDE60
void TeamClass::TMission_LOSE(TeamMissionClass * mission, bool)
{
	PlayerPtr->Flag_To_Lose();
	IsNextMission = true;
}

// FUNCTION: 0x006EDE80
void TeamClass::TMission_PLAY_SPEECH(TeamMissionClass * mission, bool)
{
	IsNextMission = true;
}

// FUNCTION: 0x006EDE90
void TeamClass::TMission_PLAY_SOUND(TeamMissionClass * mission, bool)
{
	VocClass::PlayGlobal(mission->Data.Sound, 1.0f, 0x2000);
	IsNextMission = true;
}

// FUNCTION: 0x006EDEC0
void TeamClass::TMission_PLAY_MOVIE(TeamMissionClass * mission, bool)
{
	FUN_005BF260(mission->Data.Movie, THEME_NONE, true, true, true);
	IsNextMission = true;
}

// FUNCTION: 0x006EDEF0
void TeamClass::TMission_PLAY_MUSIC(TeamMissionClass * mission, bool)
{
	Theme.Queue_Song(mission->Data.Theme);
	IsNextMission = true;
}

// FUNCTION: 0x006EDF10
void TeamClass::TMission_REDUCE_TIBERIUM(TeamMissionClass * mission, bool)
{
	FootClass *unit = Member;
	while (unit != NULL) {
		Cell cell = unit->Center_Coord().As_Cell();
		Map.Area_Reduce_Tiberium(cell);
		unit = unit->Member;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006EDF90
void TeamClass::TMission_BEGIN_PRODUCTION(TeamMissionClass * mission, bool)
{
	House->IsStarted = true;
	IsNextMission = true;
}

// FUNCTION: 0x006EDFB0
void TeamClass::TMission_FIRE_SALE(TeamMissionClass * mission, bool)
{
	House->State = STATE_ENDGAME;
	IsNextMission = true;
}

// FUNCTION: 0x006EDFD0
void TeamClass::TMission_SELF_DESTRUCT(TeamMissionClass * mission, bool)
{
	bool doit = true;
	while (doit) {
		doit = false;

		FootClass *unit = Member;
		while (unit != NULL) {
			if (unit->Strength > 0 && unit->IsActive && unit->IsDown && !unit->IsInLimbo) {
				int damage = unit->Strength;
				unit->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true);
				doit = true;
			}
			unit = unit->Member;
		}
	}
	IsNextMission = true;
}

// FUNCTION: 0x006EE050
void TeamClass::TMission_DESTROY_MEMBERS(TeamMissionClass * mission, bool)
{
	FootClass *unit = Member;
	while (unit != NULL) {
		FootClass *next = unit->Member;
		unit->Limbo();
		unit->Delete_Me();
		unit = next;
	}
	IsNextMission = true;
}

// FUNCTION: 0x006EE0A0
void TeamClass::TMission_ION_STORM_START(TeamMissionClass * mission, bool)
{
	if (!FUN_0053A100()) {
		LightningStorm_Start(mission->Data.Value, Rule->LightningDeferment, DAT_00B0E948, false);
	}
	IsNextMission = true;
}

// FUNCTION: 0x006EE0E0
void TeamClass::TMission_ION_STORM_END(TeamMissionClass * mission, bool)
{
	if (FUN_0053A100()) {
		LightningStorm_RequestStop();
	}
	IsNextMission = true;
}

// FUNCTION: 0x006EE100
void TeamClass::TMission_CENTER_VIEWPOINT(TeamMissionClass * mission, bool)
{
	Coord coord = Zone->Center_Coord();
	coord.Z = Map.Get_Height_GL(coord);

	if (Map[coord].IsUnderBridge || Map[coord].WasUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	TacticalMap->Setup_Trigger_Scroll(coord, mission->Data.Speed);
	IsNextMission = true;
}

// FUNCTION: 0x006EE1B0
void TeamClass::TMission_RESHROUD(TeamMissionClass * mission, bool)
{
	Map.Shroud_The_Map(NULL);
	IsNextMission = true;
}

// FUNCTION: 0x006EE1D0
void TeamClass::TMission_REVEAL(TeamMissionClass * mission, bool)
{
	Map.Reveal_The_Map(NULL);
	IsNextMission = true;
}
