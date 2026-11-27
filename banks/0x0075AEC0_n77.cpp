// 86.6% raw, 96.6% normalised (4977/4988, 114 diff lines), no overlay; full copy of src/walk.cpp, unchanged from 0x0075AEC0.cpp: no twin difference beat it (Robin's tube form = 0x0075AEC0_mech_inline.cpp, 97.5 normalised; the rest neutral or lower)
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
#include "geometry.h"
#include "rect.h"
#include "layer.hh"
#include "fastmath.h"
#include "coord.h"
#include "slavemgr.h"
#include <new.h>
#include "cast.h"
#include "infantry.h"

// SYNTHETIC: GAMEMD 0x0075A880 SYMBOL
// _$E13
// SYNTHETIC: GAMEMD 0x0075A8A0 SYMBOL
// _$E97
// SYNTHETIC: GAMEMD 0x0075A8D0 SYMBOL
// _$E100
// SYNTHETIC: GAMEMD 0x0075A8F0 SYMBOL
// _$E103
// SYNTHETIC: GAMEMD 0x0075A910 SYMBOL
// _$E106
// SYNTHETIC: GAMEMD 0x0075A930 SYMBOL
// _$E109
// SYNTHETIC: GAMEMD 0x0075A950 SYMBOL
// _$E112
// SYNTHETIC: GAMEMD 0x0075A970 SYMBOL
// _$E115
// SYNTHETIC: GAMEMD 0x0075A9B0 SYMBOL
// _$E118
// SYNTHETIC: GAMEMD 0x0075A9E0 SYMBOL
// _$E121
// SYNTHETIC: GAMEMD 0x0075AA10 SYMBOL
// _$E124
// SYNTHETIC: GAMEMD 0x0075AA40 SYMBOL
// _$E127
// SYNTHETIC: GAMEMD 0x0075AA50 SYMBOL
// _$E130
// SYNTHETIC: GAMEMD 0x0075AA70 SYMBOL
// _$E133

// FUNCTION: GAMEMD 0x0075AA90
WalkLocomotionClass::WalkLocomotionClass(void) :
	BASECLASS(),
	DestinationCoord(COORD_NONE),
	HeadToCoord(COORD_NONE),
	IsMoving(false),
	IsProcessingMovement(false),
	IsReallyMoving(false)
{
}

// FUNCTION: GAMEMD 0x0075AB00
WalkLocomotionClass::~WalkLocomotionClass(void)
{
}

// FUNCTION: GAMEMD 0x0075AB30
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving(void)
{
	return(IsMoving);
}

// FUNCTION: GAMEMD 0x0075AB40
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving_Now(void)
{
	if (Is_Moving() && LinkedTo->Speed > 0 && HeadToCoord != COORD_NONE) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0075ABA0
CoordStruct STDMETHODCALLTYPE WalkLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(COORD_NONE);
}

