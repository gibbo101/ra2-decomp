// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once
#include "vox.h"
#include "audio_support.h"
#include <windows.h>
#include <stddef.h>

struct AudioStream;
struct AudioChannel;
struct AudioBufferOwner;
struct AudioSampleBlock;
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
    AudioSampleBlock * FUN_00408520(void);
};
struct AudioChunk;
struct AudioSpan { char * Data; int Size; };
struct AudioPlayback {
    AudioBufferOwner * Owner;
    char * Cursor;
    int Available;
    AudioChunk * Current;
    // File refill and playback callbacks update the ring state on worker threads.
    volatile unsigned Flags;
    int Index, Deferred;
    int References;
    unsigned int : 32;
    int Status;
    AudioChunk * First;
    int Pending;
    unsigned Position;
    volatile int WritePosition;
    volatile int ReadPosition;
    unsigned int : 32;
    void FUN_00408930(void);
    int FUN_00409100(void);
    int __fastcall FUN_004091C0(int bytes);
    int FUN_00409310(void);
    unsigned int FUN_00409320(void);
    int __fastcall FUN_00408F80(FileClass * file, int length, int * transferred);
    void FUN_00408E70(void);
};
struct AudioBufferOwner {
    int References;
    SoundLink Chunks;
    unsigned Flags;
    int TotalBytes;
    AudioPlayback Playback[2];
    int ActiveReferences;
    unsigned int : 32;
    unsigned int : 32;
    void FUN_00408840(void);
    void FUN_00408970(void);
    void FUN_00408A70(void);
    int __fastcall FUN_00408B40(int count, int size, int alignment);
    unsigned __fastcall FUN_00408B10(AudioChunk * chunk);
    AudioPlayback * __fastcall FUN_00408C30(int index);
    int FUN_00408D50(void);
    unsigned int FUN_00408D30(void);
    unsigned int FUN_00408D40(void);
};
struct AudioChunk : SoundLink {
    unsigned int : 32;
    void * Allocation;
    int Alignment;
    char * Data;
    int Size;
    AudioSpan Spans[2];
    AudioBufferOwner * Owner;
    int Order;
    unsigned int : 32;
};
AudioChunk * __fastcall FUN_00408D80(int size, int alignment);
struct AudioAttributes {
    AudioRamp Ramps[3];
    unsigned unknown_78, unknown_7C;
};
struct AudioChannelDriver {
    int (__fastcall *unknown_00)(AudioChannel *);
    int (__fastcall *Open)(AudioChannel *);
    int (__fastcall *Close)(AudioChannel *);
    int (__fastcall *Play)(AudioChannel *);
    int (__fastcall *Stop)(AudioChannel *);
    int (__fastcall *Pause)(AudioChannel *);
    int (__fastcall *Resume)(AudioChannel *);
    int (__fastcall *unknown_1C)(AudioChannel *);
    int (__fastcall *unknown_20)(AudioChannel *);
    int (__fastcall *Queue)(AudioChannel *);
    void (__fastcall *Lock)(AudioChannel *);
    void (__fastcall *Unlock)(AudioChannel *);
};
// Workers record elapsed time while diagnostics read and update the timer record.
struct AudioTimer {
    unsigned __int64 Duration, Threshold, LastDelta, MaximumDelta;
    unsigned __int64 ResetLimit, PreviousTime, PeakDelta, LastReport, ReportInterval;
    int Samples, Overruns, DisplayedSamples, DisplayIndex;
    void FUN_00403820(void) volatile;
    void FUN_00403870(unsigned __int64 value) volatile;
    void FUN_004038A0(unsigned __int64 value) volatile;
    void FUN_004038C0(unsigned __int64 value) volatile;
    int FUN_004038E0(unsigned __int64 value) volatile;
    void FUN_00403920(unsigned __int64 value) volatile;
};

