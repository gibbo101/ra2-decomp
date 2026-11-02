// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "audlevel.h"

// FUNCTION: GAMEMD 0x00407100
void __fastcall AudioLevelInit(AudioLevel * level, int startLevel)
{
	level->flags = 0;
	level->lastTime = AudioGetTime();
	AudioLevelSetDuration(level, SECONDS(1), AUDIO_LEVEL_MAX);
	AudioLevelSet(level, startLevel);
	AudioLevelUpdate(level);
}

// FUNCTION: GAMEMD 0x00407150
void __fastcall AudioLevelSet(AudioLevel * level, int newLevel)
{
	level->flags |= AUDIO_LEVEL_SET;
	level->newLevel = newLevel << AUDIO_LEVEL_SCALE;
}

// FUNCTION: GAMEMD 0x00407160
void __fastcall AudioLevelForce(AudioLevel * level)
{
	level->flags |= AUDIO_LEVEL_SET;
}

// FUNCTION: GAMEMD 0x00407170
void __fastcall AudioLevelAdjust(AudioLevel * level, int newLevel)
{
	level->flags &= ~AUDIO_LEVEL_SET;

	if (level->newLevel == level->level) {
		level->lastTime = AudioGetTime();
	}

	level->newLevel = newLevel << AUDIO_LEVEL_SCALE;
}

// FUNCTION: GAMEMD 0x004071A0
void __fastcall AudioLevelSetDuration(AudioLevel * level, TimeStamp time, int range)
{
	level->change = (range << AUDIO_LEVEL_SCALE) / (unsigned)time;
	level->duration = time;
}

// FUNCTION: GAMEMD 0x004071C0
int __fastcall AudioLevelUpdate(AudioLevel * level)
{
	int dif;
	int delta;
	TimeStamp time, thisTime;

	if ((dif = (level->newLevel - level->level))) {
		if (level->flags & AUDIO_LEVEL_SET) {
			level->level = level->newLevel;
		} else {
			thisTime = AudioGetTime();
			time = thisTime - level->lastTime;
			level->lastTime = thisTime;

			if (time > level->duration) {
				time = level->duration;
			}

			delta = level->change * (unsigned)time;

			if (dif < 0) {
				if (delta > (-dif)) {
					level->level += dif;
				} else {
					level->level -= delta;
				}
			} else {
				if (delta > dif) {
					level->level += dif;
				} else {
					level->level += delta;
				}
			}
		}

		level->flags |= AUDIO_LEVEL_CHANGED;
		return 1;
	}

	return 0;
}

// FUNCTION: GAMEMD 0x004072B0
int __fastcall AudioLevelGetVal(AudioLevel * level)
{
	return (unsigned)level->newLevel >> AUDIO_LEVEL_SCALE;
}
