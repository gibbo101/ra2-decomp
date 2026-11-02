// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audioctl.h"
#include "audlist.h"
#include "audlevel.h"
#include "coord.h"
#include "vector.h"

#include "voc.hh"

class CCINIClass;
class INIClass;
class VocClass;
class VocEntry;

extern DynamicVectorClass<VocEntry *> VocArray;
extern bool DAT_008464AC;

void Init_Vocs(CCINIClass const &ini);
void Free_Vocs(void);

int Sound_Effect(VocType voc, float volume=1, int=0);
int Voice_Sound_Effect(VocType voc, float volume=1);
int Sound_Effect(VocType voc, Coord const & coord);
VocClass * VocClass_From_Name(char const * name);
VocEntry * __fastcall FUN_00751520(char const * name);
int __fastcall FUN_007515C0(VocEntry * entry);
char const * Voc_Name(VocType voc);

enum ContextType {
	IN_NOVAR,
	IN_VAR
};

class VocClass
{
	public:

		static void * PlayAt(int voc, Coord const & coord, AudioController * controller = NULL);
		static void * PlayGlobal(int voc, float volume, int panning, AudioController * controller = NULL);
		static void * PlayIndexAtPos(int voc, Coord const & coord, int flag);
		static VocType From_Name(char const * name);
		VocType Voc_Type(void);
		static int __fastcall FindIndex(char const * name);

		char const * FUN_00405170(void);
		int FUN_00405130(void);
		float FUN_00405140(void);
		int FUN_00405160(void);
		AudioEvent * FUN_00405190(void);
		int FUN_00406650(void);
		int __fastcall FUN_004064A0(char const * sample);
		void __fastcall FUN_00406520(int attack);
		void __fastcall FUN_00406530(int decay);
		void __fastcall FUN_00406540(int priority);
		void __fastcall FUN_00406550(int volume);
		void __fastcall FUN_004065C0(int type);
		void __fastcall FUN_004065D0(int limit);
		void __fastcall FUN_004065E0(int range);
		void FUN_004065F0(float minVolume);
		void __fastcall FUN_00406570(int control);
		void __fastcall FUN_00406600(int minDelay, int maxDelay);
		void __fastcall FUN_00406610(int minShift, int maxShift);
		void __fastcall FUN_00406620(int shift);
		void __fastcall FUN_00406640(int loop);

		ListNode Header;
		int SamplesOK;
		unsigned Control;
		unsigned Type;
		AudioLevel Volume;
		unsigned unknown_38;
		unsigned unknown_3C;
		int Priority;
		int Count;
		int Limit;
		int Loop;
		int Range;
		float MinVolume;
		unsigned MinDelay;
		unsigned MaxDelay;
		int MinFDelta;
		int MaxFDelta;
		int VShift;
		char Name[32];
		ListHead local;
		int lastFrame;
		unsigned lastBucketStamp;
		int maxPri;
		ListNode nodeVol;
		unsigned maxVol;
		int SampleIndex[32];
		int NumSamples;
		int Attack;
		int Decay;
		unsigned unknown_140;
		unsigned unknown_144;
};

class VocEntry
{
	public:
		VocEntry(char const * name);
		~VocEntry(void);

		bool Read_INI(INIClass & ini);

		char const * Name(void);

		VocClass * Voc;
};
