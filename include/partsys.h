// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "globals.h"
#include "noinit.h"
#include "object.h"
#include "vector.h"

#include "layer.hh"

template<class T> class DynamicVectorClass;
class ParticleSystemTypeClass;
class ObjectTypeClass;
class AbstractClass;
class ParticleClass;
class ParticleTypeClass;
class HouseClass;

class ParticleSystemClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:
		ParticleSystemClass(NoInitClass const & x) : BASECLASS(x) {};
		ParticleSystemClass(ParticleSystemTypeClass const * system, Coord const & coord1, AbstractClass * = NULL, AbstractClass * = NULL, Coord const & coord2 = COORD_NONE, HouseClass * = NULL);
		ParticleSystemClass(void);
		virtual ~ParticleSystemClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual void Delete_Me(void) override { IsMarkedForDeletion = true; }

		virtual void Compute_CRC(CRCEngine & crc) const override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;

		virtual void Detach(AbstractClass const * target, bool all = true) override;;

		virtual bool Is_Inactive(void) const override;

		void Sparks_To_Use_Random_Direction(void) { IsRandomSparkDirection = true; }

		AbstractClass *Source_Object(void) const { return(Source); }
		AbstractClass *Target_Object(void) const { return(Target); }

		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual LayerType In_Which_Layer(void) const override;
		virtual void AI(void) override;

		ParticleClass * Spawn_Held_Particle(Coord const & coord1, Coord const & coord2);
		ParticleClass * Spawn_Held_Particle_Random(Coord const & coord1, Coord const & coord2, int v);
		ParticleClass * Spawn_Particle(ParticleTypeClass *type, Coord const & coord);
		void Clear_System(void);
		bool Delete_Particle(int index);
		void FUN_006301F0(int lifetime);

	private:
		void Gas_AI(void);
		void Railgun_AI(void);
		void Smoke_AI(void);
		void Spark_AI(void);
		void Fire_AI(void);

	public:

		ParticleSystemTypeClass * Class;
	private:

		Coord CoordOffset;

		DynamicVectorClass<ParticleClass *> SystemParticles;

		Coord SpawnCoord;

		AbstractClass * Source;

		AbstractClass * Target;
	public:

		float SpawnFrames;

		int Lifetime;

		int SparkSpawnFrames;

		int SparkRadius;

		bool IsMarkedForDeletion;

		bool IsRandomSparkDirection;
		HouseClass * OwnerHouse;

};
