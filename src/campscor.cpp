// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "coopcamp.h"
#include "globals.h"
#include "house.h"
#include "scenario.h"
#include "houstype.h"
#include "ipxmgr.h"
#include "multiplayer.h"
#include "session.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include <wchar.h>
#include <string.h>
#include "stringtable.h"

extern int DAT_0089E050;
extern int DAT_0081B1E8;

class CampScoreLink
{
	public:
		CampScoreLink(void);
		virtual ~CampScoreLink(void);

		int Field4;
};

// FUNCTION: 0x0046D330
CampScoreLink::CampScoreLink(void)
{
	Field4 = 0;
	DAT_0089E050 = 0;
	DAT_0081B1E8 = -1;
}

// FUNCTION: 0x0046D350
CampScoreLink::~CampScoreLink(void)
{
}

// FUNCTION: 0x0046D360
void __fastcall FUN_0046D360(wchar_t * buffer, int seconds)
{
	if (buffer != NULL) {
		if (seconds / 3600 > 99) {
			seconds = 359999;
		}
		int hours = seconds / 3600;
		seconds -= hours * 3600;
		int minutes = seconds / 60;
		seconds -= minutes * 60;
		swprintf(buffer, StringTable::LoadString("TXT_TIME_FORMAT_HOURS", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0xa9), hours, minutes, seconds);
	}
}

class CampScorePlayer
{
	public:
		bool FUN_005E93F0(void);
		bool FUN_005E9440(void);
		char * FUN_007350C0(void);
		int FUN_00696F10(void);
		int FUN_00696F20(void);
		int FUN_00696F50(void);
		void FUN_00696F90(int country);
		void FUN_00696FC0(int color);
		void FUN_00696FF0(int start);

		wchar_t Name[1];
};

struct CampScoreTables
{
	int AIDifficulty[8];
	int AICountry[8];
	int AIColor[8];
	int AIStart[8];
	int AIAlly[8];
	char Gap[0x2814 - 0xa0];
	CampScorePlayer * Players[8];
};
extern CampScoreTables DAT_00A8B27C;
extern bool DAT_0089E000[8];

// FUNCTION: 0x0046D400
void __fastcall FUN_0046D400(int count)
{
	memset(DAT_0089E000, 1, 8);
	if (count > 0) {
		memset(DAT_0089E000, 0, count);
	}
}

// FUNCTION: 0x0046D430
void FUN_0046D430(void)
{
	for (int i = 0; i < 8; i++) {
		if (DAT_00A8B27C.Players[i]->FUN_005E93F0()) {
			DAT_0089E000[i] = false;
		}
	}
}

// FUNCTION: 0x0046D460
bool FUN_0046D460(void)
{
	int i;
	for (i = 0; i < 8; i++) {
		if (!DAT_0089E000[i]) {
			break;
		}
	}
	return i == 8;
}

// FUNCTION: 0x0046D480
void __fastcall FUN_0046D480(int index, bool value)
{
	DAT_0089E000[index] = value;
}

// FUNCTION: 0x0046D490
bool __fastcall FUN_0046D490(int index)
{
	return DAT_0089E000[index];
}

#pragma pack(push, 1)
struct CampScoreOptions
{
	int Type;
	char Name[0x2b];
	char Difficulty;
	int Start[8];
	int Country[8];
	int Color[8];
	int AICountry[8];
	int AIColor[8];
	int AIDifficulty[8];
	int AIStart[8];
	int AIAlly[8];
	char Rest[0x1c7 - 0x130];
};

struct CampScoreSlot
{
	int Type;
	char Name[0x2b];
	int Start;
	int Country;
	int Color;
	char Rest[0x1c7 - 0x3b];
};
#pragma pack(pop)

extern int DAT_00A8EB68;
extern CampScorePlayer * DAT_00AC119C;
extern CampScorePlayer * DAT_00AC11A0;
extern CampScorePlayer ** DAT_00A8DA78;
extern CampScorePlayer * DAT_00AC11B0;
extern CampScorePlayer * DAT_00AC11B4;

