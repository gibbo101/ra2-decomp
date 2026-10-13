// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ftimer.h"
#include "loco.h"
#include "timer.h"

class TunnelLocomotionClass : public LocomotionClass
{
		typedef LocomotionClass BASECLASS;

	public:

		TunnelLocomotionClass(void);
		TunnelLocomotionClass(NoInitClass const & x) : BASECLASS(x) {}

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;

		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Now(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) override;
		virtual void STDMETHODCALLTYPE Stop_Moving(void) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual VisualType STDMETHODCALLTYPE Visual_Character(boolean flag) override;
		virtual Matrix3DStruct STDMETHODCALLTYPE Draw_Matrix(int *key) override;
		virtual int STDMETHODCALLTYPE Z_Adjust(void) override;
		virtual ZGradientType STDMETHODCALLTYPE Z_Gradient(void) override;
		virtual boolean STDMETHODCALLTYPE Is_To_Have_Shadow(void) override;
		virtual MoveType STDMETHODCALLTYPE Can_Enter_Cell(CellStruct cell) override;
		virtual void STDMETHODCALLTYPE Do_Turn(DirStruct coord) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;
		virtual FireErrorType STDMETHODCALLTYPE Can_Fire(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Surfacing(void) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		void Process_Turning(void);
		void Process_Digging_In(void);
		void Process_Descending(void);
		void Process_Tunneling(void);
		void Process_Ascending(void);
		void Process_Emerging(void);
		void Process_Aborting(void);

	private:

		enum {
			STATE_IDLE,
			STATE_TURNING,
			STATE_DIGGING_IN,
			STATE_DESCENDING,
			STATE_TUNNELING,
			STATE_ASCENDING,
			STATE_EMERGING,
			STATE_ABORTING,
		} State;

		Coord DestinationCoord;
		ProgressTimerClass<FrameTimerClass> DigTimer;
		bool IsUnderground;
};
