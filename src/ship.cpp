// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
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

// SYNTHETIC: GAMEMD 0x0069EA40 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0069EA70 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0069EA90 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0069EAB0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0069EAD0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0069EAF0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0069EB10 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0069EB50 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0069EB80 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0069EBB0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0069EBE0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0069EBF0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0069EC10 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0069EC30 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x0069EC50
ShipLocomotionClass::ShipLocomotionClass(void) :
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

// FUNCTION: GAMEMD 0x0069ECF0
ShipLocomotionClass::~ShipLocomotionClass(void)
{
}

// FUNCTION: GAMEMD 0x0069ED20
HRESULT ShipLocomotionClass::Piggyback_CLSID(CLSID * classid)
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

// FUNCTION: GAMEMD 0x0069EE30
HRESULT STDMETHODCALLTYPE ShipLocomotionClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
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

// FUNCTION: GAMEMD 0x0069EE90
HRESULT STDMETHODCALLTYPE ShipLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) ShipLocomotionClass(NoInitClass());

		bool haspiggy;
		stream->Read(&haspiggy, sizeof(haspiggy), NULL);

		if (haspiggy) {
			result = OleLoadFromStream(stream, IID_ILocomotion, (void**)&Piggybacker);
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0069EF10
HRESULT STDMETHODCALLTYPE ShipLocomotionClass::Save(IStream * stream, BOOL cleardirty)
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

// FUNCTION: GAMEMD 0x0069EFF0
HRESULT STDMETHODCALLTYPE ShipLocomotionClass::Begin_Piggyback(ILocomotion *pointer)
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

// FUNCTION: GAMEMD 0x0069F040
HRESULT ShipLocomotionClass::End_Piggyback(ILocomotion **pointer)
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

// FUNCTION: GAMEMD 0x0069F080
boolean ShipLocomotionClass::Is_Ok_To_End(void)
{
	if (!Is_Moving() && (Piggybacker != NULL && IsLocomotorUnlocked && !LinkedTo->IsAttackedByLocomotor)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0069F0C0
Matrix3D ShipLocomotionClass::Get_Slope_Matrix(void) const
{
	Matrix3D mtx(true);

	if (!RampTimer.Has_Completed2()) {
		mtx = ::Get_Slope_Transition_Matrix(PreviousRamp, CurrentRamp, Get_Slope_Ratio());
	} else if (CurrentRamp) {
		mtx = ::Get_Slope_Matrix(CurrentRamp);
	}

	return(mtx);
}

// FUNCTION: GAMEMD 0x0069F1B0
double ShipLocomotionClass::Get_Slope_Ratio(void) const
{
	return(RampTimer.Progress());
}

// FUNCTION: GAMEMD 0x0069F210
void ShipLocomotionClass::Set_Slope(int ramp)
{
	int cur = CurrentRamp;
	if (ramp != cur) {
		PreviousRamp = cur;
		CurrentRamp = ramp;
		RampTimer = 3;
	}
}

static const double _deaccel = 0.3f;
static const double _sinking = 0.1f;
static const double _sinking_scale = 0.0015f;

// FUNCTION: GAMEMD 0x0069F250
void STDMETHODCALLTYPE ShipLocomotionClass::Force_New_Slope(int ramp)
{
	PreviousRamp = ramp;
	CurrentRamp = ramp;
	RampTimer = 0;
}

// FUNCTION: GAMEMD 0x0069F290
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

// FUNCTION: GAMEMD 0x0069F330
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

// FUNCTION: GAMEMD 0x0069F3A0
CoordStruct STDMETHODCALLTYPE ShipLocomotionClass::Destination(void)
{
	return(DestinationCoord);
}

// FUNCTION: GAMEMD 0x0069F3D0
CoordStruct STDMETHODCALLTYPE ShipLocomotionClass::Head_To_Coord(void)
{
	if (HeadToCoord != COORD_NONE) {
		return(HeadToCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: GAMEMD 0x0069F450
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

// FUNCTION: GAMEMD 0x0069F510
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

// FUNCTION: GAMEMD 0x0069F5E0
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

// FUNCTION: GAMEMD 0x0069FB20
Matrix3DStruct STDMETHODCALLTYPE ShipLocomotionClass::Shadow_Matrix(int *key)
{
	if (!Is_Angled() && key) {
		*key = -1;
	}
	return(BASECLASS::Shadow_Matrix(key));
}

// FUNCTION: GAMEMD 0x0069FBE0
void STDMETHODCALLTYPE ShipLocomotionClass::Unlimbo(void)
{
	Force_New_Slope(LinkedTo->Get_Cell_Ptr()->Ramp);
}

// FUNCTION: GAMEMD 0x0069FC10
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

// FUNCTION: GAMEMD 0x006A01A0
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

// FUNCTION: GAMEMD 0x006A0310
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

// FUNCTION: GAMEMD 0x006A0470
bool ShipLocomotionClass::Stop_Driver(void)
{
	if (HeadToCoord != COORD_NONE) {
		HeadToCoord = COORD_NONE;
		IsDriving = false;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006A04C0
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

// FUNCTION: GAMEMD 0x006A05C0
void STDMETHODCALLTYPE ShipLocomotionClass::Do_Turn(DirStruct coord)
{
	LinkedTo->PrimaryFacing.Set_Desired(coord);
}

// FUNCTION: GAMEMD 0x006A05F0
bool ShipLocomotionClass::While_Moving(bool just_started)
{
	if ((!IsDriving || TrackNumber == -1) && LinkedTo->Path[0] != FACING_COUNT ||
		(IsRotating && !LinkedTo->Techno_Type_Class()->IsTurretEquipped)) {
		SpeedAccum = 0;
		return false;
	}
	if (LinkedTo->Techno_Type_Class()->IsAccelerates) {
		bool not_passive = LinkedTo->Fetch_RTTI() != RTTI_UNIT || !((UnitClass *)LinkedTo)->Class->Passive;
		if (TrackNumber < FACING_COUNT * FACING_COUNT && not_passive) {
			Coord dest_coord = DestinationCoord;
			dest_coord.Z =
				Map.Get_Height_GL(dest_coord) + (Map[dest_coord].IsUnderBridge ? BRIDGE_LEPTON_HEIGHT : 0);
			int distance = (LinkedTo->Get_Coord() - dest_coord).Length();

			bool forced_speed = false;
			double speed = LinkedTo->Speed;

			TechnoTypeClass const *tclass = LinkedTo->Techno_Type_Class();

			if (distance < tclass->SlowdownDistance) {
				forced_speed = true;
				speed = MAX(_deaccel, speed - tclass->MaxSpeed * tclass->DeaccelerationFactor);
			} else if (LinkedTo->IsSinking) {
				speed = MAX(_sinking, speed - tclass->MaxSpeed * _sinking_scale);
				forced_speed = true;
			}

			if (LinkedTo->IsCrushing) {
				TargetSpeed = MIN(TargetSpeed, 0.2);
				LinkedTo->Set_Speed(TargetSpeed);
			} else if (forced_speed) {
				LinkedTo->Set_Speed(speed);
			} else if (LinkedTo->Speed < TargetSpeed) {
				LinkedTo->Set_Speed(MIN(TargetSpeed, LinkedTo->Speed + tclass->AccelerationFactor));
			} else if (LinkedTo->Speed > TargetSpeed) {
				LinkedTo->Set_Speed(
					MAX(TargetSpeed, LinkedTo->Speed - tclass->MaxSpeed * tclass->DeaccelerationFactor));
			}

			if (LinkedTo->Fetch_RTTI() == RTTI_UNIT) {
				UnitClass *unit = (UnitClass *)LinkedTo;
				UnitClass *following = (UnitClass *)unit->FollowingMe;
				if (following != 0) {
					do {
						following->Set_Speed(LinkedTo->Speed);
						following = (UnitClass *)following->FollowingMe;
					} while (following && following != following->FollowingMe);
				}
			}
		}
	} else {
		LinkedTo->Set_Speed(TargetSpeed);
	}

	int maxspeed = LinkedTo->Current_Speed();

	int actual;
	actual = SpeedAccum + (just_started ? 0 : maxspeed);

	FacingType nextface;
	nextface = LinkedTo->Path[0];

	if (nextface == FACING_COUNT && TrackNumber == -1) {
		LinkedTo->Mark(MARK_UP);
		Stop_Driver();

		CellClass *cell = LinkedTo->Get_Cell_Ptr();
		int tubenum = cell->Tube;
		if (tubenum >= 0 && tubenum < Tubes.Count()) {
			TubeClass *tube = Tubes[tubenum];
			Cell c = tube->Exit;
			HeadToCoord = c.As_Coord();
			memcpy(&LinkedTo->Path[0], &LinkedTo->Path[1], sizeof(LinkedTo->Path[0]) * (CONQUER_PATH_MAX - 1));
			LinkedTo->Path[CONQUER_PATH_MAX - 1] = FACING_NONE;
			LinkedTo->CurrentTube = tubenum;

			LinkedTo->CurrentTubeDir = 0;
			Cell entry = tube->Enter;
			LinkedTo->LastTubeCoord = Map[Adjacent_Cell(entry, tube->Dirs[0])].Cell_Coord();
			int current_height = Map.Get_Height_GL(LinkedTo->PositionCoord);
			Cell exit = tube->Exit;
			int count = tube->Count;
			LinkedTo->LastTubeCoord.Z = current_height + (Map.Get_Height_GL(exit) - current_height) / count;
			IsDriving = true;
			TrackNumber = -1;
		} else {
			LinkedTo->Path[0] = FACING_NONE;
			TrackNumber = -1;
			HeadToCoord = COORD_NONE;
			Stop_Driver();
		}
		return false;
	}

	if (actual > 7) {
		TurnTrackType const *track;
		TrackType const *ptr;
		int tracknum;
		bool adj;

		track = &TrackControl[TrackNumber];
		if (IsOnShortTrack) {
			tracknum = track->StartTrack;
		} else {
			tracknum = track->Track;
		}
		ptr = RawTracks[tracknum - 1].Track;

		if (nextface < -1 || nextface > 8) {
			LinkedTo->Path[0] = FACING_NONE;
			return false;
		}

		adj = false;
		if (nextface != FACING_COUNT && nextface != FACING_NONE && track->Exit_Facing() != nextface) {
			adj = true;
		}

		while (actual > 7) {
			Point2D offset;
			Dir256 dir;

			actual -= 7;

			offset = ptr[TrackIndex].Offset;
			if (offset != Point2D(0, 0) || !TrackIndex) {
				if (LinkedTo->Occupies_Cells()) {
					LinkedTo->Clear_Occupy_Bit(LinkedTo->PositionCoord);
					LinkedTo->IsOccupyingCell = false;
					LinkedTo->IsToPathAroundBlockage = false;
				}

				Cell oldcell;
				if (!TrackIndex) {
					oldcell = LinkedTo->Get_Cell();
				} else {
					dir = ptr[TrackIndex - 1].Facing;
					Point2D prevoffset = ptr[TrackIndex - 1].Offset;
					oldcell = Coord(Smooth_Turn(prevoffset, dir), 0).As_Cell();
				}

				dir = ptr[TrackIndex].Facing;
				Coord newcoord = Coord(Smooth_Turn(offset, dir), 0);
				Cell step_cell = newcoord.As_Cell();
				Cell current_cell = LinkedTo->PositionCoord.As_Cell();
				Cell newcell = newcoord.As_Cell();

				if (step_cell != current_cell) {
					LinkedTo->Mark(MARK_UP);
					LinkedTo->Set_Coord(newcoord);
					CellClass *oldcellptr = &Map[oldcell];
					CellClass *newcellptr = &Map[newcell];
					if (newcellptr->Height == oldcellptr->Height - BRIDGE_CELL_HEIGHT &&
						newcellptr->IsUnderBridge) {
						LinkedTo->IsOnBridge = true;
					}
					if (!newcellptr->IsUnderBridge && oldcellptr->IsUnderBridge) {
						LinkedTo->IsOnBridge = false;
					}
					if (LinkedTo->Techno_Type_Class()->IsTrain && !((UnitClass *)LinkedTo)->IsFollowing) {
						FootClass *train = LinkedTo;
						bool is_bridge = train->IsOnBridge ||
										 (LinkedTo->PositionCoord.Z >=
										  (Map.Get_Height_GL(LinkedTo->PositionCoord) + BRIDGE_LEPTON_HEIGHT));
						ObjectClass *occupier =
							is_bridge ? Map[newcell].Cell_Bridge_Occupier() : Map[newcell].Cell_Occupier();

						while (occupier != 0) {
							ObjectClass *next = occupier->Next;
							if (!occupier->IsCrushable(LinkedTo)) {
								int strength = 10000;
								occupier->Take_Damage(strength, 0, Rule->C4Warhead, 0, true, true, 0);
								strength = 20;
								train->Take_Damage(strength, 0, Rule->C4Warhead, 0, true, false, 0);
							}
							occupier = next;
						}
					}
					if (!LinkedTo->IsActive)
						return false;
					LinkedTo->Mark(MARK_DOWN);

					if (newcellptr->Overlay != OVERLAY_NONE) {
						OverlayTypeClass *otype = OverlayTypes[newcellptr->Overlay];
						if (IsRocking &&
							(LinkedTo->Techno_Type_Class()->IsCrusher || LinkedTo->Has_Ability(ABILITY_CRUSHER)) &&
							otype->HeapID == OVERLAY_SANDBAG_WALL) {
							LinkedTo->IsCrushing = true;

							if (LinkedTo->Techno_Type_Class()->IsTiltsWhenCrushes) {
								LinkedTo->RockingForwardsPerFrame = -0.02f;
							}
						}
					}
				} else {
					bool down = LinkedTo->IsDown;
					LinkedTo->IsDown = false;
					LinkedTo->Set_Coord(newcoord);
					LinkedTo->IsDown = down;
				}

				if (!LinkedTo->IsActive) {
					return false;
				}

				bool down = LinkedTo->IsDown;
				LinkedTo->IsDown = false;
				LinkedTo->Set_Height_AGL(0);
				LinkedTo->IsDown = down;

				LinkedTo->PrimaryFacing.Set(dir);

				if (TrackIndex && RawTracks[tracknum - 1].Cell == TrackIndex) {
					LinkedTo->Clear_Occupy_Bit(LinkedTo->PositionCoord);
				}

				if (nextface != FACING_COUNT && nextface != FACING_NONE && adj &&
					RawTracks[tracknum - 1].Jump == TrackIndex && TrackIndex) {
					TurnTrackType const *newtrack;
					int tnum;

					tnum = track->Exit_Facing() * FACING_COUNT + nextface;
					newtrack = &TrackControl[tnum];
					if (newtrack->Track && RawTracks[newtrack->Track - 1].Entry) {
						double oldspeed = LinkedTo->Speed;

						Coord c = Adjacent_Cell(HeadToCoord, nextface);

						switch (LinkedTo->Can_Enter_Cell(&Map[c.As_Cell()], nextface, LinkedTo->Get_Cell_Height(),
														 0, true)) {
						case MOVE_OK:
						case MOVE_MOVING_BLOCK:
							if (LinkedTo->Fetch_RTTI() == RTTI_UNIT && !((UnitClass *)LinkedTo)->Class->Passive)
								break;
							IsOnShortTrack = false;
							TrackNumber = tnum;
							track = newtrack;

							tracknum = track->Track;
							TrackIndex = RawTracks[tracknum - 1].Entry - 1;
							ptr = RawTracks[tracknum - 1].Track;
							adj = false;

							Stop_Driver();
							IsDriving = true;
							LinkedTo->Per_Cell_Process(PCP_END);
							IsDriving = false;
							if (!LinkedTo->IsActive || LinkedTo->IsInLimbo || LinkedTo->IsFalling)
								return false;
							if (Start_Driver(c)) {
								LinkedTo->Set_Speed(oldspeed);
								memcpy(&LinkedTo->Path[0], &LinkedTo->Path[1],
									   sizeof(LinkedTo->Path[0]) * (CONQUER_PATH_MAX - 1));
								LinkedTo->Path[CONQUER_PATH_MAX - 1] = FACING_NONE;
							}
							break;

						case MOVE_CLOAK:
							Map[c].Shimmer();
							break;

						case MOVE_CLOSED_GATE: {
							Cell gate = c.As_Cell();
							Map.Try_Open_Gate(LinkedTo, gate);
						} break;

						case MOVE_TEMP: {
							bool bridge;
							if (!Map[c].IsUnderBridge ||
								abs(LinkedTo->PositionCoord.Z / LEVEL_LEPTON_H - Map[c].Height) <= 2) {
								bridge = false;
							} else {
								bridge = true;
							}
							Map[HeadToCoord].Incoming(COORD_NONE, true, true, bridge);
						} break;
						}
					}
				}
				TrackIndex++;

			} else {
				Coord dc = HeadToCoord - LinkedTo->PositionCoord;
				int d = abs(dc.X) + abs(dc.Y);
				actual += (int)((1.0 - (double)d / 11.0) * 7.0);
				LinkedTo->IsOccupyingCell = true;
				LinkedTo->IsToPathAroundBlockage = false;

				Cell here = LinkedTo->PositionCoord.As_Cell();
				if (here != HeadToCoord.As_Cell()) {
					LinkedTo->Mark(MARK_UP);
					LinkedTo->Set_Coord(HeadToCoord);
					LinkedTo->Set_Height_AGL(0);
					LinkedTo->Mark(MARK_DOWN);
				} else {
					bool down = LinkedTo->IsDown;
					LinkedTo->IsDown = false;
					LinkedTo->Set_Coord(HeadToCoord);
					LinkedTo->Set_Height_AGL(0);
					LinkedTo->IsDown = down;
				}

				Stop_Driver();
				TrackNumber = -1;
				TrackIndex = 0;

				bool arrived = false;
				AbstractClass *target = LinkedTo->NavCom;
				if (target != 0 && LinkedTo->Get_Cell() == target->Destination_Coord(LinkedTo).As_Cell()) {
					if (abs(LinkedTo->Destination_Coord(LinkedTo).Z - DestinationCoord.Z) < 2 * LEVEL_LEPTON_H) {
						arrived = true;
						DestinationCoord = COORD_NONE;
						Stop_Driver();
						IsDriving = false;
					}
				}

				LinkedTo->Per_Cell_Process(PCP_END);
				if (!LinkedTo->IsActive || LinkedTo->IsInLimbo || LinkedTo->IsFalling)
					return true;

				if (arrived) {
					LinkedTo->AbortMotion();
					LinkedTo->Path[0] = FACING_NONE;
					if (LinkedTo->Get_Mission() == MISSION_MOVE && LinkedTo->Enter_Idle_Mode(false, true)) {
						return true;
					}
				}

				if (LinkedTo->Is_Docked_For_Repair()) {
					return true;
				}

				if (!LinkedTo->IsActive)
					return false;

				break;
			}
		}
	}

	SpeedAccum = actual;

	if (actual > 0 && TrackNumber > -1) {
		TurnTrackType const *track;
		TrackType const *ptr;
		int tracknum;

		track = &TrackControl[TrackNumber];
		if (IsOnShortTrack) {
			tracknum = track->StartTrack;
		} else {
			tracknum = track->Track;
		}
		ptr = RawTracks[tracknum - 1].Track;

		Dir256 dir;
		Point2D offset = ptr[TrackIndex].Offset;
		if (offset != Point2D(0, 0) || !TrackIndex) {

			Coord oldcoord = LinkedTo->PositionCoord;

			dir = ptr[TrackIndex].Facing;
			Coord movement(Smooth_Turn(offset, dir) - LinkedTo->PositionCoord, 0);
			Coord stepcoord = movement + LinkedTo->PositionCoord;
			CellClass *stepcellptr = &Map[stepcoord];

			Coord partial(0, 0, 0);
			movement = Lerp(partial, movement, SpeedAccum / 7.0);
			Coord partialcoord = oldcoord + movement;

			CellClass *oldcellptr = &Map[oldcoord];
			CellClass *partialcellptr = &Map[partialcoord];

			Coord coord = LinkedTo->PositionCoord;

			if (partialcellptr != stepcellptr && partialcellptr != oldcellptr) {
				if (SpeedAccum > 3) {
					coord = stepcoord;
				}
			} else {
				coord = partialcoord;
			}

			CellClass const &newcell = Map[Coord(coord)];

			if (coord.As_Cell() != LinkedTo->PositionCoord.As_Cell()) {
				LinkedTo->Mark(MARK_UP);
				LinkedTo->Set_Coord(coord);
				if (newcell.Height == oldcellptr->Height - BRIDGE_CELL_HEIGHT && newcell.IsUnderBridge) {
					LinkedTo->IsOnBridge = true;
				}
				if (!newcell.IsUnderBridge && oldcellptr->IsUnderBridge) {
					LinkedTo->IsOnBridge = false;
				}
				LinkedTo->Mark(MARK_DOWN);
			} else {
				bool down = LinkedTo->IsDown;
				LinkedTo->IsDown = false;
				LinkedTo->Set_Coord(coord);
				LinkedTo->IsDown = down;
			}
		}
	}

	return false;
}

// FUNCTION: GAMEMD 0x006A3DA0
void ShipLocomotionClass::Lay_Track(void)
{
}

// FUNCTION: GAMEMD 0x006A3DB0
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

// FUNCTION: GAMEMD 0x006A3E30
bool ShipLocomotionClass::Incoming(Cell cell)
{
	return(Map.Try_Open_Gate(LinkedTo, cell));
}

// FUNCTION: GAMEMD 0x006A3E50
LayerType STDMETHODCALLTYPE ShipLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: GAMEMD 0x006A3E60
HRESULT STDMETHODCALLTYPE ShipLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_ShipLocomotion;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006A3EA0
int STDMETHODCALLTYPE ShipLocomotionClass::Z_Adjust(void)
{
	return(0);
}

// FUNCTION: GAMEMD 0x006A3EB0
ZGradientType STDMETHODCALLTYPE ShipLocomotionClass::Z_Gradient(void)
{
	return(BASECLASS::Z_Gradient());
}

// FUNCTION: GAMEMD 0x006A3EC0
bool ShipLocomotionClass::Abandon_Navigation(void)
{
	if (LinkedTo->RouteQueue.Count() == 0) {
		LinkedTo->Assign_Destination(NULL);
		return(false);
	}

	LinkedTo->AbortMotion();
	return(LinkedTo->Enter_Idle_Mode());
}

// FUNCTION: GAMEMD 0x006A3F00
void STDMETHODCALLTYPE ShipLocomotionClass::Mark_All_Occupation_Bits(int mark)
{
	if (HeadToCoord != COORD_NONE) {
		Mark_Track(HeadToCoord, (MarkType)mark);
	}
}

// FUNCTION: GAMEMD 0x006A3F50
boolean STDMETHODCALLTYPE ShipLocomotionClass::Is_Moving_Here(CoordStruct to)
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
						Coord there = LinkedTo->PositionCoord;
						there.X = pt.X;
						there.Y = pt.Y;
						if (there.As_Cell() == ((Coord &)to).As_Cell() && abs(there.Z - to.Z) <= LEVEL_LEPTON_H) {
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

// FUNCTION: GAMEMD 0x006A4130
boolean STDMETHODCALLTYPE ShipLocomotionClass::Will_Jump_Tracks(void)
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
		if (nextface != FACING_COUNT && nextface != FACING_NONE && track->Exit_Facing() != nextface) {
			adj = true;
		}

		if (nextface != FACING_COUNT && nextface != FACING_NONE && adj && RawTracks[tracknum-1].Jump == TrackIndex && TrackIndex) {
			TurnTrackType const * newtrack;
			int	tnum;

			tnum = (int)(track->Exit_Facing() * FACING_COUNT) + (int)nextface;
			newtrack = &TrackControl[tnum];
			if (newtrack->Track && RawTracks[newtrack->Track-1].Entry) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x006A4210
void STDMETHODCALLTYPE ShipLocomotionClass::Lock(void)
{
	IsLocomotorUnlocked = false;
}

// FUNCTION: GAMEMD 0x006A4220
void STDMETHODCALLTYPE ShipLocomotionClass::Unlock(void)
{
	IsLocomotorUnlocked = true;
}

// FUNCTION: GAMEMD 0x006A4230
int STDMETHODCALLTYPE ShipLocomotionClass::Get_Track_Number(void)
{
	return(TrackNumber);
}

// FUNCTION: GAMEMD 0x006A4240
int STDMETHODCALLTYPE ShipLocomotionClass::Get_Track_Index(void)
{
	return(TrackIndex);
}

// FUNCTION: GAMEMD 0x006A4250
int STDMETHODCALLTYPE ShipLocomotionClass::Get_Speed_Accum(void)
{
	return(SpeedAccum);
}

// FUNCTION: GAMEMD 0x006A4260
ULONG STDMETHODCALLTYPE ShipLocomotionClass::AddRef(void)
{
	return(BASECLASS::AddRef());
}

// FUNCTION: GAMEMD 0x006A4270
ULONG STDMETHODCALLTYPE ShipLocomotionClass::Release(void)
{
	return(BASECLASS::Release());
}

// FUNCTION: GAMEMD 0x006A4280
// ShipLocomotionClass::Is_Piggybacking

// FUNCTION: GAMEMD 0x006A42A0
// ShipLocomotionClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x006A42B0
// ??_GShipLocomotionClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x006A4300
// ShipLocomotionClass::QueryInterface

// FUNCTION: GAMEMD 0x006A4310
// ShipLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x006A4320
// ShipLocomotionClass::Release

// FUNCTION: GAMEMD 0x006A4330
// ShipLocomotionClass::QueryInterface

// FUNCTION: GAMEMD 0x006A4340
// ShipLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x006A4350
// ShipLocomotionClass::Release
