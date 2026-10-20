// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <string.h>
#include "geometry.h"
#include "convert.h"
#include "draw.h"
#include "dsurface.h"
#include "rect.h"
#include "shapeset.h"
#include "vector.h"
#include "_tactica.h"
#include "globals.h"
#include "house.h"
#include "tactical.h"
#include "techno.h"
#include "event.h"
#include "target.h"
#include "planning.h"
#include <stddef.h>
#include "stringtable.h"
#include "voc.h"
#include "_rules.h"
#include "rules.h"
#include "notice.h"
#include "queue.h"
#include "mission.h"

extern double __cdecl FUN_004CAC40(double);
extern Rect __fastcall FUN_0072AD20(void);
Coord __fastcall FUN_00633BC0(EventClass const & event);
extern ConvertClass * FileSystem_MOUSE_PAL;
extern ShapeSet * DAT_00ABF294;

static inline void Shrink_Rect(Rect & r)
{
	if (r.Width <= -2) goto none;
	if (r.Height <= -2) {
none:
		r.X = 0;
		r.Y = 0;
		r.Width = 0;
		r.Height = 0;
	} else {
		r.X += 1;
		r.Y += 1;
		r.Width -= 2;
		r.Height -= 2;
	}
}

// FUNCTION: 0x006333D0 $E2
// FUNCTION: 0x00633400 $E5
// FUNCTION: 0x00633420 $E8
// FUNCTION: 0x00633440 $E11
// FUNCTION: 0x00633460 $E14
// FUNCTION: 0x00633480 $E17
// FUNCTION: 0x006334A0 $E20
// FUNCTION: 0x006334E0 $E23
// FUNCTION: 0x00633510 $E26
// FUNCTION: 0x00633540 $E29
// FUNCTION: 0x00633570 $E32
// FUNCTION: 0x00633580 $E35
// FUNCTION: 0x006335A0 $E38
// FUNCTION: 0x006335C0 $E42

// FUNCTION: 0x006335E0 $E109
// FUNCTION: 0x00633620 $E106
// FUNCTION: 0x00633680 $E115
// FUNCTION: 0x006336C0 $E112
// FUNCTION: 0x00633720 $E121
// FUNCTION: 0x00633760 $E118
// FUNCTION: 0x006337C0 $E127
// FUNCTION: 0x00633800 $E124
// FUNCTION: 0x00633860 $E132
// FUNCTION: 0x006338A0 $E130
// FUNCTION: 0x00633900 $E137
// FUNCTION: 0x00633940 $E135

// FUNCTION: 0x006339A0
void __fastcall FUN_006339A0(void)
{
}

// FUNCTION: 0x006339B0
int __fastcall FUN_006339B0(TechnoClass * techno)
{
	HouseClass * house = techno->House;
	int index = house->HeapID;
	if (house->Is_Player_Control() && PlayerPtr) {
		index = PlayerPtr->HeapID;
	}
	return index;
}

// FUNCTION: 0x006339E0
Point2D __fastcall FUN_006339E0(Coord const & coord)
{
	Point2D pixel;
	TacticalMap->Coord_To_Pixel(coord, pixel);
	pixel.X += DSurface::ViewBounds.X;
	pixel.Y += DSurface::ViewBounds.Y;
	return pixel;
}

// FUNCTION: 0x00633A20
PlanningMemberClass::PlanningMemberClass(void) : Owner(0), Packet(0), Branch(-1), Active(false)
{
}

// FUNCTION: 0x00633A40
void PlanningMemberClass::Set_Owner(TechnoClass * owner)
{
	Owner = owner;
}

// FUNCTION: 0x00633A50
void PlanningMemberClass::Set_Packet(EventClass * packet)
{
	Packet = packet;
}

// FUNCTION: 0x00633A60
void PlanningMemberClass::Set_Branch(int branch)
{
	Branch = branch;
}

// FUNCTION: 0x00633A70
void PlanningMemberClass::Set_Active(bool active)
{
	Active = active;
}

// FUNCTION: 0x00633A80
TechnoClass * PlanningMemberClass::Get_Owner(void)
{
	return Owner;
}

// FUNCTION: 0x00633A90
EventClass * PlanningMemberClass::Get_Packet(void)
{
	return Packet;
}

// FUNCTION: 0x00633AA0
int PlanningMemberClass::Get_Branch(void)
{
	return Branch;
}

// FUNCTION: 0x00633AB0
bool PlanningMemberClass::Get_Active(void)
{
	return Active;
}

// FUNCTION: 0x0063D630
void __fastcall FUN_0063D630(int outline, int fill, Point2D a, Point2D b, int frame)
{
	{
		Point2D d(a.X - b.X, a.Y - b.Y);
		double dx = d.X;
		double dy = d.Y;
		if ((int)FUN_004CAC40(dx * dx + dy * dy) < 37) {
			return;
		}
	}
	int cx = (b.X + a.X) / 2;
	int cy = (a.Y + b.Y) / 2;
	Point2D tl(cx - 10, cy - 10);
	Rect clip = FUN_0072AD20();
	Rect rect(tl.X + 1, tl.Y + 1, 19, 19);
	DSurface::Composite->Draw_Rect(clip, rect, outline);
	Shrink_Rect(rect);
	DSurface::Composite->Fill_Rect(clip, rect, fill);
	Point2D pos = tl;
	Rect bounds = clip;
	Point2D size(DAT_00ABF294->Get_Width(), DAT_00ABF294->Get_Height());
	pos = Point2D(cx - size.X / 2 - 1, cy - size.Y / 2);
	Draw_Shape(*DSurface::Composite, *FileSystem_MOUSE_PAL, DAT_00ABF294, frame, pos, bounds, (ShapeFlags_Type)0x400);
}

// FUNCTION: 0x0063D9C0 PlanningNodeClass::~PlanningNodeClass

// FUNCTION: 0x0063DE30 ?$VectorClass@PAVPlanningMemberClass@@::Clear
// FUNCTION: 0x0063DFD0 ?$DynamicVectorClass@PAVPlanningMemberClass@@::Delete
// FUNCTION: 0x0063E140 ?$VectorClass@PAVPlanningBranchClass@@::Clear
// FUNCTION: 0x0063E590 ?$DynamicVectorClass@PAVPlanningNodeClass@@::Delete_Index
// FUNCTION: 0x0063E5D0 ?$DynamicVectorClass@PAVPlanningNodeClass@@::Delete
// FUNCTION: 0x0063E900 ?$DynamicVectorClass@PAVTechnoClass@@::Delete
// FUNCTION: 0x0063EBD0 ?$DynamicVectorClass@PAVPlanningTokenClass@@::Delete
// FUNCTION: 0x0063EF70 ?$DynamicVectorClass@PAVPlanningMemberClass@@::Delete_Index
// FUNCTION: 0x0063F040 ?$DynamicVectorClass@PAVPlanningTokenClass@@::Delete_Index
// FUNCTION: 0x0063E540 ?$DynamicVectorClass@PAVPlanningNodeClass@@::Add
template class DynamicVectorClass<PlanningNodeClass *>;

// FUNCTION: 0x0063EE30 ?$VectorClass@PAVEventClass@@::~?$VectorClass@PAVEventClass@@
template class VectorClass<EventClass *>;

template class DynamicVectorClass<PlanningMemberClass *>;
template class DynamicVectorClass<PlanningBranchClass *>;

extern INoticeSink * DAT_00AC4B80;
extern PlanningNodeClass * DAT_00AC4CCC;
extern PlanningNodeClass * DAT_00AC4C38;
extern PlanningNodeClass * DAT_00AC4BF0;
extern bool DAT_00AC4CF5;
extern int DAT_00AC4B84[24];

void __fastcall FUN_00639F80(int slot);
void __fastcall FUN_006378B0(PlanningNodeClass * node);
void __fastcall FUN_00637750(PlanningTokenClass * token);
void __fastcall FUN_006377B0(TechnoClass * techno);

