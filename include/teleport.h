// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ftimer.h"
#include "ipiggy.h"
#include "loco.h"
#include "timer.h"

class TeleportLocomotionClass : public LocomotionClass, public IPiggyback
{
		typedef LocomotionClass BASECLASS;

	public:
		TeleportLocomotionClass(void);
		TeleportLocomotionClass(const NoInitClass & x) : BASECLASS(x), Timer(), Piggybacker(NULL) {}

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override {return(BASECLASS::AddRef());}
		virtual ULONG STDMETHODCALLTYPE Release(void) override {return(BASECLASS::Release());}

		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) override;
		virtual void STDMETHODCALLTYPE Stop_Moving(void) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual void STDMETHODCALLTYPE Do_Turn(DirStruct coord) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;

		virtual HRESULT STDMETHODCALLTYPE Begin_Piggyback(ILocomotion * pointer) override;
		virtual HRESULT STDMETHODCALLTYPE End_Piggyback(ILocomotion ** pointer) override;
		virtual boolean STDMETHODCALLTYPE Is_Ok_To_End(void) override;
		virtual HRESULT STDMETHODCALLTYPE Piggyback_CLSID(GUID * classid) override;
		virtual boolean STDMETHODCALLTYPE Is_Piggybacking(void) override {return(Piggybacker != NULL);}

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		virtual void STDMETHODCALLTYPE Mark_All_Occupation_Bits(int mark) override;

		virtual void STDMETHODCALLTYPE Process_Delay(void);
		virtual boolean Is_Stationary(void);

		bool FUN_00718B70(Coord const & coord);
		void Check_Landing(Coord to);
		bool FUN_00718260(Coord to, bool flag);

	private:

		Coord DestinationCoord;
		Coord LastCoords;
		bool IsMoving;
		bool Unknown35;
		bool Unknown36;
		int State;
		CDTimerClass<FrameTimerClass> Timer;
		ILocomotionPtr Piggybacker;
};
