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

// SYNTHETIC: GAMEMD 0x00770080 SYMBOL
// _$E18
double DAT_00B72BB0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x007700B0 SYMBOL
// _$E21
double DAT_00B72C00 = DAT_00B72BB0 / DAT_007F7340;

// SYNTHETIC: GAMEMD 0x007700D0 SYMBOL
// _$E24
double DAT_00B72BE8 = DAT_007E1718 / DAT_00B72C00;

// SYNTHETIC: GAMEMD 0x007700F0 SYMBOL
// _$E27
double DAT_00B72BE0 = DAT_007E1720 * DAT_007F7348;

// SYNTHETIC: GAMEMD 0x00770110 SYMBOL
// _$E30
double DAT_00B72BB8 = DAT_007E1728 * DAT_007F7348;

// SYNTHETIC: GAMEMD 0x00770130 SYMBOL
// _$E33
double DAT_00B72B90 = DAT_007E1730 * DAT_007F7348;

// SYNTHETIC: GAMEMD 0x00770150 SYMBOL
// _$E36
int DAT_00B72C08 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B72B90 - DAT_00B72BB8) * DAT_00B72BB0));

// SYNTHETIC: GAMEMD 0x00770190 SYMBOL
// _$E39
double DAT_00B72BC0 = FUN_004CADE0(DAT_00B72C08 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x007701C0 SYMBOL
// _$E42
double DAT_00B72BD8 = FUN_004CADE0((DAT_00B72C08 * 2) / DAT_00B72BB0);

// SYNTHETIC: GAMEMD 0x007701F0 SYMBOL
// _$E45
int DAT_00B72BFC = (int)(DAT_00B72C08 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x00770220 SYMBOL
// _$E48
ShortPair DAT_00B72BA8;

// SYNTHETIC: GAMEMD 0x00770230 SYMBOL
// _$E51
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

// SYNTHETIC: GAMEMD 0x00770250 SYMBOL
// _$E54
IntPairTail DAT_00B72BF0;

// FUNCTION: GAMEMD 0x00770270
// WorldDominationTour::YRVoices::YRVoices
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

// FUNCTION: GAMEMD 0x00770430
// WorldDominationTour::YRVoices::~YRVoices
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

// FUNCTION: GAMEMD 0x00770550
// WorldDominationTour::YRVoices::FUN_00770550
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

// FUNCTION: GAMEMD 0x00770850
// WorldDominationTour::YRVoices::Init_Standalone
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

// FUNCTION: GAMEMD 0x00770970
// WorldDominationTour::YRVoices::Pick_Standard_Voice
bool YRVoices::Pick_Standard_Voice(int category, int outcome, Wstring & out)
{
	int side = Side;
	return(WDT_Random_Pick_Sound_Filename(side, Categories[category].VoiceIndexes[outcome][0], out));
}

// FUNCTION: GAMEMD 0x00770A50
// WorldDominationTour::YRVoices::Pick_Emphasis_Voice
bool YRVoices::Pick_Emphasis_Voice(int category, int outcome, Wstring & out)
{
	int side = Side;
	return(WDT_Random_Pick_Sound_Filename(side, Categories[category].VoiceIndexes[outcome][1], out));
}

// FUNCTION: GAMEMD 0x00770B30
// WorldDominationTour::YRVoices::Pick_Standalone_Voice
bool YRVoices::Pick_Standalone_Voice(int voice, Wstring & out)
{
	return(WDT_Random_Pick_Sound_Filename(Side, StandaloneVoices[voice], out));
}

// FUNCTION: GAMEMD 0x00770C10
bool __fastcall WDT_Random_Pick_Sound_Filename(int side, VOICEINDEX_LIST & list, Wstring & out)
{
	out.clear();
	int count = list.Count();
	if (count <= 0) {
		return(false);
	}
	return(WDT_Make_Sound_Filename(side, list[count == 1 ? 0 : NonCriticalRandomNumber(0, count - 1)], out));
}

// FUNCTION: GAMEMD 0x00770CE0
bool __fastcall WDT_Make_Sound_Filename(int side, int index, Wstring & out)
{
	if (side == 3) {
		out = "WWD-Soviet" + Wstring_FromInt(index, 2);
	} else {
		out = "WWD_Allied" + Wstring_FromInt(index, 2);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00770D70
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

// FUNCTION: GAMEMD 0x00770E10
// WorldDominationTour::YRVoices::Category::Category
YRVoices::Category::Category(void)
{
	Name = "";
}

// FUNCTION: GAMEMD 0x00770E60
// WorldDominationTour::YRVoices::Category::Set_Name
void YRVoices::Category::Set_Name(Wstring const & name)
{
	Name = name;
}

// FUNCTION: GAMEMD 0x00770E70
// WorldDominationTour::YRVoices::Category::Read
void YRVoices::Category::Read(INIClass const & ini, Wstring const & section, int side)
{
	{ Wstring name("Win"); FUN_007710B0(ini, section, side, OUTCOME_WIN, name); }
	{ Wstring name("Lose"); FUN_007710B0(ini, section, side, OUTCOME_LOSE, name); }
	{ Wstring name("Draw"); FUN_007710B0(ini, section, side, OUTCOME_DRAW, name); }
}

static inline bool Make_Inline(int side, int index, Wstring & out)
{
	if (side == 3) {
		out = "WWD-Soviet" + Wstring_FromInt(index, 2);
	} else {
		out = "WWD_Allied" + Wstring_FromInt(index, 2);
	}
	return(true);
}

static inline bool Pick_Inline(int side, VOICEINDEX_LIST & list, Wstring & out)
{
	out.clear();
	int count = list.Count();
	if (count <= 0) {
		return(false);
	}
	return(Make_Inline(side, list[count == 1 ? 0 : NonCriticalRandomNumber(0, count - 1)], out));
}

// FUNCTION: GAMEMD 0x00770F10
// WorldDominationTour::YRVoices::Category::Pick_Standard_Sound
bool YRVoices::Category::Pick_Standard_Sound(int side, int outcome, Wstring & out)
{
	return(Pick_Inline(side, VoiceIndexes[outcome][0], out));
}

// FUNCTION: GAMEMD 0x00770FE0
// WorldDominationTour::YRVoices::Category::Pick_Emphasis_Sound
bool YRVoices::Category::Pick_Emphasis_Sound(int side, int outcome, Wstring & out)
{
	return(Pick_Inline(side, VoiceIndexes[outcome][1], out));
}

extern bool FUN_007529E0(void);
extern int __fastcall FUN_00753250(char const * name);
extern void __fastcall FUN_00752480(int index, int position, int flags);

extern char DAT_00889F64[];

static inline void Parse_Inline(Wstring const & text, VOICEINDEX_LIST & list)
{
	list.Clear();
	Wstring token;
	int pos = text.getToken(0, ",", token);
	while (pos != -1) {
		list.Add(token.toInt());
		pos = text.getToken(pos, ",", token);
	}
}

// FUNCTION: GAMEMD 0x007710B0
// WorldDominationTour::YRVoices::Category::FUN_007710B0
void YRVoices::Category::FUN_007710B0(INIClass const & ini, Wstring const & section, int side, int outcome, Wstring const & name)
{
	Wstring entry = operator+(*FUN_007AF2A0(side), Name).operator+(name);
	Wstring result;
	{
		Wstring none(DAT_00889F64);
		ini.Get_String(section, entry, none, result);
	}
	Parse_Inline(result, VoiceIndexes[outcome][0]);
	entry += "Emphasis";
	{
		Wstring none(DAT_00889F64);
		ini.Get_String(section, entry, none, result);
	}
	Parse_Inline(result, VoiceIndexes[outcome][1]);
}

extern bool __fastcall FUN_00770CE0(int side, int index, Wstring & out);

extern bool __fastcall FUN_00770C10(int side, VOICEINDEX_LIST & list, Wstring & out);

static inline bool Pick_Extern(int side, VOICEINDEX_LIST & list, Wstring & out)
{
	out.clear();
	int count = list.Count();
	if (count <= 0) {
		return(false);
	}
	return(FUN_00770CE0(side, list[count == 1 ? 0 : NonCriticalRandomNumber(0, count - 1)], out));
}

// FUNCTION: GAMEMD 0x007712C0
// WorldDominationTour::YRVoices::FUN_007712C0
void YRVoices::FUN_007712C0(int category, int outcome, bool emphasis, bool count_queued, int delay)
{
	Discard();
	Wstring name;
	int side = Side;
	if (FUN_00770C10(side, Categories[category].VoiceIndexes[outcome][0], name)) {
		if (!FUN_007529E0()) {
			Timer = delay;
		}
		QueuedSamples.Add(name);
		if (count_queued) {
			QueuedSampleCount++;
		}
		if (emphasis) {
			side = Side;
			if (Pick_Extern(side, Categories[category].VoiceIndexes[outcome][1], name)) {
				QueuedSamples.Add(name);
				if (count_queued) {
					QueuedSampleCount++;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007714B0
// WorldDominationTour::YRVoices::FUN_007714B0
void YRVoices::FUN_007714B0(int voice, bool count_queued, int delay)
{
	Discard();
	Wstring name;
	int side = Side;
	VOICEINDEX_LIST & list = StandaloneVoices[voice];
	name.clear();
	int count = list.Count();
	if (count > 0) {
		if (FUN_00770CE0(side, list[count == 1 ? 0 : NonCriticalRandomNumber(0, count - 1)], name)) {
			if (!FUN_007529E0()) {
				Timer = delay;
			}
			QueuedSamples.Add(name);
			if (count_queued) {
				QueuedSampleCount++;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00771620
// WorldDominationTour::YRVoices::Advance
void YRVoices::Advance(void)
{
	if (Timer == 0 && QueuedSamples.Count() > 0 && !FUN_007529E0()) {
		if (!IsIdle) {
			QueuedSamples.Delete_Index(0);
			if (QueuedSampleCount > 0) {
				QueuedSampleCount--;
			}
		}
		if (QueuedSamples.Count() > 0) {
			FUN_00752480(FUN_00753250(QueuedSamples[0].get()), -1, -1);
			IsIdle = false;
		} else {
			QueuedSampleCount = 0;
			IsIdle = true;
		}
	}
}

// FUNCTION: GAMEMD 0x00771710
// WorldDominationTour::YRVoices::Discard
void YRVoices::Discard(void)
{
	int last = QueuedSamples.Count();
	while (last-- > QueuedSampleCount) {
		QueuedSamples.Delete_Index(last);
	}
	if (QueuedSampleCount == 0) {
		IsIdle = true;
	}
}

// FUNCTION: GAMEMD 0x00771780
void FUN_00771780(void)
{
}

// FUNCTION: GAMEMD 0x00771790
void FUN_00771790(void)
{
}

// FUNCTION: GAMEMD 0x007717A0
// WorldDominationTour::YRVoices::Cleanup
void YRVoices::Cleanup(void)
{
	if (QueuedSamples.Count() > 0) {
		FUN_007529A0(true);
		QueuedSamples.Clear();
	}
	Timer = 0;
}

// FUNCTION: GAMEMD 0x007717F0
// WorldDominationTour::YRVoices::FUN_007717F0
bool YRVoices::FUN_007717F0(void)
{
	return(FUN_007529E0());
}

// FUNCTION: GAMEMD 0x00771800
// WorldDominationTour::YRVoices::FUN_00771800
MSAnim * YRVoices::FUN_00771800(void)
{
	return(new Anim(this));
}

// FUNCTION: GAMEMD 0x00771850
// WorldDominationTour::YRVoices::Anim::Anim
YRVoices::Anim::Anim(YRVoices * voices) :
	MSAnim(0, 0, false),
	VoicePlayer(voices)
{
}

// FUNCTION: GAMEMD 0x00771890
// WorldDominationTour::YRVoices::Anim::~Anim
YRVoices::Anim::~Anim(void)
{
}

// FUNCTION: GAMEMD 0x007718A0
// WorldDominationTour::YRVoices::Anim::Set_Active
void YRVoices::Anim::Set_Active(bool active)
{
	Active = active;
	if (!Active) {
		VoicePlayer->Cleanup();
	}
}

// FUNCTION: GAMEMD 0x00771900
// WorldDominationTour::YRVoices::Anim::Pause
void YRVoices::Anim::Pause(void)
{
	Timer.Stop();
}

// FUNCTION: GAMEMD 0x00771950
// WorldDominationTour::YRVoices::Anim::Resume
void YRVoices::Anim::Resume(void)
{
	Timer.Start();
}

// TEMPLATE: GAMEMD 0x00771970
// ?Advance@Anim@YRVoices@WorldDominationTour@@UAE_NPAVSurface@@AAV?$TRect@H@@@Z
bool YRVoices::Anim::Advance(Surface * surface, Rect & rect)
{
	VoicePlayer->Advance();
	return(false);
}

// TEMPLATE: GAMEMD 0x00771980
// ?Redraw@Anim@YRVoices@WorldDominationTour@@UAEXPAVSurface@@PBV?$TRect@H@@@Z
void YRVoices::Anim::Redraw(Surface * surface, Rect const * rect)
{
	VoicePlayer->Advance();
}

// TEMPLATE: GAMEMD 0x00771990
// ?Get_Rect@Anim@YRVoices@WorldDominationTour@@UBE?AV?$TRect@H@@XZ
Rect YRVoices::Anim::Get_Rect(void) const
{
	return(Rect());
}

// FUNCTION: GAMEMD 0x007719C0
// WorldDominationTour::YRVoices::Anim::Has_Finished
bool YRVoices::Anim::Has_Finished(void) const
{
	return(VoicePlayer->FUN_007717F0() == false);
}

// TEMPLATE: GAMEMD 0x007719D0
// ?Restore@Anim@YRVoices@WorldDominationTour@@UAEXABV?$TRect@H@@@Z
void YRVoices::Anim::Restore(Rect const & rect)
{
	VoicePlayer->Advance();
}

// TEMPLATE: GAMEMD 0x007719E0
// ??_F?$DynamicVectorClass@I@@QAEXXZ
template class DynamicVectorClass<unsigned int>;

// TEMPLATE: GAMEMD 0x00771A00
// ??1?$VectorClass@I@@UAE@XZ
template class VectorClass<unsigned int>;

// SYNTHETIC: GAMEMD 0x00771A30
// ??_GAnim@YRVoices@WorldDominationTour@@UAEPAXI@Z
