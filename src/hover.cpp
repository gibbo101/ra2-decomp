// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
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
#include "movepoint.h"
#include "ion.h"
#include "overtype.h"
#include "rules.h"
#include "tube.h"
#include "unit.h"
#include "tube.hh"

// SYNTHETIC: GAMEMD 0x00513A10 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00513A40 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00513A60 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00513A80 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00513AA0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00513AC0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00513AE0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00513B20 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00513B50 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00513B80 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00513BB0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00513BC0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00513BE0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00513C00 SYMBOL
// _$E52

// FUNCTION: GAMEMD 0x00513C20
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

// FUNCTION: GAMEMD 0x00513CB0
HRESULT STDMETHODCALLTYPE HoverLocomotionClass::Link_To_Object(void *pointer)
{
	HRESULT res = BASECLASS::Link_To_Object(pointer);
	FacingClass face(2 * LinkedTo->TClass->ROT);
	Facing = face;
	return(res);
}

// FUNCTION: GAMEMD 0x00513D00
HoverLocomotionClass::~HoverLocomotionClass(void)
{
}

// FUNCTION: GAMEMD 0x00513D20
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

// FUNCTION: GAMEMD 0x00513F40
Matrix3DStruct STDMETHODCALLTYPE HoverLocomotionClass::Draw_Matrix(int *key)
{
	if ((float)fabs(LinkedTo->AngleRotatedSideways) >= 0.005 || (float)fabs(LinkedTo->AngleRotatedForwards) >= 0.005) {
		if (key) {
			*key = -1;
		}

		Matrix3D m1(true), m2(true);

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

		return((Matrix3DStruct &)((m1 * BASECLASS::Draw_Matrix(key)) * m2));
	}

	if (!Is_Powered()) {
		int ramp = Map[(Coord const &)(LinkedTo->PositionCoord)].Ramp;
		Matrix3D mtx = Get_Slope_Matrix(ramp);
		mtx.Rotate_Z(LinkedTo->PrimaryFacing.Current().As_Radian32());
		if (Is_Cache_Key(key)) {
			*key = 32 * (ramp + (*key << 6));
			*key |= LinkedTo->PrimaryFacing.Current().As_Dir32();
		}
		return((Matrix3DStruct &)mtx);
	}
	return(BASECLASS::Draw_Matrix(key));
}

// FUNCTION: GAMEMD 0x005142A0
Matrix3DStruct STDMETHODCALLTYPE HoverLocomotionClass::Shadow_Matrix(int *key)
{
	if (!((float)fabs(LinkedTo->AngleRotatedSideways) < 0.005 && (float)fabs(LinkedTo->AngleRotatedForwards) < 0.005) && key) {
		*key = -1;
	}
	return(BASECLASS::Shadow_Matrix(key));
}

static inline DirType Facing_To(Coord const & from, Coord const & to)
{
	return(DirType().Direction(from, to));
}

