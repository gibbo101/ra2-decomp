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

class VoxClass
{
	public:
		static void Play(char const * name, int a = -1, int b = -1);
		static void PlayIndex(int index, int a = -1, int b = -1);
		static void SilenceIndex(int index);
};