// FUNCTION: GAMEMD 0x0075AC00
CoordStruct STDMETHODCALLTYPE WalkLocomotionClass::Head_To_Coord(void)
{
	if (HeadToCoord != COORD_NONE) {
		return(HeadToCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: GAMEMD 0x0075AC80
boolean STDMETHODCALLTYPE WalkLocomotionClass::Process(void)
{
	IsProcessingMovement = true;
	Movement_AI(true);
	IsProcessingMovement = false;
	return(Is_Moving());
}

// FUNCTION: GAMEMD 0x0075ACB0
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

// FUNCTION: GAMEMD 0x0075ADA0
void STDMETHODCALLTYPE WalkLocomotionClass::Stop_Moving(void)
{
	DestinationCoord = COORD_NONE;
	if (HeadToCoord == COORD_NONE) {
		IsMoving = false;
		IsReallyMoving = false;
		LinkedTo->vt_entry_54C();
	}
}

// FUNCTION: GAMEMD 0x0075AE00
void STDMETHODCALLTYPE WalkLocomotionClass::Do_Turn(DirStruct dir)
{
	LinkedTo->PrimaryFacing.Set(dir);
}

// FUNCTION: GAMEMD 0x0075AE30
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

// FUNCTION: GAMEMD 0x0075AEC0
void WalkLocomotionClass::Movement_AI(bool first_pass)
{
	if (HeadToCoord == COORD_NONE) {
		if (DestinationCoord != COORD_NONE) {
			if (LinkedTo->Path[0] == FACING_NONE) {
				if (LinkedTo->PathDelay != 0) {
					LinkedTo->Stop_Movement_Animation();
					return;
				}

				LinkedTo->PathDelay = Rule->PathDelay * TICKS_PER_MINUTE;

				if (!LinkedTo->Basic_Path(DestinationCoord.As_Cell(), 0, 0)) {
					IsReallyMoving = false;

					if (!LinkedTo->Is_In_Same_Zone(DestinationCoord)) {
						LinkedTo->Assign_Destination(NULL);
						return;
					}

					LinkedTo->On_Movement_Blocked();

					if (Distance(LinkedTo->Center_Coord(), DestinationCoord) < Rule->CloseEnoughDistance && !LinkedTo->IsTethered) {
						LinkedTo->Assign_Destination(NULL);
					} else {
						if (LinkedTo->TryTryAgain) {
							LinkedTo->TryTryAgain--;
						} else {
							if (LinkedTo->IsNewNavCom) {
								VocClass::PlayGlobal(Rule->ScoldSound, 1.0f, 0x2000);
							}
							LinkedTo->IsNewNavCom = false;

							Cell dest = DestinationCoord.As_Cell();

							if (LinkedTo->IsInPlayfield) {
								Cell nav_dest = LinkedTo->Destination_Coord().As_Cell();
								if (!Map.Is_Same_Cell_Zone(nav_dest, dest, LinkedTo->TClass->MZone, LinkedTo->Is_Moving_Onto_Bridge(), Map[dest].IsUnderBridge, LinkedTo->Is_Allowed_To_Leave_Map())) {
									LinkedTo->Assign_Destination(NULL);
								}
							}

							if (LinkedTo->TarCom != NULL) {
								Cell tar_dest = LinkedTo->TarCom->Destination_Coord(LinkedTo).As_Cell();
								if (LinkedTo->IsInPlayfield) {
									if (LinkedTo->TarCom != NULL) {
										Cell nav_dest = LinkedTo->Destination_Coord(LinkedTo).As_Cell();
										if (!Map.Is_Same_Cell_Zone(nav_dest, tar_dest, LinkedTo->TClass->MZone, LinkedTo->Is_Moving_Onto_Bridge(LinkedTo), Map[tar_dest].IsUnderBridge, LinkedTo->Is_Allowed_To_Leave_Map())) {
											LinkedTo->Assign_Target(NULL);
										}
									}
								}
							}
						}
					}

					LinkedTo->Set_Speed(0.0);
					Stop_Moving();
					return;
				}

				LinkedTo->TryTryAgain = FootClass::PATH_RETRY;
				if (LinkedTo->JumpJet_To_Walk()) {
					return;
				}
			}

			FacingType dir = (FacingType)LinkedTo->Path[0];
			if (dir == FACING_COUNT) {
				int tube = Map[LinkedTo->Get_Coord()].Tube;
				if (tube >= 0 && tube < Tubes.Count()) {
					LinkedTo->Mark(MARK_UP);
					LinkedTo->Clear_Occupy_Bit(LinkedTo->Get_Coord());

					TubeClass * tube_ptr = Tubes[tube];
					HeadToCoord = tube_ptr->Get_Exit().As_Coord();

					memcpy(&LinkedTo->Path[0], &LinkedTo->Path[1], sizeof(LinkedTo->Path[0]) * (ARRAY_SIZE(LinkedTo->Path) - 1));
					LinkedTo->Path[ARRAY_SIZE(LinkedTo->Path) - 1] = FACING_NONE;

					LinkedTo->CurrentTube = tube;
					LinkedTo->CurrentTubeDir = FACING_FIRST;

					Coord enter_coord(Cell(tube_ptr->Enter));
					LinkedTo->LastTubeCoord = LinkedTo->Get_Coord() + (Map[Adjacent_Cell(Cell(tube_ptr->Enter), tube_ptr->Dirs[0])].Cell_Coord() - enter_coord);

					int start_h = Map.Get_Height_GL(LinkedTo->Get_Coord());
					LinkedTo->LastTubeCoord.Z = start_h + (Map.Get_Height_GL(Coord(tube_ptr->Get_Exit(), 0)) - start_h) / tube_ptr->Get_Count();
					return;
				}

				LinkedTo->Path[0] = FACING_NONE;
				HeadToCoord = COORD_NONE;
				Stop_Moving();
				LinkedTo->Assign_Destination(NULL);
				return;
			} else {
				Coord coord = Adjacent_Cell(LinkedTo->Get_Coord(), dir);
				Cell cell = coord;

				CellClass & next = Map[Adjacent_Cell(LinkedTo->Get_Coord(), dir)];
				if (LinkedTo->IsOnBridge ^ next.IsUnderBridge) {
					LinkedTo->IsPlanningToLook = true;
				}

				MoveType move = LinkedTo->Can_Enter_Cell(&Map[cell], dir, LinkedTo->Get_Cell_Height(), NULL, true);
				if (move != MOVE_OK) {
					IsReallyMoving = false;
					LinkedTo->Stop_Movement_Animation();

					if (move == MOVE_TEMP) {
						CellClass & cellptr = Map[cell];
						if (first_pass) {
							LinkedTo->Path[0] = FACING_NONE;
							HeadToCoord = COORD_NONE;
							LinkedTo->PathDelay = 0;
							Movement_AI(false);
							return;
						}

						int dist = LinkedTo->Distance(DestinationCoord);
						if (dist < Rule->CloseEnoughDistance &&
							!LinkedTo->HasAnyLink() &&
							abs(DestinationCoord.Z - LinkedTo->Get_Coord().Z) < 2 * LEVEL_LEPTON_H &&
							Map[LinkedTo->Get_Coord()].Land_Type() != LAND_TUNNEL) {

							HeadToCoord = COORD_NONE;
							Stop_Moving();
							LinkedTo->Set_Speed(0.0);
							LinkedTo->Assign_Destination(NULL);
							LinkedTo->Path[0] = FACING_NONE;
						} else {
							bool isbridge = cellptr.IsUnderBridge && abs(LinkedTo->Get_Coord().Z / LEVEL_LEPTON_H - cellptr.Height) > 2;
							cellptr.Incoming(COORD_NONE, true, true, isbridge);
						}
						return;
					}

					if (move == MOVE_MOVING_BLOCK) {
						if (!LinkedTo->IsToPathAroundBlockage) {
							LinkedTo->IsToPathAroundBlockage = true;
							LinkedTo->BlockagePathDelay = Rule->BlockagePathDelay;
						}

						if (LinkedTo->PathDelay != 0) {
							return;
						}

						bool blocked = LinkedTo->IsToPathAroundBlockage && LinkedTo->BlockagePathDelay == 0;
						bool found = LinkedTo->Basic_Path(DestinationCoord.As_Cell(), 0, blocked ? 2 : 1);
						LinkedTo->PathDelay = Rule->PathDelay * TICKS_PER_MINUTE;

						if (!found) {
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

					if (move == MOVE_CLOSED_GATE) {
						Map.Try_Open_Gate(LinkedTo, cell);
						LinkedTo->TryTryAgain = FootClass::PATH_RETRY;
						return;
					}

					if (move == MOVE_DESTROYABLE || move == MOVE_FRIENDLY_DESTROYABLE) {
						ObjectClass * object = Map[cell].Cell_Object();
						if (first_pass) {
							LinkedTo->Path[0] = FACING_NONE;
							HeadToCoord = COORD_NONE;
							LinkedTo->PathDelay = 0;
							Movement_AI(false);
							return;
						}

						if (object != NULL) {
							if (!LinkedTo->House->Is_Ally(object)) {
								LinkedTo->Override_Mission(MISSION_ATTACK, object, NULL);
							}
						} else {
							if (Map[cell].Overlay != OVERLAY_NONE && OverlayTypes[Map[cell].Overlay]->IsWall) {
								LinkedTo->Override_Mission(MISSION_ATTACK, &Map[cell], NULL);
							}
						}
					}

					if (move != MOVE_CLOAK && move != MOVE_NO) {
						LinkedTo->Path[0] = FACING_NONE;
						LinkedTo->Set_Speed(0.0);
						Stop_Moving();
						LinkedTo->IsNewNavCom = false;
					} else {
						if (move == MOVE_CLOAK) {
							Map[cell].Shimmer();
						}

						if (!first_pass) return;

						LinkedTo->Path[0] = FACING_NONE;
						HeadToCoord = COORD_NONE;
						LinkedTo->PathDelay = 0;
						Movement_AI(false);
						return;
					}
				} else {
					if (Mark_Head_To(coord)) {
						IsReallyMoving = true;
						if (!LinkedTo->IsActive) return;

						DirStruct face = (DirStruct &)::Direction(LinkedTo->Center_Coord(), HeadToCoord);
						Do_Turn(face);
						LinkedTo->Set_Speed(1.0);
					} else {
						LinkedTo->Set_Speed(0.0);
					}
				}
			}
		} else {
			if (LinkedTo->Speed > 0.0) {
				LinkedTo->Set_Speed(0.0);
			}
		}
	} else {

		IsReallyMoving = true;

		if (Distance(Point2D(LinkedTo->Center_Coord()), Point2D(HeadToCoord)) < 17) {
			LinkedTo->Mark(MARK_UP);

			if (LinkedTo->Path[0] == FACING_NONE) {
				LinkedTo->Path[1] = FACING_NONE;
			}

			memcpy(&LinkedTo->Path[0], &LinkedTo->Path[1], sizeof(LinkedTo->Path[0]) * (ARRAY_SIZE(LinkedTo->Path) - 1));
			LinkedTo->Path[ARRAY_SIZE(LinkedTo->Path) - 1] = FACING_NONE;

			LinkedTo->Set_Coord(HeadToCoord);
			LinkedTo->LastPathingCell = HeadToCoord.As_Cell();
			LinkedTo->Set_Height_AGL(0);
			LinkedTo->IsToPathAroundBlockage = false;

			Mark_Head_To(COORD_NONE);

			if (LinkedTo->Path[0] == FACING_NONE) {
				LinkedTo->Set_Speed(0.0);
			}

			LinkedTo->Per_Cell_Process(PCP_END);

			if (!LinkedTo->IsActive) return;
			if (LinkedTo->IsInLimbo || LinkedTo->IsFalling) return;

			if (DestinationCoord == COORD_NONE
				|| (LinkedTo->Destination_Coord().As_Cell() == DestinationCoord.As_Cell()
					&& abs(LinkedTo->Destination_Coord().Z - DestinationCoord.Z) < 2 * LEVEL_LEPTON_H)) {
				LinkedTo->Assign_Destination(NULL);
				LinkedTo->Set_Speed(0.0);
				HeadToCoord = COORD_NONE;
				Stop_Moving();
				IsReallyMoving = false;
			}

			LinkedTo->Mark(MARK_DOWN);
		} else {
			if (LinkedTo->IsUnderEMP()) {
				IsReallyMoving = false;
			} else {
				LinkedTo->Set_Speed(1.0);
				int speed = LinkedTo->Current_Speed();
				LinkedTo->IsToPathAroundBlockage = false;

				DirType face = ::Direction(LinkedTo->Center_Coord(), HeadToCoord);
				Do_Turn((DirStruct &)face);

				Cell old_cell = LinkedTo->Get_Cell();
				Coord new_coord = Move_Coord(LinkedTo->Get_Coord(), face, speed);
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

					LinkedTo->Set_Height_AGL(0);
					LinkedTo->Mark(MARK_DOWN);
				} else {
					bool was_down = LinkedTo->IsDown;
					LinkedTo->IsDown = false;
					LinkedTo->Set_Coord(new_coord);
					LinkedTo->Set_Height_AGL(0);
					LinkedTo->IsDown = was_down;
				}

				if (moved) {
					Map[LinkedTo->Get_Coord()].Trigger_Veins();
				}
			}
		}
	}
	LinkedTo->IsNewNavCom = false;
}

// FUNCTION: GAMEMD 0x0075C240
bool WalkLocomotionClass::Mark_Head_To(Coord const & coord)
{
	if (HeadToCoord != COORD_NONE) {
		LinkedTo->Clear_Occupy_Bit(HeadToCoord);
	} else {
		LinkedTo->Clear_Occupy_Bit(LinkedTo->PositionCoord);
	}

	Coord crd = coord;
	bool any_spot = false;

	if (coord != COORD_NONE) {
		if (LinkedTo->Get_Mission() == MISSION_CAPTURE || LinkedTo->Get_Mission() == MISSION_EATEN || LinkedTo->Get_Mission() == MISSION_ENTER || LinkedTo->Get_Mission() == MISSION_GUARD_AREA || LinkedTo->Get_Mission() == MISSION_PATROL) {
			UnitClass * unit = (UnitClass *)LinkedTo->NavCom;
			unit = (unit != NULL && ((AbstractClass *)unit)->Fetch_RTTI() == RTTI_UNIT) ? unit : NULL;
			if (unit != NULL && unit->PositionCell == coord.As_Cell()) {
				any_spot = true;
			}
			AircraftClass * aircraft = (AircraftClass *)LinkedTo->NavCom;
			aircraft = (aircraft != NULL && ((AbstractClass *)aircraft)->Fetch_RTTI() == RTTI_AIRCRAFT) ? aircraft : NULL;
			if (aircraft != NULL && aircraft->PositionCell == coord.As_Cell()) {
				any_spot = true;
			}
			BuildingClass * building = (BuildingClass *)LinkedTo->NavCom;
			building = (building != NULL && ((AbstractClass *)building)->Fetch_RTTI() == RTTI_BUILDING) ? building : NULL;
			if (building != NULL && Map[coord].Cell_Building() == building) {
				any_spot = true;
			}
		}

		if (LinkedTo->SlaveOwner != NULL && LinkedTo->SlaveOwner->SlaveManager != NULL) {
			if (Map[coord].Cell_Techno() == LinkedTo->SlaveOwner) {
				InfantryClass * infantry = As_Infantry(LinkedTo);
				if (LinkedTo->SlaveOwner->SlaveManager->FUN_006B0880(infantry, &Map[coord])) {
					any_spot = true;
				}
			}
		}

		bool isbridge = Map[crd].IsUnderBridge && LinkedTo->PositionCoord.Z > 3 * LEVEL_LEPTON_H + Map.Get_Height_GL(crd);
		HeadToCoord = Map[crd].Closest_Free_Spot(crd, any_spot, isbridge);

		if (!Map[HeadToCoord].Goodie_Check(LinkedTo) && !LinkedTo->IsInLimbo) {
			HeadToCoord = COORD_NONE;
			if (!LinkedTo->IsActive) {
				return(false);
			}
		}
	} else {
		HeadToCoord = coord;
	}

	if (HeadToCoord != COORD_NONE) {
		LinkedTo->Set_Occupy_Bit(HeadToCoord);
		return(true);
	}

	LinkedTo->Set_Occupy_Bit(LinkedTo->PositionCoord);
	return(false);
}

// FUNCTION: GAMEMD 0x0075C640
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WalkLocomotion;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0075C680
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) WalkLocomotionClass(NoInitClass());

		bool haspiggy;
		stream->Read(&haspiggy, sizeof(haspiggy), NULL);

		if (haspiggy) {
			result = OleLoadFromStream(stream, IID_ILocomotion, (void**)&Piggybacker);
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0075C700
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::Save(IStream * stream, BOOL cleardirty)
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

// FUNCTION: GAMEMD 0x0075C7E0
LayerType STDMETHODCALLTYPE WalkLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: GAMEMD 0x0075C7F0
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
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

// FUNCTION: GAMEMD 0x0075C850
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

// FUNCTION: GAMEMD 0x0075C8A0
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

// FUNCTION: GAMEMD 0x0075C8E0
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Ok_To_End(void)
{
	if (!Is_Moving() && Piggybacker != NULL && !IsProcessingMovement && !LinkedTo->IsAttackedByLocomotor) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0075C920
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::Piggyback_CLSID(GUID * classid)
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

// FUNCTION: GAMEMD 0x0075CA30
void STDMETHODCALLTYPE WalkLocomotionClass::Mark_All_Occupation_Bits(int mark)
{
	if (mark == MARK_UP) {
		LinkedTo->Clear_Occupy_Bit((Coord)Head_To_Coord());
	}
}

// FUNCTION: GAMEMD 0x0075CA80
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	Coord headto = Head_To_Coord();
	if (headto.As_Cell() == Coord(to).As_Cell() && abs(headto.Z - to.Z) <= LEVEL_LEPTON_H) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0075CB20
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Really_Moving_Now(void)
{
	return(IsReallyMoving);
}

// FUNCTION: GAMEMD 0x0075CB30
void STDMETHODCALLTYPE WalkLocomotionClass::Limbo(void)
{
	DestinationCoord = COORD_NONE;
	HeadToCoord = COORD_NONE;
}

// FUNCTION: GAMEMD 0x0075CB80
// WalkLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x0075CB90
// WalkLocomotionClass::Release

// FUNCTION: GAMEMD 0x0075CBA0
// WalkLocomotionClass::Is_Piggybacking

// FUNCTION: GAMEMD 0x0075CBC0
// WalkLocomotionClass::Stop_Movement_Animation

// FUNCTION: GAMEMD 0x0075CBD0
// WalkLocomotionClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x0075CBE0
// ??_GWalkLocomotionClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x0075CC30
// WalkLocomotionClass::QueryInterface

// FUNCTION: GAMEMD 0x0075CC40
// WalkLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x0075CC50
// WalkLocomotionClass::Release

// FUNCTION: GAMEMD 0x0075CC60
// WalkLocomotionClass::QueryInterface

// FUNCTION: GAMEMD 0x0075CC70
// WalkLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x0075CC80
// WalkLocomotionClass::Release
