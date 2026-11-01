// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once
#include "audio_stream.h"
static inline void StandardChannel(AudioChannel * channel)
{
    channel->NextFrame = FUN_004022C0;
    channel->NextSample = FUN_00402330;
    channel->SampleDone = FUN_004023F0;
    channel->Fill = NULL;
    channel->unknown_B4 = NULL;
    channel->StopA = NULL;
    channel->StopB = NULL;
    channel->Stream = NULL;
    channel->Type = 1;
    channel->LoopCount = 0;
    channel->Flags = 0;
    channel->Priority = 0;
    channel->Mixer[1] = DAT_0087DE30;
    channel->Mixer[0] = NULL;
    channel->Mixer[2] = NULL;
    channel->Mixer[3] = NULL;
    FUN_00401000(&channel->Attributes);
    channel->Sample = NULL;
}
static inline void StopChannel(AudioChannel * channel)
{
    channel->Driver->Lock(channel);
    if (channel->Flags & 5) channel->Driver->Stop(channel);
    channel->Driver->Unlock(channel);
    if (channel->StopB) channel->StopB(channel);
    channel->Sample = NULL;
}
static inline void SetChannelSample(AudioChannel * channel, AudioData * sample)
{
    channel->Sample = sample;
    if (sample) {
        channel->Block = sample->FUN_00408520();
        if (channel->Block) {
            channel->QueueData = channel->Block->Data;
            channel->QueueRemaining = channel->QueueSize = channel->Block->Size;
            sample->Data = NULL;
        } else {
            channel->QueueData = channel->Sample->Data;
            channel->QueueRemaining = channel->QueueSize = channel->Sample->Size;
        }
    }
}
