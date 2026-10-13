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

class PlanningNodeClass
{
	public:
		static DynamicVectorClass<PlanningNodeClass *> Unknown1;
		static DynamicVectorClass<PlanningNodeClass *> Unknown2;
		static DynamicVectorClass<PlanningNodeClass *> Unknown3;
};
class PlanTokens { public: static DynamicVectorClass<PlanningTokenClass *> Array; };
class PlanEvents { public: static DynamicVectorClass<EventClass *> Events; };
class PlanTechnos { public: static DynamicVectorClass<TechnoClass *> Technos; };

DynamicVectorClass<EventClass *> PlanEvents::Events;
DynamicVectorClass<PlanningNodeClass *> PlanningNodeClass::Unknown3;
DynamicVectorClass<TechnoClass *> PlanTechnos::Technos;
DynamicVectorClass<PlanningNodeClass *> PlanningNodeClass::Unknown1;
DynamicVectorClass<PlanningNodeClass *> PlanningNodeClass::Unknown2;
DynamicVectorClass<PlanningTokenClass *> PlanTokens::Array;
