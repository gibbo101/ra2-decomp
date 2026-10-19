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

class PlanMember;
class PlanBranch;
struct TechnoVecOps { bool Add(TechnoClass * const &); void Delete_Index(int); };
struct TechnoSlotOps { int GetControllingHouse(void); };
struct TokenVecOps { void Delete_Index(int); };
struct MemberVecOps { bool Delete(PlanMember * const &); void Delete_Index(int); };

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

		PlanBranch(void) { Data[0] = 0; Pending = 0; Slot = -1; }
};

class PlanOwn : public PlanBranch
{
	public:
		int Slot2;
		int Slot3;

		PlanOwn(void) : Slot2(-1), Slot3(-1) {}
};

class PlanToken;
void __cdecl FUN_004068E0(char const * format, ...);

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
		PlanOwn Own;

		PlanningNodeClass(int id);
		~PlanningNodeClass() { Clear(); FUN_00637640(); }
		int Get_Id(void);
		int Get_Count(void);
		int Index_In(TechnoClass * techno);
		PlanningNodeClass * Prev_Node(TechnoClass * techno);
		PlanningNodeClass * Next_Node(TechnoClass * techno);
		void Add_Member(TechnoClass * techno, EventClass const * event);
		int Find_Member(TechnoClass * techno);
		int FUN_00634290(TechnoClass * techno);
		TechnoClass * Member_Owner(int index);
		EventClass Get_Event(int index);
		EventClass Get_Event_For(TechnoClass * techno);
		bool Has_Member(TechnoClass * techno);
		HouseClass * Lead_House(void);
		Coord Lead_Coord(void);
		Point2D Lead_Pixel(void);
		bool Hit_Test(int x, int y);
		bool Lead_At(Coord coord);
		Rect Lead_Rect(void);
		Rect Lead_Box_Rect(void);
		Rect Lead_Box_Rect2(void);
		bool Is_Past_Start(void);
		void Promote_Events(void);
		PlanMember * Get_Member(TechnoClass * techno);
		TechnoClass * Owner_At_Branch(int branch);
		TechnoClass * Active_Owner(void);
		PlanBranch * Get_Branch(int index);
		PlanBranch * Branch_Of(TechnoClass * techno);
		EventClass * Own_Event(void);
		Coord Branch_Coord(int index);
		Coord Own_Coord(void);
		Point2D Branch_Pixel(int index);
		int Branch_Count(void);
		int Has_Pending(void);
		int Is_Pending(void);
		Point2D Own_Pixel(void);
		void Set_Queued(void);
		bool Get_Queued(void);
		void Rebuild(void);
		void Update_Active(void);
		int Find_Branch(TechnoClass * techno);
		void Detach(PlanMember *);
		bool Is_Loop_Start(TechnoClass * techno);
		void Deactivate(PlanMember *);
		void Clear(void);
		void Reset_Colors(bool);
		void Select_All(void);
		void Unselect_All(void);
		void Draw_Lead_Box(int);
		void Fill_Lead_Box(int);
		void Frame_Lead_Box(int);
		void Frame_Lead_Pixel(int);
		int Node_Color(void);
		int Branch_Color(int);
		int Own_Color(void);
		void FUN_00634E10(PlanMember *);
		void FUN_0063D9C0(int);
		void FUN_00635060(PlanMember *);
		void FUN_006351E0(PlanMember *);
		void FUN_00635DB0(bool);
		void FUN_006378B0(void);
		void FUN_00637640(void);
		void FUN_006340B0(TechnoClass *);
		void Locate_Own_Branch(void);
		PlanMember * FUN_006346B0(TechnoClass * techno);
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
		bool F98;
		bool F99;

		PlanToken(TechnoClass * owner);
		TechnoClass * Get_Owner(void);
		void Cancel_Owner(void);
		~PlanToken();
		void Destroy(void);
		void Set_F1C(void);
		void Clear_F1C(void);
		bool Get_F1C(void);
		bool Owner_Is_Mission(void);
		bool Has_Started(void);
		PlanningNodeClass * FUN_00636E60(int index);
		PlanningNodeClass * Wrapped_Node_At(int index);
		PlanningNodeClass * Last_Node(void);
		bool Has_Node(PlanningNodeClass * node);
		int Node_Index(PlanningNodeClass * node);
		EventClass Event_At(int index);
		EventClass Last_Popped(void);
		PlanningNodeClass * Loop_Node(void);
		void Set_Loop(int count, int start);
		int Get_Loop_Count(void);
		int Get_Loop_Start(void);
		bool Loop_Start_Valid(void);
		bool Loop_Start_Positive(void);
		bool Loop_Start_Zero(void);
		EventClass Pop_Front(void);
		void Share_Range(TechnoClass * techno, int start, int count);
		void Share_From(TechnoClass * techno, int start);
		void Remove_Node(int index);
		void Reset_Marks(void);
		int Get_F8C(void);
		bool Reached(int index);
		int Node_Count(void);
		int Current_Or_Count(void);
		int Remaining(void);
		void Add_Node(PlanningNodeClass * node);
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

