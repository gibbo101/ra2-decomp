// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "mpu.h"
#include "win.h"
#include <math.h>

typedef union {
	LARGE_INTEGER LargeInt;
	struct QuadPart {
		unsigned int LowPart;
		unsigned int HighPart;
	} QuadPart;
} QuadValue;

// FUNCTION: 0x005CAF70
unsigned int Get_CPU_Rate(unsigned int & high)
{
	union {
		LARGE_INTEGER LargeInt;
		struct {
			unsigned int LowPart;
			unsigned int HighPart;
		} QuadPart;
	} value;

	if (QueryPerformanceFrequency(&value.LargeInt)) {
		high = value.QuadPart.HighPart;
		return(value.QuadPart.LowPart);
	}
	high = 0;
	return(0);
}

#define ASM_RDTSC _asm _emit 0x0f _asm _emit 0x31
#define MAX_TRIES 20
#define ROUND_THRESHOLD 6
#define TOLERANCE 1

static unsigned long TSC_High;
static unsigned long TSC_Low;

// FUNCTION: 0x005CAFB0
void RDTSC(void)
{
	_asm
	{
		ASM_RDTSC;
		mov	TSC_Low, eax
		mov	TSC_High, edx
	}
}

// FUNCTION: 0x005CAFC0
int Get_RDTSC_CPU_Speed(void)
{
	LARGE_INTEGER t0,t1;
	unsigned int	freq=0;
	unsigned int	freq2=0;
	unsigned int	freq3=0;
	unsigned int	total;
	int	tries=0;

	unsigned int	total_cycles=0, cycles;
	unsigned int	stamp0, stamp1;
	unsigned int	total_ticks=0, ticks;

	LARGE_INTEGER count_freq;

	if ( !QueryPerformanceFrequency(&count_freq) ) return(0);

#if 0
	HANDLE process = GetCurrentProcess();
	DWORD processPri = GetPriorityClass(process);
	SetPriorityClass(process, REALTIME_PRIORITY_CLASS);

	HANDLE thread = GetCurrentThread();
	int threadPri = GetThreadPriority(thread);
	SetThreadPriority(thread, THREAD_PRIORITY_TIME_CRITICAL);
#endif

	do	{

		tries++;

		freq3 = freq2;
		freq2 = freq;

		QueryPerformanceCounter(&t0);

		t1.LowPart = t0.LowPart;
		t1.HighPart = t0.HighPart;

		while ( (unsigned int)t1.LowPart - (unsigned int)t0.LowPart<50) {
			QueryPerformanceCounter(&t1);
		}

		ASM_RDTSC;
		_asm	mov	stamp0, EAX

		t0.LowPart = t1.LowPart;
		t0.HighPart = t1.HighPart;

		while ( (unsigned int)t1.LowPart - (unsigned int)t0.LowPart < 1000 ) {
			QueryPerformanceCounter(&t1);
		}

		ASM_RDTSC;
		_asm	mov	stamp1, EAX

		cycles = stamp1 - stamp0;

		ticks = ((unsigned int)t1.LowPart - (unsigned int)t0.LowPart);
		ticks = ticks * 100000;

		ticks = (unsigned int)(ticks / (count_freq.LowPart / 10));

		total_ticks += ticks;
		total_cycles += cycles;
		if ( (ticks % count_freq.LowPart) > (count_freq.LowPart/2) ) ticks++;

		freq = cycles/ticks;

		if ( cycles%ticks > ticks/2 ) freq++;

		total = ( freq + freq2 + freq3 );

	} while ( (tries < 3 ) || (tries < 20) && ((abs((int)(3 * freq -total)) > 3*TOLERANCE )|| (abs((int)(3 * freq2-total)) > 3*TOLERANCE )|| (abs((int)(3 * freq3-total)) > 3*TOLERANCE )));

#if 0
	SetThreadPriority(thread, threadPri);
	SetPriorityClass(process, processPri);
#endif

	freq3 = ( total_cycles * 10 ) / total_ticks;
	freq2 = ( total_cycles * 100 ) / total_ticks;

	if ( freq2 - (freq3 * 10) >= ROUND_THRESHOLD ) freq3++;

	int norm_freq = total_cycles / total_ticks;

	freq = norm_freq * 10;
	if ( (freq3 - freq) >= ROUND_THRESHOLD ) norm_freq++;

	return (norm_freq);

}

// FUNCTION: 0x005CB190 $E2
// FUNCTION: 0x005CB1C0 $E5
// FUNCTION: 0x005CB1E0 $E8
// FUNCTION: 0x005CB200 $E11
// FUNCTION: 0x005CB220 $E14
// FUNCTION: 0x005CB240 $E17
// FUNCTION: 0x005CB260 $E20
// FUNCTION: 0x005CB2A0 $E23
// FUNCTION: 0x005CB2D0 $E26
// FUNCTION: 0x005CB300 $E29
