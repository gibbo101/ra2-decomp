// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include	"vqaplayp.h"
#include	<stdio.h>

// FUNCTION: GAMEMD 0x007C3910
void VQA_SetTimer(VQAHandleP *vqap, long time)
{
	vqap->TickOffset = 0;
	unsigned long curtime = VQA_GetTime(vqap);
	vqap->TickOffset = (time - curtime);
}

// FUNCTION: GAMEMD 0x007C3960
unsigned long VQA_GetTime(VQAHandleP *vqap)
{
	unsigned long ticks;

	ticks = (vqap->Config.TimerCallback((VQAHandle *)vqap) * VQA_TIMETICKS) / vqap->Config.RefreshRate;
	ticks += vqap->TickOffset;

	return(ticks);
}

// FUNCTION: GAMEMD 0x007C3990
unsigned long VQA_GetMovieTime(VQAHandle *vqa)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;

	VQAConfig *config;

	config = &vqap->Config;

	unsigned long rate = config->RefreshRate;

	unsigned long ticks;

	ticks = (config->TimerCallback((VQAHandle *)vqap) * VQA_TIMETICKS) / rate;
	ticks += vqap->TickOffset;

	return(ticks);
}