struct AudioDeviceDriver;
struct AudioSource : SoundLink {
    unsigned Flags;
    int References, unknown_14;
    AudioDeviceDriver * DeviceDriver;
    int DriverIndex;
    unsigned unknown_20;
    AudioFormat InputFormat;
    AudioFormat DefaultFormat;
    unsigned int : 32;
    AudioAttributes Attributes;
    AudioAttributes * GroupAttributes;
    int OutputIndex;
    int ChannelCount;
    unsigned int : 32;
    SoundLink ChannelList;
    // Channel-list mutation on the game thread is observed by the backend worker.
    volatile int ListReferences;
    unsigned int : 32;
    int (__fastcall *Service)(AudioSource *);
    // The output worker updates timing statistics while the game thread reads them.
    volatile AudioTimer ServiceTimer;
    unsigned __int64 unknown_168;
    volatile AudioTimer UpdateTimer;
    unsigned __int64 unknown_1C8;
    AudioChannelDriver * Driver;
    char unknown_1D4[0x14];
    AudioStream * FUN_00407860(unsigned __int64 duration);
    AudioChannel * __fastcall FUN_004036C0(int group);
    AudioChannel * FUN_00402040(void);
    void __fastcall FUN_00402860(AudioChannel * channel);
};
struct AudioSampleBlock : SoundLink { void * Data; int Size; };
struct AudioChannel : SoundLink {
    int Type;
    AudioAttributes Attributes;
    AudioAttributes * Mixer[4];
    int Priority;
    // Playback callbacks and game-side controls update these shared fields.
    volatile unsigned Flags;
    volatile int LoopCount;
    AudioSource * Device;
    int (__fastcall *Fill)(AudioChannel *);
    int (__fastcall *unknown_B4)(AudioChannel *);
    int (__fastcall *StopA)(AudioChannel *);
    int (__fastcall *StopB)(AudioChannel *);
    AudioStream * Stream;
    char unknown_C4[0x10];
    AudioChannelDriver * Driver;
    AudioAttributes Output;
    void * BackendData;
    int (__fastcall *NextFrame)(AudioChannel *);
    int (__fastcall *NextSample)(AudioChannel *);
    int (__fastcall *SampleDone)(AudioChannel *);
    AudioData * Sample;
    AudioSampleBlock * Block;
    void * QueueData;
    int QueueSize, QueueRemaining;
    AudioFormat Format;
    int FormatChanged, DriverFormatChanged;
    unsigned int : 32;
    unsigned __int64 RatePeriod, BufferDuration;
    char unknown_1B8[8];
    void FUN_00402170(void);
    void FUN_00402190(void);
    void FUN_00402220(void);
    int FUN_00402430(void);
    void __fastcall FUN_00402440(int type);
    void FUN_004024A0(void);
    void FUN_00402550(void);
    void FUN_004025B0(void);
    void FUN_00402600(void);
    void FUN_00402650(void);
    void FUN_004026A0(void);
    void __fastcall FUN_004026B0(void);
    void FUN_004026C0(void);
    void FUN_004026F0(void);
    int FUN_00402720(void);
    void __fastcall FUN_00402790(AudioData * sample);
    int __fastcall FUN_00402800(AudioFormat * format);
    void FUN_004028B0(void);
};
void __fastcall FUN_00401000(AudioAttributes * attributes);
void __fastcall FUN_00401080(AudioAttributes * attributes);
void __fastcall FUN_004010C0(AudioAttributes * output, AudioAttributes * input);
void __fastcall FUN_00401170(AudioAttributes * attributes);
int __fastcall FUN_004022C0(AudioChannel * channel);
int __fastcall FUN_00402330(AudioChannel * channel);
int __fastcall FUN_004023F0(AudioChannel * channel);
extern AudioAttributes * DAT_0087DE30;
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
    // The backend completion callback clears this count from its worker thread.
    volatile int PauseCount;
    int unknown_84, StopCount, unknown_8C;
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

