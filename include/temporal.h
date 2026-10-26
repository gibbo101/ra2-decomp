// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "timer.h"
#include "vector.h"

EXTERN_C const CLSID CLSID_TemporalClass;

class TemporalClass;
extern DynamicVectorClass<TemporalClass *> Temporals;

class SuperClass;
class TechnoClass;

class TemporalClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		TechnoClass * Owner;
		TechnoClass * Target;
		CDTimerClass<FrameTimerClass> LifeTimer;
		void * unknown_pointer_38;
		SuperClass * SourceSW;
		TemporalClass * NextTemporal;
		TemporalClass * PrevTemporal;
		int WarpRemaining;
		int WarpPerStep;

		TemporalClass(void);
		TemporalClass(TechnoClass * owner);
		TemporalClass(NoInitClass const & x);
		virtual ~TemporalClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void AI(void) override;

		int GetWarpPerStep(int helpercount = 0);
		void Detach(void);
		void LetGo(void);
		void JustLetGo(void);
		void FUN_0071AB60(AbstractClass * ptr);
};
