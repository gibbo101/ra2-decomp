// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_stream.h"
// FUNCTION: GAMEMD 0x00403820
void AudioTimer::FUN_00403820(void) volatile
{
    Duration = 0;
    Threshold = 0;
    LastDelta = 0;
    MaximumDelta = 0;
    PreviousTime = 0;
    Samples = 0;
    DisplayedSamples = 0;
    ReportInterval = 5000;
    PeakDelta = 0;
    LastReport = 0;
    DisplayIndex = 0;
    Overruns = 0;
}
// FUNCTION: GAMEMD 0x00403870
void AudioTimer::FUN_00403870(unsigned __int64 value) volatile
{
    Duration = value;
    Threshold = value;
    ResetLimit = Duration * 10;
}
// FUNCTION: GAMEMD 0x004038A0
void AudioTimer::FUN_004038A0(unsigned __int64 value) volatile { Threshold = value; }
// FUNCTION: GAMEMD 0x004038C0
void AudioTimer::FUN_004038C0(unsigned __int64 value) volatile { ResetLimit = value; }
// FUNCTION: GAMEMD 0x004038E0
int AudioTimer::FUN_004038E0(unsigned __int64 value) volatile
{
    if (value - PreviousTime < Duration) return 0;
    return 1;
}
// FUNCTION: GAMEMD 0x00403920
void AudioTimer::FUN_00403920(unsigned __int64 value) volatile
{
    unsigned __int64 delta = value - PreviousTime;
    LastDelta = delta;
    ++Samples;
    if (MaximumDelta > ResetLimit) MaximumDelta = 0;
    if (delta > MaximumDelta) MaximumDelta = delta;
    if (delta > Threshold) ++Overruns;
    if (delta > PeakDelta) PeakDelta = delta;
    PreviousTime = value;
}
