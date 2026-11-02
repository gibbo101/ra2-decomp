// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audtimer.h"

#define AUDIO_LEVEL_SET 0x01
#define AUDIO_LEVEL_CHANGED 0x02
#define AUDIO_LEVEL_SCALE 16
#define AUDIO_LEVEL_MAX 0x4000

struct AudioLevel
{
	unsigned flags;
	int level;
	int newLevel;
	int change;
	TimeStamp duration;
	TimeStamp lastTime;
};

void __fastcall AudioLevelInit(AudioLevel * level, int startLevel);
void __fastcall AudioLevelSet(AudioLevel * level, int newLevel);
void __fastcall AudioLevelForce(AudioLevel * level);
void __fastcall AudioLevelAdjust(AudioLevel * level, int newLevel);
void __fastcall AudioLevelSetDuration(AudioLevel * level, TimeStamp time, int range);
int __fastcall AudioLevelUpdate(AudioLevel * level);
int __fastcall AudioLevelGetVal(AudioLevel * level);
