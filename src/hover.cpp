// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "hover.h"
#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "infantry.h"
#include "inline.h"
#include "ion.h"
#include "overtype.h"
#include "rules.h"
#include "tube.h"
#include "unit.h"
#include "tube.hh"

// FUNCTION: 0x00513C20
HoverLocomotionClass::HoverLocomotionClass(void) :
	DestinationCoord(COORD_NONE),
	HeadToCoord(COORD_NONE),
	Facing(0),
	Height(0),
	Acceleration(0),
	Boost(1),
	Bounciness(0),
	WasShoved(false),
	ShoveAccum(0),
	WasPushed(false)
{
}

// FUNCTION: 0x00513CB0
HRESULT STDMETHODCALLTYPE HoverLocomotionClass::Link_To_Object(void *pointer)
{
	HRESULT res = BASECLASS::Link_To_Object(pointer);
	FacingClass face(2 * LinkedTo->TClass->ROT);
	Facing = face;
	return(res);
}

// FUNCTION: 0x00513D00
HoverLocomotionClass::~HoverLocomotionClass(void)
{
}

// FUNCTION: 0x00513D20
void HoverLocomotionClass::Gravity_AI(void)
{
	int height = LinkedTo->HeightAGL;
	int clearance = height;

	if (LinkedTo->Path[0] != FACING_NONE) {
		int mheight = Map.Get_Height_GL(LinkedTo->PositionCoord);
		if (Map.Get_Height_GL(Adjacent_Cell(LinkedTo->PositionCoord, LinkedTo->Path[0])) > mheight) {
			clearance -= Rule->HoverHeight;
		}
	}

	height = (int)(height + Bounciness);
	int id = LinkedTo->Fetch_ID();

	double bob_time_span = (id & 1 ? 1.0 : 1.1) * Rule->HoverBob * TICKS_PER_MINUTE;

	double bob_angle = fastmath::sin((double)((Frame + 2 * id) % (int)bob_time_span) * DEG_TO_RAD(360) / bob_time_span);

	height = int(2 * bob_angle + height);
	if (height < 0) {
		Bounciness = 0;
		height = 0;
	}

	bool wasdown = LinkedTo->IsDown;
	LinkedTo->IsDown = false;
	LinkedTo->HeightAGL = height;
	LinkedTo->IsDown = wasdown;

	if (clearance < Rule->HoverHeight) {
		if (BASECLASS::Is_Powered()) {
			Bounciness += ((double)Rule->HoverHeight + (double)Rule->HoverHeight - (double)clearance) / (double)Rule->HoverHeight * (double)Rule->Gravity;
		}
		if (clearance < Rule->HoverHeight / 4) {
			Bounciness += (double)(Rule->Gravity / 3);
		}
	}
	Bounciness -= Rule->Gravity;
	Bounciness *= Rule->HoverDampen;
}

// FUNCTION: 0x00513F40
Matrix3DStruct STDMETHODCALLTYPE HoverLocomotionClass::Draw_Matrix(int *key)
{
	if ((float)fabs(LinkedTo->AngleRotatedSideways) >= 0.005 || (float)fabs(LinkedTo->AngleRotatedForwards) >= 0.005) {
		if (key) {
			*key = -1;
		}

		Matrix3D m1(true), m2(true);
		Matrix3D basemtx;

		float val = LinkedTo->TClass->VoxelCenterY;
		float val2 = LinkedTo->TClass->VoxelCenterX;

		float scos, fcos, fsin;
		fcos = fastmath::cos(LinkedTo->AngleRotatedForwards);
		fsin = fastmath::sin(LinkedTo->AngleRotatedForwards);

		scos = fastmath::cos(LinkedTo->AngleRotatedSideways);
		float ssin = fastmath::sin(LinkedTo->AngleRotatedSideways);

		int num = fabs(ssin) * val + fabs(fsin) * val2;

		int x_offset = (int)(val2 - (float)(int)(fcos * val2));
		int y_offset = (int)(val - (float)(int)(scos * val));
		if (LinkedTo->AngleRotatedForwards < 0.0f) {
			x_offset = -x_offset;
		}
		if (LinkedTo->AngleRotatedSideways > 0.0f) {
			y_offset = -y_offset;
		}

		m1.Translate_Z(num);
		m2.Translate_X(x_offset);
		m2.Translate_Y(y_offset);
		m2.Rotate_X(LinkedTo->AngleRotatedSideways);
		m2.Rotate_Y(LinkedTo->AngleRotatedForwards);

		return(basemtx.Matrix3D::Matrix3D(BASECLASS::Draw_Matrix(key)), (Matrix3DStruct &)((m1 * basemtx) * m2));
	}

	if (!Is_Powered()) {
		int ramp = Map[(Coord const &)(LinkedTo->PositionCoord)].Ramp;
		Matrix3D mtx = Get_Slope_Matrix(ramp);
		mtx.Rotate_Z(LinkedTo->PrimaryFacing.Current().As_Radian32());
		if (key != NULL && *key != -1) {
			*key = 32 * (ramp + (*key << 6));
			*key |= LinkedTo->PrimaryFacing.Current().As_Dir32();
		}
		return((Matrix3DStruct &)mtx);
	}
	return(BASECLASS::Draw_Matrix(key));
}

