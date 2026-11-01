// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_event_internal.h"
#include "audio_resource_internal.h"
#include "random.h"
#include <new.h>

namespace Randomizer { extern Random2Class Global; }

// FUNCTION: GAMEMD 0x004053E0
void AudioEvent::FUN_004053E0(void)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(this);
    if (!event->PauseLockCount && event->State == 2) event->Timer -= FUN_004093B0();
    ++event->PauseLockCount;
}

// FUNCTION: GAMEMD 0x00405430
void AudioEvent::FUN_00405430(void)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(this);
    if (event->PauseLockCount) {
        if (--event->PauseLockCount < 0) event->PauseLockCount = 0;
        if (!event->PauseLockCount && event->State == 2) event->Timer += FUN_004093B0();
    }
}

// FUNCTION: GAMEMD 0x00404FD0
void FUN_00404FD0(void)
{
    if (!DAT_0087E2A0) return;
    SoundLink * node = DAT_0087E180.Next;
    while (node != &DAT_0087E180) {
        AudioEventRuntime * event = static_cast<AudioEventRuntime *>(node);
        event->FUN_004053E0();
        node = event->Next;
    }
}

// FUNCTION: GAMEMD 0x00405040
void FUN_00405040(void)
{
    if (!DAT_0087E2A0) return;
    SoundLink * node = DAT_0087E180.Next;
    while (node != &DAT_0087E180) {
        AudioEventRuntime * event = static_cast<AudioEventRuntime *>(node);
        event->FUN_00405430();
        node = event->Next;
    }
}

// FUNCTION: GAMEMD 0x004059F0
int __fastcall FUN_004059F0(AudioChannel * channel)
{
    static_cast<AudioEventRuntime *>((AudioEvent *)channel->Stream)->State = 4;
    return 0;
}

static inline void SetDelayedEventState(AudioEventRuntime * event, int state)
{
    unsigned __int64 delay = event->Delay;
    event->NextState = state;
    if (event->PauseLockCount) event->Timer = delay;
    else event->Timer = FUN_004093B0() + delay;
    event->State = 2;
    event->Delay = 0;
}

static inline void SetEventState(AudioEventRuntime * event, int state)
{
    if (event->Delay) SetDelayedEventState(event, state);
    else event->State = state;
}

// FUNCTION: GAMEMD 0x00405A00
int __fastcall FUN_00405A00(AudioChannel * channel)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>((AudioEvent *)channel->Stream);
    event->Flags &= ~2U;
    int state = 4;
    if (event->State == 4) return 0;
    if (event->PlaybackBuffer || event->SampleSelectionCount || event->Delay) state = 1;
    else event->Delay = 0;
    SetEventState(event, state);
    return 0;
}

static inline unsigned SampleDelay(VocClass * type)
{
    int maximum = type->MaxDelay;
    int minimum = type->MinDelay;
    return Randomizer::Global(minimum, maximum);
}

// FUNCTION: GAMEMD 0x00405AC0
int __fastcall FUN_00405AC0(AudioChannel * channel)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>((AudioEvent *)channel->Stream);
    AudioData * sample = FUN_004047B0(event);
    if ((unsigned)event->EventType->MaxDelay < 33U) {
        channel->FUN_00402790(sample);
        return 0;
    }
    channel->FUN_00402790(NULL);
    VocClass * type = event->EventType;
    if (!(type->Control & 0x88) || sample || event->SampleSelectionCount) {
        int delay = SampleDelay(type);
        if ((unsigned)delay < 33U) {
            channel->FUN_00402790(sample);
            return 0;
        }
        event->Delay = delay;
        event->PlaybackBuffer = sample;
    }
    return 0;
}

// FUNCTION: GAMEMD 0x004052F0
void AudioEvent::FUN_004052F0(void)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(this);
    if (!(event->Flags & 1)) {
        if (event->Channel && this == (AudioEvent *)event->Channel->Stream) {
            if (event->Flags & 2) event->Channel->FUN_004025B0();
            event->Channel->FUN_004026F0();
            event->Channel->Stream = NULL;
            event->Channel->StopB = NULL;
            event->Channel->unknown_B4 = NULL;
            event->Channel->StopA = NULL;
            event->Channel->Mixer[0] = NULL;
            event->Channel->Mixer[2] = NULL;
            event->Channel->Mixer[3] = NULL;
            event->Channel = NULL;
        }
        for (int i = 0; i < event->SampleHandleCount; ++i) {
            FUN_00401F90((AudioSampleRecord *)event->SampleHandles[i]);
            event->SampleHandles[i] = NULL;
        }
        event->SampleHandleCount = 0;
        event->State = 4;
        event->Stamp = 0;
        event->Controller = NULL;
        event->Flags |= 1;
    }
}

// FUNCTION: GAMEMD 0x00404E70
void FUN_00404E70(void)
{
    if (!DAT_0087E2A0) return;
    SoundLink * node = DAT_0087E180.Next;
    while (node != &DAT_0087E180) {
        AudioEventRuntime * event = static_cast<AudioEventRuntime *>(node);
        SoundLink * next = event->Next;
        if (!(event->Flags & 1)) {
            if (event->Channel && event == (AudioEvent *)event->Channel->Stream) {
                if (event->Flags & 2) event->Channel->FUN_004025B0();
                event->Channel->FUN_004026F0();
                event->Channel->Stream = NULL;
                event->Channel->StopB = NULL;
                event->Channel->unknown_B4 = NULL;
                event->Channel->StopA = NULL;
                event->Channel->Mixer[0] = NULL;
                event->Channel->Mixer[2] = NULL;
                event->Channel->Mixer[3] = NULL;
                event->Channel = NULL;
            }
            for (int i = 0; i < event->SampleHandleCount; ++i) {
                FUN_00401F90((AudioSampleRecord *)event->SampleHandles[i]);
                event->SampleHandles[i] = NULL;
            }
            event->SampleHandleCount = 0;
            event->State = 4;
            event->Stamp = 0;
            event->Controller = NULL;
            event->Flags |= 1;
        }

        event->FUN_00407450();
        event->Stamp = 0;
        if (event->Flags & 0x80) ::operator delete(event);
        else DAT_0087E2A8->FUN_00407530((SoundNode *)event);
        --DAT_0087E28C;
        node = next;
    }
}