bool FUN_005E8760(void);
void __cdecl FUN_004068E0(char const * format, ...);
char * __fastcall FUN_007350C0(wchar_t const * text);
HWND WS_Find_Dialog(int id);

// FUNCTION: 0x0046D4A0
void FUN_0046D4A0(void)
{
	CampScoreOptions packet;
	packet.Type = 0x32;
	strcpy(packet.Name, Session.Handle);
	if (FUN_005E8760()) {
		FUN_004068E0("Sending Campaign Options...\n");
		packet.Difficulty = (char)DAT_00A8EB68;
		for (int i = 0; i < 8; i++) {
			if (DAT_00A8B27C.Players[i] != NULL && DAT_00A8B27C.Players[i] != DAT_00AC119C && DAT_00A8B27C.Players[i] != DAT_00AC11A0) {
				packet.Start[i] = DAT_00A8B27C.Players[i]->FUN_00696F50();
				packet.Country[i] = DAT_00A8B27C.Players[i]->FUN_00696F10();
				packet.Color[i] = DAT_00A8B27C.Players[i]->FUN_00696F20();
			} else {
				packet.Color[i] = packet.Country[i] = packet.Start[i] = -1;
			}
			char * name = DAT_00A8B27C.Players[i]->FUN_007350C0();
			FUN_004068E0("Player %i %s: Start %i, Country %i, Color %i\n", i, name, packet.Start[i], packet.Country[i], packet.Color[i]);
			packet.AICountry[i] = DAT_00A8B27C.AICountry[i];
			packet.AIColor[i] = DAT_00A8B27C.AIColor[i];
			packet.AIDifficulty[i] = DAT_00A8B27C.AIDifficulty[i];
			packet.AIStart[i] = DAT_00A8B27C.AIStart[i];
			packet.AIAlly[i] = DAT_00A8B27C.AIAlly[i];
			FUN_004068E0("AI %i: Difficulty %i, Country %i, Color %i, Start %i, Ally %i\n", i, packet.AIDifficulty[i], packet.AICountry[i], packet.AIColor[i], packet.AIStart[i], packet.AIAlly[i]);
		}
		for (int j = 1; j < 8; j++) {
			if (DAT_00A8B27C.Players[j] != NULL && DAT_00A8B27C.Players[j]->FUN_005E93F0()) {
				Ipx.Send_Global_Message_To_Name(&packet, 0x1c7, 1, DAT_00A8B27C.Players[j]->Name);
				Ipx.Service();
			}
		}
	}
}

