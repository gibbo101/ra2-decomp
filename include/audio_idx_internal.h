// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once
#include "audio_stream.h"
class FileClass;
struct AudioIDXEntry {
    char Name[16];
    int Offset, Size, SampleRate;
    int Flags, ChunkSize;
};
struct AudioIDXData {
    AudioIDXEntry * Samples;
    int SampleCount;
    char Path[260];
    FileClass * BagFile;
    FileClass * CurrentSampleFile;
    int PathFound;
    FileClass * ActiveSampleFile;
    int ActiveSampleRemaining;
    int unknown_120;
    static AudioIDXData * __fastcall Create(char const * name, char const * path);
    ~AudioIDXData(void);
    int __fastcall FUN_004016F0(int index);
    int __fastcall FindSampleIndex(char const * name);
    void ClearCurrentSample(void);
    int __fastcall FUN_004018C0(void * buffer, int length);
    AudioFormat * __fastcall GetSampleInformation(int index, AudioFormat * format);
};
extern AudioIDXData * DAT_0087E294;
