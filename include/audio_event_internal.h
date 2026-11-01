// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once
#include "voc.h"
#include "audio_stream.h"

struct AudioEventRuntime : public AudioEvent, public SoundLink {
    SoundLink TypeEventNode;
    // Playback callbacks and the game thread share event state and timing.
    volatile unsigned Flags;
    volatile int State;
    int NextState;
    VocClass * EventType;
    void * SampleHandles[32];
    int SampleHandleCount, PlaybackIndex;
    AudioChannel * Channel;
    unsigned int : 32;
    AudioRamp VolumeRamp, UnknownRamp, PanningRamp;
    unsigned int : 32;
    unsigned int : 32;
    unsigned Stamp;
    volatile unsigned Delay;
    volatile unsigned __int64 Timer;
    AudioData * volatile PlaybackBuffer;
    int PlaybackVolume, PlaybackAttenuationPercent, PriorityAdjust;
    volatile int PauseLockCount;
    unsigned int : 32;
    int PlaybackOrder[32];
    int PlaybackRemaining, PlaybackCycle;
    int SampleSelection[32];
    int SampleSelectionCount, SampleSelectionCycle, SampleSelectionCursor;
    int StoredRandomSampleIndex;
    AudioController * Controller;
};


struct SoundPool;
struct AudioSystem;
struct AudioIDXData;
struct AudioPriorityRow { SoundLink Headers[10]; };
extern AudioPriorityRow DAT_0087DE38[7];
extern SoundLink DAT_0087E180, DAT_0087E250;
extern AudioAttributes DAT_0087E1B8;
extern int DAT_00816108;
extern unsigned DAT_0081610C;
extern int DAT_0087E28C, DAT_0087E290;
extern AudioSystem * DAT_0087E298;
extern AudioSource * DAT_0087E29C;
extern int DAT_0087E2A0, DAT_0087E2A4;
extern SoundPool * DAT_0087E2A8;
extern unsigned DAT_0087E2AC, DAT_0087E2B0;
extern int DAT_0087E2B4;
extern AudioAttributes * DAT_0087E748, * DAT_0087E758;

void __fastcall FUN_00407150(AudioRamp * ramp, int value);
AudioChannel * __fastcall FUN_004035F0(AudioSource * source, int group);
void __fastcall FUN_00404140(AudioEvent * event);
void FUN_004041D0(void);
AudioData * __fastcall FUN_00404700(AudioEvent * event);
AudioData * __fastcall FUN_004047B0(AudioEvent * event);
void __fastcall FUN_004048B0(AudioEvent * event);
int __fastcall FUN_00404BB0(AudioEvent * event);
void __fastcall FUN_00404D80(AudioEvent * event);
void __fastcall FUN_00404DD0(AudioEvent * event);
VocClass * __fastcall FUN_00404E20(int rows);
void FUN_00404E70(void);
void FUN_00404FD0(void);
void FUN_00405040(void);
int __fastcall FUN_004054A0(AudioEvent * event);
void __fastcall FUN_004055C0(AudioEvent * event);
int __fastcall FUN_004059F0(AudioChannel * channel);
int __fastcall FUN_00405A00(AudioChannel * channel);
int __fastcall FUN_00405AC0(AudioChannel * channel);
AudioChannel * __fastcall FUN_00405B50(int priority, int persistent);
VocClass * __fastcall FUN_004063B0(char const * name);

