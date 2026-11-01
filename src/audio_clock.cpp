// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audio_support.h"
#include "audio_clock.h"
#include <windows.h>
#include <mmsystem.h>
extern __int64 DAT_0087E828;
extern unsigned __int64 (*DAT_0087E848)(void);
// Game and audio-worker calls share the fallback clock and its re-entry counter.
extern volatile unsigned DAT_0087E84C;
extern unsigned * DAT_007E1760;
extern unsigned * DAT_007E1764;
extern volatile unsigned __int64 DAT_00816320;
unsigned __int64 FUN_004093C0(void);
unsigned __int64 FUN_004093F0(void);
// FUNCTION: GAMEMD 0x00409360
void FUN_00409360(void)
{
    LARGE_INTEGER frequency;
    if (QueryPerformanceFrequency(&frequency)) {
        DAT_0087E828 = frequency.QuadPart / (__int64)1000;
        DAT_0087E848 = FUN_004093C0;
    } else DAT_0087E848 = FUN_004093F0;
}
// FUNCTION: GAMEMD 0x004093B0
unsigned __int64 __fastcall Game::AudioGetTime(void)
{
    return DAT_0087E848 ? DAT_0087E848() : 0;
}
// FUNCTION: GAMEMD 0x004093C0
unsigned __int64 FUN_004093C0(void)
{
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return count.QuadPart / DAT_0087E828;
}
// FUNCTION: GAMEMD 0x004093F0
unsigned __int64 FUN_004093F0(void)
{
    unsigned now, low, high, called;
    ++DAT_0087E84C;
retry:
    called = DAT_0087E84C;
    high = *DAT_007E1764;
    low = *DAT_007E1760;
    if (called != DAT_0087E84C) goto retry;
reread:
    now = timeGetTime();
    if (now < low) ++high;
    *DAT_007E1764 = high;
    *DAT_007E1760 = now;
    if (called != DAT_0087E84C) {
        called = DAT_0087E84C;
        goto reread;
    }
    return DAT_00816320;
}
