// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "timer.h"
#include "vector.h"

class AircraftClass;
class AircraftTypeClass;
class TechnoClass;

enum SpawnNodeStatus {
	SPAWN_IDLE = 0,
	SPAWN_TAKEOFF = 1,
	SPAWN_PREPARING = 2,
	SPAWN_ATTACKING = 3,
	SPAWN_RETURNING = 4,
	SPAWN_RELOADING = 6,
	SPAWN_DEAD = 7
};

class SpawnManagerClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		struct SpawnControl
		{
			AircraftClass * Unit;
			int Status;
			CDTimerClass<FrameTimerClass> SpawnTimer;
			BOOL IsSpawnMissile;
		};

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

		SpawnManagerClass(void);
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
		bool UpdateTarget(void);
		void UnlinkPointer(AbstractClass * ptr);
		void SetTarget(AbstractClass * target);
		void ResetTarget(void);
		int CountLaunchingSpawns(void);
		int CountAliveSpawns(void);
		int CountDockedSpawns(void);
};

extern DynamicVectorClass<SpawnManagerClass *> SpawnManagers;