#define AUDIO_EVENT_ASSERT(tag, expression) typedef char audio_event_assert_##tag[(expression) ? 1 : -1]
AUDIO_EVENT_ASSERT(size, sizeof(AudioEventRuntime) == 0x280);
AUDIO_EVENT_ASSERT(TypeEventNode, offsetof(AudioEventRuntime, TypeEventNode) == 0xC);
AUDIO_EVENT_ASSERT(Flags, offsetof(AudioEventRuntime, Flags) == 0x18);
AUDIO_EVENT_ASSERT(State, offsetof(AudioEventRuntime, State) == 0x1C);
AUDIO_EVENT_ASSERT(NextState, offsetof(AudioEventRuntime, NextState) == 0x20);
AUDIO_EVENT_ASSERT(EventType, offsetof(AudioEventRuntime, EventType) == 0x24);
AUDIO_EVENT_ASSERT(SampleHandles, offsetof(AudioEventRuntime, SampleHandles) == 0x28);
AUDIO_EVENT_ASSERT(SampleHandleCount, offsetof(AudioEventRuntime, SampleHandleCount) == 0xA8);
AUDIO_EVENT_ASSERT(PlaybackIndex, offsetof(AudioEventRuntime, PlaybackIndex) == 0xAC);
AUDIO_EVENT_ASSERT(Channel, offsetof(AudioEventRuntime, Channel) == 0xB0);
AUDIO_EVENT_ASSERT(VolumeRamp, offsetof(AudioEventRuntime, VolumeRamp) == 0xB8);
AUDIO_EVENT_ASSERT(UnknownRamp, offsetof(AudioEventRuntime, UnknownRamp) == 0xE0);
AUDIO_EVENT_ASSERT(PanningRamp, offsetof(AudioEventRuntime, PanningRamp) == 0x108);
AUDIO_EVENT_ASSERT(Stamp, offsetof(AudioEventRuntime, Stamp) == 0x138);
AUDIO_EVENT_ASSERT(Delay, offsetof(AudioEventRuntime, Delay) == 0x13C);
AUDIO_EVENT_ASSERT(Timer, offsetof(AudioEventRuntime, Timer) == 0x140);
AUDIO_EVENT_ASSERT(PlaybackBuffer, offsetof(AudioEventRuntime, PlaybackBuffer) == 0x148);
AUDIO_EVENT_ASSERT(PlaybackVolume, offsetof(AudioEventRuntime, PlaybackVolume) == 0x14C);
AUDIO_EVENT_ASSERT(PlaybackAttenuationPercent, offsetof(AudioEventRuntime, PlaybackAttenuationPercent) == 0x150);
AUDIO_EVENT_ASSERT(PriorityAdjust, offsetof(AudioEventRuntime, PriorityAdjust) == 0x154);
AUDIO_EVENT_ASSERT(PauseLockCount, offsetof(AudioEventRuntime, PauseLockCount) == 0x158);
AUDIO_EVENT_ASSERT(PlaybackOrder, offsetof(AudioEventRuntime, PlaybackOrder) == 0x160);
AUDIO_EVENT_ASSERT(PlaybackRemaining, offsetof(AudioEventRuntime, PlaybackRemaining) == 0x1E0);
AUDIO_EVENT_ASSERT(PlaybackCycle, offsetof(AudioEventRuntime, PlaybackCycle) == 0x1E4);
AUDIO_EVENT_ASSERT(SampleSelection, offsetof(AudioEventRuntime, SampleSelection) == 0x1E8);
AUDIO_EVENT_ASSERT(SampleSelectionCount, offsetof(AudioEventRuntime, SampleSelectionCount) == 0x268);
AUDIO_EVENT_ASSERT(SampleSelectionCycle, offsetof(AudioEventRuntime, SampleSelectionCycle) == 0x26C);
AUDIO_EVENT_ASSERT(SampleSelectionCursor, offsetof(AudioEventRuntime, SampleSelectionCursor) == 0x270);
AUDIO_EVENT_ASSERT(StoredRandomSampleIndex, offsetof(AudioEventRuntime, StoredRandomSampleIndex) == 0x274);
AUDIO_EVENT_ASSERT(Controller, offsetof(AudioEventRuntime, Controller) == 0x278);
AUDIO_EVENT_ASSERT(VocClass_ActiveEventCount, offsetof(VocClass, unknown_44) == 0x44);
AUDIO_EVENT_ASSERT(VocClass_ActiveEvents, offsetof(VocClass, unknown_8C) == 0x8C);
AUDIO_EVENT_ASSERT(VocClass_ActiveEventsFrame, offsetof(VocClass, unknown_98) == 0x98);
AUDIO_EVENT_ASSERT(VocClass_PriorityGridFrame, offsetof(VocClass, unknown_9C) == 0x9C);
AUDIO_EVENT_ASSERT(VocClass_HighestPriority, offsetof(VocClass, unknown_A0) == 0xA0);
AUDIO_EVENT_ASSERT(VocClass_PriorityGridNode, offsetof(VocClass, unknown_A4) == 0xA4);
AUDIO_EVENT_ASSERT(VocClass_LoudnessRank, offsetof(VocClass, unknown_B0) == 0xB0);
AUDIO_EVENT_ASSERT(VocClass_SampleIndex, offsetof(VocClass, SampleIndex) == 0xB4);
AUDIO_EVENT_ASSERT(VocClass_NumSamples, offsetof(VocClass, NumSamples) == 0x134);
AUDIO_EVENT_ASSERT(VocClass_size, sizeof(VocClass) == 0x148);
#undef AUDIO_EVENT_ASSERT
