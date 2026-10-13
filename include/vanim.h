// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audioctl.h"
#include "bounce.h"
#include "object.h"

class VoxelAnimTypeClass;
class ParticleSystemClass;
class HouseClass;
class Coord;

class VoxelAnimClass : public ObjectClass, public BounceClass
{
		typedef ObjectClass BASECLASS;

	public:

		VoxelAnimClass(void);
		VoxelAnimClass(VoxelAnimTypeClass const * type, Coord const & coord, HouseClass * house = NULL);
		VoxelAnimClass(NoInitClass const & x) : BASECLASS(x), BounceClass(x) {}
		virtual ~VoxelAnimClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual int Fetch_Object_Size(void) const override;
		virtual RTTIType Fetch_RTTI(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;

		virtual bool Render(Rect &cliprect, bool forced, bool extras_only) const override;
		virtual void Draw_It(Point2D const &point, Rect const &cliprect) const override;
		virtual void AI(void) override;
		virtual LayerType In_Which_Layer(void) const override;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual Cell const * Occupy_List(bool placement=false) const override;

		static void Init_Clear(void);

		void Make_Invisible(void) {IsInvisible = true;}
		void Make_Visible(void) {IsInvisible = false;}

	public:

		int Unused1;

		VoxelAnimTypeClass * Class;

		ParticleSystemClass * AttachedParticleSys;

		HouseClass * House;

		bool IsToDie;

		AudioController Audio3;

		AudioController Audio4;

		bool IsInvisible;

		int ECCounter;
};