// FUNCTION: GAMEMD 0x00514310
boolean STDMETHODCALLTYPE HoverLocomotionClass::Process(void)
{
	if (Is_Moving() && Is_Moving1()) {
		Motion_AI();
		if (!LinkedTo->IsActive) {
			return(false);
		}

		int height = LinkedTo->Get_Height();
		int speed = (int)(LinkedTo->Current_Speed() * Acceleration);
		DirType direction = Facing_To(LinkedTo->Center_Coord(), HeadToCoord);

		if (speed == 0 && Facing.Current().Is_Complete_Turn(direction, 0)) {
			Start();
			speed = (int)(LinkedTo->Current_Speed() * Acceleration);
		}

		Point2D delta = LinkedTo->Get_Coord();
		delta = delta - HeadToCoord;
		if (speed >= delta.Length()) {
			LinkedTo->IsOccupyingCell = true;
			LinkedTo->IsToPathAroundBlockage = false;
			WasPushed = false;

			Start_Of_Move(0);

			if (LinkedTo->Path[0] == FACING_NONE && DestinationCoord == COORD_NONE) {
				HeadToCoord = COORD_NONE;
			}

			TechnoClass * contact = LinkedTo->Contact_With_Whom();
			Cell cell = CELL_NONE;
			if (contact != NULL) {
				cell = Cell(contact->Destination_Coord(LinkedTo));
			}
			bool arrived = LinkedTo->Get_Cell() == cell;

			MoveType ok = MOVE_OK;
			if (DestinationCoord != COORD_NONE || arrived) {
				ok = While_Moving(true);
				if (!LinkedTo->IsActive || LinkedTo->IsInLimbo || LinkedTo->IsFalling) {
					return(MOVE_NO);
				}
				if (ok == MOVE_OK && HeadToCoord == COORD_NONE) {
					LinkedTo->Set_Occupy_Bit(LinkedTo->Get_Coord());
					LinkedTo->Per_Cell_Process(PCP_END);
					if (!LinkedTo->IsActive || LinkedTo->IsInLimbo || LinkedTo->IsFalling) {
						return(false);
					}
				}
			}
			if (ok == MOVE_NO) {
				if (LinkedTo->CurrentTube >= 0) {
					return(false);
				}
				Stop();
			} else if (ok != MOVE_OK) {
				Stop_Driver();
				speed = 0;
			}
		}

		if (speed > 0) {
			Coord coord = LinkedTo->Get_Coord();

			if (LinkedTo->Occupies_Cells() && HeadToCoord != COORD_NONE) {
				LinkedTo->Clear_Occupy_Bit(LinkedTo->Get_Coord());
				LinkedTo->IsOccupyingCell = false;
				LinkedTo->IsToPathAroundBlockage = false;
			}

			Point2D position(coord.X, coord.Y);
			Point2D next = Move_Point(position, Facing.Current(), speed);
			coord.X = next.X;
			coord.Y = next.Y;
			if (coord.As_Cell() != LinkedTo->Get_Coord().As_Cell()) {
				LinkedTo->Mark(MARK_UP);
				LinkedTo->Set_Coord(coord);
				LinkedTo->Set_Height(height);
				CellClass * cellptr = &Map[coord];
				if (!LinkedTo->IsOnBridge && cellptr->IsUnderBridge && LinkedTo->Get_Height_AGL() >= BRIDGE_LEPTON_HEIGHT) {
					LinkedTo->IsOnBridge = true;
				}
				if (LinkedTo->IsOnBridge == true && !cellptr->IsUnderBridge) {
					LinkedTo->IsOnBridge = false;
				}
				LinkedTo->Mark(MARK_DOWN);
			} else {
				bool wasdown = LinkedTo->IsDown;
				LinkedTo->IsDown = false;
				LinkedTo->Set_Coord(coord);
				LinkedTo->Set_Height(height);
				LinkedTo->IsDown = wasdown;
			}
		}
	}

	if (Is_Moving_Now() && (Frame % 10) == 0) {
		if (!LinkedTo->IsOnBridge && LinkedTo->Get_Cell_Ptr()->Land_Type() == LAND_WATER) {
			if (Rule->Wake != NULL) {
				new AnimClass(Rule->Wake, LinkedTo->Get_Coord(), 0, 1, ShapeFlags_Type(0x600), 0);
			}
		}
	}

	Gravity_AI();

	if (WasShoved) {
		if (Is_Powered()) {
			DirType dir = LinkedTo->PrimaryFacing.Current();
			LinkedTo->PrimaryFacing.Set(dir + DirType(Dir256(ShoveAccum)));
			int delta = abs(ShoveAccum);
			if (delta >= 1) {
				delta = 1;
			}
			if (ShoveAccum < 0) {
				ShoveAccum += delta;
			} else {
				ShoveAccum -= delta;
			}
			if (ShoveAccum == 0) {
				WasShoved = false;
			}
		} else {
			WasShoved = false;
		}
		if (!WasShoved) {
			Map[LinkedTo->Get_Coord()].Trigger_Veins();
			if (Map[LinkedTo->Get_Coord()].Land_Type() == LAND_WATER) {
				if (LinkedTo->Get_Coord().Z < Map.Get_Height_GL(LinkedTo->Get_Coord()) + LEVEL_LEPTON_H) {
					LinkedTo->Fall_From_Height();
				}
			}
		}
	}

	return(Is_Moving());
}