// FUNCTION: 0x0046D650
bool FUN_0046D650(void)
{
	CampScoreOptions & received = *(CampScoreOptions *)&Session.GPacket;
	bool changed = false;
	if (received.Type == 0x32) {
		FUN_004068E0("Decoding Campaign Options...\n");
		int difficulty = received.Difficulty;
		if (DAT_00A8EB68 != difficulty) {
			changed = true;
		}
		DAT_00A8EB68 = difficulty;
		HWND dialog = WS_Find_Dialog(0xbc6);
		if (dialog != NULL) {
			HWND item = GetDlgItem(dialog, 0x732);
			if (item != NULL) {
				PostMessage(item, 0x405, 1, DAT_00A8EB68);
			}
		}
		for (int i = 0; i < 8; i++) {
			if (Session.Slots[i] != NULL && Session.Slots[i] != (NodeNameType *)DAT_00AC119C && Session.Slots[i] != (NodeNameType *)DAT_00AC11A0) {
				int start = received.Start[i];
				if (Session.Slots[i]->Get_Start() != start) {
					changed = true;
				}
				Session.Slots[i]->Set_Start(start);
				int country = received.Country[i];
				if (Session.Slots[i]->Get_Country() != country) {
					changed = true;
				}
				Session.Slots[i]->Set_Country(country);
				int color = received.Color[i];
				if (Session.Slots[i]->Get_Color() != color) {
					changed = true;
				}
				Session.Slots[i]->Set_Color(color);
			}
			char * name = FUN_007350C0(Session.Slots[i]->Name);
			FUN_004068E0("Player %i %s: Start %i, Country %i, Color %i\n", i, name, Session.Slots[i]->Get_Start(), Session.Slots[i]->Get_Country(), Session.Slots[i]->Get_Color());
			if (Session.Options.AISlots.Countries[i] != received.AICountry[i]) {
				changed = true;
			}
			Session.Options.AISlots.Countries[i] = received.AICountry[i];
			if (Session.Options.AISlots.Colors[i] != received.AIColor[i]) {
				changed = true;
			}
			Session.Options.AISlots.Colors[i] = received.AIColor[i];
			if (Session.Options.AISlots.Difficulties[i] != received.AIDifficulty[i]) {
				changed = true;
			}
			Session.Options.AISlots.Difficulties[i] = received.AIDifficulty[i];
			if (Session.Options.AISlots.Starts[i] != received.AIStart[i]) {
				changed = true;
			}
			Session.Options.AISlots.Starts[i] = received.AIStart[i];
			if (Session.Options.AISlots.Allies[i] != received.AIAlly[i]) {
				changed = true;
			}
			Session.Options.AISlots.Allies[i] = received.AIAlly[i];
			FUN_004068E0("AI %i: Difficulty %i, Country %i, Color %i, Start %i, Ally %i\n", i, Session.Options.AISlots.Difficulties[i], Session.Options.AISlots.Countries[i], Session.Options.AISlots.Colors[i], Session.Options.AISlots.Starts[i], Session.Options.AISlots.Allies[i]);
		}
	}
	return changed;
}

// FUNCTION: 0x0046D810
void FUN_0046D810(void)
{
	CampScoreSlot packet;
	packet.Type = 0x33;
	strcpy(packet.Name, Session.Handle);
	packet.Start = DAT_00A8DA78[0]->FUN_00696F50();
	packet.Country = DAT_00A8DA78[0]->FUN_00696F10();
	packet.Color = DAT_00A8DA78[0]->FUN_00696F20();
	if (DAT_00A8B27C.Players[0] != NULL) {
		Ipx.Send_Global_Message_To_Name(&packet, 0x1c7, 1, DAT_00A8B27C.Players[0]->Name);
		Ipx.Service();
	}
}

struct CampScoreTable
{
	CoopPlayerStats Players[8];
	int Field380;
	int Count;
	int Winner;
};

extern CampScoreTable DAT_00A8D1FC;
extern int DAT_00A8DA8C;
int __fastcall FUN_0046F5E0(int index);
int __fastcall FUN_004E37D0(int index);
int __fastcall FUN_004E41D0(int index);
int __fastcall FUN_004E4E60(int index);
void __fastcall FUN_004E3F70(HWND dialog, int combo, int wanted);
extern DynamicVectorClass<HouseClass *> DAT_00A80228;

// FUNCTION: 0x0046DA20
void __fastcall FUN_0046DA20(HWND dialog, int id, int value)
{
	wchar_t text[32];
	swprintf(text, L"%d", value);
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4b2, 0, (LPARAM)text);
}

