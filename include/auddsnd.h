// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audattr.h"
#include "audtimer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#define vNO_ERROR 0
#define ERROR_CODE_FAIL 0x80040000
#define AUDIO_COMPRESS_NONE 0
#define AUDIO_COMPRESS_IMA_ADPCM 1
#define AUDIO_COMPRESS_MS_ADPCM 2
#define mAUDIO_CTRL_PLAYING 1
#define mAUDIO_CTRL_ACTIVE 5

struct AudioFormat
{
	unsigned Flags;
	int Compression;
	int Rate;
	int Channels;
	int SampleWidth;
	int BytesPerSecond;
	int BlockSize;
	unsigned : 32;
};

struct AudioSample
{
	unsigned gap0[5];
	AudioFormat * Format;
};

struct AudioChannel;
struct AudioDevice;

struct AudioServiceInfo
{
	unsigned data[26];
};

struct AudioSearch
{
	unsigned data[2];
};

struct AudioLock
{
	volatile int count;
};

struct AudioDevice
{
	unsigned gap0[17];
	AudioFormat DefaultFormat;
	unsigned gap1[36];
	int maxChannels;
	unsigned gap2[3];
	AudioLock channelAccess;
	unsigned gap3[26];
	AudioServiceInfo mixerUpdate;
	int frames;
	int over_sample;
	int frame_lag;
};

typedef void(__fastcall AudioChannelCB)(AudioChannel * chan);

struct AudioChannel
{
	unsigned gap0[41];
	volatile unsigned Status;
	unsigned gap1;
	AudioDevice * Device;
	unsigned gap2[10];
	AudioAttribs attribs;
	unsigned gap3[2];
	struct AudioDriverChannel * drvData;
	AudioChannelCB * drvCBNextFrame;
	AudioChannelCB * drvCBNextSample;
	AudioChannelCB * drvCBSampleDone;
	AudioSample * sample;
	unsigned gap4;
	void * frameData;
	int bytesInFrame;
	unsigned gap5;
	AudioFormat current_format;
	unsigned gap6;
	int drv_format_changed;
	unsigned gap7;
	TimeStamp time_min_frame;
	TimeStamp time_buffer;
};

struct AudioSystem
{
	unsigned gap0[6];
	int numUnits;
};

typedef int(__fastcall AudioDriverChannelFn)(AudioChannel * chan);

struct AudioDriver
{
	void * data;
	AudioDriverChannelFn * openChannel;
	AudioDriverChannelFn * closeChannel;
	AudioDriverChannelFn * start;
	AudioDriverChannelFn * stop;
	AudioDriverChannelFn * pause;
	AudioDriverChannelFn * resume;
	AudioDriverChannelFn * check;
	AudioDriverChannelFn * update;
	AudioDriverChannelFn * queueIt;
	AudioDriverChannelFn * lock;
	AudioDriverChannelFn * unlock;
};

struct AudioSystemMaster
{
	const char * name;
	unsigned reserved;
	unsigned props;
	unsigned state;
	unsigned stamp;
	int(__fastcall * load)(AudioSystem * system);
	void(__fastcall * unload)(AudioSystem * system);
	int(__fastcall * open)(AudioDevice * dev);
	void(__fastcall * close)(AudioDevice * dev);
	AudioDriver * driver;
};

void __cdecl FUN_004068E0(const char * fmt, ...);
void __fastcall AudioServiceSetInterval(AudioServiceInfo * info, TimeStamp interval);
void __fastcall AudioServiceSetMustServiceInterval(AudioServiceInfo * info, TimeStamp interval);
void __fastcall AudioServiceSetResetInterval(AudioServiceInfo * info, TimeStamp interval);
void __fastcall AudioServicePerform(AudioServiceInfo * info, TimeStamp now);
AudioChannel * __fastcall AudioDeviceFirstChannel(AudioDevice * dev, AudioSearch * sh);
AudioChannel * __fastcall AudioDeviceNextChannel(AudioSearch * sh);
int __fastcall AudioDeviceService(AudioDevice * dev);
void __fastcall AudioChannelSetFormat(AudioChannel * chan, AudioFormat * format);
AudioDevice * __fastcall AudioGetDevice(void);

extern AudioAttribs * DAT_0087E73C;
extern AudioAttribs * AudioVoiceAttribs;
extern AudioAttribs * AudioScoreAttribs;
extern AudioAttribs * AudioSoundAttribs;
extern AudioAttribs * DAT_0087E74C;
extern AudioAttribs * AudioGroupAttribs2;
extern AudioAttribs * AudioFadeAttribs;
extern AudioAttribs * AudioGroupAttribs1;

extern AudioSystemMaster * AudioSystemDSD;
extern LPDIRECTSOUND AUD_sound_object;
extern LPDIRECTSOUNDBUFFER AUD_primary_buffer;
