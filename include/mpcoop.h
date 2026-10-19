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
		MPCoopObject(void);
		virtual ~MPCoopObject(void);
		virtual void Send(DynamicVectorClass<CoopPlayerStats *> stats, unsigned time, bool partial, int side, wchar_t const * title);

		int Field4;
};

class MPCoopReporter
{
	public:
		MPCoopReporter(void);
		virtual ~MPCoopReporter(void);
		virtual void Send(DynamicVectorClass<CoopPlayerStats *> stats, int total, int duration, int campaign, wchar_t const * title);
};

class MPCooperative : public MPGameModeClass
{
	public:
		MPCooperative(NoInitClass const & x) : MPGameModeClass(x) {}
		MPCooperative(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex);
		virtual ~MPCooperative(void);
		virtual bool vt_entry_04(void) override;
		virtual bool vt_entry_08(void) override;
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
		virtual bool IsAIAllowed(void) override;
		virtual bool vt_entry_68(void) override;
		virtual bool StartingPositionsToHouseBaseCells2(bool arg) override;
		virtual bool AllyTeams(void) override;
		virtual bool vt_entry_8C(void) override;
		virtual void vt_entry_90(void) override;
		virtual void vt_entry_94(void) override;
		virtual int vt_entry_98(void) override;
		virtual void vt_entry_9C(void) override;
		virtual void vt_entry_A0(unsigned a) override;
		virtual void vt_entry_A4(unsigned a) override;
		virtual bool vt_entry_A8(void) override;
		virtual void vt_entry_AC(void) override;
		virtual void vt_entry_B0(void) override;
		virtual bool vt_entry_B4(DynamicVectorClass<int> list, int * out) override;
		virtual int vt_entry_B8(void) override;
		virtual bool vt_entry_BC(void) override;
		virtual Cell * AssignStartingPositionsToHouse(Cell * result, int house, DynamicVectorClass<Cell> * coords, unsigned char * satisfied) override;

		void FUN_005C2500(HWND dialog);
		void FUN_005C21D0(HWND dialog, bool flag);
		bool FUN_005C3EC0(void);
		int FUN_005C2D00(int house, DynamicVectorClass<Cell> * coords, unsigned char * satisfied);
		int FUN_005C34F0(void);
		int FUN_005C35F0(void);
		void FUN_005C23B0(HWND dialog, int control);

		CoopCamp_0049B610 * Current;
		int Field44;
		bool Field48;
		bool Field49;
		int Field4C;
		MPCoopObject * Field50;
		int Field54;
		DynamicVectorClass<char *> Names;
		int Field70;
		char Pad74[0x9C - 0x74];
		int Seed;
		int TechLevel;
		int UnitCount;
		int Credits;
		bool FogOfWar;
		bool BridgeDestruction;
		bool Goodies;
		bool MCVRedeploy;
		bool AlliesAllowed;
		bool HarvTruce;
		bool Bases;
		bool CTF;
		int AIPlayers;
		int AIDifficulty;
		bool ShortGame;
		bool SWAllowed;
		bool BuildOffAlly;
		bool CrapEngineers;
		int GameSpeed;
		IPXAddressClass Addresses[8];
		int Field124[8];
		int Field144[8];
		wchar_t Field164[22];
		int Field190;
		char Field194[16];
		DynamicVectorClass<int> Lists[8];
		DynamicVectorClass<int> Lists2[8];
		DynamicVectorClass<CoopCamp_0049B610 *> Campaigns;
		int CampaignIndex;
		bool Field340;
};
