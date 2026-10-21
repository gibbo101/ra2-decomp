// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "astar.h"
#include "_astar.h"
#include "cell.h"
#include "dbgprint.h"
#include "foot.h"
#include "globals.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "mouse.h"
#include "swap.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vector.h"
#include "_map.h"
#include "tube.hh"
#define	END			FACING_NONE
#define	EMPTY		(FacingType)-2
#define	TUNNEL		FACING_COUNT

#define	EMPTY		(FacingType)-2

#include <math.h>

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
extern const double DAT_007E3700;
extern const double DAT_007E3708;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00429590 $E78
double DAT_0089C280 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004295C0 $E81
double DAT_0089C2D0 = DAT_0089C280 / DAT_007E3700;

// FUNCTION: 0x004295E0 $E84
double DAT_0089C2B8 = DAT_007E1718 / DAT_0089C2D0;

// FUNCTION: 0x00429600 $E87
double DAT_0089C2B0 = DAT_007E1720 * DAT_007E3708;

// FUNCTION: 0x00429620 $E90
double DAT_0089C288 = DAT_007E1728 * DAT_007E3708;

// FUNCTION: 0x00429640 $E93
double DAT_0089A2F8 = DAT_007E1730 * DAT_007E3708;

// FUNCTION: 0x00429660 $E96
int DAT_0089C2D8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089A2F8 - DAT_0089C288) * DAT_0089C280));

// FUNCTION: 0x004296A0 $E99
double DAT_0089C290 = FUN_004CADE0(DAT_0089C2D8 * DAT_007E1740);

// FUNCTION: 0x004296D0 $E102
double DAT_0089C2A8 = FUN_004CADE0((DAT_0089C2D8 * 2) / DAT_0089C280);

// FUNCTION: 0x00429700 $E105
int DAT_0089C2CC = (int)(DAT_0089C2D8 * 4 + DAT_007E1738);

// FUNCTION: 0x00429730 $E108
ShortPair DAT_0089C278;

// FUNCTION: 0x00429740 $E111
IntTriple DAT_0089C298;

static unsigned int CellHeights[2000];

unsigned int MapCellStride;

struct PlayfieldDefaults {
	int A, B, C;
	PlayfieldDefaults() { C = 0; A = 128; B = 128; }
};

// FUNCTION: 0x00429760 $E114
PlayfieldDefaults DAT_0089C2C0;

inline Cell Next_Cell(Cell const & cell, FacingType facing)
{
	if (facing == TUNNEL) {
		TubeType tube = (TubeType)Map[cell].Tube;
		if (tube != TUBE_NONE) {
			return(Cell(Tubes[tube]->Exit));
		}
		return(Cell(0,0));
	}
	return(cell + (AdjacentCell[facing]));
}

// FUNCTION: 0x00429780
Cell Follow_Path(Cell const & cell, int count, FacingType const * path)
{
	Cell result = cell;
	for (int i = 0; ; i++) {
		if (i >= count) {
			return(result);
		}
		result = Next_Cell(result, path[i]);
	}
}


