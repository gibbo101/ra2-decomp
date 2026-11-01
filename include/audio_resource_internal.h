// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once
#include "audio_stream.h"
class FileClass;
#include "audio_pool.h"
#include "audio_idx_internal.h"
struct AudioSystem {
    AudioIDXData * Index;
    SoundPool * BlockPool;
    SoundPool * SamplePool;
    SoundLink Samples;
    int BlockSize;
    char unknown_1C[0xCC];
};
struct AudioSampleRecord : SoundLink {
    int Active, unknown_10, Index, References;
    AudioData Data;
    AudioSystem * Owner;
    AudioFormat Format;
    int unknown_60;
};
struct AudioPoolBlock : AudioSampleBlock { int unknown_14, unknown_18; };
AudioSystem * __fastcall FUN_004019E0(AudioIDXData * index, int totalBytes, int count, int blockSize);
AudioSampleRecord * __fastcall FUN_00401C00(AudioSystem * system, int index);
SoundLink * __fastcall FUN_00407470(SoundLink * list);
SoundLink * __fastcall FUN_00407480(SoundLink * list);
void __fastcall FUN_00408500(AudioData * data, AudioSampleBlock * block);
void __fastcall FUN_00408530(AudioData * data);
void __fastcall FUN_004085E0(AudioSampleBlock * block, void * buffer, int size);
void __fastcall FUN_00408600(AudioSampleBlock * block);
void __fastcall FUN_00401F80(AudioSampleRecord * sample);
void __fastcall FUN_00401F90(AudioSampleRecord * sample);
AudioData * __fastcall FUN_00402030(AudioSampleRecord * sample);
extern AudioIDXData * DAT_0087E294;
extern AudioSystem * DAT_0087E298;
typedef char audio_idx_size[(sizeof(AudioIDXData)==0x124)?1:-1];
typedef char audio_idx_bag[(offsetof(AudioIDXData,BagFile)==0x10C)?1:-1];
typedef char audio_system_size[(sizeof(AudioSystem)==0xE8)?1:-1];
typedef char audio_sample_size[(sizeof(AudioSampleRecord)==0x64)?1:-1];
typedef char audio_sample_data[(offsetof(AudioSampleRecord,Data)==0x1C)?1:-1];
typedef char audio_sample_format[(offsetof(AudioSampleRecord,Format)==0x40)?1:-1];
