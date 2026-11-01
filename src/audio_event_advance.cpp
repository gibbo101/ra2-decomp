// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_event_internal.h"
#include "audio_resource_internal.h"
#include "random.h"
namespace Randomizer { extern Random2Class Global; }

// FUNCTION: GAMEMD 0x004047B0
AudioData * __fastcall FUN_004047B0(AudioEvent * handle)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(handle);
    VocClass * type = event->EventType;
    unsigned control = type->Control;
    if ((unsigned)type->MinDelay >= 0x21 && !(event->Flags & 0x20)) return 0;
    if (event->PlaybackRemaining && !(event->Flags & 0x20)) {
        if (control & 2) {
            int index = Randomizer::Global(0, event->PlaybackRemaining - 1);
            event->PlaybackIndex = event->PlaybackOrder[index];
            for (int i = index; i < event->PlaybackRemaining - 1; ++i)
                event->PlaybackOrder[i] = event->PlaybackOrder[i + 1];
        } else {
            ++event->PlaybackIndex;
        }
        --event->PlaybackRemaining;
        return FUN_00402030((AudioSampleRecord *)event->SampleHandles[event->PlaybackIndex]);
    }
    if ((control & 1) && !(event->Flags & 0x20)) {
        if (!type->Loop || event->PlaybackCycle < type->Loop - 1) {
            ++event->PlaybackCycle;
            return FUN_00404700(event);
        }
    }
    if ((control & 0x40) && !(event->Flags & 0x10) && event->SampleHandleCount >= 1) {
        event->Flags |= 0x10;
        return FUN_00402030((AudioSampleRecord *)event->SampleHandles[event->SampleHandleCount - 1]);
    }
    return 0;
}
