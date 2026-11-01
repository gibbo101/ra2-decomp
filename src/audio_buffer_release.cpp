// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Derived from gagarin's GPL-3.0-or-later small_misc25_methods.cpp.
#include "always.h"
#include "audio_stream.h"

// FUNCTION: GAMEMD 0x00408E70
void AudioPlayback::FUN_00408E70(void)
{
    int previous = References;
    References = previous - 1;
    if (previous - 1 < 0) References = 0;

    AudioBufferOwner * owner = Owner;
    int owner_previous = owner->ActiveReferences;
    owner->ActiveReferences = owner_previous - 1;
    if (owner_previous - 1 < 0) Owner->ActiveReferences = 0;
}

// FUNCTION: GAMEMD 0x00409320
unsigned int AudioPlayback::FUN_00409320(void)
{
    unsigned int written = WritePosition;
    unsigned int read = ReadPosition;
    return written - read;
}

// FUNCTION: GAMEMD 0x00408D30
unsigned int AudioBufferOwner::FUN_00408D30(void)
{
    unsigned int written = Playback[1].WritePosition;
    unsigned int read = Playback[1].ReadPosition;
    return written - read;
}

// FUNCTION: GAMEMD 0x00408D40
unsigned int AudioBufferOwner::FUN_00408D40(void)
{
    unsigned int written = Playback[0].WritePosition;
    unsigned int read = Playback[0].ReadPosition;
    return written - read;
}