// FUNCTION: 0x00429830
double AStarClass::Get_Movement_Cost(CellClass **from, CellClass **to, bool bridge, MoveType move, FootClass * foot)
{
	static float _costs[MOVE_COUNT] = {
		1.0,
		1000.0,
		1.0,
		1.0,
		60.0,
		20.0,
		8.0,
		10000.0
	};

	CellClass ** from_cells = from;
	CellClass ** to_cells = to;
	CellClass * from_cell = from_cells[0];
	CellClass * to_cell = to_cells[0];

	float cost = _costs[move];

	FootClass *next_occupier;
	FootClass * occupier;
	CellClass * cellptr = to_cell;

	bool on_bridge = bridge;

	if (move == MOVE_MOVING_BLOCK) {
		ObjectClass * object = cellptr->Cell_Occupier(on_bridge);
		occupier = (FootClass *)object;
		int depth = 0;
		bool clear = false;
		if (Avoidance == AVOIDANCE_NONE) {
			for (; depth < 10; depth++) {
				if (occupier == NULL) {
					clear = true;
					break;
				}

				if (!occupier->Is_Foot()) {
					clear = false;
					break;
				}

				FacingType facing;

				if (occupier->Speed == 0.0) {
					facing = occupier->Path[0];
					if (facing == FACING_NONE) {
						clear = true;
						break;
					}
				} else {
					facing = occupier->PrimaryFacing.Current().As_Dir8();
				}

				cellptr = &Map[::Adjacent_Cell(occupier->Get_Cell(), facing)];
				on_bridge = cellptr->IsUnderBridge && (occupier->IsOnBridge || occupier->Get_Cell_Ptr()->Height - cellptr->Height > 2);
				next_occupier = (FootClass *)cellptr->Cell_Occupier(on_bridge);
				occupier = (FootClass *)next_occupier;
			}
		}

		if (!clear || Avoidance != AVOIDANCE_NONE) {
			cost = 4.0f;
		}
		if (Avoidance == AVOIDANCE_HARD) {
			cost = 1000.0f;
		}
	}

	if (to_cell->IsPredictedPath) {
		cost = cost * 4.0f;
	}

	if (bridge && IsAvoidBridges) {

		static const int _bridge_cell_offset1[FACING_COUNT] =  { -2,    -2,     0,     1, 1,   1,   0,  -2 };
		static const int _bridge_cell_offset2[FACING_COUNT] =  {  0, -2 * MAP_CELL_W, -2 * MAP_CELL_W, -2 * MAP_CELL_W, 0, MAP_CELL_W, MAP_CELL_W, MAP_CELL_W };

		static const FacingType _bridge_facings[3][3] = {
			{ FACING_NW, FACING_N, FACING_NE },
			{ FACING_W, FACING_NONE, FACING_E },
			{ FACING_SW, FACING_S, FACING_SE }
		};

		float multiplier = 1.0f;

		Cell step(to_cell->CellID - from_cell->CellID);

		unsigned int dir = _bridge_facings[step.Y + 1][step.X + 1];

		CellClass *cell1;
		CellClass *cell2;
		int cell2_offset;

		if (to_cell->IsBridgeEastWest) {
			cell2_offset = _bridge_cell_offset2[(dir - FACING_S) % FACING_COUNT];
			cell1 = to[_bridge_cell_offset2[dir]];
			cell2 = to[cell2_offset];
		} else {
			cell2_offset = _bridge_cell_offset1[(dir - FACING_S) % FACING_COUNT];
			cell1 = to[_bridge_cell_offset1[dir]];
			cell2 = to[cell2_offset];
		}

		if (!cell1->IsUnderBridge) {
			multiplier = 10.0f;
		} else if (cell2->IsUnderBridge) {
			multiplier = 2.0f;
		}

		float result = cost * multiplier;
		return(result);

	}
	return(cost);
}

// FUNCTION: 0x0042A460
AStarClass::RegularOpenNode * AStarClass::Create_Node(RegularOpenNode * parent, CellClass ** cell, Cell const & to, float movement_cost)
{
	RegularOpenNode * open_node = &RegularOpenNodes->Nodes[RegularOpenNodes->ActiveCount++];
	RegularNode * node_data = &RegularNodes->Nodes[RegularNodes->ActiveCount++];
	node_data->CellSlot = cell;

	if (parent != NULL) {
		node_data->Parent = parent->Node;
		CellClass * current_cell = *node_data->CellSlot;
		CellClass * parent_cell = *parent->Node->CellSlot;
		node_data->CellHeight = current_cell->Height;
		RegularNode * parent_node_data = parent->Node;
		if (current_cell->IsUnderBridge) {
			if (parent_cell->IsUnderBridge && (parent_node_data->CellHeight == parent_cell->Height + BRIDGE_CELL_HEIGHT)) {
				node_data->CellHeight += BRIDGE_CELL_HEIGHT;
			} else if (!parent_cell->IsUnderBridge && abs((current_cell->Height - parent->Node->CellHeight) + BRIDGE_CELL_HEIGHT - 1) <= 1) {
				node_data->CellHeight += BRIDGE_CELL_HEIGHT;
			}
		}
	} else {
		node_data->Parent = NULL;
		node_data->CellHeight = CurrentCellHeight;
	}

	open_node->Node = node_data;
	if (parent != NULL) {
		open_node->MovementCost = movement_cost + parent->MovementCost;
		open_node->PathLength = parent->PathLength + 1;
	} else {
		open_node->MovementCost = 0.0;
		open_node->PathLength = 1;
	}

	Cell & cell_id = (*cell)->CellID;
	int dx = abs(cell_id.X - to.X);
	int dy = abs(cell_id.Y - to.Y);

	open_node->Score = open_node->MovementCost + fastmath::sqrt(dx * dx + dy * dy);
	return(open_node);
}

