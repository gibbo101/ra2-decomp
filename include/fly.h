// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ipiggy.h"
#include "loco.h"

enum ImpactType {
	IMPACT_NONE,
	IMPACT_NORMAL,
	IMPACT_EDGE
};

class DirType;

class FlyLocomotionClass : public LocomotionClass
{
		typedef LocomotionClass BASECLASS;

	public:

		FlyLocomotionClass(void);
		FlyLocomotionClass(NoInitClass const & x) : BASECLASS(x) {}
		virtual ~FlyLocomotionClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Link_To_Object(void * object) override;

		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Now(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) override;
		virtual void STDMETHODCALLTYPE Stop_Moving(void) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual Matrix3DStruct STDMETHODCALLTYPE Draw_Matrix(int *key) override;
		virtual Point2DStruct STDMETHODCALLTYPE Draw_Point(void) override;
		virtual Point2DStruct STDMETHODCALLTYPE Shadow_Point(void) override;
		virtual Matrix3DStruct STDMETHODCALLTYPE Shadow_Matrix(int *key) override;
		virtual void STDMETHODCALLTYPE Do_Turn(DirStruct coord) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;
		virtual boolean STDMETHODCALLTYPE Power_Off(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Powered(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Ion_Sensitive(void) override;
		virtual int STDMETHODCALLTYPE Apparent_Speed(void) override;
		virtual int STDMETHODCALLTYPE Get_Status(void) override;
		virtual void STDMETHODCALLTYPE Acquire_Hunter_Seeker_Target(void) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		bool Landing_Takeoff_AI(void);
		bool Edge_Of_World_AI(void);
		void Movement_AI(void);
		ImpactType Physics(Coord & coord, DirType facing);
		void Rotation_AI(void);

		bool Process_Take_Off(void);
		bool Process_Landing(void);

		int Nearing_Target(bool stage_approach, CoordStruct coord);

		void Take_Off(void);
		void Land(void);
		bool Is_In_Flight(void);
		void Tumble(void);
		bool Needs_To_Land(void);
		bool Is_Locked_To_Straight_Flight(void);

	private:

		bool AirportBound;

		Coord DestinationCoord;

		Coord HeadToCoord;

		bool IsMoving;

		int FlightLevel;

		double TargetSpeed;
		double CurrentSpeed;

		bool IsTakingOff;
		bool IsLanding;
		bool CommencedLanding;

		bool IsTumbling;

		int CurrentROT;

		int Riser;

		bool IsElevating;
};
