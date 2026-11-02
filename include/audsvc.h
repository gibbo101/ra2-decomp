// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audtimer.h"

struct AudioServiceInfo
{
	volatile TimeStamp Interval;
	volatile TimeStamp MustServiceInterval;
	volatile TimeStamp Delay;
	volatile TimeStamp MaxDelay;
	volatile TimeStamp ResetInterval;
	volatile TimeStamp LastTime;
	volatile TimeStamp PeakDelay;
	volatile TimeStamp PeakTime;
	volatile TimeStamp PeakWindow;
	volatile int Count;
	volatile int Missed;
	volatile int LastCount;
	int Anim;
};

void __fastcall AudioServiceInfoInit(AudioServiceInfo * info);
void __fastcall AudioServiceSetInterval(AudioServiceInfo * info, TimeStamp interval);
void __fastcall AudioServiceSetMustServiceInterval(AudioServiceInfo * info, TimeStamp interval);
void __fastcall AudioServiceSetResetInterval(AudioServiceInfo * info, TimeStamp interval);
int __fastcall AudioServiceNeeded(AudioServiceInfo * info, TimeStamp now);
void __fastcall AudioServicePerform(AudioServiceInfo * info, TimeStamp now);
void __fastcall AudioServiceInfoDump(AudioServiceInfo * info, char * buf);
