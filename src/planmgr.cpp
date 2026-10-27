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
#include "foot.h"
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

// SYNTHETIC: GAMEMD 0x006333D0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00633400 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00633420 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00633440 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00633460 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00633480 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006334A0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006334E0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00633510 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00633540 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00633570 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00633580 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006335A0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006335C0 SYMBOL
// _$E42

// SYNTHETIC: GAMEMD 0x006335E0 SYMBOL
// _$E109
// SYNTHETIC: GAMEMD 0x00633620 SYMBOL
// _$E106
// SYNTHETIC: GAMEMD 0x00633680 SYMBOL
// _$E115
// SYNTHETIC: GAMEMD 0x006336C0 SYMBOL
// _$E112
// SYNTHETIC: GAMEMD 0x00633720 SYMBOL
// _$E121
// SYNTHETIC: GAMEMD 0x00633760 SYMBOL
// _$E118
// SYNTHETIC: GAMEMD 0x006337C0 SYMBOL
// _$E127
// SYNTHETIC: GAMEMD 0x00633800 SYMBOL
// _$E124
// SYNTHETIC: GAMEMD 0x00633860 SYMBOL
// _$E132
// SYNTHETIC: GAMEMD 0x006338A0 SYMBOL
// _$E130
// SYNTHETIC: GAMEMD 0x00633900 SYMBOL
// _$E137
// SYNTHETIC: GAMEMD 0x00633940 SYMBOL
// _$E135

// FUNCTION: GAMEMD 0x006339A0
void __fastcall FUN_006339A0(void)
{
}

// FUNCTION: GAMEMD 0x006339B0
int __fastcall FUN_006339B0(TechnoClass * techno)
{
	HouseClass * house = techno->House;
	int index = house->HeapID;
	if (house->Is_Player_Control() && PlayerPtr) {
		index = PlayerPtr->HeapID;
	}
	return index;
}

// FUNCTION: GAMEMD 0x006339E0
Point2D __fastcall FUN_006339E0(Coord const & coord)
{
	Point2D pixel;
	TacticalMap->Coord_To_Pixel(coord, pixel);
	pixel.X += DSurface::ViewBounds.X;
	pixel.Y += DSurface::ViewBounds.Y;
	return pixel;
}

// FUNCTION: GAMEMD 0x00633A20
PlanningMemberClass::PlanningMemberClass(void) : Owner(0), Packet(0), Branch(-1), Active(false)
{
}

// FUNCTION: GAMEMD 0x00633A40
void PlanningMemberClass::Set_Owner(TechnoClass * owner)
{
	Owner = owner;
}

// FUNCTION: GAMEMD 0x00633A50
void PlanningMemberClass::Set_Packet(EventClass * packet)
{
	Packet = packet;
}

// FUNCTION: GAMEMD 0x00633A60
void PlanningMemberClass::Set_Branch(int branch)
{
	Branch = branch;
}

// FUNCTION: GAMEMD 0x00633A70
void PlanningMemberClass::Set_Active(bool active)
{
	Active = active;
}

// FUNCTION: GAMEMD 0x00633A80
TechnoClass * PlanningMemberClass::Get_Owner(void)
{
	return Owner;
}

// FUNCTION: GAMEMD 0x00633A90
EventClass * PlanningMemberClass::Get_Packet(void)
{
	return Packet;
}

// FUNCTION: GAMEMD 0x00633AA0
int PlanningMemberClass::Get_Branch(void)
{
	return Branch;
}

// FUNCTION: GAMEMD 0x00633AB0
bool PlanningMemberClass::Get_Active(void)
{
	return Active;
}

// FUNCTION: GAMEMD 0x0063D630
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

// SYNTHETIC: GAMEMD 0x0063D9C0
// ??_GPlanningNodeClass@@QAEPAXI@Z

// TEMPLATE: GAMEMD 0x0063DB60
// ?Clear@?$DynamicVectorClass@PAVPlanningMemberClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0063DB90
// ?ID@?$DynamicVectorClass@PAVPlanningMemberClass@@@@UAEHPBQAVPlanningMemberClass@@@Z
// TEMPLATE: GAMEMD 0x0063DBB0
// ?GetItem@?$VectorClass@PAVPlanningMemberClass@@@@UBEPAVPlanningMemberClass@@H@Z
// TEMPLATE: GAMEMD 0x0063DBC0
// ?Clear@?$DynamicVectorClass@PAVPlanningBranchClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0063DBF0
// ?ID@?$DynamicVectorClass@PAVPlanningBranchClass@@@@UAEHPBQAVPlanningBranchClass@@@Z
// TEMPLATE: GAMEMD 0x0063DC10
// ?GetItem@?$VectorClass@PAVPlanningBranchClass@@@@UBEPAVPlanningBranchClass@@H@Z
// TEMPLATE: GAMEMD 0x0063DC20
// ?Clear@?$DynamicVectorClass@PAVPlanningNodeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0063DC50
// ?ID@?$DynamicVectorClass@PAVPlanningNodeClass@@@@UAEHPBQAVPlanningNodeClass@@@Z
// TEMPLATE: GAMEMD 0x0063DC70
// ?GetItem@?$VectorClass@PAVPlanningNodeClass@@@@UBEPAVPlanningNodeClass@@H@Z
// TEMPLATE: GAMEMD 0x0063DE30
// ?Clear@?$VectorClass@PAVPlanningMemberClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0063DFD0
// ?Delete@?$DynamicVectorClass@PAVPlanningMemberClass@@@@QAE_NABQAVPlanningMemberClass@@@Z
// TEMPLATE: GAMEMD 0x0063E140
// ?Clear@?$VectorClass@PAVPlanningBranchClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0063E590
// ?Delete_Index@?$DynamicVectorClass@PAVPlanningNodeClass@@@@QAE_NH@Z
// TEMPLATE: GAMEMD 0x0063E5D0
// ?Delete@?$DynamicVectorClass@PAVPlanningNodeClass@@@@QAE_NABQAVPlanningNodeClass@@@Z
// TEMPLATE: GAMEMD 0x0063E900
// ?Delete@?$DynamicVectorClass@PAVTechnoClass@@@@QAE_NABQAVTechnoClass@@@Z
// TEMPLATE: GAMEMD 0x0063EBD0
// ?Delete@?$DynamicVectorClass@PAVPlanningTokenClass@@@@QAE_NABQAVPlanningTokenClass@@@Z
// TEMPLATE: GAMEMD 0x0063EF70
// ?Delete_Index@?$DynamicVectorClass@PAVPlanningMemberClass@@@@QAE_NH@Z
// TEMPLATE: GAMEMD 0x0063F040
// ?Delete_Index@?$DynamicVectorClass@PAVPlanningTokenClass@@@@QAE_NH@Z
// TEMPLATE: GAMEMD 0x0063E540
// ?Add@?$DynamicVectorClass@PAVPlanningNodeClass@@@@QAE_NABQAVPlanningNodeClass@@@Z
template class DynamicVectorClass<PlanningNodeClass *>;

// SYNTHETIC: GAMEMD 0x0063EE30
// ??_G?$VectorClass@PAVEventClass@@@@UAEPAXI@Z
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

// FUNCTION: GAMEMD 0x00633AC0
PlanningBranchClass::PlanningBranchClass(void) : Count(0), Color(-1)
{
}

// FUNCTION: GAMEMD 0x00633AE0
void PlanningBranchClass::Set_Event(EventClass const & event)
{
	Event = event;
}

