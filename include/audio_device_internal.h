// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once
#include "audio_stream.h"
struct AudioRoot {
    SoundLink Drivers;
    SoundLink Devices;
    int References;
    unsigned int : 32;
    AudioAttributes Attributes;
};
extern AudioRoot * DAT_00815E98;
extern int DAT_0087DE28, DAT_0087DE2C;
typedef char audio_root_attributes[(offsetof(AudioRoot, Attributes) == 0x20) ? 1 : -1];
