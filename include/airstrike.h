// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"
#include "ftimer.h"
#include "timer.h"

class AircraftTypeClass;
class FootClass;
class ObjectClass;
class TechnoClass;

class AirstrikeClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:
		int AirstrikeTeam;
		int EliteAirstrikeTeam;
		int AirstrikeTeamTypeIndex;
		int EliteAirstrikeTeamTypeIndex;
		DWORD unknown_34;
		DWORD unknown_38;
		bool IsOnMission;
		bool unknown_bool_3D;
		DWORD TeamDissolveFrame;
		int AirstrikeRechargeTime;
		int EliteAirstrikeRechargeTime;
		TechnoClass * Owner;
		ObjectClass * Target;
		AircraftTypeClass * AirstrikeTeamType;
		AircraftTypeClass * EliteAirstrikeTeamType;
		FootClass * FirstObject;

		AirstrikeClass(TechnoClass * owner);
		AirstrikeClass(NoInitClass const & x);
		virtual ~AirstrikeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual void AI(void) override;
		void ClearTarget(void);
};
