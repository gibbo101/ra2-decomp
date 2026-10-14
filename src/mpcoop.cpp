// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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
#include <cstdio>
#include <cstring>
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include <stdlib.h>
#include "stringtable.h"

#pragma pack(push, 1)
class MPPlayerNode
{
	public:
		int FUN_00696F10(void);
		void FUN_00696F90(int slot);
		int FUN_00696F30(void);
		void FUN_00696FF0(int start);
		void FUN_00697020(int flag);
		int FUN_00696F70(void);
		int FUN_00696F50(void);
		wchar_t Name[1];
		char Pad3[0x26];
		IPXAddressClass Address;
		char Pad2[0x6f - 0x34];
		int HouseSlot;
};
#pragma pack(pop)

extern int DAT_008316A4;
extern wchar_t DAT_00887734[];
extern int DAT_00A8DA84;
extern MPPlayerNode ** DAT_00A8DA78;
extern DynamicVectorClass<MPPlayerNode *> DAT_00A8DA74;
int __fastcall FUN_005E66D0(void);
int __fastcall FUN_0049D9F0(char const * name);
void __cdecl FUN_004068E0(char const * format, ...);
void __fastcall FUN_005E3720(char const * text);
void __fastcall FUN_005E7BF0(int slot);
int __fastcall FUN_005E6520(int value);
extern int DAT_00A8B254;
extern int DAT_00A8B8D8;
extern MultiMission ** DAT_00A8B8CC;
extern IPXAddressClass DAT_00A8BB1C;
extern int DAT_00A8B3AC;
extern IPXAddressClass DAT_00A8DBCC[8];
extern int DAT_00A8DC2C[8];
extern int DAT_00A8E230[8];
extern wchar_t DAT_00A8E258[];
extern int DAT_00A8E2AC;
extern char DAT_00A8E2B2[16];
struct MPSessionTail
{
	int Array[0x34C / 4];
	int Index;
};
extern MPSessionTail DAT_00A8D23C;
extern int DAT_0088098C;
extern int DAT_00880990;
extern int DAT_00880994;
extern int DAT_00A8B538;
void __fastcall FUN_0049B930(CoopCamp_0049B610 * camp);
struct Game {
	static int TechLevel;
	static int Seed;
};

HWND WS_Find_Dialog(int id);
HWND GameoptWindow(void);
int FUN_00775B20(void);
extern char DAT_0082DC84[];
extern char DAT_0081F1D4[];
extern int DAT_00A8EB68;
extern int DAT_00A83CA8;
extern int DAT_00A8BB0C;
struct MPCountryEntry
{
	char Pad0[0x60];
	wchar_t const * Name;
};
extern MPCountryEntry ** DAT_00A83C9C;
char * __fastcall FUN_007350C0(wchar_t const * text);
extern MPPlayerNode * DAT_00A8DA90[];
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


// FUNCTION: 0x005C1300
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
		CoopCamp_0049B610 * camp = coop->Campaigns[j];
		if (camp) {
			camp->FUN_0049B720();
			operator delete(camp);
		}
	}
	DynamicVectorClass<CoopCamp_0049B610 *> * vec = &coop->Campaigns;
	int max = vec->Length();
	vec->Clear();
	vec->Resize(max, 0);
	int count;
	DAT_0089F518.FUN_0049E4F0(&count);
	for (int i = 0; i < count; ++i) {
		CoopCamp_0049B610 * raw = (CoopCamp_0049B610 *)operator new(sizeof(CoopCamp_0049B610));
		CoopCamp_0049B610 * camp = raw ? raw->FUN_0049B610() : 0;
		camp->FUN_0049CAF0(i);
		vec->Add(camp);
	}
}

