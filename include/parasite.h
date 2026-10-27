// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "vector.h"
#include "ftimer.h"
#include "timer.h"

class AnimClass;
class FootClass;

class ParasiteClass;

EXTERN_C const CLSID CLSID_ParasiteClass;
extern DynamicVectorClass<ParasiteClass *> Parasites;

class ParasiteClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		FootClass * Owner;
		FootClass * Victim;
		CDTimerClass<FrameTimerClass> SuppressionTimer;
		CDTimerClass<FrameTimerClass> DamageDeliveryTimer;
		AnimClass * GrappleAnim;
		int GrappleState;
		int GrappleAnimFrame;
		int GrappleAnimDelay;
		bool GrappleAnimGotInvalid;

		ParasiteClass(void);
		ParasiteClass(NoInitClass const & x);
		ParasiteClass(FootClass * owner);
		virtual ~ParasiteClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void AI(void) override;

		void FUN_00629530(void);
		void FUN_00629710(void);
		Coord FUN_0062AC30(void);
		bool UpdateGrapple(void);
		void UpdateSquid(void);
		void ExitUnit(void);
		bool CanInfect(FootClass * victim) const;
		void TryInfect(FootClass * target);
		bool CanExistOnVictimCell(void) const;
};
