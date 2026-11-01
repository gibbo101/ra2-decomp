// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_stream.h"
#include "ccfile.h"
#include <string.h>

// FUNCTION: GAMEMD 0x004079C0
int __fastcall FUN_004079C0(AudioChannel * channel)
{
    AudioStream * stream = channel->Stream;
    stream->ReaderB->FUN_004091C0(stream->Remaining);
    stream->ReaderB->FUN_00409310();
    if (stream->BufferedBytes > stream->ReaderB->Available)
        stream->Remaining = stream->ReaderB->Available;
    else
        stream->Remaining = stream->BufferedBytes;
    channel->QueueRemaining = channel->QueueSize = stream->Remaining;
    channel->QueueData = stream->ReaderB->Cursor;
    return 0;
}

// FUNCTION: GAMEMD 0x00407A30
int __fastcall FUN_00407A30(AudioChannel * channel)
{
    AudioStream * stream = channel->Stream;
    if (stream->Flags & 3) stream->CallbackTime = FUN_004093B0();
    stream->Flags &= ~7;
    stream->PauseCount = 0;
    return 0;
}

// FUNCTION: GAMEMD 0x00407A60
int __fastcall FUN_00407A60(AudioChannel * channel)
{
    AudioStream * stream = channel->Stream;
    if (stream->Flags & 3) stream->CallbackTime = FUN_004093B0();
    stream->Flags &= ~7;
    stream->PauseCount = 0;
    return 0;
}

// FUNCTION: GAMEMD 0x00407A90
void AudioStream::FUN_00407A90(void)
{
    if (Prev != this) {
        EnterCriticalSection(&DAT_0087E7F8);
        FUN_00407450();
        LeaveCriticalSection(&DAT_0087E7F8);
    }
    if (!StopCount && Channel) {
        EnterCriticalSection(&DAT_0087E7F8);
        Channel->FUN_004025B0();
        LeaveCriticalSection(&DAT_0087E7F8);
    }
    if (ReaderA) { ReaderA->FUN_00408E70(); ReaderA = NULL; }
    if (ReaderB) { ReaderB->FUN_00408E70(); ReaderB = NULL; }
    if (BufferOwner) BufferOwner->FUN_00408840();
    if (Channel) Channel->FUN_004024A0();
    if (File) delete File;
    delete this;
}
