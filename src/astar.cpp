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
#include "geometry.h"
#include "vector.h"
#include "_map.h"
#include "tube.hh"
#define	END			FACING_NONE
#define	EMPTY		(FacingType)-2
#define	TUNNEL		FACING_COUNT

#define	EMPTY		(FacingType)-2

#include <math.h>

// SYNTHETIC: GAMEMD 0x00429570 SYMBOL
// _$E2


static unsigned int CellHeights[2000];
unsigned int MapCellStride;
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
// SYNTHETIC: GAMEMD 0x00429590 SYMBOL
// _$E79
// SYNTHETIC: GAMEMD 0x004295C0 SYMBOL
// _$E82
// SYNTHETIC: GAMEMD 0x004295E0 SYMBOL
// _$E85
// SYNTHETIC: GAMEMD 0x00429600 SYMBOL
// _$E88
// SYNTHETIC: GAMEMD 0x00429620 SYMBOL
// _$E91
// SYNTHETIC: GAMEMD 0x00429640 SYMBOL
// _$E94
// SYNTHETIC: GAMEMD 0x00429660 SYMBOL
// _$E97
// SYNTHETIC: GAMEMD 0x004296A0 SYMBOL
// _$E100
// SYNTHETIC: GAMEMD 0x004296D0 SYMBOL
// _$E103
// SYNTHETIC: GAMEMD 0x00429700 SYMBOL
// _$E106
// SYNTHETIC: GAMEMD 0x00429730 SYMBOL
// _$E109
// SYNTHETIC: GAMEMD 0x00429740 SYMBOL
// _$E112
// SYNTHETIC: GAMEMD 0x00429760 SYMBOL
// _$E115

// FUNCTION: GAMEMD 0x00429780
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


// FUNCTION: GAMEMD 0x00429830
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

// FUNCTION: GAMEMD 0x0042A460
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

// FUNCTION: GAMEMD 0x0042A5B0
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

// FUNCTION: GAMEMD 0x0042A690
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

// FUNCTION: GAMEMD 0x0042A6D0
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

// FUNCTION: GAMEMD 0x0042A900
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

// FUNCTION: GAMEMD 0x0042AA40
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

// FUNCTION: GAMEMD 0x0042AA90
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

// SYNTHETIC: GAMEMD 0x0042ABF0 SYMBOL
// _$E118

// FUNCTION: GAMEMD 0x0042AC00
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

// FUNCTION: GAMEMD 0x0042B080
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

// FUNCTION: GAMEMD 0x0042B1C0
int Map_Cell_Index(Cell const & cell)
{
	return(((cell.X - cell.Y + Map.PlayRect.Width - 1) >> 1) + Map.PlayRect.Width * (cell.X - Map.PlayRect.Width + cell.Y - 1));
}

// FUNCTION: GAMEMD 0x0042B1F0
int Map_Cell_Count(void)
{
	return((2 * Map.PlayRect.Width) * (Map.PlayRect.Height + 4));
}

// FUNCTION: GAMEMD 0x0042B210
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

// FUNCTION: GAMEMD 0x0042B420
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

// FUNCTION: GAMEMD 0x0042BCA0
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

// FUNCTION: GAMEMD 0x0042C1C0
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

