// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "drive.h"
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

// FUNCTION: 0x004AF540
DriveLocomotionClass::DriveLocomotionClass(void) :
	BASECLASS(),
	CurrentRamp(0),
	PreviousRamp(0),
	RampTimer(),
	DestinationCoord(COORD_NONE),
	HeadToCoord(COORD_NONE),
	IsOnShortTrack(false),
	IsTurretLockedDown(false),
	IsRotating(false),
	IsDriving(false),
	IsRocking(false),
	IsLocomotorUnlocked(true),
	SpeedAccum(0),
	TargetSpeed(0),
	TrackNumber(-1),
	TrackIndex(-1),
	Piggybacker(NULL)
{
}

// FUNCTION: 0x004AF5E0
DriveLocomotionClass::~DriveLocomotionClass(void)
{
}

// FUNCTION: 0x004AF610
HRESULT DriveLocomotionClass::Piggyback_CLSID(CLSID * classid)
{
	if (classid == NULL) {
		return(E_POINTER);
	}

	if (Piggybacker != NULL) {
		IPersistPtr ptr(Piggybacker);
		if (ptr == NULL) {
			return(E_FAIL);
		}
		return(ptr->GetClassID(classid));
	}

	IPersistPtr ptr(this);
	if (ptr == NULL) {
		return(E_FAIL);
	}
	return(ptr->GetClassID(classid));
}

// FUNCTION: 0x004AF720
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	HRESULT result = BASECLASS::QueryInterface(riid, ppvObject);

	if (result == E_NOINTERFACE) {
		if (riid == IID_IPiggyback) {
			*ppvObject = (IPiggyback*)this;
		}
		if (*ppvObject == NULL) {
			result = E_NOINTERFACE;
		} else {
			AddRef();
			result = S_OK;
		}
	}
	return(result);
}

// FUNCTION: 0x004AF780
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) DriveLocomotionClass(NoInitClass());

		bool haspiggy;
		stream->Read(&haspiggy, sizeof(haspiggy), NULL);

		if (haspiggy) {
			result = OleLoadFromStream(stream, IID_ILocomotion, (void**)&Piggybacker);
		}
	}
	return(result);
}

// FUNCTION: 0x004AF800
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		bool haspiggy;
		if (Piggybacker != NULL) {
			haspiggy = true;
			stream->Write(&haspiggy, sizeof(haspiggy), NULL);
			IPersistStreamPtr ptr(Piggybacker);
			result = OleSaveToStream(ptr, stream);
		} else {
			haspiggy = false;
			stream->Write(&haspiggy, sizeof(haspiggy), NULL);
		}
	}
	return(result);
}

// FUNCTION: 0x004AF8E0
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::Begin_Piggyback(ILocomotion *pointer)
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

// FUNCTION: 0x004AF930
HRESULT DriveLocomotionClass::End_Piggyback(ILocomotion **pointer)
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

