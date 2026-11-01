// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_stream.h"

// FUNCTION: GAMEMD 0x004072C0
void SoundLink::FUN_004072C0(void) { Next = this; Prev = this; Sentinel = this; }

// FUNCTION: GAMEMD 0x004072D0
void SoundLink::FUN_004072D0(void) { Next = this; Prev = this; Sentinel = NULL; }

// FUNCTION: GAMEMD 0x004073B0
int SoundLink::FUN_004073B0(void)
{
    int count = 0;
    for (SoundLink * p = Next; p != this; p = p->Next) ++count;
    return count;
}

// FUNCTION: GAMEMD 0x004073C0
SoundLink * SoundLink::FUN_004073C0(void)
{
    if (this) { SoundLink * p = Next; if (!p || p != p->Sentinel) return p; }
    return NULL;
}

// FUNCTION: GAMEMD 0x004073E0
SoundLink * SoundLink::FUN_004073E0(void)
{
    if (this) { SoundLink * p = Next; if (!p || p != p->Sentinel) return p; }
    return NULL;
}

// FUNCTION: GAMEMD 0x00407420
void __fastcall SoundLink::FUN_00407420(SoundLink * node)
{
    node->Prev = Prev; node->Next = this;
    Prev = node; node->Prev->Next = node;
}

// FUNCTION: GAMEMD 0x00407440
void __fastcall FUN_00407440(SoundLink * list, SoundLink * node)
{
    node->Prev = list; node->Next = list->Next;
    list->Next = node; node->Next->Prev = node;
}

// FUNCTION: GAMEMD 0x00407450
void SoundLink::FUN_00407450(void)
{
    Prev->Next = Next; Next->Prev = Prev;
    Next = this; Prev = this;
}
