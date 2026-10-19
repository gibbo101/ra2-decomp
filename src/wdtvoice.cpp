// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "_mixfile.h"
#include "ccfile.h"
#include "ccrand.h"
#include "data.h"
#include "dsaudio.h"
#include "globals.h"
#include "goptions.h"
#include "ini.h"
#include "mixfile.h"
#include "wdtnet.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F7340;
extern const double DAT_007F7348;

// FUNCTION: 0x00770080 $E18
double DAT_00B72BB0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x007700B0 $E21
double DAT_00B72C00 = DAT_00B72BB0 / DAT_007F7340;

// FUNCTION: 0x007700D0 $E24
double DAT_00B72BE8 = DAT_007E1718 / DAT_00B72C00;

// FUNCTION: 0x007700F0 $E27
double DAT_00B72BE0 = DAT_007E1720 * DAT_007F7348;

// FUNCTION: 0x00770110 $E30
double DAT_00B72BB8 = DAT_007E1728 * DAT_007F7348;

// FUNCTION: 0x00770130 $E33
double DAT_00B72B90 = DAT_007E1730 * DAT_007F7348;

// FUNCTION: 0x00770150 $E36
int DAT_00B72C08 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B72B90 - DAT_00B72BB8) * DAT_00B72BB0));

// FUNCTION: 0x00770190 $E39
double DAT_00B72BC0 = FUN_004CADE0(DAT_00B72C08 * DAT_007E1740);

// FUNCTION: 0x007701C0 $E42
double DAT_00B72BD8 = FUN_004CADE0((DAT_00B72C08 * 2) / DAT_00B72BB0);

// FUNCTION: 0x007701F0 $E45
int DAT_00B72BFC = (int)(DAT_00B72C08 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00770220 $E48
ShortPair DAT_00B72BA8;

// FUNCTION: 0x00770230 $E51
IntTriple DAT_00B72BC8;

using namespace WorldDominationTour;

Wstring operator+(Wstring const & a, Wstring const & b);
extern void __fastcall FUN_007529A0(bool stop);
extern void * FUN_00407010(void);

void __fastcall Parse_Index_List(Wstring const & text, VOICEINDEX_LIST & list);
bool __fastcall WDT_Make_Sound_Filename(int side, int index, Wstring & out);
bool __fastcall WDT_Random_Pick_Sound_Filename(int side, VOICEINDEX_LIST & list, Wstring & out);

struct IntPairTail {
	int A, B, C;
	IntPairTail() { C = 0; A = 0x80; B = 0x80; }
};

// FUNCTION: 0x00770250 $E54
IntPairTail DAT_00B72BF0;

// FUNCTION: 0x00770270 WorldDominationTour::YRVoices::YRVoices
YRVoices::YRVoices(int side) :
	Side(side),
	IsIdle(true),
	QueuedSampleCount(0),
	Stream(NULL)
{
	FUN_00407010();
	{ Wstring name("OldCycle"); Categories[0].Set_Name(name); }
	{ Wstring name("History"); Categories[1].Set_Name(name); }
	{ Wstring name("Status"); Categories[2].Set_Name(name); }
	{ Wstring name("Territory"); Categories[3].Set_Name(name); }
	{ Wstring name("Game"); Categories[4].Set_Name(name); }
	{ Wstring name("Evacuate"); Categories[5].Set_Name(name); }
}

// FUNCTION: 0x00770430 WorldDominationTour::YRVoices::~YRVoices
YRVoices::~YRVoices(void)
{
	if (QueuedSamples.Count() > 0) {
		FUN_007529A0(true);
		QueuedSamples.Clear();
	}
	Timer = 0;
	if (Stream != NULL) {
		Stream->FUN_00407A90();
	}
}

// FUNCTION: 0x00770550 WorldDominationTour::YRVoices::FUN_00770550
void YRVoices::FUN_00770550(INIClass const & ini, Wstring const & section)
{
	for (int i = 0; i < 6; i++) {
		int side = Side;
		{ Wstring name("Win"); Categories[i].FUN_007710B0(ini, section, side, OUTCOME_WIN, name); }
		{ Wstring name("Lose"); Categories[i].FUN_007710B0(ini, section, side, OUTCOME_LOSE, name); }
		{ Wstring name("Draw"); Categories[i].FUN_007710B0(ini, section, side, OUTCOME_DRAW, name); }
	}
	{ Wstring name("Startup"); Init_Standalone(ini, section, 0, name); }
	{ Wstring name("TerritorySelect"); Init_Standalone(ini, section, 1, name); }
}

// FUNCTION: 0x00770850 WorldDominationTour::YRVoices::Init_Standalone
void YRVoices::Init_Standalone(INIClass const & ini, Wstring const & section, int voice, Wstring const & name)
{
	Wstring entry = operator+(*FUN_007AF2A0(Side), name);
	Wstring result;
	{
		Wstring none("");
		ini.Get_String(section, entry, none, result);
	}
	Parse_Index_List(result, StandaloneVoices[voice]);
}

// FUNCTION: 0x00770970 WorldDominationTour::YRVoices::Pick_Standard_Voice
bool YRVoices::Pick_Standard_Voice(int category, int outcome, Wstring & out)
{
	int side = Side;
	return(WDT_Random_Pick_Sound_Filename(side, Categories[category].VoiceIndexes[outcome][0], out));
}

// FUNCTION: 0x00770A50 WorldDominationTour::YRVoices::Pick_Emphasis_Voice
bool YRVoices::Pick_Emphasis_Voice(int category, int outcome, Wstring & out)
{
	int side = Side;
	return(WDT_Random_Pick_Sound_Filename(side, Categories[category].VoiceIndexes[outcome][1], out));
}

// FUNCTION: 0x00770B30 WorldDominationTour::YRVoices::Pick_Standalone_Voice
bool YRVoices::Pick_Standalone_Voice(int voice, Wstring & out)
{
	return(WDT_Random_Pick_Sound_Filename(Side, StandaloneVoices[voice], out));
}

// FUNCTION: 0x00770C10
bool __fastcall WDT_Random_Pick_Sound_Filename(int side, VOICEINDEX_LIST & list, Wstring & out)
{
	out.clear();
	int count = list.Count();
	if (count <= 0) {
		return(false);
	}
	return(WDT_Make_Sound_Filename(side, list[count == 1 ? 0 : NonCriticalRandomNumber(0, count - 1)], out));
}

// FUNCTION: 0x00770CE0
bool __fastcall WDT_Make_Sound_Filename(int side, int index, Wstring & out)
{
	if (side == 3) {
		out = "WWD-Soviet" + Wstring_FromInt(index, 2);
	} else {
		out = "WWD_Allied" + Wstring_FromInt(index, 2);
	}
	return(true);
}

// FUNCTION: 0x00770D70
void __fastcall Parse_Index_List(Wstring const & text, VOICEINDEX_LIST & list)
{
	list.Clear();
	Wstring token;
	int pos = text.getToken(0, ",", token);
	while (pos != -1) {
		list.Add(token.toInt());
		pos = text.getToken(pos, ",", token);
	}
}

// FUNCTION: 0x00770E10 WorldDominationTour::YRVoices::Category::Category
YRVoices::Category::Category(void)
{
	Name = "";
}