// FUNCTION: 0x005142A0
Matrix3DStruct STDMETHODCALLTYPE HoverLocomotionClass::Shadow_Matrix(int *key)
{
	if (!((float)fabs(LinkedTo->AngleRotatedSideways) < 0.005 && (float)fabs(LinkedTo->AngleRotatedForwards) < 0.005) && key) {
		*key = -1;
	}
	return(BASECLASS::Shadow_Matrix(key));
}

// FUNCTION: 0x00514C30
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Moving(void)
{
	return(DestinationCoord != COORD_NONE || HeadToCoord != COORD_NONE);
}

// FUNCTION: 0x00514C80
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Moving_Now(void)
{
	return(Is_Moving() && Height != 0.0);
}

// FUNCTION: 0x00514CB0
CoordStruct STDMETHODCALLTYPE HoverLocomotionClass::Destination(void)
{
	if (DestinationCoord != COORD_NONE) {
		return(DestinationCoord);
	}
	return(COORD_NONE);
}

// FUNCTION: 0x00514D10
CoordStruct STDMETHODCALLTYPE HoverLocomotionClass::Head_To_Coord(void)
{
	if (HeadToCoord != COORD_NONE) {
		return(HeadToCoord);
	}
	return(LinkedTo->PositionCoord);
}

void HoverLocomotionClass::Start(void)
{
	bool anew = HeadToCoord == COORD_NONE || LinkedTo->PositionCell == HeadToCoord.As_Cell();

	Height = 1.0;
	LinkedTo->Set_Speed(1.0);
	Start_Of_Move(0);
	if (anew) {
		While_Moving(true);
	}
}

// FUNCTION: 0x00514D90
void STDMETHODCALLTYPE HoverLocomotionClass::Move_To(CoordStruct to)
{
	DestinationCoord = to;
	if (LinkedTo->IsParalyzed()) {
		return;
	}
	if (Is_Powered() && Is_Ion_Sensitive() && LinkedTo->House->IonSensitivesShouldBeOffline()) {
		Power_Off();
	}
	DestinationCoord.Z = Map.Get_Height_GL(DestinationCoord);

	if (Map[to].IsUnderBridge) {
		DestinationCoord.Z += BRIDGE_LEPTON_HEIGHT;
	}

	if ((Coord &)to != COORD_NONE && Is_Powered()) {
		if (!Is_Moving_Now()) {
			LinkedTo->Set_Speed(1.0);
			Start_Of_Move(0);
			While_Moving(true);
		} else if (HeadToCoord == COORD_NONE) {
			Start();
		}
	}
}

// FUNCTION: 0x00515E30
void HoverLocomotionClass::Stop(void)
{
	LinkedTo->Assign_Destination(NULL);
	Acceleration = 0;
	Height = 0;
	LinkedTo->Set_Speed(0);
}

// FUNCTION: 0x00515E60
void HoverLocomotionClass::Stop_Driver(void)
{
	Acceleration = 0;
	Height = 0;
	LinkedTo->Set_Speed(0);
	if (HeadToCoord != COORD_NONE) {
		LinkedTo->Clear_Occupy_Bit(HeadToCoord);
		HeadToCoord = COORD_NONE;
	}
}

// FUNCTION: 0x005163A0
bool HoverLocomotionClass::Is_Moving1(void)
{
	if (Is_Moving()) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00516FC0
boolean STDMETHODCALLTYPE HoverLocomotionClass::Shove(DirStruct dir)
{
	if (Push(dir)) {
		Do_Shove();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00517020
void HoverLocomotionClass::Do_Shove(void)
{
	WasShoved = true;
	ShoveAccum = Random_Pick(20, 30);
	if (Percent_Chance(50)) {
		ShoveAccum = -ShoveAccum;
	}
}

// FUNCTION: 0x00517070
HRESULT STDMETHODCALLTYPE HoverLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_HoverLocomotion;
	return(S_OK);
}

// FUNCTION: 0x00517100
LayerType STDMETHODCALLTYPE HoverLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x00517210
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	Coord coord = Head_To_Coord();

	if (coord.As_Cell() == ((Coord &)to).As_Cell() && abs(coord.Z - to.Z) <= LEVEL_LEPTON_H) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x005172B0 HoverLocomotionClass::Fetch_Object_Size

// FUNCTION: 0x00517310 ?$TPoint2D@H::operator==
template<class T>
bool TPoint2D<T>::operator == (TPoint2D<T> const & rvalue) const
{
	return(X==rvalue.X && Y==rvalue.Y);
}

template class TPoint2D<int>;
