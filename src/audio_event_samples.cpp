// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_event_internal.h"
#include "audio_resource_internal.h"
#include "random.h"
namespace Randomizer { extern Random2Class Global; }


static inline void LoadHandle(AudioEventRuntime * event, int index)
{
    event->SampleHandles[event->SampleHandleCount] =
        FUN_00401C00(DAT_0087E298, event->EventType->SampleIndex[index]);
    if (event->SampleHandles[event->SampleHandleCount]) {
        FUN_00401F80((AudioSampleRecord *)event->SampleHandles[event->SampleHandleCount]);
        ++event->SampleHandleCount;
    }
}

// FUNCTION: GAMEMD 0x004048B0
void __fastcall FUN_004048B0(AudioEvent * handle)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(handle);
    VocClass * type = event->EventType;
    if (event->SampleHandleCount) {
        if ((unsigned)type->MinDelay < 0x21) return;
        for (int i = 0; i < event->SampleHandleCount; ++i) {
            FUN_00401F90((AudioSampleRecord *)event->SampleHandles[i]);
            event->SampleHandles[i] = 0;
        }
        event->SampleHandleCount = 0;
    }
    if ((unsigned)type->MinDelay < 0x21) {
        if (type->Attack) LoadHandle(event, Randomizer::Global(0, type->Attack - 1));
        if (type->Control & 4) {
            for (int i = type->Attack; i < type->NumSamples - type->Decay; ++i)
                LoadHandle(event, i);
        } else if (type->Control & 2) {
            int maximum = type->NumSamples - type->Decay - 1;
            int minimum = type->Attack;
            LoadHandle(event, Randomizer::Global(minimum, maximum));
        } else {
            LoadHandle(event, type->Attack);
        }
        if (type->Decay) {
            int maximum = type->NumSamples - 1;
            int minimum = type->NumSamples - type->Decay;
            LoadHandle(event, Randomizer::Global(minimum, maximum));
        }
    } else {
        int tail = -1;
        if (type->Decay && (type->Control & 0x40)) {
            int maximum = type->NumSamples - 1;
            int minimum = type->NumSamples - type->Decay;
            tail = Randomizer::Global(minimum, maximum);
        }
        int index = FUN_00404BB0(event);
        if (index >= 0) {
            LoadHandle(event, index);
            if (tail != -1) LoadHandle(event, tail);
        } else if (tail != -1 && !(event->Flags & 0x10)) {
            LoadHandle(event, tail);
            event->Flags |= 0x10;
        }
    }
}
