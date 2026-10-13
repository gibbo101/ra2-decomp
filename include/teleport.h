// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "loco.h"

class TeleportLocomotionClass : public LocomotionClass
{
		typedef LocomotionClass BASECLASS;

	public:
		TeleportLocomotionClass(void);
		TeleportLocomotionClass(const NoInitClass & x) : BASECLASS(x) {}

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;

		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) override;
		virtual void STDMETHODCALLTYPE Stop_Moving(void) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		virtual boolean Is_Stationary(void);

	private:

		Coord DestinationCoord;
};
