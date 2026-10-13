// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "face.h"
#include "ftimer.h"
#include "ipiggy.h"
#include "loco.h"
#include "matrix3d.h"
#include "timer.h"

#include "mark.hh"

class DriveLocomotionClass : public LocomotionClass, public IPiggyback
{
		typedef LocomotionClass BASECLASS;

	public:

		DriveLocomotionClass(void);
		DriveLocomotionClass(NoInitClass const & x) : BASECLASS(x), RampTimer(x) {}
		virtual ~DriveLocomotionClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
		virtual ULONG STDMETHODCALLTYPE Release(void) override;

		virtual boolean STDMETHODCALLTYPE Is_Moving(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Destination(void) override;
		virtual CoordStruct STDMETHODCALLTYPE Head_To_Coord(void) override;
		virtual Matrix3DStruct STDMETHODCALLTYPE Draw_Matrix(int *key) override;
		virtual int STDMETHODCALLTYPE Z_Adjust(void) override;
		virtual ZGradientType STDMETHODCALLTYPE Z_Gradient(void) override;
		virtual boolean STDMETHODCALLTYPE Process(void) override;
		virtual void STDMETHODCALLTYPE Move_To(CoordStruct to) override;
		virtual void STDMETHODCALLTYPE Stop_Moving(void) override;
		virtual void STDMETHODCALLTYPE Do_Turn(DirStruct coord) override;
		virtual void STDMETHODCALLTYPE Unlimbo(void) override;
		virtual void STDMETHODCALLTYPE Force_Track(int track, CoordStruct coord) override;
		virtual LayerType STDMETHODCALLTYPE In_Which_Layer(void) override;
		virtual void STDMETHODCALLTYPE Force_New_Slope(int ramp) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Now(void) override;
		virtual void STDMETHODCALLTYPE Mark_All_Occupation_Bits(int mark) override;
		virtual boolean STDMETHODCALLTYPE Is_Moving_Here(CoordStruct to) override;
		virtual boolean STDMETHODCALLTYPE Will_Jump_Tracks(void) override;
		virtual void STDMETHODCALLTYPE Lock(void) override;
		virtual void STDMETHODCALLTYPE Unlock(void) override;
		virtual int STDMETHODCALLTYPE Get_Track_Number(void) override;
		virtual int STDMETHODCALLTYPE Get_Track_Index(void) override;
		virtual int STDMETHODCALLTYPE Get_Speed_Accum(void) override;

		virtual HRESULT STDMETHODCALLTYPE Begin_Piggyback(ILocomotion * pointer) override;
		virtual HRESULT STDMETHODCALLTYPE End_Piggyback(ILocomotion ** pointer) override;
		virtual boolean STDMETHODCALLTYPE Is_Ok_To_End(void) override;
		virtual HRESULT STDMETHODCALLTYPE Piggyback_CLSID(GUID * classid) override;
		virtual boolean STDMETHODCALLTYPE Is_Piggybacking(void) override {return(Piggybacker != NULL);}

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		bool Stop_Driver(void);
		bool Start_Driver(Coord const & headto);

		void Mark_Track(Coord const & headto, MarkType type);

		Matrix3D Get_Slope_Matrix(void) const;
		double Get_Slope_Ratio(void) const;
		void Set_Slope(int ramp);
		BOOL Is_Angled(void) const;
		bool Incoming(Cell cell);
		bool Abandon_Navigation(void);

		enum DriveLocomotionClassEnum {
			BACKUP_INTO_REFINERY=64,
			OUT_OF_REFINERY,
			OUT_OF_WEAPON_FACTORY
		};

		enum TrackControlType {
			F_=0x00,
			F_T=0x01,
			F_X=0x02,
			F_Y=0x04,
			F_D=0x08
		};

	private:

		struct TurnTrackType {
			char					Track;
			char					StartTrack;
			Dir256				Facing;
			TrackControlType	Flag;
		};

		struct TrackType {
			Point2DStruct Offset;
			Dir256	Facing;
		};

		struct RawTrackType {
			TrackType const * Track;
			int	Jump;
			int	Entry;
			int	Cell;
		};

		int CurrentRamp;
		int PreviousRamp;
		ProgressTimerClass<FrameTimerClass> RampTimer;

		Coord DestinationCoord;

		Coord HeadToCoord;

		int SpeedAccum;
		double TargetSpeed;

		int TrackNumber;
		int TrackIndex;

		bool IsOnShortTrack;

		bool IsTurretLockedDown;

		bool IsRotating;

		bool IsDriving;

		bool IsRocking;

		bool IsLocomotorUnlocked;

		ILocomotionPtr Piggybacker;

		bool While_Moving(bool just_started=false);
		bool Start_Of_Move(bool & stop_processing, bool retry = true, bool force_straight = false);
		void Lay_Track(void);
		Point2D Smooth_Turn(Point2D const & adj, Dir256 & dir);

		static TurnTrackType const TrackControl[67];
		static RawTrackType const RawTracks[13];
		static TrackType const Track13[];
		static TrackType const Track12[];
		static TrackType const Track11[];
		static TrackType const Track10[];
		static TrackType const Track9[];
		static TrackType const Track8[];
		static TrackType const Track7[];
		static TrackType const Track6[];
		static TrackType const Track5[];
		static TrackType const Track4[];
		static TrackType const Track3[];
		static TrackType const Track2[];
		static TrackType const Track1[24];
};

