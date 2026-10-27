// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "timer.h"
#include "vector.h"

class Cell;
class CellClass;
class HouseClass;
class InfantryClass;
class InfantryTypeClass;
class TechnoClass;

class SlaveManagerClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		struct SlaveControl
		{
			InfantryClass * Slave;
			int State;
			CDTimerClass<FrameTimerClass> RespawnTimer;

			SlaveControl(void) : RespawnTimer(0) {}
		};

		TechnoClass * Owner;
		InfantryTypeClass * SlaveType;
		int SlaveCount;
		int RegenRate;
		int ReloadRate;
		DynamicVectorClass<SlaveControl *> SlaveNodes;
		CDTimerClass<FrameTimerClass> RespawnTimer;
		int State;
		int LastScanFrame;

		SlaveManagerClass(void);
		SlaveManagerClass(TechnoClass * owner, InfantryTypeClass * type, int number, int regen, int reload);
		SlaveManagerClass(NoInitClass const & x);
		virtual ~SlaveManagerClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void AI(void) override;

		void SetOwner(TechnoClass * owner);
		void CreateSlave(SlaveControl * node);
		void FUN_006AF6C0(void);
		void FUN_006AFD60(void);
		Cell FUN_006B0300(Cell cell);
		void FUN_006B0490(void);
		void FUN_006B0A90(int index);
		void FUN_006B04C0(void);
		Cell FUN_006B0690(void);
		Cell FUN_006B0750(void);
		bool FUN_006B0260(void);
		Cell FUN_006B02C0(void);
		bool FUN_006B0880(TechnoClass * slave, CellClass * cell);
		void LostSlave(InfantryClass * slave);
		void Killed(TechnoClass * killer, HouseClass * owner = NULL);
		void AllGuard(void);
		bool ShouldWakeUpNow(void);
		void FUN_006B0CC0(void);
		void FUN_006B0D10(void);
		void FUN_006B0DB0(void);
		void Deploy2(void);
};

extern DynamicVectorClass<SlaveManagerClass *> SlaveManagers;