template class DynamicVectorClass<PlanMember *>;
template class DynamicVectorClass<PlanBranch *>;
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
PlanningNodeClass::PlanningNodeClass(int id) : Id(id), Queued(false)
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

Rect __fastcall FUN_00634510(int size, Point2D center);

static inline Point2D Coord_To_View(Coord coord)
{
	Point2D pixel;
	TacticalMap->Coord_To_Pixel(coord, pixel);
	pixel.X += DSurface::ViewBounds.X;
	pixel.Y += DSurface::ViewBounds.Y;
	return pixel;
}

// FUNCTION: 0x006342E0
EventClass PlanningNodeClass::Get_Event(int index)
{
	return *Members[index]->Packet;
}

// FUNCTION: 0x00634310
EventClass PlanningNodeClass::Get_Event_For(TechnoClass * techno)
{
	int index = Find_Member(techno);
	PlanMember * member = (index == -1) ? 0 : Members[index];
	return *member->Packet;
}

// FUNCTION: 0x00634360
bool PlanningNodeClass::Has_Member(TechnoClass * techno)
{
	return Find_Member(techno) != -1;
}

// FUNCTION: 0x006343B0
HouseClass * PlanningNodeClass::Lead_House(void)
{
	return Members[0]->Owner->House;
}

// FUNCTION: 0x006343C0
Coord PlanningNodeClass::Lead_Coord(void)
{
	Coord coord = FUN_00633BC0(*Members[0]->Packet);
	return coord;
}

// FUNCTION: 0x00634400
Point2D PlanningNodeClass::Lead_Pixel(void)
{
	Coord coord = FUN_00633BC0(*Members[0]->Packet);
	return Coord_To_View(coord);
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
	Rect r = Around(8, Pixel_Of(Event_Coord(*Members[0]->Packet)));
	Rect big = r;
	big.Width += 8;
	big.Height += 8;
	big.X -= 4;
	big.Y -= 4;
	return big.Is_Point_Within(Point2D(x, y));
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
	Coord c = FUN_00633BC0(*Members[0]->Packet);
	return c.X == coord.X && c.Y == coord.Y && c.Z == coord.Z;
}

// FUNCTION: 0x006345A0
Rect PlanningNodeClass::Lead_Rect(void)
{
	Coord coord = FUN_00633BC0(*Members[0]->Packet);
	Point2D pixel;
	TacticalMap->Coord_To_Pixel(coord, pixel);
	pixel.X += DSurface::ViewBounds.X;
	pixel.Y += DSurface::ViewBounds.Y;
	return FUN_00634510(8, pixel);
}

// FUNCTION: 0x00634630
bool PlanningNodeClass::Is_Past_Start(void)
{
	if (Members.Count() == 0) return false;
	PlanToken * token = FUN_00705D20(Members[0]->Owner);
	int index = token->Nodes.ID(this);
	int start = token->LoopStart;
	return start != -1 && index >= start;
}

// FUNCTION: 0x00634680
void PlanningNodeClass::Promote_Events(void)
{
	int count = Members.Count();
	for (int i = 0; i < count; i++) {
		EventClass * packet = Members[i]->Packet;
		if (packet->Data.MegaMission.Mission == 2) packet->Data.MegaMission.Mission = 0x1d;
	}
}

