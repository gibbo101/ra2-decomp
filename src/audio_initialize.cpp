// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audio_device_internal.h"
#include "audio_resource_internal.h"
#include "ccfile.h"
#include "mixfile.h"
extern PKey DAT_00886980;
extern AudioAttributes DAT_0087E2C0, DAT_0087E4E0, DAT_0087E578, DAT_0087E450;
extern AudioAttributes DAT_0087E340, DAT_0087E6A0, DAT_0087E5F8, DAT_0087E3C0;
extern AudioAttributes * DAT_0087E73C, * DAT_0087E744, * DAT_0087E740, * DAT_0087E748;
extern AudioAttributes * DAT_0087E74C, * DAT_0087E754, * DAT_0087E750, * DAT_0087E758;
extern AudioDriverCallbacks * DAT_00816514;
extern AudioSource * DAT_0087E728;
extern AudioIDXData * DAT_0087E724;
extern MixFileClass * DAT_0087E734;
extern int DAT_0087E72C;
void FUN_00409360(void);
int FUN_00402940(void);
int __fastcall FUN_00403530(AudioSource *, int);
int FUN_00407550(void);
int __fastcall FUN_00403ED0(AudioSource *, AudioIDXData *);
int FUN_00752290(void);
int FUN_00752AD0(void);

// FUNCTION: GAMEMD 0x00406B10
int __fastcall FUN_00406B10(int enabled)
{
    FUN_00409360();
    FUN_00401000(&DAT_0087E2C0);
    DAT_0087E73C = &DAT_0087E2C0;
    FUN_00401000(&DAT_0087E4E0);
    DAT_0087E744 = &DAT_0087E4E0;
    FUN_00401000(&DAT_0087E578);
    DAT_0087E740 = &DAT_0087E578;
    FUN_00401000(&DAT_0087E450);
    DAT_0087E748 = &DAT_0087E450;
    FUN_00401000(&DAT_0087E340);
    DAT_0087E74C = &DAT_0087E340;
    FUN_00401000(&DAT_0087E6A0);
    DAT_0087E754 = &DAT_0087E6A0;
    FUN_00401000(&DAT_0087E5F8);
    DAT_0087E750 = &DAT_0087E5F8;
    FUN_00401000(&DAT_0087E3C0);
    DAT_0087E758 = &DAT_0087E3C0;
    if (enabled) {
        FUN_00402940();
        FUN_00402AF0(DAT_00816514);
        AudioFormat format;
        format.FUN_004085A0();
        format.SampleRate = 22050;
        format.Compression = 0;
        format.Channels = 2;
        format.BytesPerSample = 2;
        format.Format = 4;
        format.FUN_004085B0();
        DAT_0087E728 = FUN_00402C70(-1, &format);
        if (!DAT_0087E728 || FUN_00403530(DAT_0087E728, 16) != 16) enabled = 0;
        else {
            DAT_0087E73C = &DAT_0087E728->Attributes;
            if (CCFileClass("AUDIOMD.MIX").Is_Available()) {
                DAT_0087E734 = new MixFileClass("AUDIOMD.MIX", &DAT_00886980);
            } else {
                DAT_0087E734 = new MixFileClass("AUDIO.MIX", &DAT_00886980);
            }
            DAT_0087E724 = AudioIDXData::Create("audio", NULL);
            if (!DAT_0087E724) enabled = 0;
        }
    }
    FUN_00407550();
    if (!FUN_00403ED0(DAT_0087E728, DAT_0087E724)) enabled = 0;
    FUN_00752290();
    FUN_00752AD0();
    DAT_0087E72C = enabled;
    return 1;
}
