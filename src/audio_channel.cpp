// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audio_channel_internal.h"
#include <string.h>
// FUNCTION: GAMEMD 0x00402040
AudioChannel * AudioSource::FUN_00402040(void)
{
    AudioChannel * channel = (AudioChannel *)::operator new(sizeof(AudioChannel));
    memset(channel, 0, sizeof(*channel));
    channel->FUN_004072D0();
    channel->LoopCount = 0;
    channel->Flags = 0;
    FUN_00401000(&channel->Output);
    StandardChannel(channel);
    channel->Device = this;
    channel->Driver = Driver;
    channel->Format = DefaultFormat;
    channel->FormatChanged = 0;
    channel->DriverFormatChanged = 0;
    if (Driver->Open(channel)) {
        if (channel) {
            channel->Driver->Close(channel);
            ::operator delete(channel);
        }
        return NULL;
    }
    return channel;
}
// FUNCTION: GAMEMD 0x00402170
void AudioChannel::FUN_00402170(void)
{
    Driver->Close(this);
    ::operator delete(this);
}
// FUNCTION: GAMEMD 0x00402190
void AudioChannel::FUN_00402190(void) { StandardChannel(this); }
// FUNCTION: GAMEMD 0x00402790
void __fastcall AudioChannel::FUN_00402790(AudioData * sample) { SetChannelSample(this, sample); }
// FUNCTION: GAMEMD 0x00402800
int __fastcall AudioChannel::FUN_00402800(AudioFormat * format)
{
    if (memcmp(format, &Format, sizeof(Format)) == 0) FormatChanged = 0;
    else { FormatChanged = 1; Format = *format; }
    return FormatChanged;
}
// FUNCTION: GAMEMD 0x00402850
void __fastcall FUN_00402850(SoundLink * node) { node->Sentinel = NULL; node->Prev = NULL; }
// FUNCTION: GAMEMD 0x00402860
void __fastcall AudioSource::FUN_00402860(AudioChannel * channel)
{
    channel->Device = this;
    ++ChannelCount;
    ++ListReferences;
    ChannelList.FUN_00407420(channel);
    if (--ListReferences < 0) ListReferences = 0;
}
// FUNCTION: GAMEMD 0x004028B0
void AudioChannel::FUN_004028B0(void)
{
    AudioSource * device = Device;
    FUN_00407450();
    --device->ChannelCount;
    ++device->ListReferences;
    FUN_00407450();
    if (--device->ListReferences < 0) device->ListReferences = 0;
}
