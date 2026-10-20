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
#include "terrtype.h"
#include "tracker.h"
#include "vector.h"
#include "ramp.hh"
#include <cstdio>
#include <math.h>

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

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F6610;
extern const double DAT_007F6618;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct SizeCenter {
	int A, B, C;
	SizeCenter() { C = 0; A = 0x80; B = 0x80; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0074C190 $E158
double DAT_00B1D278 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0074C1C0 $E161
double DAT_00B1D2E0 = DAT_00B1D278 / DAT_007F6610;

// FUNCTION: 0x0074C1E0 $E164
double DAT_00B1D2C8 = DAT_007E1718 / DAT_00B1D2E0;

// FUNCTION: 0x0074C200 $E167
double DAT_00B1D2C0 = DAT_007E1720 * DAT_007F6618;

// FUNCTION: 0x0074C220 $E170
double DAT_00B1D280 = DAT_007E1728 * DAT_007F6618;

// FUNCTION: 0x0074C240 $E173
double DAT_00B1D258 = DAT_007E1730 * DAT_007F6618;

// FUNCTION: 0x0074C260 $E176
int DAT_00B1D2E8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B1D258 - DAT_00B1D280) * DAT_00B1D278));

// FUNCTION: 0x0074C2A0 $E179
double DAT_00B1D288 = FUN_004CADE0(DAT_00B1D2E8 * DAT_007E1740);

// FUNCTION: 0x0074C2D0 $E182
double DAT_00B1D2B8 = FUN_004CADE0((DAT_00B1D2E8 * 2) / DAT_00B1D278);

// FUNCTION: 0x0074C300 $E185
int DAT_00B1D2DC = (int)(DAT_00B1D2E8 * 4 + DAT_007E1738);

// FUNCTION: 0x0074C330 $E188
ShortPair DAT_00B1D270;

// FUNCTION: 0x0074C340 $E191
IntTriple DAT_00B1D2A8;

// FUNCTION: 0x0074C360 $E194
SizeCenter DAT_00B1D2D0;

// FUNCTION: 0x0074C3C0 $E197
// FUNCTION: 0x0074C380 $E200
DynamicVectorClass<VeinholeMonsterClass *> VeinholeMonsterClass::VeinholeMonsters;


// FUNCTION: 0x0074C420
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

// FUNCTION: 0x0074C5B0
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

inline void VeinholeMonsterClass::Clear_Growth(void)
{
	if (GrowthLogic.GrowthQueue) {
		GrowthLogic.GrowthQueue->Clear();
		delete GrowthLogic.GrowthQueue;
		GrowthLogic.GrowthQueue = NULL;
	}

	if (GrowthLogic.GrowthNodes) {
		delete GrowthLogic.GrowthNodes;
		GrowthLogic.GrowthNodes = NULL;
	}

	if (GrowthLogic.GrowthState) {
		delete GrowthLogic.GrowthState;
		GrowthLogic.GrowthState = NULL;
	}

	GrowthLogic.GrowthCount = 0;
}

// FUNCTION: 0x0074C9F0
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

	Clear_Growth();

	VeinholeMonsters.Delete(this);
	TargetTracker.Remove_Index(Fetch_ID());
}

// FUNCTION: 0x0074CD60
VeinholeMonsterClass * VeinholeMonsterClass::Get_Monster_At(Cell const & cell)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		if (VeinholeMonsters[i]->CellID == cell) {
			return(VeinholeMonsters[i]);
		}
	}
	return(NULL);
}

// FUNCTION: 0x0074CDB0
VeinholeMonsterClass * VeinholeMonsterClass::Get_Vein_Owner_At(Cell const & cell)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		if (VeinholeMonsters[i]->GrowthLogic.GrowthState[Map_Cell_Index(cell)]) {
			return(VeinholeMonsters[i]);
		}
	}
	return(NULL);
}

// FUNCTION: 0x0074CDF0
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

inline void VeinholeMonsterClass::Shrink(void)
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

// FUNCTION: 0x0074CE50
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

// FUNCTION: 0x0074D430
void VeinholeMonsterClass::Draw_All(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		VeinholeMonsters[i]->Draw_It();
	}
}

// FUNCTION: 0x0074D450
void VeinholeMonsterClass::Init(TheaterType theater)
{
	char buffer[32];
	sprintf(buffer, "VEINHOLE.%s", Theaters[theater].Suffix);
	MonsterShape = FileSystem::LoadFile(buffer, false);
}

// FUNCTION: 0x0074D490
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

// FUNCTION: 0x0074D760
void VeinholeMonsterClass::Reset(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		delete VeinholeMonsters[i];
	}
	VeinholeMonsters.Clear();
}

// FUNCTION: 0x0074E880
void VeinholeMonsterClass::Clear_Global_Data(void)
{
	if (GlobalGrowthState) {
		delete [] GlobalGrowthState;
		GlobalGrowthState = NULL;
	}
}

// FUNCTION: 0x0074EEE0
HRESULT STDMETHODCALLTYPE VeinholeMonsterClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0074F2D0
HRESULT STDMETHODCALLTYPE VeinholeMonsterClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_VeinholeMonsterClass;
	return(S_OK);
}