// FUNCTION: 0x0046DA70
void FUN_0046DA70(void)
{
	DAT_00A8D1FC.Count = 0;
	int count = DAT_00A80228.Count();
	for (int i = 0; i < count; i++) {
		HouseClass * house = DAT_00A80228[i];
		if (house && house->Class->IsMultiplayPassive != true && house->FUN_0050B730()) {
			int index = DAT_00A8D1FC.Count++;
			DAT_00A8D1FC.Players[index].Alive = 0;
			wcscpy(DAT_00A8D1FC.Players[index].Name, house->UIName);
			DAT_00A8D1FC.Players[index].Lost = 0;
			DAT_00A8D1FC.Players[index].Kills = 0;
			DAT_00A8D1FC.Players[index].Built = 0;
			DAT_00A8D1FC.Players[index].Score = 0;
			DAT_00A8D1FC.Players[index].Scheme = house->Scheme;
			if (!house->IsDefeated) {
				DAT_00A8D1FC.Players[index].Alive = DAT_00A8D1FC.Players[index].Alive + 1;
				DAT_00A8D1FC.Winner = index;
			}
			int kills = 0;
			int k;
			for (k = 0; k < 20; k++) {
				kills += house->UnitsKilled[k];
			}
			for (k = 0; k < 20; k++) {
				kills += house->BuildingsKilled[k];
			}
			DAT_00A8D1FC.Players[index].Kills = kills;
			DAT_00A8D1FC.Players[index].Lost = house->UnitsLost + house->BuildingsLost;
			int built = house->ABQuantity.Total();
			built += house->AUQuantity.Total();
			built += house->AIQuantity.Total();
			built += house->AAQuantity.Total();
			DAT_00A8D1FC.Players[index].Built = house->UnitsLost + built + house->BuildingsLost;
			if (house->PointTotal > 0) {
				DAT_00A8D1FC.Players[index].Score += house->PointTotal;
			}
			char const * result = DAT_00A8D1FC.Players[index].Alive > 0 ? "Winner" : "Loser";
			FUN_004068E0("%s: %s\n Scheme: %d\n Lost = %d\n Kills = %d\n Built = %d\n Score = %d\n",
				DAT_00A8D1FC.Players[index].Name, result, DAT_00A8D1FC.Players[index].Scheme, DAT_00A8D1FC.Players[index].Lost, DAT_00A8D1FC.Players[index].Kills, DAT_00A8D1FC.Players[index].Built, DAT_00A8D1FC.Players[index].Score);
		}
	}
}

extern CoopPlayerStats * DAT_0089DF80[];

int __cdecl FUN_0046DC90(void const * a, void const * b);

// FUNCTION: 0x0046DC50
void FUN_0046DC50(void)
{
	int count = DAT_00A8D1FC.Count;
	for (int i = 0; i < count; i++) {
		DAT_0089DF80[i] = &DAT_00A8D1FC.Players[i];
	}
	if (count > 1) {
		qsort(DAT_0089DF80, count, 4, FUN_0046DC90);
	}
}

// FUNCTION: 0x0046DC90
int __cdecl FUN_0046DC90(void const * a, void const * b)
{
	CoopPlayerStats * first = *(CoopPlayerStats * const *)a;
	CoopPlayerStats * second = *(CoopPlayerStats * const *)b;
	if (first->Score == second->Score) {
		return 0;
	}
	return first->Score > second->Score ? -1 : 1;
}

// FUNCTION: 0x0046DCC0
void __fastcall FUN_0046DCC0(HWND dialog, bool enable)
{
	HWND item;
	for (int i = 0; i < 8; i++) {
		if (DAT_00A8B27C.Players[i]->FUN_005E93F0() && i != DAT_00A8DA8C) {
			item = GetDlgItem(dialog, FUN_0046F5E0(i));
			EnableWindow(item, enable);
			item = GetDlgItem(dialog, FUN_004E37D0(i));
			EnableWindow(item, enable);
			item = GetDlgItem(dialog, FUN_004E41D0(i));
			EnableWindow(item, enable);
			item = GetDlgItem(dialog, FUN_004E4E60(i));
			EnableWindow(item, enable);
		}
	}
}

