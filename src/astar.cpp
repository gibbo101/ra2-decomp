// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "astar.h"
#include "_astar.h"
#include "_map.h"
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
#include "tube.hh"
#define	END			FACING_NONE
#define	EMPTY		(FacingType)-2
#define	TUNNEL		FACING_COUNT

#define	EMPTY		(FacingType)-2

unsigned int MapCellStride;

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
