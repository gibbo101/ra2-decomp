// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_device_internal.h"

// FUNCTION: GAMEMD 0x00402F60
int __fastcall FUN_00402F60(AudioSource * source)
{
    if (source->ListReferences) return 0x80120000;
    unsigned __int64 now = FUN_004093B0();
    if (now - source->ServiceTimer.PreviousTime < source->ServiceTimer.Duration) return 0;
    {
        unsigned __int64 delta = now - source->ServiceTimer.PreviousTime;
        source->ServiceTimer.LastDelta = delta;
        ++source->ServiceTimer.Samples;
        if (source->ServiceTimer.MaximumDelta > source->ServiceTimer.ResetLimit)
            source->ServiceTimer.MaximumDelta = 0;
        if (delta > source->ServiceTimer.MaximumDelta) source->ServiceTimer.MaximumDelta = delta;
        if (delta > source->ServiceTimer.Threshold) ++source->ServiceTimer.Overruns;
        if (delta > source->ServiceTimer.PeakDelta) source->ServiceTimer.PeakDelta = delta;
        source->ServiceTimer.PreviousTime = now;
        AudioAttributes * attributes = &source->Attributes;
        FUN_00401080(attributes);
        if (source->GroupAttributes) FUN_00401080(source->GroupAttributes);
        int changed;
        if (source->GroupAttributes && (changed = FUN_004010A0(source->GroupAttributes))) {
        } else changed = FUN_004010A0(attributes);
        SoundLink * list = &source->ChannelList;
        AudioChannel * channel = (AudioChannel *)list->Next;
        while (channel != list) {
            AudioChannel * next = (AudioChannel *)channel->Next;
            if ((channel->Flags & 1) && channel->Sample) {
                FUN_00401080(&channel->Attributes);
                if (changed || FUN_004010A0(&channel->Attributes)
                    || (channel->Mixer[1] && FUN_004010A0(channel->Mixer[1]))
                    || (channel->Mixer[2] && FUN_004010A0(channel->Mixer[2]))
                    || (channel->Mixer[3] && FUN_004010A0(channel->Mixer[3]))
                    || (channel->Mixer[0] && FUN_004010A0(channel->Mixer[0]))) {
                    channel->FUN_00402220();
                    channel->Driver->unknown_20(channel);
                }
            }
            channel = next;
        }
        if (source->GroupAttributes) FUN_00401170(source->GroupAttributes);
        FUN_00401170(attributes);
    }
    return 0;
}
