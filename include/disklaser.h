// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "noinit.h"
#include "point.h"

class TechnoClass;
class WeaponTypeClass;
template<class T> class DynamicVectorClass;

class DiskLaserClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		TechnoClass * Owner;
		TechnoClass * Target;
		WeaponTypeClass * Weapon;
		int unknown_30;
		int unknown_34;
		int unknown_38;
		int Damage;

		DiskLaserClass(void);
		DiskLaserClass(NoInitClass const & x);
		virtual ~DiskLaserClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void AI(void) override;

		void Mark_For_Removal(void);
		void PointerGotInvalid(AbstractClass * invalid);
		void Fire(TechnoClass * owner, TechnoClass * target, WeaponTypeClass * weapon, int damage);

		static Point2D DrawCoords[16];
};

extern DynamicVectorClass<DiskLaserClass *> DiskLasers;
