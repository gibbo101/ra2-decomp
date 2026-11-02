// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audlist.h"
#include "audattr.h"
#include "audtimer.h"

class FileClass;

#define AUDIO_COMPRESS_NONE 0
#define AUDIO_COMPRESS_IMA_ADPCM 1
#define AUDIO_COMPRESS_MS_ADPCM 2
#define mAUDIO_FORMAT_PCM 4

struct AudioFormat
{
	int Flags;
	int Compression;
	int Rate;
	int Channels;
	int SampleWidth;
	int BytesPerSecond;
	int BlockSize;
	int Reserved;
};

struct AudioSample
{
	char * Data;
	int Bytes;
	ListHead Frames;
	AudioFormat * Format;
	AudioAttribs * Attribs;
	int Reserved;
};

struct AudioFrame
{
	ListNode nd;
	void * Data;
	int Bytes;
	AudioSample * sample;
	int Reserved;
};

AudioSample * __fastcall AudioCreateSample(unsigned bytes);
void __fastcall AudioSampleDestroy(AudioSample * sample);
void __fastcall AudioSampleInit(AudioSample * sample);
void __fastcall AudioSampleAddFrame(AudioSample * sample, AudioFrame * frame);
AudioFrame * __fastcall AudioSampleFirstFrame(AudioSample * sample);
void __fastcall AudioSampleDeinit(AudioSample * sample);
int __fastcall AudioFormatBytes(AudioFormat * format, TimeStamp time);
TimeStamp __fastcall AudioFormatTime(AudioFormat * format, int bytes);
void __fastcall AudioFormatInit(AudioFormat * format);
void __fastcall AudioFormatUpdate(AudioFormat * format);
void __fastcall AudioFrameInit(AudioFrame * frame, void * data, int bytes);
void __fastcall AudioFrameDeinit(AudioFrame * frame);
int __fastcall AudioFormatReadWaveFile(FileClass * file, AudioFormat * format, int * bytes);
