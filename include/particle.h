// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "object.h"
#include "rgb.h"
#include "vector3.h"

class ParticleTypeClass;
class ParticleSystemClass;

class ParticleClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:
		ParticleClass(void);
		ParticleClass(ParticleTypeClass const * type, Coord const & origin, Coord const & target = COORD_NONE, ParticleSystemClass * partsys = NULL);
		ParticleClass(NoInitClass const & x) : BASECLASS(x), Color(x) {};
		virtual ~ParticleClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;

		virtual LayerType In_Which_Layer(void) const override;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual Cell const * Occupy_List(bool placement=false) const override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual bool Mark(MarkType=MARK_CHANGE) override;

		virtual int Shape_Number(void) const;

		void Gas_Behavior_AI(void);
		void Railgun_Behavior_AI(void);
		void Smoke_Behavior_AI(void);
		void Spark_Behavior_AI(void);
		void Fire_Behavior_AI(void);
		void Web_Behavior_AI(void);
		void Behavior_AI(void);

		void Smoke_Motion_AI(void);
		void Gas_Motion_AI(void);
		void Fire_Motion_AI(void);
		void Motion_AI(void);

	public:

		ParticleTypeClass const * Class;

		RGBClass Color;
		int ColorIndex;
		double ColorAccum;

		Coord GasDrift;
		Vector3 GasVelocity;

		Coord UnusedCoord1;

		float Speed;

		Coord FireTarget;
		Coord FireOrigin;
		Coord FireMoveDelta;

		Vector3 MovementDirection;

		Vector3 PrecisePosition;

		ParticleSystemClass * System;

		unsigned short RemainingEC;
		unsigned short RemainingDC;
		char StateAIAdvance;
		bool IsFireBelowGround;
		char StateAI;

		signed char Translucency;

		bool WasSaved;

		bool IsToDie;

	private:

		static LEPTON SmokeWindX[FACING_COUNT];
		static LEPTON SmokeWindY[FACING_COUNT];

		static LEPTON GasWindX[FACING_COUNT];
		static LEPTON GasWindY[FACING_COUNT];
};
