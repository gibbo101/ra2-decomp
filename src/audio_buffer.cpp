// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_buffer_internal.h"
#include <string.h>

static inline char * AlignBuffer(void * allocation, int alignment)
{
    return (char *)(((unsigned)allocation + (1U << alignment) - 1) & (-1 << alignment));
}

// FUNCTION: GAMEMD 0x00408930
void AudioPlayback::FUN_00408930(void)
{
    Pending = 0;
    Current = First;
    if (Current) Cursor = Current->Spans[Index].Data;
    else Cursor = NULL;
    Available = 0;
    Flags &= ~1;
}

// FUNCTION: GAMEMD 0x00408A70
void AudioBufferOwner::FUN_00408A70(void) { SetupBufferOwner(this); }

// FUNCTION: GAMEMD 0x00408970
void AudioBufferOwner::FUN_00408970(void) { ClearBufferOwner(this); }

// FUNCTION: GAMEMD 0x00408B10
unsigned __fastcall AudioBufferOwner::FUN_00408B10(AudioChunk * chunk)
{
    FUN_00407440(&Chunks, chunk);
    ++References;
    TotalBytes += chunk->Size;
    chunk->Owner = this;
    return Flags &= ~2;
}

// FUNCTION: GAMEMD 0x00408D50
int AudioBufferOwner::FUN_00408D50(void) { return TotalBytes; }

// FUNCTION: GAMEMD 0x00408D80
AudioChunk * __fastcall FUN_00408D80(int size, int alignment)
{
    AudioChunk * chunk = (AudioChunk *)::operator new(sizeof(AudioChunk));
    memset(chunk, 0, sizeof(*chunk));
    chunk->FUN_004072D0();
    unsigned boundary = 1U << alignment;
    chunk->Allocation = ::operator new(size + boundary);
    if (!chunk->Allocation) { if (chunk) ::operator delete(chunk); return NULL; }
    chunk->Alignment = alignment;
    chunk->Data = AlignBuffer(chunk->Allocation, alignment);
    chunk->Size = size;
    return chunk;
}

// FUNCTION: GAMEMD 0x00408B40
int __fastcall AudioBufferOwner::FUN_00408B40(int count, int size, int alignment)
{
    int allocated = 0;
    while (count--) {
        AudioChunk * chunk = FUN_00408D80(size, alignment);
        if (!chunk) break;
        FUN_00408B10(chunk);
        ++allocated;
    }
    return allocated;
}
