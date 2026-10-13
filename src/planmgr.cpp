// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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

class EventClass;
class PlanningNodeClass;
class TechnoClass;

extern double __cdecl FUN_004CAC40(double);
extern Rect __fastcall FUN_0072AD20(void);
Coord __fastcall FUN_00633BC0(EventClass const & event);
class xTargetClass;
class AbstractClass;
extern AbstractClass * __fastcall FUN_006E6E20(xTargetClass * target);
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

// FUNCTION: 0x006335E0 $E106
// FUNCTION: 0x00633620 $E103
// FUNCTION: 0x00633680 $E112
// FUNCTION: 0x006336C0 $E109
// FUNCTION: 0x00633720 $E118
// FUNCTION: 0x00633760 $E115
// FUNCTION: 0x006337C0 $E124
// FUNCTION: 0x00633800 $E121
// FUNCTION: 0x00633860 $E129
// FUNCTION: 0x006338A0 $E127
// FUNCTION: 0x00633900 $E134
// FUNCTION: 0x00633940 $E132

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
		return PlayerPtr->HeapID;
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

class PlanPoint
{
	public:
		int A;
		int B;
		int C;
		char D;

		PlanPoint(void);
		void Set_A(int v);
		void Set_B(int v);
		void Set_C(int v);
		void Set_D(char v);
		int Get_A(void);
		int Get_B(void);
		int Get_C(void);
		char Get_D(void);
};

// FUNCTION: 0x00633A20
PlanPoint::PlanPoint(void) : A(0), B(0), C(-1), D(0)
{
}

// FUNCTION: 0x00633A40
void PlanPoint::Set_A(int v)
{
	A = v;
}

// FUNCTION: 0x00633A50
void PlanPoint::Set_B(int v)
{
	B = v;
}

// FUNCTION: 0x00633A60
void PlanPoint::Set_C(int v)
{
	C = v;
}

// FUNCTION: 0x00633A70
void PlanPoint::Set_D(char v)
{
	D = v;
}

// FUNCTION: 0x00633A80
int PlanPoint::Get_A(void)
{
	return A;
}

// FUNCTION: 0x00633A90
int PlanPoint::Get_B(void)
{
	return B;
}

// FUNCTION: 0x00633AA0
int PlanPoint::Get_C(void)
{
	return C;
}