struct AudioWorker {
    volatile int Stop;
    volatile int Iterations;
    volatile int Finished;
    int unknown_0C;
    volatile unsigned __int64 Interval;
    volatile int Running;
    int unknown_1C;
    volatile AudioTimer Timer;
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

AUDIO_ASSERT(AudioChunk_size, sizeof(AudioChunk) == 0x3C);
AUDIO_ASSERT(AudioPlayback_size, sizeof(AudioPlayback) == 0x40);
AUDIO_ASSERT(AudioBufferOwner_size, sizeof(AudioBufferOwner) == 0xA4);
AUDIO_ASSERT(AudioPlayback_Owner, offsetof(AudioPlayback, Owner) == 0x0);
AUDIO_ASSERT(AudioPlayback_Cursor, offsetof(AudioPlayback, Cursor) == 0x4);
AUDIO_ASSERT(AudioPlayback_Available, offsetof(AudioPlayback, Available) == 0x8);
AUDIO_ASSERT(AudioPlayback_Current, offsetof(AudioPlayback, Current) == 0xC);
AUDIO_ASSERT(AudioPlayback_Flags, offsetof(AudioPlayback, Flags) == 0x10);
AUDIO_ASSERT(AudioPlayback_Index, offsetof(AudioPlayback, Index) == 0x14);
AUDIO_ASSERT(AudioPlayback_Deferred, offsetof(AudioPlayback, Deferred) == 0x18);
AUDIO_ASSERT(AudioPlayback_References, offsetof(AudioPlayback, References) == 0x1C);
AUDIO_ASSERT(AudioPlayback_Status, offsetof(AudioPlayback, Status) == 0x24);
AUDIO_ASSERT(AudioPlayback_First, offsetof(AudioPlayback, First) == 0x28);
AUDIO_ASSERT(AudioPlayback_Pending, offsetof(AudioPlayback, Pending) == 0x2C);
AUDIO_ASSERT(AudioPlayback_Position, offsetof(AudioPlayback, Position) == 0x30);
AUDIO_ASSERT(AudioPlayback_WritePosition, offsetof(AudioPlayback, WritePosition) == 0x34);
AUDIO_ASSERT(AudioPlayback_ReadPosition, offsetof(AudioPlayback, ReadPosition) == 0x38);
AUDIO_ASSERT(AudioChunk_Allocation, offsetof(AudioChunk, Allocation) == 0x10);
AUDIO_ASSERT(AudioChunk_Alignment, offsetof(AudioChunk, Alignment) == 0x14);
AUDIO_ASSERT(AudioChunk_Data, offsetof(AudioChunk, Data) == 0x18);
AUDIO_ASSERT(AudioChunk_Size, offsetof(AudioChunk, Size) == 0x1C);
AUDIO_ASSERT(AudioChunk_Spans, offsetof(AudioChunk, Spans) == 0x20);
AUDIO_ASSERT(AudioChunk_Owner, offsetof(AudioChunk, Owner) == 0x30);
AUDIO_ASSERT(AudioChunk_Order, offsetof(AudioChunk, Order) == 0x34);
AUDIO_ASSERT(AudioBufferOwner_References, offsetof(AudioBufferOwner, References) == 0x0);
AUDIO_ASSERT(AudioBufferOwner_Chunks, offsetof(AudioBufferOwner, Chunks) == 0x4);
AUDIO_ASSERT(AudioBufferOwner_Flags, offsetof(AudioBufferOwner, Flags) == 0x10);
AUDIO_ASSERT(AudioBufferOwner_TotalBytes, offsetof(AudioBufferOwner, TotalBytes) == 0x14);
AUDIO_ASSERT(AudioBufferOwner_Playback, offsetof(AudioBufferOwner, Playback) == 0x18);
AUDIO_ASSERT(AudioBufferOwner_ActiveReferences, offsetof(AudioBufferOwner, ActiveReferences) == 0x98);
AUDIO_ASSERT(attributes_size, sizeof(AudioAttributes) == 0x80);
AUDIO_ASSERT(channel_size_full, sizeof(AudioChannel) == 0x1C0);
AUDIO_ASSERT(channel_flags, offsetof(AudioChannel, Flags) == 0xA4);
AUDIO_ASSERT(channel_loop_count, offsetof(AudioChannel, LoopCount) == 0xA8);
AUDIO_ASSERT(channel_device, offsetof(AudioChannel, Device) == 0xAC);
AUDIO_ASSERT(channel_driver, offsetof(AudioChannel, Driver) == 0xD4);
AUDIO_ASSERT(channel_output, offsetof(AudioChannel, Output) == 0xD8);
AUDIO_ASSERT(channel_sample, offsetof(AudioChannel, Sample) == 0x168);
AUDIO_ASSERT(channel_format, offsetof(AudioChannel, Format) == 0x17C);
AUDIO_ASSERT(source_format, offsetof(AudioSource, DefaultFormat) == 0x44);
AUDIO_ASSERT(source_attributes, offsetof(AudioSource, Attributes) == 0x68);
AUDIO_ASSERT(source_groups, offsetof(AudioSource, GroupAttributes) == 0xE8);
AUDIO_ASSERT(source_channels, offsetof(AudioSource, ChannelList) == 0xF8);
AUDIO_ASSERT(source_references, offsetof(AudioSource, ListReferences) == 0x104);
AUDIO_ASSERT(source_driver, offsetof(AudioSource, Driver) == 0x1D0);
AUDIO_ASSERT(timer_size, sizeof(AudioTimer) == 0x58);
AUDIO_ASSERT(timer_delta, offsetof(AudioTimer, LastDelta) == 0x10);
AUDIO_ASSERT(timer_reset_limit, offsetof(AudioTimer, ResetLimit) == 0x20);
AUDIO_ASSERT(timer_previous, offsetof(AudioTimer, PreviousTime) == 0x28);
AUDIO_ASSERT(timer_peak, offsetof(AudioTimer, PeakDelta) == 0x30);
AUDIO_ASSERT(timer_samples, offsetof(AudioTimer, Samples) == 0x48);
AUDIO_ASSERT(timer_overruns, offsetof(AudioTimer, Overruns) == 0x4C);
#undef AUDIO_ASSERT

typedef char audio_source_size[(sizeof(AudioSource) == 0x1E8) ? 1 : -1];
typedef char audio_source_service_timer[(offsetof(AudioSource, ServiceTimer) == 0x110) ? 1 : -1];
typedef char audio_source_update_timer[(offsetof(AudioSource, UpdateTimer) == 0x170) ? 1 : -1];
