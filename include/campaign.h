// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"

#include "campaign.hh"
#include "vq.hh"

class CCINIClass;

class CampaignClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		CampaignClass(char const * name = NULL);
		CampaignClass(NoInitClass const & x) : BASECLASS(x) {}
		virtual ~CampaignClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;

		virtual bool Read_INI(CCINIClass & ini) override;

		static CampaignType From_Name(char const * name);

	public:
		// The game disc holding the campaign's scenarios.
		int CDNumber;

		// The opening scenario's file name, upper case.
		char ScenarioName[_MAX_FNAME+_MAX_EXT];

		// The movie played once the campaign's last mission is won.
		VQType FinalMovie;

		// The title shown in the mission selection list.
		wchar_t Description[128];
};

void Read_Battle_INI(CCINIClass const & ini);
