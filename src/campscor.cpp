// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "coopcamp.h"
#include "globals.h"
#include "goptions.h"
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

// SYNTHETIC: GAMEMD 0x0046D120 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0046D150 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0046D170 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0046D190 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0046D1B0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0046D1D0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0046D1F0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0046D230 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0046D260 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0046D290 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0046D2C0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0046D2D0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0046D2F0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0046D310 SYMBOL
// _$E55

extern int DAT_0089E050;
extern int DAT_0081B1E8;

class CampScoreLink
{
	public:
		CampScoreLink(void);
		virtual ~CampScoreLink(void);

		int Field4;
};

// FUNCTION: GAMEMD 0x0046D330
CampScoreLink::CampScoreLink(void)
{
	Field4 = 0;
	DAT_0089E050 = 0;
	DAT_0081B1E8 = -1;
}

// FUNCTION: GAMEMD 0x0046D350
CampScoreLink::~CampScoreLink(void)
{
}

// FUNCTION: GAMEMD 0x0046D360
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

bool __fastcall FUN_005E93F0(NodeNameType * slot);
bool __fastcall FUN_005E9440(NodeNameType * slot);
extern bool DAT_0089E000[8];

// FUNCTION: GAMEMD 0x0046D400
void __fastcall FUN_0046D400(int count)
{
	memset(DAT_0089E000, 1, 8);
	if (count > 0) {
		memset(DAT_0089E000, 0, count);
	}
}

// FUNCTION: GAMEMD 0x0046D430
void FUN_0046D430(void)
{
	for (int i = 0; i < 8; i++) {
		if (FUN_005E93F0(Session.Slots[i])) {
			DAT_0089E000[i] = false;
		}
	}
}

// FUNCTION: GAMEMD 0x0046D460
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

// FUNCTION: GAMEMD 0x0046D480
void __fastcall FUN_0046D480(int index, bool value)
{
	DAT_0089E000[index] = value;
}

// FUNCTION: GAMEMD 0x0046D490
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

extern NodeNameType * DAT_00AC119C;
extern NodeNameType * DAT_00AC11A0;
extern NodeNameType * DAT_00AC11B0;
extern NodeNameType * DAT_00AC11B4;

bool FUN_005E8760(void);
void __cdecl FUN_004068E0(char const * format, ...);
char * __fastcall FUN_007350C0(wchar_t const * text);
HWND WS_Find_Dialog(int id);

