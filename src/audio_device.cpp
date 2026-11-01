// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_device_internal.h"
// FUNCTION: GAMEMD 0x00402900
void FUN_00402900(void)
{
    if (!DAT_0087DE2C) return;
    if (DAT_00815E98->References) return;
    SoundLink * head = &DAT_00815E98->Devices;
    for (AudioSource * device = (AudioSource *)head->Next; device != head; device = (AudioSource *)device->Next) {
        if (!(device->Flags & 2)) device->Service(device);
    }
}
// FUNCTION: GAMEMD 0x00402940
int FUN_00402940(void)
{
    DAT_00815E98->Drivers.FUN_004072C0();
    DAT_00815E98->Devices.FUN_004072C0();
    DAT_00815E98->References = 0;
    DAT_0087DE30 = &DAT_00815E98->Attributes;
    FUN_00401000(DAT_0087DE30);
    DAT_0087DE2C = 1;
    return 0;
}
