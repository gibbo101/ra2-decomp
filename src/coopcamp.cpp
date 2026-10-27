// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "ccfile.h"
#include "ccini.h"
#include "coopcamp.h"
#include "house.h"
#include "houstype.h"
#include "mplayerscore.h"
#include <cstring>
#include <cctype>
#include <math.h>

extern char DAT_0089ED18[];
extern char DAT_0089F118[];
char * __fastcall FUN_007350C0(wchar_t const * text);
void __cdecl FUN_004068E0(char const * format, ...);
void __fastcall FUN_005E3720(char const * text);
extern DynamicVectorClass<HouseClass *> DAT_00A80228;

char __fastcall FUN_0049B430(int nibble);
void __fastcall FUN_0049B7E0(MPlayerScoreType * stats, bool second);

static inline char HexDigit(int nibble)
{
	if (nibble < 0 || nibble > 0xf) {
		return 0;
	}
	if (nibble < 10) {
		return (char)('0' + nibble);
	}
	return (char)('7' + nibble);
}

static inline int HexValue(char c)
{
	if (c <= '9' && c >= '0') {
		return c - '0';
	}
	if (c <= 'f' && c >= 'a') {
		return c - 'a' + 10;
	}
	if (c <= 'F' && c >= 'A') {
		return c - 'A' + 10;
	}
	return 0;
}

// SYNTHETIC: GAMEMD 0x0049B390 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0049B420 SYMBOL
// _$E21
CoopCampSource_0049E4F0 DAT_0089F518;

// FUNCTION: GAMEMD 0x0049B450
int __fastcall FUN_0049B450(char c)
{
	return HexValue(c);
}

