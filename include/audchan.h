// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audlist.h"
#include "audattr.h"
#include "audsrc.h"

#define mAUDIO_CTRL_PLAYING 0x01
#define mAUDIO_CTRL_PAUSED 0x04
#define mAUDIO_CTRL_ALLOCATED 0x08
#define mAUDIO_CTRL_INUSE 0x10

#define AUD_NORMAL_PRIORITY 0
#define AUDIO_CHANNEL_TYPE_STD 1
#define AUDIO_CHANNEL_TYPE_ANY (-1)
#define AUDIO_CHANNEL_TYPE_ALL (-1)
#define AUD_HIGHEST_PRIORITY 0x7f
#define mAUDIO_CTRL_BUSY 0x15
#define AUDIO_CTRL_LOOP_FOREVER (-1)

#define ERROR_CODE_NO_CHANNEL 0x800f0000
#define ERROR_CODE_MISSING_DATA 0x80070000

typedef int AudioChannelType;

struct AudioDevice;

struct AudioChannel;

struct AudioControl
{
	int Priority;
	volatile unsigned Status;
	volatile int LoopCount;
};

typedef int (__fastcall * AudioChannelCB)(AudioChannel * chan);

struct AudioChannelDriver
{
	void * unknown0;
	int (__fastcall * openChannel)(AudioChannel * chan);
	void (__fastcall * closeChannel)(AudioChannel * chan);
	int (__fastcall * start)(AudioChannel * chan);
	void (__fastcall * stop)(AudioChannel * chan);
	void (__fastcall * pause)(AudioChannel * chan);
	void (__fastcall * resume)(AudioChannel * chan);
	void * unknown1C;
	void (__fastcall * update)(AudioChannel * chan);
	void (__fastcall * queueIt)(AudioChannel * chan);
	void (__fastcall * lock)(AudioChannel * chan);
	void (__fastcall * unlock)(AudioChannel * chan);
};

struct AudioChannel
{
	ListNode nd;
	int Type;
	AudioAttribs ChannelAttribs;
	char pad88[8];
	AudioAttribs * SfxAttribs;
	AudioAttribs * GroupAttribs;
	AudioAttribs * CompAttribs;
	AudioAttribs * FadeAttribs;
	AudioControl Control;
	AudioDevice * Device;
	AudioChannelCB CB_NextFrame;
	AudioChannelCB CB_NextSample;
	AudioChannelCB CB_SampleDone;
	AudioChannelCB CB_Stop;
	void * Data;
	char padC4[0x10];
	AudioChannelDriver * driver;
	AudioAttribs attribs;
	char pad150[0xc];
	AudioChannelCB drvCBNextFrame;
	AudioChannelCB drvCBNextSample;
	AudioChannelCB drvCBSampleDone;
	AudioSample * sample;
	AudioFrame * frame;
	char * frameData;
	int bytesInFrame;
	int bytesRemaining;
	AudioFormat current_format;
	int format_changed;
	int drv_format_changed;
	char pad1A4[0x1c];
};

extern AudioAttribs * audioStdChannelAttribs;


AudioChannel * __fastcall audioChannelCreate(AudioDevice * dev);
void __fastcall audioChannelDestroy(AudioChannel * chan);
void __fastcall audioChannelMakeStandard(AudioChannel * chan);
void __fastcall audioChannelRecalcAttribs(AudioChannel * chan);
int __fastcall AudioChannelTaken(AudioChannel * chan);
void __fastcall AudioChannelReserve(AudioChannel * chan, AudioChannelType type);
void __fastcall AudioChannelRelease(AudioChannel * chan);
void __fastcall AudioChannelDestroy(AudioChannel * chan);
void __fastcall AudioChannelStop(AudioChannel * chan);
void __fastcall AudioChannelPause(AudioChannel * chan);
void __fastcall AudioChannelResume(AudioChannel * chan);
void __fastcall AudioChannelLock(AudioChannel * chan);
void __fastcall AudioChannelUnlock(AudioChannel * chan);
void __fastcall AudioChannelUse(AudioChannel * chan);
void __fastcall AudioChannelNoUse(AudioChannel * chan);
int __fastcall AudioChannelStart(AudioChannel * chan);
void __fastcall AudioChannelSetSample(AudioChannel * chan, AudioSample * sample);
int __fastcall AudioChannelSetFormat(AudioChannel * chan, AudioFormat * new_format);
void __fastcall AudioControlInit(AudioControl * ctrl);
