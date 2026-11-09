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
int __fastcall FUN_00403ED0(AudioDevice * device, AudioIDXData * idx);
void AudioEventCloseDown(void);
void AudioServiceAllEvents(void);
void FUN_00404E70(void);
void FUN_00404FD0(void);
void FUN_00405040(void);
void FUN_004081A0(void);
void FUN_00407550(void);
void FUN_00407780(void);
void Speech_Init(void);
void Speech_Shutdown(void);
void Speech_Service(void);
void Speech_Init(void);
void Speech_Shutdown(void);
void Speech_Service(void);
void FUN_00752AD0(void);
void Theme_Suspend(void);
void Theme_Resume(void);
void FUN_00753510(void);
void FUN_007535B0(void);
void FUN_00753620(void);
void FUN_007B84D0(void);
void FUN_0040A940(void);
void FUN_0040A950(void);

AudioIDXData * AudioIDX;
AudioDevice * AudioMainDevice;
int AudioEnabled;
int AudioFocusLocks;
MixFileClass * AudioMix;
MixFileClass * ScoresMix;
AudioAttribs AudioMasterData;
AudioAttribs AudioScoreData;
AudioAttribs AudioVoiceData;
AudioAttribs AudioSoundData;
AudioAttribs AudioCompData;
AudioAttribs AudioFadeData;
AudioAttribs AudioGroupData2;
AudioAttribs AudioGroupData1;
AudioAttribs * DAT_0087E73C;
AudioAttribs * AudioVoiceAttribs;
AudioAttribs * AudioScoreAttribs;
AudioAttribs * AudioSoundAttribs;
AudioAttribs * DAT_0087E74C;
AudioAttribs * AudioGroupAttribs2;
AudioAttribs * AudioFadeAttribs;
AudioAttribs * AudioGroupAttribs1;
int AudioServiceBusy;
TimeStamp AudioServiceLast;

// FUNCTION: GAMEMD 0x00406B10
int __fastcall FUN_00406B10(int enable)
{
	InitAudioTimer();
	AudioAttribsInit(&AudioMasterData);
	DAT_0087E73C = &AudioMasterData;
	AudioAttribsInit(&AudioScoreData);
	AudioScoreAttribs = &AudioScoreData;
	AudioAttribsInit(&AudioVoiceData);
	AudioVoiceAttribs = &AudioVoiceData;
	AudioAttribsInit(&AudioSoundData);
	AudioSoundAttribs = &AudioSoundData;
	AudioAttribsInit(&AudioCompData);
	DAT_0087E74C = &AudioCompData;
	AudioAttribsInit(&AudioFadeData);
	AudioFadeAttribs = &AudioFadeData;
	AudioAttribsInit(&AudioGroupData2);
	AudioGroupAttribs2 = &AudioGroupData2;
	AudioAttribsInit(&AudioGroupData1);
	AudioGroupAttribs1 = &AudioGroupData1;

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

		AudioMainDevice = FUN_00402C70(-1, &format);
		if (!AudioMainDevice || FUN_00403530(AudioMainDevice, 0x10) != 0x10) {
			enable = 0;
		} else {
			DAT_0087E73C = &AudioMainDevice->Attribs;
			if (CCFileClass("AUDIOMD.MIX").Is_Available(0)) {
				AudioMix = new MixFileClass("AUDIOMD.MIX", &DAT_00886980);
			} else {
				AudioMix = new MixFileClass("AUDIO.MIX", &DAT_00886980);
			}
			AudioIDX = FUN_004011C0("audio", 0);
			if (!AudioIDX) {
				enable = 0;
			}
		}
	}

	FUN_00407550();
	if (!FUN_00403ED0(AudioMainDevice, AudioIDX)) {
		enable = 0;
	}
	Speech_Init();
	FUN_00752AD0();
	AudioEnabled = enable;
	return 1;
}