// FUNCTION: GAMEMD 0x0046D4A0
void FUN_0046D4A0(void)
{
	CampScoreOptions packet;
	packet.Type = 0x32;
	strcpy(packet.Name, Session.Handle);
	if (FUN_005E8760()) {
		FUN_004068E0("Sending Campaign Options...\n");
		packet.Difficulty = (char)Options.CampDifficulty;
		for (int i = 0; i < 8; i++) {
			if (Session.Slots[i] != NULL && Session.Slots[i] != DAT_00AC119C && Session.Slots[i] != DAT_00AC11A0) {
				packet.Start[i] = Session.Slots[i]->Get_Start();
				packet.Country[i] = Session.Slots[i]->Get_Country();
				packet.Color[i] = Session.Slots[i]->Get_Color();
			} else {
				packet.Color[i] = packet.Country[i] = packet.Start[i] = -1;
			}
			char * name = FUN_007350C0(Session.Slots[i]->Name);
			FUN_004068E0("Player %i %s: Start %i, Country %i, Color %i\n", i, name, packet.Start[i], packet.Country[i], packet.Color[i]);
			packet.AICountry[i] = Session.Options.AISlots.Countries[i];
			packet.AIColor[i] = Session.Options.AISlots.Colors[i];
			packet.AIDifficulty[i] = Session.Options.AISlots.Difficulties[i];
			packet.AIStart[i] = Session.Options.AISlots.Starts[i];
			packet.AIAlly[i] = Session.Options.AISlots.Allies[i];
			FUN_004068E0("AI %i: Difficulty %i, Country %i, Color %i, Start %i, Ally %i\n", i, packet.AIDifficulty[i], packet.AICountry[i], packet.AIColor[i], packet.AIStart[i], packet.AIAlly[i]);
		}
		for (int j = 1; j < 8; j++) {
			if (Session.Slots[j] != NULL && FUN_005E93F0(Session.Slots[j])) {
				Ipx.Send_Global_Message_To_Name(&packet, 0x1c7, 1, Session.Slots[j]->Name);
				Ipx.Service();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0046D650
bool FUN_0046D650(void)
{
	CampScoreOptions & received = *(CampScoreOptions *)&Session.GPacket;
	bool changed = false;
	if (received.Type == 0x32) {
		FUN_004068E0("Decoding Campaign Options...\n");
		int difficulty = received.Difficulty;
		if (Options.CampDifficulty != difficulty) {
			changed = true;
		}
		Options.CampDifficulty = difficulty;
		HWND dialog = WS_Find_Dialog(0xbc6);
		if (dialog != NULL) {
			HWND item = GetDlgItem(dialog, 0x732);
			if (item != NULL) {
				PostMessage(item, 0x405, 1, Options.CampDifficulty);
			}
		}
		for (int i = 0; i < 8; i++) {
			if (Session.Slots[i] != NULL && Session.Slots[i] != DAT_00AC119C && Session.Slots[i] != DAT_00AC11A0) {
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

// FUNCTION: GAMEMD 0x0046D810
void FUN_0046D810(void)
{
	CampScoreSlot packet;
	packet.Type = 0x33;
	strcpy(packet.Name, Session.Handle);
	packet.Start = Session.Players[0]->Get_Start();
	packet.Country = Session.Players[0]->Get_Country();
	packet.Color = Session.Players[0]->Get_Color();
	if (Session.Slots[0] != NULL) {
		Ipx.Send_Global_Message_To_Name(&packet, 0x1c7, 1, Session.Slots[0]->Name);
		Ipx.Service();
	}
}

int __fastcall FUN_0046F5E0(int index);
int __fastcall FUN_004E37D0(int index);
int __fastcall FUN_004E41D0(int index);
int __fastcall FUN_004E4E60(int index);
void __fastcall FUN_004E3F70(HWND dialog, int combo, int wanted);
extern DynamicVectorClass<HouseClass *> DAT_00A80228;

// FUNCTION: GAMEMD 0x0046DA20
void __fastcall FUN_0046DA20(HWND dialog, int id, int value)
{
	wchar_t text[32];
	swprintf(text, L"%d", value);
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4b2, 0, (LPARAM)text);
}

// FUNCTION: GAMEMD 0x0046DA70
void FUN_0046DA70(void)
{
	Session.NumScores = 0;
	int count = DAT_00A80228.Count();
	for (int i = 0; i < count; i++) {
		HouseClass * house = DAT_00A80228[i];
		if (house && house->Class->IsMultiplayPassive != true && house->FUN_0050B730()) {
			int index = Session.NumScores++;
			Session.Score[index].Wins = 0;
			wcscpy(Session.Score[index].Name, house->UIName);
			Session.Score[index].Lost[0] = 0;
			Session.Score[index].Kills[0] = 0;
			Session.Score[index].Built[0] = 0;
			Session.Score[index].Score[0] = 0;
			Session.Score[index].Scheme = house->Scheme;
			if (!house->IsDefeated) {
				Session.Score[index].Wins = Session.Score[index].Wins + 1;
				Session.Winner = index;
			}
			int kills = 0;
			int k;
			for (k = 0; k < 20; k++) {
				kills += house->UnitsKilled[k];
			}
			for (k = 0; k < 20; k++) {
				kills += house->BuildingsKilled[k];
			}
			Session.Score[index].Kills[0] = kills;
			Session.Score[index].Lost[0] = house->UnitsLost + house->BuildingsLost;
			int built = house->ABQuantity.Total();
			built += house->AUQuantity.Total();
			built += house->AIQuantity.Total();
			built += house->AAQuantity.Total();
			Session.Score[index].Built[0] = house->UnitsLost + built + house->BuildingsLost;
			if (house->PointTotal > 0) {
				Session.Score[index].Score[0] += house->PointTotal;
			}
			char const * result = Session.Score[index].Wins > 0 ? "Winner" : "Loser";
			FUN_004068E0("%s: %s\n Scheme: %d\n Lost = %d\n Kills = %d\n Built = %d\n Score = %d\n",
				Session.Score[index].Name, result, Session.Score[index].Scheme, Session.Score[index].Lost[0], Session.Score[index].Kills[0], Session.Score[index].Built[0], Session.Score[index].Score[0]);
		}
	}
}

extern MPlayerScoreType * DAT_0089DF80[];

int __cdecl FUN_0046DC90(void const * a, void const * b);

// FUNCTION: GAMEMD 0x0046DC50
void FUN_0046DC50(void)
{
	int count = Session.NumScores;
	for (int i = 0; i < count; i++) {
		DAT_0089DF80[i] = &Session.Score[i];
	}
	if (count > 1) {
		qsort(DAT_0089DF80, count, 4, FUN_0046DC90);
	}
}

// FUNCTION: GAMEMD 0x0046DC90
int __cdecl FUN_0046DC90(void const * a, void const * b)
{
	MPlayerScoreType * first = *(MPlayerScoreType * const *)a;
	MPlayerScoreType * second = *(MPlayerScoreType * const *)b;
	if (first->Score[0] == second->Score[0]) {
		return 0;
	}
	return first->Score[0] > second->Score[0] ? -1 : 1;
}

// FUNCTION: GAMEMD 0x0046DCC0
void __fastcall FUN_0046DCC0(HWND dialog, bool enable)
{
	HWND item;
	for (int i = 0; i < 8; i++) {
		if (FUN_005E93F0(Session.Slots[i]) && i != Session.unknown_2854) {
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

// FUNCTION: GAMEMD 0x0046DD70
void __fastcall FUN_0046DD70(HWND dialog, bool enable)
{
	HWND item;
	item = GetDlgItem(dialog, FUN_0046F5E0(Session.unknown_2854));
	EnableWindow(item, enable);
	item = GetDlgItem(dialog, FUN_004E37D0(Session.unknown_2854));
	EnableWindow(item, enable);
	item = GetDlgItem(dialog, FUN_004E41D0(Session.unknown_2854));
	EnableWindow(item, enable);
	item = GetDlgItem(dialog, FUN_004E4E60(Session.unknown_2854));
	EnableWindow(item, enable);
	if (FUN_005E8760()) {
		item = GetDlgItem(dialog, 0x732);
		if (item != NULL) {
			EnableWindow(item, enable);
			InvalidateRect(item, NULL, FALSE);
		}
	}
}

// FUNCTION: GAMEMD 0x0046DE20
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

// FUNCTION: GAMEMD 0x0046F5E0
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

// FUNCTION: GAMEMD 0x0046F600
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
				NodeNameType * player = Session.Slots[i];
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

// FUNCTION: GAMEMD 0x0046F6D0
void __fastcall FUN_0046F6D0(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int combo = FUN_004E37D0(i);
		HWND item = GetDlgItem(dialog, combo);
		if (ShowWindow(item, 0)) {
			NodeNameType * player = Session.Slots[i];
			int fallback = -2;
			if (Session.MPGameMode != NULL) {
				if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
					fallback = Session.MPGameMode->vt_entry_28();
				}
			}
			int country;
			if (FUN_005E9440(player)) {
				country = Session.Options.AISlots.Countries[i];
			} else if (player == DAT_00AC119C) {
				country = fallback;
			} else if (player == DAT_00AC11A0) {
				country = fallback;
			} else if (player == DAT_00AC11B0) {
				country = fallback;
			} else if (player == DAT_00AC11B4) {
				country = -3;
			} else {
				country = player->Get_Country();
			}
			if (Session.MPGameMode != NULL) {
				if (!Session.MPGameMode->vt_entry_1C(country)) {
					country = fallback;
					if (FUN_005E9440(player)) {
						Session.Options.AISlots.Countries[i] = fallback;
					} else if (FUN_005E93F0(player)) {
						Session.Slots[i]->Set_Country(fallback);
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
	NodeNameType * player = Session.Slots[index];
	if (FUN_005E9440(player)) {
		return Session.Options.AISlots.Colors[index];
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
	return player->Get_Color();
}

inline int _Player_Start(int index)
{
	NodeNameType * player = Session.Slots[index];
	if (FUN_005E9440(player)) {
		return Session.Options.AISlots.Starts[index];
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
	return player->Get_Start();
}

// FUNCTION: GAMEMD 0x0046F810
int __fastcall FUN_0046F810(int index)
{
	return _Player_Color(index);
}

// FUNCTION: GAMEMD 0x0046F880
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
void __fastcall FUN_004E49A0(HWND dialog, int combo, int value);
void __fastcall FUN_004E5480(HWND dialog, int combo, int value);
void __fastcall FUN_00603D30(HWND item, int surface);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
int __fastcall FUN_0060D380(int resource, DLGPROC proc, bool focus);
struct Game
{
	static bool __cdecl CallBack(void);
};

// FUNCTION: GAMEMD 0x0046F8F0
void __fastcall FUN_0046F8F0(HWND dialog)
{
	int own = Session.unknown_2854;
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

// FUNCTION: GAMEMD 0x0046FA20
void __fastcall FUN_0046FA20(HWND dialog)
{
	int first = PCX::Instance.GetSurface(DAT_0081B1E0, NULL);
	int second = PCX::Instance.GetSurface(DAT_0081B1E4, NULL);
	for (int i = 0; i < 8; i++) {
		if (Session.Slots[i] != NULL && FUN_005E93F0(Session.Slots[i])) {
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

// FUNCTION: GAMEMD 0x0046FAC0
void __fastcall FUN_0046FAC0(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int combo = FUN_004E4E60(i);
		FUN_004E5480(dialog, combo, _Player_Start(i));
	}
}

// FUNCTION: GAMEMD 0x0046FB50
void __fastcall FUN_0046FB50(HWND dialog)
{
	HWND item = GetDlgItem(dialog, 0x6d1);
	HWND other = GetDlgItem(dialog, 0x5c0);
	if (item != NULL) {
		if (Session.Players.Count() < Session.NumScores) {
			EnableWindow(item, 0);
			if (other != NULL) {
				EnableWindow(other, 1);
			}
		} else if (DAT_0089E000[Session.unknown_2854]) {
			EnableWindow(item, 0);
		} else {
			EnableWindow(item, 1);
		}
	}
}

bool FUN_005DAF20(void);
bool FUN_0077F900(void);

// FUNCTION: GAMEMD 0x0046FBD0
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

extern MPlayerScoreType * DAT_0089DFE0[];
extern int DAT_0089E008;
extern int DAT_0089E020;
extern int DAT_0089DFD4;

class CampScoreResults
{
	public:
		CampScoreResults(void);
		virtual ~CampScoreResults(void);
		virtual void Show(DynamicVectorClass<MPlayerScoreType *> list, int a6, int a7, int a8, wchar_t const * name);
};

// FUNCTION: GAMEMD 0x00470C00
CampScoreResults::CampScoreResults(void)
{
	DAT_0089E008 = -1;
	DAT_0089E020 = 0;
	DAT_0089DFD4 = 0;
}

// FUNCTION: GAMEMD 0x00470C30
CampScoreResults::~CampScoreResults(void)
{
}

// FUNCTION: GAMEMD 0x00470C40
void __fastcall FUN_00470C40(HWND dialog, int id, int value)
{
	wchar_t text[32];
	swprintf(text, L"%d", value);
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4b2, 0, (LPARAM)text);
}

// FUNCTION: GAMEMD 0x00470C90
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

#include "scheme.h"

namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}
namespace OwnerDraw {
	void __fastcall Paint(HWND window);
	void __stdcall SetNeedsControlImage(HWND window, int value);
}
void __fastcall FUN_00607FD0(HWND window);
extern wchar_t DAT_0089E0A4[];
extern int DAT_0089DFC0;

static int DAT_0081B1EC[8][6] = {
	{ 0x781, 0x783, 0x785, 0x787, 0x789, 0x70a },
	{ 0x782, 0x784, 0x786, 0x788, 0x78a, 0x70b },
};

// FUNCTION: GAMEMD 0x00470D30
int CALLBACK FUN_00470D30(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	wchar_t text[32];
	HWND item;
	int i;
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc != 0) {
		return rc;
	}
	int * result = (int *)GetWindowLong(window, DWL_USER);
	switch (message) {
		case WM_INITDIALOG:
			text[0] = 0;
			item = GetDlgItem(window, 0x733);
			SendMessage(item, 0x4b2, 0, (LPARAM)DAT_0089E0A4);
			swprintf(text, StringTable::LoadString("GUI:CampaignLabel", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0x700), DAT_0089DFC0);
			item = GetDlgItem(window, 0x735);
			SendMessage(item, 0x4b2, 0, (LPARAM)text);
			if (DAT_0089E008 == 0) {
				swprintf(text, StringTable::LoadString("GUI:DifficultyString", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0x704), StringTable::LoadString("TXT_DIFFICULTY_EASY", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0x704));
			} else if (DAT_0089E008 == 1) {
				swprintf(text, StringTable::LoadString("GUI:DifficultyString", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0x706), StringTable::LoadString("TXT_DIFFICULTY_MEDIUM", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0x706));
			} else if (DAT_0089E008 == 2) {
				swprintf(text, StringTable::LoadString("GUI:DifficultyString", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0x708), StringTable::LoadString("TXT_DIFFICULTY_BRUTAL", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0x708));
			} else {
				swprintf(text, StringTable::LoadString("GUI:Blank", NULL, "D:\\ra2mdpost\\CampScor.cpp", 0x70a));
			}
			item = GetDlgItem(window, 0x732);
			SendMessage(item, 0x4b2, 0, (LPARAM)text);
			FUN_00470C90(text, DAT_0089DFD4 / 60);
			item = GetDlgItem(window, 0x3ea);
			SendMessage(item, 0x4b2, 0, (LPARAM)text);
			for (i = 0; i < DAT_0089E020; i++) {
				ColorScheme * scheme = ColorSchemes[DAT_0089DFE0[i]->Scheme];
				RGBClass rgb = scheme->HSV;
				COLORREF color = RGB(rgb.Get_Red(), rgb.Get_Green(), rgb.Get_Blue());
				int id = DAT_0081B1EC[i][0];
				wchar_t * name = DAT_0089DFE0[i]->Name;
				item = GetDlgItem(window, id);
				SendMessage(item, 0x4b2, 0, (LPARAM)name);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
				id = DAT_0081B1EC[i][1];
				FUN_00470C40(window, id, DAT_0089DFE0[i]->Kills[0]);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
				id = DAT_0081B1EC[i][2];
				FUN_00470C40(window, id, DAT_0089DFE0[i]->Lost[0]);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
				id = DAT_0081B1EC[i][3];
				FUN_00470C40(window, id, DAT_0089DFE0[i]->Built[0]);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
				id = DAT_0081B1EC[i][4];
				FUN_00470C40(window, id, DAT_0089DFE0[i]->Score[0]);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
				if (i == 0) {
					name = DAT_0089DFE0[0]->Name;
					item = GetDlgItem(window, 0x73a);
					SendMessage(item, 0x4b2, 0, (LPARAM)name);
					item = GetDlgItem(window, 0x73a);
					SendMessage(item, 0x498, 0, color);
					swprintf(text, L"%d", DAT_0089DFE0[0]->Score[0]);
					item = GetDlgItem(window, 0x73b);
					SendMessage(item, 0x4b2, 0, (LPARAM)text);
					item = GetDlgItem(window, 0x73b);
					SendMessage(item, 0x498, 0, color);
				} else if (i == 1) {
					name = DAT_0089DFE0[1]->Name;
					item = GetDlgItem(window, 0x739);
					SendMessage(item, 0x4b2, 0, (LPARAM)name);
					item = GetDlgItem(window, 0x739);
					SendMessage(item, 0x498, 0, color);
					swprintf(text, L"%d", DAT_0089DFE0[1]->Score[0]);
					item = GetDlgItem(window, 0x738);
					SendMessage(item, 0x4b2, 0, (LPARAM)text);
					item = GetDlgItem(window, 0x738);
					SendMessage(item, 0x498, 0, color);
				}
			}
			break;

		case WM_COMMAND:
			if (LOWORD(wparam) == 0x6d1) {
				*result = 1;
			}
			break;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			break;

		case 0x497:
			item = GetDlgItem(window, 0x72b);
			PostMessage(item, 0x4d7, 0, (LPARAM)window);
			break;

		case 0x4d8:
			if (wparam == 0) {
				SendDlgItemMessage(window, 0x72b, 0x4d5, 0, 0x1d);
			}
			break;

		case 0x4ec:
			SendDlgItemMessage(window, 0x72b, 0x4d3, 0, 0);
			break;

		case 0x4ed:
			FUN_0052FEC0(NULL, true);
			ShowWindow(window, 0);
			OwnerDraw::SetNeedsControlImage(window, 1);
			break;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00471400
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

// FUNCTION: GAMEMD 0x00471420
void CampScoreResults::Show(DynamicVectorClass<MPlayerScoreType *> list, int a6, int a7, int a8, wchar_t const * name)
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

// SYNTHETIC: GAMEMD 0x00471500
// ??_GCampScoreLink@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x00471520
// ??_GCampScoreResults@@UAEPAXI@Z