DynamicVectorClass<EventClass *> PlanningTokenClass::Events;
DynamicVectorClass<PlanningNodeClass *> PlanningNodeClass::Unknown3;
DynamicVectorClass<TechnoClass *> PlanningTokenClass::Technos;
DynamicVectorClass<PlanningNodeClass *> PlanningNodeClass::Unknown1;
DynamicVectorClass<PlanningNodeClass *> PlanningNodeClass::Unknown2;
DynamicVectorClass<PlanningTokenClass *> PlanningTokenClass::Array;

// FUNCTION: 0x00633AC0
PlanningBranchClass::PlanningBranchClass(void) : Count(0), Color(-1)
{
}

// FUNCTION: 0x00633AE0
void PlanningBranchClass::Set_Event(EventClass const & event)
{
	Event = event;
}

// FUNCTION: 0x00633B00
void PlanningBranchClass::Set_Count(int count)
{
	Count = count;
}

// FUNCTION: 0x00633B10
void PlanningBranchClass::Set_Color(int color)
{
	Color = color;
}

// FUNCTION: 0x00633B20
void PlanningBranchClass::Clear_Count(void)
{
	Count = 0;
}

// FUNCTION: 0x00633B30
int PlanningBranchClass::Inc_Count(void)
{
	return ++Count;
}

// FUNCTION: 0x00633B40
int PlanningBranchClass::Dec_Count(void)
{
	return --Count;
}

// FUNCTION: 0x00633B50
EventClass PlanningBranchClass::Get_Event(void)
{
	return Event;
}

// FUNCTION: 0x00633B70
int PlanningBranchClass::Get_Count(void)
{
	return Count;
}

// FUNCTION: 0x00633B80
int PlanningBranchClass::Get_Color(void)
{
	return Color;
}

// FUNCTION: 0x00633B90
Coord PlanningBranchClass::Get_Coord(void)
{
	return Coord(FUN_00633BC0(Event));
}

// FUNCTION: 0x00633BC0
Coord __fastcall FUN_00633BC0(EventClass const & event)
{
	if (event.Type == EventClass::MEGAMISSION) {
		AbstractClass * object;
		if (event.Data.MegaMission.Target.Is_Valid()) {
			object = event.Data.MegaMission.Target.As_Abstract();
			if (object) {
				return object->As_Coord();
			}
		} else if (event.Data.MegaMission.Follow.Is_Valid()) {
			object = event.Data.MegaMission.Follow.As_Abstract();
			if (object) {
				return object->As_Coord();
			}
		} else if (event.Data.MegaMission.Destination.Is_Valid()) {
			object = event.Data.MegaMission.Destination.As_Abstract();
			if (object) {
				return object->As_Coord();
			}
		}
	}
	return Coord(-1, -1, -1);
}

// FUNCTION: 0x00633C60
Point2D PlanningBranchClass::Get_Pixel(void)
{
	Coord coord = FUN_00633BC0(Event);
	return FUN_006339E0(coord);
}

// FUNCTION: 0x00633CC0
PlanningNodeClass::PlanningNodeClass(int id) : Id(id), Queued(false), LoopBranchIndex(-1), Color(-1)
{
}

// FUNCTION: 0x00633D30
PlanningNodeClass::~PlanningNodeClass()
{
	Clear();
	FUN_00637640(this);
}

// FUNCTION: 0x00633E50
int PlanningNodeClass::Get_Id(void)
{
	return Id;
}

// FUNCTION: 0x00633E60
int PlanningNodeClass::Get_Count(void)
{
	return PlanningMembers.Count();
}

// FUNCTION: 0x00633E70
int PlanningNodeClass::Index_In(TechnoClass * techno)
{
	PlanningTokenClass * token = techno->FUN_00705D20();
	return token->PlanningNodes.ID(this);
}

// FUNCTION: 0x00633EA0
PlanningNodeClass * PlanningNodeClass::Prev_Node(TechnoClass * techno)
{
	PlanningTokenClass * token = techno->FUN_00705D20();
	int index = token->PlanningNodes.ID(this);
	if (index == -1) return 0;
	int count = token->PlanningNodes.Count();
	if (count == 1) return this;
	index--;
	if (index < 0) index = count - 1;
	return token->Node_At(index);
}

// FUNCTION: 0x00633F20
PlanningNodeClass * PlanningNodeClass::Next_Node(TechnoClass * techno)
{
	PlanningTokenClass * token = techno->FUN_00705D20();
	int index = token->PlanningNodes.ID(this);
	if (index == -1) return 0;
	int count = token->PlanningNodes.Count();
	if (count == 1) return this;
	index++;
	if (index >= count) index = 0;
	return token->Node_At(index);
}

// FUNCTION: 0x00633FA0
void PlanningNodeClass::Add_Member(TechnoClass * techno, EventClass const * event)
{
	PlanningMemberClass * member = new PlanningMemberClass;
	member->Set_Owner(techno);
	EventClass * packet = new EventClass;
	*packet = *event;
	member->Set_Packet(packet);
	PlanningMembers.Add(member);
	techno->FUN_00705D20()->Add_Node(this);
	FUN_00634E10(member);
}

// FUNCTION: 0x006340B0
void PlanningNodeClass::Remove_Member(TechnoClass * techno)
{
	PlanningMemberClass * member = Get_Member(techno);
	if (member) {
		Detach(member);
		delete member->Get_Packet();
		PlanningMembers.Delete(member);
		delete member;
		if (Get_Count() == 0) {
			delete this;
		}
	}
}

// FUNCTION: 0x00634290
int PlanningNodeClass::Find_Member(TechnoClass * techno)
{
	int result = -1;
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		if (PlanningMembers[i]->Get_Owner() == techno) {
			result = i;
			break;
		}
	}
	return result;
}

// FUNCTION: 0x006342D0
TechnoClass * PlanningNodeClass::Member_Owner(int index)
{
	return PlanningMembers[index]->Owner;
}

Rect __fastcall FUN_00634510(int size, Point2D center);

// FUNCTION: 0x006342E0
EventClass PlanningNodeClass::Get_Event(int index)
{
	return *PlanningMembers[index]->Packet;
}

// FUNCTION: 0x00634310
EventClass PlanningNodeClass::Get_Event_For(TechnoClass * techno)
{
	return *Get_Member(techno)->Packet;
}

// FUNCTION: 0x00634360
bool PlanningNodeClass::Has_Member(TechnoClass * techno)
{
	return Find_Member(techno) != -1;
}

// FUNCTION: 0x006343B0
HouseClass * PlanningNodeClass::Lead_House(void)
{
	return PlanningMembers[0]->Owner->House;
}

// FUNCTION: 0x006343C0
Coord PlanningNodeClass::Lead_Coord(void)
{
	Coord coord = FUN_00633BC0(*PlanningMembers[0]->Packet);
	return coord;
}

// FUNCTION: 0x00634400
Point2D PlanningNodeClass::Lead_Pixel(void)
{
	Coord coord = FUN_00633BC0(*PlanningMembers[0]->Packet);
	return FUN_006339E0(coord);
}

