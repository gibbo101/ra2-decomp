// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "objtype.h"

#include "vanim.hh"

class AnimTypeClass;
class WarheadTypeClass;
class ParticleSystemTypeClass;

class VoxelAnimTypeClass : public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:

		VoxelAnimTypeClass(NoInitClass const & x) : BASECLASS(x) {}
		VoxelAnimTypeClass(char const * ininame = NULL);
		~VoxelAnimTypeClass(void);

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static VoxelAnimTypeClass * Find_Or_Make(char const * name);
		static VoxelAnimType From_Name(char const * name);
		static char const * Name_From(VoxelAnimType anim);

		virtual void Detach(AbstractClass const * target, bool all = true) override;

		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_VOXELANIMTYPE);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		virtual bool Read_INI(CCINIClass & ini) override;

		virtual bool Create_And_Place(Cell const & cell, HouseClass * house) const override {return(false);}
		virtual ObjectClass * Create_One_Of(HouseClass *) const override {return(NULL);}

	public:

		bool IsNormalized;

		bool IsTranslucent;

		bool IsSharesSourceData;
		unsigned char unused_297;

		int VoxelIndex;

		int Duration;

		double Elasticity;

		double MinAngularVelocity;
		double MaxAngularVelocity;

		double MinZVel;
		double MaxZVel;

		double MaxXYVel;

		bool IsMeteor;
		unsigned char unused_2D1[3];

		VoxelAnimTypeClass const * Spawns;

		int SpawnCount;

		VocType StartSound;
		VocType StopSound;

		AnimTypeClass const * BounceAnim;

		AnimTypeClass const * ExpireAnim;

		AnimTypeClass const * TrailerAnim;

		int Damage;

		int DamageRadius;

		WarheadTypeClass const * Warhead;

		ParticleSystemTypeClass const * AttachedSystem;

		bool IsTiberium;
		unsigned char unused_301[3];
};

char const * VoxelAnim_Name(VoxelAnimType anim);
