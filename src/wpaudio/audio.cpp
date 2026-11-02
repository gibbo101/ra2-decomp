// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "mixfile.h"
#include "ccfile.h"
#include "auddev.h"
#include "audformat.h"
#include "audtimer.h"
#include "audlevel.h"
#include "theme.h"

// SYNTHETIC: GAMEMD 0x00406920 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00406950 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00406970 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00406990 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004069B0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004069D0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004069F0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00406A30 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00406A60 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00406A90 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00406AC0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00406AD0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00406AF0 SYMBOL
// _$E38

class PKey;
extern PKey DAT_00886980;
extern void * DAT_00816514;

struct AudioIDXData;
struct ChronoScreenEffect { static bool Active(void); };
struct Game { static bool __cdecl CallBack(void); };

void __fastcall FUN_00401580(AudioIDXData * idx);
AudioIDXData * __fastcall FUN_004011C0(char const * name, int flags);
void FUN_00402940(void);
void __fastcall FUN_00402AF0(void * driver);
AudioDevice * __fastcall FUN_00402C70(int index, AudioFormat * format);
int __fastcall FUN_00403530(AudioDevice * device, int mode);
void __fastcall FUN_00403730(AudioDevice * device, int mode);
void __fastcall FUN_00403770(AudioDevice * device, int mode);
void __fastcall FUN_004037C0(AudioDevice * device, int mode);
void __fastcall FUN_004033C0(AudioDevice * device);
void FUN_00402990(void);
int __fastcall FUN_00403ED0(AudioDevice * device, AudioIDXData * idx);
void FUN_00403FA0(void);
void FUN_004041D0(void);
void FUN_00404E70(void);
void FUN_00404FD0(void);
void FUN_00405040(void);
void FUN_004081A0(void);
void FUN_00407550(void);
void FUN_00407780(void);
void FUN_00750EC0(void);
void FUN_00751040(void);
void FUN_00751080(void);
void FUN_00752290(void);
void FUN_00752340(void);
void FUN_00752760(void);
void FUN_00752AD0(void);
void FUN_00752B40(void);
void FUN_00753500(void);
void FUN_00753510(void);
void FUN_007535B0(void);
void FUN_00753620(void);
void FUN_007B84D0(void);
void FUN_0040A940(void);
void FUN_0040A950(void);

AudioIDXData * DAT_0087E724;
AudioDevice * DAT_0087E728;
int DAT_0087E72C;
int DAT_0087E730;
MixFileClass * DAT_0087E734;
MixFileClass * DAT_0087E738;
AudioAttribs DAT_0087E2C0;
AudioAttribs DAT_0087E4E0;
AudioAttribs DAT_0087E578;
AudioAttribs DAT_0087E450;
AudioAttribs DAT_0087E340;
AudioAttribs DAT_0087E6A0;
AudioAttribs DAT_0087E5F8;
AudioAttribs DAT_0087E3C0;
AudioAttribs * DAT_0087E73C;
AudioAttribs * DAT_0087E740;
AudioAttribs * DAT_0087E744;
AudioAttribs * DAT_0087E748;
AudioAttribs * DAT_0087E74C;
AudioAttribs * DAT_0087E750;
AudioAttribs * DAT_0087E754;
AudioAttribs * DAT_0087E758;
int DAT_0087E75C;
TimeStamp DAT_0087E760;

// FUNCTION: GAMEMD 0x00406B10
int __fastcall FUN_00406B10(int enable)
{
	InitAudioTimer();
	AudioAttribsInit(&DAT_0087E2C0);
	DAT_0087E73C = &DAT_0087E2C0;
	AudioAttribsInit(&DAT_0087E4E0);
	DAT_0087E744 = &DAT_0087E4E0;
	AudioAttribsInit(&DAT_0087E578);
	DAT_0087E740 = &DAT_0087E578;
	AudioAttribsInit(&DAT_0087E450);
	DAT_0087E748 = &DAT_0087E450;
	AudioAttribsInit(&DAT_0087E340);
	DAT_0087E74C = &DAT_0087E340;
	AudioAttribsInit(&DAT_0087E6A0);
	DAT_0087E754 = &DAT_0087E6A0;
	AudioAttribsInit(&DAT_0087E5F8);
	DAT_0087E750 = &DAT_0087E5F8;
	AudioAttribsInit(&DAT_0087E3C0);
	DAT_0087E758 = &DAT_0087E3C0;

	if (enable) {
		FUN_00402940();
		FUN_00402AF0(DAT_00816514);

		AudioFormat format;
		AudioFormatInit(&format);
		format.Rate = 0x5622;
		format.Compression = AUDIO_COMPRESS_NONE;
		format.Channels = 2;
		format.SampleWidth = 2;
		format.Flags = mAUDIO_FORMAT_PCM;
		AudioFormatUpdate(&format);

		DAT_0087E728 = FUN_00402C70(-1, &format);
		if (!DAT_0087E728 || FUN_00403530(DAT_0087E728, 0x10) != 0x10) {
			enable = 0;
		} else {
			DAT_0087E73C = &DAT_0087E728->Attribs;
			if (CCFileClass("AUDIOMD.MIX").Is_Available(0)) {
				DAT_0087E734 = new MixFileClass("AUDIOMD.MIX", &DAT_00886980);
			} else {
				DAT_0087E734 = new MixFileClass("AUDIO.MIX", &DAT_00886980);
			}
			DAT_0087E724 = FUN_004011C0("audio", 0);
			if (!DAT_0087E724) {
				enable = 0;
			}
		}
	}

	FUN_00407550();
	if (!FUN_00403ED0(DAT_0087E728, DAT_0087E724)) {
		enable = 0;
	}
	FUN_00752290();
	FUN_00752AD0();
	DAT_0087E72C = enable;
	return 1;
}