static inline Coord Event_Coord(EventClass const & e) { return FUN_00633BC0(e); }
static inline Point2D Pixel_0(Coord const & c) { return FUN_006339E0(c); }
static inline Point2D Pixel_1(Coord const & c) { return Pixel_0(c); }
static inline Point2D Pixel_2(Coord const & c) { return Pixel_1(c); }
static inline Point2D Pixel_3(Coord const & c) { return Pixel_2(c); }
static inline Point2D Pixel_4(Coord const & c) { return Pixel_3(c); }
static inline Point2D Pixel_5(Coord const & c) { return Pixel_4(c); }
static inline Point2D Pixel_6(Coord const & c) { return Pixel_5(c); }
static inline Point2D Pixel_7(Coord const & c) { return Pixel_6(c); }
static inline Point2D Pixel_Of(Coord c) { Point2D p = Pixel_7(c); return p; }
static inline Rect Around_0(int size, Point2D p) { return FUN_00634510(size, p); }
static inline Rect Around_1(int size, Point2D p) { return Around_0(size, p); }
static inline Rect Around_2(int size, Point2D p) { return Around_1(size, p); }
static inline Rect Around_3(int size, Point2D p) { return Around_2(size, p); }
static inline Rect Around_4(int size, Point2D p) { return Around_3(size, p); }
static inline Rect Around_5(int size, Point2D p) { return Around_4(size, p); }
static inline Rect Around_6(int size, Point2D p) { return Around_5(size, p); }
static inline Rect Around_7(int size, Point2D p) { return Around_6(size, p); }
static inline Rect Around(int size, Point2D p) { return Around_7(size, p); }

// FUNCTION: 0x00634470
bool PlanningNodeClass::Hit_Test(int x, int y)
{
	Rect rect = Lead_Box_Rect();
	return rect.Is_Point_Within(Point2D(x, y));
}

// FUNCTION: 0x00634510
Rect __fastcall FUN_00634510(int size, Point2D center)
{
	int offset = -(size / 2);
	Point2D corner = center;
	corner.X += offset;
	corner.Y += offset;
	return Rect(corner, size, size);
}

// FUNCTION: 0x00634550
bool PlanningNodeClass::Lead_At(Coord coord)
{
	Coord c = FUN_00633BC0(*PlanningMembers[0]->Packet);
	return c.X == coord.X && c.Y == coord.Y && c.Z == coord.Z;
}

// FUNCTION: 0x006345A0
Rect PlanningNodeClass::Lead_Rect(void)
{
	return FUN_00634510(8, Lead_Pixel());
}

// FUNCTION: 0x00634630
bool PlanningNodeClass::Is_Past_Start(void)
{
	if (PlanningMembers.Count() == 0) return false;
	PlanningTokenClass * token = PlanningMembers[0]->Owner->FUN_00705D20();
	int index = token->PlanningNodes.ID(this);
	int start = token->StepsToClosedLoop;
	return start != -1 && index >= start;
}

// FUNCTION: 0x00634680
void PlanningNodeClass::Promote_Events(void)
{
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		EventClass * packet = PlanningMembers[i]->Packet;
		if (packet->Data.MegaMission.Mission == 2) packet->Data.MegaMission.Mission = 0x1d;
	}
}

// FUNCTION: 0x006346B0
PlanningMemberClass * PlanningNodeClass::Get_Member(TechnoClass * techno)
{
	int index = Find_Member(techno);
	if (index == -1) {
		return NULL;
	}
	return PlanningMembers[index];
}

// FUNCTION: 0x00634700
int PlanningNodeClass::Branch_Count(void)
{
	return PlanningBranches.Count();
}

// FUNCTION: 0x00634710
int PlanningNodeClass::Has_Loop(void)
{
	return LoopBranch.Count > 0;
}

// FUNCTION: 0x00634720
int PlanningNodeClass::Is_Loop_Alone(void)
{
	if (LoopBranch.Count > 0 && LoopBranchIndex == -1) return 1;
	return 0;
}

// FUNCTION: 0x00634740
TechnoClass * PlanningNodeClass::Owner_At_Branch(int branch)
{
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		PlanningMemberClass * m = PlanningMembers[i];
		if (m->Branch == branch) return m->Owner;
	}
	return 0;
}

// FUNCTION: 0x00634770
TechnoClass * PlanningNodeClass::Active_Owner(void)
{
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		PlanningMemberClass * m = PlanningMembers[i];
		if (m->Active) return m->Owner;
	}
	return 0;
}

// FUNCTION: 0x006347A0
PlanningBranchClass * PlanningNodeClass::Get_Branch(int index)
{
	return PlanningBranches[index];
}

// FUNCTION: 0x006347B0
PlanningBranchClass * PlanningNodeClass::Branch_Of(TechnoClass * techno)
{
	PlanningMemberClass * member = Get_Member(techno);
	int branch = member->Branch;
	PlanningBranchClass * result = 0;
	if (branch != -1) result = PlanningBranches[branch];
	return result;
}

// FUNCTION: 0x00634800
EventClass * PlanningNodeClass::Loop_Event(void)
{
	return &LoopBranch.Event;
}

// FUNCTION: 0x00634810
Coord PlanningNodeClass::Branch_Coord(int index)
{
	Coord coord = FUN_00633BC0(PlanningBranches[index]->Event);
	return coord;
}

// FUNCTION: 0x00634850
Coord PlanningNodeClass::Loop_Coord(void)
{
	Coord coord = FUN_00633BC0(LoopBranch.Event);
	return coord;
}

// FUNCTION: 0x00634880
Point2D PlanningNodeClass::Branch_Pixel(int index)
{
	return PlanningBranches[index]->Get_Pixel();
}

// FUNCTION: 0x006348F0
Point2D PlanningNodeClass::Loop_Pixel(void)
{
	return LoopBranch.Get_Pixel();
}

// FUNCTION: 0x00634950
void PlanningNodeClass::Set_Queued(void)
{
	Queued = true;
}

// FUNCTION: 0x00634960
bool PlanningNodeClass::Get_Queued(void)
{
	return Queued;
}

// FUNCTION: 0x00634970
void PlanningNodeClass::Rebuild(void)
{
	Clear();
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		FUN_00634E10(PlanningMembers[i]);
	}
	Update_Active();
	Queued = false;
}

// FUNCTION: 0x006349B0
void PlanningNodeClass::Update_Active(void)
{
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		PlanningMemberClass * member = PlanningMembers[i];
		TechnoClass * owner = member->Owner;
		if (Is_Loop_Start(owner)) {
			member->Active = true;
			if (LoopBranch.Inc_Count() == 1) {
				LoopBranch.Set_Event(owner->FUN_00705D20()->Last_Node()->Get_Event_For(owner));
			}
		}
	}
	LoopBranchIndex = -1;
	if (LoopBranch.Get_Count() != 0) {
		EventClass self = LoopBranch.Get_Event();
		int branches = PlanningBranches.Count();
		int b = 0;
		if (branches > 0) {
			do {
				EventClass other = PlanningBranches[b]->Get_Event();
				Coord a = FUN_00633BC0(self);
				Coord c = FUN_00633BC0(other);
				if (a == c) {
					LoopBranchIndex = b;
					return;
				}
				b++;
			} while (b < branches);
		}
	}
}

// FUNCTION: 0x00634CC0
int PlanningNodeClass::Find_Branch(TechnoClass * techno)
{
	PlanningNodeClass * prev = Prev_Node(techno);
	if (prev == techno->FUN_00705D20()->Last_Node()) return -1;
	EventClass event = prev->Get_Event_For(techno);
	int count = PlanningBranches.Count();
	if (count == 0) return -1;
	Coord origin = FUN_00633BC0(event);
	for (int i = 0; i < count; i++) {
		PlanningBranchClass * branch = PlanningBranches[i];
		if (branch->Get_Count()) {
			EventClass other = branch->Get_Event();
			Coord c = FUN_00633BC0(other);
			if (origin == c) return i;
		}
	}
	return -1;
}

// FUNCTION: 0x00635060
void PlanningNodeClass::Detach(PlanningMemberClass * member)
{
	if (member->Get_Active()) {
		Deactivate(member);
	}
	if (member->Get_Branch() != -1) {
		PlanningBranchClass * branch = PlanningBranches[member->Get_Branch()];
		branch->Set_Color(-1);
		Reset_Colors(false);
		if (branch->Dec_Count() == 0) {
			FUN_006378B0(this);
		}
	}
	member->Set_Branch(-1);
}

