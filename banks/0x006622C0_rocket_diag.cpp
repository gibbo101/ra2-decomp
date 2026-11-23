// DIAGNOSTIC, not source: 98.1% raw (3632/3432 B); the 95.2% bank plus 20 dead stores to distinct statics before the last return; with them the only differing lines are the 20 stores themselves (19 stores change nothing; stores inside an inlined helper change nothing)
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rocket.h"
#include "fastmath.h"
#include "_map.h"
#include "_rules.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "cell.h"
#include "combat.h"
#include "foot.h"
#include "inline.h"
#include "globals.h"
#include "rules.h"

// SYNTHETIC: GAMEMD 0x00661CB0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00661CE0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00661D00 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00661D20 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00661D40 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00661D60 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00661D80 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00661DC0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00661DF0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00661E20 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00661E50 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00661E60 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00661E80 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00661EA0 SYMBOL
// _$E66

// FUNCTION: GAMEMD 0x00661EC0
RocketLocomotionClass::RocketLocomotionClass(void) :
	BASECLASS(),
	MovingDestination(COORD_NONE),
	MissionState(0),
	CurrentSpeed(0.0),
	Unknown50(true),
	IsElite(false),
	CurrentPitch(0.0f),
	Unknown58(0)
{
}

// FUNCTION: GAMEMD 0x00661F30
RocketLocomotionClass::~RocketLocomotionClass(void)
{
}

// FUNCTION: GAMEMD 0x00661F50
boolean STDMETHODCALLTYPE RocketLocomotionClass::Is_Moving(void)
{
	return(MovingDestination != COORD_NONE);
}