// FUNCTION: 0x006346B0
PlanMember * PlanningNodeClass::Get_Member(TechnoClass * techno)
{
	int index = Find_Member(techno);
	return (index == -1) ? 0 : Members[index];
}

// FUNCTION: 0x00634700
int PlanningNodeClass::Branch_Count(void)
{
	return Branches.Count();
}

// FUNCTION: 0x00634710
int PlanningNodeClass::Has_Pending(void)
{
	return Own.Pending > 0;
}

// FUNCTION: 0x00634720
int PlanningNodeClass::Is_Pending(void)
{
	if (Own.Pending > 0 && Own.Slot2 == -1) return 1;
	return 0;
}

// FUNCTION: 0x00634740
TechnoClass * PlanningNodeClass::Owner_At_Branch(int branch)
{
	int count = Members.Count();
	for (int i = 0; i < count; i++) {
		PlanMember * m = Members[i];
		if (m->Branch == branch) return m->Owner;
	}
	return 0;
}

// FUNCTION: 0x00634770
TechnoClass * PlanningNodeClass::Active_Owner(void)
{
	int count = Members.Count();
	for (int i = 0; i < count; i++) {
		PlanMember * m = Members[i];
		if (m->Active) return m->Owner;
	}
	return 0;
}

// FUNCTION: 0x006347A0
PlanBranch * PlanningNodeClass::Get_Branch(int index)
{
	return Branches[index];
}

// FUNCTION: 0x006347B0
PlanBranch * PlanningNodeClass::Branch_Of(TechnoClass * techno)
{
	int index = Find_Member(techno);
	PlanMember * member = (index == -1) ? 0 : Members[index];
	int branch = member->Branch;
	PlanBranch * result = 0;
	if (branch != -1) result = Branches[branch];
	return result;
}

// FUNCTION: 0x00634800
EventClass * PlanningNodeClass::Own_Event(void)
{
	return (EventClass *)&Own;
}

// FUNCTION: 0x00634810
Coord PlanningNodeClass::Branch_Coord(int index)
{
	Coord coord = FUN_00633BC0(*(EventClass *)Branches[index]);
	return coord;
}

// FUNCTION: 0x00634850
Coord PlanningNodeClass::Own_Coord(void)
{
	Coord coord = FUN_00633BC0(*(EventClass *)&Own);
	return coord;
}

// FUNCTION: 0x00634880
Point2D PlanningNodeClass::Branch_Pixel(int index)
{
	return Coord_To_View(FUN_00633BC0(*(EventClass *)Branches[index]));
}

// FUNCTION: 0x006348F0
Point2D PlanningNodeClass::Own_Pixel(void)
{
	return Coord_To_View(FUN_00633BC0(*(EventClass *)&Own));
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
	int count = Members.Count();
	for (int i = 0; i < count; i++) {
		FUN_00634E10(Members[i]);
	}
	Update_Active();
	Queued = false;
}

static inline void Locate_Own_Branch_Of(PlanningNodeClass * node)
{
	int * slot = &node->Own.Slot2;
	*slot = -1;
	if (node->Own.Pending != 0) {
		EventClass self = *(EventClass *)&node->Own;
		int branches = node->Branches.Count();
		int b = 0;
		if (branches > 0) {
			do {
				EventClass other = *(EventClass *)node->Branches[b];
				Coord a = FUN_00633BC0(self);
				Coord c = FUN_00633BC0(other);
				if (a == c) {
					node->Own.Slot2 = b;
					return;
				}
				b++;
			} while (b < branches);
		}
	}
}

static inline PlanningNodeClass * Last_Node(PlanToken * token)
{
	int index = token->Nodes.Count() - 1;
	if (index < 0) return 0;
	return token->Nodes[index];
}

// FUNCTION: 0x006349B0
void PlanningNodeClass::Update_Active(void)
{
	int count = Members.Count();
	for (int i = 0; i < count; i++) {
		PlanMember * member = Members[i];
		TechnoClass * owner = member->Owner;
		PlanToken * token = FUN_00705D20(owner);
		if (token->Nodes.ID(this) == token->LoopStart) {
			member->Active = true;
			PlanBranch * own = &Own;
			if (++own->Pending == 1) {
				token = FUN_00705D20(owner);
				PlanningNodeClass * last = Last_Node(token);
				int found = last->FUN_00634290(owner);
				PlanMember * source = (found == -1) ? 0 : last->Members[found];
				*(EventClass *)own = *source->Packet;
			}
		}
	}
	Locate_Own_Branch_Of(this);
}

