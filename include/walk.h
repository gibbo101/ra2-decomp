// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ipiggy.h"
#include "loco.h"

class WalkLocomotionClass : public LocomotionClass, public IPiggyback
{
		typedef LocomotionClass BASECLASS;

	public:

		WalkLocomotionClass(void);
		WalkLocomotionClass(NoInitClass const & x) : BASECLASS(x), Piggybacker(NULL) {}
		virtual ~WalkLocomotionClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override {return(BASECLASS::AddRef());}
		virtual ULONG STDMETHODCALLTYPE Release(void) override {return(BASECLASS::Release());}

		virtual HRESULT STDMETHODCALLTYPE Begin_Piggyback(ILocomotion * pointer) override;
		virtual HRESULT STDMETHODCALLTYPE End_Piggyback(ILocomotion ** pointer) override;
		virtual boolean STDMETHODCALLTYPE Is_Ok_To_End(void) override;
		virtual HRESULT STDMETHODCALLTYPE Piggyback_CLSID(GUID * classid) override;
		virtual boolean STDMETHODCALLTYPE Is_Piggybacking(void) override {return(Piggybacker != NULL);}

		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Head_To_Coord(void) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) override;
		virtual void STDMETHODCALLTYPE Stop_Moving(void) override;
		virtual void STDMETHODCALLTYPE Do_Turn(DirStruct dir) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;
		virtual void STDMETHODCALLTYPE Force_Immediate_Destination(CoordStruct coord) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Now(void) override;
		virtual void STDMETHODCALLTYPE Mark_All_Occupation_Bits(int mark) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Here(CoordStruct to) override;
		virtual boolean STDMETHODCALLTYPE Is_Really_Moving_Now(void) override;
		virtual void STDMETHODCALLTYPE Stop_Movement_Animation(void) override {IsReallyMoving = false;};
		virtual void STDMETHODCALLTYPE Limbo(void) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		void Movement_AI(bool first_pass);
		bool Mark_Head_To(Coord const & coord);

	private:

		Coord DestinationCoord;

		Coord HeadToCoord;

		bool IsMoving;

		bool IsProcessingMovement;

		bool IsReallyMoving;

		ILocomotionPtr Piggybacker;
};
