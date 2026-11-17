// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "wstring.h"

template <class T>
class ArrayList;

enum WonlineResult
{
	WONLINE_DOWNLOAD_PATCH = -1,
	WONLINE_BACK = 0,
	WONLINE_OK = 1,
};

extern Wstring g_NickName;

extern char WestwoodOnline_Clan1_Players[];
extern char WestwoodOnline_Clan2_Players[];

extern int WestwoodOnline_StartTime;
extern int WestwoodOnline_Tournament;
extern int WestwoodOnline_GameID;
extern int WestwoodOnline_GameSKU_TS;
extern int WestwoodOnline_GameSKU_FS;
extern int WestwoodOnline_GameSKU_WDT;
extern char WestwoodOnline_LoginName[];
extern char WestwoodOnline_UserName[];

extern int g_SuspendChatPump;
extern int g_PingsSent;
extern int g_PingsReceived;

extern int g_MaxPlayers;

extern char g_GameServerHost[128];
extern int g_GameServerPort;


void DoFindPage(void);
int Player_Name_To_Index(char *name);
void ViewHTML(const char * name, int no_ask);
bool Switch_Server(int type);
void Read_WOL_Settings(void);
void Write_WOL_Settings(void);
void Apply_WOL_Settings(void);
int Send_Chat_Message(char *msg);
int Send_Chat_Action(char *msg);
void Encode_Game_Options(char *out);
int GetINIHash(void);
void Encode_Channel_ExInfo(char *out);
void SetPlayerAccepted(Wstring const & who, int status);
int GetPlayerAccepted(Wstring const & who);
int Assign_House_And_Color(char *who, int house, int color);
void GoBack(void);
bool WOL_Wait_Callback(void);
void Reset_WOL_Globals(void);
void Draw_Channel_List(void);
void Display_Users(void);
int Startup_Chat(HINSTANCE hInstance);
void Handle_User_Leave(struct User & user);
void Logout_WOnline(void);
void Shutdown_Chat(void);
int Get_WDT_State_Silent(void);
WonlineResult Login_WOL(void);
int Join_WOL_Lobby(HWND win);
int Pick_Lobby(void);
int Join_Lobby(void);
void Set_Wait_Dialog_Text(char *status_text);
void Close_Wait_Window(unsigned int event);
void Fill_Session_Players(struct User * users);
bool Is_Channel_Owner(char *name);
User * Get_Channel_Host(void);
void Sync_Scenario_With_Guests(void);
bool Handle_Preview_Download(void);
void Poke_The_Host(void);
void Init_WDT(void);
int Select_WDT_Server(void);
int Select_WDT_Location(HWND window);
int Request_WDT_Server_List(void);
bool Request_WDT_Cycle(void);
void Init_WDT_Sounds(void);
void Deinit_WDT_Sounds(void);
void Play_WDT_Sound(ArrayList<Wstring> list);