// FUNCTION: 0x00635120
void PlanningNodeClass::Clear(void)
{
	int branches = PlanningBranches.Count();
	for (int i = 0; i < branches; i++) {
		delete PlanningBranches[i];
	}
	PlanningBranches.Clear();
	int members = PlanningMembers.Count();
	for (int j = 0; j < members; j++) {
		PlanningMemberClass * member = PlanningMembers[j];
		member->Set_Branch(-1);
		member->Set_Active(false);
	}
	LoopBranch.Clear_Count();
	LoopBranchIndex = -1;
}

// FUNCTION: 0x006351A0
bool PlanningNodeClass::Is_Loop_Start(TechnoClass * techno)
{
	PlanningTokenClass * token = techno->FUN_00705D20();
	return token->PlanningNodes.ID(this) == token->StepsToClosedLoop;
}

// FUNCTION: 0x006351E0
void PlanningNodeClass::Deactivate(PlanningMemberClass * member)
{
	member->Set_Active(false);
	if (LoopBranch.Dec_Count() == 0) {
		LoopBranchIndex = -1;
	}
}

// FUNCTION: 0x00635210
void PlanningNodeClass::Locate_Loop_Branch(void)
{
	LoopBranchIndex = -1;
	if (LoopBranch.Get_Count()) {
		EventClass self = LoopBranch.Get_Event();
		int branches = PlanningBranches.Count();
		int b = 0;
		if (branches > 0) {
			do {
				EventClass other = PlanningBranches[b]->Get_Event();
				Coord a = FUN_00633BC0(self);
				Coord c = FUN_00633BC0(other);
				if (a == c) {
					LoopBranchIndex = b;
					return;
				}
				b++;
			} while (b < branches);
		}
	}
}

// FUNCTION: 0x006352D0
void PlanningNodeClass::Select_All(void)
{
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * owner = PlanningMembers[i]->Owner;
		if (!owner->IsSelected) {
			owner->Select();
		}
	}
}

// FUNCTION: 0x00635310
void PlanningNodeClass::Unselect_All(void)
{
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * owner = PlanningMembers[i]->Owner;
		if (owner->IsSelected) {
			owner->Unselect();
		}
	}
}

// FUNCTION: 0x00635490
void PlanningNodeClass::Fill_Lead_Box(int color)
{
	Rect rect;
	Rect clip = FUN_0072AD20();
	rect = Lead_Rect();
	rect.X += 1;
	rect.Y += 1;
	rect.Width -= 2;
	rect.Height -= 2;
	DSurface::Composite->Fill_Rect(clip, rect, color);
}

static inline Rect Inflated(Rect const & r, int n)
{
	Rect o = r;
	o.X -= n;
	o.Y -= n;
	o.Width += 2 * n;
	o.Height += 2 * n;
	return o;
}

// FUNCTION: 0x00635560
void PlanningNodeClass::Frame_Lead_Box(int color)
{
	Rect clip = FUN_0072AD20();
	Rect rect = Inflated(Around(8, Pixel_Of(Event_Coord(*PlanningMembers[0]->Packet))), 4);
	for (int i = 0; i < 3; i++) {
		DSurface::Composite->Draw_Rect(clip, rect, color);
		Shrink_Rect(rect);
	}
}

// FUNCTION: 0x006357D0
void __fastcall FUN_006357D0(Rect * rect, int amount)
{
	int twice = amount * 2;
	if (twice < 0 && (twice >= rect->Width || twice >= rect->Height)) {
		rect->X = 0;
		rect->Y = 0;
		rect->Width = 0;
		rect->Height = 0;
		return;
	}
	rect->X -= amount;
	rect->Y -= amount;
	rect->Width += twice;
	rect->Height += twice;
}

extern RGBClass DAT_00A83CD9;
extern RGBClass DAT_00A80220;
struct Drawing {
	static unsigned char RedShiftRight;
	static unsigned char GreenShiftRight;
	static unsigned char BlueShiftRight;
	static int RedShiftLeft;
	static int GreenShiftLeft;
	static int BlueShiftLeft;
};

// FUNCTION: 0x0063D070
int __fastcall FUN_0063D070(RGBClass const & color, RGBClass const & other, int ratio)
{
	RGBClass mixed = color;
	mixed.Adjust(ratio, other);
	return (mixed.Get_Red() >> Drawing::RedShiftRight << Drawing::RedShiftLeft)
		| (mixed.Get_Green() >> Drawing::GreenShiftRight << Drawing::GreenShiftLeft)
		| (mixed.Get_Blue() >> Drawing::BlueShiftRight << Drawing::BlueShiftLeft);
}

static inline int Mix_0(RGBClass const & c, RGBClass const & o, int r) { return FUN_0063D070(c, o, r); }
static inline int Mix_1(RGBClass const & c, RGBClass const & o, int r) { return Mix_0(c, o, r); }
static inline int Mix_2(RGBClass const & c, RGBClass const & o, int r) { return Mix_1(c, o, r); }
static inline int Mix_3(RGBClass const & c, RGBClass const & o, int r) { return Mix_2(c, o, r); }
static inline int Mix_4(RGBClass const & c, RGBClass const & o, int r) { return Mix_3(c, o, r); }
static inline int Mix_5(RGBClass const & c, RGBClass const & o, int r) { return Mix_4(c, o, r); }
static inline int Mix_6(RGBClass const & c, RGBClass const & o, int r) { return Mix_5(c, o, r); }
static inline int Mix_7(RGBClass const & c, RGBClass const & o, int r) { return Mix_6(c, o, r); }

// FUNCTION: 0x00635830
int PlanningNodeClass::Node_Color(void)
{
	int result = Color;
	if (result == -1) {
		TechnoClass * owner = PlanningMembers[0]->Owner;
		int slot = FUN_006339B0(owner);
		result = Mix_7(RGBClass(Houses[slot]->RemapColorRGB), DAT_00A83CD9, 0x50);
		Color = result;
		int count = PlanningBranches.Count();
		for (int i = 0; i < count; i++) {
			if (PlanningBranches[i]->Get_Color() == result) goto done;
		}
		if (count == 0) {
			PlanningTokenClass * token = owner->FUN_00705D20();
			bool past = token->Reached(token->Node_Index(this));
			if (past) {
				slot = FUN_006339B0(owner);
				Color = FUN_0063D070(RGBClass(Houses[slot]->RemapColorRGB), DAT_00A80220, 0x64);
			}
		} else {
			slot = FUN_006339B0(owner);
			Color = FUN_0063D070(RGBClass(Houses[slot]->RemapColorRGB), DAT_00A80220, 0x64);
		}
		result = Color;
	}
done:
	return result;
}

// FUNCTION: 0x00635DB0
void PlanningNodeClass::Reset_Colors(bool all)
{
	Color = -1;
	if (all) {
		int count = PlanningBranches.Count();
		int i = 0;
		if (count > 0) do {
			PlanningBranches[i]->Set_Color(-1);
			i++;
		} while (i < count);
		LoopBranch.Set_Color(-1);
	}
}

// FUNCTION: 0x00635DF0
Rect PlanningNodeClass::Lead_Box_Rect(void)
{
	Rect rect = Lead_Rect();
	FUN_006357D0(&rect, 4);
	return rect;
}

// FUNCTION: 0x00635E90
Rect PlanningNodeClass::Lead_Box_Rect2(void)
{
	return Inflated(Inflated(Around(8, Pixel_Of(Event_Coord(*PlanningMembers[0]->Packet))), 4), 4);
}

// FUNCTION: 0x00635F20
PlanningTokenClass::PlanningTokenClass(TechnoClass * owner) : F1C(false), F8C(-1), ClosedLoopNodeCount(-1), StepsToClosedLoop(-1), F98(false), F99(false)
{
	OwnerUnit = owner;
}