// FUNCTION: GAMEMD 0x0049B490
char * __fastcall FUN_0049B490(char const * text)
{
	char const * p = text;
	int i = 0;
	for (; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = HexDigit(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	return DAT_0089ED18;
}

// FUNCTION: GAMEMD 0x0049B540
char * __fastcall FUN_0049B540(char const * text)
{
	char * out = DAT_0089F118;
	for (; *text; text++, out++) {
		if (*text == '=') {
			if (!text[1]) {
				break;
			}
			int v = HexValue(text[1]);
			text++;
			*out = (char)v;
			if (text[1]) {
				*out = (char)(*out << 4);
				*out |= HexValue(text[1]);
				text++;
			}
		} else {
			*out = *text;
		}
	}
	*out = 0;
	return DAT_0089F118;
}

// FUNCTION: GAMEMD 0x0049B610
CoopCampaignClass * CoopCampaignClass::FUN_0049B610(void)
{
	Name[0] = 0;
	Map[0] = 0;
	Field14 = -2;
	Field30 = -2;
	Field18 = -2;
	Field34 = -2;
	Items = 0;
	Field44 = -1;
	Field6C = false;
	ItemCount = 0;
	Field38 = -1;
	Field48 = 0;
	Field4C = 0;
	Field50 = 0;
	Field54 = 0;
	Field58 = 0;
	Field5C = 0;
	Field68 = 0;
	Field60 = 0;
	Field64 = 0;
	return this;
}

// FUNCTION: GAMEMD 0x0049B660
CoopCampaignClass * CoopCampaignClass::FUN_0049B660(wchar_t const * name, wchar_t const * map)
{
	if (name) {
		strcpy(Name, FUN_007350C0(name));
	} else {
		Name[0] = 0;
	}
	if (map) {
		strcpy(Map, FUN_007350C0(map));
	} else {
		Map[0] = 0;
	}
	Field14 = -2;
	Field30 = -2;
	Field18 = -2;
	Field34 = -2;
	Items = 0;
	Field44 = -1;
	Field38 = -1;
	Field6C = false;
	ItemCount = 0;
	Field48 = 0;
	Field4C = 0;
	Field50 = 0;
	Field54 = 0;
	Field58 = 0;
	Field5C = 0;
	Field68 = 0;
	Field60 = 0;
	Field64 = 0;
	return this;
}

// FUNCTION: GAMEMD 0x0049B720
void CoopCampaignClass::FUN_0049B720(void)
{
	if (Items) {
		for (int i = 0; i < ItemCount; i++) {
			if (Items[i]) {
				operator delete(Items[i]);
			}
		}
		operator delete(Items);
	}
}

// FUNCTION: GAMEMD 0x0049B760
void CoopCampaignClass::FUN_0049B760(wchar_t const * name, wchar_t const * map)
{
	if (name) {
		strcpy(Name, FUN_007350C0(name));
	} else {
		Name[0] = 0;
	}
	if (map) {
		strcpy(Map, FUN_007350C0(map));
	} else {
		Map[0] = 0;
	}
}

// FUNCTION: GAMEMD 0x0049B7E0
void __fastcall FUN_0049B7E0(MPlayerScoreType * stats, bool second)
{
	int count = DAT_00A80228.Count();
	for (int i = 0; i < count; i++) {
		HouseClass * house = DAT_00A80228[i];
		if (house && house->Class->IsMultiplayPassive != true && house->IsHuman) {
			if (!second) {
				stats->Wins = 0;
				wcscpy(stats->Name, house->UIName);
				stats->Lost[0] = 0;
				stats->Kills[0] = 0;
				stats->Built[0] = 0;
				stats->Score[0] = 0;
				stats->Scheme = house->Scheme;
				if (!house->IsDefeated) {
					stats->Wins++;
				}
				int kills = 0;
				for (int k = 0; k < 20; k++) {
					kills += house->UnitsKilled[k];
				}
				for (int m = 0; m < count; m++) {
					kills += house->BuildingsKilled[m];
				}
				stats->Kills[0] = kills;
				stats->Lost[0] = house->UnitsLost + house->BuildingsLost;
				int built = house->ABQuantity.Total();
				built += house->AUQuantity.Total();
				built += house->AIQuantity.Total();
				built += house->AAQuantity.Total();
				stats->Built[0] = house->UnitsLost + house->BuildingsLost + built;
				if (house->PointTotal > 0) {
					stats->Score[0] += house->PointTotal;
				}
				char const * result = "Winner";
				if (stats->Wins <= 0) {
					result = "Loser";
				}
				char * name = FUN_007350C0(stats->Name);
				FUN_004068E0("@@@@ %s: %s\n Scheme: %d\n Lost = %d\n Kills = %d\n Built = %d\n Score = %d\n",
					name, result, stats->Scheme, stats->Lost[0], stats->Kills[0], stats->Built[0], stats->Score[0]);
				return;
			}
			second = false;
		}
	}
}

// FUNCTION: GAMEMD 0x0049BC80
bool CoopCampaignClass::FUN_0049BC80(int * out)
{
	if (!Field6C) {
		return false;
	}
	*out = Field44;
	return true;
}

// FUNCTION: GAMEMD 0x0049BCA0
bool CoopCampaignClass::FUN_0049BCA0(int * out)
{
	if (!Field6C) {
		return false;
	}
	*out = ItemCount;
	return true;
}

// FUNCTION: GAMEMD 0x0049BCC0
bool CoopCampaignClass::FUN_0049BCC0(int * out)
{
	if (!Field6C) {
		return false;
	}
	*out = Field38;
	return true;
}

// FUNCTION: GAMEMD 0x0049BCE0
bool CoopCampaignClass::FUN_0049BCE0(DynamicVectorClass<MPlayerScoreType *> * list, int * out)
{
	if (!Field6C) {
		return false;
	}
	while (list->Count()) {
		operator delete((*list)[0]);
		list->Delete_Index(0);
	}
	MPlayerScoreType * a = (MPlayerScoreType *)operator new(0x70);
	FUN_0049B7E0(a, false);
	MPlayerScoreType * b = (MPlayerScoreType *)operator new(0x70);
	FUN_0049B7E0(b, true);
	if (strcmp(Name, FUN_007350C0(a->Name)) != 0) {
		MPlayerScoreType * t = a;
		a = b;
		b = t;
	}
	for (int i = 0; i < 2; i++) {
		MPlayerScoreType * s = (i == 0) ? a : b;
		s->Kills[0] = (&Field48)[i];
		s->Built[0] = (&Field50)[i];
		s->Lost[0] = (&Field58)[i];
		s->Wins = Field38;
		s->Score[0] = (&Field60)[i];
		list->Add(s);
	}
	*out = Field68;
	return true;
}

// FUNCTION: GAMEMD 0x0049BE40
bool CoopCampaignClass::FUN_0049BE40(void)
{
	if (!Name[0] || !Map[0]) {
		return true;
	}
	char section[100];
	CCFileClass file("coopsave.ini");
	struct Holder {
		CCINIClass ini;
	} h;
	CCINIClass & ini = h.ini;
	ini.ReadCCFile(file, false, false);
	file.Close();

	int i = 0;
	char const * p;
	for (p = Name; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = FUN_0049B430(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	strcpy(section, DAT_0089ED18);
	i = 0;
	for (p = Map; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = FUN_0049B430(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	strcat(section, DAT_0089ED18);

	CCFileClass out;
	FileClass * pf = &out;
	pf->Set_Name("coopsave.ini");
	pf->Open(2);
	ini.Clear(section, NULL);
	ini.Save(out, false);
	out.Close();
	return true;
}

// FUNCTION: GAMEMD 0x0049C0D0
bool CoopCampaignClass::FUN_0049C0D0(void)
{
	if (!Name[0] || !Map[0]) {
		return false;
	}
	int i = 0;
	char const * p;
	for (p = Name; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = HexDigit(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	char section[100];
	char key[32];
	int campaign;
	strcpy(section, DAT_0089ED18);
	i = 0;
	for (p = Map; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = HexDigit(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	strcat(section, DAT_0089ED18);

	CCFileClass file("coopsave.ini");
	struct Holder {
		CCINIClass ini;
	} h;
	CCINIClass & ini = h.ini;
	if (!ini.ReadCCFile(file, false, false)) {
		return false;
	}
	Field38 = ini.Get_Int(section, "CurrentMap", 0);
	ini.Get_Int(section, "Score", 0);
	Field44 = ini.Get_Int(section, "CampaignType", -1);
	Field14 = ini.Get_HousesType(section, "House1", (HousesType)-2);
	Field18 = ini.Get_Int(section, "Color1", -2);
	Field30 = ini.Get_HousesType(section, "House2", (HousesType)-2);
	Field34 = ini.Get_Int(section, "Color2", -2);
	Field48 = ini.Get_Int(section, "Kills1", 0);
	Field4C = ini.Get_Int(section, "Kills2", 0);
	Field50 = ini.Get_Int(section, "Built1", 0);
	Field54 = ini.Get_Int(section, "Built2", 0);
	Field58 = ini.Get_Int(section, "Lost1", 0);
	Field5C = ini.Get_Int(section, "Lost2", 0);
	Field60 = ini.Get_Int(section, "Score1", 0);
	Field64 = ini.Get_Int(section, "Score2", 0);
	Field68 = ini.Get_Int(section, "Time", 0);
	campaign = Field44;
	if (!DAT_0089F518.Loaded) {
		DAT_0089F518.FUN_0049DB00();
	}
	if (campaign < 0 || campaign >= DAT_0089F518.EntryCount) {
		ItemCount = 0;
		return false;
	}
	{
	CoopCampEntry * entry = &DAT_0089F518.Entries[campaign];
	ItemCount = entry->MapCount;
	Items = (void **)operator new(entry->MapCount << 2);
	for (int n = 0; n < ItemCount; n++) {
		sprintf(key, "Map%d", n + 1);
		Items[n] = operator new(0x104);
		ini.Get_String(section, key, "undefined", (char *)Items[n], 0x104);
	}
	Field6C = true;
	return true;
	}
}

// FUNCTION: GAMEMD 0x0049D070
bool CoopCampaignClass::FUN_0049D070(void ** out, int index)
{
	bool result = false;
	*out = 0;
	if (index == -1) {
		index = Field38;
	}
	if (index < 0 || index >= ItemCount) {
		index = -1;
	}
	if (Field6C) {
		if (index != -1) {
			*out = Items[index];
		}
		if (*out) {
			result = true;
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x0049D0C0
bool CoopCampaignClass::FUN_0049D0C0(int * house, int * color)
{
	if (Field6C) {
		*house = Field14;
		*color = Field18;
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x0049D0F0
bool CoopCampaignClass::FUN_0049D0F0(int * house, int * color)
{
	if (Field6C) {
		*house = Field30;
		*color = Field34;
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x0049D120
bool CoopCampaignClass::FUN_0049D120(void)
{
	if (!Name[0] || !Map[0]) {
		return false;
	}
	char section[100];
	char key[32];
	struct Loaded {
		CCFileClass file;
		CCINIClass ini;
		Loaded(void) : file("coopsave.ini") {}
	} l;
	CCFileClass & file = l.file;
	CCINIClass & ini = l.ini;
	if (!ini.ReadCCFile(file, false, false)) {
		l.file.Close();
		return false;
	}
	l.file.Close();

	int i = 0;
	char const * p;
	for (p = Name; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = FUN_0049B430(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	strcpy(section, DAT_0089ED18);
	i = 0;
	for (p = Map; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = FUN_0049B430(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	strcat(section, DAT_0089ED18);
	return ini.GetSection(section) != NULL;
}

// FUNCTION: GAMEMD 0x0049D390
bool CoopCampaignClass::FUN_0049D390(char * out)
{
	if (!Name[0] || !Map[0]) {
		return false;
	}
	char section[100];
	char key[32];
	struct Loaded {
		CCFileClass file;
		CCINIClass ini;
		Loaded(void) : file("coopsave.ini") {}
	} l;
	CCFileClass & file = l.file;
	CCINIClass & ini = l.ini;
	if (!ini.ReadCCFile(file, false, false)) {
		l.file.Close();
	} else {
		l.file.Close();

		int i = 0;
		char const * p;
		for (p = Name; *p && i < 0x3fd; i++, p++) {
			if (!isalnum(*p)) {
				DAT_0089ED18[i++] = '=';
				int hi = *p >> 4;
				DAT_0089ED18[i++] = HexDigit(hi);
				DAT_0089ED18[i] = FUN_0049B430(*p & 0xf);
			} else {
				DAT_0089ED18[i] = *p;
			}
		}
		DAT_0089ED18[i] = 0;
		strcpy(section, DAT_0089ED18);
		i = 0;
		for (p = Map; *p && i < 0x3fd; i++, p++) {
			if (!isalnum(*p)) {
				DAT_0089ED18[i++] = '=';
				int hi = *p >> 4;
				DAT_0089ED18[i++] = HexDigit(hi);
				DAT_0089ED18[i] = FUN_0049B430(*p & 0xf);
			} else {
				DAT_0089ED18[i] = *p;
			}
		}
		DAT_0089ED18[i] = 0;
		strcat(section, DAT_0089ED18);
		int current = ini.Get_Int(section, "CurrentMap", 0);
		sprintf(key, "Map%d", current + 1);
		ini.Get_String(section, key, "undefined", out, 0x104);
		if (strcmp(out, "undefined") == 0) {
			return true;
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x0049D680
bool CoopCampaignClass::FUN_0049D680(void)
{
	if (!Field6C) {
		return false;
	}
	CoopCampEntry * entry;
	if (!DAT_0089F518.FUN_0049E3B0(Field44, &entry)) {
		return false;
	}
	char line[0x190];
	char pick[16];
	sprintf(line, "C0,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", Field44, Field38, Field68, Field48, Field50, Field58, Field60, Field4C, Field54, Field5C, Field64);
	for (int k = 0; k < ItemCount; k++) {
		int found = 0;
		for (int j = 0; j < 3; j++) {
			if (!strcmpi(&entry->Names[k * 0x30c + j * 0x104], (char const *)Items[k])) {
				found = j;
				break;
			}
		}
		sprintf(pick, ",%d", found);
		strcat(line, pick);
	}
	strcat(line, ",-1,");
	char const * p;
	int i = 0;
	for (p = Name; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = HexDigit(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	strcat(line, DAT_0089ED18);
	strcat(line, ",");
	i = 0;
	for (p = Map; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = HexDigit(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	strcat(line, DAT_0089ED18);
	FUN_004068E0("@@@@ Sending campaign game option string: %s\n", line);
	FUN_005E3720(line);
	FUN_004068E0("@@@@ Sent\n");
	return true;
}

// FUNCTION: GAMEMD 0x0049D9F0
int __fastcall FUN_0049D9F0(char const * name)
{
	if (strcmpi(name, "<random>") == 0) {
		return -2;
	}
	if (name) {
		for (int i = 0; i < HouseTypes.Count(); i++) {
			HouseTypeClass * type = HouseTypes[i];
			if (strcmpi(type->Full_Name(), name) == 0 || strcmpi(type->Name(), name) == 0) {
				return i;
			}
		}
	}
	return -1;
}

// FUNCTION: GAMEMD 0x0049DA60
CoopCampSource_0049E4F0::CoopCampSource_0049E4F0(void)
{
	Loaded = false;
}

// FUNCTION: GAMEMD 0x0049E3B0
bool CoopCampSource_0049E4F0::FUN_0049E3B0(int index, CoopCampEntry ** out)
{
	if (!Loaded) {
		FUN_0049DB00();
	}
	if (index >= 0 && index < EntryCount) {
		*out = &Entries[index];
		return true;
	}
	*out = 0;
	return false;
}

// FUNCTION: GAMEMD 0x0049E400
bool CoopCampSource_0049E4F0::FUN_0049E400(char const * name, CoopCampEntry ** out)
{
	if (!Loaded) {
		FUN_0049DB00();
	}
	for (int i = 0; i < EntryCount; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < Entries[i].MapCount; k++) {
				if (strnicmp(name, &Entries[i].Names[k * 0x30c + j * 0x104], strlen(&Entries[i].Names[k * 0x30c + j * 0x104])) == 0) {
					*out = &Entries[i];
					return true;
				}
			}
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x0049E4F0
bool CoopCampSource_0049E4F0::FUN_0049E4F0(int * count)
{
	if (!Loaded) {
		FUN_0049DB00();
	}
	*count = EntryCount;
	return Loaded;
}

// FUNCTION: GAMEMD 0x0049E510
CoopCampSource_0049E4F0::~CoopCampSource_0049E4F0(void)
{
	if (Entries) {
		for (int i = 0; i < EntryCount; i++) {
			if (Entries[i].Names) {
				delete Entries[i].Title;
				delete Entries[i].Extra;
				delete Entries[i].Names;
			}
			for (int a = 0; a < Entries[i].ListB.Count(); a++) {
				for (int b = 0; b < Entries[i].ListB[a]->Count(); b++) {
					delete (*Entries[i].ListB[a])[b];
				}
				Entries[i].ListB[a]->Clear();
				delete Entries[i].ListB[a];
			}
			Entries[i].ListB.Clear();
			for (int c = 0; c < Entries[i].ListA.Count(); c++) {
				for (int d = 0; d < Entries[i].ListA[c]->Count(); d++) {
					delete (*Entries[i].ListA[c])[d];
				}
				Entries[i].ListA[c]->Clear();
				delete Entries[i].ListA[c];
			}
			Entries[i].ListA.Clear();
		}
		delete[] Entries;
		Entries = 0;
		Loaded = false;
	}
}

// FUNCTION: GAMEMD 0x0049E740
bool CoopCampSource_0049E4F0::FUN_0049E740(DynamicVectorClass<wchar_t *> * list)
{
	if (!Loaded) {
		FUN_0049DB00();
	}
	if (Loaded) {
		list->Resize(EntryCount, 0);
		for (int i = 0; i < EntryCount; i++) {
			(*list)[i] = new wchar_t[0x81];
			if ((*list)[i]) {
				wcscpy((*list)[i], Entries[i].Title);
			}
		}
		return true;
	}
	return false;
}

// TEMPLATE: GAMEMD 0x0049E8E0
// ??0?$List@PAUINISection@INIClass@@@@QAE@XZ

template class DynamicVectorClass<char *>;
template class DynamicVectorClass<CoopCampNameList *>;
template class VectorClass<CoopCampNameList *>;

// TEMPLATE: GAMEMD 0x0049E950
// ?Clear@?$DynamicVectorClass@PAD@@UAEXXZ
// TEMPLATE: GAMEMD 0x0049E980
// ?ID@?$DynamicVectorClass@PAD@@UAEHPBQAD@Z
// TEMPLATE: GAMEMD 0x0049E9A0
// ?GetItem@?$VectorClass@PAD@@UBEPADH@Z
// TEMPLATE: GAMEMD 0x0049E9B0
// ?Clear@?$DynamicVectorClass@PAV?$DynamicVectorClass@PAD@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0049E9E0
// ?ID@?$DynamicVectorClass@PAV?$DynamicVectorClass@PAD@@@@UAEHPBQAV?$DynamicVectorClass@PAD@@@Z
// TEMPLATE: GAMEMD 0x0049EA00
// ?GetItem@?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@UBEPAV?$DynamicVectorClass@PAD@@H@Z
// TEMPLATE: GAMEMD 0x0049EA10
// ??0?$IndexClass@HPAUINISection@INIClass@@@@QAE@XZ
// TEMPLATE: GAMEMD 0x0049EA30
// ??8?$VectorClass@PAD@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0049EA70
// ?Resize@?$DynamicVectorClass@PAD@@UAE_NHPBQAD@Z
// TEMPLATE: GAMEMD 0x0049EB20
// ?ID@?$DynamicVectorClass@PAD@@UAEHABQAD@Z
// TEMPLATE: GAMEMD 0x0049EB50
// ??8?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0049EB90
// ?Resize@?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@UAE_NHPBQAV?$DynamicVectorClass@PAD@@@Z
// TEMPLATE: GAMEMD 0x0049EC40
// ?Clear@?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0049EC70
// ?ID@?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@UAEHABQAV?$DynamicVectorClass@PAD@@@Z
// TEMPLATE: GAMEMD 0x0049ECB0
// ?ID@?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@UAEHPBQAV?$DynamicVectorClass@PAD@@@Z
// TEMPLATE: GAMEMD 0x0049ECD0
// ??0?$DynamicVectorClass@PAV?$DynamicVectorClass@PAD@@@@QAE@IPBQAV?$DynamicVectorClass@PAD@@@Z
// TEMPLATE: GAMEMD 0x0049ED30
// ?Resize@?$DynamicVectorClass@PAV?$DynamicVectorClass@PAD@@@@UAE_NHPBQAV?$DynamicVectorClass@PAD@@@Z
// TEMPLATE: GAMEMD 0x0049EDE0
// ?ID@?$DynamicVectorClass@PAV?$DynamicVectorClass@PAD@@@@UAEHABQAV?$DynamicVectorClass@PAD@@@Z
// SYNTHETIC: GAMEMD 0x0049EE10
// ??_G?$DynamicVectorClass@PAD@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0049EE60
// ?Resize@?$VectorClass@PAD@@UAE_NHPBQAD@Z
// TEMPLATE: GAMEMD 0x0049EF10
// ?Clear@?$VectorClass@PAD@@UAEXXZ
// TEMPLATE: GAMEMD 0x0049EF40
// ?ID@?$VectorClass@PAD@@UAEHABQAD@Z
// TEMPLATE: GAMEMD 0x0049EF80
// ?ID@?$VectorClass@PAD@@UAEHPBQAD@Z
// SYNTHETIC: GAMEMD 0x0049EFA0
// ??_G?$VectorClass@PAD@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0049EFF0
// ??_G?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0049F040
// ??_G?$DynamicVectorClass@PAV?$DynamicVectorClass@PAD@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0049F090
// ??0?$VectorClass@PAV?$DynamicVectorClass@PAD@@@@QAE@HPBQAV?$DynamicVectorClass@PAD@@@Z


#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0049F0E0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0049F110 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0049F130 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0049F150 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0049F170 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0049F190 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x0049F1B0 SYMBOL
// _$E44
// SYNTHETIC: GAMEMD 0x0049F1F0 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x0049F220 SYMBOL
// _$E50
// SYNTHETIC: GAMEMD 0x0049F250 SYMBOL
// _$E53
// SYNTHETIC: GAMEMD 0x0049F280 SYMBOL
// _$E56
// SYNTHETIC: GAMEMD 0x0049F290 SYMBOL
// _$E59
// SYNTHETIC: GAMEMD 0x0049F2B0 SYMBOL
// _$E62

// SYNTHETIC: GAMEMD 0x0049F2D0 SYMBOL
// _$E65
Rect DAT_0089F6A8(0, 0, 0, 0);