// FUNCTION: 0x005C1470
MPCooperative::MPCooperative(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MPGameModeClass(title, tooltip, inifile, mapfilter, false, modeindex)
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
		CoopCamp_0049B610 * camp = Campaigns[j];
		if (camp) {
			camp->FUN_0049B720();
			operator delete(camp);
		}
	}
	DynamicVectorClass<CoopCamp_0049B610 *> * vec = &Campaigns;
	int max = vec->Length();
	vec->Clear();
	vec->Resize(max, 0);
	int count;
	DAT_0089F518.FUN_0049E4F0(&count);
	for (int i = 0; i < count; ++i) {
		CoopCamp_0049B610 * raw = (CoopCamp_0049B610 *)operator new(sizeof(CoopCamp_0049B610));
		CoopCamp_0049B610 * camp = raw ? raw->FUN_0049B610() : 0;
		camp->FUN_0049CAF0(i);
		vec->Add(camp);
	}
}

// FUNCTION: 0x005C1640
MPCooperative::~MPCooperative(void)
{
	CoopCamp_0049B610 * current = Current;
	if (current) {
		current->FUN_0049B720();
		operator delete(current);
		Current = NULL;
	}
	for (int j = Campaigns.Count() - 1; j >= 0; --j) {
		CoopCamp_0049B610 * camp = Campaigns[j];
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

// FUNCTION: 0x005C17B0
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

// FUNCTION: 0x005C18A0
bool MPCooperative::vt_entry_10(void)
{
	Reload_Campaigns(this);
	return(true);
}

// FUNCTION: 0x005C1980
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

// FUNCTION: 0x005C1A10
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
				CoopCamp_0049B610 * old = Current;
				if (old) {
					old->FUN_0049B720();
					operator delete(old);
				}
				Current = NULL;
			}
		}
		if (Current != NULL) {
			if (DAT_00A8DA84 == 2) {
				Current->FUN_0049B760(DAT_00A8DA78[0]->Name, DAT_00A8DA78[1]->Name);
			} else {
				Current->FUN_0049B760(DAT_00887734, DAT_00887734);
			}
		}
	}
	if (Current == NULL) {
		CoopCamp_0049B610 * raw = (CoopCamp_0049B610 *)operator new(sizeof(CoopCamp_0049B610));
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

// FUNCTION: 0x005C1CB0
void MPCooperative::vt_entry_24(void)
{
	Field340 = true;
}

// FUNCTION: 0x005C1CC0
int MPCooperative::FirstValidMapIndex(void)
{
	int count = 0;
	void * name = NULL;
	DAT_0089F518.FUN_0049E4F0(&count);
	if (count != 0) {
		if (Current == NULL) {
			CoopCamp_0049B610 * raw = (CoopCamp_0049B610 *)operator new(sizeof(CoopCamp_0049B610));
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

// FUNCTION: 0x005C1D80
bool MPCooperative::vt_entry_14(unsigned a)
{
	if (DAT_00A8DA84 == 2 && Current != NULL) {
		Current->FUN_0049B760(DAT_00A8DA78[0]->Name, DAT_00A8DA78[1]->Name);
	}
	return(MPGameModeClass::vt_entry_14(a));
}

// FUNCTION: 0x005C1DC0
bool MPCooperative::vt_entry_18(void)
{
	if (Current == NULL) {
		return(false);
	}
	int count;
	DAT_0089F518.FUN_0049E4F0(&count);
	int selected = FUN_005E66D0();
	if (selected >= count) {
		if (DAT_00A8DA84 == 2) {
			Current->FUN_0049B760(DAT_00A8DA78[0]->Name, DAT_00A8DA78[1]->Name);
		}
		if (!Current->FUN_0049C0D0()) {
			return(false);
		}
		Field48 = true;
	} else if (FUN_00775B20() == 0x6b) {
		CoopCamp_0049B610 * old = Current;
		if (old) {
			old->FUN_0049B720();
			operator delete(old);
		}
		Current = Campaigns[selected];
		CoopCamp_0049B610 * raw = (CoopCamp_0049B610 *)operator new(sizeof(CoopCamp_0049B610));
		Campaigns[selected] = raw ? raw->FUN_0049B610() : 0;
		Campaigns[selected]->FUN_0049CAF0(selected);
		Field4C = selected;
		if (DAT_00A8DA84 == 2) {
			Current->FUN_0049B760(DAT_00A8DA78[0]->Name, DAT_00A8DA78[1]->Name);
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
	for (int i = 0; i < DAT_00A8DA84; i++) {
		MPPlayerNode * player = DAT_00A8DA78[i];
		if (!vt_entry_1C(player->FUN_00696F10())) {
			if (group > 1) {
				DAT_00A8DA78[i]->FUN_00696F90(-2);
			} else {
				DAT_00A8DA78[i]->FUN_00696F90(FUN_0049D9F0((*entry->ListA[index])[0]));
			}
		}
	}
	return(true);
}

// FUNCTION: 0x005C1FA0
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

// FUNCTION: 0x005C2040
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

// FUNCTION: 0x005C2100
bool MPCooperative::vt_entry_5C(unsigned a, unsigned b, unsigned c)
{
	if (Current == NULL) {
		return(false);
	}
	int index;
	Current->FUN_0049BCC0(&index);
	CoopPlayerStats * stats = (CoopPlayerStats *)c;
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

// FUNCTION: 0x005C21A0
void MPCooperative::vt_entry_78(unsigned a)
{
	HWND button = GetDlgItem((HWND)a, 0x732);
	if (button) {
		PostMessage(button, 0x405, 1, DAT_00A8EB68);
	}
}

// FUNCTION: 0x005C21D0
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
		PostMessage(button, 0x405, 1, DAT_00A8EB68);
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

// FUNCTION: 0x005C23B0
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

// FUNCTION: 0x005C2500
void MPCooperative::FUN_005C2500(HWND dialog)
{
	if (dialog == NULL) {
		return;
	}
	int count = DAT_00A8BB0C;
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
			DAT_00A8DA90[i]->FUN_00696F90(-2);
		} else {
			int value = FUN_0049D9F0((*entry->ListA[index])[0]);
			FUN_004E3F70(dialog, slot, value);
			DAT_00A8DA90[i]->FUN_00696F90(value);
		}
	}
	FUN_005EBC00(dialog, count);
	HWND options = GameoptWindow();
	if (options) {
		FUN_005C21D0(options, true);
	}
}

// FUNCTION: 0x005C2640
void MPCooperative::vt_entry_38(unsigned a)
{
	HWND dialog = (HWND)a;
	if (dialog) {
		int count = DAT_00A8DA84;
		for (int i = 0; i < count; i++) {
			FUN_005C23B0(dialog, FUN_004E37D0(i));
		}
		FUN_005C21D0(dialog, true);
	}
}

// FUNCTION: 0x005C2680
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

// FUNCTION: 0x005C26D0
void FUN_005C26D0(void)
{
	for (int k = 0; k < 8; k++) {
		DAT_00ABF460[k] = -2;
	}
	int count = DAT_00A8DA84;
	for (int i = 0; i < count; i++) {
		DAT_00ABF460[i] = DAT_00A8DA78[i]->FUN_00696F30();
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

// FUNCTION: 0x005C2710
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
	int count = DAT_00A8DA84;
	for (int k = 0; k < entry->MapCount; k++) {
		for (int m = 0; m < 8; m++) {
			DAT_00ABF460[m] = -2;
		}
		int humans = DAT_00A8DA84;
		for (int p = 0; p < humans; p++) {
			DAT_00ABF460[p] = DAT_00A8DA78[p]->FUN_00696F30();
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

// FUNCTION: 0x005C2A60
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

// FUNCTION: 0x005C2CB0
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

// FUNCTION: 0x005C2CE0
void MPCooperative::PopulateTeamDropdownForPlayer(HWND hwnd, int index)
{
	MPGameModeClass::PopulateTeamDropdownForPlayer(hwnd, index);
}


static inline int Find_Player_Node(int house)
{
	for (int k = 0; k < DAT_00A8DA74.Count(); k++) {
		if (DAT_00A8DA74[k]->HouseSlot == house) {
			return(k);
		}
	}
	return(-1);
}

// FUNCTION: 0x005C2D00
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

// FUNCTION: 0x005C2DE0
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

// FUNCTION: 0x005C2EF0
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
					DAT_00A8DA74[node]->FUN_00696FF0(start);
					DAT_00A8DA74[node]->FUN_00697020(0);
					FUN_004068E0("Playernum %d:Starting position is %d, team is %d\n", node, DAT_00A8DA74[node]->FUN_00696F50(), DAT_00A8DA74[node]->FUN_00696F70());
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

// FUNCTION: 0x005C30C0
bool MPCooperative::vt_entry_68(void)
{
	int aiCount = FUN_005E6520(DAT_00A8B254) - Scen->NumCoopHumanStartSpots;
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
			if (DAT_00A8EB68 == 0) {
				Session.Options.AISlots.Difficulties[slot] = 2;
			} else {
				Session.Options.AISlots.Difficulties[slot] = (DAT_00A8EB68 == 1);
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

// FUNCTION: 0x005C3220
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

// FUNCTION: 0x005C34F0
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

// FUNCTION: 0x005C35F0
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

// FUNCTION: 0x005C36F0
bool MPCooperative::vt_entry_8C(void)
{
	return(MPGameModeClass::vt_entry_8C());
}

// FUNCTION: 0x005C3700
bool MPCooperative::IsAIAllowed(void)
{
	return(false);
}

// FUNCTION: 0x005C3710
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
	int total = DAT_00A8B8D8;
	sprintf(mapname, "%s.MAP", file);
	int index = 0;
	for (; index < total; index++) {
		if (strncmp(mapname, Scenario_Filename(DAT_00A8B8CC[index]), _MAX_PATH) == 0) {
			DAT_00A8B254 = index;
			break;
		}
	}
	FUN_005E7BF0(DAT_00A8B254);
	Scen->IsSkipScore = true;
	Scen->IsEndOfGame = false;
	Scen->IsNoMapSel = true;
	Scen->IsOneTimeOnly = false;
	Field54 = 0;
	for (int k = 0; k < DAT_00A8DA84; k++) {
		if (DAT_00A8BB1C == DAT_00A8DA78[k]->Address) {
			Field54 = k;
		}
	}
	memcpy(Addresses, DAT_00A8DBCC, sizeof(Addresses));
	memcpy(Field124, DAT_00A8DC2C, sizeof(Field124));
	memcpy(Field144, DAT_00A8E230, sizeof(Field144));
	memcpy(Field194, DAT_00A8E2B2, sizeof(Field194));
	wcscpy(Field164, DAT_00A8E258);
	Field190 = DAT_00A8E2AC;
	Field70 = DAT_00A8B3AC;
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
	DAT_00A8B538 = 0;
	Map.SetActiveFoundation(NULL);
	DAT_0088098C = 0;
	DAT_00880990 = 0;
	DAT_00880994 = -1;
	for (int n = 0; n < 0x70; n += 0x1c) {
		DAT_00A8D23C.Array[DAT_00A8D23C.Index + n] = -1;
	}
}

// FUNCTION: 0x005C3EC0
bool MPCooperative::FUN_005C3EC0(void)
{
	CoopCamp_0049B610 * old = Current;
	if (old) {
		old->FUN_0049B720();
		operator delete(old);
	}
	Current = NULL;
	CoopCamp_0049B610 * raw = (CoopCamp_0049B610 *)operator new(sizeof(CoopCamp_0049B610));
	Current = raw ? raw->FUN_0049B610() : 0;
	Current->FUN_0049CAF0(0);
	if (DAT_00A8DA84 == 2) {
		Current->FUN_0049B760(DAT_00A8DA78[0]->Name, DAT_00A8DA78[1]->Name);
	}
	if (0) {
		Current->FUN_0049B720();
		Current->FUN_0049B720();
	}
	return(Current ? true : false);
}
