// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "vein.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "astar.h"
#include "ccrand.h"
#include "cell.h"
#include "draw.h"
#include "dsurface.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "inline.h"
#include "isotype.h"
#include "lightcon.h"
#include "mixfile.h"
#include "mouse.h"
#include "overtype.h"
#include "particle.h"
#include "partsys.h"
#include "ptype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "geometry.h"
#include "terrtype.h"
#include "tracker.h"
#include "vector.h"
#include "ramp.hh"
#include <cstdio>
#include <math.h>

// SYNTHETIC: GAMEMD 0x0074C170 SYMBOL
// _$E2

static bool * GlobalGrowthState = NULL;

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};
void const * VeinholeMonsterClass::MonsterShape = NULL;

inline VeinholeLogic::VeinholeLogic(bool) :
	GrowthCount(0),
	GrowthQueue(NULL),
	GrowthNodes(NULL),
	GrowthTimer(Rule->VeinholeGrowthRate),
	GrowthState(NULL)
{
}


// SYNTHETIC: GAMEMD 0x0074C190 SYMBOL
// _$E158
// SYNTHETIC: GAMEMD 0x0074C1C0 SYMBOL
// _$E161
// SYNTHETIC: GAMEMD 0x0074C1E0 SYMBOL
// _$E164
// SYNTHETIC: GAMEMD 0x0074C200 SYMBOL
// _$E167
// SYNTHETIC: GAMEMD 0x0074C220 SYMBOL
// _$E170
// SYNTHETIC: GAMEMD 0x0074C240 SYMBOL
// _$E173
// SYNTHETIC: GAMEMD 0x0074C260 SYMBOL
// _$E176
// SYNTHETIC: GAMEMD 0x0074C2A0 SYMBOL
// _$E179
// SYNTHETIC: GAMEMD 0x0074C2D0 SYMBOL
// _$E182
// SYNTHETIC: GAMEMD 0x0074C300 SYMBOL
// _$E185
// SYNTHETIC: GAMEMD 0x0074C330 SYMBOL
// _$E188
// SYNTHETIC: GAMEMD 0x0074C340 SYMBOL
// _$E191
// SYNTHETIC: GAMEMD 0x0074C360 SYMBOL
// _$E194

// SYNTHETIC: GAMEMD 0x0074C3C0 SYMBOL
// _$E197
// SYNTHETIC: GAMEMD 0x0074C380 SYMBOL
// _$E200
DynamicVectorClass<VeinholeMonsterClass *> VeinholeMonsterClass::VeinholeMonsters;


// FUNCTION: GAMEMD 0x0074C420
VeinholeMonsterClass::VeinholeMonsterClass(NoInitClass const & x) :
	BASECLASS(x),
	Control(x),
	LogicTimer(x)
{
	VeinholeMonsters.Add(this);
	GrowthLogic.GrowthNodes = new CellNode[Rule->MaxVeinholeGrowth];
	GrowthLogic.GrowthQueue = new PriorityQueueClass<CellNode>(Rule->MaxVeinholeGrowth);
	GrowthLogic.GrowthQueue->Clear();
	if (GlobalGrowthState == NULL) {
		GlobalGrowthState = new bool [Map_Cell_Count()];
	}
	GrowthLogic.GrowthState = new bool [Map_Cell_Count()];
}

// FUNCTION: GAMEMD 0x0074C5B0
VeinholeMonsterClass::VeinholeMonsterClass(Cell const & cell) :
	BASECLASS(),
	GrowthLogic(true),
	CurrentState(IDLE),
	DesiredState(IDLE),
	Control(),
	LogicTimer(0),
	CellID(cell),
	ShapeFrame(0),
	IsDead(NULL),
	IsToPuffGas(NULL),
	VeinCount(0)
{
	Create_ID();

	if (Can_Monster_Go_Here(cell)) {
		VeinholeMonsters.Add(this);

		if (!ScenarioInit) {
			static char _ramps[3][3] = {
				{ RAMP_MID_SE, RAMP_SOUTH, RAMP_MID_SW },
				{ RAMP_EAST,   RAMP_NONE,  RAMP_WEST   },
				{ RAMP_MID_NE, RAMP_NORTH, RAMP_MID_NW },
			};
			for (int y = -1; y <= 1; y++) {
				for (int x = -1; x <= 1; x++) {
					CellClass * cellptr = &Map[cell + Cell(x, y)];
					cellptr->Ramp = _ramps[y + 1][x + 1];
					if (cellptr->Ramp > RAMP_NONE) {
						cellptr->ITType = IsometricTileType(cellptr->Ramp + IsometricTileTypeClass::RampStart - 1);
					} else {
						cellptr->ITType = ISOTILE_CLEAR;
					}
					cellptr->SubTile = 0;
					cellptr->Height--;
				}
			}
		}

		Control.Set_Rate(4);
		Control.Set_Step(0);
		Control.Set_Stage(-1);

		GrowthLogic.GrowthNodes = new CellNode[Rule->MaxVeinholeGrowth];
		GrowthLogic.GrowthQueue = new PriorityQueueClass<CellNode>(Rule->MaxVeinholeGrowth);
		GrowthLogic.GrowthQueue->Clear();

		if (GlobalGrowthState == NULL) {
			GlobalGrowthState = new bool[Map_Cell_Count()];
		}
		GrowthLogic.GrowthState = new bool[Map_Cell_Count()];

		Coord coord = cell.As_Coord();
		coord.Z = Map.Get_Height_GL(coord);
		Unlimbo(coord);

		Strength = Rule->VeinholeTypeClass->MaxStrength;

		TargetTracker.Add_Index(Fetch_ID(), this);
	}
}

