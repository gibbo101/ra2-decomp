// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Derived from gagarin's GPL-3.0-or-later small_misc45_methods.cpp.
#include "audio_event_internal.h"
// FUNCTION: GAMEMD 0x00404E20
VocClass * __fastcall FUN_00404E20(int rows)
{
    AudioPriorityRow * cursor = DAT_0087DE38;
    for (int row = 0; row < rows; ++row, ++cursor) {
        for (int column = 0; column < 10; ++column) {
            SoundLink * node = cursor->Headers[column].FUN_004073C0();
            if (node) return (VocClass *)((char *)node - offsetof(VocClass, unknown_A4));
        }
    }
    return NULL;
}