// FUNCTION: 0x0046DD70
void __fastcall FUN_0046DD70(HWND dialog, bool enable)
{
	HWND item;
	item = GetDlgItem(dialog, FUN_0046F5E0(DAT_00A8DA8C));
	EnableWindow(item, enable);
	item = GetDlgItem(dialog, FUN_004E37D0(DAT_00A8DA8C));
	EnableWindow(item, enable);
	item = GetDlgItem(dialog, FUN_004E41D0(DAT_00A8DA8C));
	EnableWindow(item, enable);
	item = GetDlgItem(dialog, FUN_004E4E60(DAT_00A8DA8C));
	EnableWindow(item, enable);
	if (FUN_005E8760()) {
		item = GetDlgItem(dialog, 0x732);
		if (item != NULL) {
			EnableWindow(item, enable);
			InvalidateRect(item, NULL, FALSE);
		}
	}
}

// FUNCTION: 0x0046DE20
void __fastcall FUN_0046DE20(HWND dialog, int show)
{
	HWND item = GetDlgItem(dialog, 0x732);
	if (item != NULL) {
		ShowWindow(item, show);
		InvalidateRect(item, NULL, FALSE);
	}
	item = GetDlgItem(dialog, 0x72f);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x798);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x799);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x79a);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x79b);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x77e);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x6d2);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x6d3);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x411);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x412);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x6da);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x6db);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x6a1);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x510);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x6a2);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x522);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x6a3);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x6a4);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x6d1);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x5a8);
	if (item != NULL) {
		ShowWindow(item, show);
	}
	item = GetDlgItem(dialog, 0x71c);
	if (item != NULL) {
		if (show == 0) {
			ShowWindow(item, 1);
		} else {
			ShowWindow(item, 0);
		}
	}
	item = GetDlgItem(dialog, 0x468);
	if (item != NULL) {
		ShowWindow(item, show);
	}
}

// FUNCTION: 0x0046F5E0
int __fastcall FUN_0046F5E0(int index)
{
	if (index == 0) {
		return 0x411;
	}
	if (index == 1) {
		return 0x412;
	}
	return -1;
}

// FUNCTION: 0x0046F600
void __fastcall FUN_0046F600(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int id = FUN_0046F5E0(i);
		if (id == -1) {
			continue;
		}
		HWND item = GetDlgItem(dialog, id);
		if (ShowWindow(item, 0)) {
			if (id != -1) {
				HWND combo = GetDlgItem(dialog, id);
				CampScorePlayer * player = DAT_00A8B27C.Players[i];
				if (player == NULL) {
					continue;
				}
				SendDlgItemMessage(dialog, id, 0x14b, 0, 0);
				int index = SendMessage(combo, 0x4c2, 0, (LPARAM)player);
				SendMessage(combo, 0x151, index, (LPARAM)player);
				SendMessage(combo, 0x14e, index, 0);
			}
			ShowWindow(item, 5);
		}
	}
}

// FUNCTION: 0x0046F6D0
void __fastcall FUN_0046F6D0(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int combo = FUN_004E37D0(i);
		HWND item = GetDlgItem(dialog, combo);
		if (ShowWindow(item, 0)) {
			CampScorePlayer * player = DAT_00A8B27C.Players[i];
			int fallback = -2;
			if (Session.MPGameMode != NULL) {
				if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
					fallback = Session.MPGameMode->vt_entry_28();
				}
			}
			int country;
			if (player->FUN_005E9440()) {
				country = DAT_00A8B27C.AICountry[i];
			} else if (player == DAT_00AC119C) {
				country = fallback;
			} else if (player == DAT_00AC11A0) {
				country = fallback;
			} else if (player == DAT_00AC11B0) {
				country = fallback;
			} else if (player == DAT_00AC11B4) {
				country = -3;
			} else {
				country = player->FUN_00696F10();
			}
			if (Session.MPGameMode != NULL) {
				if (!Session.MPGameMode->vt_entry_1C(country)) {
					country = fallback;
					if (player->FUN_005E9440()) {
						DAT_00A8B27C.AICountry[i] = fallback;
					} else if (player->FUN_005E93F0()) {
						DAT_00A8B27C.Players[i]->FUN_00696F90(fallback);
					}
				}
			}
			FUN_004E3F70(dialog, combo, country);
			ShowWindow(item, 5);
		}
	}
}