// FUNCTION: GAMEMD 0x00633B00
void PlanningBranchClass::Set_Count(int count)
{
	Count = count;
}

// FUNCTION: GAMEMD 0x00633B10
void PlanningBranchClass::Set_Color(int color)
{
	Color = color;
}

// FUNCTION: GAMEMD 0x00633B20
void PlanningBranchClass::Clear_Count(void)
{
	Count = 0;
}

// FUNCTION: GAMEMD 0x00633B30
int PlanningBranchClass::Inc_Count(void)
{
	return ++Count;
}

// FUNCTION: GAMEMD 0x00633B40
int PlanningBranchClass::Dec_Count(void)
{
	return --Count;
}

// FUNCTION: GAMEMD 0x00633B50
EventClass PlanningBranchClass::Get_Event(void)
{
	return Event;
}

// FUNCTION: GAMEMD 0x00633B70
int PlanningBranchClass::Get_Count(void)
{
	return Count;
}

// FUNCTION: GAMEMD 0x00633B80
int PlanningBranchClass::Get_Color(void)
{
	return Color;
}

// FUNCTION: GAMEMD 0x00633B90
Coord PlanningBranchClass::Get_Coord(void)
{
	return Coord(FUN_00633BC0(Event));
}

// FUNCTION: GAMEMD 0x00633BC0
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

// FUNCTION: GAMEMD 0x00633C60
Point2D PlanningBranchClass::Get_Pixel(void)
{
	Coord coord = FUN_00633BC0(Event);
	return FUN_006339E0(coord);
}

// FUNCTION: GAMEMD 0x00633CC0
PlanningNodeClass::PlanningNodeClass(int id) : Id(id), Queued(false), LoopBranchIndex(-1), Color(-1)
{
}

// FUNCTION: GAMEMD 0x00633D30
PlanningNodeClass::~PlanningNodeClass()
{
	Clear();
	FUN_00637640(this);
}

// FUNCTION: GAMEMD 0x00633E50
int PlanningNodeClass::Get_Id(void)
{
	return Id;
}

// FUNCTION: GAMEMD 0x00633E60
int PlanningNodeClass::Get_Count(void)
{
	return PlanningMembers.Count();
}

// FUNCTION: GAMEMD 0x00633E70
int PlanningNodeClass::Index_In(TechnoClass * techno)
{
	PlanningTokenClass * token = techno->FUN_00705D20();
	return token->PlanningNodes.ID(this);
}

// FUNCTION: GAMEMD 0x00633EA0
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

// FUNCTION: GAMEMD 0x00633F20
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

// FUNCTION: GAMEMD 0x00633FA0
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

// FUNCTION: GAMEMD 0x006340B0
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

// FUNCTION: GAMEMD 0x00634290
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

// FUNCTION: GAMEMD 0x006342D0
TechnoClass * PlanningNodeClass::Member_Owner(int index)
{
	return PlanningMembers[index]->Owner;
}

Rect __fastcall FUN_00634510(int size, Point2D center);

// FUNCTION: GAMEMD 0x006342E0
EventClass PlanningNodeClass::Get_Event(int index)
{
	return *PlanningMembers[index]->Packet;
}

// FUNCTION: GAMEMD 0x00634310
EventClass PlanningNodeClass::Get_Event_For(TechnoClass * techno)
{
	return *Get_Member(techno)->Packet;
}

// FUNCTION: GAMEMD 0x00634360
bool PlanningNodeClass::Has_Member(TechnoClass * techno)
{
	return Find_Member(techno) != -1;
}

// FUNCTION: GAMEMD 0x006343B0
HouseClass * PlanningNodeClass::Lead_House(void)
{
	return PlanningMembers[0]->Owner->House;
}

// FUNCTION: GAMEMD 0x006343C0
Coord PlanningNodeClass::Lead_Coord(void)
{
	Coord coord = FUN_00633BC0(*PlanningMembers[0]->Packet);
	return coord;
}

// FUNCTION: GAMEMD 0x00634400
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

// FUNCTION: GAMEMD 0x00634470
bool PlanningNodeClass::Hit_Test(int x, int y)
{
	Rect rect = Lead_Box_Rect();
	return rect.Is_Point_Within(Point2D(x, y));
}

// FUNCTION: GAMEMD 0x00634510
Rect __fastcall FUN_00634510(int size, Point2D center)
{
	int offset = -(size / 2);
	Point2D corner = center;
	corner.X += offset;
	corner.Y += offset;
	return Rect(corner, size, size);
}

// FUNCTION: GAMEMD 0x00634550
bool PlanningNodeClass::Lead_At(Coord coord)
{
	Coord c = FUN_00633BC0(*PlanningMembers[0]->Packet);
	return c.X == coord.X && c.Y == coord.Y && c.Z == coord.Z;
}

// FUNCTION: GAMEMD 0x006345A0
Rect PlanningNodeClass::Lead_Rect(void)
{
	return FUN_00634510(8, Lead_Pixel());
}

// FUNCTION: GAMEMD 0x00634630
bool PlanningNodeClass::Is_Past_Start(void)
{
	if (PlanningMembers.Count() == 0) return false;
	PlanningTokenClass * token = PlanningMembers[0]->Owner->FUN_00705D20();
	int index = token->PlanningNodes.ID(this);
	int start = token->StepsToClosedLoop;
	return start != -1 && index >= start;
}

// FUNCTION: GAMEMD 0x00634680
void PlanningNodeClass::Promote_Events(void)
{
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		EventClass * packet = PlanningMembers[i]->Packet;
		if (packet->Data.MegaMission.Mission == 2) packet->Data.MegaMission.Mission = 0x1d;
	}
}

// FUNCTION: GAMEMD 0x006346B0
PlanningMemberClass * PlanningNodeClass::Get_Member(TechnoClass * techno)
{
	int index = Find_Member(techno);
	if (index == -1) {
		return NULL;
	}
	return PlanningMembers[index];
}

// FUNCTION: GAMEMD 0x00634700
int PlanningNodeClass::Branch_Count(void)
{
	return PlanningBranches.Count();
}

// FUNCTION: GAMEMD 0x00634710
int PlanningNodeClass::Has_Loop(void)
{
	return LoopBranch.Count > 0;
}

// FUNCTION: GAMEMD 0x00634720
int PlanningNodeClass::Is_Loop_Alone(void)
{
	if (LoopBranch.Count > 0 && LoopBranchIndex == -1) return 1;
	return 0;
}

