// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audioctl.h"
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
char const * Voc_Name(VocType voc);

enum ContextType {
	IN_NOVAR,
	IN_VAR
};

struct VocClassHeader
{
	VocClassHeader * Next;
	VocClassHeader * Prev;
	unsigned Magic;
};

struct VolumeStruct
{
	int Volume;
	unsigned unknown_4;
	int unknown_int_8;
	int unknown_int_C;
	int unknown_int_10;
	int unknown_int_14;
	unsigned nknown_18;
	int unknown_int_1C;
};

class AudioEvent
{
	public:
		void FUN_004052E0(void);
		void FUN_004052F0(void);
		void FUN_004053E0(void);
		void FUN_00405430(void);
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
		bool __fastcall FUN_004064A0(char const * sample);
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

		VocClassHeader Header;
		int SamplesOK;
		unsigned Control;
		unsigned Type;
		VolumeStruct Volume;
		unsigned unknown_38;
		unsigned unknown_3C;
		int Priority;
		unsigned unknown_44;
		int Limit;
		int Loop;
		int Range;
		float MinVolume;
		int MinDelay;
		int MaxDelay;
		int MinFDelta;
		int MaxFDelta;
		int VShift;
		char Name[32];
		unsigned unknown_8C;
		unsigned unknown_90;
		unsigned unknown_94;
		unsigned unknown_98;
		unsigned unknown_9C;
		unsigned unknown_A0;
		unsigned unknown_A4;
		unsigned unknown_A8;
		unsigned unknown_AC;
		unsigned unknown_B0;
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

		char const * Name(void) { return Voc != NULL ? Voc->FUN_00405170() : "Invalid Voc"; }

		VocClass * Voc;
};