// FUNCTION: 0x00635F80
PlanningTokenClass::~PlanningTokenClass()
{
	if (OwnerUnit) {
		OwnerUnit->FUN_00705D10(0);
	}
	FUN_00637750(this);
	Cancel_Owner();
}

// FUNCTION: 0x00636110
TechnoClass * PlanningTokenClass::Get_Owner(void)
{
	return OwnerUnit;
}

// FUNCTION: 0x00636120
void PlanningTokenClass::Cancel_Owner(void)
{
	int count = PlanningNodes.Count();
	bool none = (count == 0);
	for (int i = 0; i < count; i++) {
		PlanningNodeClass * node = PlanningNodes[i];
		node->Remove_Member(Get_Owner());
	}
	if (!none) {
		FUN_006377B0(OwnerUnit);
	}
	PlanningNodes.Clear();
	F8C = -1;
	ClosedLoopNodeCount = -1;
	StepsToClosedLoop = -1;
	F98 = false;
	F99 = false;
}

// FUNCTION: 0x00636310
void PlanningTokenClass::Destroy(void)
{
	Cancel_Owner();
	OwnerUnit = 0;
	delete this;
}

// FUNCTION: 0x00636520
void PlanningTokenClass::Set_F1C(void)
{
	F1C = true;
}

// FUNCTION: 0x00636530
void PlanningTokenClass::Clear_F1C(void)
{
	F1C = false;
}

// FUNCTION: 0x00636540
bool PlanningTokenClass::Get_F1C(void)
{
	return F1C;
}

// FUNCTION: 0x00636550
bool PlanningTokenClass::Owner_Is_Mission(void)
{
	return OwnerUnit->CurrentMission == 0x1c || OwnerUnit->MissionQueue == 0x1c;
}

// FUNCTION: 0x00636570
bool PlanningTokenClass::Has_Started(void)
{
	return PlanningNodes.Count() > 0 && (F8C == -1 || F8C > 0);
}

// FUNCTION: 0x006367B0
void PlanningTokenClass::Share_Range(TechnoClass * techno, int start, int count)
{
	if (techno->FUN_00705D20()) {
		int end = start + count;
		for (int i = start; i < end; i++) {
			PlanningNodeClass * node = PlanningNodes[i];
			EventClass event = node->Get_Event_For(OwnerUnit);
			event.Data.Target.Whom = TargetClass(techno);
			node->Add_Member(techno, &event);
		}
	}
}

// FUNCTION: 0x00636980
void PlanningTokenClass::Share_From(TechnoClass * techno, int start)
{
	int count = PlanningNodes.Count() - start;
	if (techno->FUN_00705D20()) {
		for (int i = start; i < count + start; i++) {
			PlanningNodeClass * node = PlanningNodes[i];
			EventClass event = node->Get_Event_For(OwnerUnit);
			event.Data.Target.Whom = TargetClass(techno);
			node->Add_Member(techno, &event);
		}
	}
}

// FUNCTION: 0x00636D90
int PlanningTokenClass::Get_F8C(void)
{
	return F8C;
}

// FUNCTION: 0x00636DA0
bool PlanningTokenClass::Reached(int index)
{
	if (F8C == -1) return false;
	return index >= F8C;
}

// FUNCTION: 0x00636DC0
int PlanningTokenClass::Node_Count(void)
{
	return PlanningNodes.Count();
}

// FUNCTION: 0x00636DD0
int PlanningTokenClass::Current_Or_Count(void)
{
	if (F8C == -1) return PlanningNodes.Count();
	return F8C;
}

// FUNCTION: 0x00636DE0
int PlanningTokenClass::Remaining(void)
{
	if (F8C == -1) return 0;
	return PlanningNodes.Count() - F8C;
}

// FUNCTION: 0x00636E00
void PlanningTokenClass::Add_Node(PlanningNodeClass * node)
{
	PlanningNodes.Add(node);
	if (F8C == -1) {
		F8C = PlanningNodes.Count() - 1;
	}
}

// FUNCTION: 0x00636E60
PlanningNodeClass * PlanningTokenClass::Node_At(int index)
{
	int count = PlanningNodes.Count();
	if (count == 0) return 0;
	if (index >= count) {
		int start = StepsToClosedLoop;
		if (start <= 0 && start != 0) return 0;
		index = (index - start) % ClosedLoopNodeCount + start;
	}
	if (index < 0 || index >= count) return 0;
	return PlanningNodes[index];
}

// FUNCTION: 0x00636EB0
PlanningNodeClass * PlanningTokenClass::Last_Node(void)
{
	int index = PlanningNodes.Count() - 1;
	if (index < 0) return 0;
	return PlanningNodes[index];
}

// FUNCTION: 0x00636EC0
bool PlanningTokenClass::Has_Node(PlanningNodeClass * node)
{
	return PlanningNodes.ID(node) != -1;
}

// FUNCTION: 0x00636EE0
int PlanningTokenClass::Node_Index(PlanningNodeClass * node)
{
	return PlanningNodes.ID(node);
}

// FUNCTION: 0x00636F00
EventClass PlanningTokenClass::Event_At(int index)
{
	PlanningNodeClass * node = PlanningNodes[index];
	TechnoClass * owner = OwnerUnit;
	return *node->Get_Member(owner)->Packet;
}

// FUNCTION: 0x00636F60
EventClass PlanningTokenClass::Last_Popped(void)
{
	return Popped;
}

// FUNCTION: 0x00636F80
PlanningNodeClass * PlanningTokenClass::Loop_Node(void)
{
	if (StepsToClosedLoop == -1) return 0;
	return PlanningNodes[StepsToClosedLoop];
}

// FUNCTION: 0x00636FA0
void PlanningTokenClass::Set_Loop(int count, int start)
{
	ClosedLoopNodeCount = count;
	StepsToClosedLoop = start;
	F98 = true;
}

// FUNCTION: 0x00636FC0
int PlanningTokenClass::Get_Loop_Count(void)
{
	return ClosedLoopNodeCount;
}

// FUNCTION: 0x00636FD0
int PlanningTokenClass::Get_Loop_Start(void)
{
	return StepsToClosedLoop;
}

// FUNCTION: 0x00636FE0
bool PlanningTokenClass::Loop_Start_Valid(void)
{
	return StepsToClosedLoop > 0 || StepsToClosedLoop == 0;
}

// FUNCTION: 0x00637000
bool PlanningTokenClass::Loop_Start_Positive(void)
{
	return StepsToClosedLoop > 0;
}

// FUNCTION: 0x00637010
bool PlanningTokenClass::Loop_Start_Zero(void)
{
	return StepsToClosedLoop == 0;
}

// FUNCTION: 0x00637020
bool PlanningTokenClass::Get_F98(void)
{
	return F98;
}

// FUNCTION: 0x00637030
bool PlanningTokenClass::Get_F99(void)
{
	return F99;
}

// FUNCTION: 0x00637040
void PlanningTokenClass::Dec_Loop_Start(void)
{
	StepsToClosedLoop--;
	StepsToClosedLoop = (StepsToClosedLoop <= 0) ? 0 : StepsToClosedLoop;
}

extern bool DAT_00AC4CB0;
extern Point2D DAT_00AC4C68;

// FUNCTION: 0x00637060
void __fastcall FUN_00637060(INoticeSink * value)
{
	DAT_00AC4B80 = value;
}

// FUNCTION: 0x00637070
void __fastcall FUN_00637070(void)
{
	PlanningNodeClass::PlanningModeActive = false;
	DAT_00AC4CF5 = false;
	DAT_00AC4CCC = 0;
	DAT_00AC4C38 = 0;
	DAT_00AC4BF0 = 0;
	DAT_00AC4C68 = Point2D(0, 0);
	DAT_00AC4CB0 = false;
}

