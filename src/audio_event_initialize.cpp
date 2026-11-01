// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_resource_internal.h"
extern SoundLink DAT_0087E180, DAT_0087E250;
extern AudioAttributes DAT_0087E1B8;
extern AudioIDXData * DAT_0087E294;
extern AudioSource * DAT_0087E29C;
extern AudioSystem * DAT_0087E298;
extern SoundPool * DAT_0087E2A8;
extern int DAT_0087E290, DAT_0087E28C, DAT_0087E2A0, DAT_00816108, DAT_0087E2A4;
extern char const DAT_008161E8[];
AudioSystem * __fastcall FUN_004019E0(AudioIDXData *, int, int, int);
SoundPool * __fastcall FUN_004074D0(int, int);
void __cdecl FUN_004068E0(char const *, ...);

// FUNCTION: GAMEMD 0x00403ED0
int __fastcall FUN_00403ED0(AudioSource * source, AudioIDXData * index)
{
    DAT_0087E180.FUN_004072C0();
    DAT_0087E250.FUN_004072C0();
    DAT_0087E294 = index;
    DAT_0087E29C = source;
    FUN_00401000(&DAT_0087E1B8);
    DAT_0087E290 = 0;
    DAT_0087E28C = 0;
    DAT_0087E2A0 = 1;
    DAT_00816108 = 1;
    if (DAT_0087E294) {
        DAT_0087E298 = FUN_004019E0(DAT_0087E294, 0x100000, 200, 8192);
        DAT_0087E2A8 = FUN_004074D0(300, 0x280);
        FUN_004068E0(DAT_008161E8, 0x2EE00);
    }
    if (DAT_0087E298 && DAT_0087E294 && DAT_0087E29C && DAT_0087E2A8) {
        DAT_0087E2A4 = 1;
        return 1;
    }
    return 0;
}
