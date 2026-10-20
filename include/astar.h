// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_astar.h"
#include "coord.h"
#include "nodes.h"
#include "priority.h"
#include "vector.h"

#include "facing.hh"
#include "move.hh"
#include "mzone.hh"
#include "zone.hh"

class CellClass;
class FootClass;
class TechnoClass;
template<class T> class DynamicVectorClass;
template<class T> class TRect;
typedef TRect<int> Rect;

class AStarClass
{
	public:
		enum ObstacleAvoidanceType {
			AVOIDANCE_NONE,
			AVOIDANCE_SOFT,
			AVOIDANCE_HARD,
		};

		AStarClass(void);
		~AStarClass(void);

		PathStruct * Find_Path(Cell const & from, Cell const & to, FootClass * foot, FacingType * moves, int max_loops, MZoneType mzone, ObstacleAvoidanceType avoidance);

		void Clear(void);
		void Reset(void);
		void Update_Map_Dimensions(Rect const & dimensions);

		int Test_Cell_Walk(Cell const & from, Cell const & to, FootClass const * foot, bool from_bridge, bool to_bridge, MZoneType mzone);

	private:
		struct RegularNode {
			CellClass ** CellSlot;

			int CellHeight;

			RegularNode * Parent;
		};

		struct RegularOpenNode {

			RegularNode * Node;

			float MovementCost;

			float Score;

			int PathLength;

			RegularOpenNode(void)
			{
				PathLength = 0;
				Score = 0;
				MovementCost = 0;
			}

			bool operator==(const RegularOpenNode & other) const { return((double)Score == (double)other.Score); }
			bool operator!=(const RegularOpenNode & other) const { return((double)Score != (double)other.Score); }
			bool operator<(const RegularOpenNode & other) const { return((double)Score < (double)other.Score); }
			bool operator>(const RegularOpenNode & other) const { return((double)Score > (double)other.Score); }
			bool operator<=(const RegularOpenNode & other) const { return((double)Score <= (double)other.Score); }
			bool operator>=(const RegularOpenNode & other) const { return((double)Score >= (double)other.Score); }
		};

		struct RegularNodePool {

			RegularNode Nodes[131072];

			int ActiveCount;
			RegularNodePool(void) : ActiveCount(0) {}
		};

		struct RegularOpenNodePool {

			RegularOpenNode Nodes[65536];

			int ActiveCount;
			RegularOpenNodePool(void) : ActiveCount(0) {}
		};

	private:

		PathStruct * Find_Path_Regular(Cell const & from, Cell const & to, FootClass * foot, FacingType * moves, int max_loops, bool with_hs);
		RegularOpenNode * Create_Node(RegularOpenNode * parent, CellClass ** cell, Cell const & to, float movement_cost);
		bool Is_Visited(int, bool base_level, int index);
		double Get_Movement_Cost(CellClass ** from, CellClass ** to, bool bridge, MoveType move, FootClass * foot);
		void Apply_Path_Collision_Avoidance(FootClass * foot);
		FootClass * Find_Moving_Blocker(Cell const & cell, int cell_height);
		PathStruct * Build_Final_Path(RegularOpenNode * nodes, FacingType * moves);
		void Cut_Corners(PathStruct * path, FootClass * foot);
		int Try_Diagonal_Shortcut(FootClass * foot, FacingType * moves, unsigned int * heights, int initial_first_leg_length, int initial_second_leg_length, Cell & cell);
		void Optimize_Moves(PathStruct * path, FootClass * foot);
		void Splice_Path(FacingType * moves, int start_index, int end_index, int & splice_index, Cell & cell);
		bool Plot_Straight_Line(FacingType * moves, int move_count, Cell const & from, Cell const & to, FootClass * foot, int cell_height, bool fearless);

		bool Find_Path_Hierarchical(Cell const & from, Cell const & to, MZoneType mzone, FootClass const * foot);
		void Ban_Blocked_Subzone_Edges(FootClass const * foot);
		bool Subzone_Edge_Banned(unsigned short subzone1, unsigned short subzone2, int subzone_level);
		void Ban_Subzone_Edge(unsigned int subzone1, unsigned int subzone2, int subzone_level);
		void Ban_Neighborhood_Subzone_Edges(unsigned int subzone, int subzone_level);

	private:

		bool field_0;

		bool IsAvoidBridges;

		bool field_2;

		bool IsAvoidPathCollision;

		float MovementCostMultiplier;

		bool UseLocomotorEnterCheck;

		RegularNodePool * RegularNodes;
		RegularOpenNodePool * RegularOpenNodes;

		PriorityQueueClass<RegularOpenNode> * RegularQueue;

		int * RegularVisited;

		int * RegularBridgeVisited;

		float * RegularBridgeMovementCosts;

		float * RegularMovementCosts;

		int UniqueID;

		int ObjectSpeed;

		int CurrentCellHeight;

		int DestCellHeight;

		bool IsHSEnabled;

		ObstacleAvoidanceType Avoidance;

		int * HierOnPath[SUBZONE_COUNT];

		int * HierOpened[SUBZONE_COUNT];

		float * HierCosts[SUBZONE_COUNT];

		AStarHierarchicalNode * HierNodePool;

		PriorityQueueClass<AStarHierarchicalNode> * HierQueue;

		int HierNodeIndex;

		Cell HierLastNodeCell;

		DynamicVectorClass<unsigned int> HierBannedEdges[SUBZONE_COUNT];

		unsigned short HierSubzonePath[SUBZONE_COUNT][500];

		int HierSubzonePathCount[SUBZONE_COUNT];
};

Cell Follow_Path(Cell const & cell, int count, FacingType const * path);

int Map_Cell_Index(Cell const & cell);
int Map_Cell_Count(void);

extern int AStarFacingToOffset[FACING_COUNT];
