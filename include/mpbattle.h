// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "multiplayer.h"

class MPBattleClass : public MultiplayerGameMode
{
	public:
		MPBattleClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex);
		virtual ~MPBattleClass(void);
		virtual bool vt_entry_04(void) override;
		virtual bool vt_entry_40(void) override;
		virtual bool vt_entry_08(void) override;
		virtual bool vt_entry_0C(unsigned a) override;
		virtual void vt_entry_20(void) override;
		virtual void vt_entry_24(void) override;
		virtual void vt_entry_38(unsigned a) override;
		virtual bool vt_entry_54(int index, DynamicVectorClass<int> picks) override;
		virtual bool vt_entry_58(int index, DynamicVectorClass<int> picks, char const * map, char const * name) override;
		virtual bool vt_entry_5C(unsigned a, unsigned b, unsigned c) override;
		virtual bool vt_entry_68(void) override;
		virtual void vt_entry_74(unsigned a, unsigned b) override;
		virtual void vt_entry_78(HWND dialog) override;
		virtual void vt_entry_90(void) override;
		virtual void vt_entry_94(void) override;
		virtual int vt_entry_98(void) override;
		virtual void vt_entry_9C(void) override;
		virtual void vt_entry_A0(bool playerWon) override;
		virtual void vt_entry_A4(unsigned a) override;
		virtual bool vt_entry_A8(void) override;
		virtual void vt_entry_AC(void) override;
		virtual void vt_entry_B0(void) override;
		virtual bool vt_entry_B4(DynamicVectorClass<int> list, int * out) override;
		virtual int vt_entry_B8(void) override;
		virtual bool vt_entry_BC(void) override;
};
