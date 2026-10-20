// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "win.h"

struct IStorage;
struct IPropertySetStorage;

enum {
	PIDSI_SCEN_DESCRIP = 2,
	PIDSI_PLAYER_HOUSE = 3,
	PIDSI_PLAYER_NAME1 = 4,
	PIDSI_PLAYER_NAME2 = 8,
	PIDSI_G_VERSION = 9,
	PIDSI_G_PLAY_TIME = 10,
	PIDSI_G_START_TIME = 12,
	PIDSI_LAST_SAVE_TIME = 13,
	PIDSI_INTERNAL_VER = 16,
	PIDSI_EXEC_NAME = 18,

	PIDSI_SCENARIO_NUM = 100,
	PIDSI_CAMPAIGN_NUM,
	PIDSI_GAME_TYPE,
};

class SaveVersionInfo
{
	public:
		SaveVersionInfo(void);

		void Set_Version(int num);
		int Get_Version(void);

		void Set_Internal_Version(int num);
		int Get_Internal_Version(void);

		void Set_Scenario_Description(const wchar_t * desc);
		const wchar_t * Get_Scenario_Description(void);

		void Set_Player_House(const wchar_t * name);
		const wchar_t * Get_Player_House(void);

		void Set_Campaign_Number(int num);
		int Get_Campaign_Number(void);

		void Set_Scenario_Number(int num);
		int Get_Scenario_Number(void);

		void Set_Unknown_String(const char * name);
		const char * Get_Unknown_String(void);

		void Set_Player_Name(const char * name);
		const char * Get_Player_Name(void);

		void Set_Executable_Name(const char * name);
		const char * Get_Executable_Name(void);

		void Set_Start_Time(FILETIME &time);
		FILETIME Get_Start_Time(void);

		void Set_Play_Time(FILETIME &time);
		FILETIME Get_Play_Time(void);

		void Set_Last_Time(FILETIME &time);
		FILETIME Get_Last_Time(void);

		void Set_Game_Type(int id);
		int Get_Game_Type(void);

		HRESULT Save(IStorage *storage);
		HRESULT Load(IStorage *storage);

	private:
		HRESULT Load_String(IStorage *storage, int id, char *string);
		HRESULT Load_String_Set(IPropertySetStorage *storageset, int id, char *string);
		HRESULT Load_String(IStorage *storage, int id, wchar_t *string);
		HRESULT Load_String_Set(IPropertySetStorage *storageset, int id, wchar_t *string);

		HRESULT Load_Int(IStorage *storage, int id, int *integer);
		HRESULT Load_Int_Set(IPropertySetStorage *storageset, int id, int *integer);

		HRESULT Save_String(IStorage *storage, int id, char *string);
		HRESULT Save_String_Set(IPropertySetStorage *storageset, int id, const char *string);
		HRESULT Save_String(IStorage *storage, int id, const wchar_t *string);
		HRESULT Save_String_Set(IPropertySetStorage *storageset, int id, const wchar_t *string);

		HRESULT Save_Int(IStorage *storage, int id, int integer);
		HRESULT Save_Int_Set(IPropertySetStorage *storageset, int id, int integer);

		HRESULT Load_Time(IStorage *storage, int id, FILETIME *time);
		HRESULT Load_Time_Set(IPropertySetStorage *storageset, int id, FILETIME *time);

		HRESULT Save_Time(IStorage *storage, int id, FILETIME *time);
		HRESULT Save_Time_Set(IPropertySetStorage *storageset, int id, FILETIME *time);

	private:

		int InternalVersion;

		int Version;

		wchar_t ScenarioDescription[128];

		wchar_t PlayerHouse[64];

		int CampaignNumber;
		int ScenarioNumber;

		char UnknownString[260];

		char PlayerName[64];

		char ExecutableName[260];

		FILETIME StartTime;
		FILETIME PlayTime;
		FILETIME LastSaveTime;

		int GameType;
};

const WCHAR *Stream_Name_From_ID(int id);
