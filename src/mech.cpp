// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "mech.h"
#include "_map.h"
#include "_rules.h"
#include "assert.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "noinit.h"
#include "overtype.h"
#include "rules.h"
#include "tactical.h"
#include "tube.h"
#include "layer.hh"
#include <new.h>

// SYNTHETIC: GAMEMD 0x005AFCE0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005AFD10 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005AFD30 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005AFD50 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005AFD70 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005AFD90 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005AFDB0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005AFDF0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005AFE20 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005AFE50 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005AFE80 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005AFE90 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005AFEB0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005AFED0 SYMBOL
// _$E52

// FUNCTION: GAMEMD 0x005AFEF0
MechLocomotionClass::MechLocomotionClass(void) :
	BASECLASS(),
	DestinationCoord(COORD_NONE),
	HeadToCoord(COORD_NONE),
	IsMoving(false)
{
}

// FUNCTION: GAMEMD 0x005AFF50
// FUNCTION: GAMEMD 0x005B1B50
MechLocomotionClass::~MechLocomotionClass(void)
{
}

// FUNCTION: GAMEMD 0x005AFF70
boolean STDMETHODCALLTYPE MechLocomotionClass::Is_Moving(void)
{
	return(IsMoving);
}

// FUNCTION: GAMEMD 0x005AFF80
CoordStruct STDMETHODCALLTYPE MechLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(COORD_NONE);
}

// FUNCTION: GAMEMD 0x005AFFE0
CoordStruct STDMETHODCALLTYPE MechLocomotionClass::Head_To_Coord(void)
{
	if (HeadToCoord != COORD_NONE) {
		return(HeadToCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: GAMEMD 0x005B0060
boolean STDMETHODCALLTYPE MechLocomotionClass::Process(void)
{
	Movement_AI(true);
	return(Is_Moving());
}

// FUNCTION: GAMEMD 0x005B0080
void STDMETHODCALLTYPE MechLocomotionClass::Move_To(CoordStruct to)
{
	if (!LinkedTo->IsUnderEMP() && !LinkedTo->IsParalyzed() && !LinkedTo->IsBeingWarpedOut() && !LinkedTo->IsWarpingIn()) {
		DestinationCoord = to;
		if (Map[(Coord)to].IsUnderBridge) {
			DestinationCoord.Z += Tactical::Pixel_To_Z_Lepton(60);
		}
		IsMoving = true;
	}
}

// FUNCTION: GAMEMD 0x005B0120
void STDMETHODCALLTYPE MechLocomotionClass::Stop_Moving(void)
{
	DestinationCoord = COORD_NONE;
	if (HeadToCoord == COORD_NONE) {
		IsMoving = false;
	}
}

// FUNCTION: GAMEMD 0x005B0170
void MechLocomotionClass::Do_Turn(DirStruct coord)
{
	LinkedTo->PrimaryFacing.Set_Desired(coord);
}

// FUNCTION: GAMEMD 0x005B01A0
void STDMETHODCALLTYPE MechLocomotionClass::Force_Immediate_Destination(CoordStruct coord)
{
	HeadToCoord = coord;
}

// FUNCTION: GAMEMD 0x005B17B0
bool MechLocomotionClass::Mark_Head_To(Coord const & coord)
{
	Coord crd = coord;

	if (crd != COORD_NONE) {
		crd.Z = Map.Get_Height_GL(crd);
		if (Map[coord].IsUnderBridge && LinkedTo->Get_Coord().Z > crd.Z + 2 * LEVEL_LEPTON_H + LEVEL_LEPTON_H) {
			crd.Z += BRIDGE_LEPTON_HEIGHT;
		}
	}

	if (HeadToCoord != COORD_NONE) {
		LinkedTo->Clear_Occupy_Bit(HeadToCoord);
	}

	if (Map[crd].Goodie_Check(LinkedTo) && !LinkedTo->IsInLimbo) {
		HeadToCoord = crd;
	} else {
		HeadToCoord = COORD_NONE;
		if (!LinkedTo->IsActive) {
			return(false);
		}
	}

	if (HeadToCoord != COORD_NONE) {
		LinkedTo->Set_Occupy_Bit(HeadToCoord);
		return(true);
	}

	LinkedTo->Set_Occupy_Bit(LinkedTo->PositionCoord);
	return(false);
}

// FUNCTION: GAMEMD 0x005B1960
HRESULT STDMETHODCALLTYPE MechLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_MechLocomotion;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x005B19A0
HRESULT STDMETHODCALLTYPE MechLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) MechLocomotionClass(NoInitClass());
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005B19D0
LayerType STDMETHODCALLTYPE MechLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: GAMEMD 0x005B19E0
boolean STDMETHODCALLTYPE MechLocomotionClass::Is_Moving_Now(void)
{
	if (LinkedTo->PrimaryFacing.Is_Rotating()) {
		return(true);
	}
	if (Is_Moving() && HeadToCoord != COORD_NONE && LinkedTo->Current_Speed() > 0) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005B1A50
void STDMETHODCALLTYPE MechLocomotionClass::Mark_All_Occupation_Bits(int mark)
{
	if (mark == MARK_UP) {
		LinkedTo->Clear_Occupy_Bit((Coord)Head_To_Coord());
	}
}

// FUNCTION: GAMEMD 0x005B1AA0
boolean STDMETHODCALLTYPE MechLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	CoordStruct coord = Head_To_Coord();

	if (Coord(coord).As_Cell() == ((Coord &)to).As_Cell() && abs(coord.Z - to.Z) <= LEVEL_LEPTON_H) {
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x005B1B40
// MechLocomotionClass::Fetch_Object_Size
