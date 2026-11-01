// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "audio_event_internal.h"
#include "audioctl.h"
#include "audio_clock.h"
#include "random.h"
namespace Randomizer { extern Random2Class Global; }
extern AudioIDXData * DAT_0087E294;
struct AudioSampleRecord;
void __fastcall FUN_00401F90(AudioSampleRecord *);

static inline AudioEventRuntime * GetLinkedEvent(AudioController * controller)
{
    if (!DAT_0087E2A0 || controller->AudioIndex != (void **)&DAT_0087E294) return NULL;
    AudioEventRuntime * linked = static_cast<AudioEventRuntime *>((AudioEvent *)controller->Event);
    if (linked && (controller->EventType != linked->EventType || controller->Stamp != linked->Stamp)) {
        linked = NULL;
        controller->Event = NULL;
    }
    return linked;
}

static inline void FinishEvent(AudioEventRuntime * event)
{
    if (!(event->Flags & 1)) {
        FUN_00404140(event);
        for (int index = 0; index < event->SampleHandleCount; ++index) {
            FUN_00401F90((AudioSampleRecord *)event->SampleHandles[index]);
            event->SampleHandles[index] = NULL;
        }
        event->SampleHandleCount = 0;
        event->State = 4;
        event->Stamp = 0;
        event->Controller = NULL;
        event->Flags |= 1;
    }
}
// FUNCTION: GAMEMD 0x004055C0
void __fastcall FUN_004055C0(AudioEvent * handle)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(handle);
    if (!DAT_00816108) event->State = 4;
again:
    switch (event->State) {
    case 2:
        if (event->Flags & 0x40) goto advance;
        if (!event->PauseLockCount && Game::AudioGetTime() > event->Timer) {
            event->State = event->NextState;
            goto again;
        }
        break;
    case 0:
        {
            VocClass * type = event->EventType;
            if (type->NumSamples && type->SamplesOK) {
                event->PlaybackCycle = 0;
                event->Channel = FUN_00405B50(event->PriorityAdjust + type->Priority, 0);
                if (event->Channel) {
                    int maximumPitch = type->MaxFDelta;
                    int minimumPitch = type->MinFDelta;
                    event->PlaybackVolume = Randomizer::Global(minimumPitch, maximumPitch) + 100;
                    int maximumAttenuation = type->VShift;
                    event->PlaybackAttenuationPercent = Randomizer::Global(0, maximumAttenuation);
                    event->Channel->FUN_004026C0();
                    event->Channel->Stream = (AudioStream *)event;
                    event->Channel->unknown_B4 = FUN_00405AC0;
                    event->Channel->StopA = FUN_00405A00;
                    event->Channel->StopB = FUN_004059F0;
                    event->State = 1;
                    event->Channel->Mixer[0] = (AudioAttributes *)&event->VolumeRamp;
                    event->Channel->Mixer[2] = &DAT_0087E1B8;
                    event->Channel->Mixer[3] = DAT_0087E758;
                    if (type->Control & 0x88) {
                        int minimum = (type->Control & 0x80) ? 33 : type->MinDelay;
                        int maximum = type->MaxDelay;
                        int delay = Randomizer::Global(minimum, maximum);
                        if (delay >= 33) {
                            __int64 time = delay;
                            event->NextState = 1;
                            if (event->PauseLockCount) event->Timer = time;
                            else event->Timer = Game::AudioGetTime() + time;
                            event->State = 2;
                        }
                    }
                    break;
                }
            }
            if (!(event->Flags & 1)) {
                FUN_00404140(event);
                FUN_00404D80(event);
                event->State = 4;
                event->Stamp = 0;
                event->Controller = NULL;
                event->Flags |= 1;
            }
        }
        return;
    case 3:
        if ((event->EventType->Control & 1) && event->EventType->Loop == 0 && !(event->Flags & 0x20)) {
            AudioController * controller = event->Controller;
            if (!controller || GetLinkedEvent(controller) != event) FinishEvent(event);
        }
        if (!(event->Flags & 0x40)) break;
advance:
        {
            AudioChannel * channel = event->Channel;
            if (channel) {
                channel->FUN_004026A0();
                event->Flags &= ~0x40;
                event->Channel->FUN_004025B0();
                if ((event->EventType->Control & 0x40) && !(event->Flags & 0x10)) {
                    event->State = 1;
                    event->PlaybackBuffer = FUN_004047B0(event);
                    if (!event->PauseLockCount && !FUN_004054A0(event)) FinishEvent(event);
                }
                channel->FUN_004026B0();
            } else event->Flags &= ~0x40;
        }
        break;
    case 4:
        FinishEvent(event);
        return;
    default:
        break;
    }
}
