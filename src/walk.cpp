// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "walk.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "aircraft.h"
#include "building.h"
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
#include "unit.h"
#include "layer.hh"
#include "fastmath.h"
#include "coord.h"

extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F69C0;
extern const double DAT_007F69C8;

// FUNCTION: 0x0075A8A0 $E96
double DAT_00B45BD0 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0075A8D0 $E99
double DAT_00B45C20 = DAT_00B45BD0 / DAT_007F69C0;

// FUNCTION: 0x0075A8F0 $E102
double DAT_00B45C08 = DAT_007E1718 / DAT_00B45C20;

// FUNCTION: 0x0075A910 $E105
double DAT_00B45C00 = DAT_007E1720 * DAT_007F69C8;

// FUNCTION: 0x0075A930 $E108
double DAT_00B45BD8 = DAT_007E1728 * DAT_007F69C8;

// FUNCTION: 0x0075A950 $E111
double DAT_00B45BB0 = DAT_007E1730 * DAT_007F69C8;

// FUNCTION: 0x0075A970 $E114
int DAT_00B45C28 = (int)(fastmath::tan(DAT_00B45BB0 - DAT_00B45BD8) * DAT_007E1738 * DAT_00B45BD0);

// FUNCTION: 0x0075A9B0 $E117
double DAT_00B45BE0 = fastmath::atan(DAT_00B45C28 * DAT_007E1740);

// FUNCTION: 0x0075A9E0 $E120
double DAT_00B45BF8 = fastmath::atan((DAT_00B45C28 * 2) / DAT_00B45BD0);

// FUNCTION: 0x0075AA10 $E123
int DAT_00B45C1C = (int)(DAT_00B45C28 * 4 + DAT_007E1738);

// FUNCTION: 0x0075AA40 $E126
Cell DAT_00B45BC8(0, 0);

// FUNCTION: 0x0075AA50 $E129
Coord DAT_00B45BE8(0, 0, 0);

// FUNCTION: 0x0075AA70 $E132
Coord DAT_00B45C10(128, 128, 0);

// FUNCTION: 0x0075AA90
WalkLocomotionClass::WalkLocomotionClass(void) :
	BASECLASS(),
	DestinationCoord(COORD_NONE),
	HeadToCoord(COORD_NONE),
	IsMoving(false),
	IsProcessingMovement(false),
	IsReallyMoving(false)
{
}

// FUNCTION: 0x0075AB00
WalkLocomotionClass::~WalkLocomotionClass(void)
{
}

// FUNCTION: 0x0075AB30
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving(void)
{
	return(IsMoving);
}

// FUNCTION: 0x0075AB40
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving_Now(void)
{
	if (Is_Moving() && LinkedTo->Speed > 0 && HeadToCoord != COORD_NONE) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0075ABA0
CoordStruct STDMETHODCALLTYPE WalkLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(COORD_NONE);
}

// FUNCTION: 0x0075AC00
CoordStruct STDMETHODCALLTYPE WalkLocomotionClass::Head_To_Coord(void)
{
	if (HeadToCoord != COORD_NONE) {
		return(HeadToCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: 0x0075AC80
boolean STDMETHODCALLTYPE WalkLocomotionClass::Process(void)
{
	IsProcessingMovement = true;
	Movement_AI(true);
	IsProcessingMovement = false;
	return(Is_Moving());
}

// FUNCTION: 0x0075ACB0
void STDMETHODCALLTYPE WalkLocomotionClass::Move_To(CoordStruct to)
{
	if (!LinkedTo->IsUnderEMP() && !LinkedTo->IsBeingWarpedOut() && !LinkedTo->IsWarpingIn()) {
		DestinationCoord = to;
		if ((Coord &)to != COORD_NONE) {
			if (Map[(Coord)to].IsUnderBridge) {
				DestinationCoord.Z += Tactical::Pixel_To_Z_Lepton(60);
			}
			IsMoving = true;
		} else {
			if (HeadToCoord == COORD_NONE) {
				bool was_moving = IsMoving;
				IsMoving = false;
				if (was_moving) {
					LinkedTo->vt_entry_54C();
				}
			}
		}
	}
}

// FUNCTION: 0x0075ADA0
void STDMETHODCALLTYPE WalkLocomotionClass::Stop_Moving(void)
{
	DestinationCoord = COORD_NONE;
	if (HeadToCoord == COORD_NONE) {
		IsMoving = false;
		IsReallyMoving = false;
		LinkedTo->vt_entry_54C();
	}
}

// FUNCTION: 0x0075AE00
void STDMETHODCALLTYPE WalkLocomotionClass::Do_Turn(DirStruct dir)
{
	LinkedTo->PrimaryFacing.Set(dir);
}

// FUNCTION: 0x0075AE30
void STDMETHODCALLTYPE WalkLocomotionClass::Force_Immediate_Destination(CoordStruct coord)
{
	bool was_moving = IsMoving;
	Coord head = coord;
	Mark_Head_To(head);
	if (HeadToCoord == COORD_NONE && DestinationCoord == COORD_NONE) {
		IsMoving = false;
		if (was_moving) {
			LinkedTo->vt_entry_54C();
		}
	}
}

// FUNCTION: 0x0075C640
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WalkLocomotion;
	return(S_OK);
}

// FUNCTION: 0x0075C7E0
LayerType STDMETHODCALLTYPE WalkLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x0075C850
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::Begin_Piggyback(ILocomotion * pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (Piggybacker == NULL) {
		Piggybacker = pointer;
		return(S_OK);
	}
	return(E_FAIL);
}

// FUNCTION: 0x0075C8A0
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::End_Piggyback(ILocomotion ** pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (Piggybacker != NULL) {
		*pointer = Piggybacker;
		Piggybacker.Detach();
		return(S_OK);
	}
	return(S_FALSE);
}

// FUNCTION: 0x0075CA80
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	Coord headto = Head_To_Coord();
	if (headto.As_Cell() == Coord(to).As_Cell() && abs(headto.Z - to.Z) <= LEVEL_LEPTON_H) {
		return(true);
	}
	return(false);
}
