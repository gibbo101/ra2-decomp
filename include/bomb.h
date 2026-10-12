// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "audioctl.h"

class HouseClass;
class ObjectClass;
class TechnoClass;

class BombClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		TechnoClass * Owner;
		HouseClass * OwnerHouse;
		ObjectClass * Target;
		BOOL DeathBomb;
		int PlantingFrame;
		int DetonationFrame;
		AudioController Audio;
		int TickSound;
		BOOL ShouldPlayTickingSound;
		bool Harmless;

		BombClass(void);
		virtual ~BombClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(bool oldsave) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;

		void Detonate(void);
		void Disarm(void);
		BOOL Is_Death_Bomb(void) const;
		int Get_Current_Flicker_Frame(void) const;
		bool Time_To_Explode(void) const;
};
