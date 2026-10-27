// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "always.h"
#include "vector.h"
#include "widewstring.h"
#include "wstring.h"
#include "cell.h"

#include <windows.h>

class CCINIClass;
class NoInitClass;
class HouseClass;
class MultiMission;

class MultiplayerTeam
{
	public:
		MultiplayerTeam(WideWstring const & title, int index);
		virtual ~MultiplayerTeam(void);
		virtual bool IsTeamIncluded(int index);
		virtual bool SetPlayerTeam(int player);
		int CountMembers(void);
		int AddToList(HWND list);

		WideWstring Title;
		int Index;
};

class MultiplayerGameMode
{
	public:
		class InitializerBase
		{
			public:
				InitializerBase(void);
				virtual ~InitializerBase(void);
				virtual void Allocate(void) = 0;
		};

		static MultiplayerGameMode * __fastcall Get(int index);
		CCINIClass * FUN_005D67B0(void);
		bool FUN_005D63E0(MultiMission * map);
		MultiplayerTeam * FUN_005D6620(int index);
		void FUN_005D6750(void);
		bool FUN_005D6D80(void);

		MultiplayerGameMode(NoInitClass const &) {}
		MultiplayerGameMode(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex);
		virtual ~MultiplayerGameMode(void);
		virtual bool vt_entry_04(void);
		virtual bool vt_entry_08(void);
		virtual bool vt_entry_0C(unsigned a);
		virtual bool vt_entry_10(void);
		virtual bool vt_entry_14(unsigned a);
		virtual bool vt_entry_18(void);
		virtual bool vt_entry_1C(unsigned a);
		virtual void vt_entry_20(void);
		virtual void vt_entry_24(void);
		virtual int vt_entry_28(void);
		virtual int vt_entry_2C(void);
		virtual int vt_entry_30(void);
		virtual bool CanAllyWith(int index);
		virtual void vt_entry_38(unsigned a);
		virtual bool IsAIAllowed(void);
		virtual bool vt_entry_40(void);
		virtual int FirstValidMapIndex(void);
		virtual void PopulateTeamDropdown(HWND hwnd, DynamicVectorClass<MultiplayerTeam *> * teams, MultiplayerTeam * team);
		virtual void DrawTeamDropdown(HWND hwnd, DynamicVectorClass<MultiplayerTeam *> * teams, MultiplayerTeam * team);
		virtual void PopulateTeamDropdownForPlayer(HWND hwnd, int index);
		virtual bool vt_entry_54(int index, DynamicVectorClass<int> picks);
		virtual bool vt_entry_58(int index, DynamicVectorClass<int> picks, char const * map, char const * name);
		virtual bool vt_entry_5C(unsigned a, unsigned b, unsigned c);
		virtual bool vt_entry_60(void);
		virtual bool vt_entry_64(void);
		virtual bool vt_entry_68(void);
		virtual int RandomHumanCountryIndex(void);
		virtual int RandomAICountryIndex(void);
		virtual void vt_entry_74(unsigned a, unsigned b);
		virtual void vt_entry_78(unsigned a);
		virtual bool UnfixAlliances(void);
		virtual bool StartingPositionsToHouseBaseCells(char unused);
		virtual bool StartingPositionsToHouseBaseCells2(bool arg);
		virtual bool AllyTeams(void);
		virtual bool vt_entry_8C(void);
		virtual void vt_entry_90(void);
		virtual void vt_entry_94(void);
		virtual int vt_entry_98(void);
		virtual void vt_entry_9C(void);
		virtual void vt_entry_A0(unsigned a);
		virtual void vt_entry_A4(unsigned a);
		virtual bool vt_entry_A8(void);
		virtual void vt_entry_AC(void);
		virtual void vt_entry_B0(void);
		virtual bool vt_entry_B4(DynamicVectorClass<int> list, int * out);
		virtual int vt_entry_B8(void);
		virtual bool vt_entry_BC(void);
		virtual void CreateMPTeams(DynamicVectorClass<MultiplayerTeam *> * teams);
		virtual Cell * AssignStartingPositionsToHouse(Cell * result, int house, DynamicVectorClass<Cell> * coords, unsigned char * satisfied);
		virtual bool SpawnBaseUnits(HouseClass * house, unsigned unused);
		virtual bool GenerateStartingUnits(HouseClass * house, int & amount);

		bool unknown_4;
		DynamicVectorClass<MultiplayerTeam *> MPTeams;
		WideWstring UIName;
		WideWstring CSFTooltip;
		int MPModeIndex;
		Wstring INIFilename;
		Wstring MapFilter;
		bool AIAllowed;
		CCINIClass * INI;
		bool AlliesAllowed;
		bool wolTourney;
		bool wolClanTourney;
		bool MustAlly;
};