// FUNCTION: 0x00634CC0
int PlanningNodeClass::Find_Branch(TechnoClass * techno)
{
	PlanToken * token = FUN_00705D20(techno);
	PlanningNodeClass * prev;
	int index = token->Nodes.ID(this);
	if (index == -1) {
		prev = 0;
	} else {
		int count = token->Nodes.Count();
		if (count == 1) {
			prev = this;
		} else {
			index--;
			if (index < 0) index = count - 1;
			prev = token->FUN_00636E60(index);
		}
	}
	if (prev == Last_Node(FUN_00705D20(techno))) goto none;
	{
		int found = prev->FUN_00634290(techno);
		PlanMember * member = (found == -1) ? 0 : prev->Members[found];
		EventClass event = *member->Packet;
		int count = Branches.Count();
		if (count == 0) goto none;
		{
			Coord origin = FUN_00633BC0(event);
			int i = 0;
			if (count > 0) {
				do {
					PlanBranch * branch = Branches[i];
					if (branch->Pending != 0) {
						EventClass other = *(EventClass *)branch;
						Coord c = FUN_00633BC0(other);
						if (origin.X == c.X && origin.Y == c.Y && origin.Z == c.Z) return i;
					}
					i++;
				} while (i < count);
			}
		}
	}
none:
	return -1;
}

static inline void Queue_Node(PlanningNodeClass * node)
{
	if (node && !node->Queued) {
		node->Queued = true;
		PlanningNodeClass::Unknown3.Add(node);
	}
}

// FUNCTION: 0x00635060
void PlanningNodeClass::Detach(PlanMember * member)
{
	if (member->Active) {
		member->Active = false;
		if (--Own.Pending == 0) {
			Own.Slot2 = -1;
		}
	}
	int branch = member->Branch;
	if (branch != -1) {
		PlanBranch * entry = Branches[branch];
		entry->Slot = -1;
		Own.Slot3 = -1;
		if (--entry->Pending == 0) {
			Queue_Node(this);
		}
	}
	member->Branch = -1;
}

// FUNCTION: 0x00635120
void PlanningNodeClass::Clear(void)
{
	int branches = Branches.Count();
	for (int i = 0; i < branches; i++) {
		delete Branches[i];
	}
	Branches.Clear();
	int members = Members.Count();
	for (int j = 0; j < members; j++) {
		PlanMember * member = Members[j];
		member->Branch = -1;
		member->Active = false;
	}
	Own.Pending = 0;
	int * slot = &Own.Slot2;
	*slot = -1;
}


// FUNCTION: 0x006351A0
bool PlanningNodeClass::Is_Loop_Start(TechnoClass * techno)
{
	PlanToken * token = FUN_00705D20(techno);
	return token->Nodes.ID(this) == token->LoopStart;
}

// FUNCTION: 0x006351E0
void PlanningNodeClass::Deactivate(PlanMember * member)
{
	member->Active = false;
	if (--Own.Pending == 0) {
		Own.Slot2 = -1;
	}
}

// FUNCTION: 0x00635210
void PlanningNodeClass::Locate_Own_Branch(void)
{
	Locate_Own_Branch_Of(this);
}

// FUNCTION: 0x006352D0
void PlanningNodeClass::Select_All(void)
{
	int count = Members.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * owner = Members[i]->Owner;
		if (!owner->IsSelected) {
			owner->Select();
		}
	}
}

