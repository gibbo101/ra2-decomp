// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "vox.hh"

void Speak(VoxType voice, bool now=false);
void Speak_AI(void);
void Stop_Speaking(void);
bool Is_Speaking(void);

void Set_Speech_Volume(int volume);
void Set_Speech_State(bool state);
bool Get_Speech_State(void);

char const * Speech_Name(VoxType speech);

extern char const * Speech[VOX_COUNT];

class INIClass;

struct SoundLink
{
	SoundLink * Next;
	SoundLink * Prev;
	SoundLink * Sentinel;

	void FUN_004072C0(void);
	void FUN_004072D0(void);
	int FUN_004073B0(void);
	SoundLink * FUN_004073C0(void);
	SoundLink * FUN_004073E0(void);
	void __fastcall FUN_00407420(SoundLink * node);
	void FUN_00407450(void);
};

template<class T> class DynamicVectorClass;
class VocClass;
extern DynamicVectorClass<VocClass *> Vocs;

class VoxClass
{
	public:
		VoxClass(char const * name);
		~VoxClass(void);
		bool LoadFromINI(INIClass & ini);
		char const * GetFilename(void) const;

		static void Play(char const * name, int a = -1, int b = -1);
		static void PlayIndex(int index, int a = -1, int b = -1);
		static void SilenceIndex(int index);

		char Name[0x28];
		float Volume;
		char Files[3][9];
		int Priority;
		int Type;
		int State;
};
