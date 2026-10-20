// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "objtype.h"
#include "rgb.h"
#include "typelist.h"

#include "particle.hh"
#include "partsys.hh"

class ParticleSystemTypeClass : public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:
		ParticleSystemTypeClass(char const * ininame = NULL);
		ParticleSystemTypeClass(NoInitClass const & x);
		virtual ~ParticleSystemTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;

		virtual bool Read_INI(CCINIClass & ini) override;
		virtual bool Create_And_Place(Cell const & , HouseClass * house = NULL) const override;
		virtual ObjectClass * Create_One_Of(HouseClass *) const override;

		static ParticleSystemType From_Name(char const * name);
		static char const * Name_From(ParticleSystemType partsys);
		static ParticleSystemTypeClass * FindOrAllocate(char const * name);
		static ParticleSystemTypeClass * Find_Or_Make(char const * name);

		ParticleSystemBehaviorType Behaves_Like(void) const { return(BehavesLike); }

	public:

		ParticleType HoldsWhat;

		bool Spawns;

		int SpawnFrames;

		float Slowdown;

		int ParticleCap;

		int SpawnRadius;

		float SpawnCutoff;

		float SpawnTranslucencyCutoff;

		ParticleSystemBehaviorType BehavesLike;

		int Lifetime;

		TPoint3D<float> SpawnDirection;

		double ParticlesPerCoord;

		double SpiralDeltaPerCoord;

		double SpiralRadius;

		double PositionPerturbationCoefficient;

		double MovementPerturbationCoefficient;

		double VelocityPerturbationCoefficient;

		double SpawnSparkPercentage;

		int SparkSpawnFrames;

		int LightSize;

		RGBClass LaserColor;

		bool IsLaser;

		bool OneFrameLight;
};

ParticleSystemBehaviorType Particle_System_Behavior_From_Name(char const * name);
