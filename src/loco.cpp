// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "loco.h"
#include "_map.h"
#include "_tactica.h"
#include "assert.h"
#include "cell.h"
#include "coord.h"
#include "foot.h"
#include "globals.h"
#include "map.h"
#include "swizzle.h"
#include "tactical.h"
#include "fire.hh"
#include "move.hh"
#include "visual.hh"
#include "zgrad.hh"

// SYNTHETIC: GAMEMD 0x0055A4B0 SYMBOL
// _$E52
// SYNTHETIC: GAMEMD 0x0055A4D0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0055A500 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0055A520 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0055A540 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0055A560 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0055A580 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0055A5A0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0055A5E0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0055A610 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0055A640 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0055A670 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0055A680 SYMBOL
// _$E35

extern ULONG COMRefCount;
extern int DAT_00ABCCA0;
extern int DAT_00ABCCA4;
extern int DAT_00ABCCA8;

// FUNCTION: GAMEMD 0x0055A6A0
void FUN_0055A6A0(void)
{
	DAT_00ABCCA8 = 0;
	DAT_00ABCCA0 = 128;
	DAT_00ABCCA4 = 128;
}

// FUNCTION: GAMEMD 0x0055A6C0
LocomotionClass::LocomotionClass(void) :
	Owner(NULL),
	LinkedTo(NULL),
	IsPowered(true),
	Dirty(true),
	RefCount(0)
{
}

// FUNCTION: GAMEMD 0x0055A6F0
LocomotionClass::~LocomotionClass(void)
{
	LinkedTo = NULL;
}

