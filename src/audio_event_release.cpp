// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Derived from gagarin's GPL-3.0-or-later small_misc45_methods.cpp.
#include "always.h"
#include "audio_event_internal.h"
#include "audio_resource_internal.h"
#include "random.h"
namespace Randomizer { extern Random2Class Global; }

// FUNCTION: GAMEMD 0x00404D80
void __fastcall FUN_00404D80(AudioEvent * handle)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(handle);
    int i = 0;
    if (i < event->SampleHandleCount) {
        void ** slot = event->SampleHandles;
        do {
            FUN_00401F90((AudioSampleRecord *)*slot);
            *slot = 0;
            ++i;
            ++slot;
        } while (i < event->SampleHandleCount);
    }
    event->SampleHandleCount = 0;
}