// FUNCTION: 0x00635310
void PlanningNodeClass::Unselect_All(void)
{
	int count = Members.Count();
	for (int i = 0; i < count; i++) {
		TechnoClass * owner = Members[i]->Owner;
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
	rect = FUN_00634510(8, Coord_To_View(Event_Coord(*Members[0]->Packet)));
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
	Rect rect = Inflated(Around(8, Pixel_Of(Event_Coord(*Members[0]->Packet))), 4);
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

struct SchemeColor
{
	char Gap[0x56f9];
	RGBClass Color;
};

extern SchemeColor ** DAT_00A8022C;
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

static inline int Owner_Slot(TechnoClass * techno)
{
	HouseClass * house = techno->House;
	int index = house->HeapID;
	if (house->Is_Player_Control() && PlayerPtr) {
		index = PlayerPtr->HeapID;
	}
	return index;
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
	int result = Own.Slot3;
	if (result == -1) {
		TechnoClass * owner = Members[0]->Owner;
		int slot = Owner_Slot(owner);
		result = Mix_7(RGBClass(DAT_00A8022C[slot]->Color), DAT_00A83CD9, 0x50);
		Own.Slot3 = result;
		int count = Branches.Count();
		for (int i = 0; i < count; i++) {
			if (Branches[i]->Slot == result) goto done;
		}
		if (count == 0) {
			PlanToken * token = FUN_00705D20(owner);
			int index = token->Nodes.ID(this);
			int start = token->F8C;
			bool past = false;
			if (start != -1) past = index >= start;
			if (past) {
				slot = Owner_Slot(owner);
				Own.Slot3 = FUN_0063D070(RGBClass(DAT_00A8022C[slot]->Color), DAT_00A80220, 0x64);
			}
		} else {
			slot = Owner_Slot(owner);
			Own.Slot3 = FUN_0063D070(RGBClass(DAT_00A8022C[slot]->Color), DAT_00A80220, 0x64);
		}
		result = Own.Slot3;
	}
done:
	return result;
}

// FUNCTION: 0x00635DB0
void PlanningNodeClass::Reset_Colors(bool all)
{
	Own.Slot3 = -1;
	if (all) {
		int count = Branches.Count();
		int i = 0;
		if (count > 0) do {
			Branches[i]->Slot = -1;
			i++;
		} while (i < count);
		Own.Slot = -1;
	}
}

static inline Rect Box_Inline(int size, Point2D center)
{
	int offset = -(size / 2);
	Point2D corner = center;
	corner.X += offset;
	corner.Y += offset;
	return Rect(corner, size, size);
}

// FUNCTION: 0x00635DF0
Rect PlanningNodeClass::Lead_Box_Rect(void)
{
	Coord coord = FUN_00633BC0(*Members[0]->Packet);
	Point2D pixel;
	TacticalMap->Coord_To_Pixel(coord, pixel);
	pixel.X += DSurface::ViewBounds.X;
	pixel.Y += DSurface::ViewBounds.Y;
	return Inflated(Box_Inline(8, pixel), 4);
}

// FUNCTION: 0x00635E90
Rect PlanningNodeClass::Lead_Box_Rect2(void)
{
	return Inflated(Inflated(Around(8, Pixel_Of(Event_Coord(*Members[0]->Packet))), 4), 4);
}

// FUNCTION: 0x00635F20
PlanToken::PlanToken(TechnoClass * owner) : F8C(-1), LoopCount(-1), LoopStart(-1)
{
	F1C = false;
	F1D = false;
	F98 = false;
	F99 = false;
	Owner = owner;
}

extern bool DAT_00AC4CF5;
void __fastcall FUN_00639F80(int slot);

static inline PlanMember * Lookup_Member(PlanningNodeClass * node, TechnoClass * techno)
{
	int index = node->FUN_00634290(techno);
	if (index == -1) {
		return 0;
	}
	return node->Members[index];
}

static inline void Unregister_Token(PlanningTokenClass * token)
{
	if (!DAT_00AC4CF5) {
		DynamicVectorClass<PlanningTokenClass *> * array = &PlanTokens::Array;
		int id = array->ID(token);
		if (id != -1) {
			array->Delete_Index(id);
		}
	}
}

// FUNCTION: 0x00635F80
PlanToken::~PlanToken()
{
	if (Owner) {
		Owner->FUN_00705D10(0);
	}
	Unregister_Token((PlanningTokenClass *)this);
	int count = Nodes.Count();
	bool none = (count == 0);
	for (int i = 0; i < count; i++) {
		PlanningNodeClass * node = Nodes[i];
		PlanMember * member = Lookup_Member(node, Owner);
		if (member) {
			node->FUN_00635060(member);
			delete member->Packet;
			((MemberVecOps *)&node->Members)->Delete(member);
			delete member;
			if (node->Members.Count() == 0 && node) {
				node->FUN_0063D9C0(1);
			}
		}
	}
	if (!none) {
		TechnoClass * owner = Owner;
		int slot = Owner_Slot(owner);
		FUN_00639F80(slot);
		((TechnoVecOps *)&PlanTechnos::Technos)->Add(owner);
	}
	Nodes.Clear();
	F8C = -1;
	LoopCount = -1;
	LoopStart = -1;
	F98 = false;
	F99 = false;
}

extern int DAT_00AC4B84[24];

// FUNCTION: 0x00636110
TechnoClass * PlanToken::Get_Owner(void)
{
	return Owner;
}

static inline void Release_Owner(PlanningNodeClass * node, TechnoClass * owner)
{
	PlanMember * member = Lookup_Member(node, owner);
	PlanMember * key = member;
	if (member) {
		if (member->Active) {
			node->FUN_006351E0(member);
		}
		if (member->Branch != -1) {
			PlanBranch * branch = node->Branches[member->Branch];
			branch->Slot = -1;
			node->FUN_00635DB0(false);
			if (--branch->Pending == 0) {
				node->FUN_006378B0();
			}
		}
		member->Branch = -1;
		delete key->Packet;
		DynamicVectorClass<PlanMember *> * members = &node->Members;
		int id = members->ID(key);
		if (id != -1) {
			((MemberVecOps *)members)->Delete_Index(id);
		}
		delete key;
		if (node->Members.Count() == 0 && node) {
			delete node;
		}
	}
}

// FUNCTION: 0x00636120
void PlanToken::Cancel_Owner(void)
{
	int count = Nodes.Count();
	bool none = (count == 0);
	for (int i = 0; i < count; i++) {
		PlanningNodeClass * node = Nodes[i];
		Release_Owner(node, Owner);
	}
	if (!none) {
		TechnoClass * owner = Owner;
		int slot = Owner_Slot(owner);
		if (slot >= 0 && slot < 24) {
			int value = DAT_00AC4B84[slot] - 1;
			DAT_00AC4B84[slot] = (value < 0) ? 0 : value;
		}
		DynamicVectorClass<TechnoClass *> * technos = &PlanTechnos::Technos;
		int id = technos->ID(owner);
		if (id != -1) {
			technos->Delete_Index(id);
		}
	}
	Nodes.Clear();
	F8C = -1;
	LoopCount = -1;
	LoopStart = -1;
	F98 = false;
	F99 = false;
}

static inline void Unregister_Token_B(PlanningTokenClass * token)
{
	if (!DAT_00AC4CF5) {
		DynamicVectorClass<PlanningTokenClass *> * array = &PlanTokens::Array;
		int id = array->ID(token);
		if (id != -1) {
			((TokenVecOps *)array)->Delete_Index(id);
		}
	}
}

struct PlanTokenB
{
	TechnoClass * Owner;
	DynamicVectorClass<PlanningNodeClass *> Nodes;
	bool F1C;
	bool F1D;
	char Gap[0x6c];
	int F8C;
	int LoopCount;
	int LoopStart;
	bool F98;
	bool F99;

	~PlanTokenB()
	{
		if (Owner) {
			Owner->FUN_00705D10(0);
		}
		Unregister_Token_B((PlanningTokenClass *)this);
		int count = Nodes.Count();
		bool none = (count == 0);
		for (int i = 0; i < count; i++) {
			Nodes[i]->FUN_006340B0(Owner);
		}
		if (!none) {
			TechnoClass * owner = Owner;
			FUN_00639F80(((TechnoSlotOps *)owner)->GetControllingHouse());
			((TechnoVecOps *)&PlanTechnos::Technos)->Add(owner);
		}
		Nodes.Clear();
		F98 = false;
		F8C = -1;
		LoopCount = -1;
		LoopStart = -1;
		F99 = false;
	}
};

// FUNCTION: 0x00636310
void PlanToken::Destroy(void)
{
	int count = Nodes.Count();
	bool none = (count == 0);
	for (int i = 0; i < count; i++) {
		PlanningNodeClass * node = Nodes[i];
		PlanMember * member = Lookup_Member(node, Owner);
		if (member) {
			node->FUN_00635060(member);
			delete member->Packet;
			DynamicVectorClass<PlanMember *> * members = &node->Members;
			int id = members->ID(member);
			if (id != -1) {
				((MemberVecOps *)members)->Delete_Index(id);
			}
			delete member;
			if (node->Members.Count() == 0 && node) {
				node->FUN_0063D9C0(1);
			}
		}
	}
	if (!none) {
		TechnoClass * owner = Owner;
		int slot = Owner_Slot(owner);
		FUN_00639F80(slot);
		((TechnoVecOps *)&PlanTechnos::Technos)->Add(owner);
	}
	Nodes.Clear();
	F98 = false;
	F8C = -1;
	LoopCount = -1;
	LoopStart = -1;
	F99 = false;
	Owner = 0;
	PlanTokenB * self = (PlanTokenB *)this;
	delete self;
}

// FUNCTION: 0x00636520
void PlanToken::Set_F1C(void)
{
	F1C = true;
}

// FUNCTION: 0x00636530
void PlanToken::Clear_F1C(void)
{
	F1C = false;
}

// FUNCTION: 0x00636540
bool PlanToken::Get_F1C(void)
{
	return F1C;
}

// FUNCTION: 0x00636550
bool PlanToken::Owner_Is_Mission(void)
{
	return Owner->CurrentMission == 0x1c || Owner->MissionQueue == 0x1c;
}

// FUNCTION: 0x00636570
bool PlanToken::Has_Started(void)
{
	return Nodes.Count() > 0 && (F8C == -1 || F8C > 0);
}

// FUNCTION: 0x00636590
EventClass PlanToken::Pop_Front(void)
{
	PlanningNodeClass * node = Nodes[0];
	PlanMember * first = Lookup_Member(node, Owner);
	EventClass event = *first->Packet;
	*(EventClass *)&F1D = event;
	Nodes.Delete_Index(0);
	if (LoopStart == 0) {
		F99 = true;
		Nodes.Add(node);
		Queue_Node(node);
	} else {
		PlanMember * member = Lookup_Member(node, Owner);
		if (member) {
			node->FUN_00635060(member);
			delete member->Packet;
			DynamicVectorClass<PlanMember *> * members = &node->Members;
			((MemberVecOps *)members)->Delete(member);
			delete member;
			if (node->Members.Count() == 0 && node) {
				node->FUN_0063D9C0(1);
			}
		}
		if (LoopStart > 0) {
			LoopStart--;
			LoopStart = (LoopStart <= 0) ? 0 : LoopStart;
		}
	}
	int f = F8C - 1;
	if (f < -1) f = -1;
	F8C = f;
	if (Nodes.Count() == 0) {
		TechnoClass * owner = Owner;
		int slot = Owner_Slot(owner);
		if (slot >= 0 && slot < 24) {
			int value = DAT_00AC4B84[slot] - 1;
			DAT_00AC4B84[slot] = (value < 0) ? 0 : value;
		}
		DynamicVectorClass<TechnoClass *> * technos = &PlanTechnos::Technos;
		int id = technos->ID(owner);
		if (id != -1) {
			((TechnoVecOps *)technos)->Delete_Index(id);
		}
	}
	return event;
}

static inline void Add_Member_Inline(PlanningNodeClass * node, TechnoClass * techno, EventClass const * event)
{
	PlanMember * member = new PlanMember;
	member->Owner = techno;
	EventClass * packet = new EventClass;
	*packet = *event;
	member->Packet = packet;
	node->Members.Add(member);
	PlanToken * token = FUN_00705D20(techno);
	token->Nodes.Add(node);
	if (token->F8C == -1) {
		token->F8C = token->Nodes.Count() - 1;
	}
	node->FUN_00634E10(member);
}

static inline PlanMember * Lookup_Member_Inline(PlanningNodeClass * node, TechnoClass * techno)
{
	int index = node->Find_Member(techno);
	if (index == -1) {
		return 0;
	}
	return node->Members[index];
}

// FUNCTION: 0x006367B0
void PlanToken::Share_Range(TechnoClass * techno, int start, int count)
{
	if (FUN_00705D20(techno)) {
		int end = start + count;
		for (int i = start; i < end; i++) {
			PlanningNodeClass * node = Nodes[i];
			TechnoClass * owner = Owner;
			PlanMember * member = Lookup_Member_Inline(node, owner);
			EventClass event = *member->Packet;
			event.Data.Target.Whom = TargetClass(techno);
			Add_Member_Inline(node, techno, &event);
		}
	}
}

// FUNCTION: 0x00636980
void PlanToken::Share_From(TechnoClass * techno, int start)
{
	int count = Nodes.Count() - start;
	if (FUN_00705D20(techno)) {
		for (int i = start; i < count + start; i++) {
			PlanningNodeClass * node = Nodes[i];
			TechnoClass * owner = Owner;
			PlanMember * member = Lookup_Member_Inline(node, owner);
			EventClass event = *member->Packet;
			event.Data.Target.Whom = TargetClass(techno);
			Add_Member_Inline(node, techno, &event);
		}
	}
}

// FUNCTION: 0x00636D90
int PlanToken::Get_F8C(void)
{
	return F8C;
}

// FUNCTION: 0x00636DA0
bool PlanToken::Reached(int index)
{
	if (F8C == -1) return false;
	return index >= F8C;
}

// FUNCTION: 0x00636DC0
int PlanToken::Node_Count(void)
{
	return Nodes.Count();
}

// FUNCTION: 0x00636DD0
int PlanToken::Current_Or_Count(void)
{
	if (F8C == -1) return Nodes.Count();
	return F8C;
}

// FUNCTION: 0x00636DE0
int PlanToken::Remaining(void)
{
	if (F8C == -1) return 0;
	return Nodes.Count() - F8C;
}

// FUNCTION: 0x00636E00
void PlanToken::Add_Node(PlanningNodeClass * node)
{
	Nodes.Add(node);
	if (F8C == -1) {
		F8C = Nodes.Count() - 1;
	}
}

// FUNCTION: 0x00636E60
PlanningNodeClass * PlanToken::Wrapped_Node_At(int index)
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

// FUNCTION: 0x00636EB0
PlanningNodeClass * PlanToken::Last_Node(void)
{
	int index = Nodes.Count() - 1;
	if (index < 0) return 0;
	return Nodes[index];
}

// FUNCTION: 0x00636EC0
bool PlanToken::Has_Node(PlanningNodeClass * node)
{
	return Nodes.ID(node) != -1;
}

// FUNCTION: 0x00636EE0
int PlanToken::Node_Index(PlanningNodeClass * node)
{
	return Nodes.ID(node);
}

// FUNCTION: 0x00636F00
EventClass PlanToken::Event_At(int index)
{
	PlanningNodeClass * node = Nodes[index];
	TechnoClass * owner = Owner;
	int found = -1;
	int count = node->Members.Count();
	for (int i = 0; i < count; i++) {
		if (node->Members[i]->Owner == owner) {
			found = i;
			break;
		}
	}
	PlanMember * member = (found == -1) ? 0 : node->Members[found];
	return *member->Packet;
}

// FUNCTION: 0x00636F60
EventClass PlanToken::Last_Popped(void)
{
	return *(EventClass *)&F1D;
}

// FUNCTION: 0x00636F80
PlanningNodeClass * PlanToken::Loop_Node(void)
{
	if (LoopStart == -1) return 0;
	return Nodes[LoopStart];
}

// FUNCTION: 0x00636FA0
void PlanToken::Set_Loop(int count, int start)
{
	LoopCount = count;
	LoopStart = start;
	F98 = true;
}

// FUNCTION: 0x00636FC0
int PlanToken::Get_Loop_Count(void)
{
	return LoopCount;
}

// FUNCTION: 0x00636FD0
int PlanToken::Get_Loop_Start(void)
{
	return LoopStart;
}

// FUNCTION: 0x00636FE0
bool PlanToken::Loop_Start_Valid(void)
{
	return LoopStart > 0 || LoopStart == 0;
}

// FUNCTION: 0x00637000
bool PlanToken::Loop_Start_Positive(void)
{
	return LoopStart > 0;
}

// FUNCTION: 0x00637010
bool PlanToken::Loop_Start_Zero(void)
{
	return LoopStart == 0;
}
