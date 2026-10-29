// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "mpcoop.h"
#include "ccfile.h"
#include "ccini.h"
#include "session.h"
#include "scenario.h"
#include "house.h"
#include "houstype.h"
#include "rules.h"
#include "_rules.h"
#include "gadget.h"
#include "display.h"
#include "ipxmgr.h"
#include "wsproto.h"
#include "tooltip.h"
#include "globals.h"
#include "goptions.h"
#include "event.h"
#include "queue.h"
#include <cstdio>
#include <cstring>
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include <stdlib.h>
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x005C10F0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005C1120 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005C1140 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005C1160 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005C1180 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005C11A0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005C11C0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005C1200 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005C1230 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005C1260 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005C1290 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005C12A0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005C12C0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005C12E0 SYMBOL
// _$E55

extern int DAT_008316A4;
extern wchar_t DAT_00887734[];
struct MPLobbyEntry
{
	char Pad0[0x1602a];
	wchar_t Name[1];
};
extern MPLobbyEntry ** DAT_00A8022C;
extern int DAT_00A80238;
void __fastcall FUN_004E3B90(HWND dialog);
void __fastcall FUN_005E74E0(HWND dialog);
void __fastcall FUN_005E2EF0(HWND dialog, MultiplayerGameMode * mode);
void __fastcall FUN_005E2F60(HWND dialog);
void __fastcall FUN_005E32D0(bool force, bool now);
void __fastcall FUN_005C6120(DynamicVectorClass<NodeNameType *> * vec);
extern bool DAT_0089E04C;
int __fastcall FUN_005E66D0(void);
int __fastcall FUN_0049D9F0(char const * name);
void __cdecl FUN_004068E0(char const * format, ...);
void __fastcall FUN_005E3720(char const * text);
void __fastcall FUN_005E7BF0(int slot);
int __fastcall FUN_005E6520(int value);
extern int DAT_0088098C;
extern int DAT_00880990;
extern int DAT_00880994;
void __fastcall FUN_0049B930(CoopCampaignClass * camp);
struct Game {
	static int TechLevel;
	static int Seed;
	static void InitRandom(void);
};

HWND WS_Find_Dialog(int id);
HWND GameoptWindow(void);
HWND FUN_00775B10(void);
void __cdecl PMessagePrintf(int color, const wchar_t * fmt, ...);
int FUN_00775B20(void);
extern char DAT_0082DC84[];
extern char DAT_0081F1D4[];
extern int DAT_00A83CA8;
struct MPCountryEntry
{
	char Pad0[0x60];
	wchar_t const * Name;
};
extern MPCountryEntry ** DAT_00A83C9C;
char * __fastcall FUN_007350C0(wchar_t const * text);
extern int DAT_00ABF460[8];
void __fastcall FUN_005EBC00(HWND dialog, int count);
int __fastcall FUN_004E37D0(int index);
void __fastcall FUN_004E3F70(HWND dialog, int slot, int value);
bool __fastcall FUN_005E8760(void);

DynamicVectorClass<Cell> __fastcall FUN_00688380(char arg);

static inline char const * Scenario_Filename(MultiMission const * scenario)
{
	return((char const *)scenario + 0x58);
}


// FUNCTION: GAMEMD 0x005C1300
int __fastcall FUN_005C1300(char const * name)
{
	if (name == NULL) {
		return(0);
	}
	CCFileClass file(name);
	INIClass ini;
	if (!ini.Load(file, false)) {
		return(0);
	}
	int count = 0;
	for (int i = 0; i < 8; i++) {
		char buffer[32];
		sprintf(buffer, "%d", i);
		if (ini.Get_Int("Waypoints", buffer, -1) != -1) {
			count++;
		}
	}
	if (count == 0) {
		count = ini.Get_Int("RandomMap", "NumPlayers", 0);
	}
	return(count);
}

static inline void Reload_Campaigns(MPCooperative * coop)
{
	for (int j = coop->Campaigns.Count() - 1; j >= 0; --j) {
		CoopCampaignClass * camp = coop->Campaigns[j];
		if (camp) {
			camp->FUN_0049B720();
			operator delete(camp);
		}
	}
	DynamicVectorClass<CoopCampaignClass *> * vec = &coop->Campaigns;
	int max = vec->Length();
	vec->Clear();
	vec->Resize(max, 0);
	int count;
	DAT_0089F518.FUN_0049E4F0(&count);
	for (int i = 0; i < count; ++i) {
		CoopCampaignClass * raw = (CoopCampaignClass *)operator new(sizeof(CoopCampaignClass));
		CoopCampaignClass * camp = raw ? raw->FUN_0049B610() : 0;
		camp->FUN_0049CAF0(i);
		vec->Add(camp);
	}
}

