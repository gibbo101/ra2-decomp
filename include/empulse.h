// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "coord.h"
#include "noinit.h"
#include "rect.h"

class Cell;
class TechnoClass;
template<class T> class DynamicVectorClass;

class EMPulseClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		EMPulseClass(Cell cell, int spread, int duration, TechnoClass *source);
		EMPulseClass(void);
		EMPulseClass(NoInitClass const & x);
		virtual ~EMPulseClass(void) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(EMPulseClass));}
		virtual void Compute_CRC(CRCEngine &crc) const override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_EMPULSE);}

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static void Update_All(void);
		static void Reset(void);

		static DynamicVectorClass<EMPulseClass *> EMPulses;

	private:
		void Create(TechnoClass * source);
		void Destroy(void);

		Cell CellID;

		int Spread;

		int CreationFrame;

		int Duration;
};

void Update_EMPulses(void);