void __fastcall FUN_006370B0(void);

// FUNCTION: 0x006370A0
void __fastcall FUN_006370A0(void)
{
	FUN_006370B0();
}

extern bool DAT_00AC4CF6;
extern int DAT_00AC4BF4[5];
extern bool DAT_00AC4CD0;
extern bool DAT_00AC4CC8;
extern int DAT_00AC4C3C;
extern bool DAT_00AC4C08;
extern int DAT_00AC4C90;

// FUNCTION: 0x006370B0
void __fastcall FUN_006370B0(void)
{
	int count = PlanningTokenClass::Array.Count();
	DAT_00AC4CF5 = true;
	PlanningNodeClass::PlanningModeActive = false;
	DAT_00AC4CF6 = false;
	for (int i = 0; i < count; i++) {
		PlanningTokenClass * token = PlanningTokenClass::Array[i];
		delete token;
	}
	memset(DAT_00AC4B84, 0, sizeof(DAT_00AC4B84));
	PlanningTokenClass::Technos.Clear();
	PlanningTokenClass::Array.Clear();
	PlanningNodeClass::Unknown1.Clear();
	PlanningNodeClass::Unknown2.Clear();
	DAT_00AC4CCC = 0;
	DAT_00AC4CD0 = false;
	DAT_00AC4CC8 = false;
	DAT_00AC4C3C = 0;
	DAT_00AC4C08 = false;
	DAT_00AC4CF5 = false;
	DAT_00AC4C90 = -1;
	memset(DAT_00AC4BF4, 0, sizeof(DAT_00AC4BF4));
}

// FUNCTION: 0x00637550
void __fastcall FUN_00637550(void)
{
	for (int i = PlanningTokenClass::Events.Count() - 1; i >= 0; i--) {
		EventClass * event = PlanningTokenClass::Events[i];
		event->FUN_00652230();
		PlanningTokenClass::Events.Delete_Index(i);
		delete event;
	}
}

// FUNCTION: 0x006375B0
void __fastcall FUN_006375B0(EventClass const & source)
{
	EventClass * event = new EventClass;
	*event = source;
	if (event->Type == EventClass::MEGAMISSION) {
		*(unsigned char *)&event->Data.MegaMission.IsPlanningEvent = 2;
	}
	PlanningTokenClass::Events.Add(event);
}

// FUNCTION: 0x00637640
void PlanningNodeClass::FUN_00637640(PlanningNodeClass * node)
{
	Unknown3.Delete(node);
	Unknown1.Delete(node);
	Unknown2.Delete(node);
	if (DAT_00AC4CCC == node) {
		DAT_00AC4CCC = 0;
	}
	if (DAT_00AC4C38 == node) {
		DAT_00AC4C38 = 0;
	}
	if (DAT_00AC4BF0 == node) {
		DAT_00AC4BF0 = 0;
	}
}

// FUNCTION: 0x00637750
void __fastcall FUN_00637750(PlanningTokenClass * token)
{
	if (!DAT_00AC4CF5) {
		PlanningTokenClass::Array.Delete(token);
	}
}

// FUNCTION: 0x006377B0
void __fastcall FUN_006377B0(TechnoClass * techno)
{
	int slot = FUN_006339B0(techno);
	FUN_00639F80(slot);
	PlanningTokenClass::Technos.Delete(techno);
}

// FUNCTION: 0x00637840
void __fastcall FUN_00637840(TechnoClass * techno)
{
	if (!DAT_00AC4CF6) {
		DAT_00AC4CF6 = true;
		if (PlanningNodeClass::PlanningModeActive) {
			PlanningTokenClass * token = techno->FUN_00705D20();
			if (token) {
				PlanningNodeClass * node = token->Last_Node();
				if (node) {
					int count = node->PlanningMembers.Count();
					for (int i = 0; i < count; i++) {
						TechnoClass * owner = node->PlanningMembers[i]->Owner;
						if (!owner->IsSelected) {
							owner->Select();
						}
					}
				}
			}
		}
		DAT_00AC4CF6 = false;
	}
}

// FUNCTION: 0x006378B0
void __fastcall FUN_006378B0(PlanningNodeClass * node)
{
	if (node && !node->Get_Queued()) {
		node->Set_Queued();
		PlanningNodeClass::Unknown3.Add(node);
	}
}


void __fastcall FUN_006521C0(EventClass event);

// FUNCTION: 0x00637920
void __fastcall FUN_00637920(void)
{
	int count = PlanningNodeClass::Unknown3.Count();
	for (int i = 0; i < count; i++) {
		PlanningNodeClass * node = PlanningNodeClass::Unknown3[i];
		node->Clear();
		int members = node->PlanningMembers.Count();
		for (int j = 0; j < members; j++) {
			node->FUN_00634E10(node->PlanningMembers[j]);
		}
		node->Update_Active();
		node->Queued = false;
	}
	PlanningNodeClass::Unknown3.Clear();
}

// FUNCTION: 0x006379C0
void __fastcall FUN_006379C0(void)
{
	if (!PlanningNodeClass::PlanningModeActive) {
		PlanningNodeClass::PlanningModeActive = true;
		VocClass::PlayGlobal(Rule->StartPlanningModeSound, 1.0f, 0x2000);
		if (DAT_00AC4B80) {
			DAT_00AC4B80->INoticeSink_Unknown(0x11c7, 0);
		}
	}
}

// FUNCTION: 0x00637A10
void __fastcall FUN_00637A10(void)
{
	if (PlanningNodeClass::PlanningModeActive) {
		EventClass event(PlayerPtr->HeapID, EventClass::PLAN_COMMIT);
		FUN_006521C0(event);
		DAT_00AC4CCC = 0;
		DAT_00AC4C38 = 0;
		DAT_00AC4BF0 = 0;
		PlanningNodeClass::PlanningModeActive = false;
		VocClass::PlayGlobal(Rule->EndPlanningModeSound, 1.0f, 0x2000);
		if (DAT_00AC4B80) {
			DAT_00AC4B80->INoticeSink_Unknown(0x11c8, 0);
		}
	}
}

// FUNCTION: 0x00637AA0
bool __fastcall FUN_00637AA0(void)
{
	return PlanningNodeClass::PlanningModeActive;
}

// FUNCTION: 0x00637D00
void __fastcall FUN_00637D00(PlanningNodeClass * node)
{
	EventClass event = *node->PlanningMembers[0]->Packet;
	xTargetClass * target;
	if (event.Data.MegaMission.Target.Is_Valid()) {
		target = &event.Data.MegaMission.Target;
	} else if (event.Data.MegaMission.Follow.Is_Valid()) {
		target = &event.Data.MegaMission.Follow;
	} else if (event.Data.MegaMission.Destination.Is_Valid()) {
		target = &event.Data.MegaMission.Destination;
	} else {
		return;
	}
	AbstractClass * object = target->As_Abstract();
	if (object) {
		EventClass remove(PlayerPtr->HeapID, EventClass::PLAN_NODE_DELETE, TargetClass(object));
		FUN_006521C0(remove);
	}
}

// FUNCTION: 0x00637DB0
bool __fastcall FUN_00637DB0(int type)
{
	if (PlanningNodeClass::PlanningModeActive && type == 4) {
		return false;
	}
	return true;
}

// FUNCTION: 0x00637DD0
void __fastcall FUN_00637DD0(EventClass event)
{
	if (event.Type == EventClass::MEGAMISSION) {
		event.Data.MegaMission.IsPlanningEvent = true;
		FUN_006521C0(event);
	}
}

// FUNCTION: 0x006385C0
bool TechnoClass::TryNextPlanningTokenNode(void)
{
	PlanningTokenClass * token = FUN_00705D20();
	if (token && token->PlanningNodes.Count() > 0 && (token->F8C == -1 || token->F8C > 0) && !token->F1C) {
		QueueMission(MISSION_WAIT, true);
		EventClass popped = token->Pop_Front();
		FUN_006375B0(popped);
		token->F1C = true;
		return true;
	}
	return false;
}

