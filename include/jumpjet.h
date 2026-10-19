// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "facing.h"
#include "ipiggy.h"
#include "loco.h"

class JumpjetLocomotionClass : public LocomotionClass, public IPiggyback
{
		typedef LocomotionClass BASECLASS;

	public:

		JumpjetLocomotionClass(void);
		JumpjetLocomotionClass(NoInitClass const & x) : BASECLASS(x), Facing(x) {}
		virtual ~JumpjetLocomotionClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
		virtual ULONG STDMETHODCALLTYPE Release(void) override;

		virtual HRESULT STDMETHODCALLTYPE Link_To_Object(void * object) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Head_To_Coord(void) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) override;
		virtual void STDMETHODCALLTYPE Stop_Moving(void) override;
		virtual void STDMETHODCALLTYPE Do_Turn(DirStruct coord) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Now(void) override;
		virtual void STDMETHODCALLTYPE Mark_All_Occupation_Bits(int mark) override;

		virtual HRESULT STDMETHODCALLTYPE Begin_Piggyback(ILocomotion * pointer) override;
		virtual HRESULT STDMETHODCALLTYPE End_Piggyback(ILocomotion ** pointer) override;
		virtual boolean STDMETHODCALLTYPE Is_Ok_To_End(void) override;
		virtual HRESULT STDMETHODCALLTYPE Piggyback_CLSID(GUID * classid) override;
		virtual boolean STDMETHODCALLTYPE Is_Piggybacking(void) override {return(Piggybacker != NULL);}

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		void Process_Grounded(void);
		void Process_Ascent(void);
		void Process_Hover(void);
		void Process_Cruise(void);
		void Process_Descent(void);
		void Process_Crashing(void);
		void Process_Unknown(void);
		void Movement_AI(void);
		Coord Closest_Free_Spot(Coord const & to) const;
		int Desired_Flight_Level(void) const;

	private:

		int TurnRate;
		int Speed;
		float Climb;
		float Crash;
		int Height;
		float Accel;
		float Wobbles;
		int Deviation;
		bool NoWobbles;

		Coord HeadToCoord;

		bool IsMoving;

		enum ProcessStateType {
			GROUNDED,
			ASCENDING,
			HOVERING,
			CRUISING,
			DESCENDING,
			CRASHING,
			UNKNOWN,
		} CurrentState;

		FacingClass Facing;

		double CurrentSpeed;
		double TargetSpeed;

		int FlightLevel;

		double CurrentWobble;

		bool IsLanding;

		bool IsPiggyReady;

		ILocomotionPtr Piggybacker;
};
