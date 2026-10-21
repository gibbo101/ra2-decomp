// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ftimer.h"
#include "nodes.h"
#include "object.h"
#include "priority.h"
#include "timer.h"
#include "stage.h"

#include "theater.hh"

template<class T>
class DynamicVectorClass;

struct VeinholeLogic {
	int GrowthCount;
	PriorityQueueClass<CellNode> * GrowthQueue;
	CellNode * GrowthNodes;
	CDTimerClass<FrameTimerClass> GrowthTimer;
	bool * GrowthState;
	VeinholeLogic(void) {}
	VeinholeLogic(bool rules);
};

class VeinholeMonsterClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:

		VeinholeMonsterClass(NoInitClass const & x);
		VeinholeMonsterClass(Cell const & cell);
		~VeinholeMonsterClass(void);

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static VeinholeMonsterClass * Get_Monster_At(Cell const & cell);
		static VeinholeMonsterClass * Get_Vein_Owner_At(Cell const & cell);

		static void Update_All(void);
		virtual void AI(void) override;
		static void Draw_All(void);
		static void Init(TheaterType theater);
		void Draw_It(void);
		virtual ResultType Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source=0, bool forced=false, bool=false, HouseClass * =0) override;
		static bool Can_Monster_Go_Here(Cell const & cell);
		static void Reset(void);
		void Grow(void);
		void Shrink(void);
		static void Init_Vein_Growth_System(bool clear);
		static void Deinit_Vein_Growth_System(void);
		void Build_Growth_Queue(void);
		void Build_Shrinking_Queue(void);
		static void Clear_Global_Data(void);
		void Clear_Growth(void);
		void Destroy_Monster(void);
		static void Remove_Dead(void);
		static bool Load_All(IStream * stream);
		static bool Save_All(IStream * stream);
		static TerrainTypeClass const * __fastcall GetTerrainType(void);
		void Reduce_Veins_At(CellClass * cellptr);

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_VEINHOLEMONSTER);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual LayerType In_Which_Layer(void) const override {return(LAYER_NONE);}

		virtual ObjectTypeClass const * Class_Of(void) const override;

	public:

		VeinholeLogic GrowthLogic;

		int CurrentState;

		int DesiredState;

		StageClass Control;

		CDTimerClass<FrameTimerClass> LogicTimer;

		Cell CellID;

		int ShapeFrame;

		bool IsDead;

		bool IsToPuffGas;

		int VeinCount;

		static DynamicVectorClass<VeinholeMonsterClass *> VeinholeMonsters;

		enum {
			IDLE,
			ALERT,
			ATTACKING,
			DYING,
			STATE_COUNT,

			FRAME_COUNT = 12,
		};

	private:

		static void const * MonsterShape;
};
