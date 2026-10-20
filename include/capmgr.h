// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "timer.h"
#include "vector.h"

class HouseClass;
class TechnoClass;
struct ControlNode;

class CaptureManagerClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		DynamicVectorClass<ControlNode *> ControlNodes;
		int MaxControlNodes;
		bool InfiniteMindControl;
		bool OverloadDeathSoundPlayed;
		int OverloadPipState;
		TechnoClass * Owner;
		int OverloadDamageDelay;

		CaptureManagerClass(void);
		CaptureManagerClass(TechnoClass * owner, int damage, bool infinite);
		CaptureManagerClass(NoInitClass const & x);
		virtual ~CaptureManagerClass(void) override;

		void FreeAll(void);
		bool FreeUnit(TechnoClass * techno);
		bool FUN_00472330(TechnoClass * techno);
		bool CannotControlAnyMore(void);
		bool FUN_00471F90(AbstractClass * ptr);
		int GetControlledCount(void) const;
		HouseClass * GetOriginalOwner(TechnoClass * techno) const;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
};
