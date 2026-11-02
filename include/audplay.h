// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audtimer.h"

struct AudioStreamer;
struct AudioDevice;
struct AudioChannel;
struct AudioAttribs;

int AudioStreamerInit(void);
void AudioStreamerDeinit(void);
AudioStreamer * __fastcall AudioStreamerCreate(AudioDevice * dev, TimeStamp buffering);
void __fastcall AudioStreamerDestroy(AudioStreamer * as);
void __fastcall AudioStreamerSetAttribs(AudioStreamer * as, AudioAttribs * attribs);
void __fastcall AudioStreamerSetFadeAttribs(AudioStreamer * as, AudioAttribs * attribs);
int __fastcall AudioStreamerPlayWAV(AudioStreamer * as, const char * filename, int flag);
void __fastcall AudioStreamerStop(AudioStreamer * as);
void __fastcall AudioStreamerLock(AudioStreamer * as);
void __fastcall AudioStreamerUnlock(AudioStreamer * as);
void __fastcall AudioStreamerPause(AudioStreamer * as);
void __fastcall AudioStreamerResume(AudioStreamer * as);
int __fastcall AudioStreamerIsPlaying(AudioStreamer * as);
void __fastcall AudioStreamerSetName(AudioStreamer * as, const char * name);
const char * __fastcall AudioStreamerName(AudioStreamer * as);
void __fastcall AudioStreamerFadeIn(AudioStreamer * as);
void __fastcall AudioStreamerFadeOut(AudioStreamer * as);
int __fastcall AudioStreamerIsFading(AudioStreamer * as);
void __fastcall AudioStreamerWaitForFade(AudioStreamer * as);
TimeStamp __fastcall AudioStreamerEndTimeStamp(AudioStreamer * as);
void AudioStreamerStarve(void);
void AudioStreamerStopAllStreams(void);
void AudioStreamerLockAllStreams(void);
void AudioStreamerUnlockAllStreams(void);
void AudioStreamerPauseAllStreams(void);
void AudioStreamerResumeAllStreams(void);
void AudioStreamerFadeOutAllStreams(void);
void AudioStreamerFadeInAllStreams(void);
int AudioStreamerAllFaded(void);
