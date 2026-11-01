// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#include "always.h"
#include "audio_stream.h"

namespace Game { extern bool IsReadingAudioFile; }
void __cdecl FUN_004068E0(char const * format, ...);
extern char DAT_0081628C[];

// FUNCTION: GAMEMD 0x00407550
int FUN_00407550(void)
{
    char error[1024];
    DAT_0087E810.FUN_004072C0();
    InitializeCriticalSection(&DAT_0087E7F8);
    DAT_0087E770.Stop = 0;
    DAT_0087E770.Finished = 0;
    DAT_0087E770.Running = 0;
    DAT_0087E770.Interval = 1000;
    DAT_0087E770.Iterations = 0;
    DAT_0087E770.Timer.FUN_00403820();
    DAT_0087E770.Timer.FUN_00403870(DAT_0087E770.Interval);
    DAT_0087E770.Timer.FUN_004038A0(DAT_0087E770.Interval * 4);
    DAT_0087E770.Timer.FUN_004038C0(DAT_0087E770.Interval * 4);
    DAT_0087E7F0 = CreateThread(NULL, 4096, FUN_00407680, &DAT_0087E770, 0, &DAT_0087E7F4);
    if (!DAT_0087E7F0) return 0;
    if (!SetThreadPriority(DAT_0087E7F0, THREAD_PRIORITY_HIGHEST)) {
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, error, sizeof(error), NULL);
        FUN_004068E0(DAT_0081628C, error);
    }
    DAT_0087E81C = DAT_0087E820 = 1;
    return 1;
}

// FUNCTION: GAMEMD 0x00407780
void FUN_00407780(void)
{
    if (DAT_0087E81C) {
        if (DAT_0087E820) {
            DAT_0087E770.Stop = 1;
            while (!DAT_0087E770.Finished) Sleep(0);
            WaitForSingleObject(DAT_0087E7F0, 5000);
            CloseHandle(DAT_0087E7F0);
            DAT_0087E820 = 0;
        }
        AudioStream * stream = (AudioStream *)DAT_0087E810.FUN_004073C0();
        while (stream) {
            if (!stream->StopCount && stream->Channel) {
                EnterCriticalSection(&DAT_0087E7F8);
                stream->Channel->FUN_004025B0();
                LeaveCriticalSection(&DAT_0087E7F8);
            }
            stream->FUN_00407A90();
            stream = (AudioStream *)DAT_0087E810.FUN_004073C0();
        }
        DeleteCriticalSection(&DAT_0087E7F8);
        DAT_0087E81C = 0;
    }
}
