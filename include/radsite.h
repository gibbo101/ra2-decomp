// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "light.h"
#include "timer.h"
#include "vector.h"

class RadSiteClass;

EXTERN_C const CLSID CLSID_RadSiteClass;
extern DynamicVectorClass<RadSiteClass *> RadSites;

struct TintStruct
{
	int Red;
	int Green;
	int Blue;
};

class RadSiteClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		LightSourceClass * LightSource;
		CDTimerClass<FrameTimerClass> RadLevelTimer;
		CDTimerClass<FrameTimerClass> RadLightTimer;
		Cell BaseCell;
		int Spread;
		int SpreadInLeptons;
		int RadLevel;
		int LevelSteps;
		int Intensity;
		TintStruct Tint;
		int IntensitySteps;
		int IntensityDecrement;
		int RadDuration;
		int RadTimeLeft;

		RadSiteClass(void);
		RadSiteClass(NoInitClass const & x);
		virtual ~RadSiteClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void AI(void) override;

		void SetBaseCell(Cell * cell);
		void SetSpread(int spread);
		void SetRadLevel(int level);
		int GetRadLevel(void);
		void Add(int level);
		void Activate(void);
		void Deactivate(void);
		double GetRadLevelAt(Cell const & cell);
		void Radiate(void);
		void DecreaseRadiation(void);
		void DecreaseLight(void);
		int GetSpread(void);
};
