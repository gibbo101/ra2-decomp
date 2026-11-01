// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_stream.h"
#include "ccfile.h"
#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>

// FUNCTION: GAMEMD 0x00408610
int __fastcall FUN_00408610(FileClass * file, AudioFormat * format, int * length)
{
    WAVEFORMATEX * wave = NULL;
    int result = 0;
    struct { unsigned id, size, type; } riff;
    struct { unsigned id, size; } chunk;
    if (length) *length = 0;
    if (!file) goto out;
    file->Seek(0, SEEK_SET);
    if (file->Read(&riff, sizeof(riff)) != sizeof(riff) || riff.id != 0x46464952 || riff.type != 0x45564157) goto out;
    while (file->Is_Open()) {
        if (file->Read(&chunk, sizeof(chunk)) != sizeof(chunk)) break;
        switch (chunk.id) {
        case 0x20746D66:
            if (chunk.size < sizeof(WAVEFORMATEX)) {
                wave = (WAVEFORMATEX *)malloc(sizeof(WAVEFORMATEX));
                memset(wave, 0, sizeof(WAVEFORMATEX));
            } else {
                wave = (WAVEFORMATEX *)malloc(chunk.size);
                memset(wave, 0, chunk.size);
            }
            file->Read(wave, chunk.size);
            wave->cbSize = (unsigned short)chunk.size;
            break;
        case 0x61746164:
            goto found;
        default:
            file->Seek(chunk.size, SEEK_CUR);
            break;
        }
    }
    goto done;
found:
    *length = chunk.size;
    if (!wave) goto out;
    if (wave->wFormatTag == 0x11) {
        format->unknown_18 = wave->nBlockAlign;
        format->Compression = 1;
        format->BytesPerSample = 2;
    } else if (wave->wFormatTag == 1) {
        format->Compression = 0;
        format->BytesPerSample = wave->wBitsPerSample >> 3;
    } else goto done;
    format->Channels = wave->nChannels;
    format->BytesPerSecond = wave->nAvgBytesPerSec;
    format->SampleRate = wave->nSamplesPerSec;
    format->Format = 4;
    format->BytesPerSecond = format->BytesPerSample * format->Channels * format->SampleRate;
    if (format->Compression == 1 || format->Compression == 2) format->BytesPerSecond >>= 2;
    result = 1;
done:
    if (wave) free(wave);
out:
    return result;
}
