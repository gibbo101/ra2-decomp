// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "timer.h"
#include "vector.h"

class AircraftTypeClass;
class TechnoClass;
struct SpawnControl;

class SpawnManagerClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		TechnoClass * Owner;
		AircraftTypeClass * SpawnType;
		int SpawnCount;
		int RegenRate;
		int ReloadRate;
		DynamicVectorClass<SpawnControl *> SpawnedNodes;
		CDTimerClass<FrameTimerClass> UpdateTimer;
		CDTimerClass<FrameTimerClass> SpawnTimer;
		AbstractClass * Target;
		AbstractClass * NewTarget;
		int Status;

		SpawnManagerClass(TechnoClass * owner, AircraftTypeClass * type, int number, int regen, int reload);
		SpawnManagerClass(NoInitClass const & x);
		virtual ~SpawnManagerClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void AI(void) override;

		void KillNodes(void);
		void UnlinkPointer(AbstractClass * ptr);
		void SetTarget(AbstractClass * target);
		void ResetTarget(void);
		int CountLaunchingSpawns(void);
};
