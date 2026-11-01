// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audio_channel_internal.h"
AudioSampleBlock * __fastcall FUN_00407470(AudioSampleBlock * block);
// FUNCTION: GAMEMD 0x00402220
void AudioChannel::FUN_00402220(void)
{
    AudioSource * device = Device;
    FUN_00401000(&Output);
    FUN_004010C0(&Output, &Attributes);
    FUN_00401170(&Attributes);
    if (Mixer[0]) { FUN_004010C0(&Output, Mixer[0]); FUN_00401170(Mixer[0]); }
    if (Mixer[1]) FUN_004010C0(&Output, Mixer[1]);
    if (Mixer[2]) FUN_004010C0(&Output, Mixer[2]);
    if (Mixer[3]) FUN_004010C0(&Output, Mixer[3]);
    FUN_004010C0(&Output, &device->Attributes);
    if (device->GroupAttributes) FUN_004010C0(&Output, device->GroupAttributes);
}
// FUNCTION: GAMEMD 0x004022C0
int __fastcall FUN_004022C0(AudioChannel * channel)
{
    if (channel->Fill) {
        int result = channel->Fill(channel);
        if (result) {
            channel->QueueSize = 0;
            channel->QueueRemaining = 0;
            channel->QueueData = NULL;
            return result;
        }
        return 0;
    }
    if (channel->Block) {
        channel->Block = FUN_00407470(channel->Block);
        if (channel->Block) {
            channel->QueueData = channel->Block->Data;
            channel->QueueSize = channel->Block->Size;
            channel->QueueRemaining = channel->QueueSize;
            return 0;
        }
    }
    channel->QueueSize = 0;
    return 0;
}
// FUNCTION: GAMEMD 0x00402330
int __fastcall FUN_00402330(AudioChannel * channel)
{
    if (channel->LoopCount) {
        if (channel->LoopCount != -1) --channel->LoopCount;
        SetChannelSample(channel, channel->Sample);
    } else SetChannelSample(channel, NULL);
    if (channel->unknown_B4) channel->unknown_B4(channel);
    if (channel->Sample) channel->Driver->Queue(channel);
    return 0;
}
// FUNCTION: GAMEMD 0x004023F0
int __fastcall FUN_004023F0(AudioChannel * channel)
{
    channel->Sample = NULL;
    channel->Flags &= ~5;
    channel->LoopCount = 0;
    FUN_00401000(&channel->Attributes);
    if (channel->StopA) return channel->StopA(channel);
    return 0;
}