// FUNCTION: 0x004AF970
boolean DriveLocomotionClass::Is_Ok_To_End(void)
{
	if (!Is_Moving() && (Piggybacker != NULL && IsLocomotorUnlocked && !LinkedTo->IsAttackedByLocomotor)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004AF9B0
Matrix3D DriveLocomotionClass::Get_Slope_Matrix(void) const
{
	Matrix3D mtx(true);

	if (!RampTimer.Has_Completed2()) {
		mtx = ::Get_Slope_Transition_Matrix(PreviousRamp, CurrentRamp, Get_Slope_Ratio());
	} else if (CurrentRamp) {
		mtx = ::Get_Slope_Matrix(CurrentRamp);
	}

	return(mtx);
}

// FUNCTION: 0x004AFAA0
double DriveLocomotionClass::Get_Slope_Ratio(void) const
{
	return(RampTimer.Progress());
}

// FUNCTION: 0x004AFB00
void DriveLocomotionClass::Set_Slope(int ramp)
{
	int cur = CurrentRamp;
	if (ramp != cur) {
		PreviousRamp = cur;
		CurrentRamp = ramp;
		RampTimer = 3;
	}
}

// FUNCTION: 0x004AFB40
void STDMETHODCALLTYPE DriveLocomotionClass::Force_New_Slope(int ramp)
{
	PreviousRamp = ramp;
	CurrentRamp = ramp;
	RampTimer = 0;
}

// FUNCTION: 0x004AFB80
boolean STDMETHODCALLTYPE DriveLocomotionClass::Is_Moving(void)
{
	if (DestinationCoord != COORD_NONE) {
		return(true);
	}
	if (HeadToCoord != COORD_NONE && (HeadToCoord.X != LinkedTo->PositionCoord.X || HeadToCoord.Y != LinkedTo->PositionCoord.Y)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004AFC20
boolean STDMETHODCALLTYPE DriveLocomotionClass::Is_Moving_Now(void)
{
	if (LinkedTo->PrimaryFacing.Is_Rotating()) {
		return(true);
	}
	if (Is_Moving() && HeadToCoord != COORD_NONE && LinkedTo->Current_Speed() > 0) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004AFC90
CoordStruct STDMETHODCALLTYPE DriveLocomotionClass::Destination(void)
{
	return(DestinationCoord);
}

// FUNCTION: 0x004AFCC0
CoordStruct STDMETHODCALLTYPE DriveLocomotionClass::Head_To_Coord(void)
{
	if (HeadToCoord != COORD_NONE) {
		return(HeadToCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: 0x004AFD40
void STDMETHODCALLTYPE DriveLocomotionClass::Move_To(CoordStruct to)
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

// FUNCTION: 0x004AFE00
void STDMETHODCALLTYPE DriveLocomotionClass::Stop_Moving(void)
{
	if (HeadToCoord != COORD_NONE) {
		if (LinkedTo->TClass->IsTrain) {
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

// FUNCTION: 0x004AFED0
BOOL DriveLocomotionClass::Is_Angled(void) const
{
	float percent = Get_Slope_Ratio();
	if (percent == 1.0) {
		if ((float)fabs(LinkedTo->AngleRotatedSideways) < 0.005 && (float)fabs(LinkedTo->AngleRotatedForwards) < 0.005) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x004B0410
Matrix3DStruct STDMETHODCALLTYPE DriveLocomotionClass::Shadow_Matrix(int *key)
{
	if (!Is_Angled()) {
		if (key) {
			*key = -1;
		}
	}
	return(BASECLASS::Shadow_Matrix(key));
}

// FUNCTION: 0x004B04D0
void STDMETHODCALLTYPE DriveLocomotionClass::Unlimbo(void)
{
	Force_New_Slope(LinkedTo->Get_Cell_Ptr()->Ramp);
}

// FUNCTION: 0x004B0DA0
bool DriveLocomotionClass::Stop_Driver(void)
{
	if (HeadToCoord != COORD_NONE) {
		HeadToCoord = COORD_NONE;
		IsDriving = false;
		return(true);
	}
	return(false);
}


// FUNCTION: 0x004B4890
bool DriveLocomotionClass::Abandon_Navigation(void)
{
	if (LinkedTo->RouteQueue.Count() == 0) {
		LinkedTo->Assign_Destination(NULL);
		return(false);
	}

	LinkedTo->AbortMotion();
	return(LinkedTo->Enter_Idle_Mode());
}


// FUNCTION: 0x004B0500
boolean STDMETHODCALLTYPE DriveLocomotionClass::Process(void)
{
	Set_Slope(LinkedTo->Get_Cell_Ptr()->Ramp);

	if (TrackNumber != -1 && IsDriving) {
		if (While_Moving()) return(false);
		if (!LinkedTo->IsActive) return(false);
		if (TrackNumber == -1 && (Is_Moving() || LinkedTo->Path[0] != FACING_NONE) && (LinkedTo->Fetch_RTTI() != RTTI_UNIT || !((UnitClass*)LinkedTo)->IsDumping)) {
			bool res = false;
			if (LinkedTo->Fetch_RTTI() == RTTI_UNIT && LinkedTo->NavCom != NULL && LinkedTo->NavCom->Fetch_RTTI() == RTTI_INFANTRY) {
				Coord dest = LinkedTo->NavCom->Destination_Coord(LinkedTo);
				if (dest != DestinationCoord) {
					Move_To(dest);
				}
			}
			Start_Of_Move(res);
			if (res) return(false);
			if (!LinkedTo->IsActive) return(false);
			While_Moving(true);
			if (LinkedTo == NULL || !LinkedTo->IsActive) return(false);
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
							if (LinkedTo == NULL || !LinkedTo->IsActive) return(false);
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

	if (Is_Moving_Now() && (Frame % 10) == 0) {
		if (!LinkedTo->IsOnBridge && LinkedTo->Get_Cell_Ptr()->Land_Type() == LAND_WATER) {
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


// FUNCTION: 0x004B0AD0
void DriveLocomotionClass::Mark_Track(Coord const & headto, MarkType type)
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


// FUNCTION: 0x004B0C40
void STDMETHODCALLTYPE DriveLocomotionClass::Force_Track(int track, CoordStruct coord)
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


// FUNCTION: 0x004B0DF0
bool DriveLocomotionClass::Start_Driver(Coord const & headto)
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


// FUNCTION: 0x004B0EF0
void STDMETHODCALLTYPE DriveLocomotionClass::Do_Turn(DirStruct coord)
{
	DirType dir = (DirType &)coord;
	LinkedTo->PrimaryFacing.Set_Desired(dir);
}


// FUNCTION: 0x004B4820
LayerType STDMETHODCALLTYPE DriveLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x004B4830
HRESULT STDMETHODCALLTYPE DriveLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_DriveLocomotion;
	return(S_OK);
}

// FUNCTION: 0x004B4770
void DriveLocomotionClass::Lay_Track(void)
{
}


// FUNCTION: 0x004B4780
Point2D DriveLocomotionClass::Smooth_Turn(Point2D const & adj, Dir256 & dir)
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


// FUNCTION: 0x004B4800
bool DriveLocomotionClass::Incoming(Cell cell)
{
	return(Map.Try_Open_Gate(LinkedTo, cell));
}


// FUNCTION: 0x004B4880
ZGradientType STDMETHODCALLTYPE DriveLocomotionClass::Z_Gradient(void)
{
	return(BASECLASS::Z_Gradient());
}

// FUNCTION: 0x004B4870
int STDMETHODCALLTYPE DriveLocomotionClass::Z_Adjust(void)
{
	return(0);
}


// FUNCTION: 0x004B48D0
void STDMETHODCALLTYPE DriveLocomotionClass::Mark_All_Occupation_Bits(int mark)
{
	if (HeadToCoord != COORD_NONE) {
		Mark_Track(HeadToCoord, (MarkType)mark);
	}
}


// FUNCTION: 0x004B4920
boolean STDMETHODCALLTYPE DriveLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	Coord coord = Head_To_Coord();

	if (coord != COORD_NONE) {
		if (!IsOnShortTrack && TrackNumber != -1) {
			int tracknum = TrackControl[TrackNumber].Track;
			if (tracknum) {
				TrackType const * ptr = RawTracks[tracknum - 1].Track;
				int cellidx = RawTracks[tracknum - 1].Cell;
				if (cellidx > -1) {
					Dir256 dir = ptr[cellidx].Facing;

					if (TrackIndex < cellidx && cellidx != -1) {
						Point2D pt = Smooth_Turn(ptr[cellidx].Offset, dir);
						Coord coord = LinkedTo->PositionCoord;
						coord.X = pt.X;
						coord.Y = pt.Y;
						if (coord.As_Cell() == ((Coord &)to).As_Cell() && abs(coord.Z - to.Z) <= LEVEL_LEPTON_H) {
							return(true);
						}
					}
				}
			}
		}

		if (coord.As_Cell() == ((Coord &)to).As_Cell() && abs(coord.Z - to.Z) <= LEVEL_LEPTON_H) {
			return(true);
		}
	}

	return(false);
}


// FUNCTION: 0x004B4B00
boolean STDMETHODCALLTYPE DriveLocomotionClass::Will_Jump_Tracks(void)
{
	TurnTrackType	const * track;
	int				tracknum;
	FacingType		nextface;
	bool				adj;

	nextface = LinkedTo->Path[0];

	if (nextface >= FACING_NONE && nextface <= FACING_COUNT) {

		track = &TrackControl[TrackNumber];
		if (IsOnShortTrack) {
			tracknum = track->StartTrack;
		} else {
			tracknum = track->Track;
		}

		adj = false;
		if (nextface != FACING_COUNT && nextface != FACING_NONE && Dir_Facing(track->Facing) != nextface) {
			adj = true;
		}

		if (nextface != FACING_COUNT && nextface != FACING_NONE && adj && RawTracks[tracknum-1].Jump == TrackIndex && TrackIndex) {
			TurnTrackType const * newtrack;
			int	tnum;

			tnum = (int)(Dir_Facing(track->Facing) * FACING_COUNT) + (int)nextface;
			newtrack = &TrackControl[tnum];
			if (newtrack->Track && RawTracks[newtrack->Track-1].Entry) {
				return(true);
			}
		}
	}
	return(false);
}


// FUNCTION: 0x004B4BE0
void STDMETHODCALLTYPE DriveLocomotionClass::Lock(void)
{
	IsLocomotorUnlocked = false;
}

// FUNCTION: 0x004B4BF0
void STDMETHODCALLTYPE DriveLocomotionClass::Unlock(void)
{
	IsLocomotorUnlocked = true;
}

// FUNCTION: 0x004B4C00
int STDMETHODCALLTYPE DriveLocomotionClass::Get_Track_Number(void)
{
	return(TrackNumber);
}

// FUNCTION: 0x004B4C10
int STDMETHODCALLTYPE DriveLocomotionClass::Get_Track_Index(void)
{
	return(TrackIndex);
}

// FUNCTION: 0x004B4C20
int STDMETHODCALLTYPE DriveLocomotionClass::Get_Speed_Accum(void)
{
	return(SpeedAccum);
}

// FUNCTION: 0x004B4C30 LocomotionClass::IsDirty

// FUNCTION: 0x004B4CB0
ULONG STDMETHODCALLTYPE DriveLocomotionClass::AddRef(void)
{
	return(BASECLASS::AddRef());
}

// FUNCTION: 0x004B4CC0
ULONG STDMETHODCALLTYPE DriveLocomotionClass::Release(void)
{
	return(BASECLASS::Release());
}

// FUNCTION: 0x004B4CF0 DriveLocomotionClass::Fetch_Object_Size