static inline void Clear_Growth_Data(VeinholeMonsterClass * monster)
{
	if (monster->GrowthLogic.GrowthQueue) {
		monster->GrowthLogic.GrowthQueue->Clear();
		delete monster->GrowthLogic.GrowthQueue;
		monster->GrowthLogic.GrowthQueue = NULL;
	}

	if (monster->GrowthLogic.GrowthNodes) {
		delete monster->GrowthLogic.GrowthNodes;
		monster->GrowthLogic.GrowthNodes = NULL;
	}

	if (monster->GrowthLogic.GrowthState) {
		delete monster->GrowthLogic.GrowthState;
		monster->GrowthLogic.GrowthState = NULL;
	}

	monster->GrowthLogic.GrowthCount = 0;
}

// FUNCTION: GAMEMD 0x0074C9F0
VeinholeMonsterClass::~VeinholeMonsterClass(void)
{
	for (int x = -2; x <= 2; x++) {
		for (int y = -2; y <= 2; y++) {
			Cell cell(CellID + Cell(x,y));

			int cellid = Map_Cell_Index(cell);
			if (cellid >= 0 && cellid < Map_Cell_Count()) {
				GlobalGrowthState[cellid] = false;
				GrowthLogic.GrowthState[cellid] = false;
			}

			CellClass * cellptr = &Map[cell];
			if (cellptr->Overlay == OVERLAY_VEINS || (cellptr->Overlay != OVERLAY_NONE && OverlayTypes[cellptr->Overlay]->IsVeins)) {
				Map.Radar_Background(cellptr->CellID);
				Point2D point(DSurface::ViewBounds.Top_Left());
				TacticalMap->Register_Dirty_Area(Union(cellptr->Overlay_Render_Rect(), cellptr->Overlay_Shadow_Render_Rect()) - point);
				cellptr->Overlay = OVERLAY_NONE;
				cellptr->OverlayData = 0;
			}
		}
	}

	IsInLimbo = true;
	IsDown = false;

	if (Strength > 0) {
		Detach_This_From_All(this);
	}

	Clear_Growth_Data(this);

	VeinholeMonsters.Delete(this);
	TargetTracker.Remove_Index(Fetch_ID());
}