// FUNCTION: GAMEMD 0x00406D40
void Audio_Shutdown(void)
{
	if (AudioMainDevice) {
		FUN_00403730(AudioMainDevice, 1);
	}
	FUN_00404E70();
	FUN_004081A0();
	Speech_Shutdown();
	Theme_Suspend();
	AudioEventCloseDown();
	FUN_00407780();
	if (AudioIDX) {
		FUN_00401580(AudioIDX);
		AudioIDX = NULL;
	}
	if (AudioMix) {
		delete AudioMix;
		AudioMix = NULL;
	}
	if (ScoresMix) {
		delete ScoresMix;
		ScoresMix = NULL;
	}
	if (AudioMainDevice) {
		FUN_004033C0(AudioMainDevice);
		AudioMainDevice = NULL;
	}
	AudioCloseDown();
}

// FUNCTION: GAMEMD 0x00406DE0
int Audio_Open_Device(void)
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

	AudioMainDevice = FUN_00402C70(-1, &format);
	if (AudioMainDevice && FUN_00403530(AudioMainDevice, 0x10) == 0x10) {
		return 1;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00406E60
void Audio_Close_Device(void)
{
	if (AudioMainDevice) {
		FUN_004033C0(AudioMainDevice);
		AudioMainDevice = NULL;
	}
}

// FUNCTION: GAMEMD 0x00406E80
void Audio_Stop_Events(void)
{
	if (AudioMainDevice) {
		FUN_00403730(AudioMainDevice, 1);
	}
	FUN_00404E70();
}

// FUNCTION: GAMEMD 0x00406EA0
void Audio_Pause_Channels(void)
{
	if (AudioMainDevice) {
		FUN_00403770(AudioMainDevice, 1);
	}
	FUN_00404FD0();
	Speech_Shutdown();
}

// FUNCTION: GAMEMD 0x00406EC0
void Audio_Resume_Channels(void)
{
	Speech_Service();
	FUN_00405040();
	if (AudioMainDevice) {
		FUN_004037C0(AudioMainDevice, 1);
	}
}

// FUNCTION: GAMEMD 0x00406EE0
void Audio_Stop_All(void)
{
	if (AudioMainDevice) {
		FUN_00403730(AudioMainDevice, 1);
	}
	FUN_00404E70();
	FUN_004081A0();
}

// FUNCTION: GAMEMD 0x00406F00
void FUN_00406F00(void)
{
	if (!ChronoScreenEffect::Active()) {
		if (AudioMainDevice) {
			FUN_00403770(AudioMainDevice, 1);
		}
		FUN_00404FD0();
		Speech_Shutdown();
	}
	FUN_007535B0();
	Theme_Resume();
}

// FUNCTION: GAMEMD 0x00406F40
void FUN_00406F40(void)
{
	FUN_00753510();
	FUN_00753620();
	Speech_Service();
	FUN_00405040();
	if (AudioMainDevice) {
		FUN_004037C0(AudioMainDevice, 1);
	}
}

// FUNCTION: GAMEMD 0x00406F70
void FUN_00406F70(void)
{
	if (!AudioServiceBusy && AudioMainDevice) {
		AudioServiceBusy = 1;
		TimeStamp now = AudioGetTime();
		if (now - AudioServiceLast > 33) {
			AudioServiceAllEvents();
			Speech_Init();
			Speech_Service();
			Theme.AI();
			FUN_007B84D0();
			AudioServiceLast = now;
		}
		AudioServiceBusy = 0;
	}
}

// FUNCTION: GAMEMD 0x00407000
int FUN_00407000(void)
{
	return AudioMainDevice != NULL;
}

// FUNCTION: GAMEMD 0x00407010
AudioDevice * AudioGetDevice(void)
{
	return AudioMainDevice;
}

// FUNCTION: GAMEMD 0x00407020
void Audio_Focus_Lose(void)
{
	if (!AudioFocusLocks) {
		FUN_0040A940();
	}
	AudioFocusLocks++;
}

// FUNCTION: GAMEMD 0x00407040
void Audio_Focus_Regain(void)
{
	if (AudioFocusLocks) {
		AudioFocusLocks--;
		if (!AudioFocusLocks) {
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
int __fastcall AudioLevelIsFading(AudioLevel * level)
{
	return level->level != level->newLevel;
}

// FUNCTION: GAMEMD 0x004070B0
void __fastcall AudioLevelWaitFade(AudioLevel * level)
{
	TimeStamp end = AudioGetTime() + level->duration;

	while (level->level != level->newLevel && AudioGetTime() < end) {
		Game::CallBack();
	}
}