inline int _Player_Color(int index)
{
	CampScorePlayer * player = DAT_00A8B27C.Players[index];
	if (player->FUN_005E9440()) {
		return DAT_00A8B27C.AIColor[index];
	}
	if (player == DAT_00AC119C) {
		return -2;
	}
	if (player == DAT_00AC11A0) {
		return -2;
	}
	if (player == DAT_00AC11B0) {
		return -2;
	}
	if (player == DAT_00AC11B4) {
		return -2;
	}
	return player->FUN_00696F20();
}

inline int _Player_Start(int index)
{
	CampScorePlayer * player = DAT_00A8B27C.Players[index];
	if (player->FUN_005E9440()) {
		return DAT_00A8B27C.AIStart[index];
	}
	if (player == DAT_00AC119C) {
		return -2;
	}
	if (player == DAT_00AC11A0) {
		return -2;
	}
	if (player == DAT_00AC11B0) {
		return -2;
	}
	if (player == DAT_00AC11B4) {
		return -2;
	}
	return player->FUN_00696F50();
}

// FUNCTION: 0x0046F810
int __fastcall FUN_0046F810(int index)
{
	return _Player_Color(index);
}

// FUNCTION: 0x0046F880
int __fastcall FUN_0046F880(int index)
{
	return _Player_Start(index);
}

struct PCX
{
	static PCX Instance;
	int GetSurface(char const * name, void * palette);
};
extern char * DAT_0081B1E0;
extern char * DAT_0081B1E4;
extern int DAT_00A8D580;
extern int DAT_00A8DA84;
void __fastcall FUN_004E49A0(HWND dialog, int combo, int value);
void __fastcall FUN_004E5480(HWND dialog, int combo, int value);
void __fastcall FUN_00603D30(HWND item, int surface);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
int __fastcall FUN_0060D380(int resource, DLGPROC proc, bool focus);
struct Game
{
	static bool __cdecl CallBack(void);
};

// FUNCTION: 0x0046F8F0
void __fastcall FUN_0046F8F0(HWND dialog)
{
	int own = DAT_00A8DA8C;
	if (own != -1) {
		int combo = FUN_004E41D0(own);
		FUN_004E49A0(dialog, combo, _Player_Color(own));
	}
	for (int i = 0; i < 8; i++) {
		if (i != own) {
			int combo = FUN_004E41D0(i);
			FUN_004E49A0(dialog, combo, _Player_Color(i));
		}
	}
}

inline void _Set_Image(HWND item, int i, int first, int second)
{
	if (item != NULL) {
		if (DAT_0089E000[i]) {
			if (i == 0) {
				FUN_00603D30(item, first);
			} else {
				FUN_00603D30(item, second);
			}
		} else {
			FUN_00603D30(item, 0);
		}
	}
}

// FUNCTION: 0x0046FA20
void __fastcall FUN_0046FA20(HWND dialog)
{
	int first = PCX::Instance.GetSurface(DAT_0081B1E0, NULL);
	int second = PCX::Instance.GetSurface(DAT_0081B1E4, NULL);
	for (int i = 0; i < 8; i++) {
		if (DAT_00A8B27C.Players[i] != NULL && DAT_00A8B27C.Players[i]->FUN_005E93F0()) {
			int id;
			if (i == 0) {
				id = 0x6d2;
			} else if (i == 1) {
				id = 0x6d3;
			} else {
				continue;
			}
			_Set_Image(GetDlgItem(dialog, id), i, first, second);
		}
	}
}

// FUNCTION: 0x0046FAC0
void __fastcall FUN_0046FAC0(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int combo = FUN_004E4E60(i);
		FUN_004E5480(dialog, combo, _Player_Start(i));
	}
}

