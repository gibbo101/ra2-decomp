// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "face.h"
#include "loco.h"

#include "mark.hh"

class MechLocomotionClass : public LocomotionClass
{
		typedef LocomotionClass BASECLASS;

	public:

		MechLocomotionClass(void);
		MechLocomotionClass(NoInitClass const & x) : BASECLASS(x) {}
		virtual ~MechLocomotionClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;

		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Head_To_Coord(void) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) override;
		virtual void STDMETHODCALLTYPE Stop_Moving(void) override;
		virtual void STDMETHODCALLTYPE Do_Turn(DirStruct coord) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;
		virtual void STDMETHODCALLTYPE Force_Immediate_Destination(CoordStruct coord) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Now(void) override;
		virtual void STDMETHODCALLTYPE Mark_All_Occupation_Bits(int mark) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Here(CoordStruct to) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

	private:
		Coord DestinationCoord;

		Coord HeadToCoord;

		bool IsMoving;

		void Movement_AI(bool continue_moving);
		bool Mark_Head_To(Coord const & coord);
};
