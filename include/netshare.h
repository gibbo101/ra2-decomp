// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "dict.h"
#include "globals.h"
#include "preview.h"
#include "wstring.h"
#include "widewstring.h"

class HouseClass;

int ODMessageBox(const wchar_t *text, int type, bool (*callback)(void), bool large = false);
int ODMessageBox(const char *text, int type, bool (*callback)(void), bool large = false);
int CALLBACK ODMessageBox_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

bool Set_Scenario_Info_From_Index(int index);
void PregameSetup(void);
void Update_Network_Dialog_Preview(HWND win);
void Receive_Random_Map_Preview(void);
void Send_Preview_To_Guests(void);
int CountAliveTeams(HouseClass * house);

int RandomMapWaypointCount(int index);
int Scenario_Dialog(HWND hWndParent);

extern struct IChat *g_pChat;
extern struct INetUtil * g_pNetUtil;
extern struct IDownload * g_pDownload;

unsigned int Wstring_Hash(Wstring & string);
unsigned int Wstring_Hash(WideWstring & string);

void __cdecl PMessagePrintf(int color, const wchar_t * fmt, ...);
void __cdecl PMessagePrintf(int color, const char * fmt, ...);
void __cdecl SMessagePrintf(int color, const wchar_t * fmt, ...);

void _DrawMessage(int color, const wchar_t * msg, HWND window);
void _SetMessageString(HWND window, const wchar_t * msg, int len, int color);

HWND GameoptWindow(void);

void PumpGameopts(bool, bool = false);
bool DecodePubGameopt(char * options, char * name);
void SendPublicGameopts(char const * options);
void SendPrivateGameopts(char const * player, char const * options);
void DisplayGameopts(HWND window, BOOL initialize);

void LBSaveSelections(HWND win, Dictionary<Wstring,bool> & lbdict);
void LBRestoreSelections(HWND win, Dictionary<Wstring,bool> & lbdict);

char * CalcRandomMapDigest(void);
int CreateRandomMap(void);

extern COLORREF PlayerColorTable[MAX_PLAYERS];

extern const COLORREF ColorSystem;
extern const COLORREF ColorUser;
extern const COLORREF ColorPriv;
extern const COLORREF ColorPrivAction;
extern const COLORREF ColorAction;
extern const COLORREF ColorOp;
extern const COLORREF ColorPaged;
extern const COLORREF ColorMe;
extern const COLORREF ColorNoJoin;

extern MapPreviewClass *MultiplayerMapPreview;

extern bool IsRandomMap;

extern int IsColorChangePending;