// FUNCTION: GAMEMD 0x00406D40
void FUN_00406D40(void)
{
	if (DAT_0087E728) {
		FUN_00403730(DAT_0087E728, 1);
	}
	FUN_00404E70();
	FUN_004081A0();
	FUN_00752340();
	FUN_00752B40();
	FUN_00403FA0();
	FUN_00407780();
	if (DAT_0087E724) {
		FUN_00401580(DAT_0087E724);
		DAT_0087E724 = NULL;
	}
	if (DAT_0087E734) {
		delete DAT_0087E734;
		DAT_0087E734 = NULL;
	}
	if (DAT_0087E738) {
		delete DAT_0087E738;
		DAT_0087E738 = NULL;
	}
	if (DAT_0087E728) {
		FUN_004033C0(DAT_0087E728);
		DAT_0087E728 = NULL;
	}
	FUN_00402990();
}

// FUNCTION: GAMEMD 0x00406DE0
int FUN_00406DE0(void)
{
	AudioFormat format;

	FUN_00402AF0(DAT_00816514);
	AudioFormatInit(&format);
	format.Rate = 0x5622;
	format.Compression = AUDIO_COMPRESS_NONE;
	format.Channels = 2;
	format.SampleWidth = 2;
	format.Flags = mAUDIO_FORMAT_PCM;
	AudioFormatUpdate(&format);

	DAT_0087E728 = FUN_00402C70(-1, &format);
	if (DAT_0087E728 && FUN_00403530(DAT_0087E728, 0x10) == 0x10) {
		return 1;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00406E60
void FUN_00406E60(void)
{
	if (DAT_0087E728) {
		FUN_004033C0(DAT_0087E728);
		DAT_0087E728 = NULL;
	}
}

// FUNCTION: GAMEMD 0x00406E80
void FUN_00406E80(void)
{
	if (DAT_0087E728) {
		FUN_00403730(DAT_0087E728, 1);
	}
	FUN_00404E70();
}

// FUNCTION: GAMEMD 0x00406EA0
void FUN_00406EA0(void)
{
	if (DAT_0087E728) {
		FUN_00403770(DAT_0087E728, 1);
	}
	FUN_00404FD0();
	FUN_00751040();
}

// FUNCTION: GAMEMD 0x00406EC0
void FUN_00406EC0(void)
{
	FUN_00751080();
	FUN_00405040();
	if (DAT_0087E728) {
		FUN_004037C0(DAT_0087E728, 1);
	}
}

// FUNCTION: GAMEMD 0x00406EE0
void FUN_00406EE0(void)
{
	if (DAT_0087E728) {
		FUN_00403730(DAT_0087E728, 1);
	}
	FUN_00404E70();
	FUN_004081A0();
}

// FUNCTION: GAMEMD 0x00406F00
void FUN_00406F00(void)
{
	if (!ChronoScreenEffect::Active()) {
		if (DAT_0087E728) {
			FUN_00403770(DAT_0087E728, 1);
		}
		FUN_00404FD0();
		FUN_00751040();
	}
	FUN_007535B0();
	FUN_00753500();
}

// FUNCTION: GAMEMD 0x00406F40
void FUN_00406F40(void)
{
	FUN_00753510();
	FUN_00753620();
	FUN_00751080();
	FUN_00405040();
	if (DAT_0087E728) {
		FUN_004037C0(DAT_0087E728, 1);
	}
}

// FUNCTION: GAMEMD 0x00406F70
void FUN_00406F70(void)
{
	if (!DAT_0087E75C && DAT_0087E728) {
		DAT_0087E75C = 1;
		TimeStamp now = AudioGetTime();
		if (now - DAT_0087E760 > 33) {
			FUN_004041D0();
			FUN_00750EC0();
			FUN_00752760();
			Theme.AI();
			FUN_007B84D0();
			DAT_0087E760 = now;
		}
		DAT_0087E75C = 0;
	}
}

// FUNCTION: GAMEMD 0x00407000
int FUN_00407000(void)
{
	return DAT_0087E728 != NULL;
}

// FUNCTION: GAMEMD 0x00407010
AudioDevice * FUN_00407010(void)
{
	return DAT_0087E728;
}

// FUNCTION: GAMEMD 0x00407020
void FUN_00407020(void)
{
	if (!DAT_0087E730) {
		FUN_0040A940();
	}
	DAT_0087E730++;
}

// FUNCTION: GAMEMD 0x00407040
void FUN_00407040(void)
{
	if (DAT_0087E730) {
		DAT_0087E730--;
		if (!DAT_0087E730) {
			FUN_0040A950();
		}
	}
}

// FUNCTION: GAMEMD 0x00407060
void FUN_00407060(AudioLevel * level)
{
	AudioLevelAdjust(level, AUDIO_LEVEL_MAX);
}

// FUNCTION: GAMEMD 0x00407070
void FUN_00407070(AudioLevel * level)
{
	AudioLevelAdjust(level, 0);
}

// FUNCTION: GAMEMD 0x00407080
void FUN_00407080(AudioLevel * level, int newLevel)
{
	AudioLevelAdjust(level, newLevel);
}

// FUNCTION: GAMEMD 0x00407090
int __fastcall FUN_00407090(AudioLevel * level)
{
	return level->level != level->newLevel;
}

// FUNCTION: GAMEMD 0x004070B0
void __fastcall FUN_004070B0(AudioLevel * level)
{
	TimeStamp end = AudioGetTime() + level->duration;

	while (level->level != level->newLevel && AudioGetTime() < end) {
		Game::CallBack();
	}
}
