// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ccini.h"
#include "vector.h"
#include "wstring.h"

struct MPlayerScoreType;

class CoopCampaignClass
{
	public:
		CoopCampaignClass * FUN_0049B610(void);
		CoopCampaignClass * FUN_0049B660(wchar_t const * name, wchar_t const * map);
		void FUN_0049B720(void);
		void FUN_0049B760(wchar_t const * name, wchar_t const * map);
		bool FUN_0049BC80(int * out);
		bool FUN_0049BCA0(int * out);
		bool FUN_0049BCC0(int * out);
		bool FUN_0049BCE0(DynamicVectorClass<MPlayerScoreType *> * list, int * out);
		bool FUN_0049BE40(void);
		bool FUN_0049C0D0(void);
		bool FUN_0049C5A0(void);
		bool FUN_0049CC50(int index, int time, MPlayerScoreType first, MPlayerScoreType second);
		bool FUN_0049CCF0(int index, DynamicVectorClass<int> picks, char const * map, char const * name);
		bool FUN_0049D070(void ** out, int index);
		bool FUN_0049D0C0(int * house, int * color);
		bool FUN_0049D0F0(int * house, int * color);
		bool FUN_0049CAF0(int index);
		bool FUN_0049D120(void);
		bool FUN_0049D390(char * out);
		bool FUN_0049D680(void);

		char Name[0x14];
		int Field14;
		int Field18;
		char Map[0x14];
		int Field30;
		int Field34;
		int Field38;
		void ** Items;
		int ItemCount;
		int Field44;
		int Field48;
		int Field4C;
		int Field50;
		int Field54;
		int Field58;
		int Field5C;
		int Field60;
		int Field64;
		int Field68;
		bool Field6C;
};

typedef DynamicVectorClass<char *> CoopCampNameList;

struct CoopCampEntry
{
	int Pad0;
	wchar_t * Title;
	char MapCount;
	char Pad9[3];
	char * Names;
	DynamicVectorClass<CoopCampNameList *> ListA;
	DynamicVectorClass<CoopCampNameList *> ListB;
	wchar_t * Extra;
	Wstring Text44;
	Wstring Text48;
};

class CoopCampSource_0049E4F0
{
	public:
		CCINIClass Ini;
		CoopCampSource_0049E4F0(void);
		~CoopCampSource_0049E4F0(void);
		bool FUN_0049E3B0(int index, CoopCampEntry ** out);
		bool FUN_0049E400(char const * name, CoopCampEntry ** out);
		bool FUN_0049E4F0(int * count);
		bool FUN_0049E740(DynamicVectorClass<wchar_t *> * list);
		void FUN_0049DB00(void);

		CoopCampEntry * Entries;
		int EntryCount;
		bool Loaded;
};

extern CoopCampSource_0049E4F0 DAT_0089F518;
