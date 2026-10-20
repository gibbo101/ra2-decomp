// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"

class TechnoClass;
class WeaponTypeClass;

class DiskLaserClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		TechnoClass * Owner;
		TechnoClass * Target;
		WeaponTypeClass * Weapon;
		DWORD unknown_30;
		DWORD unknown_34;
		DWORD unknown_38;
		int Damage;

		DiskLaserClass(void);
		virtual ~DiskLaserClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void AI(void) override;

		void Fire(TechnoClass * owner, TechnoClass * target, WeaponTypeClass * weapon, int damage);
};
