// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"

class LightConvertClass;
template<class T> class DynamicVectorClass;

class LightSourceClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		LightSourceClass(Coord coord, int visibility, int intensity, int red, int green, int blue);
		LightSourceClass(void);
		LightSourceClass(NoInitClass const & x);
		virtual ~LightSourceClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static void Reset(void);
		static void Process_Lighting(int time_budget_ms, bool force = false);

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;

		bool Is_Enabled(void) const {return(IsEnabled);};
		void Enable(bool update = false);
		void Disable(bool update = false);
		void Recalculate_Affected_Cells(bool defer = false);

	public:
		class PendingCellClass
		{
			public:
				PendingCellClass(Cell cell) :
					Converter(NULL),
					Intensity(0x10000),
					Ambient(0),
					Brightness(1000),
					TileBrightness(1000),
					AltBrightness(1000),
					CellID(cell)
				{
				}

			public:

				LightConvertClass * Converter;

				unsigned int Intensity;
				unsigned short Ambient;
				unsigned short Brightness;
				unsigned short TileBrightness;
				unsigned short AltBrightness;

				Cell CellID;
		};

	public:

		int Intensity;

		int RedTint;
		int GreenTint;
		int BlueTint;

		int DetailLevel;

		Coord Position;

		int Visibility;

	private:

		bool IsEnabled;

		static DynamicVectorClass<PendingCellClass *> PendingCells;

	public:

		static bool Recalc;
};

extern DynamicVectorClass<LightSourceClass *> LightSources;
