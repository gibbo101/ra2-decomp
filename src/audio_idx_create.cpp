// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_idx_internal.h"
#include "ccfile.h"
#include <string.h>
#include <stdlib.h>

// FUNCTION: GAMEMD 0x004011C0
AudioIDXData * __fastcall AudioIDXData::Create(char const * name, char const * path)
{
    AudioIDXData * data = new AudioIDXData;
    memset(data, 0, sizeof(*data));
    char filename[260];
    strcpy(filename, name);
    char * dot = strrchr(filename, '.');
    if (dot) *dot = '\0';
    strcat(filename, ".idx");
    CCFileClass file(filename);
    struct { int Magic, Version, Count; } header;
    if (!file.Is_Available()) goto failed;
    dot = strrchr(filename, '.');
    if (dot) *dot = '\0';
    strcat(filename, ".bag");
    data->BagFile = new CCFileClass(filename);
    if (!data->BagFile->Is_Available()) goto failed;
    file.Open(1);
    data->BagFile->Open(1);
    if (file.Read(&header, sizeof(header)) != sizeof(header)) goto failed;
    data->SampleCount = header.Count;
    if (data->SampleCount <= 0) goto sample_loaded;
    data->Samples = (AudioIDXEntry *)::operator new(data->SampleCount * sizeof(AudioIDXEntry));
    memset(data->Samples, 0, data->SampleCount * sizeof(AudioIDXEntry));
    if (header.Version == 1) {
        { for (int i = 0; i < data->SampleCount; ++i) {
            if (file.Read(&data->Samples[i], 0x20) != 0x20) goto failed;
            data->Samples[i].ChunkSize = 0;
        } }
sample_loaded:
        qsort(data->Samples, data->SampleCount, sizeof(AudioIDXEntry), (int (__cdecl *)(void const *, void const *))stricmp);
        data->PathFound = 0;
        if (path) {
            strcpy(data->Path, path);
            if (data->Path[strlen(data->Path)] == '\\') data->Path[strlen(data->Path)] = '\0';
            WIN32_FIND_DATA info;
            HANDLE search = FindFirstFile(data->Path, &info);
            if (search != INVALID_HANDLE_VALUE) {
                do {
                    if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { data->PathFound = 1; break; }
                } while (FindNextFile(search, &info));
                FindClose(search);
            }
            if (data->Path[strlen(data->Path)] != '\\') strcat(data->Path, "\\");
        }
        return data;
    }
    if (file.Read(data->Samples, data->SampleCount * sizeof(AudioIDXEntry)) != data->SampleCount * sizeof(AudioIDXEntry)) {
failed:
        if (data) {
            if (data->BagFile) delete data->BagFile;
            if (data->CurrentSampleFile) delete data->CurrentSampleFile;
            if (data->Samples) ::operator delete(data->Samples);
            ::operator delete(data);
        }
        return NULL;
    }
    goto sample_loaded;
}
