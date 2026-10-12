// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "objtype.h"
#include "rgb.h"
#include "typelist.h"

#include "particle.hh"

class WarheadTypeClass;
class ParticleTypeClass;

class ParticleTypeClass : public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:
		ParticleTypeClass(char const * ininame = NULL);
		ParticleTypeClass(NoInitClass const & x);
		virtual ~ParticleTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;

		virtual bool Read_INI(CCINIClass & ini) override;
		virtual bool Create_And_Place(Cell const & , HouseClass * house = NULL) const override;
		virtual ObjectClass * Create_One_Of(HouseClass *) const override;

		static ParticleType From_Name(char const * name);
		static const char * Name_From(ParticleType particle);
		static ParticleTypeClass * Find_Or_Make(char const * name);

	public:

		Point3D NextParticleOffset;

		int XVelocity;
		int YVelocity;

		int MinZVelocity;
		int ZVelocityRange;

		double ColorSpeed;

		TypeList<RGBClass> ColorList;

		RGBClass StartColor1;
		RGBClass StartColor2;

		int MaxDC;

		int MaxEC;

		WarheadTypeClass * Warhead;

		int Damage;

		int StartFrame;
		int NumLoopFrames;

		int Translucency;

		int WindEffect;

		float Velocity;

		float Deacc;

		int Radius;

		bool DeleteOnStateLimit;

		char EndStateAI;

		char StartStateAI;

		char StateAIAdvance;

		char FinalDamageState;

		unsigned char Translucent25State;
		unsigned char Translucent50State;

		bool IsNormalized;

		ParticleType NextParticle;

		ParticleBehaviorType BehavesLike;
};

ParticleBehaviorType Particle_Behavior_From_Name(char const * name);
