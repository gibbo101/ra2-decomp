// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_event_internal.h"
#include "audio_resource_internal.h"
#include <string.h>

void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: GAMEMD 0x004064A0
int __fastcall VocClass::FUN_004064A0(char const * name)
{
    if (NumSamples == 32) return 0;
    if (DAT_0087E2A0 && DAT_0087E294) {
        while (*name == '$' || *name == '#') ++name;
        int index = DAT_0087E294->FindSampleIndex(name);
        if (index != -1) {
            SampleIndex[NumSamples] = index;
            ++NumSamples;
            return 1;
        }
        FUN_004068E0("Missing sample %s\n", name);
        SamplesOK = 0;
    }
    return 1;
}