// FUNCTION: GAMEMD 0x0042C290
bool AStarClass::Find_Path_Hierarchical(Cell const & from, Cell const & to, MZoneType mzone, FootClass const * foot)
{
	int *pass_table = (int *)MZonePassability[mzone];

	double threat_avoidance = foot != NULL ? foot->Threat_Avoidance_Value() : threat_avoidance = 0.0;

	HouseClass * house = foot != NULL ? foot->House : NULL;

	bool avoid_threats = threat_avoidance > 0.00001;

	for (int subzone_level = SUBZONE_COARSE; subzone_level >= SUBZONE_FINE; subzone_level--) {
		HierQueue->Clear();

		CellSubzoneStruct & start_cell_subzones = Map.CellSubzones[Map.Get_Cell_Zone_Index(from)];
		int start_subzone = start_cell_subzones.SubzoneID[subzone_level];
		CellSubzoneStruct & end_cell_subzones = Map.CellSubzones[Map.Get_Cell_Zone_Index(to)];
		int end_subzone = end_cell_subzones.SubzoneID[subzone_level];

		bool is_coarse = subzone_level == SUBZONE_COARSE;
		int * coarser_final_ids;
		if (!is_coarse) {
			coarser_final_ids = HierOnPath[subzone_level + 1];
		} else {
			coarser_final_ids = NULL;
		}

		int * final_ids = HierOnPath[subzone_level];
		int * working_ids = HierOpened[subzone_level];
		float * costs = HierCosts[subzone_level];
		final_ids[start_subzone] = UniqueID;
		final_ids[end_subzone] = UniqueID;

		if (start_subzone == end_subzone) {
			if (subzone_level == SUBZONE_FINE) {
				AStarHierarchicalNode * node = &HierNodePool[0];
				node->Depth = 0;
				node->SubzoneID = start_subzone;
			}
			HierSubzonePath[subzone_level][0] = start_subzone;
			HierSubzonePathCount[subzone_level] = 1;
		} else {
			int node_count = 0;
			AStarHierarchicalNode * start_node = &HierNodePool[0];
			start_node->ParentIndex = -1;
			start_node->SubzoneID = start_subzone;
			start_node->Score = 0.0;
			start_node->Depth = 0;

			HierQueue->Insert(*start_node);
			node_count++;
			working_ids[start_subzone] = UniqueID;
			costs[start_subzone] = 0.0;

			AStarHierarchicalNode * best_node = HierQueue->Extract_Min();
			bool no_banned_edges = HierBannedEdges[subzone_level].Count() == 0;

			while (best_node != NULL) {
				int from_subzone = best_node->SubzoneID;
				if (from_subzone == end_subzone) {
					break;
				}

				SubzoneConnectionStruct * connection = &Map.SubzoneTracking[subzone_level][from_subzone].Connections[0];

				int conn_count = Map.SubzoneTracking[subzone_level][from_subzone].Connections.Count();
				for (int conn_idx = 0; conn_idx < conn_count; conn_idx++) {
					int threat = 0;
					int to_subzone = connection[conn_idx].SubzoneID;
					bool is_cross_block = connection[conn_idx].IsCrossBlock;
					PassabilityType passability = Map.SubzoneTracking[subzone_level][to_subzone].Passability;
					int to_coarser_subzone = Map.SubzoneTracking[subzone_level][to_subzone].ParentSubzoneID;

					if (avoid_threats) {
						threat = Map.Region_Threat(house, subzone_level, from_subzone, to_subzone) * threat_avoidance;
					}

					double extra_score = is_cross_block ? 0.001 : 0.0;

					static const float _passability_scores[PASSABLE_COUNT] = {1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0};
					float score = _passability_scores[passability] + best_node->Score + (double)threat + extra_score;

					if ((working_ids[to_subzone] != UniqueID || costs[to_subzone] > score) && (is_coarse || coarser_final_ids[to_coarser_subzone] == UniqueID || passability == PASSABLE_CRUSH) && pass_table[passability] == TRAVERSAL_PASSABLE) {
						if (no_banned_edges || !Subzone_Edge_Banned(from_subzone, to_subzone, subzone_level)) {
							AStarHierarchicalNode * new_node = &HierNodePool[node_count];
							new_node->ParentIndex = best_node - HierNodePool;
							new_node->SubzoneID = to_subzone;
							new_node->Score = score;
							new_node->Depth = best_node->Depth + 1;
							HierQueue->Insert(*new_node);
							working_ids[to_subzone] = UniqueID;
							costs[to_subzone] = score;
							node_count++;
						}
					}
				}
				best_node = HierQueue->Extract_Min();
			}

			if (best_node == NULL) {
				return(false);
			}

			AStarHierarchicalNode * tail_node = best_node;
			while (best_node->ParentIndex != -1) {
				final_ids[best_node->SubzoneID] = UniqueID;
				best_node = &HierNodePool[best_node->ParentIndex];
			}

			int path_count = tail_node->Depth + 1;
			HierSubzonePathCount[subzone_level] = path_count;
			path_count--;
			while (path_count > 0) {
				HierSubzonePath[subzone_level][path_count] = tail_node->SubzoneID;
				tail_node = &HierNodePool[tail_node->ParentIndex];
				path_count--;
			}

			HierSubzonePath[subzone_level][0] = tail_node->SubzoneID;
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x0042C900
PathStruct * AStarClass::Find_Path(Cell const & from, Cell const & to, FootClass * foot, FacingType * moves, int max_loops, MZoneType mzone, ObstacleAvoidanceType avoidance)
{
	int i;

	IsHSEnabled = true;

	Clear();

	for (i = 0; i < ARRAY_SIZE(HierBannedEdges); i++) {
		HierBannedEdges[i].Clear();
	}

	Avoidance = avoidance;

	CellClass * from_ptr = &Map[from];
	CellClass * to_ptr = &Map[to];

	int from_zone = Map.Get_Cell_Zone(from, mzone != MZONE_NONE ? mzone : foot->TClass->MZone, foot->IsOnBridge);
	int to_zone = Map.Get_Cell_Zone(to,  mzone != MZONE_NONE ? mzone : foot->TClass->MZone, Map[to].IsUnderBridge);

	Cell hs_from = Map.Get_Bridge_Zone_Connection_Cell(from_ptr, foot->IsOnBridge);
	Cell hs_to = Map.Get_Bridge_Zone_Connection_Cell(to_ptr, to_ptr->IsUnderBridge);

	MZoneType move_zone = mzone != MZONE_NONE ? mzone : foot->TClass->MZone;

	if (foot->RTTI == RTTI_INFANTRY) {
		if (((InfantryClass *)foot)->Class->JumpJet) {
			move_zone = MZONE_INFANTRY;
			CLSID clsid;
			IPersistPtr persist(foot->Locomotion);
			persist->GetClassID(&clsid);
		}
	}

	bool with_hs;
	if (foot->TClass->IsTrain) {
		with_hs = false;
	} else if (!foot->IsInPlayfield || foot->Is_Allowed_To_Leave_Map()) {
		with_hs = false;
	} else if (!Map.In_Local_Radar(hs_from) || !Map.In_Local_Radar(hs_to)) {
		with_hs = false;
	} else {
		with_hs = true;
	}

	if (from_zone != to_zone) {
		if (with_hs) {
			return(NULL);
		}
	}

	if (with_hs) {
		if (!Find_Path_Hierarchical(hs_from, hs_to, move_zone, foot)) {
			DebugString("Hierarchical findpath failure: (%d,%d) to (%d, %d)\n", hs_from.X, hs_from.Y, hs_to.X, hs_to.Y);
			with_hs = false;
		}
	}

	PathStruct * path = NULL;

	i = 0;
	int tries = (max_loops == -1) ? 5 : 1;

	while (true) {

		if (!with_hs && hs_from != hs_to) {
			DebugString("Warning.  A* without HS: (%d,%d) to (%d, %d)\n", hs_from.X, hs_from.Y, hs_to.X, hs_to.Y);
		}

		path = Find_Path_Regular(from, to, foot, moves, max_loops, with_hs);
		if (path != NULL) {
			break;
		}

		if (!with_hs) {
			break;
		}

		int dy = abs(from.Y - to.Y);
		int dx = abs(from.X - to.X);
		if (dx <= dy) {
			dx = dy;
		}

		if (dx > 1) {
			DebugString("Regular findpath failure: (%d,%d) to (%d, %d)\n", hs_from.X, hs_from.Y, hs_to.X, hs_to.Y);
		}

		i++;

		Ban_Blocked_Subzone_Edges(foot);
		Clear();

		with_hs = IsHSEnabled ? true : false;

		if (i >= tries) {
			break;
		}

		if (with_hs) {
			if (!Find_Path_Hierarchical(hs_from, hs_to, move_zone, foot)) {
				break;
			}
		}

	}

	return(path);
}

// FUNCTION: GAMEMD 0x0042CCD0
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

// FUNCTION: GAMEMD 0x0042CEB0
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

// FUNCTION: GAMEMD 0x0042CF10
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

// FUNCTION: GAMEMD 0x0042CF80
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

// FUNCTION: GAMEMD 0x0042D170
int AStarClass::Test_Cell_Walk(Cell const & from, Cell const & to, FootClass const * foot, bool from_bridge, bool to_bridge, MZoneType mzone)
{
	IsHSEnabled = true;
	Clear();

	for (int i = 0; i < ARRAY_SIZE(HierBannedEdges); i++) {
		HierBannedEdges[i].Clear();
	}

	CellClass * from_ptr = &Map[from];
	CellClass * to_ptr = &Map[to];

	Cell hs_from = Map.Get_Bridge_Zone_Connection_Cell(from_ptr, from_bridge);
	Cell hs_to = Map.Get_Bridge_Zone_Connection_Cell(to_ptr, to_bridge);

	if (mzone == MZONE_NONE) {
		if (foot) {
			mzone = foot->TClass->MZone;
		} else {
			mzone = MZONE_NORMAL;
		}
	}

	if (Find_Path_Hierarchical(hs_from, hs_to, mzone, foot)) {
		int radius = MAX(abs(from.X - to.X), abs(from.Y - to.Y));
		int node_count = HierSubzonePathCount[SUBZONE_FINE];
		int distance = 2 * node_count - 2;

		bool add_bridge_dist = true;
		if (to_bridge && from_bridge) {
			int to_conn = Map.Zone_Connection_Index(to, 3, 0);
			int from_conn = Map.Zone_Connection_Index(from, 3, 0);
			if (to_conn == from_conn && to_conn != -1) {
				add_bridge_dist = false;
			}
		}

		if (add_bridge_dist) {
			if (to_bridge) {
				Cell bridge_end = CELL_NONE;
				if (node_count >= 4) {
					bridge_end = Map.Find_Bridge_End_Cell_For_Subzone(to, SUBZONE_FINE, HierSubzonePath[SUBZONE_FINE][node_count - 2]);
				}
				if (node_count < 4 || bridge_end == CELL_NONE) {
					bridge_end = Map.Find_Bridge_End_Cell_For_Subzone(to, SUBZONE_FINE, HierSubzonePath[SUBZONE_FINE][node_count - 1]);
				}
				if (bridge_end == CELL_NONE) {
					bridge_end = hs_to;
				}
				if (bridge_end != CELL_NONE) {
					distance += MAX(abs(to.X - bridge_end.X), abs(to.Y - bridge_end.Y));
				}
			}

			if (from_bridge) {
				Cell bridge_end = CELL_NONE;
				if (node_count >= 4) {
					bridge_end = Map.Find_Bridge_End_Cell_For_Subzone(from, 0, HierSubzonePath[SUBZONE_FINE][1]);
				}
				if (node_count < 4 || bridge_end == CELL_NONE) {
					bridge_end = Map.Find_Bridge_End_Cell_For_Subzone(from, 0, HierSubzonePath[SUBZONE_FINE][0]);
				}
				if (bridge_end == CELL_NONE) {
					bridge_end = hs_from;
				}
				if (bridge_end != CELL_NONE) {
					distance += MAX(abs(from.X - bridge_end.X), abs(from.Y - bridge_end.Y));
				}
			}

			if (distance > radius) {
				return(distance);
			}
		}

		return(radius);
	}
	return(INT_MAX);
}

// FUNCTION: GAMEMD 0x0042D470
// ??0Cell@@QAE@HH@Z

// FUNCTION: GAMEMD 0x0042D490
// ?Next_Cell@@YI?AVCell@@ABV1@W4FacingType@@@Z

// FUNCTION: GAMEMD 0x0042D510
// ??H?$TPoint2D@F@@QBE?BV0@ABV0@@Z

// FUNCTION: GAMEMD 0x0042D540
// ?Clear@?$DynamicVectorClass@I@@UAEXXZ

// FUNCTION: GAMEMD 0x0042D570
// ?ID@?$DynamicVectorClass@I@@UAEHPBI@Z

// FUNCTION: GAMEMD 0x0042D590
// ?GetItem@?$VectorClass@I@@UBEIH@Z

// FUNCTION: GAMEMD 0x0042D5A0
// ?Clear@?$DynamicVectorClass@G@@UAEXXZ

// FUNCTION: GAMEMD 0x0042D5D0
// ?ID@?$DynamicVectorClass@G@@UAEHPBG@Z

// FUNCTION: GAMEMD 0x0042D5F0
// ?GetItem@?$VectorClass@G@@UBEGH@Z

// FUNCTION: GAMEMD 0x0042D600
// ??8?$VectorClass@I@@UBE_NABV0@@Z

// FUNCTION: GAMEMD 0x0042D640
// ?Resize@?$VectorClass@I@@UAE_NHPBI@Z

// FUNCTION: GAMEMD 0x0042D6F0
// ?Clear@?$VectorClass@I@@UAEXXZ

// FUNCTION: GAMEMD 0x0042D720
// ?ID@?$VectorClass@I@@UAEHABI@Z

// FUNCTION: GAMEMD 0x0042D760
// ?ID@?$VectorClass@I@@UAEHPBI@Z

// FUNCTION: GAMEMD 0x0042D780
// ?Resize@?$DynamicVectorClass@I@@UAE_NHPBI@Z

// FUNCTION: GAMEMD 0x0042D830
// ?Add@?$DynamicVectorClass@I@@QAE_NABI@Z

// FUNCTION: GAMEMD 0x0042D880
// ?ID@?$DynamicVectorClass@I@@UAEHABI@Z

// FUNCTION: GAMEMD 0x0042D8B0
// ??8?$VectorClass@G@@UBE_NABV0@@Z

// FUNCTION: GAMEMD 0x0042D8F0
// ?Resize@?$VectorClass@G@@UAE_NHPBG@Z

// FUNCTION: GAMEMD 0x0042D9A0
// ?Clear@?$VectorClass@G@@UAEXXZ

// FUNCTION: GAMEMD 0x0042D9D0
// ?ID@?$VectorClass@G@@UAEHABG@Z

// FUNCTION: GAMEMD 0x0042DA10
// ?ID@?$VectorClass@G@@UAEHPBG@Z

// FUNCTION: GAMEMD 0x0042DA30
// ?Resize@?$DynamicVectorClass@G@@UAE_NHPBG@Z

// FUNCTION: GAMEMD 0x0042DAE0
// ?ID@?$DynamicVectorClass@G@@UAEHABG@Z

// SYNTHETIC: GAMEMD 0x0042DB10
// ??_G?$VectorClass@I@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0042DB60
// ??_G?$DynamicVectorClass@I@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0042DBB0
// ??_G?$VectorClass@G@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x0042DC00
// ??_G?$DynamicVectorClass@G@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0042DC50
// ??0?$VectorClass@I@@QAE@HPBI@Z

// TEMPLATE: GAMEMD 0x0042DCA0
// ?Heapify@?$PriorityQueueClass@UAStarHierarchicalNode@@@@QAEXH@Z

// TEMPLATE: GAMEMD 0x0042DD60
// ??0?$VectorClass@G@@QAE@HPBG@Z

template class VectorClass<unsigned int>;
template class DynamicVectorClass<unsigned int>;
template class VectorClass<unsigned short>;
template class DynamicVectorClass<unsigned short>;