// FUNCTION: GAMEMD 0x005C1470
MPCooperative::MPCooperative(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MultiplayerGameMode(title, tooltip, inifile, mapfilter, false, modeindex)
{
	Current = NULL;
	Field44 = 0;
	CampaignIndex = 0;
	Field50 = NULL;
	unknown_4 = false;
	Field48 = false;
	Field49 = false;
	Field4C = 0;
	Field340 = true;

	for (int j = Campaigns.Count() - 1; j >= 0; --j) {
		CoopCampaignClass * camp = Campaigns[j];
		if (camp) {
			camp->FUN_0049B720();
			operator delete(camp);
		}
	}
	DynamicVectorClass<CoopCampaignClass *> * vec = &Campaigns;
	int max = vec->Length();
	vec->Clear();
	vec->Resize(max, 0);
	int count;
	DAT_0089F518.FUN_0049E4F0(&count);
	for (int i = 0; i < count; ++i) {
		CoopCampaignClass * raw = (CoopCampaignClass *)operator new(sizeof(CoopCampaignClass));
		CoopCampaignClass * camp = raw ? raw->FUN_0049B610() : 0;
		camp->FUN_0049CAF0(i);
		vec->Add(camp);
	}
}

// FUNCTION: GAMEMD 0x005C1640
MPCooperative::~MPCooperative(void)
{
	CoopCampaignClass * current = Current;
	if (current) {
		current->FUN_0049B720();
		operator delete(current);
		Current = NULL;
	}
	for (int j = Campaigns.Count() - 1; j >= 0; --j) {
		CoopCampaignClass * camp = Campaigns[j];
		if (camp) {
			camp->FUN_0049B720();
			operator delete(camp);
		}
	}
	Campaigns.Clear();
	for (int i = Names.Count() - 1; i >= 0; --i) {
		operator delete(Names[i]);
	}
	int max = Names.Length();
	Names.Clear();
	Names.Resize(max, 0);
	if (Field50) {
		delete Field50;
		Field50 = NULL;
	}
	unknown_4 = false;
}

// FUNCTION: GAMEMD 0x005C17B0
bool MPCooperative::vt_entry_1C(unsigned a)
{
	CoopCampEntry * entry;
	int index = 0;
	DAT_0089F518.FUN_0049E3B0(CampaignIndex, &entry);
	if (entry == NULL) {
		return(true);
	}
	if (Current != NULL) {
		if (!Current->FUN_0049BCC0(&index)) {
			index = 0;
		}
	} else {
		index = 0;
	}
	if (index >= entry->ListA.Count()) {
		return(true);
	}
	if (a == (unsigned)-2) {
		if (entry->ListA[index]->Count() > 1) {
			return(true);
		}
	} else if (a == (unsigned)-3) {
		return(true);
	}
	bool found = false;
	for (int i = 0; i < entry->ListA[index]->Count(); i++) {
		if (FUN_0049D9F0((*entry->ListA[index])[i]) == a) {
			found = true;
			break;
		}
	}
	return(found);
}

// FUNCTION: GAMEMD 0x005C18A0
bool MPCooperative::vt_entry_10(void)
{
	Reload_Campaigns(this);
	return(true);
}

// FUNCTION: GAMEMD 0x005C1980
int __fastcall FUN_005C1980(MPCooperative * coop)
{
	int found = -1;
	for (int i = 0; i < DAT_0089F518.EntryCount; i++) {
		CoopCampEntry * entry;
		DAT_0089F518.FUN_0049E3B0(i, &entry);
		for (int j = 0; j < 3; j++) {
			if (Session.Options.ScenarioIndex != 0) {
				if (strnicmp(Scenario_Filename(Session.Scenarios[Session.Options.ScenarioIndex]), &entry->Names[j * _MAX_PATH], strlen(&entry->Names[j * _MAX_PATH])) == 0) {
					found = i;
				}
			}
		}
	}
	return(found);
}

// FUNCTION: GAMEMD 0x005C1A10
void MPCooperative::vt_entry_20(void)
{
	DAT_008316A4 = -1;
	FUN_004068E0("Sending observer slot of 8");
	FUN_005E3720("O8");
	if (WS_Find_Dialog(0xc2) != NULL || WS_Find_Dialog(0xbc) != NULL) {
		if (Current != NULL) {
			int index;
			Current->FUN_0049BC80(&index);
			bool remove = false;
			if (FUN_005C1980(this) != index) {
				if (FUN_00775B20() == 0x6b) {
					remove = true;
				}
			}
			if (!remove && FUN_00775B20() != 0x6b) {
				remove = true;
			}
			if (remove) {
				CoopCampaignClass * old = Current;
				if (old) {
					old->FUN_0049B720();
					operator delete(old);
				}
				Current = NULL;
			}
		}
		if (Current != NULL) {
			if (Session.Players.Count() == 2) {
				Current->FUN_0049B760(Session.Players[0]->Name, Session.Players[1]->Name);
			} else {
				Current->FUN_0049B760(DAT_00887734, DAT_00887734);
			}
		}
	}
	if (Current == NULL) {
		CoopCampaignClass * raw = (CoopCampaignClass *)operator new(sizeof(CoopCampaignClass));
		Current = raw ? raw->FUN_0049B610() : 0;
		if (Current == NULL) {
			return;
		}
		Current->FUN_0049CAF0(0);
		int map = FUN_005C1980(this);
		if (map < 0) {
			CampaignIndex = 0;
			void * name = NULL;
			Current->FUN_0049D070(&name, 0);
			int i;
			for (i = 0; i < Session.Scenarios.Count(); i++) {
				if (strnicmp(Scenario_Filename(Session.Scenarios[i]), (char const *)name, strlen((char const *)name)) == 0) {
					Session.Options.ScenarioIndex = i;
					FUN_005E7BF0(i);
					break;
				}
			}
			if (i == Session.Scenarios.Count()) {
				Session.Options.ScenarioIndex = 0;
				FUN_005E7BF0(0);
			}
		} else {
			CampaignIndex = map;
			CoopCampEntry * entry;
			DAT_0089F518.FUN_0049E3B0(map, &entry);
			void * name = NULL;
			Current->FUN_0049D070(&name, 0);
			FUN_004068E0("@@@ Looking for %s", Scenario_Filename(Session.Scenarios[Session.Options.ScenarioIndex]));
			if (strnicmp(Scenario_Filename(Session.Scenarios[Session.Options.ScenarioIndex]), (char const *)name, strlen((char const *)name)) != 0) {
				int first = (map == 0);
				do {
					Current->FUN_0049CAF0(first);
					Current->FUN_0049CAF0(map);
					Current->FUN_0049D070(&name, 0);
				} while (strnicmp(Scenario_Filename(Session.Scenarios[Session.Options.ScenarioIndex]), (char const *)name, strlen((char const *)name)) != 0);
			}
		}
	}
	Field340 = true;
	HWND dialog = GameoptWindow();
	if (dialog != NULL) {
		FUN_005C2500(dialog);
		FUN_005C21D0(dialog, true);
	}
}

// FUNCTION: GAMEMD 0x005C1CB0
void MPCooperative::vt_entry_24(void)
{
	Field340 = true;
}

// FUNCTION: GAMEMD 0x005C1CC0
int MPCooperative::FirstValidMapIndex(void)
{
	int count = 0;
	void * name = NULL;
	DAT_0089F518.FUN_0049E4F0(&count);
	if (count != 0) {
		if (Current == NULL) {
			CoopCampaignClass * raw = (CoopCampaignClass *)operator new(sizeof(CoopCampaignClass));
			Current = raw ? raw->FUN_0049B610() : 0;
		}
		Current->FUN_0049CAF0(0);
		Current->FUN_0049D070(&name, 0);
		for (int i = 0; i < Session.Scenarios.Count(); i++) {
			if (strnicmp(Scenario_Filename(Session.Scenarios[i]), (char const *)name, strlen((char const *)name)) == 0) {
				return(i);
			}
		}
		vt_entry_10();
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x005C1D80
bool MPCooperative::vt_entry_14(WideWstring * message)
{
	if (Session.Players.Count() == 2 && Current != NULL) {
		Current->FUN_0049B760(Session.Players[0]->Name, Session.Players[1]->Name);
	}
	return(MultiplayerGameMode::vt_entry_14(message));
}

static inline void Set_Player_Country(int index, int country)
{
	Session.Players[index]->Set_Country(country);
}

// FUNCTION: GAMEMD 0x005C1DC0
bool MPCooperative::vt_entry_18(void)
{
	if (Current == NULL) {
		return(false);
	}
	int count;
	DAT_0089F518.FUN_0049E4F0(&count);
	int selected = FUN_005E66D0();
	if (selected >= count) {
		if (Session.Players.Count() == 2) {
			Current->FUN_0049B760(Session.Players[0]->Name, Session.Players[1]->Name);
		}
		if (!Current->FUN_0049C0D0()) {
			return(false);
		}
		Field48 = true;
	} else if (FUN_00775B20() == 0x6b) {
		CoopCampaignClass * old = Current;
		if (old) {
			old->FUN_0049B720();
			operator delete(old);
		}
		Current = Campaigns[selected];
		CoopCampaignClass * raw = (CoopCampaignClass *)operator new(sizeof(CoopCampaignClass));
		Campaigns[selected] = raw ? raw->FUN_0049B610() : 0;
		Campaigns[selected]->FUN_0049CAF0(selected);
		Field4C = selected;
		if (Session.Players.Count() == 2) {
			Current->FUN_0049B760(Session.Players[0]->Name, Session.Players[1]->Name);
		}
	}
	CoopCampEntry * entry;
	int index = 0;
	DAT_0089F518.FUN_0049E3B0(CampaignIndex, &entry);
	if (entry == NULL) {
		return(false);
	}
	if (Current != NULL) {
		if (!Current->FUN_0049BCC0(&index)) {
			index = 0;
		}
	} else {
		index = 0;
	}
	if (index >= entry->ListA.Count()) {
		return(false);
	}
	if (entry->ListA.Count() == 0) {
		return(false);
	}
	int group = entry->ListA[index]->Count();
	FUN_005C2500(GameoptWindow());
	for (int i = 0; i < Session.Players.Count(); i++) {
		NodeNameType * player = Session.Players[i];
		if (!vt_entry_1C(player->Get_Country())) {
			if (group > 1) {
				Set_Player_Country(i, -2);
			} else {
				Set_Player_Country(i, FUN_0049D9F0((*entry->ListA[index])[0]));
			}
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005C1FA0
int MPCooperative::vt_entry_28(void)
{
	CoopCampEntry * entry;
	int index = 0;
	DAT_0089F518.FUN_0049E3B0(CampaignIndex, &entry);
	if (entry == NULL) {
		return(-2);
	}
	if (Current != NULL) {
		if (!Current->FUN_0049BCC0(&index)) {
			index = 0;
		}
	} else {
		index = 0;
	}
	if (index >= entry->ListA.Count()) {
		return(-2);
	}
	int group = entry->ListA[index]->Count();
	if (group == 0) {
		return(-2);
	}
	if (group > 1) {
		return(-2);
	}
	return(FUN_0049D9F0((*entry->ListA[index])[0]));
}

// FUNCTION: GAMEMD 0x005C2040
bool MPCooperative::vt_entry_58(int index, DynamicVectorClass<int> picks, char const * map, char const * name)
{
	if (Current == NULL) {
		FUN_005C3EC0();
		Field340 = true;
	}
	if (Current == NULL) {
		return(false);
	}
	int selected;
	Current->FUN_0049BC80(&selected);
	if (selected != index) {
		Field340 = true;
	}
	Current->FUN_0049CCF0(index, picks, map, name);
	CampaignIndex = index;
	return(true);
}

// FUNCTION: GAMEMD 0x005C2100
bool MPCooperative::vt_entry_5C(unsigned a, unsigned b, unsigned c)
{
	if (Current == NULL) {
		return(false);
	}
	int index;
	Current->FUN_0049BCC0(&index);
	MPlayerScoreType * stats = (MPlayerScoreType *)c;
	bool result = Current->FUN_0049CC50(a, b, stats[0], stats[1]);
	if (index != (int)a || Field340) {
		HWND dialog = GameoptWindow();
		if (dialog) {
			FUN_005C2500(dialog);
			Field340 = false;
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005C21A0
void MPCooperative::vt_entry_78(unsigned a)
{
	HWND button = GetDlgItem((HWND)a, 0x732);
	if (button) {
		PostMessage(button, 0x405, 1, Options.CampDifficulty);
	}
}

// FUNCTION: GAMEMD 0x005C21D0
void MPCooperative::FUN_005C21D0(HWND dialog, bool show)
{
	bool ok = true;
	if (show) {
		FUN_005EBC00(dialog, 2);
	}
	if (Current) {
		int campaign;
		int index;
		if (!Current->FUN_0049BC80(&campaign)) {
			ok = false;
		}
		if (Current->FUN_0049BCC0(&index) && ok) {
			char name[64];
			sprintf(name, "NAME:COOPDESC%d%d", campaign + 1, index + 1);
			HWND text = GetDlgItem(dialog, 0x72f);
			if (text == NULL) {
				return;
			}
			SendMessage(text, 0x4b2, 0, (LPARAM)StringTable::LoadString(name, NULL, "D:\\ra2mdpost\\MPCoop.cpp", 0x281));
			if (show) {
				ShowWindow(text, SW_SHOW);
				InvalidateRect(text, NULL, FALSE);
			} else {
				ShowWindow(text, SW_HIDE);
				InvalidateRect(text, NULL, FALSE);
			}
		}
	}
	HWND button = GetDlgItem(dialog, 0x732);
	if (button) {
		PostMessage(button, 0x4ac, 0, 0);
		PostMessage(button, 0x406, 1, 0x20000);
		PostMessage(button, 0x405, 1, Options.CampDifficulty);
		if (show) {
			ShowWindow(button, SW_SHOW);
		} else {
			ShowWindow(button, SW_HIDE);
		}
		if (FUN_005E8760()) {
			EnableWindow(button, TRUE);
		} else {
			EnableWindow(button, FALSE);
		}
		InvalidateRect(button, NULL, TRUE);
	}
	HWND item = GetDlgItem(dialog, 0x78e);
	if (item) {
		if (show) {
			ShowWindow(item, SW_SHOW);
		} else {
			ShowWindow(item, SW_HIDE);
		}
	}
	item = GetDlgItem(dialog, 0x78d);
	if (item) {
		if (show) {
			ShowWindow(item, SW_SHOW);
		} else {
			ShowWindow(item, SW_HIDE);
		}
	}
	item = GetDlgItem(dialog, 0x78c);
	if (item) {
		if (show) {
			ShowWindow(item, SW_SHOW);
		} else {
			ShowWindow(item, SW_HIDE);
		}
	}
}

// FUNCTION: GAMEMD 0x005C23B0
void MPCooperative::FUN_005C23B0(HWND dialog, int control)
{
	int total = DAT_00A83CA8;
	if (total == 0) {
		return;
	}
	HWND list = GetDlgItem(dialog, control);
	ShowWindow(list, SW_HIDE);
	SendMessage(list, 0x14b, 0, 0);
	CoopCampEntry * entry;
	int index = 0;
	DAT_0089F518.FUN_0049E3B0(CampaignIndex, &entry);
	if (entry == NULL || entry->ListA.Count() == 0) {
		return;
	}
	if (Current != NULL) {
		if (!Current->FUN_0049BCC0(&index)) {
			index = 0;
		}
	} else {
		index = 0;
	}
	if (index >= entry->ListA.Count()) {
		return;
	}
	int group = entry->ListA[index]->Count();
	if (group > 1) {
		LRESULT item = SendMessage(list, 0x4c2, 0, (LPARAM)StringTable::LoadString("GUI:Random", NULL, "D:\\ra2mdpost\\MPCoop.cpp", 0x2ed));
		SendMessage(list, 0x151, item, -2);
	}
	for (int i = 0; i < group; i++) {
		int value = FUN_0049D9F0((*entry->ListA[index])[i]);
		if (value >= 0 && value < total) {
			LPARAM name = (LPARAM)DAT_00A83C9C[value]->Name;
			LRESULT item = SendMessage(list, 0x4c2, 0, name);
			SendMessage(list, 0x151, item, value);
		}
	}
	SendMessage(list, 0x14e, 0, 0);
	ShowWindow(list, SW_SHOW);
}

// FUNCTION: GAMEMD 0x005C2500
void MPCooperative::FUN_005C2500(HWND dialog)
{
	if (dialog == NULL) {
		return;
	}
	int count = Session.unknown_8D4;
	CoopCampEntry * entry;
	int index = 0;
	DAT_0089F518.FUN_0049E3B0(CampaignIndex, &entry);
	if (entry == NULL || entry->ListA.Count() == 0) {
		return;
	}
	if (Current != NULL) {
		if (!Current->FUN_0049BCC0(&index)) {
			index = 0;
		}
	} else {
		index = 0;
	}
	if (index >= entry->ListA.Count()) {
		return;
	}
	int group = entry->ListA[index]->Count();
	for (int i = 0; i < count; i++) {
		int slot = FUN_004E37D0(i);
		FUN_005C23B0(dialog, slot);
		if (group > 1) {
			FUN_004E3F70(dialog, slot, -2);
			Session.Slots[i]->Set_Country(-2);
		} else {
			int value = FUN_0049D9F0((*entry->ListA[index])[0]);
			FUN_004E3F70(dialog, slot, value);
			Session.Slots[i]->Set_Country(value);
		}
	}
	FUN_005EBC00(dialog, count);
	HWND options = GameoptWindow();
	if (options) {
		FUN_005C21D0(options, true);
	}
}

// FUNCTION: GAMEMD 0x005C2640
void MPCooperative::vt_entry_38(unsigned a)
{
	HWND dialog = (HWND)a;
	if (dialog) {
		int count = Session.Players.Count();
		for (int i = 0; i < count; i++) {
			FUN_005C23B0(dialog, FUN_004E37D0(i));
		}
		FUN_005C21D0(dialog, true);
	}
}

// FUNCTION: GAMEMD 0x005C2680
bool MPCooperative::vt_entry_60(void)
{
	if (Current == NULL) {
		return(false);
	}
	Current->FUN_0049BC80(&CampaignIndex);
	HWND dialog = GameoptWindow();
	if (dialog && Field340) {
		FUN_005C2500(dialog);
		Field340 = false;
	}
	return(Current->FUN_0049D680());
}

// FUNCTION: GAMEMD 0x005C26D0
void FUN_005C26D0(void)
{
	for (int k = 0; k < 8; k++) {
		DAT_00ABF460[k] = -2;
	}
	int count = Session.Players.Count();
	for (int i = 0; i < count; i++) {
		DAT_00ABF460[i] = Session.Players[i]->Get_Color_Or_Random();
	}
}

static inline bool Slot_Taken(int value)
{
	for (int q = 0; q < 8; q++) {
		if (DAT_00ABF460[q] == value) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005C2710
bool MPCooperative::vt_entry_64(void)
{
	CoopCampEntry * entry;
	char buffer[400];
	char piece[16];
	DAT_0089F518.FUN_0049E3B0(CampaignIndex, &entry);
	sprintf(buffer, "C1,%d", entry->MapCount);
	for (int i = 0; i < 8; i++) {
		int max = Lists[i].Length();
		Lists[i].Clear();
		Lists[i].Resize(max, 0);
		max = Lists2[i].Length();
		Lists2[i].Clear();
		Lists2[i].Resize(max, 0);
	}
	int count = Session.Players.Count();
	for (int k = 0; k < entry->MapCount; k++) {
		for (int m = 0; m < 8; m++) {
			DAT_00ABF460[m] = -2;
		}
		int humans = Session.Players.Count();
		for (int p = 0; p < humans; p++) {
			DAT_00ABF460[p] = Session.Players[p]->Get_Color_Or_Random();
		}
		for (int j = 0; j < 8; j++) {
			if (j < count) {
				Lists[j].Add(RandomHumanCountryIndex());
				Lists2[j].Add(DAT_00ABF460[j]);
			} else {
				Lists[j].Add(RandomAICountryIndex());
				int pick;
				do {
					pick = Scen->RandomNumber(0, 7);
				} while (pick != -2 && Slot_Taken(pick));
				DAT_00ABF460[j] = pick;
				Lists2[j].Add(pick);
			}
			sprintf(piece, ",%d,%d", Lists[j][k], Lists2[j][k]);
			strcat(buffer, piece);
		}
	}
	FUN_004068E0(DAT_0082DC84, buffer);
	FUN_005E3720(buffer);
	FUN_004068E0(DAT_0081F1D4);
	return(true);
}

// FUNCTION: GAMEMD 0x005C2A60
void MPCooperative::vt_entry_74(unsigned a, unsigned b)
{
	if (a != 1) {
		return;
	}
	char * token = strtok((char *)b, ",");
	if (token == NULL) {
		return;
	}
	int rounds = atol(token);
	if (rounds < 1) {
		return;
	}
	for (int i = 0; i < 8; i++) {
		int max = Lists[i].Length();
		Lists[i].Clear();
		Lists[i].Resize(max, 0);
		max = Lists2[i].Length();
		Lists2[i].Clear();
		Lists2[i].Resize(max, 0);
	}
	for (int round = 0; round < rounds; round++) {
		for (int j = 0; j < 8; j++) {
			token = strtok(NULL, ",");
			if (token) {
				Lists[j].Add(atol(token));
			} else {
				Lists[j].Add(-1);
			}
			token = strtok(NULL, ",");
			if (token) {
				Lists2[j].Add(atol(token));
			} else {
				Lists2[j].Add(-1);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005C2CB0
bool MPCooperative::UnfixAlliances(void)
{
	HWND dialog = GameoptWindow();
	if (dialog == NULL) {
		dialog = WS_Find_Dialog(0xbc6);
	}
	if (dialog) {
		FUN_005C21D0(dialog, true);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005C2CE0
void MPCooperative::PopulateTeamDropdownForPlayer(HWND hwnd, int index)
{
	MultiplayerGameMode::PopulateTeamDropdownForPlayer(hwnd, index);
}


static inline int Find_Player_Node(int house)
{
	for (int k = 0; k < Session.Players.Count(); k++) {
		if (Session.Players[k]->Slot.HouseIndex == house) {
			return(k);
		}
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x005C2D00
int MPCooperative::FUN_005C2D00(int house, DynamicVectorClass<Cell> * coords, unsigned char * satisfied)
{
	int usedCount = 0;
	for (int i = 0; i < coords->Count(); i++) {
		if (satisfied[i]) {
			usedCount++;
		}
	}
	int humans = 0;
	for (int h = 0; h < Houses.Count(); h++) {
		if (Houses[h] != NULL && Houses[h]->FUN_0050B730()) {
			humans++;
		}
	}
	int index;
	if (usedCount < humans) {
		int last = Scen->NumCoopHumanStartSpots - 1;
		index = Scen->RandomNumber(0, last);
		while (satisfied[index]) {
			index = (index + 1) % Scen->NumCoopHumanStartSpots;
		}
	} else {
		index = Scen->NumCoopHumanStartSpots;
		while (satisfied[index] && index < coords->Count()) {
			index++;
		}
	}
	satisfied[index] = 1;
	Scen->HouseIndices[index] = house;
	return(index);
}

// FUNCTION: GAMEMD 0x005C2DE0
Cell * MPCooperative::AssignStartingPositionsToHouse(Cell * result, int house, DynamicVectorClass<Cell> * coords, unsigned char * satisfied)
{
	int usedCount = 0;
	for (int i = 0; i < coords->Count(); i++) {
		if (satisfied[i]) {
			usedCount++;
		}
	}
	int humans = 0;
	for (int h = 0; h < Houses.Count(); h++) {
		if (Houses[h] != NULL && Houses[h]->FUN_0050B730()) {
			humans++;
		}
	}
	if (usedCount < humans) {
		int last = Scen->NumCoopHumanStartSpots - 1;
		int index = Scen->RandomNumber(0, last);
		while (satisfied[index]) {
			index = (index + 1) % Scen->NumCoopHumanStartSpots;
		}
		satisfied[index] = 1;
		Scen->HouseIndices[index] = house;
		*result = (*coords)[index];
		return(result);
	}
	int index = Scen->NumCoopHumanStartSpots;
	while (satisfied[index] && index < coords->Count()) {
		index++;
	}
	satisfied[index] = 1;
	Scen->HouseIndices[index] = house;
	*result = (*coords)[index];
	return(result);
}

// FUNCTION: GAMEMD 0x005C2EF0
bool MPCooperative::StartingPositionsToHouseBaseCells2(bool arg)
{
	DynamicVectorClass<Cell> cells = FUN_00688380(arg);
	unsigned char satisfied[16];
	for (int i = 0; i < 16; i++) {
		if (Scen->HouseIndices[i] == -1) {
			satisfied[i] = 0;
		} else {
			satisfied[i] = 1;
		}
	}
	for (int h = 0; h < Houses.Count(); h++) {
		bool found = false;
		int slot = -1;
		HouseClass * house = Houses[h];
		if (!house->Class->IsMultiplayPassive) {
			for (int i = 0; i < 16; i++) {
				if (Scen->HouseIndices[i] == h) {
					found = true;
					slot = i;
				}
			}
			if (!found) {
				FUN_004068E0("Assigning Starting Points for house %d (%s)\n", h, (char const *)house->Class->IniName);
				int start = FUN_005C2D00(h, &cells, satisfied);
				int node = Find_Player_Node(h);
				if (node != -1) {
					Session.Players[node]->Set_Start(start);
					Session.Players[node]->Set_Team(0);
					FUN_004068E0("Playernum %d:Starting position is %d, team is %d\n", node, Session.Players[node]->Get_Start(), Session.Players[node]->Get_Team());
				} else {
					int c = 0;
					while ((Session.Options.AISlots.Countries[c] == -1 || Session.Options.AISlots.Starts[c] != -2) && c < 8) {
						c++;
					}
					Session.Options.AISlots.Starts[c] = start;
					Session.Options.AISlots.Allies[c] = 1;
					FUN_004068E0("AI Playernum %d:Starting position is %d, team is %d\n", c, Session.Options.AISlots.Starts[c], 1);
				}
				house->Set_Base_Spawn_Cell(cells[start]);
			} else {
				house->Set_Base_Spawn_Cell(cells[slot]);
			}
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005C30C0
bool MPCooperative::vt_entry_68(void)
{
	int aiCount = FUN_005E6520(Session.Options.ScenarioIndex) - Scen->NumCoopHumanStartSpots;
	FUN_004068E0("@@@@ Adding %d AI players\n", aiCount);
	if (aiCount <= 0) {
		return(false);
	}
	Session.Options.AIPlayers = aiCount;
	CoopCampEntry * entry;
	int added = 0;
	int group = 0;
	DAT_0089F518.FUN_0049E3B0(CampaignIndex, &entry);
	if (entry == NULL) {
		return(false);
	}
	if (entry->ListB.Count() == 0) {
		return(false);
	}
	if (Current == NULL || !Current->FUN_0049BCC0(&group)) {
		group = 0;
	}
	if (group >= entry->ListB.Count()) {
		return(false);
	}
	for (int slot = Scen->NumCoopHumanStartSpots; slot < 8; slot++) {
		if (added < aiCount) {
			FUN_004068E0("@@@@ Setting slot %d for AI\n", slot);
			if (Options.CampDifficulty == 0) {
				Session.Options.AISlots.Difficulties[slot] = 2;
			} else {
				Session.Options.AISlots.Difficulties[slot] = (Options.CampDifficulty == 1);
			}
			FUN_004068E0("@@@@     AI difficulty: %d\n", Session.Options.AISlots.Difficulties[slot]);
			Session.Options.AISlots.Countries[slot] = -2;
			Session.Options.AISlots.Colors[slot] = -2;
			Session.Options.AISlots.Starts[slot] = -2;
			Session.Options.AISlots.Allies[slot] = -2;
			added++;
		} else {
			Session.Options.AISlots.Difficulties[slot] = -1;
			Session.Options.AISlots.Countries[slot] = -1;
			Session.Options.AISlots.Colors[slot] = -1;
			Session.Options.AISlots.Starts[slot] = -1;
			Session.Options.AISlots.Allies[slot] = -1;
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005C3220
bool MPCooperative::AllyTeams(void)
{
	FUN_004068E0("@@@@ Making Alliances\n");
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * first = Houses[i];
		char * houseName = FUN_007350C0(first->UIName);
		FUN_004068E0("@@@@     House1 = %s, team is %d\n", houseName, first->StartingAllies);
		if (!first->Class->IsMultiplayPassive && first->IsHuman ) {
			for (int j = i + 1; j < Houses.Count(); j++) {
				HouseClass * second = Houses[j];
				FUN_004068E0("@@@@         House2 = %s\n", FUN_007350C0(second->UIName));
				if (!second->Class->IsMultiplayPassive && second->IsHuman ) {
					char * secondName = FUN_007350C0(second->UIName);
					char * firstName = FUN_007350C0(first->UIName);
					FUN_004068E0("@@@@ Making Allies of %s(%d/%d/%d) and %s(%d/%d/%d)\n", firstName, first->Class->House, first->Class->Side, first->Class->Scheme, secondName, second->Class->House, second->Class->Side, second->Class->Scheme);
					first->Make_Ally(second, false);
					second->Make_Ally(first, false);
				}
			}
		}
	}
	for (int m = 0; m < Houses.Count(); m++) {
		HouseClass * first = Houses[m];
		char * houseName = FUN_007350C0(first->UIName);
		FUN_004068E0("@@@@     House1 = %s, team is %d\n", houseName, first->StartingAllies);
		if (!first->Class->IsMultiplayPassive && !first->IsHuman) {
			for (int n = m + 1; n < Houses.Count(); n++) {
				HouseClass * second = Houses[n];
				FUN_004068E0("@@@@         House2 = %s\n", FUN_007350C0(second->UIName));
				if (!second->Class->IsMultiplayPassive && !second->IsHuman) {
					char * secondName = FUN_007350C0(second->UIName);
					char * firstName = FUN_007350C0(first->UIName);
					FUN_004068E0("@@@@ Making Allies of %s(%d/%d/%d) and %s(%d/%d/%d)\n", firstName, first->Class->House, first->Class->Side, first->Class->Scheme, secondName, second->Class->House, second->Class->Side, second->Class->Scheme);
					first->Make_Ally(second, false);
					second->Make_Ally(first, false);
				}
			}
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005C34F0
int MPCooperative::FUN_005C34F0(void)
{
	int total = DAT_00A83CA8;
	bool found = false;
	int campaign = 0;
	CoopCampEntry * entry;
	Current->FUN_0049BC80(&campaign);
	int group = 0;
	DAT_0089F518.FUN_0049E3B0(campaign, &entry);
	if (entry == NULL) {
		return(0);
	}
	if (Current == NULL || !Current->FUN_0049BCC0(&group)) {
		group = 0;
	}
	if (group >= entry->ListA.Count()) {
		return(0);
	}
	int pick;
	do {
		pick = Scen->RandomNumber(0, total - 1);
		for (int i = 0; i < entry->ListA[group]->Count(); i++) {
			if (pick == FUN_0049D9F0((*entry->ListA[group])[i])) {
				found = true;
			}
		}
	} while (!found);
	FUN_004068E0("@@@@ Choosing a random player house: %d(%s)\n", pick, FUN_007350C0(DAT_00A83C9C[pick]->Name));
	return(pick);
}

// FUNCTION: GAMEMD 0x005C35F0
int MPCooperative::FUN_005C35F0(void)
{
	int total = DAT_00A83CA8;
	bool found = false;
	int campaign = 0;
	CoopCampEntry * entry;
	Current->FUN_0049BC80(&campaign);
	int group = 0;
	DAT_0089F518.FUN_0049E3B0(campaign, &entry);
	if (entry == NULL) {
		return(0);
	}
	if (Current == NULL || !Current->FUN_0049BCC0(&group)) {
		group = 0;
	}
	if (group >= entry->ListB.Count()) {
		return(0);
	}
	int pick;
	do {
		pick = Scen->RandomNumber(0, total - 1);
		for (int i = 0; i < entry->ListB[group]->Count(); i++) {
			if (pick == FUN_0049D9F0((*entry->ListB[group])[i])) {
				found = true;
			}
		}
	} while (!found);
	FUN_004068E0("@@@@ Choosing a random AI house: %d(%s)\n", pick, FUN_007350C0(DAT_00A83C9C[pick]->Name));
	return(pick);
}

// FUNCTION: GAMEMD 0x005C36F0
bool MPCooperative::vt_entry_8C(void)
{
	return(MultiplayerGameMode::vt_entry_8C());
}

// FUNCTION: GAMEMD 0x005C3700
bool MPCooperative::IsAIAllowed(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x005C3710
void MPCooperative::vt_entry_A0(unsigned a)
{
	bool playerWins = false;
	for (int i = 0; i < Houses.Count(); i++) {
		if (Houses[i] != NULL && Houses[i]->FUN_0050B730() && !Houses[i]->IsDefeated) {
			playerWins = true;
		}
	}
	FUN_004068E0("@@@@ End-of-Game: PlayerWins:%d, win:%d\n", (unsigned char)a, playerWins);
	if (playerWins) {
		int scheme = GadgetClass::Get_Color_Scheme();
		if (Current != NULL) {
			FUN_0049B930(Current);
			if (Current->FUN_0049C5A0()) {
				Session.Messages.Add_Message(NULL, 0, StringTable::LoadString("TXT_COOP_SAVE_SUCCESS", NULL, "D:\\ra2mdpost\\MPCoop.cpp", 0x5a8), scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
			} else {
				Session.Messages.Add_Message(NULL, 0, StringTable::LoadString("TXT_COOP_SAVE_FAILED", NULL, "D:\\ra2mdpost\\MPCoop.cpp", 0x5aa), scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
			}
		}
	}
	void * file;
	Current->FUN_0049D070(&file, -1);
	char mapname[_MAX_PATH];
	sprintf(mapname, "%s.MAP", file);
	strcpy(Scen->NextScenarioName, mapname);
	Scen->Campaign = (CampaignType)-1;
	Scen->IsSkipScore = true;
	int total = Session.Scenarios.Count();
	sprintf(mapname, "%s.MAP", file);
	int index = 0;
	for (; index < total; index++) {
		if (strncmp(mapname, Scenario_Filename(Session.Scenarios[index]), _MAX_PATH) == 0) {
			Session.Options.ScenarioIndex = index;
			break;
		}
	}
	FUN_005E7BF0(Session.Options.ScenarioIndex);
	Scen->IsSkipScore = true;
	Scen->IsEndOfGame = false;
	Scen->IsNoMapSel = true;
	Scen->IsOneTimeOnly = false;
	Field54 = 0;
	for (int k = 0; k < Session.Players.Count(); k++) {
		if (Session.HostAddress == Session.Players[k]->Address) {
			Field54 = k;
		}
	}
	memcpy(Addresses, Session.unknown_2994, sizeof(Addresses));
	memcpy(Field124, Session.unknown_29F4, sizeof(Field124));
	memcpy(Field144, Session.unknown_2FF8, sizeof(Field144));
	memcpy(Field194, Session.PlayerPorts, sizeof(Field194));
	wcscpy(Field164, Session.MasterPlayerName);
	Field190 = Session.MasterPlayerID;
	Field70 = Session.House;
	Seed = Game::Seed;
	TechLevel = Game::TechLevel;
	UnitCount = Session.Options.UnitCount;
	Credits = Session.Options.Credits;
	FogOfWar = Session.Options.FogOfWar;
	BridgeDestruction = Session.Options.BridgeDestruction;
	Goodies = Session.Options.Goodies;
	MCVRedeploy = Session.Options.MCVRedeploy;
	AlliesAllowed = Session.Options.AlliesAllowed;
	HarvTruce = Session.Options.HarvTruce;
	Bases = Session.Options.Bases;
	CTF = Session.Options.CTF;
	AIPlayers = Session.Options.AIPlayers;
	AIDifficulty = Session.Options.AIDifficulty;
	ShortGame = Session.Options.ShortGame;
	SWAllowed = Session.Options.SWAllowed;
	BuildOffAlly = Session.Options.BuildOffAlly;
	CrapEngineers = Session.Options.CrapEngineers;
	GameSpeed = Session.Options.GameSpeed;
	Session.MultiplayerObserver = 0;
	Map.SetActiveFoundation(NULL);
	DAT_0088098C = 0;
	DAT_00880990 = 0;
	DAT_00880994 = -1;
	for (int n = 0; n < 4; n++) {
		Session.Score[n].Kills[Session.CurGame] = -1;
	}
}

// FUNCTION: GAMEMD 0x005C3EC0
bool MPCooperative::FUN_005C3EC0(void)
{
	CoopCampaignClass * old = Current;
	if (old) {
		old->FUN_0049B720();
		operator delete(old);
	}
	Current = NULL;
	CoopCampaignClass * raw = (CoopCampaignClass *)operator new(sizeof(CoopCampaignClass));
	Current = raw ? raw->FUN_0049B610() : 0;
	Current->FUN_0049CAF0(0);
	if (Session.Players.Count() == 2) {
		Current->FUN_0049B760(Session.Players[0]->Name, Session.Players[1]->Name);
	}
	if (0) {
		Current->FUN_0049B720();
		Current->FUN_0049B720();
	}
	return(Current ? true : false);
}

// FUNCTION: GAMEMD 0x005C3F30
void MPCooperative::vt_entry_9C(void)
{
	HWND dialog = GameoptWindow();
	if (dialog) {
		FUN_005C21D0(dialog, false);
		FUN_004E3B90(dialog);
	}
	for (int i = 0; i < Session.Players.Count(); i++) {
		Session.Players[i]->Slot.SpectatorFlag = i;
	}
}

// FUNCTION: GAMEMD 0x005C3F80
void MPCooperative::vt_entry_90(void)
{
	if (FUN_00775B20() == 0x6b) {
		if (Field49) {
			HWND list = GetDlgItem(FUN_00775B10(), 0x553);
			int count = SendMessage(list, LB_GETCOUNT, 0, 0);
			if (count == -1) {
				if (Current) {
					Current->FUN_0049B760(DAT_00887734, DAT_00887734);
				}
				return;
			}
			if (SendMessage(list, LB_GETCURSEL, 0, 0) == count - 1) {
				SendMessage(list, LB_SETCURSEL, count - 2, 0);
			}
			SendMessage(list, LB_DELETESTRING, count - 1, 0);
			Field49 = false;
		}
	} else {
		if (Current && Field48) {
			if (Field4C) {
				Current->FUN_0049CAF0(0);
				Current->FUN_0049CAF0(Field4C);
			} else {
				Current->FUN_0049CAF0(1);
				Current->FUN_0049CAF0(Field4C);
			}
			void * name;
			char buffer[_MAX_PATH];
			Current->FUN_0049D070(&name, -1);
			int n = Session.Scenarios.Count();
			sprintf(buffer, "%s.MAP", name);
			int i;
			for (i = 0; i < n; i++) {
				char const * file = Scenario_Filename(Session.Scenarios[i]);
				if (strcmpi(file, buffer) == 0) {
					break;
				}
			}
			Session.Options.ScenarioIndex = i;
			Scen->Scenario = i;
			FUN_005E7BF0(i);
		}
		HWND options = GameoptWindow();
		if (options) {
			FUN_005E2EF0(options, this);
			FUN_005E2F60(options);
			FUN_005E74E0(options);
		}
		FUN_005E32D0(true, true);
	}
	if (Current) {
		Current->FUN_0049B760(DAT_00887734, DAT_00887734);
	}
}

// FUNCTION: GAMEMD 0x005C4110
void MPCooperative::vt_entry_94(void)
{
	if (Current == NULL) {
		CoopCampaignClass * raw = (CoopCampaignClass *)operator new(sizeof(CoopCampaignClass));
		Current = raw ? raw->FUN_0049B610() : 0;
		Current->FUN_0049CAF0(0);
	}
	if (Current == NULL) {
		return;
	}
	if (Session.Players.Count() == 2) {
		Current->FUN_0049B760(Session.Players[0]->Name, Session.Players[1]->Name);
	}
	vt_entry_60();
	if (!Current->FUN_0049D120()) {
		return;
	}
	if (FUN_00775B20() == 0x6b) {
		if (!Field49) {
			char buffer[_MAX_PATH];
			Current->FUN_0049D390(buffer);
			strcat(buffer, ".MAP");
			MultiMission * found = NULL;
			int n = Session.Scenarios.Count();
			for (int i = 0; i < n; i++) {
				if (found) {
					break;
				}
				char const * file = Scenario_Filename(Session.Scenarios[i]);
				if (strcmpi(file, buffer) == 0) {
					found = Session.Scenarios[i];
				}
			}
			if (!found) {
				return;
			}
			HWND list = GetDlgItem(FUN_00775B10(), 0x553);
			int count = SendMessage(list, LB_GETCOUNT, 0, 0);
			if (count == -1) {
				return;
			}
			SendMessage(list, 0x4cd, 0, (LPARAM)StringTable::LoadString("TXT_COOP_USE_SAVE", NULL, "D:\\ra2mdpost\\MPCoop.cpp", 0x77a));
			SendMessage(list, LB_SETITEMDATA, count, (LPARAM)found);
			Field49 = true;
		}
	} else {
		PMessagePrintf(-1, StringTable::LoadString("TXT_SAVE_CAMP_AVAILABLE", NULL, "D:\\ra2mdpost\\MPCoop.cpp", 0x780));
	}
}

// FUNCTION: GAMEMD 0x005C42C0
int MPCooperative::vt_entry_98(void)
{
	return(2);
}

// FUNCTION: GAMEMD 0x005C42D0
bool MPCooperative::vt_entry_A8(void)
{
	if (Field50 == NULL) {
		Field50 = new MPCoopObject;
	}
	if (Current == NULL) {
		return(false);
	}
	DynamicVectorClass<MPlayerScoreType *> stats;
	int unused;
	Current->FUN_0049BCE0(&stats, &unused);
	CoopCampEntry * entry = NULL;
	int campaign;
	if (!Current->FUN_0049BC80(&campaign)) {
		campaign = -1;
	} else {
		DAT_0089F518.FUN_0049E3B0(campaign, &entry);
	}
	int side = -1;
	if (campaign == 0 || campaign == 1) {
		side = 0;
	} else if (campaign == 2 || campaign == 3) {
		side = 1;
	} else if (campaign == 4) {
		side = 2;
	}
	int total;
	int index;
	Current->FUN_0049BCA0(&total);
	Current->FUN_0049BCC0(&index);
	if (entry) {
		Field50->Send(stats, Scen->ElapsedTimer.Value(), index < total, side, entry->Title);
	} else {
		Field50->Send(stats, Scen->ElapsedTimer.Value(), index < total, side, DAT_00887734);
	}
	while (stats.Count()) {
		if (stats[0]) {
			delete stats[0];
		}
		stats[0] = NULL;
		stats.Delete_Index(0);
	}
	bool result = DAT_0089E04C;
	DAT_0089E04C = false;
	return(result);
}

// FUNCTION: GAMEMD 0x005C44E0
void MPCooperative::vt_entry_A4(unsigned a)
{
	if (Current == NULL) {
		return;
	}
	int total;
	int index;
	Current->FUN_0049BCA0(&total);
	Current->FUN_0049BCC0(&index);
	if ((char)a == 0) {
		return;
	}
	if (index < total) {
		return;
	}
	MPCoopReporter * reporter = new MPCoopReporter;
	if (reporter == NULL) {
		return;
	}
	DynamicVectorClass<MPlayerScoreType *> stats;
	int unused;
	Current->FUN_0049BCE0(&stats, &unused);
	CoopCampEntry * entry = NULL;
	int & campaign = (int &)a;
	if (!Current->FUN_0049BC80(&campaign)) {
		campaign = -1;
	} else {
		DAT_0089F518.FUN_0049E3B0(campaign, &entry);
	}
	reporter->Send(stats, unused, Options.CampDifficulty, campaign + 1, entry->Title);
	delete reporter;
	while (stats.Count()) {
		if (stats[0]) {
			delete stats[0];
		}
		stats[0] = NULL;
		stats.Delete_Index(0);
	}
}

// FUNCTION: GAMEMD 0x005C4670
int MPCooperative::vt_entry_B8(void)
{
	int result = 0;
	if (Current == NULL) {
		return(0);
	}
	int total;
	int index;
	Current->FUN_0049BCA0(&total);
	Current->FUN_0049BCC0(&index);
	if (total == index) {
		int campaign;
		Current->FUN_0049BC80(&campaign);
		result = 1 << campaign;
		FUN_004068E0("Woohoo!, I just finished cooperative campaign %d.  I get persistant accomplishment code 0x%032x\n", campaign, result);
	}
	return(result);
}

extern int DAT_0084A234;
extern int DAT_00B77D6C;
extern int DAT_00B779D4;
extern int DAT_00B779C4;
extern unsigned char DAT_00A8F900;
extern Wstring DAT_00B73E40;
extern char DAT_00B77830[];
extern WinsockInterfaceClass * DAT_00887628;
extern int DAT_00B7790C[];
extern unsigned char DAT_00B77A04;
extern unsigned char DAT_00B76568;
extern int DAT_00A8EA0C;
extern int DAT_00B052D0;
void __fastcall FUN_00664530(int * object);
void __fastcall FUN_005E7460(void);
struct CCToolTip {
	static ToolTipManager * Instance;
};

// FUNCTION: GAMEMD 0x005C46E0
void MPCooperative::vt_entry_AC(void)
{
	DAT_00B779D4 = 0;
	DAT_00B77D6C = DAT_0084A234;
	DAT_00B779C4 = 0;
	DAT_00A8F900 = 0;
	strcpy(DAT_00B77830, DAT_00B73E40.get());
	Session.HostAddress = Session.Players[Field54]->Address;
	wcscpy(Session.MasterPlayerName, Session.Players[Field54]->Name);
	Session.MasterPlayerID = -1;
	Session.House = Field70;
	DAT_00887628->Clear_Broadcast_Addresses();
	char ip[32] = "xxx.xxx.xxx.xxx";
	for (int i = 1; i < Session.Players.Count(); i++) {
		NetNumType net;
		NetNodeType node;
		int port;
		Session.Players[i]->Address.Get_Address(net, node, (unsigned short *)&port);
		DAT_00887628->Set_Broadcast_Address(ip, port);
	}
	memcpy(Session.unknown_2994, Addresses, sizeof(Addresses));
	memcpy(Session.unknown_29F4, Field124, sizeof(Field124));
	memcpy(Session.unknown_2FF8, Field144, sizeof(Field144));
	memcpy(Session.PlayerPorts, Field194, sizeof(Field194));
	wcscpy(Session.MasterPlayerName, Field164);
	Session.MasterPlayerID = Field190;
	Game::Seed = Seed;
	Game::InitRandom();
	FUN_004068E0("Setup Rematch: Random Seed = %d\n", Game::Seed);
	Game::TechLevel = TechLevel;
	Session.Options.UnitCount = UnitCount;
	Session.Options.Credits = Credits;
	Session.Options.FogOfWar = FogOfWar;
	Session.Options.BridgeDestruction = BridgeDestruction;
	Session.Options.Goodies = Goodies;
	Session.Options.MCVRedeploy = MCVRedeploy;
	Session.Options.AlliesAllowed = AlliesAllowed;
	Session.Options.HarvTruce = HarvTruce;
	Session.Options.Bases = Bases;
	Session.Options.CTF = CTF;
	Session.Options.AIDifficulty = (DiffType)AIDifficulty;
	Session.Options.ShortGame = ShortGame;
	Session.Options.SWAllowed = SWAllowed;
	Session.Options.BuildOffAlly = BuildOffAlly;
	Session.Options.CrapEngineers = CrapEngineers;
	Session.Options.GameSpeed = GameSpeed;
	FUN_005E7BF0(Session.Options.ScenarioIndex);
	Options.GameSpeed = Session.Options.GameSpeed;
	Scen->Scenario = Session.Options.ScenarioIndex;
	switch (Session.Options.GameSpeed) {
		case 0:
			Session.MaxAhead = 40;
			Session.PrecalcDesiredFrameRate = 60;
			Session.FrameSendRate = 10;
			break;
		case 1:
			Session.MaxAhead = 40;
			Session.PrecalcDesiredFrameRate = 45;
			Session.FrameSendRate = 10;
			break;
		case 2:
			Session.MaxAhead = 30;
			Session.PrecalcDesiredFrameRate = 30;
			break;
		case 3:
			Session.MaxAhead = 20;
			Session.PrecalcDesiredFrameRate = 20;
			break;
		case 4:
			Session.MaxAhead = 20;
			Session.PrecalcDesiredFrameRate = 15;
			break;
		case 5:
			Session.MaxAhead = 20;
			Session.PrecalcDesiredFrameRate = 12;
			break;
		default:
			Session.MaxAhead = 10;
			Session.PrecalcDesiredFrameRate = 10;
			break;
	}
	Session.PrecalcMaxAhead = 0;
	if (!unknown_4) {
		Session.PrecalcDesiredFrameRate = 0;
	}
	int worst = 0;
	for (int p = 0; p < Session.Players.Count(); p++) {
		if (DAT_00B7790C[p] > worst) {
			worst = DAT_00B7790C[p];
		}
	}
	Session.LatencyFudge = 1;
	if (worst != 0 && worst != 1000) {
		FUN_004068E0("Max ping is %d ms\n", worst);
		if (worst > 1000) {
			Session.LatencyFudge = 2;
		} else if (worst < 600) {
			Session.LatencyFudge = 1;
		}
		int ticks = 60 * worst / 1000;
		ticks += 120 * ticks / 100;
		Ipx.Set_Timing(ticks, (unsigned int)-1, ticks * 8);
		if (unknown_4) {
			Ipx.Set_Lobby_Timing(5, (unsigned int)-1, 0x78);
		}
		if (unknown_4) {
			unsigned ahead;
			switch (Session.LatencyFudge) {
				case 0:
				case 1:
					ahead = Session.MaxAhead;
					break;
				case 2:
					ahead = (Session.MaxAhead / 2) + Session.MaxAhead;
					break;
				case 3:
					ahead = Session.MaxAhead * 2;
					break;
				default:
					ahead = Session.PrecalcMaxAhead;
					break;
			}
			ahead = 10 * (ahead / 10);
			Session.PrecalcMaxAhead = ahead;
		}
	} else {
		Session.PrecalcDesiredFrameRate = 0;
		FUN_004068E0("Ping times not available\n");
	}
	Session.MaxMaxAhead = Session.MaxAhead;
	memset(Session.ConnectionStats, 0, sizeof(Session.ConnectionStats));
	DAT_00B77A04 = 0;
	Options.GameSpeed = Session.Options.GameSpeed;
	DAT_00B76568 = 0;
	FUN_005E7460();
	DoList.Init();
	OutList.Init();
	CCToolTip::Instance->Activate(false);
	FUN_004068E0("@@@@ Before destroying connections players.count() = %d\n", Session.Players.Count());
	while (Ipx.Num_Connections()) {
		Ipx.Delete_Connection(Ipx.Connection_ID(0));
	}
	if (Session.ViaPacketRouter) {
		if (DAT_00A8EA0C) {
			FUN_00664530(&DAT_00B052D0);
		}
		Session.ViaPacketRouter = 0;
	}
	FUN_004068E0("@@@@ After destroying connections players.count() = %d\n", Session.Players.Count());
}

// FUNCTION: GAMEMD 0x005C4CA0
void MPCooperative::vt_entry_B0(void)
{
	FUN_005C6120(&Session.Players);
	Session.NumPlayers = 0;
	for (int n = 0; n < Names.Count(); n++) {
		for (int i = 0; i < DAT_00A80238; i++) {
			if (wcsicmp(DAT_00A8022C[i]->Name, Names[n]->Name) == 0) {
				Names[n]->Slot.HouseIndex = i;
				NodeNameType * node = new NodeNameType;
				wcscpy(node->Name, Names[n]->Name);
				node->Slot.Clan = Names[n]->Slot.Clan;
				node->Slot.Country = Names[n]->Slot.Country;
				node->Slot.ColorIdx = Names[n]->Slot.ColorIdx;
				strcpy(node->Player.Serial, Names[n]->Player.Serial);
				node->Slot.InitialCountry = Names[n]->Slot.InitialCountry;
				node->Slot.InitialColor = Names[n]->Slot.InitialColor;
				node->Slot.SpectatorFlag = Names[n]->Slot.SpectatorFlag;
				node->Slot.Time = Names[n]->Slot.Time;
				node->Slot.Unknown77 = Names[n]->Slot.Unknown77;
				node->Slot.Clan = Names[n]->Slot.Clan;
				NetNumType net;
				NetNodeType host;
				unsigned short port;
				Names[n]->Address.Get_Address(net, host, &port);
				node->Address.Set_Address(net, host, port);
				Session.Players.Add(node);
				Session.NumPlayers++;
				break;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005C4E90
bool MPCooperative::vt_entry_B4(DynamicVectorClass<int> list, int * out)
{
	*out = 0;
	if (Current) {
		return(true);
	} else {
		return(false);
	}
}

// FUNCTION: GAMEMD 0x005C4EE0
bool MPCooperative::vt_entry_04(void)
{
	return(true);
}

// FUNCTION: GAMEMD 0x005C4EF0
bool MPCooperative::vt_entry_08(void)
{
	return(true);
}

// FUNCTION: GAMEMD 0x005C4F00
bool MPCooperative::vt_entry_BC(void)
{
	return(false);
}

// SYNTHETIC: GAMEMD 0x005C4F10
// ??_GMPCooperative@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x005C4F30
// ??0?$DynamicVectorClass@PAUMPlayerScoreType@@@@QAE@ABV0@@Z
// SYNTHETIC: GAMEMD 0x005C4FC0
// ??_G?$DynamicVectorClass@PAUMPlayerScoreType@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x005C5010
// ?Clear@?$DynamicVectorClass@PAUMPlayerScoreType@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005C5040
// ?ID@?$DynamicVectorClass@PAUMPlayerScoreType@@@@UAEHPBQAUMPlayerScoreType@@@Z
// TEMPLATE: GAMEMD 0x005C5060
// ?GetItem@?$VectorClass@PAUMPlayerScoreType@@@@UBEPAUMPlayerScoreType@@H@Z
template class VectorClass<MPlayerScoreType *>;
template class DynamicVectorClass<MPlayerScoreType *>;

// TEMPLATE: GAMEMD 0x005C5070
// ?Clear@?$DynamicVectorClass@PAUNodeNameType@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005C50A0
// ?ID@?$DynamicVectorClass@PAUNodeNameType@@@@UAEHPBQAUNodeNameType@@@Z
// TEMPLATE: GAMEMD 0x005C50C0
// ?GetItem@?$VectorClass@PAUNodeNameType@@@@UBEPAUNodeNameType@@H@Z
template class DynamicVectorClass<NodeNameType *>;

// TEMPLATE: GAMEMD 0x005C50D0
// ?Clear@?$DynamicVectorClass@PAVCoopCampaignClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005C5100
// ?ID@?$DynamicVectorClass@PAVCoopCampaignClass@@@@UAEHPBQAVCoopCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x005C5120
// ?GetItem@?$VectorClass@PAVCoopCampaignClass@@@@UBEPAVCoopCampaignClass@@H@Z
// TEMPLATE: GAMEMD 0x005C5130
// ??8?$VectorClass@PAUMPlayerScoreType@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005C5170
// ?Resize@?$VectorClass@PAUMPlayerScoreType@@@@UAE_NHPBQAUMPlayerScoreType@@@Z
// TEMPLATE: GAMEMD 0x005C5220
// ?Clear@?$VectorClass@PAUMPlayerScoreType@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005C5250
// ?ID@?$VectorClass@PAUMPlayerScoreType@@@@UAEHABQAUMPlayerScoreType@@@Z
// TEMPLATE: GAMEMD 0x005C5290
// ?ID@?$VectorClass@PAUMPlayerScoreType@@@@UAEHPBQAUMPlayerScoreType@@@Z
// TEMPLATE: GAMEMD 0x005C52B0
// ?Resize@?$DynamicVectorClass@PAUMPlayerScoreType@@@@UAE_NHPBQAUMPlayerScoreType@@@Z
// TEMPLATE: GAMEMD 0x005C5360
// ?ID@?$DynamicVectorClass@PAUMPlayerScoreType@@@@UAEHABQAUMPlayerScoreType@@@Z
template class VectorClass<CoopCampaignClass *>;
template class DynamicVectorClass<CoopCampaignClass *>;

// TEMPLATE: GAMEMD 0x005C5390
// ??8?$VectorClass@PAUNodeNameType@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005C53D0
// ?Resize@?$VectorClass@PAUNodeNameType@@@@UAE_NHPBQAUNodeNameType@@@Z
// TEMPLATE: GAMEMD 0x005C5480
// ?Clear@?$VectorClass@PAUNodeNameType@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005C54B0
// ?ID@?$VectorClass@PAUNodeNameType@@@@UAEHABQAUNodeNameType@@@Z
// TEMPLATE: GAMEMD 0x005C54F0
// ?ID@?$VectorClass@PAUNodeNameType@@@@UAEHPBQAUNodeNameType@@@Z
// TEMPLATE: GAMEMD 0x005C5510
// ?Resize@?$DynamicVectorClass@PAUNodeNameType@@@@UAE_NHPBQAUNodeNameType@@@Z
// TEMPLATE: GAMEMD 0x005C55C0
// ?ID@?$DynamicVectorClass@PAUNodeNameType@@@@UAEHABQAUNodeNameType@@@Z
// TEMPLATE: GAMEMD 0x005C55F0
// ??8?$VectorClass@PAVCoopCampaignClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x005C5630
// ?Resize@?$VectorClass@PAVCoopCampaignClass@@@@UAE_NHPBQAVCoopCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x005C56E0
// ?Clear@?$VectorClass@PAVCoopCampaignClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x005C5710
// ?ID@?$VectorClass@PAVCoopCampaignClass@@@@UAEHABQAVCoopCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x005C5750
// ?ID@?$VectorClass@PAVCoopCampaignClass@@@@UAEHPBQAVCoopCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x005C5770
// ?Resize@?$DynamicVectorClass@PAVCoopCampaignClass@@@@UAE_NHPBQAVCoopCampaignClass@@@Z
// TEMPLATE: GAMEMD 0x005C5820
// ?ID@?$DynamicVectorClass@PAVCoopCampaignClass@@@@UAEHABQAVCoopCampaignClass@@@Z
// SYNTHETIC: GAMEMD 0x005C5850
// ??_G?$VectorClass@PAUMPlayerScoreType@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005C58A0
// ??_G?$VectorClass@PAUNodeNameType@@@@UAEPAXI@Z
template class VectorClass<NodeNameType *>;
// SYNTHETIC: GAMEMD 0x005C58F0
// ??_G?$DynamicVectorClass@PAUNodeNameType@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005C5940
// ??_G?$VectorClass@PAVCoopCampaignClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005C5990
// ??_G?$DynamicVectorClass@PAVCoopCampaignClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x005C59E0
// ??0?$VectorClass@PAUMPlayerScoreType@@@@QAE@HPBQAUMPlayerScoreType@@@Z
// TEMPLATE: GAMEMD 0x005C5A30
// ??0?$VectorClass@PAUNodeNameType@@@@QAE@HPBQAUNodeNameType@@@Z
// TEMPLATE: GAMEMD 0x005C5A80
// ??0?$VectorClass@PAVCoopCampaignClass@@@@QAE@HPBQAVCoopCampaignClass@@@Z
