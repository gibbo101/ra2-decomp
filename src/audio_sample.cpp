// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_stream.h"
#include "ccfile.h"
#include <string.h>

// FUNCTION: GAMEMD 0x004084E0
void AudioData::FUN_004084E0(void)
{
    Data = NULL; Size = 0; unknown_14 = 0; unknown_18 = 0;
    Buffers.FUN_004072C0();
}

// FUNCTION: GAMEMD 0x004085A0
void AudioFormat::FUN_004085A0(void)
{
    memset(this, 0, sizeof(*this));
}

// FUNCTION: GAMEMD 0x00408540
int AudioFormat::FUN_00408540(unsigned __int64 duration)
{
    return (BytesPerSecond * (int)duration) / 1000;
}

// FUNCTION: GAMEMD 0x00408560
unsigned __int64 __fastcall AudioFormat::FUN_00408560(int bytes)
{
    if (BytesPerSecond) return ((unsigned __int64)bytes * 1000) / BytesPerSecond;
    return 0;
}

// FUNCTION: GAMEMD 0x004085B0
void AudioFormat::FUN_004085B0(void)
{
    BytesPerSecond = BytesPerSample * Channels * SampleRate;
    if (Compression == 1 || Compression == 2) BytesPerSecond >>= 2;
}
