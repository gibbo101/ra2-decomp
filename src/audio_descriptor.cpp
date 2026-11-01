// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Derived from gagarin's GPL-3.0-or-later audio_event_create_methods.cpp.
#include "always.h"
#include "audio_event_internal.h"
#include "audio_resource_internal.h"
#include <string.h>

extern "C" int __cdecl _stricmp(char const *, char const *);
void __fastcall FUN_00407100(AudioRamp * ramp, int value);

// FUNCTION: GAMEMD 0x004063B0
VocClass * __fastcall FUN_004063B0(char const * name)
{
    if (!DAT_0087E2A0) return 0;
    SoundLink * node = DAT_0087E250.FUN_004073C0();
    while (node) {
        if (!_stricmp(name, ((VocClass *)node)->Name)) break;
        node = node->FUN_004073E0();
    }
    if (node) return (VocClass *)node;
    VocClass * type = (VocClass *)operator new(sizeof(VocClass));
    memset(type, 0, sizeof(*type));
    memset(type, 0, sizeof(*type));
    ((SoundLink *)type)->FUN_004072D0();
    type->SamplesOK = 1;
    type->Control = 0;
    type->Type = 0x20;
    type->Limit = 3;
    type->Priority = 2;
    type->Range = 10;
    type->MinVolume = 0.2f;
    FUN_00407100((AudioRamp *)&type->Volume, 0x4000);
    strncpy(type->Name, name, 31);
    type->Name[31] = 0;
    DAT_0087E250.FUN_00407420((SoundLink *)type);
    return type;
}
