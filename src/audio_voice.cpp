// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "audio_stream.h"
extern AudioSource * DAT_0087E29C;
extern AudioAttributes * DAT_0087E748;
extern AudioAttributes * DAT_0087E758;
AudioChannel * __fastcall FUN_004035F0(AudioSource *, int);
// FUNCTION: GAMEMD 0x00405B50
AudioChannel * __fastcall FUN_00405B50(int priority, int persistent)
{
    AudioChannel * channel = NULL;
    if (DAT_0087E29C) {
        channel = FUN_004035F0(DAT_0087E29C, 1);
        if (channel) {
            if ((channel->Flags & 0x15) && channel->Priority > priority) return NULL;
            channel->FUN_004025B0();
            channel->Mixer[1] = DAT_0087E748;
            channel->Mixer[3] = DAT_0087E758;
            channel->Priority = priority;
            if (persistent & 1) channel->LoopCount = -1;
            else channel->LoopCount = 0;
        }
    }
    return channel;
}
