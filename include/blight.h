// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "object.h"

#include "blight.hh"

class BuildingLightClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:
		BuildingLightClass(TechnoClass * owner = NULL);
		BuildingLightClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~BuildingLightClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_LIGHT);}
		virtual LayerType In_Which_Layer(void) const override;

		virtual bool Limbo(void) override;
		virtual bool Unlimbo(Coord const & , Dir256 facing=DIR_N) override;

		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;

		virtual void AI(void) override;

		void Init_Rotation_Arc(TechnoClass * obj);
		void Set_Behavior_Type(LightBehaviorType type);
		int Detection_Radius(void) const;
		int Sweep_Stage(void) const;

	public:

		double Speed;

		Coord RotationPivot;

		Coord RotationTarget;

		double Acceleration;

		bool IsOppositeDirection;

		int Behavior;

		TechnoClass * Target;

		TechnoClass * Owner;
};