// FUNCTION: GAMEMD 0x00514C30
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Moving(void)
{
	return(DestinationCoord != COORD_NONE || HeadToCoord != COORD_NONE);
}

// FUNCTION: GAMEMD 0x00514C80
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Moving_Now(void)
{
	return(Is_Moving() && Height != 0.0);
}

// FUNCTION: GAMEMD 0x00514CB0
CoordStruct STDMETHODCALLTYPE HoverLocomotionClass::Destination(void)
{
	if (DestinationCoord != COORD_NONE) {
		return(DestinationCoord);
	}
	return(COORD_NONE);
}

// FUNCTION: GAMEMD 0x00514D10
CoordStruct STDMETHODCALLTYPE HoverLocomotionClass::Head_To_Coord(void)
{
	if (HeadToCoord != COORD_NONE) {
		return(HeadToCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: GAMEMD 0x00517110
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

// FUNCTION: GAMEMD 0x00514D90
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

// FUNCTION: GAMEMD 0x00514F70
MoveType HoverLocomotionClass::While_Moving(bool first_pass)
{
	if (HeadToCoord != COORD_NONE) {
		LinkedTo->Clear_Occupy_Bit(HeadToCoord);
		HeadToCoord = COORD_NONE;
	}

	if (LinkedTo->IsParalyzed()) {
		return(MOVE_NO);
	}

	if (!Is_Moving() || DestinationCoord == COORD_NONE) {
		return(MOVE_OK);
	}

	FacingType nextface;
	nextface = LinkedTo->Path[0];

	if (nextface == FACING_COUNT) {
		TubeType tubenum = (TubeType)Map[LinkedTo->Get_Coord()].Tube;
		if (tubenum >= 0 && tubenum < Tubes.Count()) {
			LinkedTo->Mark(MARK_UP);
			LinkedTo->Clear_Occupy_Bit(LinkedTo->PositionCoord);
			TubeClass * tube = Tubes[tubenum];
			Cell exit = tube->Get_Exit();
			HeadToCoord = exit.As_Coord();
			memcpy((char*)&LinkedTo->Path, (char*)&LinkedTo->Path[1], sizeof(LinkedTo->Path[0]) * (CONQUER_PATH_MAX-1));
			LinkedTo->LastPathingCell = HeadToCoord.As_Cell();
			LinkedTo->Path[CONQUER_PATH_MAX-1] = FACING_NONE;
			LinkedTo->CurrentTube = tubenum;
			LinkedTo->CurrentTubeDir = FACING_FIRST;
			Coord enter = (Cell)tube->Enter;
			Coord offset = Map[Adjacent_Cell((Cell)tube->Enter, tube->Dirs[0])].Cell_Coord() - enter;
			LinkedTo->LastTubeCoord = LinkedTo->PositionCoord + offset;
			int height = LinkedTo->Height;
			int current_height = Map.Get_Height_GL(LinkedTo->PositionCoord);
			LinkedTo->LastTubeCoord.Z = height + (Map.Get_Height_GL(tube->Get_Exit()) - current_height) / tube->Get_Count();
		} else {
			LinkedTo->Path[0] = FACING_NONE;
			HeadToCoord = COORD_NONE;
			Stop();
			LinkedTo->Assign_Destination(NULL);
		}
		return(MOVE_NO);
	}

	if (nextface != FACING_NONE) {
		HeadToCoord = Adjacent_Cell(LinkedTo->PositionCell, nextface).As_Coord();
		HeadToCoord.Z = Map.Get_Height_GL(HeadToCoord);
		if (LinkedTo->PositionCoord.Z >= HeadToCoord.Z + 2 * LEVEL_LEPTON_H + LEVEL_LEPTON_H) {
			HeadToCoord.Z += BRIDGE_LEPTON_HEIGHT;
		}

		if (!Map[HeadToCoord].Goodie_Check(LinkedTo) && !LinkedTo->IsInLimbo) {
			if (LinkedTo->IsActive && !LinkedTo->IsFalling) {
				LinkedTo->Path[0] = FACING_NONE;
				HeadToCoord = COORD_NONE;
				Stop();
				return(MOVE_NO);
			}
			return(MOVE_NO);
		}

		if (!LinkedTo->IsActive || LinkedTo->IsInLimbo || LinkedTo->IsFalling) return(MOVE_NO);

		CellClass const & next = Map[Adjacent_Cell(LinkedTo->PositionCoord, nextface)];
		if (LinkedTo->IsOnBridge ^ next.IsUnderBridge) {
			LinkedTo->IsPlanningToLook = true;
		}

		Cell c = HeadToCoord;
		MoveType ok = LinkedTo->Can_Enter_Cell(&Map[c], nextface, LinkedTo->Get_Cell_Height());

		switch (ok) {
		case MOVE_CLOAK:
		case MOVE_NO:
			Map[HeadToCoord].Shimmer();
			if (first_pass) {
				Stop_Driver();
				LinkedTo->PathDelay = 0;
				LinkedTo->Path[0] = FACING_NONE;
				Start_Of_Move(0);
				ok = While_Moving(false);
			}
			if (ok != MOVE_OK) {
				HeadToCoord = COORD_NONE;
				Stop_Driver();
			}
			return(ok);

		case MOVE_CLOSED_GATE:
			HeadToCoord = COORD_NONE;
			Map.Try_Open_Gate(LinkedTo, c);
			Stop_Driver();
			break;

		case MOVE_TEMP:
			if (first_pass) {
				HeadToCoord = COORD_NONE;
				Stop_Driver();
				LinkedTo->PathDelay = 0;
				LinkedTo->Path[0] = FACING_NONE;
				Start_Of_Move(0);
				ok = While_Moving(false);
			} else {
				if (LinkedTo->Center_Coord().Distance_To(DestinationCoord) < Rule->CloseEnoughDistance && abs(DestinationCoord.Z - LinkedTo->PositionCoord.Z) < 2 * LEVEL_LEPTON_H && Map[LinkedTo->Get_Coord()].Land_Type() != LAND_TUNNEL) {
					Stop_Moving();
					LinkedTo->Assign_Destination(NULL);
					return(MOVE_NO);
				}
				bool bridge;
				if (!Map[HeadToCoord].IsUnderBridge || abs(LinkedTo->PositionCoord.Z / LEVEL_LEPTON_H - Map[HeadToCoord].Height) <= 2) {
					bridge = false;
				} else {
					bridge = true;
				}
				Map[HeadToCoord].Incoming(COORD_NONE, true, true, bridge);
				HeadToCoord = COORD_NONE;
				Stop_Driver();
			}
			break;

		case MOVE_OK:
			LinkedTo->Per_Cell_Process(PCP_END);
			if (!LinkedTo->IsActive || LinkedTo->IsInLimbo || LinkedTo->IsFalling) return(MOVE_NO);
			if (LinkedTo->IsInPlayfield) {
				LinkedTo->Set_Occupy_Bit(HeadToCoord);
			}
			memcpy((char *)&LinkedTo->Path, (char *)&LinkedTo->Path[1], sizeof(LinkedTo->Path[0]) * (CONQUER_PATH_MAX - 1));
			LinkedTo->Path[CONQUER_PATH_MAX - 1] = FACING_NONE;
			LinkedTo->LastPathingCell = HeadToCoord.As_Cell();
			break;

		case MOVE_FRIENDLY_DESTROYABLE:
		case MOVE_DESTROYABLE:
			HeadToCoord = COORD_NONE;
			if (first_pass) {
				Stop_Driver();
				LinkedTo->PathDelay = 0;
				LinkedTo->Path[0] = FACING_NONE;
				Start_Of_Move(0);
				ok = While_Moving(false);
				break;
			}
			if (Map[c].Cell_Object() != NULL) {
				if (!LinkedTo->House->Is_Ally(Map[c].Cell_Object())) {
					LinkedTo->Override_Mission(MISSION_ATTACK, Map[c].Cell_Object(), NULL);
				}
			} else {
				if (Map[c].Overlay != OVERLAY_NONE && OverlayTypes[Map[c].Overlay]->IsWall) {
					LinkedTo->Override_Mission(MISSION_ATTACK, &Map[c], NULL);
				}
			}
			break;

		case MOVE_MOVING_BLOCK:
			HeadToCoord = COORD_NONE;
			Stop_Driver();
			if (!LinkedTo->IsToPathAroundBlockage) {
				LinkedTo->IsToPathAroundBlockage = true;
				LinkedTo->BlockagePathDelay = Rule->BlockagePathDelay;
			}
			if (LinkedTo->PathDelay == 0) {
				LinkedTo->Path[0] = FACING_NONE;
			}
			bool blocked = LinkedTo->IsToPathAroundBlockage && LinkedTo->BlockagePathDelay == 0;
			Start_Of_Move((blocked != 0) + 1);
			break;
		}
		return(ok);
	}

	Start_Of_Move(0);
	return(MOVE_OK);
}

// FUNCTION: GAMEMD 0x00515E30
void HoverLocomotionClass::Stop(void)
{
	LinkedTo->Assign_Destination(NULL);
	Acceleration = 0;
	Height = 0;
	LinkedTo->Set_Speed(0);
}

// FUNCTION: GAMEMD 0x00515E60
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

// FUNCTION: GAMEMD 0x00515ED0
void HoverLocomotionClass::Motion_AI(void)
{
	if (Is_Moving() && HeadToCoord != COORD_NONE) {
		DirType newdir = Facing_To(LinkedTo->Center_Coord(), HeadToCoord);
		if (!WasPushed) {
			Facing.Set_Desired(newdir);
		} else {
			Facing.Set(newdir);
		}

		if (Is_Powered() && Facing.Current().Is_Complete_Turn(newdir, DIR_STEP_8 << 8) || WasPushed) {
			if ((DestinationCoord == COORD_NONE && LinkedTo->Distance(HeadToCoord) < CELL_LEPTON) ||
				 DestinationCoord != COORD_NONE && LinkedTo->Distance(DestinationCoord) < CELL_LEPTON) {

				Height = 0.5;
			} else {
				Height = 1;
				if (WasPushed) {
					Acceleration = 1;
				}
			}
		} else {
			Height = 0;
		}

		if (Height > 0) {
			Boost = 1;
			if (LinkedTo->Path[0] != FACING_NONE && LinkedTo->Path[0] == LinkedTo->Path[1]) {
				Boost = Rule->HoverBoost;
			}
		}

		double accel = MIN(1, Boost * Height);

		if (accel > Acceleration) {
			Acceleration += 1.0 / (Rule->HoverAcceleration * TICKS_PER_MINUTE);
			Acceleration = MIN(Acceleration, accel);
		}

		if (accel < Acceleration) {
			Acceleration -= 1.0 / (Rule->HoverBrake * TICKS_PER_MINUTE);
			Acceleration = MAX(Acceleration, 0);
		}

		if (Height > 0 && !WasPushed) {
			Coord coord = HeadToCoord;
			if (LinkedTo->Path[0] != FACING_NONE && LinkedTo->Path[0] != FACING_COUNT) {
				coord = Adjacent_Cell(coord, LinkedTo->Path[0]);
			}
			DirType dir = Direction(LinkedTo->Center_Coord(), coord);
			LinkedTo->PrimaryFacing.Set_Desired(dir);
		}
		return;
	}

	if (Is_Moving() && HeadToCoord == COORD_NONE) {
		LinkedTo->Set_Speed(1);
		Start_Of_Move(0);
		While_Moving(true);
	}
}


// FUNCTION: GAMEMD 0x00516320
void STDMETHODCALLTYPE HoverLocomotionClass::Stop_Moving(void)
{
	if (DestinationCoord != HeadToCoord) {
		DestinationCoord = COORD_NONE;
		LinkedTo->Path[0] = FACING_NONE;
	}
}

// FUNCTION: GAMEMD 0x00516370
void HoverLocomotionClass::Do_Turn(DirStruct coord)
{
	DirType dir = (DirType &)coord;

	LinkedTo->PrimaryFacing.Set_Desired(dir);
}

// FUNCTION: GAMEMD 0x005163A0
bool HoverLocomotionClass::Is_Moving1(void)
{
	if (Is_Moving()) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005163B0
bool HoverLocomotionClass::Reduce_Path_Length(void)
{
	if (Is_Moving()) {
		if (LinkedTo->Path[0] != FACING_NONE) {
			int dist;

			if ((LinkedTo->NavCom != NULL && LinkedTo->NavCom->Fetch_RTTI() == RTTI_UNIT) || (LinkedTo->NavCom != NULL && LinkedTo->NavCom->Fetch_RTTI() == RTTI_INFANTRY)) {
				dist = Lepton_To_Cell((LEPTON)LinkedTo->Distance(DestinationCoord));

				if (dist < ARRAY_SIZE(LinkedTo->Path)) {
					LinkedTo->Path[dist] = FACING_NONE;
					return(true);
				}
			}

			if (LinkedTo->TarCom != NULL && LinkedTo->IsCloseEnoughToAttack(LinkedTo->TarCom)) {
				LinkedTo->Path[0] = FACING_NONE;
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005164D0
void HoverLocomotionClass::Start_Of_Move(int num)
{
	FacingType facing;

	Reduce_Path_Length();

	facing = LinkedTo->Path[0];

	if (DestinationCoord != COORD_NONE) {
		if (LinkedTo->PositionCell == DestinationCoord.As_Cell() &&
			abs(LinkedTo->Destination_Coord().Z - DestinationCoord.Z) <
				2 * LEVEL_LEPTON_H) {
			Stop_Moving();
			LinkedTo->Assign_Destination(NULL);
			return;
		}
	}

	if (facing == FACING_NONE) {
		if (DestinationCoord == COORD_NONE) {
			return;
		}

		if (LinkedTo->PositionCell == DestinationCoord.As_Cell() &&
			abs(LinkedTo->Destination_Coord().Z - DestinationCoord.Z) <=
				2 * LEVEL_LEPTON_H) {
			return;
		}

		if (LinkedTo->PathDelay != 0) {
			return;
		}

		if (!LinkedTo->Basic_Path(DestinationCoord.As_Cell(), 0, num)) {
			LinkedTo->PathDelay = Rule->PathDelay * TICKS_PER_MINUTE;

			if (!LinkedTo->Is_In_Same_Zone(DestinationCoord)) {
				LinkedTo->Assign_Destination(NULL);
				return;
			}

			if (!LinkedTo->Is_On_Priority_Mission() &&
				LinkedTo->Center_Coord().Distance_To(DestinationCoord) <
					Rule->CloseEnoughDistance &&
				(LinkedTo->Mission == MISSION_MOVE ||
				 LinkedTo->Mission == MISSION_GUARD_AREA)) {
				LinkedTo->Assign_Destination(NULL);
				if (!LinkedTo->IsActive) return;
			} else {
				if (LinkedTo->TryTryAgain > 0) {
					LinkedTo->TryTryAgain--;
				} else {
					LinkedTo->Assign_Destination(NULL);
					if (!LinkedTo->IsActive) return;
					if (LinkedTo->IsNewNavCom) {
						VocClass::PlayGlobal(
							Rule->ScoldSound, 1.0f, 0x2000);
					}
					LinkedTo->IsNewNavCom = false;
				}
			}

			if (!Is_Moving() &&
				LinkedTo->TarCom != NULL &&
				!LinkedTo->IsCloseEnoughToAttack(LinkedTo->TarCom)) {
				LinkedTo->IsScanLimited = true;
				if (LinkedTo->Team != NULL) {
					LinkedTo->Team->Scan_Limit();
				}
				LinkedTo->Assign_Target(NULL);
			}

			if (HeadToCoord != COORD_NONE) {
				LinkedTo->Clear_Occupy_Bit(HeadToCoord);
				HeadToCoord = COORD_NONE;
			}
			Stop_Moving();
			return;
		} else {
			LinkedTo->PathDelay = Rule->PathDelay * TICKS_PER_MINUTE;

			Cell cell =
				Adjacent_Cell(LinkedTo->Center_Coord().As_Cell(), LinkedTo->Path[0]);

			if (Map.In_Local_Radar(cell, true)) {
				MoveType ok = LinkedTo->Can_Enter_Cell(
					&Map[cell],
					LinkedTo->Path[0],
					LinkedTo->Get_Cell_Height());

				if (ok == MOVE_TEMP) {
					CellClass * cellptr = &Map[cell];
					ObjectClass * blockage = cellptr->Cell_Techno(
						Point2D(0, 0),
						LinkedTo->PositionCoord.Z > Map.Get_Height_GL(Coord(cell)) +
							3 * LEVEL_LEPTON_H);

					if (blockage && LinkedTo->House->Is_Ally(blockage)) {
						int distance = LinkedTo->Distance(DestinationCoord);
						if (distance < Rule->CloseEnoughDistance &&
							!LinkedTo->HasAnyLink() &&
							abs(DestinationCoord.Z -
								LinkedTo->PositionCoord.Z) <
								2 * LEVEL_LEPTON_H &&
							Map[LinkedTo->Get_Coord()].Land_Type() !=
								LAND_TUNNEL) {
							Stop_Moving();
							LinkedTo->Assign_Destination(NULL);
							return;
						} else {
							bool bridge =
								cellptr->IsUnderBridge &&
								abs(LinkedTo->PositionCoord.Z /
										LEVEL_LEPTON_H -
									cellptr->Height) > 2;

							cellptr->Incoming(
								COORD_NONE, true, true, bridge);
						}
					}
				}
			}

			LinkedTo->TryTryAgain = FootClass::PATH_RETRY;
			facing = LinkedTo->Path[0];
		}
	}
}

// FUNCTION: GAMEMD 0x00516BF0
boolean STDMETHODCALLTYPE HoverLocomotionClass::Power_Off(void)
{
	if (Is_Powered() && LinkedTo->CurrentMission != MISSION_SLEEP && LinkedTo->CurrentMission != MISSION_ENTER) {
		Do_Shove();
	}
	if (Is_Moving()) {
		Stop_Moving();
	}

	return(BASECLASS::Power_Off());
}

// FUNCTION: GAMEMD 0x00516C70
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Powered(void)
{
	if (!BASECLASS::Is_Powered() && LinkedTo->HeightAGL <= 0) {
		return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00516CA0
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Ion_Sensitive(void)
{
	BuildingClass *bptr;
	if (LinkedTo->HasAnyLink()) {
		BuildingClass * ptr = (BuildingClass *)LinkedTo->FUN_0065AD30(0);

		if (ptr->Fetch_RTTI() == RTTI_BUILDING) {
			ptr = (BuildingClass *)LinkedTo->FUN_0065AD30(0);
			if (ptr->Class->IsWeaponsFactory) {
				return(false);
			}
		}
	}

	bptr = Map[(Coord const &)LinkedTo->PositionCoord].Cell_Building();

	if (bptr != NULL && bptr->Class->IsWeaponsFactory) {

		Cell diff = LinkedTo->PositionCoord.As_Cell() - bptr->PositionCoord.As_Cell();

		if (diff.X == 0 && diff.Y == 1) {
			return(false);
		} else
		if (diff.X == 2 && diff.Y == 1) {
			return(false);
		} else
		if (diff.X == 3 && diff.Y == 1) {
			return(false);
		}

	}

	return(true);
}

// FUNCTION: GAMEMD 0x00516E10
boolean STDMETHODCALLTYPE HoverLocomotionClass::Push(DirStruct dir)
{
	if (Is_Powered() && !WasPushed) {

		FacingType face = ((DirType &)dir).As_Facing();
		Cell cell = Adjacent_Cell(LinkedTo->PositionCell, face);

		FootClass * link = LinkedTo;

		if (link->Can_Enter_Cell(&Map[cell]) == MOVE_OK) {
			WasPushed = true;

			if (Is_Moving()) {

				LinkedTo->Path[0] = FACING_NONE;
				if (HeadToCoord != COORD_NONE) {
					LinkedTo->Clear_Occupy_Bit(HeadToCoord);
				}

				HeadToCoord = cell;
				HeadToCoord.Z = Map.Get_Height_GL(HeadToCoord);

				if (LinkedTo->PositionCoord.Z >= HeadToCoord.Z + 2 * LEVEL_LEPTON_H + LEVEL_LEPTON_H) {
					HeadToCoord.Z += BRIDGE_LEPTON_HEIGHT;
				}

				LinkedTo->Set_Occupy_Bit(HeadToCoord);

			} else {
				Move_To((CoordStruct const &)(Coord)cell);
			}
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00516FC0
boolean STDMETHODCALLTYPE HoverLocomotionClass::Shove(DirStruct dir)
{
	if (Push(dir)) {
		Do_Shove();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00517020
void HoverLocomotionClass::Do_Shove(void)
{
	WasShoved = true;
	ShoveAccum = Random_Pick(20, 30);
	if (Percent_Chance(50)) {
		ShoveAccum = -ShoveAccum;
	}
}

// FUNCTION: GAMEMD 0x00517070
HRESULT STDMETHODCALLTYPE HoverLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_HoverLocomotion;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x005170B0
HRESULT STDMETHODCALLTYPE HoverLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) HoverLocomotionClass(NoInitClass());
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00517100
LayerType STDMETHODCALLTYPE HoverLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: GAMEMD 0x005171C0
void STDMETHODCALLTYPE HoverLocomotionClass::Mark_All_Occupation_Bits(int mark)
{
	if (mark == MARK_UP) {
		Coord coord = Head_To_Coord();
		LinkedTo->Clear_Occupy_Bit(coord);
	}
}

// FUNCTION: GAMEMD 0x00517210
boolean STDMETHODCALLTYPE HoverLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	Coord coord = Head_To_Coord();

	if (coord.As_Cell() == ((Coord &)to).As_Cell() && abs(coord.Z - to.Z) <= LEVEL_LEPTON_H) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005172B0
// HoverLocomotionClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x005172C0
// ??_GHoverLocomotionClass@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x005172F0
// ??_GLocomotionClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x00517310
// ??8?$TPoint2D@H@@QBE_NABV0@@Z
template<class T>
bool TPoint2D<T>::operator == (TPoint2D<T> const & rvalue) const
{
	return(X==rvalue.X && Y==rvalue.Y);
}

template class TPoint2D<int>;

extern unsigned char DAT_00A8F1C4;
extern unsigned char DAT_00A8F1C5;
extern unsigned char DAT_00A8F1C6;

// FUNCTION: GAMEMD 0x00517340
void FUN_00517340(void)
{
	DAT_00A8F1C4 = 0;
	DAT_00A8F1C5 = 0;
	DAT_00A8F1C6 = 0;
}
