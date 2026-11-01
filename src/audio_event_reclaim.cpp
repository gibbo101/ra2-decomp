// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Derived from gagarin's GPL-3.0-or-later small_misc45_methods.cpp.
#include "always.h"
#include "audio_event_internal.h"
#include "audio_resource_internal.h"
#include <string.h>

// FUNCTION: GAMEMD 0x00404DD0
void __fastcall FUN_00404DD0(AudioEvent * handle)
{
    AudioEventRuntime * event = static_cast<AudioEventRuntime *>(handle);
    event->FUN_00407450();
    event->Stamp = 0;
    if (event->Flags & 0x80) {
        operator delete(event);
        --DAT_0087E28C;
        return;
    }
    DAT_0087E2A8->FUN_00407530((SoundNode *)event);
    --DAT_0087E28C;
}
