// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "audio_stream.h"

static inline AudioChunk * NextChunk(AudioChunk * chunk)
{
    AudioBufferOwner * owner = chunk->Owner;
    AudioChunk * head = (AudioChunk *)&owner->Chunks;
    chunk = (AudioChunk *)chunk->Next;
    if (chunk == head) {
        chunk = (AudioChunk *)chunk->Next;
        if (chunk == head) return NULL;
    }
    return chunk;
}

// FUNCTION: GAMEMD 0x00409100
int AudioPlayback::FUN_00409100(void)
{
    int remaining = Pending;
    if (!remaining) return 0;
    int other = Index + 1;
    if (other == 2) other = 0;
    AudioPlayback * peer = &Owner->Playback[other];
    ReadPosition += remaining;
    peer->WritePosition += remaining;
    AudioChunk * chunk = First;
    int committed = remaining;
    while (remaining) {
        AudioSpan * span = &chunk->Spans[Index];
        if (span->Size > remaining) {
            span->Data += remaining;
            span->Size -= remaining;
            break;
        }
        remaining -= span->Size;
        span->Data = chunk->Data;
        span->Size = chunk->Size;
        chunk = NextChunk(chunk);
        if (!remaining) break;
    }
    First = chunk;
    Pending = 0;
    Current = First;
    if (Current) Cursor = Current->Spans[Index].Data;
    else Cursor = NULL;
    Available = 0;
    Flags &= ~1;
    FUN_004091C0(0);
    return committed;
}


static inline unsigned TotalBytes(AudioPlayback * access)
{
    unsigned in = access->WritePosition;
    unsigned out = access->ReadPosition;
    unsigned total;
    if (in < out) total = (0xFFFFFFFFU - out) + 1 + in;
    else total = in - out;
    return total;
}

// FUNCTION: GAMEMD 0x004091C0
int __fastcall AudioPlayback::FUN_004091C0(int amount)
{
    int moved = 0, available, advance;
    int total;
    int ok = TRUE;
    total = TotalBytes(this);
    if ((available = total - Pending) == 0) {
        Available = 0;
        return 0;
    }
    if (available < amount) amount = available;
    do {
        AudioSpan * span = &Current->Spans[Index];
        if (Flags & 1) Available = available;
        else {
            int actual = span->Size - ((unsigned)Cursor - (unsigned)span->Data);
            if (actual < available) Available = actual;
            else Available = available;
        }
        if ((advance = amount) == 0) break;
        char * data;
        if (advance >= Available) {
            advance = Available;
            Current = NextChunk(Current);
            if (Current == First) {
                Flags |= 1;
                data = Current->Data;
            } else data = Current->Spans[Index].Data;
        } else data = Cursor + advance;
        Cursor = data;
        amount -= advance;
        moved += advance;
        available -= advance;
    } while (ok);
    Pending += moved;
    Position += moved;
    if (Position >= (unsigned)Owner->TotalBytes) Position %= (unsigned)Owner->TotalBytes;
    if (Pending && Deferred == 0) FUN_00409100();
    return moved;
}