// FUNCTION: 0x00638A20
void __fastcall FUN_00638A20(PlanningNodeClass * node)
{
	PlanningNodeClass::Unknown2.Add(node);
}

// FUNCTION: 0x00638A80
PlanningTokenClass * __fastcall FUN_00638A80(TechnoClass * techno)
{
	PlanningTokenClass * token = new PlanningTokenClass(techno);
	PlanningTokenClass::Array.Add(token);
	techno->FUN_00705D10(token);
	return token;
}

// FUNCTION: 0x00638B50
bool __fastcall FUN_00638B50(char type)
{
	return type == 9 || type == 6 || type == 7;
}

#include "cell.h"

// FUNCTION: 0x00638B70
bool __fastcall FUN_00638B70(TechnoClass * techno, int mission, TargetClass target)
{
	if (mission == 0xb) {
		if (!target.Is_Valid()) {
			return true;
		}
		if (techno) {
			Cell here = techno->Get_Cell();
			Cell there = techno->Get_Cell();
			CellClass * cell = target.As_Cell();
			if (cell) {
				there = cell->CellID;
			} else {
				TechnoClass * other = target.As_Techno();
				if (other) {
					there = other->Get_Cell();
				}
			}
			if (here.X == there.X && here.Y == there.Y) {
				return true;
			}
		}
	}
	return false;
}

// FUNCTION: 0x00638C20
bool __fastcall FUN_00638C20(int mission)
{
	return mission == 7 || mission == 8 || mission == 9;
}

// FUNCTION: 0x00638C40
bool __fastcall FUN_00638C40(int mission, TargetClass target)
{
	if (mission == 1 || mission == 0xb) {
		if ((RTTIType)target == RTTI_CELL) {
			return true;
		}
		if (mission == 0xb && (RTTIType)target == RTTI_NONE) {
			return true;
		}
	}
	return false;
}

static inline bool Mission_Fits(int mission, TargetClass target)
{
	if (mission == 1 || mission == 0xb) {
		if ((RTTIType)target == RTTI_CELL) goto fits;
		if (mission == 0xb && (RTTIType)target == RTTI_NONE) {
fits:
			return true;
		}
	}
	return false;
}

// FUNCTION: 0x00638C70
bool __fastcall FUN_00638C70(TechnoClass * techno)
{
	if (techno) {
		int mission = techno->Get_Mission();
		if (mission == 5) {
			return true;
		}
		return Mission_Fits(mission, TargetClass(techno->TarCom));
	}
	return false;
}

// FUNCTION: 0x00638DC0
void __fastcall FUN_00638DC0(PlanningNodeClass * node)
{
	PlanningNodeClass::Unknown1.Add(node);
}

// FUNCTION: 0x00638E20
void __fastcall FUN_00638E20(void)
{
	PlanningNodeClass::Unknown2.Clear();
}

extern bool DAT_00AC4CB0;
extern int DAT_00AC4C3C;
extern bool DAT_00AC4C08;
int __fastcall FUN_0063AC70(PlanningNodeClass * node);
void __fastcall FUN_00634B20(PlanningNodeClass * node);

// FUNCTION: 0x00638E60
void __fastcall FUN_00638E60(void)
{
	if (!DAT_00AC4CB0) {
		int x = MouseCursor->Get_Mouse_X();
		int y = MouseCursor->Get_Mouse_Y();
		int count = PlanningNodeClass::Unknown2.Count();
		PlanningNodeClass * node;
		int i = 0;
		if (count > 0) {
			do {
				node = PlanningNodeClass::Unknown2[i];
				if (node->Hit_Test(x, y)) goto found;
				i++;
			} while (i < count);
		}
		node = 0;
found:
		if (node != DAT_00AC4CCC) {
			DAT_00AC4C3C = 0;
			if (node) {
				if (node->PlanningMembers[0]->Owner->House == PlayerPtr
					&& (DAT_00AC4C38 == 0 || DAT_00AC4C38 == node)
					&& FUN_0063AC70(node) == -1) {
				} else {
					DAT_00AC4C3C = (int)node;
					node = 0;
				}
			}
			DAT_00AC4CCC = node;
			if (node) {
				FUN_00634B20(node);
			}
		}
	}
}







bool __fastcall FUN_00639E30(PlanningTokenClass * a, PlanningTokenClass * b);
void __fastcall FUN_00730A90(wchar_t const * text, int timeout);

