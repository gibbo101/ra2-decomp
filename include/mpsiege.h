// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "multiplayer.h"

class MPObserverTeam : public MultiplayerTeam
{
	public:
		MPObserverTeam(void);
};

class MPSiegeDefenderTeam : public MultiplayerTeam
{
	public:
		MPSiegeDefenderTeam(void);
};

class MPSiegeAttackerTeam : public MultiplayerTeam
{
	public:
		MPSiegeAttackerTeam(void);
};

class MPSiegeClass : public MPGameModeClass
{
	public:
		MPSiegeClass(NoInitClass const & x) : MPGameModeClass(x) {}
		MPSiegeClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex);
		virtual ~MPSiegeClass(void);
		virtual bool vt_entry_10(void) override;
		virtual bool vt_entry_14(unsigned a) override;
		virtual bool vt_entry_18(void) override;
		virtual bool IsAIAllowed(void) override;
		virtual bool UnfixAlliances(void) override;
		virtual bool StartingPositionsToHouseBaseCells2(bool arg) override;
		virtual void CreateMPTeams(DynamicVectorClass<MultiplayerTeam *> * teams) override;
		virtual bool SpawnBaseUnits(HouseClass * house, unsigned unused) override;
};
