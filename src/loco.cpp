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

// FUNCTION: 0x0055A4D0 $E2
// FUNCTION: 0x0055A500 $E5
// FUNCTION: 0x0055A520 $E8
// FUNCTION: 0x0055A540 $E11
// FUNCTION: 0x0055A560 $E14
// FUNCTION: 0x0055A580 $E17
// FUNCTION: 0x0055A5A0 $E20
// FUNCTION: 0x0055A5E0 $E23
// FUNCTION: 0x0055A610 $E26
// FUNCTION: 0x0055A640 $E29
// FUNCTION: 0x0055A670 $E32
// FUNCTION: 0x0055A680 $E35

extern ULONG COMRefCount;
extern int DAT_00ABCCA0;
extern int DAT_00ABCCA4;
extern int DAT_00ABCCA8;

// FUNCTION: 0x0055A6A0
void FUN_0055A6A0(void)
{
	DAT_00ABCCA8 = 0;
	DAT_00ABCCA0 = 128;
	DAT_00ABCCA4 = 128;
}

// FUNCTION: 0x0055A6C0
LocomotionClass::LocomotionClass(void) :
	Owner(NULL),
	LinkedTo(NULL),
	IsPowered(true),
	Dirty(true),
	RefCount(0)
{
}

// FUNCTION: 0x0055A6F0
LocomotionClass::~LocomotionClass(void)
{
	LinkedTo = NULL;
}

// FUNCTION: 0x0055A710
HRESULT STDMETHODCALLTYPE LocomotionClass::Link_To_Object(void *pointer)
{
	LinkedTo = (FootClass *)pointer;
	Owner = (FootClass *)pointer;
	return(S_OK);
}

// FUNCTION: 0x0055A730
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

// FUNCTION: 0x0055A7D0
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

// FUNCTION: 0x0055A8C0
Point2DStruct STDMETHODCALLTYPE LocomotionClass::Shadow_Point(void)
{
	Point2DStruct pt;

	pt.X = 0;
	pt.Y = TacticalMap->Z_Lepton_To_Pixel(LinkedTo->HeightAGL);

	return(pt);
}

// FUNCTION: 0x0055A8F0
boolean STDMETHODCALLTYPE LocomotionClass::Power_On(void)
{
	IsPowered = true;
	return(Is_Powered());
}

// FUNCTION: 0x0055A910
boolean STDMETHODCALLTYPE LocomotionClass::Power_Off(void)
{
	IsPowered = false;
	return(Is_Powered());
}

// FUNCTION: 0x0055A930
boolean STDMETHODCALLTYPE LocomotionClass::Is_Powered(void)
{
	return(IsPowered);
}

// FUNCTION: 0x0055A940
boolean STDMETHODCALLTYPE LocomotionClass::Is_Ion_Sensitive(void)
{
	return(false);
}

// FUNCTION: 0x0055A950
ULONG STDMETHODCALLTYPE LocomotionClass::AddRef(void)
{
	++COMRefCount;
	return(InterlockedIncrement(&RefCount));
}

// FUNCTION: 0x0055A9B0
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

// FUNCTION: 0x0055ABB0
ZGradientType STDMETHODCALLTYPE LocomotionClass::Z_Gradient(void)
{
	return(ZGRAD_90DEG);
}

// FUNCTION: 0x0055ABD0
Point2DStruct STDMETHODCALLTYPE LocomotionClass::Draw_Point(void)
{
	Point2DStruct pt;
	pt.X = 0;
	pt.Y = 0;
	return(pt);
}

// FUNCTION: 0x0055AC70
CoordStruct STDMETHODCALLTYPE LocomotionClass::Destination(void)
{
	CoordStruct coord;
	coord.X = COORD_NONE.X;
	coord.Y = COORD_NONE.Y;
	coord.Z = COORD_NONE.Z;
	return(coord);
}
