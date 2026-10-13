// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "facing.h"
#include "loco.h"
#include "typelist.h"

class CCINIClass;

class LevitateLocomotionClass : public LocomotionClass
{
		typedef LocomotionClass BASECLASS;

	public:

		LevitateLocomotionClass(void);
		LevitateLocomotionClass(NoInitClass const & x) : BASECLASS(x) {}
		virtual ~LevitateLocomotionClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;

		virtual HRESULT STDMETHODCALLTYPE Link_To_Object(void *pointer) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Head_To_Coord(void) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Now(void) override;
		virtual void STDMETHODCALLTYPE Mark_All_Occupation_Bits(int mark) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		void Hover_AI(void);

		void State_AI(void);
		void Process_Idle(void);
		void Process_Accelerating(void);
		void Process_Cruising(void);
		void Process_Decelerating(void);
		void Process_Drifting(void);
		void Process_Arrived(void);
		void Process_Recentering(void);

		void Process_Departing(void);

		bool Validate_TarCom(void);
		bool Validate_NavCom(void);

		void Accelerate(double&);
		void Accelerate_Towards(Coord const & coord);
		void Drift_Towards(Coord const & coord);
		void Drift(DirType const & dir);
		void Decelerate(void);
		bool Is_In_Proximity(Coord const & coord);
		bool Has_Arrived(Coord const & coord);

		void Steer_Towards(Coord const & coord);
		void Move_AI(void);
		double Update_Speed(void);
		bool Is_Not_On_Cell(Coord const & coord);
		void Set_Coord(Coord const & coord);
		bool Can_Move_Here(Coord const & coord);
		void Update_Bridge_State(Coord const & coord);
		bool Needs_New_Target(void);
		void Stop(void);

		static void Read_INI(CCINIClass const & ini);

	private:

		enum {
			STATE_IDLE,
			STATE_ACCELERATING,
			STATE_CRUISING,
			STATE_DECELERATING,
			STATE_DRIFTING,
			STATE_ARRIVED,
			STATE_RECENTERING,
			STATE_DEPARTING,
		} State;

		double Speed;

		double MoveX;
		double MoveY;

		double AccelerationX;
		double AccelerationY;

		int AccelerationsRemaining;

		int BlockTriesRemaining;

		double MoveRate;

		double Dampen;

	private:

		static struct GlobalControlsStruct {

			double Drag;

			double MaxVelocityWhenHappy;
			double MaxVelocityWhenFollowing;
			double MaxVelocityWhenPissedOff;

			double AccelerationProbability;

			int AccelerationDuration;

			double Acceleration;

			double InitialBoost;

			int MaxBlockCount;

			double IntentionalDeacceleration;

			double IntentionalDriftVelocity;

			double ProximityDistance;
		} GlobalControls;

		static TypeList<int> PropulsionSoundEffects;

		static char const * INI_NAME;
};
