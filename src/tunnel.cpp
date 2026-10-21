// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "tunnel.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "anim.h"
#include "cell.h"
#include "combat.h"
#include "coord.h"
#include "face.h"
#include "fastmath.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "map.h"
#include "noinit.h"
#include "rules.h"
#include "scenario.h"
#include "sun.h"
#include "tactical.h"
#include "layer.hh"
#include "visual.hh"
#include <new.h>

// FUNCTION: GAMEMD 0x00728A00
TunnelLocomotionClass::TunnelLocomotionClass(void) :
	BASECLASS(),
	State(STATE_IDLE),
	DestinationCoord(COORD_NONE),
	IsUnderground(false)
{
}

// FUNCTION: GAMEMD 0x00728A50
boolean STDMETHODCALLTYPE TunnelLocomotionClass::Is_Moving(void)
{
	if (State != STATE_IDLE) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00728A60
boolean STDMETHODCALLTYPE TunnelLocomotionClass::Is_Moving_Now(void)
{
	if (Is_Moving() && State != STATE_PRE_DIG_IN) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00728A90
CoordStruct STDMETHODCALLTYPE TunnelLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: GAMEMD 0x00728AF0
void STDMETHODCALLTYPE TunnelLocomotionClass::Move_To(CoordStruct to)
{
	if (!LinkedTo->IsUnderEMP() && !LinkedTo->IsParalyzed() && !LinkedTo->IsBeingWarpedOut() && !LinkedTo->IsWarpingIn()) {
		Coord coord = to;
		DestinationCoord = LinkedTo->Class_Of()->Coord_Fixup(to);
		if (coord != COORD_NONE && LinkedTo->HeightAGL < 0) {
			if (State != STATE_DUG_IN && State != STATE_PRE_DIG_OUT && State != STATE_PRE_DIG_IN && State != STATE_DIGGING_OUT && State != STATE_DIGGING_IN) {
				LinkedTo->Mark(MARK_UP);
				State = STATE_DIGGING;
			}
		}
		if (State == STATE_IDLE) {
			State = STATE_PRE_DIG_IN;
		}
	}
}

// FUNCTION: GAMEMD 0x00728C00
void STDMETHODCALLTYPE TunnelLocomotionClass::Stop_Moving(void)
{
	switch (State) {
		case STATE_DUG_OUT:
			DestinationCoord = COORD_NONE;
			break;
		case STATE_DIGGING_OUT:
			DestinationCoord = COORD_NONE;
			break;
		case STATE_PRE_DIG_OUT:
			DestinationCoord = COORD_NONE;
			break;

		case STATE_PRE_DIG_IN:
			DestinationCoord = COORD_NONE;
			break;

		case STATE_DIGGING: {
			Coord pos = LinkedTo->PositionCoord;
			if ((Point2D(pos) - Point2D(DestinationCoord)).Length() > CELL_LEPTON_DIAG) {

				Cell cell = LinkedTo->PositionCell;
				Cell nearby = Map.Nearby_Location(cell, SPEED_TRACK, Map.Get_Cell_Zone(cell), MZONE_NORMAL, false, Point2D(1, 1), false, false, true);

				if (nearby == CELL_NONE) {
					nearby = Map.Nearby_Location(cell, SPEED_TRACK, -1, MZONE_NORMAL, false, Point2D(1, 1), false, false, true);
				}

				if (nearby == CELL_NONE) {
					LinkedTo->Take_Damage(LinkedTo->Strength, 0, Rule->C4Warhead, NULL, true, true);
					DestinationCoord = COORD_NONE;
				} else {
					DestinationCoord = nearby;
				}
			}
			break;
		}

		case STATE_DUG_IN:
			State = STATE_PRE_DIG_OUT;
			DestinationCoord = COORD_NONE;
			break;

		case STATE_DIGGING_IN:
			State = STATE_DUG_OUT;
			DestinationCoord = COORD_NONE;
			break;
	}
}

// FUNCTION: GAMEMD 0x00728E30
boolean STDMETHODCALLTYPE TunnelLocomotionClass::Process(void)
{
	if (Is_Moving()) {
		int agl = LinkedTo->HeightAGL;
		int hgt = LinkedTo->Height;
		switch (State) {

			case STATE_PRE_DIG_IN:
				if (agl < 0) {
					State = STATE_DUG_IN;
				} else {
					ProcessPreDigIn();
				}
				break;

			case STATE_DIGGING_IN:
				if (agl < 0) {
					State = STATE_DUG_IN;
				} else {
					ProcessDiggingIn();
				}
				break;

			case STATE_DUG_IN:
				if (hgt == -CELL_LEPTON) {
					LinkedTo->Mark(MARK_UP);
					State = STATE_DIGGING;
				} else {
					ProcessDugIn();
				}
				break;

			case STATE_DIGGING:
				ProcessDigging();
				break;

			case STATE_PRE_DIG_OUT:
				ProcessPreDigOut();
				break;

			case STATE_DIGGING_OUT:
				ProcessDiggingOut();
				break;

			case STATE_DUG_OUT:
				ProcessDugOut();
				break;
		}

		if (LinkedTo->IsSelected) {
			if (!LinkedTo->House->Is_Player_Control()) {
				if (Map.Is_Shrouded(LinkedTo->PositionCoord) || (Scen->Special.IsFogOfWar && Map.Is_Fogged(LinkedTo->PositionCoord))) {
					LinkedTo->Unselect();
				}
			}
		}
		LinkedTo->Remove_Damage_Particle();
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x007291D0
VisualType STDMETHODCALLTYPE TunnelLocomotionClass::Visual_Character(boolean flag)
{
	if (State == STATE_DIGGING) {
		if (flag) {
			return(VISUAL_HIDDEN);
		}
		return(VISUAL_RIPPLE);
	}
	return(VISUAL_NORMAL);
}

// FUNCTION: GAMEMD 0x007291F0
void TunnelLocomotionClass::ProcessPreDigIn(void)
{
	if (!LinkedTo->PrimaryFacing.Is_Rotating()) {
		Coord d = LinkedTo->PositionCoord;
		d.X = DestinationCoord.X - d.X;
		d.Y = DestinationCoord.Y - d.Y;
		DirType dir = DirType(fastmath::atan2((double)-d.Y, (double)d.X));
		if (dir != LinkedTo->PrimaryFacing.Current()) {
			Do_Turn((DirStruct &)dir);
		} else {
			DigTimer = ((64.0 / LinkedTo->TClass->ROT) / Rule->TunnelSpeed);
			State = STATE_DIGGING_IN;
			VocClass::PlayAt(Rule->DigSound, LinkedTo->PositionCoord, NULL);
			new AnimClass(Rule->Dig, LinkedTo->PositionCoord);
			IsUnderground = false;
		}
	}
}

// FUNCTION: GAMEMD 0x00729370
void TunnelLocomotionClass::ProcessDiggingIn(void)
{
	if (DigTimer.Progress() == 1.0) {
		LinkedTo->Set_Speed(1.0);
		State = STATE_DUG_IN;
		VocClass::PlayAt(Rule->DigSound, LinkedTo->PositionCoord, NULL);
		new AnimClass(Rule->Dig, LinkedTo->PositionCoord);
		LinkedTo->Detach_All(false);
	}
}

// FUNCTION: GAMEMD 0x00729480
void TunnelLocomotionClass::ProcessDugOut(void)
{
	if (DigTimer.Progress() == 1.0) {
		State = STATE_IDLE;
		LinkedTo->AbortMotion();
	}
}

// FUNCTION: GAMEMD 0x007294E0
void TunnelLocomotionClass::ProcessDugIn(void)
{
	Coord coord = LinkedTo->PositionCoord;

	int mheight = -CELL_LEPTON;

	if (coord.Z > mheight) {

		int speed = int(LinkedTo->Current_Speed() * Rule->TunnelSpeed);
		if (speed <= 5) {
			speed = 5;
		}
		coord.Z -= speed;
		if (coord.Z < mheight) {
			coord.Z = mheight;
		}
		LinkedTo->PositionCoord = coord;

	} else {
		LinkedTo->Mark(MARK_UP);
		State = STATE_DIGGING;
	}
}

// FUNCTION: GAMEMD 0x007298F0
void TunnelLocomotionClass::ProcessPreDigOut(void)
{
	Coord coord = LinkedTo->PositionCoord;

	int mheight = Map.Get_Height_GL(coord);

	if (coord.Z < mheight) {
		int height = LinkedTo->HeightAGL;

		int speed = int(LinkedTo->Current_Speed() * Rule->TunnelSpeed);
		if (speed <= 5) {
			speed = 5;
		}
		coord.Z += speed;
		if (coord.Z > mheight) {
			coord.Z = mheight;
		}
		LinkedTo->PositionCoord = coord;

		if (height <= -50 && LinkedTo->HeightAGL > -50) {
			VocClass::PlayAt(Rule->DigSound, LinkedTo->PositionCoord, NULL);
			new AnimClass(Rule->Dig, LinkedTo->PositionCoord);
		}
	} else {
		LinkedTo->Look();
		DigTimer = int(64.0 / LinkedTo->TClass->ROT);
		LinkedTo->Set_Speed(0);
		State = STATE_DIGGING_OUT;
	}
}

// FUNCTION: GAMEMD 0x00729AA0
void TunnelLocomotionClass::ProcessDiggingOut(void)
{
	if (DigTimer.Progress() == 1.0) {
		LinkedTo->Per_Cell_Process(PCP_END);
		if (LinkedTo->IsActive && !LinkedTo->IsInLimbo && !LinkedTo->IsFalling) {
			Stop_Moving();
			State = STATE_IDLE;
			LinkedTo->AbortMotion();
		}
	}
}

// FUNCTION: GAMEMD 0x00729B40
Matrix3DStruct STDMETHODCALLTYPE TunnelLocomotionClass::Draw_Matrix(int * key)
{
	if (State == STATE_IDLE) {
		int ramp = Map[(Coord const &)(LinkedTo->PositionCoord)].Ramp;
		Matrix3D mtx;
		if (key != NULL && *key != -1) {
			*key = ramp + (*key << 6);
		}
		mtx.Make_Identity();
		if (ramp != 0) {
			mtx = Get_Slope_Matrix(ramp) * BASECLASS::Draw_Matrix(key);
		} else {
			mtx = BASECLASS::Draw_Matrix(key);
		}
		return((Matrix3DStruct &)mtx);
	}
	double theta;

	switch (State) {

		case STATE_DUG_IN:
			theta = DEG_TO_RAD(90);
			break;

		case STATE_PRE_DIG_OUT:
			theta = DEG_TO_RAD(-90);
			break;

		case STATE_DIGGING_IN:
			if (key != NULL) {
				*key = -1;
			}
			theta = DigTimer.Progress() * DEG_TO_RAD(90);
			break;

		case STATE_DIGGING_OUT:
			if (key != NULL) {
				*key = -1;
			}
			theta = (1.0 - DigTimer.Progress()) * -DEG_TO_RAD(90);
			break;

		case STATE_DUG_OUT:
			if (key != NULL) {
				*key = -1;
			}
			theta = (1.0 - DigTimer.Progress()) * DEG_TO_RAD(90);
			break;

		default:
			theta = 0.0;
			break;
	}
	Matrix3D mtx = BASECLASS::Draw_Matrix(key);
	if (key != NULL && *key != -1) {
		*key = *key << 8;
		*key |= (int(theta * 32.0) & 64-1);
	}
	mtx.Rotate_Y(theta);
	return((Matrix3DStruct &)mtx);
}

// FUNCTION: GAMEMD 0x00729E50
int STDMETHODCALLTYPE TunnelLocomotionClass::Z_Adjust(void)
{
	static int tunnel_Z_Adjust[] = {45, 45};

	Coord coord = LinkedTo->PositionCoord;
	Point2D point1;
	TacticalMap->Coord_To_Pixel(coord, point1);

	coord.Z = Map.Get_Height_GL(coord);

	Point2D point2;
	TacticalMap->Coord_To_Pixel(coord, point2);

	int zadjust = point1.Y - point2.Y;

	double progress = DigTimer.Progress();

	Dir256 face = LinkedTo->PrimaryFacing.Current().As_Dir256();

	switch (State) {

		case STATE_DIGGING:
			return(tunnel_Z_Adjust[1] + zadjust);

		case STATE_DIGGING_OUT:
			return(tunnel_Z_Adjust[0] * (1.0 - progress) + zadjust);

		case STATE_DIGGING_IN:
			if (progress > 0.4 && !IsUnderground) {
				IsUnderground = true;
			}
			if (face > Dir256(70) && face < Dir256(140)) {
				zadjust = tunnel_Z_Adjust[1] + zadjust;
			} else {
				zadjust = tunnel_Z_Adjust[1] * progress + zadjust;
			}
			return(zadjust);

		case STATE_DUG_IN:
			return(tunnel_Z_Adjust[1] + zadjust);

		case STATE_PRE_DIG_OUT:
			return(tunnel_Z_Adjust[0] + zadjust);
	}

	return(BASECLASS::Z_Adjust());
}

// FUNCTION: GAMEMD 0x0072A020
ZGradientType STDMETHODCALLTYPE TunnelLocomotionClass::Z_Gradient(void)
{
	if (State == STATE_DUG_IN || State == STATE_DIGGING_IN || State == STATE_DUG_OUT || State == STATE_DIGGING_OUT || State == STATE_PRE_DIG_OUT) {
		return(ZGRAD_90DEG);
	}
	return(BASECLASS::Z_Gradient());
}

// FUNCTION: GAMEMD 0x0072A060
boolean STDMETHODCALLTYPE TunnelLocomotionClass::Is_To_Have_Shadow(void)
{
	if (State == STATE_IDLE || State == STATE_PRE_DIG_IN || State == STATE_DUG_OUT || State == STATE_DIGGING_IN || State == STATE_DIGGING_OUT) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0072A090
MoveType STDMETHODCALLTYPE TunnelLocomotionClass::Can_Enter_Cell(CellStruct cell)
{
	if (!Debug_Map && !Map[cell].Can_Burrow_Here()) {
		return(MOVE_NO);
	}
	return(MOVE_OK);
}

// FUNCTION: GAMEMD 0x0072A0E0
void STDMETHODCALLTYPE TunnelLocomotionClass::Do_Turn(DirStruct coord)
{
	DirType dir = (DirType &)coord;
	LinkedTo->PrimaryFacing.Set_Desired(dir);
}

// FUNCTION: GAMEMD 0x0072A110
HRESULT STDMETHODCALLTYPE TunnelLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TunnelLocomotion;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0072A150
HRESULT STDMETHODCALLTYPE TunnelLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TunnelLocomotionClass(NoInitClass());
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0072A1A0
LayerType STDMETHODCALLTYPE TunnelLocomotionClass::In_Which_Layer(void)
{
	if (State != STATE_DIGGING) {
		return(LAYER_GROUND);
	}
	return(LAYER_UNDERGROUND);
}

// FUNCTION: GAMEMD 0x0072A1C0
FireErrorType STDMETHODCALLTYPE TunnelLocomotionClass::Can_Fire(void)
{
	FireErrorType fire = BASECLASS::Can_Fire();

	if (fire == FIRE_OK && State != STATE_IDLE) {
		fire = FIRE_MOVING;
	}

	return(fire);
}

// FUNCTION: GAMEMD 0x0072A1E0
boolean STDMETHODCALLTYPE TunnelLocomotionClass::Is_Surfacing(void)
{
	return(State == STATE_PRE_DIG_OUT || State == STATE_DIGGING_OUT);
}

// FUNCTION: GAMEMD 0x0072A200
// TunnelLocomotionClass::Fetch_Object_Size
