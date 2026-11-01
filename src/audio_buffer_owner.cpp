// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_buffer_internal.h"

// FUNCTION: GAMEMD 0x00408840
void AudioBufferOwner::FUN_00408840(void)
{
    ClearBufferOwner(this);
    delete this;
}
