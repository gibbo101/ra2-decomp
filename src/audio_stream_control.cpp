// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_stream.h"
#include <string.h>

// FUNCTION: GAMEMD 0x00407B40
void __fastcall FUN_00407B40(AudioStream * stream, int mixer) { stream->Channel->Mixer[1] = (AudioAttributes *)mixer; }

// FUNCTION: GAMEMD 0x00407B50
void __fastcall FUN_00407B50(AudioStream * stream, int mixer) { stream->Channel->Mixer[3] = (AudioAttributes *)mixer; }

// FUNCTION: GAMEMD 0x00407F40
void AudioStream::FUN_00407F40(void)
{
    if (!StopCount && Channel) {
        EnterCriticalSection(&DAT_0087E7F8);
        Channel->FUN_004025B0();
        LeaveCriticalSection(&DAT_0087E7F8);
    }
}

// FUNCTION: GAMEMD 0x00407F80
int AudioStream::FUN_00407F80(void) { return ++StopCount; }

// FUNCTION: GAMEMD 0x00407F90
int AudioStream::FUN_00407F90(void)
{
    int count = --StopCount;
    if (count < 0) StopCount = 0;
    return count;
}

// FUNCTION: GAMEMD 0x00407FB0
int AudioStream::FUN_00407FB0(void)
{
    if (!PauseCount && Channel) {
        EnterCriticalSection(&DAT_0087E7F8);
        Channel->FUN_00402600();
        unsigned flags = Flags;
        if (flags & 1) Flags = (flags & ~1) | 2;
        LeaveCriticalSection(&DAT_0087E7F8);
    }
    return ++PauseCount;
}

// FUNCTION: GAMEMD 0x00408070
int AudioStream::FUN_00408070(void) { return Flags & 1; }

// FUNCTION: GAMEMD 0x00408080
void __fastcall FUN_00408080(AudioStream * stream, char const * name)
{
    strncpy(stream->Name, name, sizeof(stream->Name));
    stream->Name[sizeof(stream->Name) - 1] = 0;
}

// FUNCTION: GAMEMD 0x004080A0
char const * AudioStream::FUN_004080A0(void) { return Name; }

// FUNCTION: GAMEMD 0x00408140
unsigned __int64 AudioStream::FUN_00408140(void) { return CallbackTime; }

// FUNCTION: GAMEMD 0x00408000
void AudioStream::FUN_00408000(void)
{
    if (PauseCount) {
        if (--PauseCount < 0) PauseCount = 0;
        if (!PauseCount && Channel) {
            EnterCriticalSection(&DAT_0087E7F8);
            unsigned flags = Flags;
            if (flags & 2) Flags = (flags & ~2) | 1;
            Channel->FUN_00402650();
            LeaveCriticalSection(&DAT_0087E7F8);
        }
    }
}
