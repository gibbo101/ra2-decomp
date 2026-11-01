// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_resource_internal.h"
#include <string.h>

static inline void ReleaseSample(AudioSampleRecord * sample)
{
    if (sample->Prev != sample) sample->FUN_00407450();
    AudioSampleBlock * block;
    while ((block = (AudioSampleBlock *)FUN_00407470(&sample->Data.Buffers)) != NULL) {
        block->FUN_00407450();
        FUN_00408600(block);
        sample->Owner->BlockPool->FUN_00407530((SoundNode *)block);
    }
    FUN_00408530(&sample->Data);
    sample->Owner->SamplePool->FUN_00407530((SoundNode *)sample);
}

// FUNCTION: GAMEMD 0x004019E0
AudioSystem * __fastcall FUN_004019E0(AudioIDXData * index, int totalBytes, int count, int blockSize)
{
    AudioSystem * system = new AudioSystem;
    memset(system, 0, sizeof(*system));
    system->Index = index;
    system->BlockSize = blockSize;
    int size = blockSize + sizeof(AudioPoolBlock);
    system->BlockPool = FUN_004074D0(totalBytes / size, size);
    system->SamplePool = FUN_004074D0(count, sizeof(AudioSampleRecord));
    system->Samples.FUN_004072C0();
    if (!system->BlockPool) {
fail:
        AudioSampleRecord * sample;
        while ((sample = (AudioSampleRecord *)FUN_00407470(&system->Samples)) != NULL) ReleaseSample(sample);
        if (system->BlockPool) system->BlockPool->FUN_00407510();
        if (system->SamplePool) system->SamplePool->FUN_00407510();
        delete system;
        return NULL;
    }
    if (!system->SamplePool) goto fail;
    return system;

}