// FUNCTION: GAMEMD 0x00634740
TechnoClass * PlanningNodeClass::Owner_At_Branch(int branch)
{
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		PlanningMemberClass * m = PlanningMembers[i];
		if (m->Branch == branch) return m->Owner;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00634770
TechnoClass * PlanningNodeClass::Active_Owner(void)
{
	int count = PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		PlanningMemberClass * m = PlanningMembers[i];
		if (m->Active) return m->Owner;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x006347A0
PlanningBranchClass * PlanningNodeClass::Get_Branch(int index)
{
	return PlanningBranches[index];
}

// FUNCTION: GAMEMD 0x006347B0
PlanningBranchClass * PlanningNodeClass::Branch_Of(TechnoClass * techno)
{
	PlanningMemberClass * member = Get_Member(techno);
	int branch = member->Branch;
	PlanningBranchClass * result = 0;
	if (branch != -1) result = PlanningBranches[branch];
	return result;
}

// FUNCTION: GAMEMD 0x00634800
EventClass * PlanningNodeClass::Loop_Event(void)
{
	return &LoopBranch.Event;
}

// FUNCTION: GAMEMD 0x00634810
Coord PlanningNodeClass::Branch_Coord(int index)
{
	Coord coord = FUN_00633BC0(PlanningBranches[index]->Event);
	return coord;
}

// FUNCTION: GAMEMD 0x00634850
Coord PlanningNodeClass::Loop_Coord(void)
{
	Coord coord = FUN_00633BC0(LoopBranch.Event);
	return coord;
}

// FUNCTION: GAMEMD 0x00634880
Point2D PlanningNodeClass::Branch_Pixel(int index)
{
	return PlanningBranches[index]->Get_Pixel();
}

// FUNCTION: GAMEMD 0x006348F0
Point2D PlanningNodeClass::Loop_Pixel(void)
{
	return LoopBranch.Get_Pixel();
}

// FUNCTION: GAMEMD 0x00634950
void PlanningNodeClass::Set_Queued(void)
{
	Queued = true;
}

// FUNCTION: GAMEMD 0x00634960
bool PlanningNodeClass::Get_Queued(void)
{
	return Queued;
}

// FUNCTION: GAMEMD 0x00634970
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

// FUNCTION: GAMEMD 0x006349B0
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

// FUNCTION: GAMEMD 0x00634CC0
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

// FUNCTION: GAMEMD 0x00635060
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

// FUNCTION: GAMEMD 0x00635120
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

// FUNCTION: GAMEMD 0x006351A0
bool PlanningNodeClass::Is_Loop_Start(TechnoClass * techno)
{
	PlanningTokenClass * token = techno->FUN_00705D20();
	return token->PlanningNodes.ID(this) == token->StepsToClosedLoop;
}

// FUNCTION: GAMEMD 0x006351E0
void PlanningNodeClass::Deactivate(PlanningMemberClass * member)
{
	member->Set_Active(false);
	if (LoopBranch.Dec_Count() == 0) {
		LoopBranchIndex = -1;
	}
}

// FUNCTION: GAMEMD 0x00635210
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

// FUNCTION: GAMEMD 0x006352D0
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

// FUNCTION: GAMEMD 0x00635310
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

// FUNCTION: GAMEMD 0x00635490
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

// FUNCTION: GAMEMD 0x00635560
void PlanningNodeClass::Frame_Lead_Box(int color)
{
	Rect clip = FUN_0072AD20();
	Rect rect = Inflated(Around(8, Pixel_Of(Event_Coord(*PlanningMembers[0]->Packet))), 4);
	for (int i = 0; i < 3; i++) {
		DSurface::Composite->Draw_Rect(clip, rect, color);
		Shrink_Rect(rect);
	}
}

// FUNCTION: GAMEMD 0x006357D0
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

// FUNCTION: GAMEMD 0x0063D070
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

// FUNCTION: GAMEMD 0x00635830
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

// FUNCTION: GAMEMD 0x00635DB0
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

// FUNCTION: GAMEMD 0x00635DF0
Rect PlanningNodeClass::Lead_Box_Rect(void)
{
	Rect rect = Lead_Rect();
	FUN_006357D0(&rect, 4);
	return rect;
}

// FUNCTION: GAMEMD 0x00635E90
Rect PlanningNodeClass::Lead_Box_Rect2(void)
{
	return Inflated(Inflated(Around(8, Pixel_Of(Event_Coord(*PlanningMembers[0]->Packet))), 4), 4);
}

// FUNCTION: GAMEMD 0x00635F20
PlanningTokenClass::PlanningTokenClass(TechnoClass * owner) : F1C(false), F8C(-1), ClosedLoopNodeCount(-1), StepsToClosedLoop(-1), F98(false), F99(false)
{
	OwnerUnit = owner;
}

// FUNCTION: GAMEMD 0x00635F80
PlanningTokenClass::~PlanningTokenClass()
{
	if (OwnerUnit) {
		OwnerUnit->FUN_00705D10(0);
	}
	FUN_00637750(this);
	Cancel_Owner();
}

// FUNCTION: GAMEMD 0x00636110
TechnoClass * PlanningTokenClass::Get_Owner(void)
{
	return OwnerUnit;
}

// FUNCTION: GAMEMD 0x00636120
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

// FUNCTION: GAMEMD 0x00636310
void PlanningTokenClass::Destroy(void)
{
	Cancel_Owner();
	OwnerUnit = 0;
	delete this;
}

// FUNCTION: GAMEMD 0x00636520
void PlanningTokenClass::Set_F1C(void)
{
	F1C = true;
}

// FUNCTION: GAMEMD 0x00636530
void PlanningTokenClass::Clear_F1C(void)
{
	F1C = false;
}

// FUNCTION: GAMEMD 0x00636540
bool PlanningTokenClass::Get_F1C(void)
{
	return F1C;
}

// FUNCTION: GAMEMD 0x00636550
bool PlanningTokenClass::Owner_Is_Mission(void)
{
	return OwnerUnit->CurrentMission == 0x1c || OwnerUnit->MissionQueue == 0x1c;
}

// FUNCTION: GAMEMD 0x00636570
bool PlanningTokenClass::Has_Started(void)
{
	return PlanningNodes.Count() > 0 && (F8C == -1 || F8C > 0);
}

// FUNCTION: GAMEMD 0x006367B0
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

// FUNCTION: GAMEMD 0x00636980
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

// FUNCTION: GAMEMD 0x00636D90
int PlanningTokenClass::Get_F8C(void)
{
	return F8C;
}

// FUNCTION: GAMEMD 0x00636DA0
bool PlanningTokenClass::Reached(int index)
{
	if (F8C == -1) return false;
	return index >= F8C;
}

// FUNCTION: GAMEMD 0x00636DC0
int PlanningTokenClass::Node_Count(void)
{
	return PlanningNodes.Count();
}

// FUNCTION: GAMEMD 0x00636DD0
int PlanningTokenClass::Current_Or_Count(void)
{
	if (F8C == -1) return PlanningNodes.Count();
	return F8C;
}

// FUNCTION: GAMEMD 0x00636DE0
int PlanningTokenClass::Remaining(void)
{
	if (F8C == -1) return 0;
	return PlanningNodes.Count() - F8C;
}

// FUNCTION: GAMEMD 0x00636E00
void PlanningTokenClass::Add_Node(PlanningNodeClass * node)
{
	PlanningNodes.Add(node);
	if (F8C == -1) {
		F8C = PlanningNodes.Count() - 1;
	}
}

// FUNCTION: GAMEMD 0x00636E60
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

// FUNCTION: GAMEMD 0x00636EB0
PlanningNodeClass * PlanningTokenClass::Last_Node(void)
{
	int index = PlanningNodes.Count() - 1;
	if (index < 0) return 0;
	return PlanningNodes[index];
}

// FUNCTION: GAMEMD 0x00636EC0
bool PlanningTokenClass::Has_Node(PlanningNodeClass * node)
{
	return PlanningNodes.ID(node) != -1;
}

// FUNCTION: GAMEMD 0x00636EE0
int PlanningTokenClass::Node_Index(PlanningNodeClass * node)
{
	return PlanningNodes.ID(node);
}

// FUNCTION: GAMEMD 0x00636F00
EventClass PlanningTokenClass::Event_At(int index)
{
	PlanningNodeClass * node = PlanningNodes[index];
	TechnoClass * owner = OwnerUnit;
	return *node->Get_Member(owner)->Packet;
}

// FUNCTION: GAMEMD 0x00636F60
EventClass PlanningTokenClass::Last_Popped(void)
{
	return Popped;
}

// FUNCTION: GAMEMD 0x00636F80
PlanningNodeClass * PlanningTokenClass::Loop_Node(void)
{
	if (StepsToClosedLoop == -1) return 0;
	return PlanningNodes[StepsToClosedLoop];
}

// FUNCTION: GAMEMD 0x00636FA0
void PlanningTokenClass::Set_Loop(int count, int start)
{
	ClosedLoopNodeCount = count;
	StepsToClosedLoop = start;
	F98 = true;
}

// FUNCTION: GAMEMD 0x00636FC0
int PlanningTokenClass::Get_Loop_Count(void)
{
	return ClosedLoopNodeCount;
}

// FUNCTION: GAMEMD 0x00636FD0
int PlanningTokenClass::Get_Loop_Start(void)
{
	return StepsToClosedLoop;
}

// FUNCTION: GAMEMD 0x00636FE0
bool PlanningTokenClass::Loop_Start_Valid(void)
{
	return StepsToClosedLoop > 0 || StepsToClosedLoop == 0;
}

// FUNCTION: GAMEMD 0x00637000
bool PlanningTokenClass::Loop_Start_Positive(void)
{
	return StepsToClosedLoop > 0;
}

// FUNCTION: GAMEMD 0x00637010
bool PlanningTokenClass::Loop_Start_Zero(void)
{
	return StepsToClosedLoop == 0;
}

// FUNCTION: GAMEMD 0x00637020
bool PlanningTokenClass::Get_F98(void)
{
	return F98;
}

// FUNCTION: GAMEMD 0x00637030
bool PlanningTokenClass::Get_F99(void)
{
	return F99;
}

// FUNCTION: GAMEMD 0x00637040
void PlanningTokenClass::Dec_Loop_Start(void)
{
	StepsToClosedLoop--;
	StepsToClosedLoop = (StepsToClosedLoop <= 0) ? 0 : StepsToClosedLoop;
}

extern bool DAT_00AC4CB0;
extern Point2D DAT_00AC4C68;

// FUNCTION: GAMEMD 0x00637060
void __fastcall FUN_00637060(INoticeSink * value)
{
	DAT_00AC4B80 = value;
}

// FUNCTION: GAMEMD 0x00637070
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

// FUNCTION: GAMEMD 0x006370A0
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
void __fastcall FUN_006DA740(Tactical * tactical);
extern int DAT_00AC4C90;

// FUNCTION: GAMEMD 0x006370B0
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

// FUNCTION: GAMEMD 0x00637550
void __fastcall FUN_00637550(void)
{
	for (int i = PlanningTokenClass::Events.Count() - 1; i >= 0; i--) {
		EventClass * event = PlanningTokenClass::Events[i];
		event->FUN_00652230();
		PlanningTokenClass::Events.Delete_Index(i);
		delete event;
	}
}

// FUNCTION: GAMEMD 0x006375B0
void __fastcall FUN_006375B0(EventClass const & source)
{
	EventClass * event = new EventClass;
	*event = source;
	if (event->Type == EventClass::MEGAMISSION) {
		*(unsigned char *)&event->Data.MegaMission.IsPlanningEvent = 2;
	}
	PlanningTokenClass::Events.Add(event);
}

// FUNCTION: GAMEMD 0x00637640
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

// FUNCTION: GAMEMD 0x00637750
void __fastcall FUN_00637750(PlanningTokenClass * token)
{
	if (!DAT_00AC4CF5) {
		PlanningTokenClass::Array.Delete(token);
	}
}

// FUNCTION: GAMEMD 0x006377B0
void __fastcall FUN_006377B0(TechnoClass * techno)
{
	int slot = FUN_006339B0(techno);
	FUN_00639F80(slot);
	PlanningTokenClass::Technos.Delete(techno);
}

// FUNCTION: GAMEMD 0x00637840
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

// FUNCTION: GAMEMD 0x006378B0
void __fastcall FUN_006378B0(PlanningNodeClass * node)
{
	if (node && !node->Get_Queued()) {
		node->Set_Queued();
		PlanningNodeClass::Unknown3.Add(node);
	}
}


void __fastcall FUN_006521C0(EventClass event);

// FUNCTION: GAMEMD 0x00637920
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

// FUNCTION: GAMEMD 0x006379C0
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

// FUNCTION: GAMEMD 0x00637A10
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

// FUNCTION: GAMEMD 0x00637AA0
bool __fastcall FUN_00637AA0(void)
{
	return PlanningNodeClass::PlanningModeActive;
}

// FUNCTION: GAMEMD 0x00637D00
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

// FUNCTION: GAMEMD 0x00637DB0
bool __fastcall FUN_00637DB0(int type)
{
	if (PlanningNodeClass::PlanningModeActive && type == 4) {
		return false;
	}
	return true;
}

// FUNCTION: GAMEMD 0x00637DD0
void __fastcall FUN_00637DD0(EventClass event)
{
	if (event.Type == EventClass::MEGAMISSION) {
		event.Data.MegaMission.IsPlanningEvent = true;
		FUN_006521C0(event);
	}
}

// FUNCTION: GAMEMD 0x006385C0
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

// FUNCTION: GAMEMD 0x00638A20
void __fastcall FUN_00638A20(PlanningNodeClass * node)
{
	PlanningNodeClass::Unknown2.Add(node);
}

// FUNCTION: GAMEMD 0x00638A80
PlanningTokenClass * __fastcall FUN_00638A80(TechnoClass * techno)
{
	PlanningTokenClass * token = new PlanningTokenClass(techno);
	PlanningTokenClass::Array.Add(token);
	techno->FUN_00705D10(token);
	return token;
}

// FUNCTION: GAMEMD 0x00638B50
bool __fastcall FUN_00638B50(char type)
{
	return type == 9 || type == 6 || type == 7;
}

#include "cell.h"

// FUNCTION: GAMEMD 0x00638B70
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

// FUNCTION: GAMEMD 0x00638C20
bool __fastcall FUN_00638C20(int mission)
{
	return mission == 7 || mission == 8 || mission == 9;
}

// FUNCTION: GAMEMD 0x00638C40
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

// FUNCTION: GAMEMD 0x00638C70
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

// FUNCTION: GAMEMD 0x00638DC0
void __fastcall FUN_00638DC0(PlanningNodeClass * node)
{
	PlanningNodeClass::Unknown1.Add(node);
}

// FUNCTION: GAMEMD 0x00638E20
void __fastcall FUN_00638E20(void)
{
	PlanningNodeClass::Unknown2.Clear();
}

extern bool DAT_00AC4CB0;
extern int DAT_00AC4C3C;
extern bool DAT_00AC4C08;
int __fastcall FUN_0063AC70(PlanningNodeClass * node);
void __fastcall FUN_00634B20(PlanningNodeClass * node);

// FUNCTION: GAMEMD 0x00638E60
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

// FUNCTION: GAMEMD 0x00639040
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

// FUNCTION: GAMEMD 0x00639130
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

// FUNCTION: GAMEMD 0x00639210
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

// FUNCTION: GAMEMD 0x006392D0
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

// FUNCTION: GAMEMD 0x006393A0
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

// FUNCTION: GAMEMD 0x00639470
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

// FUNCTION: GAMEMD 0x006394E0
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

// FUNCTION: GAMEMD 0x006395B0
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

// FUNCTION: GAMEMD 0x00639740
void __fastcall FUN_00639740(TechnoClass * techno, PlanningNodeClass * node)
{
	Close_Loop(techno, node);
}


// FUNCTION: GAMEMD 0x00639A50
void __fastcall FUN_00639A50(PlanningNodeClass * node, PlanningNodeClass * last, EventClass event)
{
	int count = last->PlanningMembers.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * owner = last->Member_Owner(i);
		event.Data.Target.Whom = TargetClass(owner);
		node->Add_Member(owner, &event);
	}
}

// FUNCTION: GAMEMD 0x00639BC0
void __fastcall FUN_00639BC0(PlanningNodeClass * node)
{
	if (node) {
		PlanningNodeClass * next = node->Next_Node(node->Member_Owner(0));
		for (int i = node->PlanningMembers.Count() - 1; i >= 0; i--) {
			TechnoClass * owner = node->Member_Owner(i);
			PlanningTokenClass * token = owner->FUN_00705D20();
			token->FUN_00636B50(token->Node_Index(node));
		}
		if (next && next != node) {
			next->Clear();
			int count = next->PlanningMembers.Count();
			for (int i = 0; i < count; i++) {
				next->FUN_00634E10(next->PlanningMembers[i]);
			}
			next->Update_Active();
			next->Queued = false;
		}
	}
}

// FUNCTION: GAMEMD 0x00639E30
bool __fastcall FUN_00639E30(PlanningTokenClass * a, PlanningTokenClass * b)
{
	if (!a && !b) {
		return true;
	}
	int countA = 0;
	if (a) {
		countA = a->PlanningNodes.Count();
	}
	int countB = 0;
	if (b) {
		countB = b->PlanningNodes.Count();
	}
	if (countA == 0 && countB == 0) {
		return true;
	}
	if (countA == 0 || countB == 0) {
		return false;
	}
	PlanningNodeClass * nodeA = a->Node_At(countA - 1);
	PlanningNodeClass * nodeB = b->Node_At(countB - 1);
	TechnoClass * ownerA = a->OwnerUnit;
	TechnoClass * ownerB = b->OwnerUnit;
	if (nodeA->Find_Member(ownerB) == -1) {
		return false;
	}
	if (nodeB->Find_Member(ownerA) == -1) {
		return false;
	}
	return true;
}

// FUNCTION: GAMEMD 0x00639F60
void __fastcall FUN_00639F60(int slot)
{
	if (slot >= 0 && slot < 24) {
		DAT_00AC4B84[slot]++;
	}
}

// FUNCTION: GAMEMD 0x00639F80
void __fastcall FUN_00639F80(int slot)
{
	if (slot >= 0 && slot < 24) {
		int value = DAT_00AC4B84[slot] - 1;
		DAT_00AC4B84[slot] = (value < 0) ? 0 : value;
	}
}

// FUNCTION: GAMEMD 0x00639FB0
int __fastcall FUN_00639FB0(int slot)
{
	bool valid = slot >= 0 && slot < 24;
	int value = 0;
	if (valid) {
		value = DAT_00AC4B84[slot];
	}
	return value;
}

// FUNCTION: GAMEMD 0x00639FD0
void __fastcall FUN_00639FD0(EventClass * event)
{
	if (event->Type == EventClass::DEPLOY) {
		if (!DAT_00AC4C08) {
			DAT_00AC4C08 = true;
			wchar_t const * text = StringTable::LoadString("MSG:PlanningModeNoDeploy", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
			FUN_00730A90(text, 0x1e0);
			VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
		}
	} else if (event->Type == EventClass::IDLE) {
		if (!DAT_00AC4C08) {
			DAT_00AC4C08 = true;
			wchar_t const * text = StringTable::LoadString("MSG:PlanningModeNoStop", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
			FUN_00730A90(text, 0x1e0);
			VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
		}
	} else if (event->Type == EventClass::SCATTER) {
		if (!DAT_00AC4C08) {
			DAT_00AC4C08 = true;
			wchar_t const * text = StringTable::LoadString("MSG:PlanningModeNoScatter", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
			FUN_00730A90(text, 0x1e0);
			VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
		}
	}
	VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
}

// FUNCTION: GAMEMD 0x0063A0D0
void __fastcall FUN_0063A0D0(void)
{
	if (!DAT_00AC4C08) {
		DAT_00AC4C08 = true;
		wchar_t const * text = StringTable::LoadString("MSG:PlanningModeNoGuardArea", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
		FUN_00730A90(text, 0x1e0);
		VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
	}
}

// FUNCTION: GAMEMD 0x0063A120
void __fastcall FUN_0063A120(bool button, bool intro)
{
	int slot = intro ? (button == 0) : 2;
	int limit = button ? DAT_00AC4BF4[slot] + 1 : 1;
	if (DAT_00AC4BF4[slot] < limit) {
		wchar_t const * text;
		if (intro) {
			if (button) {
				text = StringTable::LoadString("MSG:PlanningModeIntro1Button", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
			} else {
				text = StringTable::LoadString("MSG:PlanningModeIntro1Key", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
			}
		} else {
			text = StringTable::LoadString("MSG:PlanningModeIntro3", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
		}
		FUN_00730A90(text, 0x1e0);
		DAT_00AC4BF4[slot]++;
	}
}

// FUNCTION: GAMEMD 0x0063A1B0
void __fastcall FUN_0063A1B0(int slot)
{
	if (DAT_00AC4BF4[slot] < 1) {
		if (slot == 3) {
			wchar_t const * text = StringTable::LoadString("MSG:PlanningModeLoopHelp", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
			FUN_00730A90(text, 0x1e0);
		} else if (slot == 4) {
			wchar_t const * text = StringTable::LoadString("MSG:PlanningModeDeleteHelp", NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
			FUN_00730A90(text, 0x1e0);
		}
		DAT_00AC4BF4[slot]++;
	}
}

// FUNCTION: GAMEMD 0x0063A210
void __fastcall FUN_0063A210(char const * name)
{
	wchar_t const * text = StringTable::LoadString(name, NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
	FUN_00730A90(text, 0x1e0);
}

// FUNCTION: GAMEMD 0x0063A230
void __fastcall FUN_0063A230(char const * name)
{
	if (!DAT_00AC4C08) {
		DAT_00AC4C08 = true;
		wchar_t const * text = StringTable::LoadString(name, NULL, "D:\\ra2mdpost\\PlanMgr.cpp", 0xca1);
		FUN_00730A90(text, 0x1e0);
		VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
	}
}

// FUNCTION: GAMEMD 0x0063A280
void __fastcall FUN_0063A280(char const * type)
{
	if (*type == 9) {
		FUN_0063A230("MSG:PlanningModeNoDeploy");
	} else if (*type == 6) {
		FUN_0063A230("MSG:PlanningModeNoStop");
	} else if (*type == 7) {
		FUN_0063A230("MSG:PlanningModeNoScatter");
	}
}

// FUNCTION: GAMEMD 0x0063A360
void __fastcall FUN_0063A360(int kind)
{
	if (kind == 0) {
		FUN_0063A230("MSG:PlanningModeNoAircraft");
	} else if (kind == 1) {
		FUN_0063A230("MSG:PlanningModeNoBuildings");
	} else if (kind == 2) {
		FUN_0063A230("MSG:PlanningModeNoUnknowns");
	}
}

// FUNCTION: GAMEMD 0x0063A4A0
void __fastcall FUN_0063A4A0(int value)
{
	DAT_00AC4C90 = value;
}

// FUNCTION: GAMEMD 0x0063A4B0
void __fastcall FUN_0063A4B0(int kind)
{
	FUN_006DA740(TacticalMap);
	if (kind == 0) {
		FUN_0063A230("MSG:PostTerminatingCommand");
	} else if (kind == 1) {
		FUN_0063A230("MSG:PostContinualCommand");
	} else if (kind == 2) {
		FUN_0063A230("MSG:NodeMaximum");
	}
}

extern int DAT_00A8ECC8;

// FUNCTION: GAMEMD 0x0063AAC0
void __stdcall FUN_0063AAC0(Point2D xy)
{
	DAT_00AC4CB0 = true;
	DAT_00AC4C68 = Point2D(-1, -1);
	if (PlanningNodeClass::PlanningModeActive && DAT_00A8ECC8 <= 0) {
		DAT_00AC4C68 = xy;
	}
}

// FUNCTION: GAMEMD 0x0063AB00
void __stdcall FUN_0063AB00(Point2D xy)
{
	DAT_00AC4CB0 = false;
	if (PlanningNodeClass::PlanningModeActive && DAT_00A8ECC8 <= 0 && DAT_00AC4BF0 != 0
		&& abs(xy.X - DAT_00AC4C68.X) <= 3 && abs(xy.Y - DAT_00AC4C68.Y) <= 3) {
		DAT_00AC4BF0 = 0;
	}
}

// FUNCTION: GAMEMD 0x0063AB60
bool __stdcall FUN_0063AB60(Point2D xy)
{
	if (!PlanningNodeClass::PlanningModeActive) {
		return false;
	}
	if (DAT_00AC4CB0) {
		return false;
	}
	PlanningNodeClass * hover = DAT_00AC4CCC;
	PlanningNodeClass * sel = DAT_00AC4C38;
	PlanningNodeClass * hit;
	if (hover != 0) {
		Point2D corner = Pixel_Of(Node_Lead_7(hover));
		corner.X -= 4;
		corner.Y -= 4;
		Rect rect;
		Box_Into_7(&rect, corner.X, corner.Y, 8, 8);
		Rect box = Boxed(rect);
		if (box.Is_Point_Within(xy)) {
			hit = DAT_00AC4CCC;
		} else {
			hit = 0;
		}
	} else {
		hit = 0;
	}
	if (sel == 0) {
		if (hit == 0) {
			return false;
		}
		Map.Set_Default_Mouse(MOUSE_NORMAL, true);
	} else if (hit != sel) {
		Map.Set_Default_Mouse(MOUSE_NORMAL, false);
	} else {
		Map.Set_Default_Mouse(MOUSE_NORMAL, true);
	}
	return true;
}

// FUNCTION: GAMEMD 0x0063AC60
PlanningNodeClass * __fastcall FUN_0063AC60(void)
{
	return DAT_00AC4BF0;
}

extern DynamicVectorClass<ObjectClass *> DAT_00A8ECB8;

// FUNCTION: GAMEMD 0x0063AC70
int __fastcall FUN_0063AC70(PlanningNodeClass * node)
{
	int count = DAT_00A8ECB8.Count();
	for (int i = 0; i < count; i++) {
		ObjectClass * object = DAT_00A8ECB8[i];
		if (object && object->Is_Techno()) {
			TechnoClass * techno = (TechnoClass *)object;
			PlanningTokenClass * token = techno->FUN_00705D20();
			if (token) {
				if (token->Loop_Start_Valid()) {
					return 1;
				}
				PlanningNodeClass * key = node;
				int index = token->PlanningNodes.ID(key);
				if (index == -1) {
					return 5;
				}
				int last = token->PlanningNodes.Count();
				if (index == last - 1) {
					return 2;
				}
				if (index == last - 2) {
					return 3;
				}
			}
		}
	}
	return -1;
}

// FUNCTION: GAMEMD 0x0063AD30
void __fastcall FUN_0063AD30(PlanningNodeClass * node)
{
	DAT_00AC4C38 = node;
}

// FUNCTION: GAMEMD 0x0063AD40
PlanningNodeClass * __fastcall FUN_0063AD40(void)
{
	return DAT_00AC4C38;
}

// FUNCTION: GAMEMD 0x0063B050
AbstractClass * __fastcall FUN_0063B050(EventClass * event)
{
	AbstractClass * result = 0;
	if (event->Data.MegaMission.Target.Is_Valid()) {
		result = event->Data.MegaMission.Target.As_Abstract();
	} else if (event->Data.MegaMission.Follow.Is_Valid()) {
		result = event->Data.MegaMission.Follow.As_Abstract();
	} else if (event->Data.MegaMission.Destination.Is_Valid()) {
		result = event->Data.MegaMission.Destination.As_Abstract();
	}
	return result;
}

// FUNCTION: GAMEMD 0x0063B080
void __fastcall FUN_0063B080(PlanningNodeClass * node)
{
	DAT_00AC4BF0 = node;
}

// FUNCTION: GAMEMD 0x0063B090
void __fastcall FUN_0063B090(void)
{
	DAT_00AC4BF0 = 0;
}

extern char * DAT_00AC4C1C;
extern int DAT_00AC4C28;
extern bool DAT_00AC4C25;
extern int DAT_00AC4C20;

void __fastcall FUN_0063B550(TechnoClass * techno);

// FUNCTION: GAMEMD 0x0063B0A0
void __fastcall FUN_0063B0A0(void)
{
	DAT_00AC4C28 = 0;
	if (DAT_00AC4C1C && DAT_00AC4C25) {
		delete DAT_00AC4C1C;
		DAT_00AC4C1C = 0;
	}
	DAT_00AC4C25 = false;
	DAT_00AC4C20 = 0;
	int nodes = PlanningNodeClass::Unknown1.Count();
	for (int i = 0; i < nodes; i++) {
		PlanningNodeClass * node = PlanningNodeClass::Unknown1[i];
		HouseClass * house = node->PlanningMembers[0]->Owner->House;
		if (house->Is_Ally(PlayerPtr)) {
			node->FUN_0063C690();
		}
	}
	int count = Technos.Count();
	for (int j = 0; j < count; j++) {
		TechnoClass * techno = Technos[j];
		HouseClass * house = techno->House;
		if (house->Is_Ally(PlayerPtr)) {
			FUN_0063B550(techno);
		}
	}
}

void __fastcall FUN_0063BE60(TechnoClass * techno);

// FUNCTION: GAMEMD 0x0063B150
void __fastcall FUN_0063B150(void)
{
	int nodes = PlanningNodeClass::Unknown1.Count();
	for (int i = 0; i < nodes; i++) {
		PlanningNodeClass * node = PlanningNodeClass::Unknown1[i];
		HouseClass * house = node->PlanningMembers[0]->Owner->House;
		if (house->Is_Ally(PlayerPtr)) {
			node->FUN_0063CAE0();
		}
	}
	for (int j = 0; j < nodes; j++) {
		PlanningNodeClass * node = PlanningNodeClass::Unknown1[j];
		HouseClass * house = node->PlanningMembers[0]->Owner->House;
		if (house->Is_Ally(PlayerPtr)) {
			int color = node->Node_Color();
			Rect clip = FUN_0072AD20();
			Rect rect = Around(8, Pixel_Of(Node_Lead_7(node)));
			Shrink_Rect(rect);
			DSurface::Composite->Fill_Rect(clip, rect, color);
		}
	}
	int count = Technos.Count();
	for (int k = 0; k < count; k++) {
		TechnoClass * techno = Technos[k];
		HouseClass * house = techno->House;
		if (house->Is_Ally(PlayerPtr)) {
			FUN_0063BE60(techno);
		}
	}
}

static inline int Pixel_Color(RGBClass const & c)
{
	return (c.Get_Red() >> Drawing::RedShiftRight << Drawing::RedShiftLeft)
		| (c.Get_Green() >> Drawing::GreenShiftRight << Drawing::GreenShiftLeft)
		| (c.Get_Blue() >> Drawing::BlueShiftRight << Drawing::BlueShiftLeft);
}

static inline Point2D Node_Pixel_0(PlanningNodeClass * n) { return n->Lead_Pixel(); }
static inline Point2D Node_Pixel_1(PlanningNodeClass * n) { return Node_Pixel_0(n); }
static inline Point2D Node_Pixel_2(PlanningNodeClass * n) { return Node_Pixel_1(n); }
static inline Point2D Node_Pixel_3(PlanningNodeClass * n) { return Node_Pixel_2(n); }
static inline Point2D Node_Pixel_4(PlanningNodeClass * n) { return Node_Pixel_3(n); }
static inline Point2D Node_Pixel_5(PlanningNodeClass * n) { return Node_Pixel_4(n); }
static inline Point2D Node_Pixel_6(PlanningNodeClass * n) { return Node_Pixel_5(n); }
static inline Point2D Node_Pixel_7(PlanningNodeClass * n) { return Node_Pixel_6(n); }
static inline Rect Node_Rect_0(PlanningNodeClass * n) { return n->Lead_Rect(); }
static inline Rect Node_Rect_1(PlanningNodeClass * n) { return Node_Rect_0(n); }
static inline Rect Node_Rect_2(PlanningNodeClass * n) { return Node_Rect_1(n); }
static inline Rect Node_Rect_3(PlanningNodeClass * n) { return Node_Rect_2(n); }
static inline Rect Node_Rect_4(PlanningNodeClass * n) { return Node_Rect_3(n); }
static inline Rect Node_Rect_5(PlanningNodeClass * n) { return Node_Rect_4(n); }
static inline Rect Node_Rect_6(PlanningNodeClass * n) { return Node_Rect_5(n); }
static inline Rect Node_Rect_7(PlanningNodeClass * n) { return Node_Rect_6(n); }
static inline void Widen_0(Rect * r) { FUN_006357D0(r, 4); }
static inline void Widen_1(Rect * r) { Widen_0(r); }
static inline void Widen_2(Rect * r) { Widen_1(r); }
static inline void Widen_3(Rect * r) { Widen_2(r); }
static inline void Widen_4(Rect * r) { Widen_3(r); }
static inline void Widen_5(Rect * r) { Widen_4(r); }
static inline void Widen_6(Rect * r) { Widen_5(r); }
static inline Rect Grown(Rect r) { Widen_6(&r); return r; }

// FUNCTION: GAMEMD 0x0063B2F0
void __fastcall FUN_0063B2F0(void)
{
	int color = Pixel_Color(DAT_00A80220);
	PlanningNodeClass * hover = DAT_00AC4CCC;
	if (hover) {
		Rect clip = FUN_0072AD20();
		Rect rect = Inflated(Around(8, Node_Pixel_7(hover)), 4);
		for (int i = 0; i < 3; i++) {
			DSurface::Composite->Draw_Rect(clip, rect, color);
			Shrink_Rect(rect);
		}
	}
	PlanningNodeClass * sel = DAT_00AC4BF0;
	if (sel) {
		Rect clip = FUN_0072AD20();
		Rect rect = Grown(Grown(Node_Rect_7(sel)));
		for (int i = 0; i < 3; i++) {
			DSurface::Composite->Draw_Rect(clip, rect, color);
			Shrink_Rect(rect);
		}
	}
}

// FUNCTION: GAMEMD 0x0063BE10
bool __fastcall FUN_0063BE10(TechnoClass * techno)
{
	if (techno->Get_Mission() == MISSION_GUARD_AREA) {
		if (techno->Is_Foot()) {
			FootClass * foot = (FootClass *)techno;
			if (foot->NavCom != 0 || foot->TarCom != 0) {
				return false;
			}
		}
		return true;
	}
	return false;
}
// FUNCTION: GAMEMD 0x0063CA80
Coord __fastcall FUN_0063CA80(EventClass const & event, TechnoClass * owner)
{
	Coord coord = FUN_00633BC0(event);
	if (coord == Coord(-1, -1, -1) && owner) {
		coord = owner->As_Coord();
	}
	return coord;
}

// FUNCTION: GAMEMD 0x0063CE40
void __fastcall FUN_0063CE40(PlanningNodeClass * node)
{
	int color = node->Node_Color();
	Rect clip = FUN_0072AD20();
	Rect rect = FUN_00634510(8, node->Lead_Pixel());
	Shrink_Rect(rect);
	DSurface::Composite->Fill_Rect(clip, rect, color);
}

// FUNCTION: GAMEMD 0x0063CF10
int __fastcall FUN_0063CF10(TechnoClass * techno)
{
	int slot = FUN_006339B0(techno);
	RGBClass mixed = Houses[slot]->RemapColorRGB;
	mixed.Adjust(0x50, DAT_00A83CD9);
	return (mixed.Get_Green() >> Drawing::GreenShiftRight << Drawing::GreenShiftLeft)
		| (mixed.Get_Red() >> Drawing::RedShiftRight << Drawing::RedShiftLeft)
		| (mixed.Get_Blue() >> Drawing::BlueShiftRight << Drawing::BlueShiftLeft);
}

extern int DAT_008A0DD4;
extern int DAT_008A0DE4;
extern int DAT_008A0DDC;

static inline int Pack_Wide(RGBClass const & c)
{
	unsigned char rr = DAT_008A0DD4;
	unsigned char gr = DAT_008A0DE4;
	unsigned char br = DAT_008A0DDC;
	return (c.Get_Red() >> rr << Drawing::RedShiftLeft)
		| (c.Get_Green() >> gr << Drawing::GreenShiftLeft)
		| (c.Get_Blue() >> br << Drawing::BlueShiftLeft);
}


extern int FUN_00731CE0(void);
extern int FUN_00731CD0(void);


extern "C" __declspec(dllimport) unsigned long __stdcall timeGetTime(void);
extern bool DAT_00836880[];
extern bool __fastcall Line_In_Bounds(Point2D & start, Point2D & end, Rect const & bounds);

// FUNCTION: GAMEMD 0x0063CFC0
int __fastcall FUN_0063CFC0(TechnoClass * techno)
{
	int slot = FUN_006339B0(techno);
	RGBClass mixed = Houses[slot]->RemapColorRGB;
	mixed.Adjust(0x64, DAT_00A80220);
	return (mixed.Get_Green() >> Drawing::GreenShiftRight << Drawing::GreenShiftLeft)
		| (mixed.Get_Red() >> Drawing::RedShiftRight << Drawing::RedShiftLeft)
		| (mixed.Get_Blue() >> Drawing::BlueShiftRight << Drawing::BlueShiftLeft);
}

// FUNCTION: GAMEMD 0x0063D0F0
int __fastcall FUN_0063D0F0(TechnoClass * techno)
{
	int result = Pack_Wide(DAT_00A83CD9);
	if (techno) {
		int highlight = Pack_Wide(DAT_00A80220);
		if (DAT_00AC4BF0) {
			if (DAT_00AC4BF0->Has_Member(techno)) {
				result = highlight;
			}
		} else if (techno->IsSelected) {
			result = highlight;
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x0063D210
Point2D __fastcall FUN_0063D210(EventClass const * event, TechnoClass * owner)
{
	Point2D pixel(-1, -1);
	if (event) {
		EventClass copy = *event;
		Coord coord = FUN_0063CA80(copy, owner);
		pixel = FUN_006339E0(coord);
	}
	return pixel;
}

// FUNCTION: GAMEMD 0x0063D2D0
int PlanningNodeClass::Lead_Color(void)
{
	TechnoClass * techno = PlanningMembers[0]->Owner;
	int result = Pack_Wide(DAT_00A83CD9);
	if (techno) {
		int highlight = Pack_Wide(DAT_00A80220);
		if (DAT_00AC4BF0) {
			if (DAT_00AC4BF0->Has_Member(techno)) {
				result = highlight;
			}
		} else if (techno->IsSelected) {
			result = highlight;
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x0063D400
void __fastcall FUN_0063D400(int color, int unused, int x, int y)
{
	Rect clip = FUN_0072AD20();
	Rect rect = FUN_00634510(6, Point2D(x, y));
	DSurface::Composite->Draw_Rect(clip, rect, color);
}

// FUNCTION: GAMEMD 0x0063D480
void __fastcall FUN_0063D480(int color, int unused, int x, int y)
{
	Rect clip = FUN_0072AD20();
	Rect rect = FUN_00634510(6, Point2D(x, y));
	Shrink_Rect(rect);
	DSurface::Composite->Fill_Rect(clip, rect, color);
}

// FUNCTION: GAMEMD 0x0063D500
int __fastcall FUN_0063D500(TechnoClass * techno)
{
	if (techno->Get_Mission() == MISSION_GUARD_AREA) {
		return FUN_00731CE0();
	}
	return -1;
}

// FUNCTION: GAMEMD 0x0063D520
int __fastcall FUN_0063D520(TechnoClass * techno)
{
	if (techno->GetMegaMission() == 0x1d) {
		return FUN_00731CD0();
	}
	return -1;
}

// FUNCTION: GAMEMD 0x0063D540
int __fastcall FUN_0063D540(EventClass const & event)
{
	int mission = event.Data.MegaMission.Mission;
	if (mission == MISSION_GUARD_AREA) {
		return FUN_00731CE0();
	}
	if (mission == 0x1d) {
		return FUN_00731CD0();
	}
	return -1;
}

// FUNCTION: GAMEMD 0x0063D560
void __fastcall FUN_0063D560(int color, int unused, Point2D a, Point2D b)
{
	Point2D delta(a.X - b.X, a.Y - b.Y);
	double fx = delta.X;
	double fy = delta.Y;
	if ((int)FUN_004CAC40(fy * fy + fx * fx) >= 0x25) {
		Rect clip = FUN_0072AD20();
		Rect rect((a.X + b.X) / 2 - 10, (a.Y + b.Y) / 2 - 10, 0x15, 0x15);
		DSurface::Composite->Draw_Rect(clip, rect, color);
	}
}

// FUNCTION: GAMEMD 0x0063D810
void __fastcall FUN_0063D810(Point2D p1, int spread, int color, Point2D p2, bool dashed)
{
	bool horizontal = abs(p2.X - p1.X) > abs(p2.Y - p1.Y);
	int shiftX = horizontal ? 0 : spread;
	int shiftY = horizontal ? spread : 0;
	p1.X += shiftX;
	p1.Y += shiftY;
	p2.X += shiftX;
	p2.Y += shiftY;
	Rect bounds = DSurface::ViewBounds;
	if (Line_In_Bounds(p1, p2, bounds)) {
		if (dashed) {
			int phase = -(int)timeGetTime() >> 5;
			DSurface::Composite->Draw_Dashed_Line(p1, p2, color, DAT_00836880, phase & 0xf);
		} else {
			DSurface::Composite->Draw_Line(p1, p2, color);
		}
	}
}

// FUNCTION: GAMEMD 0x0063D910
void __fastcall FUN_0063D910(int width, int color, Point2D p1, Point2D p2)
{
	int high = width / 2;
	for (int i = high - width + 1; i <= high; i++) {
		FUN_0063D810(p1, i, color, p2, false);
	}
}

// FUNCTION: GAMEMD 0x0063D960
void __fastcall FUN_0063D960(int width, int color, Point2D p1, Point2D p2)
{
	int span = width - 2;
	int high;
	int low;
	if (span <= 0) {
		low = 0;
		high = 0;
	} else {
		high = span / 2;
		low = high - span + 1;
	}
	for (int i = low; i <= high; i++) {
		FUN_0063D810(p1, i, color, p2, true);
	}
}

struct PlanRGB {
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
};

// FUNCTION: GAMEMD 0x0063DAD0
int __fastcall FUN_0063DAD0(PlanRGB const & c)
{
	return (c.Red >> Drawing::RedShiftRight << Drawing::RedShiftLeft)
		| (c.Green >> Drawing::GreenShiftRight << Drawing::GreenShiftLeft)
		| (c.Blue >> Drawing::BlueShiftRight << Drawing::BlueShiftLeft);
}

// FUNCTION: GAMEMD 0x0063DB20
// ?Is_Point_Within@?$TRect@H@@QBE_NABV?$TPoint2D@H@@@Z
template class TRect<int>;

// TEMPLATE: GAMEMD 0x0063DC80
// ?Clear@?$DynamicVectorClass@PAVEventClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0063DCB0
// ?ID@?$DynamicVectorClass@PAVEventClass@@@@UAEHPBQAVEventClass@@@Z
// TEMPLATE: GAMEMD 0x0063DCD0
// ?GetItem@?$VectorClass@PAVEventClass@@@@UBEPAVEventClass@@H@Z
template class DynamicVectorClass<EventClass *>;

// TEMPLATE: GAMEMD 0x0063DCE0
// ?Clear@?$DynamicVectorClass@PAVPlanningTokenClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0063DD10
// ?ID@?$DynamicVectorClass@PAVPlanningTokenClass@@@@UAEHPBQAVPlanningTokenClass@@@Z
// TEMPLATE: GAMEMD 0x0063DD30
// ?GetItem@?$VectorClass@PAVPlanningTokenClass@@@@UBEPAVPlanningTokenClass@@H@Z
template class DynamicVectorClass<PlanningTokenClass *>;

// TEMPLATE: GAMEMD 0x0063DD40
// ??8?$VectorClass@PAVPlanningMemberClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0063DD80
// ?Resize@?$VectorClass@PAVPlanningMemberClass@@@@UAE_NHPBQAVPlanningMemberClass@@@Z
// TEMPLATE: GAMEMD 0x0063DE60
// ?ID@?$VectorClass@PAVPlanningMemberClass@@@@UAEHABQAVPlanningMemberClass@@@Z
// TEMPLATE: GAMEMD 0x0063DEA0
// ?ID@?$VectorClass@PAVPlanningMemberClass@@@@UAEHPBQAVPlanningMemberClass@@@Z
template class VectorClass<PlanningMemberClass *>;

// TEMPLATE: GAMEMD 0x0063DEC0
// ??0?$DynamicVectorClass@PAVPlanningMemberClass@@@@QAE@IPBQAVPlanningMemberClass@@@Z
// TEMPLATE: GAMEMD 0x0063DF20
// ?Resize@?$DynamicVectorClass@PAVPlanningMemberClass@@@@UAE_NHPBQAVPlanningMemberClass@@@Z
template class DynamicVectorClass<PlanningMemberClass *>;
