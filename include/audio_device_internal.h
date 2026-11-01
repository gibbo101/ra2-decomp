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

struct AudioDriverCallbacks {
    unsigned unknown_00, unknown_04, Capabilities, Flags, unknown_10;
    int (__fastcall *Initialize)(AudioDeviceDriver *);
    void (__fastcall *Shutdown)(AudioDeviceDriver *);
    int (__fastcall *CreateSource)(AudioSource *);
    void (__fastcall *DestroySource)(AudioSource *);
    AudioChannelDriver * ChannelDriver;
};
struct AudioDeviceDriver : SoundLink {
    AudioDriverCallbacks * Callbacks;
    int References;
    unsigned unknown_14;
    int SourceCount;
    AudioSource * Sources[17];
};
AudioDeviceDriver * __fastcall FUN_00402AF0(AudioDriverCallbacks * callbacks);
AudioSource * __fastcall FUN_00402C70(int index, AudioFormat * format);
void __fastcall FUN_00402EF0(AudioSource * source);
int __fastcall FUN_00402F60(AudioSource * source);
int __fastcall FUN_004010A0(AudioAttributes * attributes);
typedef char audio_device_driver_size[(sizeof(AudioDeviceDriver) == 0x60) ? 1 : -1];
typedef char audio_device_driver_callbacks[(offsetof(AudioDeviceDriver, Callbacks) == 0xC) ? 1 : -1];
typedef char audio_device_driver_sources[(offsetof(AudioDeviceDriver, Sources) == 0x1C) ? 1 : -1];
