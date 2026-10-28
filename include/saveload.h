// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <stddef.h>
#include "win.h"

struct IStream;
class SaveVersionInfo;

int Load_Misc_Values(IStream * stream);
int Save_Misc_Values(IStream * stream);
bool Get_Savefile_Info(char const * name, SaveVersionInfo * info);
bool Load_Game(const char *file_name);
bool Save_Game(const char *file_name, wchar_t const * descr, bool bargraph=false);

extern unsigned int ExpectedGameVersion;

class SavegameInformation
{
	public:
		SavegameInformation(void);

		void Set_Version(int num);
		int Get_Version(void);

		int InternalVersion;
		int Version;
		wchar_t ScenarioDescription[128];
		wchar_t PlayerHouse[64];
		int Campaign;
		int ScenarioNumber;
		char UnknownString[260];
		char PlayerName[64];
		char ExecutableName[260];
		FILETIME StartTime;
		FILETIME PlayTime;
		FILETIME LastSaveTime;
		int GameType;
};
