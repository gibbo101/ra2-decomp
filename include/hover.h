// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "face.h"
#include "facing.h"
#include "ftimer.h"
#include "ipiggy.h"
#include "loco.h"
#include "matrix3d.h"
#include "timer.h"

#include "mark.hh"
#include "move.hh"

class HoverLocomotionClass : public LocomotionClass
{
		typedef LocomotionClass BASECLASS;

	public:

		HoverLocomotionClass(void);
		HoverLocomotionClass(NoInitClass const & x) : BASECLASS(x) {}
		virtual ~HoverLocomotionClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;

		virtual HRESULT STDMETHODCALLTYPE Link_To_Object(void *pointer) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Head_To_Coord(void) override;
		virtual Matrix3DStruct STDMETHODCALLTYPE Draw_Matrix(int *key) override;
		virtual Matrix3DStruct STDMETHODCALLTYPE Shadow_Matrix(int *key) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) override;
		virtual void STDMETHODCALLTYPE Stop_Moving(void) override;
		virtual void STDMETHODCALLTYPE Do_Turn(DirStruct coord) override;
		virtual boolean STDMETHODCALLTYPE Power_Off(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Powered(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Ion_Sensitive(void) override;
		virtual boolean STDMETHODCALLTYPE Push(DirStruct dir) override;
		virtual boolean STDMETHODCALLTYPE Shove(DirStruct dir) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Now(void) override;
		virtual void STDMETHODCALLTYPE Mark_All_Occupation_Bits(int mark) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Here(CoordStruct to) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

	private:

		Coord DestinationCoord;

		Coord HeadToCoord;

		FacingClass Facing;

		double Height;

		double Acceleration;

		double Boost;

		double Bounciness;

		bool WasShoved;

		int ShoveAccum;

		bool WasPushed;

		void Gravity_AI(void);
		void Do_Shove(void);
		MoveType While_Moving(bool);
		bool Is_Moving1(void);
		void Stop(void);
		void Stop_Driver(void);
		void Motion_AI(void);
		bool Reduce_Path_Length(void);
		void Start_Of_Move(int);
		void Start(void);

};
