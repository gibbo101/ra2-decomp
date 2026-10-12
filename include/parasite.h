// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "timer.h"

class AnimClass;
class FootClass;

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

		ParasiteClass(FootClass * owner = NULL);
		virtual ~ParasiteClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(bool oldsave) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void AI(void) override;
};