// FUNCTION: 0x0046FB50
void __fastcall FUN_0046FB50(HWND dialog)
{
	HWND item = GetDlgItem(dialog, 0x6d1);
	HWND other = GetDlgItem(dialog, 0x5c0);
	if (item != NULL) {
		if (DAT_00A8DA84 < DAT_00A8D580) {
			EnableWindow(item, 0);
			if (other != NULL) {
				EnableWindow(other, 1);
			}
		} else if (DAT_0089E000[DAT_00A8DA8C]) {
			EnableWindow(item, 0);
		} else {
			EnableWindow(item, 1);
		}
	}
}

bool FUN_005DAF20(void);
bool FUN_0077F900(void);

// FUNCTION: 0x0046FBD0
bool FUN_0046FBD0(void)
{
	if (Session.Type == GAME_IPX) {
		return FUN_005DAF20();
	}
	if (Session.Type == GAME_INTERNET) {
		return FUN_0077F900();
	}
	Game::CallBack();
	return false;
}

extern CoopPlayerStats * DAT_0089DFE0[];
extern int DAT_0089E008;
extern int DAT_0089E020;
extern int DAT_0089DFD4;

class CampScoreResults
{
	public:
		CampScoreResults(void);
		virtual ~CampScoreResults(void);
		virtual void Show(DynamicVectorClass<CoopPlayerStats *> list, int a6, int a7, int a8, wchar_t const * name);
};

// FUNCTION: 0x00470C00
CampScoreResults::CampScoreResults(void)
{
	DAT_0089E008 = -1;
	DAT_0089E020 = 0;
	DAT_0089DFD4 = 0;
}

// FUNCTION: 0x00470C30
CampScoreResults::~CampScoreResults(void)
{
}

// FUNCTION: 0x00470C40
void __fastcall FUN_00470C40(HWND dialog, int id, int value)
{
	wchar_t text[32];
	swprintf(text, L"%d", value);
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4b2, 0, (LPARAM)text);
}

// FUNCTION: 0x00470C90
void __fastcall FUN_00470C90(wchar_t * buffer, int seconds)
{
	if (buffer != NULL) {
		if (seconds / 3600 > 99) {
			seconds = 359999;
		}
		int hours = seconds / 3600;
		seconds -= hours * 3600;
		int minutes = seconds / 60;
		seconds -= minutes * 60;
		swprintf(buffer, StringTable::LoadString("TXT_TIME_FORMAT_HOURS", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0x6ea), hours, minutes, seconds);
	}
}

// FUNCTION: 0x00471400
void FUN_00471400(void)
{
	int count = DAT_0089E020;
	if (count > 1) {
		qsort(DAT_0089DFE0, count, 4, FUN_0046DC90);
	}
}

void __fastcall FUN_0072D300(int side);
void __fastcall FUN_0072D730(int side);
void FUN_0072D780(void);
void FUN_0072D350(void);
extern wchar_t DAT_0089E0A4[];
extern int DAT_0089DFC0;
int CALLBACK FUN_00470D30(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: 0x00471420
void CampScoreResults::Show(DynamicVectorClass<CoopPlayerStats *> list, int a6, int a7, int a8, wchar_t const * name)
{
	DAT_0089E020 = list.Count();
	wcscpy(DAT_0089E0A4, name);
	for (int i = 0; i < DAT_0089E020; i++) {
		DAT_0089DFE0[i] = list[i];
	}
	if (DAT_0089E020 > 1) {
		qsort(DAT_0089DFE0, DAT_0089E020, 4, FUN_0046DC90);
	}
	DAT_0089DFC0 = a8;
	DAT_0089E008 = a7;
	DAT_0089DFD4 = a6;
	int side = Scen->PlayerSideIndex;
	FUN_0072D300(side);
	FUN_0072D730(side);
	FUN_0052FEC0(NULL, true);
	FUN_0060D380(0xbc7, FUN_00470D30, 0);
	FUN_0072D780();
	FUN_0072D350();
}

// FUNCTION: 0x00471500 CampScoreLink::~CampScoreLink
