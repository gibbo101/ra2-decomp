// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "audio_event_internal.h"
void __fastcall FUN_00407150(AudioRamp *, int);
// FUNCTION: GAMEMD 0x004054A0
int __fastcall FUN_004054A0(AudioEvent * handle)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(handle);
    if ((AudioStream *)event != event->Channel->Stream) return 0;
    event->Channel->FUN_004026A0();
    AudioData * buffer = event->PlaybackBuffer;
    event->Flags |= 8;
    event->Channel->FUN_00402790(buffer);
    FUN_00407150(&event->Channel->Attributes.Ramps[1], event->PlaybackVolume);
    int volume = 0x4000;
    if (event->PlaybackAttenuationPercent > 0) volume -= ((unsigned)event->PlaybackAttenuationPercent << 14) / 100;
    FUN_00407150(&event->Channel->Attributes.Ramps[0], volume);
    event->State = 3;
    event->Flags |= 2;
    event->PlaybackBuffer = NULL;
    if (event->Channel->FUN_00402720()) {
        event->State = 1;
        event->PlaybackBuffer = buffer;
        event->Flags &= ~2;
        event->Channel->FUN_004026B0();
        return 0;
    }
    event->Channel->FUN_004026B0();
    return 1;
}
