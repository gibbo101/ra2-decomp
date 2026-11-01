// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audio_device_internal.h"
#include "audio_resource_internal.h"
#include "mixfile.h"
extern AudioSource * DAT_0087E728;
extern AudioIDXData * DAT_0087E724;
extern MixFileClass * DAT_0087E734, * DAT_0087E738;
void __fastcall FUN_00403730(AudioSource *, int);
void FUN_00404E70(void);
void FUN_004081A0(void);
void FUN_00752340(void);
void FUN_00752B40(void);
void FUN_00403FA0(void);
void FUN_00407780(void);
void __fastcall FUN_004033C0(AudioSource *);
void FUN_00402990(void);

// FUNCTION: GAMEMD 0x00406D40
void FUN_00406D40(void)
{
    if (DAT_0087E728) FUN_00403730(DAT_0087E728, 1);
    FUN_00404E70();
    FUN_004081A0();
    FUN_00752340();
    FUN_00752B40();
    FUN_00403FA0();
    FUN_00407780();
    if (DAT_0087E724) {
        DAT_0087E724->~AudioIDXData();
        DAT_0087E724 = NULL;
    }
    if (DAT_0087E734) {
        delete DAT_0087E734;
        DAT_0087E734 = NULL;
    }
    if (DAT_0087E738) {
        delete DAT_0087E738;
        DAT_0087E738 = NULL;
    }
    if (DAT_0087E728) {
        FUN_004033C0(DAT_0087E728);
        DAT_0087E728 = NULL;
    }
    FUN_00402990();
}
