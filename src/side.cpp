// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ship.h"
#include "_map.h"
#include "_rules.h"
#include "anim.h"
#include "assert.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "infantry.h"
#include "inline.h"
#include "noinit.h"
#include "overtype.h"
#include "rules.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "wave.h"
#include "layer.hh"
#include "tube.hh"
#include <new.h>

static const double _deaccel = 0.3f;


inline double ShipLocomotionClass::Get_Slope_Ratio(void) const
{
	return(RampTimer.Progress());
}

inline void ShipLocomotionClass::Set_Slope(int ramp)
{
	int cur = CurrentRamp;
	if (ramp != cur) {
		PreviousRamp = cur;
		CurrentRamp = ramp;
		RampTimer = 3;
	}
}

// FUNCTION: 0x006A0470
bool ShipLocomotionClass::Stop_Driver(void)
{
	if (HeadToCoord != COORD_NONE) {
		HeadToCoord = COORD_NONE;
		IsDriving = false;
		return(true);
	}
	return(false);
}

inline bool ShipLocomotionClass::Abandon_Navigation(void)
{
	if (LinkedTo->RouteQueue.Count() == 0) {
		LinkedTo->Assign_Destination(NULL);
		return(false);
	}

	LinkedTo->AbortMotion();
	return(LinkedTo->Enter_Idle_Mode());
}

// FUNCTION: 0x0069F250
void STDMETHODCALLTYPE ShipLocomotionClass::Force_New_Slope(int ramp)
{
	PreviousRamp = ramp;
	CurrentRamp = ramp;
	RampTimer = 0;
}

