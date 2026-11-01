// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once
#include <stddef.h>
struct AudioRamp {
    unsigned Flags;
    int Current;
    unsigned Target;
    int Rate;
    unsigned __int64 Duration, LastTime;
    unsigned int : 32;
    unsigned int : 32;
};
void __fastcall FUN_00407170(AudioRamp * ramp, int value);
int __fastcall FUN_004071C0(AudioRamp * ramp);
typedef char audio_ramp_size[(sizeof(AudioRamp) == 0x28) ? 1 : -1];
typedef char audio_ramp_duration[(offsetof(AudioRamp, Duration) == 0x10) ? 1 : -1];
typedef char audio_ramp_last_time[(offsetof(AudioRamp, LastTime) == 0x18) ? 1 : -1];
