// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "vector.h"
#include "event.h"

class TechnoClass;
class HouseClass;
class PlanningNodeClass;

class PlanningMemberClass
{
	public:
		PlanningMemberClass(void);
		void Set_Owner(TechnoClass * owner);
		void Set_Packet(EventClass * packet);
		void Set_Branch(int branch);
		void Set_Active(bool active);
		TechnoClass * Get_Owner(void);
		EventClass * Get_Packet(void);
		int Get_Branch(void);
		bool Get_Active(void);

		TechnoClass * Owner;
		EventClass * Packet;
		int Branch;
		bool Active;
};

class PlanningBranchClass
{
	public:
		PlanningBranchClass(void);
		void Set_Event(EventClass const & event);
		void Set_Count(int count);
		void Set_Color(int color);
		void Clear_Count(void);
		int Inc_Count(void);
		int Dec_Count(void);
		EventClass Get_Event(void);
		int Get_Count(void);
		int Get_Color(void);
		Coord Get_Coord(void);
		Point2D Get_Pixel(void);

		EventClass Event;
		int Count;
		int Color;
};

class PlanningNodeClass
{
	public:
		static DynamicVectorClass<PlanningNodeClass *> Unknown1;
		static DynamicVectorClass<PlanningNodeClass *> Unknown2;
		static DynamicVectorClass<PlanningNodeClass *> Unknown3;
		static bool PlanningModeActive;

		PlanningNodeClass(int id);
		~PlanningNodeClass();
		int Get_Id(void);
		int Get_Count(void);
		int Index_In(TechnoClass * techno);
		PlanningNodeClass * Prev_Node(TechnoClass * techno);
		PlanningNodeClass * Next_Node(TechnoClass * techno);
		void Add_Member(TechnoClass * techno, EventClass const * event);
		void Remove_Member(TechnoClass * techno);
		int Find_Member(TechnoClass * techno);
		TechnoClass * Member_Owner(int index);
		EventClass Get_Event(int index);
		EventClass Get_Event_For(TechnoClass * techno);
		bool Has_Member(TechnoClass * techno);
		HouseClass * Lead_House(void);
		Coord Lead_Coord(void);
		Point2D Lead_Pixel(void);
		bool Hit_Test(int x, int y);
		bool Lead_At(Coord coord);
		Rect Lead_Rect(void);
		Rect Lead_Box_Rect(void);
		Rect Lead_Box_Rect2(void);
		bool Is_Past_Start(void);
		void Promote_Events(void);
		PlanningMemberClass * Get_Member(TechnoClass * techno);
		TechnoClass * Owner_At_Branch(int branch);
		TechnoClass * Active_Owner(void);
		PlanningBranchClass * Get_Branch(int index);
		PlanningBranchClass * Branch_Of(TechnoClass * techno);
		EventClass * Loop_Event(void);
		Coord Branch_Coord(int index);
		Coord Loop_Coord(void);
		Point2D Branch_Pixel(int index);
		int Branch_Count(void);
		int Has_Loop(void);
		int Is_Loop_Alone(void);
		Point2D Loop_Pixel(void);
		void Set_Queued(void);
		bool Get_Queued(void);
		void Rebuild(void);
		void Update_Active(void);
		void FUN_00634E10(PlanningMemberClass * member);
		int Find_Branch(TechnoClass * techno);
		void Detach(PlanningMemberClass * member);
		void Clear(void);
		bool Is_Loop_Start(TechnoClass * techno);
		void Deactivate(PlanningMemberClass * member);
		void Locate_Loop_Branch(void);
		void Select_All(void);
		void Unselect_All(void);
		void Fill_Lead_Box(int color);
		void Frame_Lead_Box(int color);
		int Node_Color(void);
		void Reset_Colors(bool all);
		static void FUN_00637640(PlanningNodeClass * node);

		DynamicVectorClass<PlanningMemberClass *> PlanningMembers;
		int Id;
		bool Queued;
		DynamicVectorClass<PlanningBranchClass *> PlanningBranches;
		PlanningBranchClass LoopBranch;
		int LoopBranchIndex;
		int Color;
};

class PlanningTokenClass
{
	public:
		static DynamicVectorClass<PlanningTokenClass *> Array;
		static DynamicVectorClass<EventClass *> Events;
		static DynamicVectorClass<TechnoClass *> Technos;

		PlanningTokenClass(TechnoClass * owner);
		~PlanningTokenClass();
		TechnoClass * Get_Owner(void);
		void Cancel_Owner(void);
		void Destroy(void);
		void Set_F1C(void);
		void Clear_F1C(void);
		bool Get_F1C(void);
		bool Owner_Is_Mission(void);
		bool Has_Started(void);
		EventClass Pop_Front(void);
		void Share_Range(TechnoClass * techno, int start, int count);
		void Share_From(TechnoClass * techno, int start);
		int Get_F8C(void);
		bool Reached(int index);
		int Node_Count(void);
		int Current_Or_Count(void);
		int Remaining(void);
		void Add_Node(PlanningNodeClass * node);
		PlanningNodeClass * Node_At(int index);
		PlanningNodeClass * Last_Node(void);
		bool Has_Node(PlanningNodeClass * node);
		int Node_Index(PlanningNodeClass * node);
		EventClass Event_At(int index);
		EventClass Last_Popped(void);
		PlanningNodeClass * Loop_Node(void);
		void Set_Loop(int count, int start);
		int Get_Loop_Count(void);
		int Get_Loop_Start(void);
		bool Loop_Start_Valid(void);
		bool Loop_Start_Positive(void);
		bool Loop_Start_Zero(void);
		bool Get_F98(void);
		bool Get_F99(void);
		void Dec_Loop_Start(void);

		TechnoClass * OwnerUnit;
		DynamicVectorClass<PlanningNodeClass *> PlanningNodes;
		bool F1C;
		EventClass Popped;
		int F8C;
		int ClosedLoopNodeCount;
		int StepsToClosedLoop;
		bool F98;
		bool F99;
};
