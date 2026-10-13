// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ccfile.h"
#include "ccini.h"
#include "coopcamp.h"
#include "house.h"
#include "houstype.h"
#include <cstring>
#include <cctype>

extern char DAT_0089ED18[];
extern char DAT_0089F118[];
char * __fastcall FUN_007350C0(wchar_t const * text);
void __cdecl FUN_004068E0(char const * format, ...);
extern DynamicVectorClass<HouseClass *> DAT_00A80228;

char __fastcall FUN_0049B430(int nibble);
void __fastcall FUN_0049B7E0(CoopPlayerStats * stats, bool second);

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

// FUNCTION: 0x0049B390 $E23
// FUNCTION: 0x0049B420 $E21
CoopCampSource_0049E4F0 DAT_0089F518;

// FUNCTION: 0x0049B450
int __fastcall FUN_0049B450(char c)
{
	return HexValue(c);
}

// FUNCTION: 0x0049B490
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

// FUNCTION: 0x0049B540
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

// FUNCTION: 0x0049B610
CoopCamp_0049B610 * CoopCamp_0049B610::FUN_0049B610(void)
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

// FUNCTION: 0x0049B660
CoopCamp_0049B610 * CoopCamp_0049B610::FUN_0049B660(wchar_t const * name, wchar_t const * map)
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

// FUNCTION: 0x0049B720
void CoopCamp_0049B610::FUN_0049B720(void)
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

// FUNCTION: 0x0049B760
void CoopCamp_0049B610::FUN_0049B760(wchar_t const * name, wchar_t const * map)
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

// FUNCTION: 0x0049B7E0
void __fastcall FUN_0049B7E0(CoopPlayerStats * stats, bool second)
{
	int count = DAT_00A80228.Count();
	for (int i = 0; i < count; i++) {
		HouseClass * house = DAT_00A80228[i];
		if (house && house->Class->IsMultiplayPassive != true && house->IsHuman) {
			if (!second) {
				stats->Alive = 0;
				wcscpy(stats->Name, house->UIName);
				stats->Lost = 0;
				stats->Kills = 0;
				stats->Built = 0;
				stats->Score = 0;
				stats->Scheme = house->Scheme;
				if (!house->IsDefeated) {
					stats->Alive++;
				}
				int kills = 0;
				for (int k = 0; k < 20; k++) {
					kills += house->UnitsKilled[k];
				}
				for (int m = 0; m < count; m++) {
					kills += house->BuildingsKilled[m];
				}
				stats->Kills = kills;
				stats->Lost = house->UnitsLost + house->BuildingsLost;
				int built = house->ABQuantity.Total();
				built += house->AUQuantity.Total();
				built += house->AIQuantity.Total();
				built += house->AAQuantity.Total();
				stats->Built = house->UnitsLost + house->BuildingsLost + built;
				if (house->PointTotal > 0) {
					stats->Score += house->PointTotal;
				}
				char const * result = "Winner";
				if (stats->Alive <= 0) {
					result = "Loser";
				}
				char * name = FUN_007350C0(stats->Name);
				FUN_004068E0("@@@@ %s: %s\n Scheme: %d\n Lost = %d\n Kills = %d\n Built = %d\n Score = %d\n",
					name, result, stats->Scheme, stats->Lost, stats->Kills, stats->Built, stats->Score);
				return;
			}
			second = false;
		}
	}
}

// FUNCTION: 0x0049BC80
bool CoopCamp_0049B610::FUN_0049BC80(int * out)
{
	if (!Field6C) {
		return false;
	}
	*out = Field44;
	return true;
}

// FUNCTION: 0x0049BCA0
bool CoopCamp_0049B610::FUN_0049BCA0(int * out)
{
	if (!Field6C) {
		return false;
	}
	*out = ItemCount;
	return true;
}

// FUNCTION: 0x0049BCC0
bool CoopCamp_0049B610::FUN_0049BCC0(int * out)
{
	if (!Field6C) {
		return false;
	}
	*out = Field38;
	return true;
}

// FUNCTION: 0x0049BCE0
bool CoopCamp_0049B610::FUN_0049BCE0(DynamicVectorClass<CoopPlayerStats *> * list, int * out)
{
	if (!Field6C) {
		return false;
	}
	while (list->Count()) {
		operator delete((*list)[0]);
		list->Delete_Index(0);
	}
	CoopPlayerStats * a = (CoopPlayerStats *)operator new(0x70);
	FUN_0049B7E0(a, false);
	CoopPlayerStats * b = (CoopPlayerStats *)operator new(0x70);
	FUN_0049B7E0(b, true);
	if (strcmp(Name, FUN_007350C0(a->Name)) != 0) {
		CoopPlayerStats * t = a;
		a = b;
		b = t;
	}
	for (int i = 0; i < 2; i++) {
		CoopPlayerStats * s = (i == 0) ? a : b;
		s->Kills = (&Field48)[i];
		s->Built = (&Field50)[i];
		s->Lost = (&Field58)[i];
		s->Alive = Field38;
		s->Score = (&Field60)[i];
		list->Add(s);
	}
	*out = Field68;
	return true;
}

// FUNCTION: 0x0049BE40
bool CoopCamp_0049B610::FUN_0049BE40(void)
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

// FUNCTION: 0x0049C0D0
bool CoopCamp_0049B610::FUN_0049C0D0(void)
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

// FUNCTION: 0x0049D070
bool CoopCamp_0049B610::FUN_0049D070(void ** out, int index)
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

// FUNCTION: 0x0049D0C0
bool CoopCamp_0049B610::FUN_0049D0C0(int * house, int * color)
{
	if (Field6C) {
		*house = Field14;
		*color = Field18;
		return true;
	}
	return false;
}

// FUNCTION: 0x0049D120
bool CoopCamp_0049B610::FUN_0049D120(void)
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
