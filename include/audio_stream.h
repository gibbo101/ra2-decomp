// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once
#include "vox.h"
#include <windows.h>
#include <stddef.h>

struct AudioStream;
struct AudioChannel;
struct AudioBufferOwner;
class FileClass;

struct AudioFormat {
    int Format, Compression, SampleRate, Channels, BytesPerSample;
    int BytesPerSecond, unknown_18, unknown_1C;
    void FUN_004085A0(void);
    int FUN_00408540(unsigned __int64 duration);
    unsigned __int64 __fastcall FUN_00408560(int bytes);
    void FUN_004085B0(void);
};
struct AudioData {
    void * Data;
    int Size;
    SoundLink Buffers;
    int unknown_14, unknown_18, unknown_1C;
    void FUN_004084E0(void);
};
struct AudioPlayback {
    void * Owner;
    void * Cursor;
    int Available;
    void __fastcall FUN_004091C0(int bytes);
    int FUN_00409310(void);
    int __fastcall FUN_00408F80(FileClass * file, int length, int * transferred);
    void FUN_00408E70(void);
};
struct AudioBufferOwner {
    void FUN_00408840(void);
};
struct AudioSource {
    AudioStream * FUN_00407860(unsigned __int64 duration);
    AudioChannel * __fastcall FUN_004036C0(int group);
};
struct AudioChannel {
    char unknown_00[0x90];
    int Mixer[4];
    char unknown_A0[0x10];
    int (__fastcall *Fill)(AudioChannel *);
    void * unknown_B4;
    int (__fastcall *StopA)(AudioChannel *);
    int (__fastcall *StopB)(AudioChannel *);
    AudioStream * Stream;
    char unknown_C4[0xAC];
    void * QueueData;
    int QueueSize, QueueRemaining;
    void FUN_004025B0(void);
    void FUN_00402600(void);
    void FUN_00402650(void);
    void FUN_004024A0(void);
};
struct AudioStream : SoundLink {
    // The file-reading worker clears the read flag while stream controls update it.
    volatile unsigned Flags;
    AudioSource * Source;
    AudioChannel * Channel;
    unsigned __int64 Duration;
    unsigned __int64 CallbackTime;
    unsigned __int64 PreviousTime;
    AudioBufferOwner * BufferOwner;
    AudioPlayback * ReaderA;
    AudioPlayback * ReaderB;
    AudioData Data;
    AudioFormat Format;
    FileClass * File;
    int PauseCount, unknown_84, StopCount, unknown_8C;
    int Remaining, SourceSize, FileSize, BytesRemaining, BufferedBytes;
    char Name[0x50];
    void FUN_00407A90(void);
    void FUN_00407F40(void);
    void FUN_004080C0(void);
    int FUN_004080D0(void);
    void FUN_004080F0(void);
    int FUN_00407F80(void);
    int FUN_00407F90(void);
    int FUN_00407FB0(void);
    void FUN_00408000(void);
    int FUN_00408070(void);
    char const * FUN_004080A0(void);
    unsigned __int64 FUN_00408140(void);
};
extern CRITICAL_SECTION DAT_0087E7F8;
extern SoundLink DAT_0087E810;
void __fastcall FUN_00407440(SoundLink * list, SoundLink * node);
AudioBufferOwner * FUN_004087F0(void);
unsigned __int64 FUN_004093B0(void);
int __fastcall FUN_004079C0(AudioChannel * channel);
int __fastcall FUN_00407A30(AudioChannel * channel);
int __fastcall FUN_00407A60(AudioChannel * channel);

#define AUDIO_ASSERT(tag, expression) typedef char audio_assert_##tag[(expression) ? 1 : -1]
AUDIO_ASSERT(format_size, sizeof(AudioFormat) == 0x20);
AUDIO_ASSERT(data_size, sizeof(AudioData) == 0x20);
AUDIO_ASSERT(stream_size, sizeof(AudioStream) == 0xF8);
AUDIO_ASSERT(stream_source, offsetof(AudioStream, Source) == 0x10);
AUDIO_ASSERT(stream_channel, offsetof(AudioStream, Channel) == 0x14);
AUDIO_ASSERT(stream_duration, offsetof(AudioStream, Duration) == 0x18);
AUDIO_ASSERT(stream_time, offsetof(AudioStream, CallbackTime) == 0x20);
AUDIO_ASSERT(stream_owner, offsetof(AudioStream, BufferOwner) == 0x30);
AUDIO_ASSERT(stream_reader_a, offsetof(AudioStream, ReaderA) == 0x34);
AUDIO_ASSERT(stream_reader_b, offsetof(AudioStream, ReaderB) == 0x38);
AUDIO_ASSERT(stream_data, offsetof(AudioStream, Data) == 0x3C);
AUDIO_ASSERT(stream_format, offsetof(AudioStream, Format) == 0x5C);
AUDIO_ASSERT(stream_file, offsetof(AudioStream, File) == 0x7C);
AUDIO_ASSERT(stream_pause, offsetof(AudioStream, PauseCount) == 0x80);
AUDIO_ASSERT(stream_stop, offsetof(AudioStream, StopCount) == 0x88);
AUDIO_ASSERT(stream_remaining, offsetof(AudioStream, Remaining) == 0x90);
AUDIO_ASSERT(stream_buffered, offsetof(AudioStream, BufferedBytes) == 0xA0);
AUDIO_ASSERT(stream_name, offsetof(AudioStream, Name) == 0xA4);
AUDIO_ASSERT(channel_mixer, offsetof(AudioChannel, Mixer) == 0x90);
AUDIO_ASSERT(channel_fill, offsetof(AudioChannel, Fill) == 0xB0);
AUDIO_ASSERT(channel_stop_a, offsetof(AudioChannel, StopA) == 0xB8);
AUDIO_ASSERT(channel_stop_b, offsetof(AudioChannel, StopB) == 0xBC);
AUDIO_ASSERT(channel_stream, offsetof(AudioChannel, Stream) == 0xC0);
AUDIO_ASSERT(channel_data, offsetof(AudioChannel, QueueData) == 0x170);
AUDIO_ASSERT(channel_size, offsetof(AudioChannel, QueueSize) == 0x174);
AUDIO_ASSERT(channel_remaining, offsetof(AudioChannel, QueueRemaining) == 0x178);

struct AudioTimer {
    unsigned __int64 unknown_00[11];
    void FUN_00403820(void);
    void FUN_00403870(unsigned __int64 value);
    void FUN_004038A0(unsigned __int64 value);
    void FUN_004038C0(unsigned __int64 value);
    void FUN_00403920(unsigned __int64 value);
};
struct AudioWorker {
    volatile int Stop;
    volatile int Iterations;
    volatile int Finished;
    int unknown_0C;
    volatile unsigned __int64 Interval;
    volatile int Running;
    int unknown_1C;
    AudioTimer Timer;
};
extern AudioWorker DAT_0087E770;
extern HANDLE DAT_0087E7F0;
extern DWORD DAT_0087E7F4;
extern int DAT_0087E81C, DAT_0087E820;
DWORD WINAPI FUN_00407680(void * context);
AUDIO_ASSERT(worker_timer, offsetof(AudioWorker, Timer) == 0x20);
AUDIO_ASSERT(worker_interval, offsetof(AudioWorker, Interval) == 0x10);

void __fastcall FUN_00407B40(AudioStream * stream, int mixer);
void __fastcall FUN_00407B50(AudioStream * stream, int mixer);
void __fastcall FUN_00408080(AudioStream * stream, char const * name);
int __fastcall FUN_00408610(FileClass * file, AudioFormat * format, int * length);

#undef AUDIO_ASSERT
