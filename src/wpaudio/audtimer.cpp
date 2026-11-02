// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include "audtimer.h"

static LONGLONG timerMilliSecScale;
static TimeStamp lastTime = 0;
static TimeStamp interval = 0;
static TimeStamp timeOut = 0;
static TimeStamp (*timerFunc)(void) = NULL;

static TimeStamp failsafeGetTime(void);
static TimeStamp highResGetTime(void);

// FUNCTION: GAMEMD 0x00409360
void InitAudioTimer(void)
{
	LARGE_INTEGER freq;

	if (QueryPerformanceFrequency(&freq)) {
		timerMilliSecScale = freq.QuadPart / (LONGLONG)SECONDS(1);
		timerFunc = highResGetTime;
	} else {
		timerFunc = failsafeGetTime;
	}
}

// FUNCTION: GAMEMD 0x004093B0
TimeStamp AudioGetTime(void)
{
	return timerFunc ? timerFunc() : 0;
}

// FUNCTION: GAMEMD 0x004093C0
static TimeStamp highResGetTime(void)
{
	LARGE_INTEGER count;
	union {
		TimeStamp timeStamp;
		LARGE_INTEGER largeInt;
	} myTime;

	QueryPerformanceCounter(&count);
	myTime.largeInt.QuadPart = count.QuadPart / timerMilliSecScale;
	return myTime.timeStamp;
}

// FUNCTION: GAMEMD 0x004093F0
static TimeStamp failsafeGetTime(void)
{
	static volatile TimeStamp time = (TimeStamp)0x100000000;
	static unsigned int * const pl = (unsigned int * const)&time;
	static unsigned int * const ph = (unsigned int * const)((unsigned int)&time + 4);
	unsigned int now, lw, hw, called;
	static volatile unsigned int calls = 0;

	calls++;

retry:
	called = calls;
	hw = *ph;
	lw = *pl;
	if (called != calls) {
		goto retry;
	}

reread:
	now = timeGetTime();
	if (now < lw) {
		hw++;
	}
	*ph = hw;
	*pl = now;
	if (called != calls) {
		called = calls;
		goto reread;
	}
	return time;
}
