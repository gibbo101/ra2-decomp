// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_event_internal.h"
#include "audio_resource_internal.h"
#include "random.h"
namespace Randomizer { extern Random2Class Global; }

// FUNCTION: GAMEMD 0x00404700
AudioData * __fastcall FUN_00404700(AudioEvent * handle)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(handle);
    int count = event->SampleHandleCount;
    int first = 0;
    event->PlaybackRemaining = count;
    if (count && !(event->Flags & 0x20)) {
        VocClass * type = event->EventType;
        if ((unsigned)type->MinDelay < 0x21) {
            if (type->Control & 0x20) {
                --event->PlaybackRemaining;
                first = 1;
                event->PlaybackIndex = 0;
            } else {
                event->PlaybackIndex = -1;
            }
            if (type->Control & 0x40) --event->PlaybackRemaining;
            if (event->PlaybackRemaining <= 0) {
                type->SamplesOK = 0;
                return 0;
            }
            for (int i = 0; i < event->PlaybackRemaining; ++i, ++first)
                event->PlaybackOrder[i] = first;
            if ((event->Flags & 8) || !(event->EventType->Control & 0x20))
                return FUN_004047B0(event);
        }
        return FUN_00402030((AudioSampleRecord *)event->SampleHandles[0]);
    }
    return 0;
}
