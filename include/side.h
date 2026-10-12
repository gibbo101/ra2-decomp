// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"
#include "typelist.h"

#include "side.hh"

class SideClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		SideClass(char const * ininame = NULL);
		SideClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~SideClass() override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_SIDE);}
		virtual void Compute_CRC(CRCEngine &) const override;

		static SideType From_Name(char const * name);

	public:

		TypeList<int> Houses;
};
