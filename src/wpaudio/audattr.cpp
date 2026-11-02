// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "audattr.h"

// FUNCTION: GAMEMD 0x00401000
void __fastcall AudioAttribsInit(AudioAttribs * attr)
{
	AudioLevelInit(&attr->VolumeLevel, AUDIO_VOLUME_MAX);
	AudioLevelInit(&attr->PitchLevel, 100);
	AudioLevelInit(&attr->PanPosition, AUDIO_PAN_CENTER);
	AudioLevelSetDuration(&attr->PitchLevel, SECONDS(1), 10);
	AudioLevelSetDuration(&attr->VolumeLevel, SECONDS(1), AUDIO_LEVEL_MAX);
	AudioLevelSetDuration(&attr->PanPosition, SECONDS(1), AUDIO_LEVEL_MAX);
	AudioLevelSet(&attr->VolumeLevel, AUDIO_VOLUME_MAX);
}

// FUNCTION: GAMEMD 0x00401080
void __fastcall AudioAttribsUpdate(AudioAttribs * attr)
{
	AudioLevelUpdate(&attr->VolumeLevel);
	AudioLevelUpdate(&attr->PitchLevel);
	AudioLevelUpdate(&attr->PanPosition);
}

// FUNCTION: GAMEMD 0x004010A0
int __fastcall AudioAttribsChanged(AudioAttribs * attr)
{
	return (attr->VolumeLevel.flags & AUDIO_LEVEL_CHANGED) || (attr->PitchLevel.flags & AUDIO_LEVEL_CHANGED) || (attr->PanPosition.flags & AUDIO_LEVEL_CHANGED);
}

// FUNCTION: GAMEMD 0x004010C0
void __fastcall AudioAttribsApply(AudioAttribs * attr, AudioAttribs * mod)
{
	AudioLevelSet(&attr->VolumeLevel, AUDIO_LEVEL_APPLY(&mod->VolumeLevel, AudioAttribsGetVolume(attr)));
	AudioLevelUpdate(&attr->VolumeLevel);

	{
		int level = AudioAttribsGetPitch(attr);
		int change = AudioAttribsGetPitch(mod);
		level = (level * change) / 100;
		AudioLevelSet(&attr->PitchLevel, level);
		AudioLevelUpdate(&attr->PitchLevel);
	}

	{
		int pos;
		if ((pos = AudioAttribsGetPan(mod) - AUDIO_PAN_CENTER) != 0) {
			if ((pos = pos + AudioAttribsGetPan(attr)) < AUDIO_PAN_LEFT) {
				pos = AUDIO_PAN_LEFT;
			} else if (pos > AUDIO_PAN_RIGHT) {
				pos = AUDIO_PAN_RIGHT;
			}
			AudioLevelSet(&attr->PanPosition, pos);
		}
		AudioLevelUpdate(&attr->PanPosition);
	}
}

// FUNCTION: GAMEMD 0x00401170
void __fastcall AudioAttribsUsed(AudioAttribs * attr)
{
	attr->VolumeLevel.flags &= ~AUDIO_LEVEL_CHANGED;
	attr->PanPosition.flags &= ~AUDIO_LEVEL_CHANGED;
	attr->PitchLevel.flags &= ~AUDIO_LEVEL_CHANGED;
}

// FUNCTION: GAMEMD 0x00401190
int __fastcall AudioAttribsCalcPitch(AudioAttribs * attr, int pitch)
{
	int level = AudioAttribsGetPitch(attr);
	return (pitch * level) / 100;
}
