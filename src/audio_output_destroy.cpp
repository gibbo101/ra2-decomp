// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_device_internal.h"
#include <string.h>

// FUNCTION: GAMEMD 0x00402EF0
void __fastcall FUN_00402EF0(AudioSource * source)
{
    AudioChannel * next = (AudioChannel *)source->ChannelList.FUN_004073C0();
    AudioChannel * channel = next;
    while (next) {
        next = (AudioChannel *)next->FUN_004073E0();
        channel->FUN_00402550();
        channel = next;
    }
    source->DeviceDriver->Callbacks->DestroySource(source);
    source->DeviceDriver->Sources[source->DriverIndex] = NULL;
    int previous = source->DeviceDriver->References;
    source->DeviceDriver->References = previous - 1;
    if (previous - 1 < 0) source->DeviceDriver->References = 0;
    ::operator delete(source);
}
