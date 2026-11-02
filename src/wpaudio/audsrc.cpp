// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audsrc.h"
#include "wwfile.h"
#include <stdlib.h>
#include <string.h>

struct WaveFormat
{
	unsigned short wFormatTag;
	unsigned short nChannels;
	unsigned nSamplesPerSec;
	unsigned nAvgBytesPerSec;
	unsigned short nBlockAlign;
	unsigned short wBitsPerSample;
	unsigned short cbSize;
};

struct RiffHeader
{
	unsigned form;
	unsigned length;
	unsigned type;
};

struct RiffChunk
{
	unsigned type;
	unsigned length;
};

// FUNCTION: GAMEMD 0x00408460
AudioSample * __fastcall AudioCreateSample(unsigned bytes)
{
	AudioSample * sample;

	sample = (AudioSample *)operator new(sizeof(AudioSample));
	memset(sample, 0, sizeof(AudioSample));
	AudioSampleInit(sample);
	sample->Bytes = bytes;

	if (!(sample->Data = (char *)operator new(sample->Bytes))) {
		if (sample) {
			AudioSampleDestroy(sample);
		}
		return NULL;
	}

	return sample;
}

// FUNCTION: GAMEMD 0x004084C0
void __fastcall AudioSampleDestroy(AudioSample * sample)
{
	if (sample->Data) {
		operator delete(sample->Data);
	}
	operator delete(sample);
}

// FUNCTION: GAMEMD 0x004084E0
void __fastcall AudioSampleInit(AudioSample * sample)
{
	sample->Data = NULL;
	sample->Bytes = 0;
	sample->Format = NULL;
	sample->Attribs = NULL;
	ListInit(&sample->Frames);
}

// FUNCTION: GAMEMD 0x00408500
void __fastcall AudioSampleAddFrame(AudioSample * sample, AudioFrame * frame)
{
	ListNodeInsert(&sample->Frames, &frame->nd);
	sample->Bytes += frame->Bytes;
	frame->sample = sample;
}

// FUNCTION: GAMEMD 0x00408520
AudioFrame * __fastcall AudioSampleFirstFrame(AudioSample * sample)
{
	return (AudioFrame *)ListNodeNext(&sample->Frames);
}

// FUNCTION: GAMEMD 0x00408530
void __fastcall AudioSampleDeinit(AudioSample * sample)
{
}

// FUNCTION: GAMEMD 0x00408540
int __fastcall AudioFormatBytes(AudioFormat * format, TimeStamp time)
{
	return format->BytesPerSecond * (int)time / SECONDS(1);
}

// FUNCTION: GAMEMD 0x00408560
TimeStamp __fastcall AudioFormatTime(AudioFormat * format, int bytes)
{
	if (format->BytesPerSecond) {
		return (((TimeStamp)bytes) * (TimeStamp)SECONDS(1)) / (TimeStamp)format->BytesPerSecond;
	}

	return 0;
}

// FUNCTION: GAMEMD 0x004085A0
void __fastcall AudioFormatInit(AudioFormat * format)
{
	memset(format, 0, sizeof(AudioFormat));
}

// FUNCTION: GAMEMD 0x004085B0
void __fastcall AudioFormatUpdate(AudioFormat * format)
{
	format->BytesPerSecond = format->Channels * format->SampleWidth * format->Rate;
	if (format->Compression == AUDIO_COMPRESS_IMA_ADPCM || format->Compression == AUDIO_COMPRESS_MS_ADPCM) {
		format->BytesPerSecond >>= 2;
	}
}

// FUNCTION: GAMEMD 0x004085E0
void __fastcall AudioFrameInit(AudioFrame * frame, void * data, int bytes)
{
	frame->Bytes = bytes;
	frame->Data = data;
	ListNodeInit(&frame->nd);
	frame->sample = NULL;
}

// FUNCTION: GAMEMD 0x00408600
void __fastcall AudioFrameDeinit(AudioFrame * frame)
{
	frame->sample = NULL;
}

// FUNCTION: GAMEMD 0x00408610
int __fastcall AudioFormatReadWaveFile(FileClass * file, AudioFormat * format, int * bytes)
{
	RiffHeader rh;
	RiffChunk chunk;
	WaveFormat * wformat = NULL;
	int result = 0;

	if (bytes) {
		*bytes = 0;
	}

	if (!file) {
		goto done;
	}

	file->Seek(0, 0);

	if (file->Read(&rh, sizeof(rh)) != sizeof(rh)) {
		goto done;
	}

	if (rh.form != 0x46464952 || rh.type != 0x45564157) {
		goto done;
	}

	while (file->Is_Open()) {
		if (file->Read(&chunk, sizeof(chunk)) != sizeof(chunk)) {
			goto done;
		}

		switch (chunk.type) {
		case 0x20746d66:
			if (chunk.length < 0x12) {
				wformat = (WaveFormat *)malloc(0x12);
				memset(wformat, 0, 0x12);
			} else {
				wformat = (WaveFormat *)malloc(chunk.length);
				memset(wformat, 0, chunk.length);
			}
			file->Read(wformat, chunk.length);
			wformat->cbSize = (unsigned short)chunk.length;
			break;
		case 0x61746164:
			*bytes = chunk.length;
			goto got_data;
		default:
			file->Seek(chunk.length, 1);
			break;
		}
	}

	goto done;

got_data:
	if (!wformat) {
		goto done;
	}

	if (wformat->wFormatTag == 0x11) {
		format->BlockSize = wformat->nBlockAlign;
		format->Compression = AUDIO_COMPRESS_IMA_ADPCM;
		format->SampleWidth = 2;
	} else if (wformat->wFormatTag == 1) {
		format->Compression = AUDIO_COMPRESS_NONE;
		format->SampleWidth = wformat->wBitsPerSample / 8;
	} else {
		goto done;
	}

	format->Channels = wformat->nChannels;
	format->BytesPerSecond = wformat->nAvgBytesPerSec;
	format->Rate = wformat->nSamplesPerSec;
	format->Flags = mAUDIO_FORMAT_PCM;
	AudioFormatUpdate(format);
	result = 1;

done:
	if (wformat) {
		free(wformat);
	}
	return result;
}