// FUNCTION: 0x00639040
bool __fastcall FUN_00639040(void)
{
	if (!PlanningNodeClass::PlanningModeActive) {
		return true;
	}
	int count = CurrentObject.Count();
	if (count == 0) {
		return true;
	}
	int i;
	for (i = 0; i < count; i++) {
		ObjectClass * object = CurrentObject[i];
		if (object->Is_Techno()) {
			PlanningTokenClass * first = ((TechnoClass *)object)->FUN_00705D20();
			for (i = 0; i < count; i++) {
				ObjectClass * other = CurrentObject[i];
				if (!other->Is_Techno()) goto fail;
				PlanningTokenClass * t = ((TechnoClass *)other)->FUN_00705D20();
				if (!FUN_00639E30(first, t)) goto fail;
			}
			return true;
		}
	}
	return false;
fail:
	if (!DAT_00AC4C08) {
		DAT_00AC4C08 = true;
		wchar_t const * text = StringTable::LoadString("MSG:PlanningModeHeteroSel", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
		FUN_00730A90(text, 0x1e0);
		VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
	}
	return false;
}

static inline bool Slot_Valid(int slot)
{
	return slot >= 0 && slot < 24;
}

static inline int Used_Count(HouseClass * house)
{
	int slot = house->HeapID;
	bool valid = Slot_Valid(slot);
	int used = 0;
	if (valid) {
		used = DAT_00AC4B84[slot];
	}
	return used;
}

// FUNCTION: 0x00639130
bool __fastcall FUN_00639130(void)
{
	if (PlanningNodeClass::PlanningModeActive) {
		int used = Used_Count(PlayerPtr);
		int count = CurrentObject.Count();
		int fresh = 0;
		for (int i = 0; i < count; i++) {
			ObjectClass * object = CurrentObject[i];
			if (object->Is_Techno()) {
				PlanningTokenClass * token = ((TechnoClass *)object)->FUN_00705D20();
				if (!token) {
					fresh++;
				} else if (token->PlanningNodes.Count() == 0) {
					fresh++;
				}
			}
		}
		if (used + fresh > 128) {
			if (!DAT_00AC4C08) {
				DAT_00AC4C08 = true;
				wchar_t const * text = StringTable::LoadString("MSG:PlannerMaximum", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
				FUN_00730A90(text, 0x1e0);
				VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
			}
			return false;
		}
	}
	return true;
}

// FUNCTION: 0x00639210
PlanningNodeClass * __fastcall FUN_00639210(int house, int id, Coord coord)
{
	int count = PlanningNodeClass::Unknown1.Count();
	for (int i = 0; i < count; i++) {
		PlanningNodeClass * node = PlanningNodeClass::Unknown1[i];
		Coord at = FUN_00633BC0(*node->PlanningMembers[0]->Packet);
		if (at.X == coord.X && at.Y == coord.Y && at.Z == coord.Z) {
			if (id == -1 || id == node->Id) {
				int owner = FUN_006339B0(node->PlanningMembers[0]->Owner);
				if (house == -1 || house == owner) {
					return node;
				}
			}
		}
	}
	return 0;
}

static inline Coord Node_Lead_0(PlanningNodeClass * n) { return n->Lead_Coord(); }
static inline Coord Node_Lead_1(PlanningNodeClass * n) { return Node_Lead_0(n); }
static inline Coord Node_Lead_2(PlanningNodeClass * n) { return Node_Lead_1(n); }
static inline Coord Node_Lead_3(PlanningNodeClass * n) { return Node_Lead_2(n); }
static inline Coord Node_Lead_4(PlanningNodeClass * n) { return Node_Lead_3(n); }
static inline Coord Node_Lead_5(PlanningNodeClass * n) { return Node_Lead_4(n); }
static inline Coord Node_Lead_6(PlanningNodeClass * n) { return Node_Lead_5(n); }
static inline Coord Node_Lead_7(PlanningNodeClass * n) { return Node_Lead_6(n); }
static inline void Box_Into_0(Rect * r, int x, int y, int w, int h) { r->TRect<int>::TRect(x, y, w, h); }
static inline void Box_Into_1(Rect * r, int x, int y, int w, int h) { Box_Into_0(r, x, y, w, h); }
static inline void Box_Into_2(Rect * r, int x, int y, int w, int h) { Box_Into_1(r, x, y, w, h); }
static inline void Box_Into_3(Rect * r, int x, int y, int w, int h) { Box_Into_2(r, x, y, w, h); }
static inline void Box_Into_4(Rect * r, int x, int y, int w, int h) { Box_Into_3(r, x, y, w, h); }
static inline void Box_Into_5(Rect * r, int x, int y, int w, int h) { Box_Into_4(r, x, y, w, h); }
static inline void Box_Into_6(Rect * r, int x, int y, int w, int h) { Box_Into_5(r, x, y, w, h); }
static inline void Box_Into_7(Rect * r, int x, int y, int w, int h) { Box_Into_6(r, x, y, w, h); }

static inline void Grow(Rect * rect, int amount)
{
	int twice = amount * 2;
	if (twice < 0 && (twice >= rect->Width || twice >= rect->Height)) {
		rect->X = 0;
		rect->Y = 0;
		rect->Width = 0;
		rect->Height = 0;
		return;
	}
	rect->X -= amount;
	rect->Y -= amount;
	rect->Width += twice;
	rect->Height += twice;
}

static inline Rect Boxed(Rect r) { Rect b = r; Grow(&b, 4); return b; }

// FUNCTION: 0x006392D0
PlanningNodeClass * __fastcall FUN_006392D0(Point2D point)
{
	int count = PlanningNodeClass::Unknown1.Count();
	int i = 0;
	if (count > 0) {
		do {
			PlanningNodeClass * node = PlanningNodeClass::Unknown1[i];
			Coord coord = Node_Lead_7(node);
			Point2D corner = Pixel_Of(coord);
			corner.X -= 4;
			corner.Y -= 4;
			Rect rect;
			Box_Into_7(&rect, corner.X, corner.Y, 8, 8);
			Rect box = Boxed(rect);
			if (box.Is_Point_Within(point)) {
				return node;
			}
			i++;
		} while (i < count);
	}
	return 0;
}

// FUNCTION: 0x006393A0
PlanningNodeClass * __fastcall FUN_006393A0(Point2D point)
{
	int count = PlanningNodeClass::Unknown2.Count();
	int i = 0;
	if (count > 0) {
		do {
			PlanningNodeClass * node = PlanningNodeClass::Unknown2[i];
			Coord coord = Node_Lead_7(node);
			Point2D corner = Pixel_Of(coord);
			corner.X -= 4;
			corner.Y -= 4;
			Rect rect;
			Box_Into_7(&rect, corner.X, corner.Y, 8, 8);
			Rect box = Boxed(rect);
			if (box.Is_Point_Within(point)) {
				return node;
			}
			i++;
		} while (i < count);
	}
	return 0;
}

// FUNCTION: 0x00639470
PlanningNodeClass * __fastcall FUN_00639470(Coord coord)
{
	int count = PlanningNodeClass::Unknown2.Count();
	int i = 0;
	if (count > 0) {
		do {
			PlanningNodeClass * node = PlanningNodeClass::Unknown2[i];
			Coord at = FUN_00633BC0(*node->PlanningMembers[0]->Packet);
			if (at.X == coord.X && at.Y == coord.Y && at.Z == coord.Z) {
				return node;
			}
			i++;
		} while (i < count);
	}
	return 0;
}

// FUNCTION: 0x006394E0
void __fastcall FUN_006394E0(AbstractClass * source, PlanningNodeClass * node)
{
	EventClass event = *node->PlanningMembers[0]->Packet;
	xTargetClass * target;
	if (event.Data.MegaMission.Target.Is_Valid()) {
		target = &event.Data.MegaMission.Target;
	} else if (event.Data.MegaMission.Follow.Is_Valid()) {
		target = &event.Data.MegaMission.Follow;
	} else if (event.Data.MegaMission.Destination.Is_Valid()) {
		target = &event.Data.MegaMission.Destination;
	} else {
		return;
	}
	AbstractClass * object = target->As_Abstract();
	if (object) {
		EventClass connect(PlayerPtr->HeapID, EventClass::PLAN_CONNECT, TargetClass(source), TargetClass(object));
		FUN_006521C0(connect);
	}
}

void __fastcall FUN_00639800(TechnoClass * techno, PlanningNodeClass * node);
extern bool DAT_00AC4CD0;
void __fastcall FUN_00639CB0(TechnoClass * techno);

static inline void Token_Add_0(PlanningTokenClass * const & token) { PlanningTokenClass::Array.Add(token); }
static inline void Token_Add_1(PlanningTokenClass * const & token) { Token_Add_0(token); }
static inline void Token_Add_2(PlanningTokenClass * const & token) { Token_Add_1(token); }
static inline void Token_Add_3(PlanningTokenClass * const & token) { Token_Add_2(token); }
static inline void Token_Add_4(PlanningTokenClass * const & token) { Token_Add_3(token); }
static inline void Token_Add_5(PlanningTokenClass * const & token) { Token_Add_4(token); }
static inline void Token_Add_6(PlanningTokenClass * const & token) { Token_Add_5(token); }
static inline void Token_Add_7(PlanningTokenClass * const & token) { Token_Add_6(token); }

static inline void Close_Loop(TechnoClass * techno, PlanningNodeClass * node)
{
	PlanningTokenClass * token = techno->FUN_00705D20();
	int index = token->Node_Index(node);
	token = techno->FUN_00705D20();
	token->Set_Loop(token->Node_Count() - index, index);
	if (!DAT_00AC4CC8) {
		if (techno->House->Is_Player_Control()) {
			DAT_00AC4CC8 = true;
			PlanningNodeClass * last = token->Last_Node();
			int count = last->Get_Count();
			for (int i = 0; i < count; i++) {
				TechnoClass * owner = last->Member_Owner(i);
				if (owner->IsSelected) {
					owner->Unselect();
				}
			}
		}
		FUN_00639CB0(techno);
	}
	node->Update_Active();
}

// FUNCTION: 0x006395B0
void __fastcall FUN_006395B0(TechnoClass * techno, PlanningNodeClass * node)
{
	PlanningTokenClass * token = techno->FUN_00705D20();
	if (!token) {
		PlanningTokenClass * created = new PlanningTokenClass(techno);
		Token_Add_7(created);
		techno->FUN_00705D10(created);
		token = created;
	}
	if (token->Has_Node(node)) {
		Close_Loop(techno, node);
	} else {
		FUN_00639800(techno, node);
	}
}

// FUNCTION: 0x00639740
void __fastcall FUN_00639740(TechnoClass * techno, PlanningNodeClass * node)
{
	Close_Loop(techno, node);
}


// FUNCTION: 0x00639F80
void __fastcall FUN_00639F80(int slot)
{
	if (slot >= 0 && slot < 24) {
		int value = DAT_00AC4B84[slot] - 1;
		DAT_00AC4B84[slot] = (value < 0) ? 0 : value;
	}
}
