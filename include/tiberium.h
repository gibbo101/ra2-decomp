// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"
#include "coord.h"
#include "ftimer.h"
#include "nodes.h"
#include "priority.h"
#include "timer.h"
#include "typelist.h"

#include "tiberium.hh"

class CCINIClass;
class OverlayTypeClass;
class AnimTypeClass;

class TiberiumClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:

		TiberiumClass(char const * ininame = NULL);
		TiberiumClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~TiberiumClass() override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;
		virtual HRESULT STDMETHODCALLTYPE GetSizeMax(ULARGE_INTEGER *pcbSize) override;

		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual int Fetch_Object_Size(void) const override { return(sizeof(*this)); }
		virtual RTTIType Fetch_RTTI(void) const override { return(RTTI_TIBERIUM); }
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual int Fetch_Heap_ID(void) const override { return(HeapID); }
		virtual bool Read_INI(CCINIClass & ini) override;

		static bool Process(CCINIClass & ini);

		static void Tiberium_Spread(void);
		static void Init_Tiberium_Spread_System(void);
		static void Deinit_Tiberium_Spread_System(void);

		void Spread_AI(void);
		void Init_Spread(void);
		void Recalc_Spread(void);
		void Clear_Spread(void);
		void Queue_Spread(Cell const & cell);

		static void Clear_Spread_State(Cell const & cell);

		static void Tiberium_Growth(void);
		static void Init_Tiberium_Growth_System(void);
		static void Deinit_Tiberium_Growth_System(void);

		void Growth_AI(void);
		void Init_Growth(void);
		void Recalc_Growth(void);
		void Clear_Growth(void);
		void Queue_Growth(Cell const & cell);

		static void Post_Load_Game(void);

		TiberiumType HeapID;

		int SpreadDelay;

		double SpreadPercentage;

		int GrowthDelay;

		double GrowthPercentage;

		int CreditValue;

		int Power;

		int Color;

		TypeList<AnimTypeClass const *> Debris;

		OverlayTypeClass const * Overlay;

		int FrameCount;

		int Variety;

		int RampVariety;

		int SpreadCount;

		PriorityQueueClass<CellNode> * SpreadQueue;

		bool * SpreadState;

		CellNode * SpreadNodes;

		CDTimerClass<FrameTimerClass> SpreadTimer;

		int GrowthCount;

		PriorityQueueClass<CellNode> * GrowthQueue;

		bool * GrowthState;

		CellNode * GrowthNodes;

		CDTimerClass<FrameTimerClass> GrowthTimer;
};

extern DynamicVectorClass<TiberiumClass *> Tiberiums;
