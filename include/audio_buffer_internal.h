// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once
#include "audio_stream.h"

static inline void SetupBufferOwner(AudioBufferOwner * owner)
{
    AudioChunk * first = (AudioChunk *)owner->Chunks.Next;
    if (first == &owner->Chunks) first = NULL;
    if (first) {
        int order = 0;
        for (AudioChunk * chunk = first; chunk != &owner->Chunks; chunk = (AudioChunk *)chunk->Next) {
            for (int i = 0; i < 2; ++i) {
                chunk->Spans[i].Data = chunk->Data;
                chunk->Spans[i].Size = chunk->Size;
            }
            chunk->Order = order++;
        }
    }
    for (int i = 0; i < 2; ++i) {
        AudioPlayback * record = &owner->Playback[i];
        record->First = first;
        record->WritePosition = -1001;
        record->ReadPosition = record->WritePosition;
        record->Position = 0;
        record->FUN_00408930();
    }
    owner->Playback[0].WritePosition += owner->TotalBytes;
}

static inline void ClearBufferOwner(AudioBufferOwner * self)
{
    while (self->Chunks.Next != &self->Chunks) {
        AudioChunk * chunk = (AudioChunk *)self->Chunks.Next;
        chunk->FUN_00407450();
        AudioBufferOwner * owner = chunk->Owner;
        chunk->Owner = NULL;
        --owner->References;
        owner->TotalBytes -= chunk->Size;
        owner->Flags &= ~2;
        if (chunk->Allocation) ::operator delete(chunk->Allocation);
        delete chunk;
    }
    SetupBufferOwner(self);
}
