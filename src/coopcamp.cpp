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

// FUNCTION: 0x0049D120
bool CoopCamp_0049B610::FUN_0049D120(void)
{
	if (!Name[0] || !Map[0]) {
		return false;
	}
	char section[100];
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
