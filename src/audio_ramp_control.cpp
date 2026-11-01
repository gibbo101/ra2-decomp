// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audio_support.h"
#include "audio_clock.h"
// FUNCTION: GAMEMD 0x00407060
void __fastcall FUN_00407060(AudioRamp * ramp) { FUN_00407170(ramp, 0x4000); }
// FUNCTION: GAMEMD 0x00407070
void __fastcall FUN_00407070(AudioRamp * ramp) { FUN_00407170(ramp, 0); }
// FUNCTION: GAMEMD 0x00407080
void __fastcall FUN_00407080(AudioRamp * ramp, int value) { FUN_00407170(ramp, value); }
// FUNCTION: GAMEMD 0x00407090
int __fastcall FUN_00407090(AudioRamp * ramp) { return ramp->Current != ramp->Target; }
// FUNCTION: GAMEMD 0x004070B0
void __fastcall FUN_004070B0(AudioRamp * ramp)
{
    unsigned __int64 deadline = Game::AudioGetTime() + ramp->Duration;
    while (ramp->Current != ramp->Target) {
        if (Game::AudioGetTime() >= deadline) return;
        Game::CallBack();
    }
}
// FUNCTION: GAMEMD 0x00407100
void __fastcall FUN_00407100(AudioRamp * ramp, int value)
{
    ramp->Flags = 0;
    ramp->LastTime = Game::AudioGetTime();
    ramp->Rate = (0x4000 << 16) / 1000U;
    ramp->Duration = 1000;
    ramp->Flags |= 1;
    ramp->Target = value << 16;
    FUN_004071C0(ramp);
}
