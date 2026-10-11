// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ahandle.h"
#include "dsaudio.h"
#include "assert.h"
#include "dbgprint.h"
#include "gametime.h"
#include "vqaplayp.h"
#ifndef DSBCAPS_GETCURRENTPOSITION2
#define DSBCAPS_GETCURRENTPOSITION2	0x00010000
#endif

Ahandle _handles[Ahandle::MAX_HANDLES];

long __cdecl Stop_Audio_Handler(VQAHandleP *vqap);

// FUNCTION: 0x00412320
long __cdecl Stop_Audio_Handler(VQAHandleP *vqap)
{
	Ahandle *handle = &_handles[vqap->AudioHandleIndex];

	if (handle->SecondaryBufferPtr != NULL) {
		EnterCriticalSection(&handle->CriticalSection);
		handle->SecondaryBufferPtr->Stop();
		handle->SecondaryBufferPtr->Release();
		handle->SecondaryBufferPtr = NULL;
		handle->AudioBufReadIndex = 0;
		handle->AudioBufWriteIndex = 0;
		for (int i = 0; i < Ahandle::MAX_BUFFERS; i++) {
			handle->AudioBufInUse[i] = false;
		}

		LeaveCriticalSection(&handle->CriticalSection);
	}
	return(VQAERR_NONE);
}

// FUNCTION: 0x00412550
void Pause_All_Audio_Handler(void)
{
	for (int i = 0; i < Ahandle::MAX_HANDLES; i++) {
		Ahandle *handle = &_handles[i];
		if (handle->Used == true && handle->SecondaryBufferPtr != NULL) {
			EnterCriticalSection(&handle->CriticalSection);

			handle->SecondaryBufferPtr->Stop();
			handle->Flags |= AHANDLEF_IS_PAUSED;

			LeaveCriticalSection(&handle->CriticalSection);
		}
	}
}

// FUNCTION: 0x004125A0
void Resume_All_Audio_Handler(void)
{
	for (int i = 0; i < Ahandle::MAX_HANDLES; i++) {
		Ahandle *handle = &_handles[i];
		if (handle->Used == true && handle->SecondaryBufferPtr != NULL) {
			EnterCriticalSection(&handle->CriticalSection);

			handle->SecondaryBufferPtr->Play(0, 0, DSBPLAY_LOOPING);
			handle->Flags &= ~AHANDLEF_IS_PAUSED;

			LeaveCriticalSection(&handle->CriticalSection);
		}
	}
}