// FUNCTION: GAMEMD 0x00661F90
boolean STDMETHODCALLTYPE RocketLocomotionClass::Is_Moving_Now(void)
{
	if (MissionState >= 3 && MissionState <= 5) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00661FB0
CoordStruct STDMETHODCALLTYPE RocketLocomotionClass::Destination(void)
{
	return(MovingDestination);
}

// FUNCTION: GAMEMD 0x00661FE0
Coord RocketLocomotionClass::Position_At(int distance)
{
	Coord pos = LinkedTo->PositionCoord;
	int flat = (int)(fastmath::cos(CurrentPitch) * (double)distance);
	Coord result = Move_Coord(pos, LinkedTo->PrimaryFacing.Current(), flat);
	result.Z = (int)(fastmath::sin(CurrentPitch) * (double)distance + pos.Z);
	return(result);
}

// FUNCTION: GAMEMD 0x006620F0
bool RocketLocomotionClass::Reached_Target(RocketStruct const * data)
{
	int z = Position_At(data->BodyLength).Z;
	if (z <= MovingDestination.Z) {
		Detonate();
		return(true);
	}
	CellClass * cell = LinkedTo->Get_Cell_Ptr();
	if (cell != NULL && cell->IsUnderBridge) {
		if (cell->Center_Coord().Z == MovingDestination.Z && z <= MovingDestination.Z + BRIDGE_LEPTON_HEIGHT) {
			Detonate();
			return(true);
		}
	}
	if (LinkedTo->Get_Height_AGL() <= 0) {
		Detonate();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00662240
double RocketLocomotionClass::Pitch_To_Target(void)
{
	Coord pos = LinkedTo->PositionCoord;
	Coord diff = MovingDestination - pos;
	double flat = fastmath::sqrt((double)(diff.X * diff.X + diff.Y * diff.Y));
	if (flat > 0.0) {
		return(fastmath::atan((double)diff.Z / flat));
	}
	return(-1.5707963267948966);
}

extern char const * DAT_008399AC;
extern char const * DAT_008399B0;

static inline int _Planar_Distance(ObjectClass const * object, Coord const & to)
{
	return(Point2D(object->Center_Coord()).Distance_To(Point2D(to)));
}

static inline DirType _Direction(ObjectClass const * object, Coord const & to)
{
	return(Direction(object->Center_Coord(), to));
}

extern int Flip_Probe_Store_[64];
// FUNCTION: GAMEMD 0x006622C0
boolean STDMETHODCALLTYPE RocketLocomotionClass::Process(void)
{
	AircraftTypeClass const * type = ((AircraftClass *)LinkedTo)->Class;
	RocketStruct const * data;
	if (type == Rule->V3Rocket.Type) {
		data = &Rule->V3Rocket;
	} else if (type == Rule->CMisl.Type) {
		data = &Rule->CMisl;
	} else {
		data = &Rule->DMisl;
	}
	bool detonated = false;
	bool exploded = false;

	switch (MissionState) {
		case 1:
			CurrentSpeed = 0.0;
			IsElite = LinkedTo->SpawnOwner != NULL && LinkedTo->SpawnOwner->Veterancy.Is_Elite();
			if (data->Type == Rule->CMisl.Type) {
				if (TrailerTimer == 0) {
					new AnimClass(AnimTypes[AnimTypeClass::From_Name(DAT_008399AC)], LinkedTo->PositionCoord, 2, 1, ShapeFlags_Type(0x600), -10, false);
					TrailerTimer = 24;
				}
				if (Unknown50) {
					LinkedTo->Mark(MARK_UP);
					Unknown50 = false;
					Map.Submit(LinkedTo);
					LinkedTo->Mark(MARK_DOWN);
				}
			} else {
				Unknown50 = true;
			}
			if (MissionTimer.Progress() == 1.0) {
				MissionState = (data->Type == Rule->CMisl.Type) ? 6 : 2;
				MissionTimer = data->TiltFrames;
			}
			break;

		case 6: {
			IsElite = LinkedTo->SpawnOwner != NULL && LinkedTo->SpawnOwner->Veterancy.Is_Elite();
			if (TrailerTimer == 0) {
				new AnimClass(AnimTypes[AnimTypeClass::From_Name(DAT_008399AC)], LinkedTo->PositionCoord, 2, 1, ShapeFlags_Type(0x600), -10, false);
				TrailerTimer = 24;
			}
			Coord coord = LinkedTo->PositionCoord;
			if (MissionTimer.Progress() == 1.0) {
				CurrentPitch = data->PitchFinal * 1.5707963267948966;
				if (LinkedTo->GetLastFlightMapCoords() == CELL_NONE) {
					VocClass::PlayAt(LinkedTo->TClass->AuxSound1, LinkedTo->PositionCoord, NULL);
					AircraftTracker.Add(LinkedTo);
				}
				TrailerTimer = 0;
				MissionState = 3;
			} else {
				coord = coord + Coord(0, 0, data->RaiseRate);
				if (Map.In_Radar(coord.As_Cell())) {
					LinkedTo->Set_Coord(coord);
				}
			}
			break;
		}

		case 2:
			CurrentSpeed = 0.0;
			IsElite = LinkedTo->SpawnOwner != NULL && LinkedTo->SpawnOwner->Veterancy.Is_Elite();
			if (MissionTimer.Progress() == 1.0) {
				CurrentPitch = data->PitchFinal * 1.5707963267948966;
				MissionState = 3;
				if (LinkedTo->GetLastFlightMapCoords() == CELL_NONE) {
					AircraftTracker.Add(LinkedTo);
				}
				new AnimClass(AnimTypes[AnimTypeClass::From_Name(DAT_008399AC)], LinkedTo->PositionCoord, 2, 1, ShapeFlags_Type(0x600), -10, false);
				VocClass::PlayAt(LinkedTo->TClass->AuxSound1, LinkedTo->PositionCoord, NULL);
			} else {
				float initial = data->PitchInitial * 1.5707963267948966;
				float final = data->PitchFinal * 1.5707963267948966;
				CurrentPitch = (final - initial) * MissionTimer.Progress() + initial;
			}
			break;

		case 3:
			if (!Unknown50) {
				LinkedTo->Mark(MARK_UP);
				Unknown50 = true;
				Map.Submit(LinkedTo);
				LinkedTo->Mark(MARK_DOWN);
			}
			CurrentSpeed += data->Acceleration;
			if (CurrentSpeed > LinkedTo->TClass->MaxSpeed) {
				CurrentSpeed = LinkedTo->TClass->MaxSpeed;
			}
			if (LinkedTo->Get_Height_AGL() >= data->Altitude) {
				MissionState = 4;
				Unknown58 = _Planar_Distance(LinkedTo, MovingDestination);
			}
			break;

		case 4:
			if (LinkedTo->Get_Height_AGL() <= 0) {
				Detonate();
				detonated = true;
				break;
			}
			CurrentSpeed += data->Acceleration;
			if (CurrentSpeed > LinkedTo->TClass->MaxSpeed) {
				CurrentSpeed = LinkedTo->TClass->MaxSpeed;
			}
			if (data->LazyCurve && Unknown58 != 0) {
				exploded = Reached_Target(data);
				if (exploded) {
					break;
				}
				int dist = _Planar_Distance(LinkedTo, MovingDestination);
				double ratio = (double)dist / Unknown58;
				double inverse = 1.0 - ratio;
				CurrentPitch = Pitch_To_Target() * inverse + data->PitchFinal * ratio * 1.5707963267948966;
			} else {
				if (CurrentPitch > 0.0f) {
					CurrentPitch -= data->TurnRate;
					if (CurrentPitch < 0.0f) {
						CurrentPitch = 0;
					}
				}
				Coord dest = MovingDestination;
				int height = LinkedTo->PositionCoord.Z - dest.Z;
				dest.Z = LinkedTo->PositionCoord.Z;
				if (LinkedTo->Distance(dest) <= height) {
					MissionState = 5;
				}
			}
			LinkedTo->PrimaryFacing.Set_Desired(_Direction(LinkedTo, MovingDestination));
			break;

		case 5:
			exploded = Reached_Target(data);
			if (!exploded) {
				double delta = Pitch_To_Target() - CurrentPitch;
				float step = delta;
				if (step < 0.0) {
					step = -step;
				}
				if (step > data->TurnRate) {
					if (delta < 0.0) {
						CurrentPitch -= data->TurnRate;
					} else {
						float rate = data->TurnRate;
						CurrentPitch += rate;
					}
				} else {
					CurrentPitch += delta;
				}
			}
			break;

	}

	if (detonated) {
		return(false);
	}
	if (exploded) {
		return(false);
	}

	if (Is_Moving_Now()) {
		if (TrailerTimer == 0) {
			new AnimClass(AnimTypes[AnimTypeClass::From_Name(DAT_008399B0)], LinkedTo->PositionCoord, 0, 1, ShapeFlags_Type(0x600), 0, false);
			TrailerTimer = 3;
		}
	}

	if (CurrentSpeed > 0.0) {
		Coord coord = Position_At((int)CurrentSpeed);
		if (Map.In_Radar(coord.As_Cell())) {
			LinkedTo->Set_Coord(coord);
		}
		Cell cell = LinkedTo->Get_Cell();
		if (cell != LinkedTo->LastFlightMapCoords && LinkedTo->IsActive) {
			AircraftTracker.Update(LinkedTo, LinkedTo->LastFlightMapCoords, cell);
		}
		if (LinkedTo->Strength <= 0) {
			Detonate();
		}
	}

	Flip_Probe_Store_[0] = 0x7F00 + 0;
	Flip_Probe_Store_[1] = 0x7F00 + 1;
	Flip_Probe_Store_[2] = 0x7F00 + 2;
	Flip_Probe_Store_[3] = 0x7F00 + 3;
	Flip_Probe_Store_[4] = 0x7F00 + 4;
	Flip_Probe_Store_[5] = 0x7F00 + 5;
	Flip_Probe_Store_[6] = 0x7F00 + 6;
	Flip_Probe_Store_[7] = 0x7F00 + 7;
	Flip_Probe_Store_[8] = 0x7F00 + 8;
	Flip_Probe_Store_[9] = 0x7F00 + 9;
	Flip_Probe_Store_[10] = 0x7F00 + 10;
	Flip_Probe_Store_[11] = 0x7F00 + 11;
	Flip_Probe_Store_[12] = 0x7F00 + 12;
	Flip_Probe_Store_[13] = 0x7F00 + 13;
	Flip_Probe_Store_[14] = 0x7F00 + 14;
	Flip_Probe_Store_[15] = 0x7F00 + 15;
	Flip_Probe_Store_[16] = 0x7F00 + 16;
	Flip_Probe_Store_[17] = 0x7F00 + 17;
	Flip_Probe_Store_[18] = 0x7F00 + 18;
	Flip_Probe_Store_[19] = 0x7F00 + 19;
	return(Is_Moving());
}

// FUNCTION: GAMEMD 0x00663030
void RocketLocomotionClass::Detonate(void)
{
	AircraftTracker.Remove(LinkedTo);
	AircraftTypeClass const * type = ((AircraftClass *)LinkedTo)->Class;
	RocketStruct const * data;
	WarheadTypeClass const * warhead;
	if (type == Rule->V3Rocket.Type) {
		data = &Rule->V3Rocket;
		warhead = IsElite ? Rule->V3EliteWarhead : Rule->V3Warhead;
	} else if (type == Rule->CMisl.Type) {
		data = &Rule->CMisl;
		warhead = IsElite ? Rule->CMislEliteWarhead : Rule->CMislWarhead;
	} else {
		data = &Rule->DMisl;
		warhead = IsElite ? Rule->DMislEliteWarhead : Rule->DMislWarhead;
	}
	Coord coord = Position_At(data->BodyLength);
	Cell cell = coord;
	int damage = IsElite ? data->EliteDamage : data->Damage;
	new AnimClass(Combat_Anim(damage, warhead, Map[cell].Land_Type(), coord), coord, 0, 1, ShapeFlags_Type(0x2600), Get_Explosion_Z(coord), false);
	Combat_Lighting(coord, damage, warhead);
	MapClass::DamageArea(coord, damage, LinkedTo, warhead, true, NULL);
	LinkedTo->Delete_Me();
}

// FUNCTION: GAMEMD 0x006632E0
void STDMETHODCALLTYPE RocketLocomotionClass::Move_To(CoordStruct to)
{
	AircraftTypeClass const * type = ((AircraftClass *)LinkedTo)->Class;
	RocketStruct const * data;
	if (type == Rule->V3Rocket.Type) {
		data = &Rule->V3Rocket;
	} else if (type == Rule->CMisl.Type) {
		data = &Rule->CMisl;
	} else {
		data = &Rule->DMisl;
	}
	if (MovingDestination == COORD_NONE) {
		if (data->PauseFrames != 0) {
			MissionState = 1;
			MissionTimer = data->PauseFrames;
		} else {
			MissionState = 2;
			MissionTimer = data->TiltFrames;
		}
		CurrentPitch = data->PitchInitial * 1.5707963267948966;
		MovingDestination = to;
	}
}

// FUNCTION: GAMEMD 0x006633C0
void STDMETHODCALLTYPE RocketLocomotionClass::Stop_Moving(void)
{
}

// FUNCTION: GAMEMD 0x006633D0
HRESULT STDMETHODCALLTYPE RocketLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSIDs::Rocket;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00663410
HRESULT STDMETHODCALLTYPE RocketLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) RocketLocomotionClass(NoInitClass());
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00663460
LayerType STDMETHODCALLTYPE RocketLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_AIR);
}

