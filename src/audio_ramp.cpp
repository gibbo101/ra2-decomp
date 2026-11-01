// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audio_support.h"
#include "audio_clock.h"
// FUNCTION: GAMEMD 0x00407150
void __fastcall FUN_00407150(AudioRamp * ramp, int value)
{
    ramp->Flags |= 1;
    ramp->Target = value << 16;
}
// FUNCTION: GAMEMD 0x00407160
void __fastcall FUN_00407160(AudioRamp * ramp) { ramp->Flags |= 1; }
// FUNCTION: GAMEMD 0x00407170
void __fastcall FUN_00407170(AudioRamp * ramp, int value)
{
    ramp->Flags &= ~1;
    if (ramp->Target == ramp->Current) ramp->LastTime = Game::AudioGetTime();
    ramp->Target = value << 16;
}
// FUNCTION: GAMEMD 0x004071A0
void __fastcall FUN_004071A0(AudioRamp * ramp, int range, unsigned __int64 duration)
{
    ramp->Rate = (range << 16) / (unsigned)duration;
    ramp->Duration = duration;
}
// FUNCTION: GAMEMD 0x004071C0
int __fastcall FUN_004071C0(AudioRamp * ramp)
{
    int dif, delta;
    unsigned __int64 time, thisTime;
    if ((dif = (ramp->Target - ramp->Current))) {
        if (ramp->Flags & 1) ramp->Current = ramp->Target;
        else {
            thisTime = Game::AudioGetTime();
            time = thisTime - ramp->LastTime;
            ramp->LastTime = thisTime;
            if (time > ramp->Duration) time = ramp->Duration;
            delta = ramp->Rate * (unsigned)time;
            if (dif < 0) {
                if (delta > -dif) ramp->Current += dif;
                else ramp->Current -= delta;
            } else {
                if (delta > dif) ramp->Current += dif;
                else ramp->Current += delta;
            }
        }
        ramp->Flags |= 2;
        return 1;
    }
    return 0;
}
// FUNCTION: GAMEMD 0x004072B0
int __fastcall FUN_004072B0(AudioRamp * ramp) { return ramp->Target >> 16; }
