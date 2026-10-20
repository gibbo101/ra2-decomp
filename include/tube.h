// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "coord.h"
#include "globals.h"

#include "facing.hh"

class CCINIClass;

class TubeClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

	public:
		TubeClass(Cell const &cell = CELL_NONE, FacingType dir = FACING_N);

		TubeClass(NoInitClass const & x) {};

		virtual ~TubeClass(void) override;

		virtual int Fetch_Object_Size(void) const override;
		virtual RTTIType Fetch_RTTI(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;

		static void Assign_Tubes(void);
		static void Write_INI(CCINIClass & ini);
		static void Read_INI(CCINIClass const & ini);

		bool Not_In_Tube(Cell const & cell) const;
		void Add_Direction(FacingType dir);

	public:

		Cell Enter;

		Cell Exit;

		FacingType EnterDir;

		FacingType Dirs[100];

		int Count;

		static char const * INI_NAME;
};