// FUNCTION: GAMEMD 0x0055A710
HRESULT STDMETHODCALLTYPE LocomotionClass::Link_To_Object(void *pointer)
{
	LinkedTo = (FootClass *)pointer;
	Owner = (FootClass *)pointer;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0055A730
Matrix3DStruct STDMETHODCALLTYPE LocomotionClass::Draw_Matrix(int *key)
{
	Matrix3D draw_matrix(true);

	draw_matrix.Rotate_Z(LinkedTo->PrimaryFacing.Current().As_Radian32());

	if (key && *key != -1) {
		*key *= 32;
		*key |= LinkedTo->PrimaryFacing.Current().As_Dir32();
	}
	return((Matrix3DStruct &)draw_matrix);
}

// FUNCTION: GAMEMD 0x0055A7D0
Matrix3DStruct STDMETHODCALLTYPE LocomotionClass::Shadow_Matrix(int *key)
{
	int ramp = Map[LinkedTo->Get_Coord()].Ramp;

	Matrix3D draw_matrix(Get_Slope_Matrix(ramp));
	draw_matrix.Rotate_Z(LinkedTo->PrimaryFacing.Current().As_Radian32());

	if (key && *key != -1) {
		*key = 32 * (ramp + (*key << 6));
		*key |= LinkedTo->PrimaryFacing.Current().As_Dir32();
	}
	return((Matrix3DStruct &)draw_matrix);
}

// FUNCTION: GAMEMD 0x0055A8C0
Point2DStruct STDMETHODCALLTYPE LocomotionClass::Shadow_Point(void)
{
	Point2DStruct pt;

	pt.X = 0;
	pt.Y = TacticalMap->Z_Lepton_To_Pixel(LinkedTo->HeightAGL);

	return(pt);
}

// FUNCTION: GAMEMD 0x0055A8F0
boolean STDMETHODCALLTYPE LocomotionClass::Power_On(void)
{
	IsPowered = true;
	return(Is_Powered());
}

// FUNCTION: GAMEMD 0x0055A910
boolean STDMETHODCALLTYPE LocomotionClass::Power_Off(void)
{
	IsPowered = false;
	return(Is_Powered());
}

// FUNCTION: GAMEMD 0x0055A930
boolean STDMETHODCALLTYPE LocomotionClass::Is_Powered(void)
{
	return(IsPowered);
}

// FUNCTION: GAMEMD 0x0055A940
boolean STDMETHODCALLTYPE LocomotionClass::Is_Ion_Sensitive(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x0055A950
ULONG STDMETHODCALLTYPE LocomotionClass::AddRef(void)
{
	++COMRefCount;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: GAMEMD 0x0055A970
ULONG STDMETHODCALLTYPE LocomotionClass::Release(void)
{
	--COMRefCount;

	ULONG count = InterlockedDecrement(&RefCount);
	if (count == 0) {
		delete this;
	}
	return(count);
}

// FUNCTION: GAMEMD 0x0055A9B0
LONG STDMETHODCALLTYPE LocomotionClass::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	*ppvObject = NULL;

	if (riid == IID_IUnknown) {
		*ppvObject = (IUnknown *)(ILocomotion *)this;
	}
	if (riid == IID_IPersistStream) {
		*ppvObject = (IPersistStream *)this;
	}
	if (riid == IID_ILocomotion) {
		*ppvObject = (ILocomotion *)this;
	}
	if (riid == IID_IPersist) {
		*ppvObject = (IPersist *)this;
	}
	if (*ppvObject == NULL) {
		return(E_NOINTERFACE);
	}

	AddRef();
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0055AA60
HRESULT STDMETHODCALLTYPE LocomotionClass::Save(IStream * stream, BOOL cleardirty)
{
	if (stream == NULL) {
		return(E_POINTER);
	}

	ULONG id = (ULONG)(this);

	HRESULT result = stream->Write(&id, sizeof(id), NULL);
	if (SUCCEEDED(result)) {
		result = stream->Write(this, Fetch_Object_Size(), NULL);
		if (SUCCEEDED(result)) {
			if (cleardirty) {
				Dirty = false;
			}
		}
		return(result);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0055AAC0
HRESULT STDMETHODCALLTYPE LocomotionClass::Load(IStream * stream)
{
	if (stream == NULL) {
		return(E_POINTER);
	}

	ULONG id;
	HRESULT result = stream->Read(&id, sizeof(id), NULL);
	if (SUCCEEDED(result)) {
		Swizzle_Here_I_Am(id, this);

		LONG prev_refcount = RefCount;

		result = stream->Read((void *)this, Fetch_Object_Size(), NULL);

		RefCount = prev_refcount;
		Swizzle_Pointer(&LinkedTo);
		Swizzle_Pointer(&Owner);
		return(result);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0055AB40
LONG STDMETHODCALLTYPE LocomotionClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	if (pcbSize == NULL) {
		return(E_POINTER);
	}
	pcbSize->QuadPart = Fetch_Object_Size() + sizeof(FootClass *);
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0055AB70
boolean STDMETHODCALLTYPE LocomotionClass::Push(DirStruct dir)
{
	return(false);
}

// FUNCTION: GAMEMD 0x0055AB80
boolean STDMETHODCALLTYPE LocomotionClass::Shove(DirStruct dir)
{
	return(false);
}

// FUNCTION: GAMEMD 0x0055AB90
void STDMETHODCALLTYPE LocomotionClass::Tilt_Pitch_AI(void)
{
}

// FUNCTION: GAMEMD 0x0055ABA0
int STDMETHODCALLTYPE LocomotionClass::Z_Adjust(void)
{
	return(0);
}

// FUNCTION: GAMEMD 0x0055ABB0
ZGradientType STDMETHODCALLTYPE LocomotionClass::Z_Gradient(void)
{
	return(ZGRAD_90DEG);
}

// FUNCTION: GAMEMD 0x0055ABC0
VisualType STDMETHODCALLTYPE LocomotionClass::Visual_Character(boolean flag)
{
	return(VISUAL_NORMAL);
}

// FUNCTION: GAMEMD 0x0055ABD0
Point2DStruct STDMETHODCALLTYPE LocomotionClass::Draw_Point(void)
{
	Point2DStruct pt;
	pt.X = 0;
	pt.Y = 0;
	return(pt);
}

// FUNCTION: GAMEMD 0x0055ABE0
boolean STDMETHODCALLTYPE LocomotionClass::Is_To_Have_Shadow(void)
{
	return(true);
}

// FUNCTION: GAMEMD 0x0055ABF0
MoveType STDMETHODCALLTYPE LocomotionClass::Can_Enter_Cell(CellStruct cell)
{
	return(MOVE_OK);
}

// FUNCTION: GAMEMD 0x0055AC00
void STDMETHODCALLTYPE LocomotionClass::Force_Immediate_Destination(CoordStruct coord)
{
}

// FUNCTION: GAMEMD 0x0055AC10
void STDMETHODCALLTYPE LocomotionClass::Force_Track(int track, CoordStruct coord)
{
}

// FUNCTION: GAMEMD 0x0055AC20
void STDMETHODCALLTYPE LocomotionClass::Unlimbo(void)
{
}

// FUNCTION: GAMEMD 0x0055AC30
void STDMETHODCALLTYPE LocomotionClass::Do_Turn(DirStruct coord)
{
}

// FUNCTION: GAMEMD 0x0055AC40
void STDMETHODCALLTYPE LocomotionClass::Stop_Moving(void)
{
}

// FUNCTION: GAMEMD 0x0055AC50
void STDMETHODCALLTYPE LocomotionClass::Move_To(CoordStruct to)
{
}

// FUNCTION: GAMEMD 0x0055AC60
boolean STDMETHODCALLTYPE LocomotionClass::Process(void)
{
	return(true);
}

// FUNCTION: GAMEMD 0x0055AC70
CoordStruct STDMETHODCALLTYPE LocomotionClass::Destination(void)
{
	CoordStruct coord;
	coord.X = COORD_NONE.X;
	coord.Y = COORD_NONE.Y;
	coord.Z = COORD_NONE.Z;
	return(coord);
}

// FUNCTION: GAMEMD 0x0055ACA0
CoordStruct STDMETHODCALLTYPE LocomotionClass::Head_To_Coord(void)
{
	return(LinkedTo->PositionCoord);
}

// FUNCTION: GAMEMD 0x0055ACD0
boolean STDMETHODCALLTYPE LocomotionClass::Is_Moving(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x0055ACE0
void STDMETHODCALLTYPE LocomotionClass::Force_New_Slope(int ramp)
{
}

// FUNCTION: GAMEMD 0x0055ACF0
int STDMETHODCALLTYPE LocomotionClass::Drawing_Code(void)
{
	return(0);
}

// FUNCTION: GAMEMD 0x0055AD00
FireErrorType STDMETHODCALLTYPE LocomotionClass::Can_Fire(void)
{
	return(FIRE_OK);
}

// FUNCTION: GAMEMD 0x0055AD10
int STDMETHODCALLTYPE LocomotionClass::Apparent_Speed(void)
{
	return(LinkedTo->Current_Speed());
}