// FUNCTION: GAMEMD 0x0074CD60
VeinholeMonsterClass * VeinholeMonsterClass::Get_Monster_At(Cell const & cell)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		if (VeinholeMonsters[i]->CellID == cell) {
			return(VeinholeMonsters[i]);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0074CDB0
VeinholeMonsterClass * VeinholeMonsterClass::Get_Vein_Owner_At(Cell const & cell)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		if (VeinholeMonsters[i]->GrowthLogic.GrowthState[Map_Cell_Index(cell)]) {
			return(VeinholeMonsters[i]);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0074CDF0
void VeinholeMonsterClass::Update_All(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		VeinholeMonsters[i]->AI();
	}
	int j;
	for (j = VeinholeMonsters.Count() - 1; j >= 0; j--) {
		if (VeinholeMonsters[j]->IsDead && VeinholeMonsters[j]->GrowthLogic.GrowthQueue->Count() == 0) {
			delete VeinholeMonsters[j];
		}
	}
}

static inline void Halt_Stage(StageClass & stage)
{
	stage.Set_Step(0);
}

// FUNCTION: GAMEMD 0x0074DC00
void VeinholeMonsterClass::Shrink(void)
{
	if (GrowthLogic.GrowthQueue != NULL) {
		int index = 0;
		int count = abs(Scen->RandomNumber()) % 4 + 1;
		CellNode * node = GrowthLogic.GrowthQueue->Extract_Min();

		while (index < count && node != NULL) {
			Reduce_Veins_At(&Map[Map[node->Element].CellID]);
			index++;
			if (index < count) {
				node = GrowthLogic.GrowthQueue->Extract_Min();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0074CE50
void VeinholeMonsterClass::AI(void)
{
	static int _state_to_mouth_frame[STATE_COUNT][FRAME_COUNT + 1] = {{1, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0}, {4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1, 8, 9, 10, 11, 11, 11, 11, 11, 10, 9, 8, 1}, {12, 11, 10, 9, 8, 1, 0, 0, 0, 0, 0, 0, 0}};
	static int _state_transition_stage[STATE_COUNT] = {-1, 4, -1, -1};
	static int _state_loop_stage[STATE_COUNT] = {8, 4, 13, 6};
	static int _state_entry_step[STATE_COUNT] = {0, -1, -1, -1};
	static int _state_entry_stage[STATE_COUNT] = {-1, 3, 12, 5};
	static bool _unused_bools[FRAME_COUNT] = {true, true, false, false, false, false, false, true, false, false, false, true};

	Control.Graphic_Logic();

	switch (CurrentState) {
	case IDLE:
		if (LogicTimer != 0) {
			if (DesiredState != DYING) {
				DesiredState = ATTACKING;
			}
		} else {
			if (DesiredState != DYING) {
				if (Map.Is_Something_Nearby(CellID, 2)) {
					DesiredState = ALERT;
				} else if (Control.Fetch_Stage() == _state_transition_stage[CurrentState]) {
					if ((abs(Scen->RandomNumber()) % 10000) * 0.0001 < 0.004) {
						Control.Set_Stage(_state_loop_stage[CurrentState] - 1);
						Control.Set_Step(-1);
					}
				}
			}
		}
		break;

	case ALERT:
		if (LogicTimer != 0) {
			if (DesiredState != DYING) {
				DesiredState = ATTACKING;
			}
			Control.Set_Step(1);
			if (Control.Fetch_Stage() < 0) {
				Control.Set_Stage(0);
			}
		} else if (!Map.Is_Something_Nearby(CellID, 2) && DesiredState == ALERT) {
			DesiredState = IDLE;
			Control.Set_Step(1);
			if (Control.Fetch_Stage() < 0) {
				Control.Set_Stage(0);
			}
		}
		break;

	case ATTACKING:
		if (Control.Fetch_Stage() >= 0 && _state_to_mouth_frame[CurrentState][Control.Fetch_Stage()] == 11) {
			if (!IsToPuffGas) {
				Coord coord = Coord(CellID, Map.Get_Height_GL(CellID) + 400);
				ParticleClass * particle = GasSystem->Spawn_Particle(ParticleTypes[ParticleTypeClass::From_Name("GasCloudM1")], coord);
				particle->GasDrift.Z = 12;
				double angle = Random_Double(0.0, 1.0) * DEG_TO_RAD(360);
				particle->GasDrift.X = fastmath::cos(angle) * 8;
				particle->GasDrift.Y = fastmath::sin(angle) * 8;
				IsToPuffGas = true;
			}
		} else {
			IsToPuffGas = false;
		}
		if (LogicTimer != 0 || IsToPuffGas) {
			if (Control.Fetch_Stage() == _state_transition_stage[CurrentState]) {
				if (Random_Double(0.0, 1.0) < 0.1) {
					Control.Set_Stage(_state_loop_stage[CurrentState] - 1);
					Control.Set_Step(-1);
				}
			}
		} else {
			if (DesiredState != DYING) {
				DesiredState = IDLE;
			}
			Control.Set_Step(-1);
		}
		break;

	case DYING:
		if (Control.Fetch_Stage() == _state_transition_stage[CurrentState] && !IsDead) {
			Destroy_Monster();
		}
		break;
	}

	if (GrowthLogic.GrowthTimer == 0) {
		if (!IsDead) {
			Grow();
			GrowthLogic.GrowthTimer = Rule->VeinholeGrowthRate;
		} else {
			Shrink();
			GrowthLogic.GrowthTimer = Rule->VeinholeShrinkRate;
		}
	}

	if (!IsDead) {
		if (Control.Fetch_Stage() == _state_transition_stage[CurrentState]) {
			if (DesiredState != CurrentState) {
				CurrentState = DesiredState;
				Control.Set_Step(_state_entry_step[CurrentState]);
				Control.Set_Stage(_state_entry_stage[CurrentState]);
			} else {
				Halt_Stage(Control);
			}
		} else if (Control.Fetch_Stage() != -1) {
			ShapeFrame = _state_to_mouth_frame[CurrentState][Control.Fetch_Stage()] - 1;
		} else {
			Halt_Stage(Control);
		}
	}
}

// FUNCTION: GAMEMD 0x0074D430
void VeinholeMonsterClass::Draw_All(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		VeinholeMonsters[i]->Draw_It();
	}
}

// FUNCTION: GAMEMD 0x0074D450
void VeinholeMonsterClass::Init(TheaterType theater)
{
	char buffer[32];
	sprintf(buffer, "VEINHOLE.%s", Theaters[theater].Suffix);
	MonsterShape = FileSystem::LoadFile(buffer, false);
}

// FUNCTION: GAMEMD 0x0074D490
void VeinholeMonsterClass::Draw_It(void)
{
	if (!IsDead) {
		ShapeSet const * shape = (ShapeSet const *)MonsterShape;
		int frame = ShapeFrame;

		Coord coord = CellID.As_Coord();
		if (Map[coord].IsFogged) {
			frame = 0;
		}

		Point2D drawpoint;
		TacticalMap->Coord_To_Pixel(coord, drawpoint);
		int height = 15 * Map[CellID].Height;
		drawpoint.Y += -58 - height;
		TacticalMap->Add_To_Selectables(this, Point2D(drawpoint.X, DSurface::ViewBounds.Y + drawpoint.Y + 58));
		ConvertClass * drawer = ColorSchemes[PlayerPtr->Scheme]->Converter;
		Draw_Shape(*LogicalSurface, *drawer, shape, frame, drawpoint, DSurface::ViewBounds, ShapeFlags_Type(SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_ALPHA|SHAPE_ZGRAD), 0, -17 - height, ZGRAD_GROUND, Map[coord].TileBrightness);
	}
}

// FUNCTION: GAMEMD 0x0074D5D0
ResultType VeinholeMonsterClass::Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source, bool forced, bool no_crew, HouseClass * house)
{
	ResultType result = BASECLASS::Take_Damage(damage, distance, warhead, source, forced, no_crew, house);

	switch (result) {
	case RESULT_NONE:
		break;

	case RESULT_DESTROYED:
		DesiredState = DYING;
		Detach_This_From_All(this, false);
		break;

	case RESULT_ALREADY_DESTROYED:
		return(RESULT_ALREADY_DESTROYED);

	default:
		DesiredState = ATTACKING;
		LogicTimer = 8 * TICKS_PER_SECOND;
		break;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0074D670
bool VeinholeMonsterClass::Can_Monster_Go_Here(Cell const & cell)
{
	if (ScenarioInit > 0) {
		return(true);
	}

	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		VeinholeMonsterClass * monster = VeinholeMonsters[i];
		if (abs(cell.X - monster->CellID.X) < 3 && abs(cell.Y - monster->CellID.Y) < 3) {
			return(false);
		}
	}

	int cell_height = Map[cell].Height;
	if (cell_height < 1) {
		return(false);
	}

	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			CellClass & cptr = Map[cell + Cell(x, y)];
			if (cptr.Ramp != RAMP_NONE || cptr.Land_Type() != LAND_CLEAR || cptr.Height != cell_height) {
				return(false);
			}
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x0074D760
void VeinholeMonsterClass::Reset(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		delete VeinholeMonsters[i];
	}
	VeinholeMonsters.Clear();
}

// FUNCTION: GAMEMD 0x0074D7C0
void VeinholeMonsterClass::Grow(void)
{
	if (GrowthLogic.GrowthQueue && GrowthLogic.GrowthCount <= Rule->MaxVeinholeGrowth - 40 && VeinCount <= Rule->MaxVeinholeGrowth - 100 && Scen->IsVeinGrowth) {
		int index = 0;
		int count = (abs(Scen->RandomNumber) % 5) + 1;
		CellNode * node = GrowthLogic.GrowthQueue->Extract_Min();

		while (index < count && node != NULL) {
			CellClass & cell = Map[node->Element];

			if (cell.OverlayData < OVERLAYDATA_FIRST_SOLID_VEIN) {
				cell.Place_Veins();
				VeinCount++;
			}

			if (cell.Overlay == OVERLAY_VEINS) {
				int cindex = Map_Cell_Index(node->Element);
				if (cindex >= 0 && cindex < Map_Cell_Count()) {
					GrowthLogic.GrowthState[cindex] = true;
				}
			}

			if (cell.OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN) {
				for (FacingType facing = FACING_FIRST; facing < FACING_COUNT; facing = (FacingType)(facing + FACING_90)) {
					Cell adjacent = Adjacent_Cell(node->Element, facing);
					if (Map.In_Local_Radar(adjacent)) {
						CellClass & adj_cell = Map[adjacent];
						if (abs(adj_cell.Height - cell.Height) < 2) {
							int cindex = Map_Cell_Index(adjacent);
							if (cindex >= 0 && cindex < Map_Cell_Count()) {
								if (adj_cell.Can_Place_Veins() && !GlobalGrowthState[cindex] && GrowthLogic.GrowthCount < Rule->MaxVeinholeGrowth) {
									GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Element = adjacent;
									GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Score = float(Frame / 50 + abs(Scen->RandomNumber() % 50) + 1);
									GlobalGrowthState[cindex] = true;
									GrowthLogic.GrowthQueue->Insert(GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount++]);
								}
								GrowthLogic.GrowthState[cindex] = true;
							}
						}
					}
				}
			}

			index++;
			if (index < count) {
				node = GrowthLogic.GrowthQueue->Extract_Min();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0074DE90
void VeinholeMonsterClass::Init_Vein_Growth_System(bool clear)
{
	int i;
	int cell_count = Map_Cell_Count();

	if (clear) {
		Deinit_Vein_Growth_System();

		for (i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
			VeinholeMonsters[i]->GrowthLogic.GrowthNodes = new CellNode[Rule->MaxVeinholeGrowth];
			VeinholeMonsters[i]->GrowthLogic.GrowthQueue = new PriorityQueueClass<CellNode>(Rule->MaxVeinholeGrowth);
			VeinholeMonsters[i]->GrowthLogic.GrowthState = new bool[cell_count];
			memset(VeinholeMonsters[i]->GrowthLogic.GrowthState, 0, cell_count);
			VeinholeMonsters[i]->GrowthLogic.GrowthQueue->Clear();
		}
	}

	if (GlobalGrowthState == NULL) {
		GlobalGrowthState = new bool[cell_count];
	}

	Map.Reset_Iterator();
	CellClass * iter = Map.Iterate();
	while (iter != NULL) {
		iter->IsToGrowVeins = false;
		iter = Map.Iterate();
	}

	memset(GlobalGrowthState, 0, cell_count);

	for (i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		if (VeinholeMonsters[i]->IsDead) {
			VeinholeMonsters[i]->Build_Shrinking_Queue();
		} else {
			VeinholeMonsters[i]->Build_Growth_Queue();
		}
	}

	Map.Reset_Iterator();
	iter = Map.Iterate();
	while (iter != NULL) {
		if (iter->Overlay == OVERLAY_VEINS && !GlobalGrowthState[Map_Cell_Index(iter->CellID)]) {
			iter->Overlay = OVERLAY_NONE;
		}
		iter->IsToGrowVeins = false;
		iter = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0074E100
void VeinholeMonsterClass::Deinit_Vein_Growth_System(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		VeinholeMonsters[i]->Clear_Growth();
	}
	Clear_Global_Data();
}

// FUNCTION: GAMEMD 0x0074E1C0
void VeinholeMonsterClass::Build_Growth_Queue(void)
{
	DynamicVectorClass<Cell> cells;
	cells.Set_Growth_Step(5000);

	GrowthLogic.GrowthCount = 0;
	VeinCount = 0;
	GrowthLogic.GrowthQueue->Clear();

	for (int i = Map_Cell_Count() - 1; i >= 0; i--) {
		GrowthLogic.GrowthState[i] = false;
	}

	for (int y = -2; y <= 2; y++) {
		for (int x = -2; x <= 2; x++) {
			CellClass * cellptr = &Map[CellID + Cell(x, y)];
			if (cellptr->Overlay == OVERLAY_VEINS) {
				int index = Map_Cell_Index(cellptr->CellID);
				cells.Add(CellID + Cell(x, y));
				if (index >= 0 && index < Map_Cell_Count()) {
					GlobalGrowthState[index] = true;
					GrowthLogic.GrowthState[index] = true;
					cellptr->IsToGrowVeins = true;
					if (cellptr->OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN && cellptr->OverlayData <= OVERLAYDATA_FIRST_SOLID_VEIN + 3) {
						VeinCount++;
					} else {
						if (GrowthLogic.GrowthCount < Rule->MaxVeinholeGrowth) {
							GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Element = cellptr->CellID;
							GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Score = 0;
							GrowthLogic.GrowthQueue->Insert(GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount++]);
						}
					}
				}
			}
		}
	}

	while (cells.Count() > 0) {
		int last = cells.Count() - 1;
		CellClass * cellptr = &Map[cells[last]];
		cells.Delete_Index(last);

		for (y = FACING_FIRST; y < FACING_COUNT; y++) {
			CellClass * adjacent = &cellptr->Adjacent_Cell((FacingType)y);
			int index = Map_Cell_Index(adjacent->CellID);
			if (adjacent->Overlay == OVERLAY_VEINS && !adjacent->IsToGrowVeins) {
				if ((adjacent->OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN && adjacent->OverlayData < (OVERLAYDATA_FIRST_SOLID_VEIN + 5)) || !adjacent->Can_Place_Veins()) {
					cells.Add(adjacent->CellID);
					VeinCount++;
				} else {
					if (GrowthLogic.GrowthCount < Rule->MaxVeinholeGrowth) {
						GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Element = adjacent->CellID;
						GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Score = 0;
						GrowthLogic.GrowthQueue->Insert(GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount++]);
						if (adjacent->OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN) {
							cells.Add(adjacent->CellID);
						}
					}
				}
				adjacent->IsToGrowVeins = true;
				if (index >= 0 && index < Map_Cell_Count()) {
					GlobalGrowthState[index] = true;
					GrowthLogic.GrowthState[index] = true;
				}
				VeinCount++;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0074E6B0
void VeinholeMonsterClass::Build_Shrinking_Queue(void)
{
	GrowthLogic.GrowthCount = 0;
	GrowthLogic.GrowthQueue->Clear();
	VeinCount = 0;

	Map.Reset_Iterator();
	CellClass * iter = Map.Iterate();
	while (iter != NULL) {
		if (iter->Overlay == OVERLAY_VEINS && iter->OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN) {
			int index = Map_Cell_Index(iter->CellID);
			if (index >= 0 && index < Map_Cell_Count() && GrowthLogic.GrowthState[index]) {
				if (GrowthLogic.GrowthCount < Rule->MaxVeinholeGrowth) {
					GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Element = iter->CellID;
					GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Score = 1000 - Point2D(iter->CellID.X, iter->CellID.Y).Distance_To(Point2D(CellID.X, CellID.Y));
					GrowthLogic.GrowthQueue->Insert(GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount++]);
					VeinCount++;
				}
			}
		}
		iter = Map.Iterate();
	}
}

// FUNCTION: GAMEMD 0x0074E880
void VeinholeMonsterClass::Clear_Global_Data(void)
{
	if (GlobalGrowthState) {
		delete [] GlobalGrowthState;
		GlobalGrowthState = NULL;
	}
}

// FUNCTION: GAMEMD 0x0074E8A0
void VeinholeMonsterClass::Clear_Growth(void)
{
	Clear_Growth_Data(this);
}

// FUNCTION: GAMEMD 0x0074E930
void VeinholeMonsterClass::Destroy_Monster(void)
{
	CellClass & cptr = Map[CellID];
	cptr.Overlay = OVERLAY_VEINS;
	cptr.OverlayData = 0;
	cptr.Recalc_Attributes();

	for (FacingType dir = FACING_FIRST; dir < FACING_COUNT; dir++) {
		CellClass & adj = cptr.Adjacent_Cell(dir);
		if (dir % 2 != 0) {
			adj.Overlay = OVERLAY_NONE;
			adj.OverlayData = 0;
		} else {
			adj.Overlay = OVERLAY_VEINS;
			adj.OverlayData = 0;
		}
	}

	for (int y = -2; y <= 2; y++) {
		for (int x = -2; x <= 2; x++) {
			CellClass & cptr2 = Map[Cell(x, y) + CellID];
			cptr2.Place_Veins();
			cptr2.Recalc_Attributes();
		}
	}

	Build_Shrinking_Queue();
	Map.Update_Cell_Zone(CellID);
	Map.Update_Cell_Subzones(CellID);
	IsDead = true;
}

// FUNCTION: GAMEMD 0x0074EA30
void VeinholeMonsterClass::Remove_Dead(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		if (VeinholeMonsters[i]->IsDead && VeinholeMonsters[i]->GrowthLogic.GrowthQueue->Count() == 0) {
			delete VeinholeMonsters[i];
		}
	}
}

// FUNCTION: GAMEMD 0x0074EA70
bool VeinholeMonsterClass::Load_All(IStream * stream)
{
	Reset();

	int cell_count = Map_Cell_Count();

	int monster_count;
	if (FAILED(stream->Read(&monster_count, sizeof(monster_count), NULL))) {
		return(false);
	}

	GlobalGrowthState = new bool[cell_count];
	if (FAILED(stream->Read(GlobalGrowthState, cell_count, NULL))) {
		return(false);
	}

	for (int i = 0; i < monster_count; i++) {
		VeinholeMonsterClass * monster = (VeinholeMonsterClass *)new char[sizeof(VeinholeMonsterClass)];

		LONG id;
		if (FAILED(stream->Read(&id, sizeof(id), NULL))) {
			return(false);
		}

		Swizzle_Here_I_Am(id, monster);
		if (FAILED(stream->Read(monster, sizeof(VeinholeMonsterClass), NULL))) {
			return(false);
		}

		if (monster != NULL) {
			new (monster) VeinholeMonsterClass(NoInitClass());
		}

		bool * state = monster->GrowthLogic.GrowthState;
		PriorityQueueClass<CellNode> * queue = monster->GrowthLogic.GrowthQueue;
		CellNode * nodes = monster->GrowthLogic.GrowthNodes;

		if (FAILED(stream->Read(state, cell_count, NULL))) {
			return(false);
		}
		monster->GrowthLogic.GrowthState = state;

		if (FAILED(stream->Read(nodes, sizeof(CellNode) * Rule->MaxVeinholeGrowth, NULL))) {
			return(false);
		}
		monster->GrowthLogic.GrowthNodes = nodes;

		if (!queue->Load(stream, nodes)) {
			return(false);
		}
		monster->GrowthLogic.GrowthQueue = queue;

		TargetTracker.Add_Index(monster->Fetch_ID(), monster);
	}

	return(true);
}

// FUNCTION: GAMEMD 0x0074ED60
bool VeinholeMonsterClass::Save_All(IStream * stream)
{
	int monster_count = VeinholeMonsters.Count();
	if (FAILED(stream->Write(&monster_count, sizeof(monster_count), NULL))) {
		return(false);
	}

	int cell_count = Map_Cell_Count();
	if (FAILED(stream->Write(GlobalGrowthState, cell_count, NULL))) {
		return(false);
	}

	for (int i = 0; i < monster_count; i++) {
		LONG id = (LONG)VeinholeMonsters[i];
		if (FAILED(stream->Write(&id, sizeof(id), NULL))) {
			return(false);
		}

		if (FAILED(stream->Write(VeinholeMonsters[i], sizeof(VeinholeMonsterClass), NULL))) {
			return(false);
		}

		if (FAILED(stream->Write(VeinholeMonsters[i]->GrowthLogic.GrowthState, cell_count, NULL))) {
			return(false);
		}

		if (FAILED(stream->Write(VeinholeMonsters[i]->GrowthLogic.GrowthNodes, sizeof(CellNode) * Rule->MaxVeinholeGrowth, NULL))) {
			return(false);
		}

		if (FAILED(VeinholeMonsters[i]->GrowthLogic.GrowthQueue->Save(stream, VeinholeMonsters[i]->GrowthLogic.GrowthNodes))) {
			return(false);
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x0074EEE0
HRESULT STDMETHODCALLTYPE VeinholeMonsterClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x0074EF00
TerrainTypeClass const * VeinholeMonsterClass::GetTerrainType(void)
{
	return(Rule->VeinholeTypeClass);
}

// FUNCTION: GAMEMD 0x0074EF10
void VeinholeMonsterClass::Reduce_Veins_At(CellClass * cellptr)
{
	if (cellptr->Overlay == OVERLAY_VEINS && cellptr->OverlayData >= OVERLAYDATA_FIRST_SOLID_VEIN) {

		bool facingbools[4];
		int facing;
		for (facing = FACING_FIRST; facing < FACING_COUNT; facing += FACING_90) {
			CellClass * adjacent = &cellptr->Adjacent_Cell((FacingType)facing);
			facingbools[facing / 2] = (adjacent->Overlay == OVERLAY_VEINS && (adjacent->OverlayData < OVERLAYDATA_FIRST_SOLID_VEIN || adjacent->OverlayData > OVERLAYDATA_FIRST_SOLID_VEIN + 3));
		}

		cellptr->Redraw_Veins();
		VeinCount--;

		for (facing = FACING_FIRST; facing < FACING_COUNT; facing += FACING_90) {
			CellClass * adjacent = &cellptr->Adjacent_Cell((FacingType)facing);
			if (adjacent->Overlay == OVERLAY_NONE) {
				int cindex = Map_Cell_Index(adjacent->CellID);
				if (cindex >= 0 && cindex < Map_Cell_Count()) {
					GrowthLogic.GrowthState[cindex] = false;
					GlobalGrowthState[cindex] = false;
				}

				if (facingbools[facing / 2] == true && !IsDead) {
					GrowthLogic.GrowthQueue->Remove_Matching(CellNode(adjacent->CellID));
				}
			}
		}

		OverlayType overlay = cellptr->Overlay;
		Cell & cell = cellptr->CellID;
		int cindex = Map_Cell_Index(cell);
		if (cindex >= 0 && cindex < Map_Cell_Count()) {
			if (overlay == OVERLAY_NONE) {
				GrowthLogic.GrowthState[cindex] = false;
				GlobalGrowthState[cindex] = false;
			} else if (!IsDead) {
				if (GrowthLogic.GrowthCount < Rule->MaxVeinholeGrowth) {
					GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Element = cell;
					GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount].Score = float(Frame / 50 + abs(Scen->RandomNumber() % 50) + 1);
					GrowthLogic.GrowthQueue->Insert(GrowthLogic.GrowthNodes[GrowthLogic.GrowthCount++]);
					GrowthLogic.GrowthState[cindex] = true;
					GlobalGrowthState[cindex] = true;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0074F2D0
HRESULT STDMETHODCALLTYPE VeinholeMonsterClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_VeinholeMonsterClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0074F310
// VeinholeMonsterClass::Fetch_RTTI

// FUNCTION: GAMEMD 0x0074F320
// VeinholeMonsterClass::Fetch_Object_Size

// FUNCTION: GAMEMD 0x0074F330
// VeinholeMonsterClass::In_Which_Layer

// SYNTHETIC: GAMEMD 0x0074F340
// ??_GVeinholeMonsterClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0074F360
// ?Clear@?$DynamicVectorClass@PAVVeinholeMonsterClass@@@@UAEXXZ
template class DynamicVectorClass<VeinholeMonsterClass *>;

// TEMPLATE: GAMEMD 0x0074F390
// ?ID@?$DynamicVectorClass@PAVVeinholeMonsterClass@@@@UAEHPBQAVVeinholeMonsterClass@@@Z

// TEMPLATE: GAMEMD 0x0074F3B0
// ?GetItem@?$VectorClass@PAVVeinholeMonsterClass@@@@UBEPAVVeinholeMonsterClass@@H@Z

// TEMPLATE: GAMEMD 0x0074F3C0
// ??8?$VectorClass@PAVVeinholeMonsterClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x0074F400
// ?Resize@?$VectorClass@PAVVeinholeMonsterClass@@@@UAE_NHPBQAVVeinholeMonsterClass@@@Z

// TEMPLATE: GAMEMD 0x0074F4B0
// ?Clear@?$VectorClass@PAVVeinholeMonsterClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x0074F4E0
// ?ID@?$VectorClass@PAVVeinholeMonsterClass@@@@UAEHABQAVVeinholeMonsterClass@@@Z

// TEMPLATE: GAMEMD 0x0074F520
// ?ID@?$VectorClass@PAVVeinholeMonsterClass@@@@UAEHPBQAVVeinholeMonsterClass@@@Z

// TEMPLATE: GAMEMD 0x0074F540
// ?Resize@?$DynamicVectorClass@PAVVeinholeMonsterClass@@@@UAE_NHPBQAVVeinholeMonsterClass@@@Z

// TEMPLATE: GAMEMD 0x0074F5F0
// ?ID@?$DynamicVectorClass@PAVVeinholeMonsterClass@@@@UAEHABQAVVeinholeMonsterClass@@@Z

// TEMPLATE: GAMEMD 0x0074F620
// ?Clear@?$PriorityQueueClass@UCellNode@@@@QAEXXZ
template class PriorityQueueClass<CellNode>;

// SYNTHETIC: GAMEMD 0x0074F650
// ??_G?$VectorClass@PAVVeinholeMonsterClass@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0074F6A0
// ??_G?$DynamicVectorClass@PAVVeinholeMonsterClass@@@@UAEPAXI@Z