// FUNCTION: 0x0042A5B0
void AStarClass::Clear(void)
{
	int i;
	RegularNodes->ActiveCount = 0;
	RegularOpenNodes->ActiveCount = 0;
	RegularQueue->Clear();

	HierQueue->Clear();

	UniqueID++;

	if (UniqueID == 0) {
		for (i = MapCellStride * MapCellStride - 1; i >= 0; i--) {
			RegularVisited[i] = NULL;
			RegularBridgeVisited[i] = NULL;
		}

		for (i = 0; i < ARRAY_SIZE(HierOnPath); i++) {
			int * final_ids = HierOnPath[i];
			int * working_ids = HierOpened[i];
			float * costs = HierCosts[i];
			int count = Map.SubzoneTrackingEntryCount[i];
			for (int j = count - 1; j >= 0; j--) {
				final_ids[j] = 0;
				working_ids[j] = 0;
				costs[j] = 0;
			}
		}

		UniqueID++;
	}
}

// FUNCTION: 0x0042A690
bool AStarClass::Is_Visited(int, bool base_level, int index)
{
	if (base_level) {
		if (RegularVisited[index] == UniqueID) {
			return(true);
		}
		return(false);
	}
	if (RegularBridgeVisited[index] == UniqueID) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0042A6D0
AStarClass::AStarClass(void) :
	field_0(false),
	IsAvoidBridges(false),
	field_2(false),
	IsAvoidPathCollision(true),
	MovementCostMultiplier(1),
	UseLocomotorEnterCheck(true),
	RegularVisited(NULL),
	RegularBridgeVisited(NULL),
	RegularBridgeMovementCosts(NULL),
	RegularMovementCosts(NULL),
	UniqueID(-1),
	ObjectSpeed(-1),
	IsHSEnabled(true),
	Avoidance(AVOIDANCE_NONE),
	HierNodeIndex(-1),
	HierLastNodeCell(0,0)
{
	RegularQueue = new PriorityQueueClass<RegularOpenNode>(65536);
	HierQueue = new PriorityQueueClass<AStarHierarchicalNode>(10000);
	RegularOpenNodes = new RegularOpenNodePool;
	RegularNodes = new RegularNodePool;

	RegularNodes->ActiveCount = 0;
	RegularOpenNodes->ActiveCount = 0;
	RegularQueue->Clear();
	HierQueue->Clear();

	for (int i = 0; i < 3; i++) {
		HierOnPath[i] = NULL;
		HierOpened[i] = NULL;
		HierCosts[i] = NULL;
		HierBannedEdges[i].Clear();
		memset(HierSubzonePath[i], 0, sizeof(HierSubzonePath[i]));
		HierSubzonePathCount[i] = 0;
	}
	HierNodePool = new AStarHierarchicalNode[10000];
}

// FUNCTION: 0x0042A900
AStarClass::~AStarClass(void)
{
	delete RegularQueue;
	RegularQueue = NULL;

	delete HierQueue;
	HierQueue = NULL;

	if (RegularOpenNodes != NULL) {
		delete RegularOpenNodes;
	}
	RegularOpenNodes = NULL;

	if (RegularNodes != NULL) {
		delete RegularNodes;
	}
	RegularNodes = NULL;

	if (RegularVisited != NULL) {
		delete RegularVisited;
		RegularVisited = NULL;
	}

	if (RegularBridgeVisited != NULL) {
		delete RegularBridgeVisited;
		RegularBridgeVisited = NULL;
	}

	if (RegularMovementCosts != NULL) {
		delete RegularMovementCosts;
		RegularMovementCosts = NULL;
	}

	if (RegularBridgeMovementCosts != NULL) {
		delete RegularBridgeMovementCosts;
		RegularBridgeMovementCosts = NULL;
	}

	for (int i = 0; i < 3; i++) {
		if (HierOnPath[i]) {
			delete HierOnPath[i];
			HierOnPath[i] = NULL;
		}

		if (HierOpened[i]) {
			delete HierOpened[i];
			HierOpened[i] = NULL;
		}

		if (HierCosts[i]) {
			delete HierCosts[i];
			HierCosts[i] = NULL;
		}
	}

	delete [] HierNodePool;
}

// FUNCTION: 0x0042AA40
FacingType Facing_Between(Cell const & cell1, Cell const & cell2)
{
	static FacingType _table[3][3] = {
		{ FACING_SE, FACING_S, FACING_SW },
		{ FACING_E, FACING_NONE, FACING_W },
		{ FACING_NE, FACING_N, FACING_NW }
	};

	if (abs(cell2.Y - cell1.Y) <= 1 && abs(cell2.X - cell1.X) <= 1) {
		return(_table[cell2.Y - cell1.Y + 1][cell2.X - cell1.X + 1]);
	}
	return(FACING_COUNT);
}

// FUNCTION: 0x0042AA90
PathStruct * AStarClass::Build_Final_Path(RegularOpenNode *final_node, FacingType *moves)
{
	static PathStruct path;

	path.Cost			= int(final_node->Score);
	path.Length 		= final_node->PathLength;
	path.Overlap		= 0;
	path.LastOverlap.X	= 0;
	path.LastOverlap.Y	= 0;
	path.Command 		= moves;
	path.Height			= CellHeights;

	RegularNode * current = final_node->Node;
	RegularNode * parent = current->Parent;

	for (int i = (final_node->PathLength - 2); i >= 0; i--) {
		if (parent != NULL) {
			CellHeights[i] = parent->CellHeight;
			moves[i] = Facing_Between((*current->CellSlot)->CellID, (*parent->CellSlot)->CellID);
		}
		current = current->Parent;
		parent = parent->Parent;
	}

	moves[final_node->PathLength - 1] = FACING_NONE;

	path.Start = (*current->CellSlot)->CellID;

	if (path.Cost == 0) {
		path.Cost = 1;
	}
	return(&path);
}

// FUNCTION: 0x0042ABF0 $E117

// FUNCTION: 0x0042AC00
void AStarClass::Update_Map_Dimensions(Rect const & dimensions)
{
	if (RegularVisited != NULL) {
		delete RegularVisited;
		RegularVisited = NULL;
	}
	if (RegularBridgeVisited != NULL) {
		delete RegularBridgeVisited;
		RegularBridgeVisited = NULL;
	}
	if (RegularMovementCosts != NULL) {
		delete RegularMovementCosts;
		RegularMovementCosts = NULL;
	}
	if (RegularBridgeMovementCosts != NULL) {
		delete RegularBridgeMovementCosts;
		RegularBridgeMovementCosts = NULL;
	}

	MapCellStride = dimensions.Height + dimensions.Width + 1;
	int count = MapCellStride * MapCellStride;
	RegularBridgeVisited = new int[count];
	RegularVisited = new int[count];
	RegularMovementCosts = new float[count];
	RegularBridgeMovementCosts = new float[count];

	AStarFacingToOffset[FACING_N]	= -MapCellStride;
	AStarFacingToOffset[FACING_NE]	= 1 - MapCellStride;
	AStarFacingToOffset[FACING_E]	= +1;
	AStarFacingToOffset[FACING_SE]	= MapCellStride + 1;
	AStarFacingToOffset[FACING_S]	= +MapCellStride;
	AStarFacingToOffset[FACING_SW]	= MapCellStride - 1;
	AStarFacingToOffset[FACING_W]	= -1;
	AStarFacingToOffset[FACING_NW]	= -1 - MapCellStride;
}

// FUNCTION: 0x0042B080
FootClass * AStarClass::Find_Moving_Blocker(Cell const & cell, int cell_height)
{
	Coord coord = Coord(cell, 0);
	coord.Z = cell_height * LEVEL_LEPTON_H;
	for (int y = -2; y < 3; y++) {
		for (int x = -2; x < 3; x++) {
			CellClass & neighbor = Map[Cell(x,y) + cell];
			FootClass * occupier = (FootClass *)neighbor.Cell_Occupier(neighbor.IsUnderBridge && abs(neighbor.Height - cell_height) > 2);
			while (occupier != NULL) {
				if (occupier->Is_Foot()) {
					if (occupier->Locomotion->Is_Moving_Here(coord)) {
						return(occupier);
					}
				}
				occupier = (FootClass *)occupier->Next;
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x0042B1C0
int Map_Cell_Index(Cell const & cell)
{
	return(((cell.X - cell.Y + Map.PlayRect.Width - 1) >> 1) + Map.PlayRect.Width * (cell.X - Map.PlayRect.Width + cell.Y - 1));
}

// FUNCTION: 0x0042B1F0
int Map_Cell_Count(void)
{
	return((2 * Map.PlayRect.Width) * (Map.PlayRect.Height + 4));
}

// FUNCTION: 0x0042B210
void AStarClass::Cut_Corners(PathStruct * path, FootClass * foot)
{
	int max_len = path->Length;

	max_len--;

	FacingType leg2_dir = FACING_NONE;

	unsigned int * heights = path->Height;
	FacingType * moves = path->Command;
	Cell source = path->Start;

	int leg1_start = 0;
	int leg2_start = 0;

	bool found_corner = false;

	int leg1_len = 0;
	int leg2_len = 0;

	Cell start_cell = source;
	Cell cursor_cell = source;

	FacingType leg1_dir = FACING_NONE;

	while (leg1_start + leg1_len < max_len && leg2_start + leg2_len < max_len) {

		if (!found_corner) {
			FacingType move_dir = moves[leg1_start + leg1_len];
			FacingType turn = Facing_Sub(move_dir, leg1_dir);

			if (move_dir == leg1_dir) {
				leg1_len++;

			} else if ((turn == FACING_90 || turn == FACING_270) && leg1_dir != FACING_NONE && leg1_dir != TUNNEL && move_dir != TUNNEL) {
				found_corner = true;
				leg2_dir = move_dir;
				leg2_len = 1;
				leg2_start = leg1_start + leg1_len;

			} else {
				leg1_start += leg1_len;
				leg1_len = 1;

				leg1_dir = (move_dir & 1) ? move_dir : FACING_NONE;
				start_cell = cursor_cell;
			}

			cursor_cell = Next_Cell(cursor_cell, move_dir);

		} else if (moves[leg2_len + leg2_start] == leg2_dir) {
			leg2_len++;

		} else {
			leg1_start += Try_Diagonal_Shortcut(foot, &moves[leg1_start], &heights[leg1_start], leg1_len, leg2_len, start_cell);
			leg1_len = 1;
			found_corner = false;

			cursor_cell = start_cell + AdjacentCell[(int)moves[leg1_start] & 0x07];
			leg1_dir = moves[leg1_start];
		}
	}

	if (found_corner) {
		Try_Diagonal_Shortcut(foot, &moves[leg1_start], &heights[leg1_start], leg1_len, leg2_len, start_cell);
	}
}

static Cell Follow_Path2(Cell const & cell, int count, FacingType const * path)
{
	return(Follow_Path(cell, count, path));
}

// FUNCTION: 0x0042B420
int AStarClass::Try_Diagonal_Shortcut(FootClass * foot, FacingType * moves, unsigned int * heights, int initial_first_leg_length, int initial_second_leg_length, Cell & cell)
{
	int first_leg_length = initial_first_leg_length;
	int second_leg_length = initial_second_leg_length;
	bool is_blocked;
	FacingType second_leg_direction = moves[first_leg_length];
	FacingType first_leg_direction = moves[0];
	FacingType diagonal_direction = FacingType((first_leg_direction + second_leg_direction) >> 1);

	if (diagonal_direction + FACING_45 != second_leg_direction
	&& diagonal_direction + FACING_45 != first_leg_direction) {
		diagonal_direction = FACING_N;
	}

	if (first_leg_direction == TUNNEL || second_leg_direction == TUNNEL) {
		int total_length = first_leg_length + second_leg_length;
		cell = Follow_Path2(cell, total_length, moves);
		return(total_length);
	}

	Cell diagonal_start_cell = cell;

	int max_diagonal_length = second_leg_length;
	second_leg_length = first_leg_length < max_diagonal_length ? first_leg_length : max_diagonal_length;
	if (second_leg_length < first_leg_length) {
		diagonal_start_cell = Follow_Path(diagonal_start_cell, first_leg_length - second_leg_length, moves);
	}

	double threat_avoidance = foot->Threat_Avoidance_Value();
	HouseClass * house = foot->House;

	while (second_leg_length > 0) {
		int moves_remaining = second_leg_length * 2;
		is_blocked = false;
		Cell scan_cell = Adjacent_Cell(diagonal_start_cell, diagonal_direction);
		int leg_total = first_leg_length + second_leg_length;
		int scan_height = heights[leg_total - moves_remaining];
		CellClass * scan_cell_ptr = &Map[scan_cell];

		while (!is_blocked && moves_remaining > 0) {
			is_blocked = foot->Can_Enter_Cell(scan_cell_ptr, diagonal_direction, scan_height) != MOVE_OK
			|| scan_cell_ptr->IsPredictedPath
			|| Map.Cell_Threat(diagonal_start_cell, *house) * threat_avoidance >= 1.0;

			moves_remaining--;

			scan_cell = Adjacent_Cell(scan_cell, diagonal_direction);
			scan_cell_ptr = &Map[scan_cell];

			scan_height = (scan_height - scan_cell_ptr->Height == BRIDGE_CELL_HEIGHT && scan_cell_ptr->IsUnderBridge)
			? scan_cell_ptr->Height + BRIDGE_CELL_HEIGHT
			: scan_cell_ptr->Height;

		}

		if (!is_blocked) {
			FacingType * diagonal_moves = &moves[first_leg_length - second_leg_length];
			int diagonal_count = 2 * second_leg_length;
			for (int move_index = 0; move_index < diagonal_count; move_index++) {
				diagonal_moves[move_index] = diagonal_direction;
			}

			cell = Follow_Path(cell, first_leg_length - second_leg_length, moves);
			return(first_leg_length - second_leg_length);
		}

		diagonal_start_cell = Adjacent_Cell(diagonal_start_cell, first_leg_direction);
		second_leg_length--;
	}

	cell = Follow_Path2(cell, first_leg_length, moves);
	return(first_leg_length);
}

// FUNCTION: 0x0042BCA0
void AStarClass::Splice_Path(FacingType * moves, int start_index, int end_index, int & splice_index, Cell & cell)
{
	int max_radius = 0;

	Cell displacement;
	displacement.X = 0;
	displacement.Y = 0;

	int cur_index = start_index;
	Cell base = cell;
	bool found = false;

	while (cur_index >= end_index) {
		if (moves[cur_index] == EMPTY) {
			cur_index--;
		} else {
			FacingType back = Facing_Sub(moves[cur_index], 4);
			displacement = ::Adjacent_Cell(displacement, back);
			base = ::Adjacent_Cell(base, back);
			int abs_y = abs(displacement.Y);
			int abs_x = abs(displacement.X);
			int radius = MAX(abs_x, abs_y);
			if (radius > max_radius) {
				if (found) {
					splice_index = cur_index + 1;
					cell = ::Adjacent_Cell(base, Facing_Sub(back, 4));
					return;
				}
				max_radius = radius;
			} else {
				found = true;
			}
			cur_index--;
		}
	}

	splice_index = end_index;
	cell = base;
	return;
}

// FUNCTION: 0x0042C1C0
void AStarClass::Reset(void)
{
	int j;

	for (int i = 0; i < ARRAY_SIZE(HierOnPath); i++) {
		if (HierOnPath[i] != NULL) {
			delete[] HierOnPath[i];
		}
		int * final_ids = new int[Map.SubzoneTrackingEntryCount[i]];
		HierOnPath[i] = final_ids;
		for (j = Map.SubzoneTrackingEntryCount[i] - 1; j >= 0; j--) {
			final_ids[j] = 0;
		}

		if (HierOpened[i] != NULL) {
			delete[] HierOpened[i];
		}
		int * working_ids = new int[Map.SubzoneTrackingEntryCount[i]];
		HierOpened[i] = working_ids;
		for (j = Map.SubzoneTrackingEntryCount[i] - 1; j >= 0; j--) {
			working_ids[j] = 0;
		}

		if (HierCosts[i] != NULL) {
			delete[] HierCosts[i];
		}
		float * costs = new float[Map.SubzoneTrackingEntryCount[i]];
		HierCosts[i] = costs;
		for (j = Map.SubzoneTrackingEntryCount[i] - 1; j >= 0; j--) {
			costs[j] = 0;
		}
	}
}

// FUNCTION: 0x0042CCD0
void AStarClass::Ban_Blocked_Subzone_Edges(FootClass const * foot)
{
	CellSubzoneStruct & subzone = Map.CellSubzones[Map.Get_Cell_Zone_Index(HierLastNodeCell)];

	for (int subzone_level = 0; subzone_level < SUBZONE_COUNT; subzone_level++) {
		unsigned short from_subzone = subzone.SubzoneID[subzone_level];
		DynamicVectorClass<unsigned short> to_subzones;
		to_subzones.Clear();

		if (Map.Build_Reachable_Subzones(&Map[HierLastNodeCell], subzone_level, to_subzones, foot)) {
			CellSubzoneStruct & last_subzone = Map.CellSubzones[Map.Get_Cell_Zone_Index(HierLastNodeCell)];
			unsigned int subzone_id = (unsigned short)last_subzone.SubzoneID[subzone_level];
			Ban_Neighborhood_Subzone_Edges(subzone_id, subzone_level);
			continue;
		}

		for (int j = to_subzones.Count() - 1; j >= 0; j--) {
			Ban_Subzone_Edge(from_subzone, to_subzones[j], subzone_level);
		}
	}
}

// FUNCTION: 0x0042CEB0
bool AStarClass::Subzone_Edge_Banned(unsigned short subzone1, unsigned short subzone2, int subzone_level)
{
	if (subzone2 < subzone1) {
		swap(subzone1, subzone2);
	}
	unsigned int edge = subzone2 | (subzone1 << 16);

	DynamicVectorClass<unsigned int> & edges = HierBannedEdges[subzone_level];
	for (int i = edges.Count() - 1; i >= 0; i--) {
		if (edges[i] == edge) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x0042CF10
void AStarClass::Ban_Subzone_Edge(unsigned int subzone1, unsigned int subzone2, int subzone_level)
{
	if (subzone1 != subzone2) {
		if (subzone2 < subzone1) {
			swap(subzone2, subzone1);
		}
		unsigned int edge = subzone2 | (subzone1 << 16);
		HierBannedEdges[subzone_level].Add(edge);
	}
}

// FUNCTION: 0x0042CF80
void AStarClass::Ban_Neighborhood_Subzone_Edges(unsigned int subzone, int subzone_level)
{
	int i;
	int j;

	int path_count = HierSubzonePathCount[subzone_level];
	if (path_count <= 1) {
		IsHSEnabled = false;
		return;
	}

	int path_index = -1;
	for (i = 0; i < path_count; i++) {
		if (HierSubzonePath[subzone_level][i] == subzone) {
			path_index = i;
			break;
		}
	}

	if (path_index == -1) {
		IsHSEnabled = false;
	} else {
		unsigned short path_node;
		unsigned short path_neighbor;
		if (path_index == path_count - 1) {
			path_node = HierSubzonePath[subzone_level][path_index];
			path_neighbor = HierSubzonePath[subzone_level][path_index - 1];
		} else {
			path_node = HierSubzonePath[subzone_level][path_index + 1];
			path_neighbor = HierSubzonePath[subzone_level][path_index];
		}

		Ban_Subzone_Edge(path_node, path_neighbor, subzone_level);

		DynamicVectorClass<SubzoneConnectionStruct> & node_conn = Map.SubzoneTracking[subzone_level][path_node].Connections;
		DynamicVectorClass<SubzoneConnectionStruct> & neighbor_conn = Map.SubzoneTracking[subzone_level][path_neighbor].Connections;

		for (j = node_conn.Count() - 1; j >= 0; j--) {
			unsigned short common_subzone = node_conn[j].SubzoneID;
			if (common_subzone != path_neighbor) {
				for (i = neighbor_conn.Count() - 1; i >= 0; i--) {
					if (neighbor_conn[i].SubzoneID == common_subzone) {
						Ban_Subzone_Edge(path_neighbor, common_subzone, subzone_level);
					}
				}
			}
		}
	}
}
