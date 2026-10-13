// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ipiggy.h"
#include "loco.h"

class DropPodLocomotionClass : public LocomotionClass, public IPiggyback
{
		typedef LocomotionClass BASECLASS;

	public:

		DropPodLocomotionClass(void);
		DropPodLocomotionClass(NoInitClass const & x) : BASECLASS(x), Piggybacker(NULL) {}
		virtual ~DropPodLocomotionClass(void) override;

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
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;
		virtual int STDMETHODCALLTYPE Drawing_Code(void) override;

		virtual HRESULT STDMETHODCALLTYPE Begin_Piggyback(ILocomotion * pointer) override;
		virtual HRESULT STDMETHODCALLTYPE End_Piggyback(ILocomotion ** pointer) override;
		virtual boolean STDMETHODCALLTYPE Is_Ok_To_End(void) override;
		virtual HRESULT STDMETHODCALLTYPE Piggyback_CLSID(GUID * classid) override;
		virtual boolean STDMETHODCALLTYPE Is_Piggybacking(void) override {return(Piggybacker != NULL);}

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

	private:
		enum DropPodDirType {
			DPOD_DIR_NE,
			DPOD_DIR_NW,
			DPOD_DIR_SE,
			DPOD_DIR_SW,
		};

		int Direction;

		Coord DestinationCoord;

		ILocomotionPtr Piggybacker;
};