// FUNCTION: 0x00633AB0
char PlanPoint::Get_D(void)
{
	return D;
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

// FUNCTION: 0x0063E540 ?$DynamicVectorClass@PAVPlanningNodeClass@@::Add
template class DynamicVectorClass<PlanningNodeClass *>;

// FUNCTION: 0x0063EE30 ?$VectorClass@PAVEventClass@@::~?$VectorClass@PAVEventClass@@
template class VectorClass<EventClass *>;

class PlanMember
{
	public:
		TechnoClass * Owner;
		EventClass * Packet;
		int Branch;
		bool Active;

		PlanMember(void) : Owner(0), Packet(0), Branch(-1), Active(false) {}
};

class PlanBranch
{
	public:
		char Data[0x70];
		int Pending;
		int Slot;
		int Tail;
};

class PlanToken;

class PlanningNodeClass
{
	public:
		static DynamicVectorClass<PlanningNodeClass *> Unknown1;
		static DynamicVectorClass<PlanningNodeClass *> Unknown2;
		static DynamicVectorClass<PlanningNodeClass *> Unknown3;

		DynamicVectorClass<PlanMember *> Members;
		int Id;
		bool Queued;
		DynamicVectorClass<PlanBranch *> Branches;
		bool Flag;
		char Gap[0x6f];
		int ActiveCount;
		int Slot1;
		int Slot2;
		int Slot3;

		PlanningNodeClass(int id);
		int Get_Id(void);
		int Get_Count(void);
		int Index_In(TechnoClass * techno);
		PlanningNodeClass * Prev_Node(TechnoClass * techno);
		PlanningNodeClass * Next_Node(TechnoClass * techno);
		void Add_Member(TechnoClass * techno, EventClass const * event);
		int Find_Member(TechnoClass * techno);
		TechnoClass * Member_Owner(int index);
		void FUN_00635120(void);
		void FUN_00635DB0(int);
		void FUN_00634E10(PlanMember *);
};

class PlanToken
{
	public:
		TechnoClass * Owner;
		DynamicVectorClass<PlanningNodeClass *> Nodes;
		bool F1C;
		bool F1D;
		char Gap[0x6c];
		int F8C;
		int LoopCount;
		int LoopStart;

		PlanningNodeClass * Node_At(int index)
		{
			int count = Nodes.Count();
			if (count == 0) return 0;
			if (index >= count) {
				int start = LoopStart;
				if (start <= 0 && start != 0) return 0;
				index = (index - start) % LoopCount + start;
			}
			if (index < 0 || index >= count) return 0;
			return Nodes[index];
		}
};

extern PlanningNodeClass * DAT_00AC4CCC;
extern PlanningNodeClass * DAT_00AC4C38;
extern PlanningNodeClass * DAT_00AC4BF0;
extern PlanToken * __fastcall FUN_00705D20(TechnoClass * techno);

class PlanTokens { public: static DynamicVectorClass<PlanningTokenClass *> Array; };
class PlanEvents { public: static DynamicVectorClass<EventClass *> Events; };
class PlanTechnos { public: static DynamicVectorClass<TechnoClass *> Technos; };

DynamicVectorClass<EventClass *> PlanEvents::Events;
DynamicVectorClass<PlanningNodeClass *> PlanningNodeClass::Unknown3;
DynamicVectorClass<TechnoClass *> PlanTechnos::Technos;
DynamicVectorClass<PlanningNodeClass *> PlanningNodeClass::Unknown1;
DynamicVectorClass<PlanningNodeClass *> PlanningNodeClass::Unknown2;
DynamicVectorClass<PlanningTokenClass *> PlanTokens::Array;

#include "event.h"
#include "target.h"

class PlanEvent
{
	public:
		EventClass Event;
		int Index;
		int Link;

		PlanEvent(void);
		void Set_Event(EventClass const & event);
		void Set_Index(int index);
		void Set_Link(int link);
		void Clear_Index(void);
		int Next_Index(void);
		int Prev_Index(void);
		EventClass * Get_Event(EventClass * event);
		int Get_Index(void);
		int Get_Link(void);
		Coord Get_Coord(void);
		Point2D Get_Pixel(void);
};

// FUNCTION: 0x00633AC0
PlanEvent::PlanEvent(void) : Index(0), Link(-1)
{
}

// FUNCTION: 0x00633AE0
void PlanEvent::Set_Event(EventClass const & event)
{
	Event = event;
}

// FUNCTION: 0x00633B00
void PlanEvent::Set_Index(int index)
{
	Index = index;
}

// FUNCTION: 0x00633B10
void PlanEvent::Set_Link(int link)
{
	Link = link;
}

// FUNCTION: 0x00633B20
void PlanEvent::Clear_Index(void)
{
	Index = 0;
}

// FUNCTION: 0x00633B30
int PlanEvent::Next_Index(void)
{
	return ++Index;
}

// FUNCTION: 0x00633B40
int PlanEvent::Prev_Index(void)
{
	return --Index;
}

// FUNCTION: 0x00633B50
EventClass * PlanEvent::Get_Event(EventClass * event)
{
	*event = Event;
	return event;
}

// FUNCTION: 0x00633B70
int PlanEvent::Get_Index(void)
{
	return Index;
}

// FUNCTION: 0x00633B80
int PlanEvent::Get_Link(void)
{
	return Link;
}

// FUNCTION: 0x00633B90
Coord PlanEvent::Get_Coord(void)
{
	return Coord(FUN_00633BC0(Event));
}

// FUNCTION: 0x00633BC0
Coord __fastcall FUN_00633BC0(EventClass const & event)
{
	if (event.Type == EventClass::MEGAMISSION) {
		AbstractClass * object;
		if (event.Data.MegaMission.Target.Is_Valid()) {
			object = FUN_006E6E20((xTargetClass *)&event.Data.MegaMission.Target);
			if (object) {
				return object->As_Coord();
			}
		} else if (event.Data.MegaMission.Follow.Is_Valid()) {
			object = FUN_006E6E20((xTargetClass *)&event.Data.MegaMission.Follow);
			if (object) {
				return object->As_Coord();
			}
		} else if (event.Data.MegaMission.Destination.Is_Valid()) {
			object = FUN_006E6E20((xTargetClass *)&event.Data.MegaMission.Destination);
			if (object) {
				return object->As_Coord();
			}
		}
	}
	return Coord(-1, -1, -1);
}

// FUNCTION: 0x00633C60
Point2D PlanEvent::Get_Pixel(void)
{
	Coord coord = FUN_00633BC0(Event);
	return FUN_006339E0(coord);
}

// FUNCTION: 0x00633CC0
PlanningNodeClass::PlanningNodeClass(int id) : Id(id), Queued(false), Flag(false), ActiveCount(0), Slot1(-1), Slot2(-1), Slot3(-1)
{
}

// FUNCTION: 0x00633E50
int PlanningNodeClass::Get_Id(void)
{
	return Id;
}

// FUNCTION: 0x00633E60
int PlanningNodeClass::Get_Count(void)
{
	return Members.Count();
}

// FUNCTION: 0x00633E70
int PlanningNodeClass::Index_In(TechnoClass * techno)
{
	PlanToken * token = FUN_00705D20(techno);
	return token->Nodes.ID(this);
}

// FUNCTION: 0x00633EA0
PlanningNodeClass * PlanningNodeClass::Prev_Node(TechnoClass * techno)
{
	PlanToken * token = FUN_00705D20(techno);
	int index = token->Nodes.ID(this);
	if (index != -1) {
		int count = token->Nodes.Count();
		if (count == 1) return this;
		index--;
		if (index < 0) index = count - 1;
		return token->Node_At(index);
	}
	return 0;
}

// FUNCTION: 0x00633F20
PlanningNodeClass * PlanningNodeClass::Next_Node(TechnoClass * techno)
{
	PlanToken * token = FUN_00705D20(techno);
	int index = token->Nodes.ID(this);
	if (index != -1) {
		int count = token->Nodes.Count();
		if (count == 1) return this;
		index++;
		if (index >= count) index = 0;
		return token->Node_At(index);
	}
	return 0;
}

// FUNCTION: 0x00633FA0
void PlanningNodeClass::Add_Member(TechnoClass * techno, EventClass const * event)
{
	PlanMember * member = new PlanMember;
	member->Owner = techno;
	EventClass * packet = new EventClass;
	*packet = *event;
	member->Packet = packet;
	Members.Add(member);
	PlanToken * token = FUN_00705D20(techno);
	token->Nodes.Add(this);
	if (token->F8C == -1) {
		token->F8C = token->Nodes.Count() - 1;
	}
	FUN_00634E10(member);
}

// FUNCTION: 0x00634290
int PlanningNodeClass::Find_Member(TechnoClass * techno)
{
	int result = -1;
	int count = Members.Count();
	for (int i = 0; i < count; i++) {
		if (Members[i]->Owner == techno) {
			result = i;
			break;
		}
	}
	return result;
}

// FUNCTION: 0x006342D0
TechnoClass * PlanningNodeClass::Member_Owner(int index)
{
	return Members[index]->Owner;
}
