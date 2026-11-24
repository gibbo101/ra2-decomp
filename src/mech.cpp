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

// FUNCTION: GAMEMD 0x005B01C0
void MechLocomotionClass::Movement_AI(bool continue_moving)
{
	if (LinkedTo->PrimaryFacing.Is_Rotating()) {
		return;
	}

	if (HeadToCoord == COORD_NONE) {
		if (DestinationCoord != COORD_NONE) {

			if (LinkedTo->Path[0] == FACING_NONE) {

				if (LinkedTo->PathDelay != 0) {
					LinkedTo->Stop_Movement_Animation();
					return;
				}

				LinkedTo->PathDelay = Rule->PathDelay * TICKS_PER_MINUTE;

				if (!LinkedTo->Basic_Path(DestinationCoord.As_Cell(), 0, 0)) {

					if (!LinkedTo->Is_In_Same_Zone(DestinationCoord)) {
						LinkedTo->Assign_Destination(NULL);
						return;
					}

					int dist = Distance(LinkedTo->Center_Coord(), DestinationCoord);

					if (dist < (int)Rule->CloseEnoughDistance && !LinkedTo->IsTethered) {
						LinkedTo->Assign_Destination(NULL);
					} else {

						if (LinkedTo->TryTryAgain != 0) {
							LinkedTo->TryTryAgain--;
						} else {

							if (LinkedTo->IsNewNavCom) {
								VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
							}
							LinkedTo->IsNewNavCom = false;

							Cell dest_cell = DestinationCoord;
							if (LinkedTo->IsInPlayfield) {
								Cell src_cell = LinkedTo->Destination_Coord().As_Cell();
								if (!Map.Is_Same_Cell_Zone(src_cell, dest_cell, LinkedTo->TClass->MZone, LinkedTo->Is_Moving_Onto_Bridge(), Map[dest_cell].IsUnderBridge, LinkedTo->Is_Allowed_To_Leave_Map())) {
									LinkedTo->Assign_Destination(NULL);
								}
							}

							if (LinkedTo->TarCom != NULL) {
								Cell tar_cell = LinkedTo->TarCom->Destination_Coord(LinkedTo).As_Cell();
								if (LinkedTo->IsInPlayfield) {
									Cell src_cell = LinkedTo->Destination_Coord(LinkedTo).As_Cell();
									if (!Map.Is_Same_Cell_Zone(src_cell, tar_cell, LinkedTo->TClass->MZone, LinkedTo->Is_Moving_Onto_Bridge(LinkedTo), Map[tar_cell].IsUnderBridge, LinkedTo->Is_Allowed_To_Leave_Map())) {
										LinkedTo->Assign_Target(NULL);
									}
								}
							}
						}
					}

					LinkedTo->Set_Speed(0);
					Stop_Moving();
					IsMoving = false;
					return;
				}

				LinkedTo->TryTryAgain = FootClass::PATH_RETRY;
			}

			FacingType dir = LinkedTo->Path[0];
			if (dir == FACING_COUNT) {

				int tube = Map[LinkedTo->Get_Coord()].Tube;
				if (tube >= 0 && tube < Tubes.Count()) {
					LinkedTo->Mark(MARK_UP);
					LinkedTo->Clear_Occupy_Bit(LinkedTo->Get_Coord());

					TubeClass * tubeptr = Tubes[tube];

					Cell exit = tubeptr->Get_Exit();
					HeadToCoord = exit.As_Coord();

					memcpy(&LinkedTo->Path[0], &LinkedTo->Path[1], sizeof(LinkedTo->Path[0]) * (CONQUER_PATH_MAX - 1));
					LinkedTo->Path[CONQUER_PATH_MAX - 1] = FACING_NONE;

					LinkedTo->CurrentTube = tube;
					LinkedTo->CurrentTubeDir = FACING_FIRST;

					Coord enter = (Cell)tubeptr->Enter;
					Coord offset = Map[Adjacent_Cell((Cell)tubeptr->Enter, tubeptr->Dirs[0])].Cell_Coord() - enter;
					LinkedTo->LastTubeCoord = LinkedTo->PositionCoord + offset;

					int start_h = Map.Get_Height_GL(LinkedTo->PositionCoord);
					LinkedTo->LastTubeCoord.Z = start_h + (Map.Get_Height_GL(tubeptr->Get_Exit()) - start_h) / tubeptr->Get_Count();
					return;
				}

				LinkedTo->Path[0] = FACING_NONE;
				HeadToCoord = COORD_NONE;
				Stop_Moving();
				LinkedTo->Assign_Destination(NULL);
				return;
			} else {
				Cell what = Adjacent_Cell(LinkedTo->Get_Coord().As_Cell(), dir);

				Coord head_coord = Coord(what, 0);
				head_coord.Z = Map.Get_Height_GL(head_coord);

				CellClass * what_cell = &Map[what];

				if (LinkedTo->IsOnBridge ^ what_cell->IsUnderBridge) {
					LinkedTo->IsPlanningToLook = true;
				}

				MoveType can_enter = LinkedTo->Can_Enter_Cell(&Map[what], dir, LinkedTo->Get_Cell_Height(), 0, true);

				if (((can_enter != MOVE_DESTROYABLE && can_enter != MOVE_FRIENDLY_DESTROYABLE) || !LinkedTo->TClass->IsCrusher || what_cell->Overlay != OVERLAY_FIRST) && can_enter != MOVE_OK) {

					LinkedTo->Stop_Movement_Animation();

					if (can_enter == MOVE_TEMP) {

						CellClass * cellptr = &Map[what];

						if (continue_moving) {
							LinkedTo->Path[0] = FACING_NONE;
							HeadToCoord = COORD_NONE;
							LinkedTo->PathDelay = 0;
							Movement_AI(false);
							return;
						}

						int dist = LinkedTo->Distance(DestinationCoord);

						if (dist < (int)Rule->CloseEnoughDistance && !LinkedTo->HasAnyLink()
							&& abs(DestinationCoord.Z - LinkedTo->Get_Coord().Z) < 2 * LEVEL_LEPTON_H
							&& Map[LinkedTo->Get_Coord()].Land_Type() != LAND_TUNNEL) {

							LinkedTo->Set_Speed(0);
							LinkedTo->Assign_Destination(NULL);
							LinkedTo->Path[0] = FACING_NONE;
							Stop_Moving();
							IsMoving = false;
						} else {

							bool bridge = cellptr->IsUnderBridge && abs(LinkedTo->Get_Coord().Z / LEVEL_LEPTON_H - cellptr->Height) > 2;
							cellptr->Incoming(COORD_NONE, true, true, bridge);
						}
						return;
					}

					if (can_enter == MOVE_MOVING_BLOCK) {

						if (!LinkedTo->IsToPathAroundBlockage) {
							LinkedTo->IsToPathAroundBlockage = true;
							LinkedTo->BlockagePathDelay = Rule->BlockagePathDelay;
						}

						if (LinkedTo->PathDelay == 0) {

							bool blockage_expired = LinkedTo->IsToPathAroundBlockage && LinkedTo->BlockagePathDelay == 0;

							bool path_ok = LinkedTo->Basic_Path(DestinationCoord.As_Cell(), 0, blockage_expired ? 2 : 1);
							LinkedTo->PathDelay = Rule->PathDelay * TICKS_PER_MINUTE;

							if (!path_ok) {
								if (!LinkedTo->Is_In_Same_Zone(DestinationCoord)) {
									LinkedTo->Assign_Destination(NULL);
								} else {
									LinkedTo->On_Movement_Blocked();
								}
							} else {
								LinkedTo->JumpJet_To_Walk();
							}
							return;
						}

						DirStruct face = (DirStruct &)::Direction(LinkedTo->Center_Coord(), head_coord);
						Do_Turn(face);
						return;
					}

					if (can_enter == MOVE_CLOSED_GATE) {
						Map.Try_Open_Gate(LinkedTo, what);
						LinkedTo->TryTryAgain = FootClass::PATH_RETRY;
						return;
					}

					if (can_enter != MOVE_CLOAK && can_enter != MOVE_NO) {
						if (can_enter == MOVE_DESTROYABLE || can_enter == MOVE_FRIENDLY_DESTROYABLE) {

							ObjectClass * obj = Map[what].Cell_Object(Point2D(0, 0), false);

							if (continue_moving) {
								LinkedTo->Path[0] = FACING_NONE;
								HeadToCoord = COORD_NONE;
								LinkedTo->PathDelay = 0;
								Movement_AI(false);
								return;
							}

							if (obj != NULL) {
								if (!LinkedTo->House->Is_Ally(obj)) {
									LinkedTo->Override_Mission(MISSION_ATTACK, obj, 0);
								}
							} else {
								if (Map[what].Overlay != OVERLAY_NONE && OverlayTypes[Map[what].Overlay]->IsWall) {
									LinkedTo->Override_Mission(MISSION_ATTACK, &Map[what], 0);
								}
							}
						}

						LinkedTo->Path[0] = FACING_NONE;
						LinkedTo->Set_Speed(0);
						Stop_Moving();
						LinkedTo->IsNewNavCom = false;
					} else {
						if (can_enter == MOVE_CLOAK) {
							Map[what].Shimmer();
						}

						if (!continue_moving) return;

						LinkedTo->Path[0] = FACING_NONE;
						HeadToCoord = COORD_NONE;
						LinkedTo->PathDelay = 0;
						Movement_AI(false);
						return;
					}
				} else {

					if (Mark_Head_To(head_coord)) {

						if (LinkedTo->Path[0] != FACING_NONE && LinkedTo->Path[0] != FACING_COUNT) {
							Cell cell = Adjacent_Cell(LinkedTo->Get_Cell(), LinkedTo->Path[0]);
							CellClass * cellptr = &Map[cell];
							if (cellptr->Overlay != OVERLAY_NONE) {
								OverlayTypeClass * otype = OverlayTypes[cellptr->Overlay];
								if ((LinkedTo->TClass->IsCrusher || LinkedTo->Has_Ability(ABILITY_CRUSHER)) && otype->HeapID == OVERLAY_SANDBAG_WALL) {
									LinkedTo->IsCrushing = true;
									if (LinkedTo->TClass->IsTiltsWhenCrushes) {
										LinkedTo->RockingForwardsPerFrame = -0.01f;
									}
								}
							}
						}

						if (!LinkedTo->IsActive) return;

						DirStruct face = (DirStruct &)::Direction(LinkedTo->Center_Coord(), HeadToCoord);
						Do_Turn(face);

						if (LinkedTo->PrimaryFacing.Is_Rotating()) return;

						LinkedTo->Set_Speed(1.0);
					} else {
						LinkedTo->Set_Speed(0);
					}
				}
			}

		} else {

			if (LinkedTo->Speed > 0.0) {
				LinkedTo->Set_Speed(0.0);
			}
		}
	} else {

		if (Distance(Point2D(LinkedTo->Center_Coord()), Point2D(HeadToCoord)) < 16) {

			LinkedTo->Mark(MARK_UP);

			if (LinkedTo->Path[0] == FACING_NONE) {
				LinkedTo->Path[1] = FACING_NONE;
			}

			memcpy(&LinkedTo->Path[0], &LinkedTo->Path[1], sizeof(LinkedTo->Path[0]) * (CONQUER_PATH_MAX - 1));
			LinkedTo->Path[CONQUER_PATH_MAX - 1] = FACING_NONE;

			LinkedTo->Set_Coord(HeadToCoord);
			LinkedTo->LastPathingCell = HeadToCoord.As_Cell();
			LinkedTo->HeightAGL = 0;

			Mark_Head_To(COORD_NONE);

			if (LinkedTo->Path[0] == FACING_NONE) {
				LinkedTo->Set_Speed(0);
			}

			LinkedTo->IsOccupyingCell = true;

			LinkedTo->IsToPathAroundBlockage = false;

			if (DestinationCoord == COORD_NONE
				|| (LinkedTo->Get_Cell() == DestinationCoord
					&& abs(LinkedTo->Destination_Coord().Z - DestinationCoord.Z) < 2 * LEVEL_LEPTON_H)) {

				LinkedTo->Assign_Destination(NULL);
				LinkedTo->Set_Speed(0.0);
				Stop_Moving();
				IsMoving = false;
			}

			LinkedTo->Per_Cell_Process(PCP_END);

			if (!LinkedTo->IsActive) return;
			if (LinkedTo->IsInLimbo || LinkedTo->IsFalling) return;

			if (LinkedTo->Path[0] != FACING_NONE && LinkedTo->Path[0] != FACING_COUNT) {
				Cell check_coord = Adjacent_Cell(LinkedTo->Get_Cell(), LinkedTo->Path[0]);

				CellClass * cellptr = &Map[check_coord];
				if (cellptr->Overlay != OVERLAY_NONE) {
					OverlayTypeClass * otype = OverlayTypes[cellptr->Overlay];
					if ((LinkedTo->TClass->IsCrusher || LinkedTo->Has_Ability(ABILITY_CRUSHER)) && otype->HeapID == OVERLAY_SANDBAG_WALL) {
						LinkedTo->IsCrushing = true;

						if (LinkedTo->TClass->IsTiltsWhenCrushes) {
							LinkedTo->RockingForwardsPerFrame = -0.01f;
						}
					}
				}
			}

			if (!LinkedTo->IsActive || LinkedTo->IsInLimbo) return;

			LinkedTo->Mark(MARK_DOWN);
		} else {

			LinkedTo->Set_Speed(1.0);

			int speed = LinkedTo->Current_Speed();

			DirType desired_facing = ::Direction(LinkedTo->Center_Coord(), HeadToCoord);
			Do_Turn((DirStruct &)desired_facing);

			if (LinkedTo->PrimaryFacing.Is_Rotating()) {
				return;
			}

			if (LinkedTo->Occupies_Cells()) {
				LinkedTo->Clear_Occupy_Bit(LinkedTo->Get_Coord());
				LinkedTo->IsOccupyingCell = false;
				LinkedTo->IsToPathAroundBlockage = false;
			}

			Cell old_cell = LinkedTo->Get_Cell();

			Coord new_coord = Move_Coord(LinkedTo->Get_Coord(), desired_facing, speed);
			Cell new_cell = new_coord;

			bool moved = (old_cell != new_cell);
			if (moved) {
				LinkedTo->Mark(MARK_UP);
				LinkedTo->Set_Coord(new_coord);

				CellClass * old_cellptr = &Map[old_cell];
				CellClass * new_cellptr = &Map[new_cell];

				if (new_cellptr->Height == old_cellptr->Height - BRIDGE_CELL_HEIGHT && new_cellptr->IsUnderBridge) {
					LinkedTo->IsOnBridge = true;
				}
				if (!new_cellptr->IsUnderBridge && old_cellptr->IsUnderBridge) {
					LinkedTo->IsOnBridge = false;
				}

				LinkedTo->HeightAGL = 0;
				LinkedTo->Mark(MARK_DOWN);
			} else {
				bool was_down = LinkedTo->IsDown;
				LinkedTo->IsDown = false;
				LinkedTo->Set_Coord(new_coord);
				LinkedTo->HeightAGL = 0;
				LinkedTo->IsDown = was_down;
			}

			if (moved) {
				Map[LinkedTo->Get_Coord()].Trigger_Veins();
			}
		}
	}
	LinkedTo->IsNewNavCom = false;
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
