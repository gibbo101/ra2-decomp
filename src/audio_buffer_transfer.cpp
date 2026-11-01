// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_stream.h"
#include "wwfile.h"

// FUNCTION: GAMEMD 0x00408F80
int __fastcall AudioPlayback::FUN_00408F80(FileClass * file, int length, int * transferred)
{
    int local;
    FUN_004091C0(0);
    if (!transferred) transferred = &local;
    *transferred = 0;
    for (;;) {
        int amount = Available;
        if (amount > length) amount = length;
        if (!amount) return 1;
        int moved;
        if (Index) {
            moved = file->Write(Cursor, amount);
            if (moved < 0) Status = 0x80040000;
        } else {
            moved = file->Read(Cursor, amount);
            if (moved < 0) Status = 0x80040000;
        }
        if (!moved) return 2;
        if (moved == -1) return 0;
        FUN_004091C0(moved);
        length -= moved;
        *transferred += moved;
        if (!length) return 1;
    }
}

// FUNCTION: GAMEMD 0x00409310
int AudioPlayback::FUN_00409310(void)
{
    FUN_004091C0(0);
    return Available;
}

#include <stdio.h>

// FUNCTION: GAMEMD 0x00409030
int __fastcall FUN_00409030(AudioPlayback * playback, FILE * file, int length, int * transferred)
{
    int local;
    playback->FUN_004091C0(0);
    if (!transferred) transferred = &local;
    *transferred = 0;
    for (;;) {
        int amount = playback->Available;
        if (amount > length) amount = length;
        if (!amount) return 1;
        int moved;
        if (playback->Index) {
            moved = fwrite(playback->Cursor, 1, amount, file);
            if (moved < 0) playback->Status = 0x80040000;
        } else {
            moved = fread(playback->Cursor, 1, amount, file);
            if (moved < 0) playback->Status = 0x80040000;
        }
        if (!moved) return 2;
        if (moved == -1) return 0;
        playback->FUN_004091C0(moved);
        length -= moved;
        *transferred += moved;
        if (!length) return 1;
    }
}
#include <string.h>

// FUNCTION: GAMEMD 0x00408EE0
int __fastcall FUN_00408EE0(AudioPlayback * playback, void * buffer, int length)
{
    int total = 0;
    char * cursor = (char *)buffer;
    playback->FUN_004091C0(0);
    do {
        int amount = playback->Available;
        if (amount > length) amount = length;
        if (!amount) break;
        if (playback->Index) memcpy(cursor, playback->Cursor, amount);
        else memcpy(playback->Cursor, cursor, amount);
        playback->FUN_004091C0(amount);
        cursor += amount;
        length -= amount;
        total += amount;
    } while (length);
    return total;
}