// FUNCTION: 0x0069F290
boolean STDMETHODCALLTYPE ShipLocomotionClass::Is_Moving(void)
{
	if (DestinationCoord != COORD_NONE) {
		return(true);
	}
	if (HeadToCoord != COORD_NONE && (HeadToCoord.X != LinkedTo->PositionCoord.X || HeadToCoord.Y != LinkedTo->PositionCoord.Y)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0069F330
boolean STDMETHODCALLTYPE ShipLocomotionClass::Is_Moving_Now(void)
{
	if (LinkedTo->PrimaryFacing.Is_Rotating()) {
		return(true);
	}
	if (Is_Moving() && HeadToCoord != COORD_NONE && LinkedTo->Current_Speed() > 0) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0069F3A0
CoordStruct STDMETHODCALLTYPE ShipLocomotionClass::Destination(void)
{
	return(DestinationCoord);
}

// FUNCTION: 0x0069F3D0
CoordStruct STDMETHODCALLTYPE ShipLocomotionClass::Head_To_Coord(void)
{
	if (HeadToCoord != COORD_NONE) {
		return(HeadToCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: 0x0069F450
void STDMETHODCALLTYPE ShipLocomotionClass::Move_To(CoordStruct to)
{
	if (!LinkedTo->IsUnderEMP() && !LinkedTo->IsParalyzed() && !LinkedTo->IsBeingWarpedOut() && !LinkedTo->IsWarpingIn()) {
		DestinationCoord = to;
		if (Coord(to) != COORD_NONE) {
			if (Map[to].IsUnderBridge) {
				DestinationCoord.Z += BRIDGE_LEPTON_HEIGHT;
			}
		}
	}
}

// FUNCTION: 0x0069F510
void STDMETHODCALLTYPE ShipLocomotionClass::Stop_Moving(void)
{
	if (HeadToCoord != COORD_NONE) {
		if (LinkedTo->Techno_Type_Class()->IsTrain) {
			UnitClass *unit = (UnitClass *)LinkedTo;

			if (!unit->IsFollowing) {
				UnitClass *follower = unit->FollowingMe;
				while (follower != NULL) {
					follower->Locomotion->Stop_Moving();
					follower = follower->FollowingMe;
					if (follower == NULL || follower == follower->FollowingMe) {
						break;
					}
				}
			}
		}
	}

	TargetSpeed = TargetSpeed < _deaccel ? TargetSpeed : _deaccel;
	DestinationCoord = COORD_NONE;
}

// FUNCTION: 0x0069F5E0
BOOL ShipLocomotionClass::Is_Angled(void) const
{
	float percent = Get_Slope_Ratio();
	if (percent == 1.0) {
		if ((float)fabs(LinkedTo->AngleRotatedSideways) < 0.005 && (float)fabs(LinkedTo->AngleRotatedForwards) < 0.005) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x0069FB20
Matrix3DStruct STDMETHODCALLTYPE ShipLocomotionClass::Shadow_Matrix(int *key)
{
	if (!Is_Angled() && key) {
		*key = -1;
	}
	return(BASECLASS::Shadow_Matrix(key));
}

// FUNCTION: 0x0069FBE0
void STDMETHODCALLTYPE ShipLocomotionClass::Unlimbo(void)
{
	Force_New_Slope(LinkedTo->Get_Cell_Ptr()->Ramp);
}

// FUNCTION: 0x0069FC10
boolean STDMETHODCALLTYPE ShipLocomotionClass::Process(void)
{
	Set_Slope(LinkedTo->Get_Cell_Ptr()->Ramp);

	if (TrackNumber != -1 && IsDriving) {
		if (While_Moving()) return(false);
		if (!LinkedTo->IsActive) return(false);
		if (TrackNumber == -1 && (Is_Moving() || LinkedTo->Path[0] != FACING_NONE) && (LinkedTo->Fetch_RTTI() != RTTI_UNIT || !((UnitClass*)LinkedTo)->IsDumping)) {
			bool res = false;
			Start_Of_Move(res);
			if (res) return(false);
			if (!LinkedTo->IsActive) return(false);
			While_Moving(true);
			if (!LinkedTo->IsActive) return(false);
		}
	} else {
		if (LinkedTo->NavCom != NULL && LinkedTo->NavCom->Fetch_RTTI() == RTTI_CELL && LinkedTo->PositionCell == ((CellClass *)LinkedTo->NavCom)->CellID) {
			Abandon_Navigation();
			return(false);
		} else {
			if (LinkedTo->CurrentMission == MISSION_GUARD && !IsDriving && DestinationCoord != COORD_NONE && LinkedTo->PositionCoord == DestinationCoord) {
				Abandon_Navigation();
				return(false);
			}

			if (LinkedTo->PrimaryFacing.Is_Rotating()) {
				IsRotating = true;
			} else {
				if (IsRotating) {
					IsRotating = false;
					LinkedTo->Per_Cell_Process(PCP_ROTATION);
					if (!LinkedTo->IsActive || LinkedTo->IsInLimbo || LinkedTo->IsFalling) return(false);
				}

				if ((LinkedTo->Mission != MISSION_GUARD || Is_Moving()) && LinkedTo->Mission != MISSION_UNLOAD) {
					if (Is_Moving() || LinkedTo->Path[0] != FACING_NONE) {
						if (LinkedTo->IsInPlayfield && LinkedTo->Mission != MISSION_ENTER && Is_Moving() && !LinkedTo->Is_In_Same_Zone(DestinationCoord)) {
							Stop_Driver();
							if (Abandon_Navigation()) {
								return(false);
							}
						} else {
							bool res = false;
							Start_Of_Move(res);
							if (res) return(false);
							if (!LinkedTo->IsActive) return(false);
							While_Moving();
							if (!LinkedTo->IsActive) return(false);
						}
					} else if (LinkedTo->IsSinking) {
						Stop_Driver();
						TargetSpeed = 0;
					} else {
						if (LinkedTo->NavCom != NULL) {
							Move_To(LinkedTo->NavCom->Destination_Coord(LinkedTo));
						}
					}
				}
			}
		}
	}

	if (Is_Moving_Now() && (Frame % 8) == 0) {
		if (!LinkedTo->Techno_Type_Class()->Underwater && !LinkedTo->IsOnBridge && LinkedTo->Get_Cell_Ptr()->Land_Type() == LAND_WATER) {
			if (Rule->Wake != NULL) {
				new AnimClass(Rule->Wake, LinkedTo->PositionCoord, 0, 1, ShapeFlags_Type(0x600), 0, false);
			}
		}
	}

	if (DestinationCoord == COORD_NONE && HeadToCoord == COORD_NONE) {
		if (LinkedTo->Path[0] == FACING_NONE && LinkedTo->Speed > 0) {
			LinkedTo->Set_Speed(0);
		}
	}

	return(Is_Moving());
}

// FUNCTION: 0x006A01A0
void ShipLocomotionClass::Mark_Track(Coord const & headto, MarkType type)
{
	if (headto != COORD_NONE) {
		if (!IsOnShortTrack && TrackNumber != -1) {
			int tracknum = TrackControl[TrackNumber].Track;
			if (tracknum) {
				TrackType const * ptr = RawTracks[tracknum - 1].Track;
				int cellidx = RawTracks[tracknum - 1].Cell;
				if (cellidx > -1) {
					Dir256 dir = ptr[cellidx].Facing;

					if (TrackIndex < cellidx && cellidx != -1) {
						Point2D offset = Smooth_Turn(ptr[cellidx].Offset, dir);
						if (type == MARK_UP) {
							Coord coord(offset.X, offset.Y, LinkedTo->Height);
							LinkedTo->Clear_Occupy_Bit(coord);
						} else {
							if (type == MARK_DOWN || type == MARK_DOWN_FORCED) {
								Coord coord(offset.X, offset.Y, LinkedTo->Height);
								LinkedTo->Set_Occupy_Bit(coord);
							}
						}
					}
				}
			}
		}

		if (type == MARK_UP) {
			LinkedTo->Clear_Occupy_Bit(headto);
		} else {
			if (type == MARK_DOWN || type == MARK_DOWN_FORCED) {
				LinkedTo->Set_Occupy_Bit(headto);
			}
		}
	}
}

// FUNCTION: 0x006A0310
void STDMETHODCALLTYPE ShipLocomotionClass::Force_Track(int track, CoordStruct coord)
{
	TrackNumber = track;
	TrackIndex = 0;
	if ((Coord)coord != COORD_NONE) {
		if (Start_Driver(coord)) {
			DestinationCoord = coord;
			TargetSpeed = 1.0;
		}
	}
}

// FUNCTION: 0x006A04C0
bool ShipLocomotionClass::Start_Driver(Coord const & headto)
{
	Stop_Driver();
	if (headto != COORD_NONE) {
		HeadToCoord = headto;
		IsDriving = true;

		if (Map[headto].Goodie_Check(LinkedTo) && !LinkedTo->IsInLimbo) {
			Mark_Track(headto, MARK_DOWN);
			return(true);
		}
		if (!LinkedTo->IsActive) return(false);

		HeadToCoord = COORD_NONE;
		IsDriving = false;
	}
	return(false);
}

// FUNCTION: 0x006A05C0
void STDMETHODCALLTYPE ShipLocomotionClass::Do_Turn(DirStruct coord)
{
	LinkedTo->PrimaryFacing.Set_Desired(coord);
}

// FUNCTION: 0x006A3DA0
void ShipLocomotionClass::Lay_Track(void)
{
}

// FUNCTION: 0x006A3DB0
Point2D ShipLocomotionClass::Smooth_Turn(Point2D const & adj, Dir256 & dir)
{
	Dir256 workdir = dir;
	int x,y;
	int temp;
	TrackControlType flags = TrackControl[TrackNumber].Flag;

	x = adj.X;
	y = adj.Y;

	if (flags & F_T) {
		temp	= x;
		x		= y;
		y 		= temp;
		workdir = (Dir256)((DIR_W - workdir) & DIR_MAX);
	}

	if (flags & F_X) {
		x 		 = -x;
		workdir = (Dir256)(-workdir & DIR_MAX);
	}

	if (flags & F_Y) {
		y = -y;
		workdir = (Dir256)((DIR_S - workdir) & DIR_MAX);
	}

	dir = workdir;

	return(Point2D(HeadToCoord.X + x, HeadToCoord.Y + y));
}

// FUNCTION: 0x006A3E30
bool ShipLocomotionClass::Incoming(Cell cell)
{
	return(Map.Try_Open_Gate(LinkedTo, cell));
}

// FUNCTION: 0x006A3E50
LayerType STDMETHODCALLTYPE ShipLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x006A3E60
HRESULT STDMETHODCALLTYPE ShipLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ShipLocomotion;
	return(S_OK);
}

// FUNCTION: 0x006A3EA0
int STDMETHODCALLTYPE ShipLocomotionClass::Z_Adjust(void)
{
	return(0);
}

// FUNCTION: 0x006A3EB0
ZGradientType STDMETHODCALLTYPE ShipLocomotionClass::Z_Gradient(void)
{
	return(BASECLASS::Z_Gradient());
}