// FUNCTION: GAMEMD 0x00663470
Matrix3DStruct STDMETHODCALLTYPE RocketLocomotionClass::Draw_Matrix(int * key)
{
	Matrix3D mtx(true);
	mtx.Rotate_Z(LinkedTo->PrimaryFacing.Current().As_Radian32());
	if (CurrentPitch != 0.0f) {
		mtx.Rotate_Y(-CurrentPitch);
		AircraftTypeClass const * type = ((AircraftClass *)LinkedTo)->Class;
		RocketStruct const * data;
		if (type == Rule->V3Rocket.Type) {
			data = &Rule->V3Rocket;
		} else if (type == Rule->CMisl.Type) {
			data = &Rule->CMisl;
		} else {
			data = &Rule->DMisl;
		}
		float final = data->PitchFinal * 1.5707963267948966;
		if (key != NULL) {
			if ((float)(data->PitchInitial * 1.5707963267948966) == CurrentPitch) {
				*key |= 0x20;
			} else if (CurrentPitch == final) {
				*key |= 0x40;
			} else {
				*key = -1;
			}
		}
	}
	if (key != NULL) {
		*key |= LinkedTo->PrimaryFacing.Current().As_Dir32();
	}
	return((Matrix3DStruct &)mtx);
}

// FUNCTION: GAMEMD 0x006635B0
// RocketLocomotionClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x006635C0
// ??_GRocketLocomotionClass@@UAEPAXI@Z
