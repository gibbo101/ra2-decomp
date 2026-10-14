// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coopcamp.h"
#include "ipxaddr.h"
#include "multiplayer.h"

class MPCoopObject
{
	public:
		virtual ~MPCoopObject(void);
};

class MPCooperative : public MPGameModeClass
{
	public:
		MPCooperative(NoInitClass const & x) : MPGameModeClass(x) {}
		MPCooperative(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex);
		virtual ~MPCooperative(void);
		virtual bool vt_entry_10(void) override;
		virtual bool vt_entry_14(unsigned a) override;
		virtual bool vt_entry_18(void) override;
		virtual bool vt_entry_1C(unsigned a) override;
		virtual void vt_entry_20(void) override;
		virtual void vt_entry_24(void) override;
		virtual int vt_entry_28(void) override;
		virtual int FirstValidMapIndex(void) override;
		virtual bool vt_entry_58(int index, DynamicVectorClass<int> picks, char const * map, char const * name) override;
		virtual void vt_entry_38(unsigned a) override;
		virtual void PopulateTeamDropdownForPlayer(HWND hwnd, int index) override;
		virtual bool vt_entry_5C(unsigned a, unsigned b, unsigned c) override;
		virtual bool vt_entry_60(void) override;
		virtual bool vt_entry_64(void) override;
		virtual void vt_entry_74(unsigned a, unsigned b) override;
		virtual void vt_entry_78(unsigned a) override;
		virtual bool UnfixAlliances(void) override;

		void FUN_005C2500(HWND dialog);
		void FUN_005C21D0(HWND dialog, bool flag);
		void FUN_005C3EC0(void);
		void FUN_005C23B0(HWND dialog, int control);

		CoopCamp_0049B610 * Current;
		int Field44;
		bool Field48;
		bool Field49;
		int Field4C;
		MPCoopObject * Field50;
		int Field54;
		DynamicVectorClass<char *> Names;
		char Pad70[0xC4 - 0x70];
		IPXAddressClass Addresses[8];
		char Pad124[0x1A4 - 0x124];
		DynamicVectorClass<int> Lists[8];
		DynamicVectorClass<int> Lists2[8];
		DynamicVectorClass<CoopCamp_0049B610 *> Campaigns;
		int CampaignIndex;
		bool Field340;
};
