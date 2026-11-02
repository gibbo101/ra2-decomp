// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audlevel.h"

#define AUDIO_VOLUME_MAX 0x4000
#define AUDIO_PAN_CENTER 0x2000
#define AUDIO_PAN_LEFT   0
#define AUDIO_PAN_RIGHT  0x4000u

struct AudioAttribs
{
	AudioLevel VolumeLevel;
	unsigned : 32;
	unsigned : 32;
	AudioLevel PitchLevel;
	unsigned : 32;
	unsigned : 32;
	AudioLevel PanPosition;
	unsigned : 32;
	unsigned : 32;
};

#define AUDIO_LEVEL_APPLY(l, v) ((unsigned)((v) * ((unsigned)(l)->level >> AUDIO_LEVEL_SCALE)) >> 14)
#define AudioAttribsGetVolume(a) ((unsigned)(a)->VolumeLevel.level >> AUDIO_LEVEL_SCALE)
#define AudioAttribsGetPitch(a)  ((unsigned)(a)->PitchLevel.level >> AUDIO_LEVEL_SCALE)
#define AudioAttribsGetPan(a)    ((unsigned)(a)->PanPosition.level >> AUDIO_LEVEL_SCALE)

void __fastcall AudioAttribsInit(AudioAttribs * attr);
void __fastcall AudioAttribsUpdate(AudioAttribs * attr);
int __fastcall AudioAttribsChanged(AudioAttribs * attr);
void __fastcall AudioAttribsApply(AudioAttribs * attr, AudioAttribs * mod);
void __fastcall AudioAttribsUsed(AudioAttribs * attr);
int __fastcall AudioAttribsCalcPitch(AudioAttribs * attr, int pitch);
