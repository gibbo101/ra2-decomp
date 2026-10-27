// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ahandle.h"
#include <ctype.h>
#include <math.h>
#include "dsaudio.h"
#include "assert.h"
#include "globals.h"
#include "goptions.h"
#include "geometry.h"
#include "dbgprint.h"
#include "gametime.h"
#include "vqaplayp.h"
#ifndef DSBCAPS_GETCURRENTPOSITION2
#define DSBCAPS_GETCURRENTPOSITION2	0x00010000
#endif

LPDIRECTSOUND FUN_0040A7A0(void);
int FUN_00407000(void);


Ahandle _handles[Ahandle::MAX_HANDLES];

bool _restore_primary = false;
WAVEFORMATEX _restore_format;

AHANDLE_CALLBACK_1 _AHandleCallbackFunc1;
AHANDLE_CALLBACK_2 _AHandleCallbackFunc2;

long __cdecl Open_Audio_Handler(VQAHandleP *vqap, AhandleInitParams *params, long);
long __cdecl Close_Audio_Handler(VQAHandleP *vqap);
long __cdecl Start_Audio_Handler(VQAHandleP *vqap);
long __cdecl Stop_Audio_Handler(VQAHandleP *vqap);
long __cdecl Play_Audio_Handler(VQAHandleP *vqap);
long __cdecl Pause_Audio_Handler(VQAHandleP *vqap);
long __cdecl Resume_Audio_Handler(VQAHandleP *vqap);
long __cdecl Load_Audio_Handler(VQAHandleP *vqap, void *buffer, long nbytes);
void CALLBACK AudioCallback(UINT uTimerID, UINT, DWORD dwUser, DWORD, DWORD);

inline unsigned long Get_Playback_Position(VQAHandle *vqa, Ahandle *audio, VQAConfig *config)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;

	unsigned long s = audio->SecondaryBufferSize;

	unsigned long dma_diff;
	unsigned long totalbytes;
	DWORD				play_cursor;
	DWORD				write_cursor;

	EnterCriticalSection(&audio->CriticalSection);

	long r = vqap->RepeatedBuffers;
	long l = audio->LastChunkPosition;
	long m = audio->ChunksMovedToAudioBuffer;

	totalbytes = config->HMIBufSize * m;

	if (audio->SecondaryBufferPtr &&
		audio->SecondaryBufferPtr->GetCurrentPosition (&play_cursor, &write_cursor) == S_OK) {
		if (l) {
			totalbytes += play_cursor;
		} else {
			if (play_cursor >= (s / 2)) {
				totalbytes += play_cursor - (s / 2);
			} else {
				if (totalbytes > 0) {
					totalbytes += play_cursor + (s / 2);
				}
			}
		}
	}

	LeaveCriticalSection(&audio->CriticalSection);

	dma_diff = totalbytes - config->HMIBufSize * r;
	if (dma_diff > totalbytes) {
		dma_diff = 0;
	}
	return(dma_diff);
}

inline BOOL Move_HMI_Audio_Block_To_Direct_Sound_Buffer(VQAHandleP *vqap)
{
	Ahandle  	*audio;

	LPVOID		play_buffer_ptr1;
	LPVOID		play_buffer_ptr2;
	DWORD			lock_length1;
	DWORD			lock_length2;
	unsigned		next_fill_pos;
	HRESULT		return_code;

	audio = &_handles[vqap->AudioHandleIndex];
	VQAConfig *config = &vqap->Config;

	if (audio->SecondaryBufferPtr == NULL) {
		return(0);
	}

	next_fill_pos = audio->EndLastAudioChunk;

	return_code= audio->SecondaryBufferPtr->Lock ((DWORD)next_fill_pos,
																	(DWORD)config->HMIBufSize,
																	&play_buffer_ptr1,
																	&lock_length1,
																	&play_buffer_ptr2,
																	&lock_length2,
																	0 );

	if (return_code!=DS_OK) return(FALSE);

	int index = audio->AudioBufReadIndex;

	if (audio->AudioBufSize[index] > lock_length1) {

		memcpy((char*)play_buffer_ptr1, audio->AudioBuf[index], lock_length1);

		if (audio->AudioBufSize[index] - lock_length1 > lock_length2) {
			memcpy(play_buffer_ptr2, audio->AudioBuf[index], lock_length2);
		} else {
			memcpy(play_buffer_ptr2, audio->AudioBuf[index], audio->AudioBufSize[index] - lock_length1);
		}

	} else {
		memcpy(play_buffer_ptr1, audio->AudioBuf[index], audio->AudioBufSize[index]);
	}

	audio->SecondaryBufferPtr->Unlock(play_buffer_ptr1,
												lock_length1,
												play_buffer_ptr2,
												lock_length2);

	audio->LastChunkPosition = next_fill_pos;
	audio->EndLastAudioChunk = next_fill_pos + audio->AudioBufSize[index];
	if (audio->EndLastAudioChunk >= audio->SecondaryBufferSize) {
		audio->EndLastAudioChunk = 0;
	}

	return(TRUE);
}

// FUNCTION: GAMEMD 0x004116A0
unsigned long __cdecl Timer_Callback_Audio_Handler(VQAHandle *vqa)
{
	VQAHandleP    *vqap = (VQAHandleP *)vqa;
	Ahandle       *handle = &_handles[vqap->AudioHandleIndex];
	unsigned long d;
	unsigned long t;

	if (handle->Flags & AHANDLEF_IS_PAUSED) {
		DebugString("Ahandle: Paused %ld\n", handle->TickCount);
		return(handle->TickCount);
	}

	t = Simple_Timer_Callback_Audio_Handler(NULL) - handle->PauseAdjust;

	d = Get_Playback_Position(vqa, handle, &vqap->Config);

	if (d > 0 && d <= handle->LastPlaybackPosition) {
		if (t > handle->LastTimerTick) {
			d = (t - handle->LastTimerTick);
			handle->LastTimerTick = t;
			handle->TickCount += d;
		}
	} else {
		handle->LastPlaybackPosition = d;
		handle->LastTimerTick = t;
		handle->TickCount = (VQA_TIMETICKS * (d / (vqap->Channels * (vqap->BitsPerSample >> 3))) / vqap->SampleRate);

		if (handle->TickCount >= vqap->Config.LatencyAdjustment) {
			handle->TickCount -= vqap->Config.LatencyAdjustment;
		} else {
			handle->TickCount = 0;
		}
	}
	return(handle->TickCount);
}

// FUNCTION: GAMEMD 0x00411820
long __cdecl Stream_Audio_Handler(VQAHandle *vqa, long action, void *buffer, long nbytes)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;
	VQAConfig *config;

	long error = VQAERR_NONE;

	switch (action) {

		case VQAAUDIO_INIT:
			config = &vqap->Config;
			config->TimerCallback = Simple_Timer_Callback_Audio_Handler;
			config->RefreshRate = VQA_TIMETICKS;
			break;

		case VQAAUDIO_OPEN:
			error = Open_Audio_Handler((VQAHandleP *)vqa, (AhandleInitParams *)buffer, nbytes);
			break;

		case VQAAUDIO_CLOSE:
			error = Close_Audio_Handler((VQAHandleP *)vqa);
			break;

		case VQAAUDIO_START:
			error = Start_Audio_Handler((VQAHandleP *)vqa);
			break;

		case VQAAUDIO_LOAD:
			error = Load_Audio_Handler((VQAHandleP *)vqa, buffer, nbytes);
			break;

		case VQAAUDIO_PAUSE:
			error = Pause_Audio_Handler((VQAHandleP *)vqa);
			break;

		case VQAAUDIO_PLAY:
			error = Play_Audio_Handler((VQAHandleP *)vqa);
			break;

		case VQAAUDIO_STOP:
			error = Stop_Audio_Handler((VQAHandleP *)vqa);
			break;
	}

	return(error);
}

// FUNCTION: GAMEMD 0x00411B20
long __cdecl Open_Audio_Handler(VQAHandleP *vqap, AhandleInitParams *params, long b)
{
	DSCAPS dscaps;
	LPDIRECTSOUND sound = FUN_0040A7A0();

	DebugString("Opening VQ audio handler\n");
	DebugString("Current thread ID is %08x\n", GetCurrentThreadId());

	if (!FUN_00407000()) {
		return(VQAERR_AUDIO);
	}

	int index = 0;
	while (index < Ahandle::MAX_HANDLES) {
		if (!_handles[index].Used) {
			break;
		}
		index++;
	}

	if (index != 1 && b == 16) {

		VQAConfig *config = &vqap->Config;

		vqap->AudioHandleIndex = index;

		Ahandle *handle = &_handles[index];
		memset(handle, 0, sizeof(*handle));
		handle->Used = TRUE;
		handle->Volume = config->Volume;

		InitializeCriticalSection(&handle->CriticalSection);

		if (config->AudioRate != -1) {
			handle->SampleRate = config->AudioRate;
		} else {
			if (config->FrameRate != vqap->FrameRate) {
				handle->SampleRate = params->SampleRate * (unsigned)config->FrameRate / vqap->FrameRate;
			} else {
				handle->SampleRate = params->SampleRate;
			}
		}

		memset(&dscaps, 0, sizeof(dscaps));
		dscaps.dwSize = sizeof(DSCAPS);

		sound->GetCaps(&dscaps);

		if (dscaps.dwFlags & DSCAPS_EMULDRIVER) {
			DebugString("Ahandle detected emulated sound driver. LatencyAdjustment = %ld ticks\n", config->LatencyAdjustment);
		} else {
			config->LatencyAdjustment = 0;
		}

		handle->Channels = params->Channels;
		handle->BitsPerSample = params->BitsPerSample;
		handle->InitFlags = params->Flags;
		_AHandleCallbackFunc1 = (AHANDLE_CALLBACK_1)params->Callback1;
		_AHandleCallbackFunc2 = (AHANDLE_CALLBACK_2)params->Callback2;

		DebugString("Calling timeBeginPeriod\n");
		if (timeBeginPeriod(1000/VQA_TIMETICKS) != TIMERR_NOCANDO) {
			DebugString("Creating VQ audio timer thread\n");
			handle->TimerHandle = timeSetEvent ( 1000/VQA_TIMETICKS , 1 , AudioCallback , (DWORD)vqap , TIME_PERIODIC);
			DebugString("VQ audio handler opened OK\n");
			if (handle->TimerHandle != 0) {
				return(VQAERR_NONE);
			}
			return(VQAERR_AUDIO);
		}
	}
	return(VQAERR_AUDIO);
}

// FUNCTION: GAMEMD 0x00411CE0
long __cdecl Close_Audio_Handler(VQAHandleP *vqap)
{
	DebugString("Closing VQ audio handler\n");

	Ahandle *handle = &_handles[vqap->AudioHandleIndex];

	if (handle->Used == 1) {

		if (handle->SecondaryBufferPtr != NULL) {
			DebugString("Stop_Audio_Handler\n");
			Stop_Audio_Handler(vqap);
		}

		DebugString("Calling timeKillEvent\n");
		timeKillEvent(handle->TimerHandle);
		handle->TimerHandle = 0;

		DebugString("Calling timeEndPeriod\n");
		timeEndPeriod(1000/VQA_TIMETICKS);

		handle->Used = false;

		DebugString("Deleting the critical section object\n");
		DeleteCriticalSection(&handle->CriticalSection);
	}

	DebugString("VQ audio handler closed OK\n");
	return(VQAERR_NONE);
}

// FUNCTION: GAMEMD 0x00411DF0
long __cdecl Start_Audio_Handler(VQAHandleP *vqap)
{
	VQAConfig *config = &vqap->Config;
	Ahandle *audio = &_handles[vqap->AudioHandleIndex];

	if (audio->Flags & AHANDLEF_IS_PAUSED) {
		return(Play_Audio_Handler(vqap));
	}

	EnterCriticalSection(&audio->CriticalSection);

	if (audio->SecondaryBufferPtr != NULL){
		audio->SecondaryBufferPtr->Stop();
		audio->SecondaryBufferPtr->Release();
		audio->SecondaryBufferPtr = NULL;
	}

	audio->SecondaryBufferSize = config->HMIBufSize*2;

	memset (&audio->BufferDesc , 0 , sizeof(DSBUFFERDESC));
	audio->BufferDesc.dwSize				= sizeof(DSBUFFERDESC);
	audio->BufferDesc.dwFlags				= DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_CTRLFREQUENCY|DSBCAPS_GETCURRENTPOSITION2;
	audio->BufferDesc.dwBufferBytes		= audio->SecondaryBufferSize;
	audio->BufferDesc.lpwfxFormat 		= (LPWAVEFORMATEX) &audio->DsBuffFormat;
	memset (&audio->DsBuffFormat , 0 , sizeof(WAVEFORMATEX));
	audio->DsBuffFormat.wFormatTag		= WAVE_FORMAT_PCM;
	audio->DsBuffFormat.nSamplesPerSec	= audio->SampleRate;
	audio->DsBuffFormat.nChannels			= audio->Channels;
	audio->DsBuffFormat.wBitsPerSample	= audio->BitsPerSample;
	audio->DsBuffFormat.nBlockAlign		= (short) ((audio->DsBuffFormat.wBitsPerSample/8) * audio->DsBuffFormat.nChannels);
	audio->DsBuffFormat.nAvgBytesPerSec	= audio->DsBuffFormat.nSamplesPerSec * audio->DsBuffFormat.nBlockAlign;

	FUN_0040A7A0()->CreateSoundBuffer (&audio->BufferDesc , &audio->SecondaryBufferPtr , NULL);

	if (audio->SecondaryBufferPtr == NULL) {
		LeaveCriticalSection(&audio->CriticalSection);
		return(VQAERR_AUDIO);
	}

	_AHandleCallbackFunc1((VQAHandle *)vqap);
	_AHandleCallbackFunc1((VQAHandle *)vqap);
	audio->AudioBufWriteIndex = 0;
	audio->EndLastAudioChunk = 0;
	audio->ChunksMovedToAudioBuffer = 0;
	audio->SecondaryBufferPtr->SetCurrentPosition (0);
	audio->Volume = (int)(Options.VoiceVolume * 255.0f);

	audio->SecondaryBufferPtr->SetVolume(Convert_HMI_To_Direct_Sound_Volume(audio->Volume & 255));

	HRESULT return_code = audio->SecondaryBufferPtr->Play(0, 0, DSBPLAY_LOOPING);
	LeaveCriticalSection(&audio->CriticalSection);
	return(return_code == DS_OK ? VQAERR_NONE : VQAERR_AUDIO);
}

inline void Update_Volume(Ahandle *audio)
{
	if (audio != NULL && audio->SecondaryBufferPtr != NULL) {
		if ((double)audio->Volume != Options.VoiceVolume * 256.0f) {
			audio->Volume = (int)(Options.VoiceVolume * 255.0f);
			audio->SecondaryBufferPtr->SetVolume(Convert_HMI_To_Direct_Sound_Volume(audio->Volume & 255));
		}
	}
}

// FUNCTION: GAMEMD 0x00412000
long __cdecl Load_Audio_Handler(VQAHandleP *vqap, void *buffer, long nbytes)
{
	Ahandle *handle = &_handles[vqap->AudioHandleIndex];

	if (buffer != NULL && nbytes != 0) {
		EnterCriticalSection(&handle->CriticalSection);
		Update_Volume(handle);
		if (handle->AudioBufInUse[handle->AudioBufReadIndex] == TRUE) {
			LeaveCriticalSection(&handle->CriticalSection);
			return(VQAERR_AUDIO);
		}
		int readindex = handle->AudioBufReadIndex;
		handle->AudioBuf[readindex] = buffer;
		handle->AudioBufSize[readindex] = nbytes;
		handle->AudioBufInUse[readindex] = TRUE;
		Move_HMI_Audio_Block_To_Direct_Sound_Buffer(vqap);
		int index = readindex + 1;
		if (index >= Ahandle::MAX_BUFFERS) {
			index = 0;
		}
		handle->AudioBufReadIndex = index;
		LeaveCriticalSection(&handle->CriticalSection);
		return(VQAERR_NONE);
	}
	return(VQAERR_AUDIO);
}

// FUNCTION: GAMEMD 0x00412230
long __cdecl Pause_Audio_Handler(VQAHandleP *vqap)
{
	Ahandle *handle = &_handles[vqap->AudioHandleIndex];

	EnterCriticalSection(&handle->CriticalSection);

	if (handle->Used == true && handle->SecondaryBufferPtr != NULL) {
		handle->SecondaryBufferPtr->Stop();
	}
	handle->Flags |= AHANDLEF_IS_PAUSED;

	LeaveCriticalSection(&handle->CriticalSection);

	return(VQAERR_NONE);
}

// FUNCTION: GAMEMD 0x00412280
long __cdecl Play_Audio_Handler(VQAHandleP *vqap)
{
	Ahandle *handle = &_handles[vqap->AudioHandleIndex];

	long rc;
	if (!handle->Used || handle->SecondaryBufferPtr == NULL) {
		return(VQAERR_AUDIO);
	}

	EnterCriticalSection(&handle->CriticalSection);

	rc = handle->SecondaryBufferPtr->Play(0, 0, DSBPLAY_LOOPING);
	if (rc == S_OK) {
		handle->PauseAdjust = Simple_Timer_Callback_Audio_Handler(NULL) - handle->LastTimerTick;
		DebugString("Ahandle: PauseAdjust %ld\n", handle->PauseAdjust);
		handle->Flags &= ~AHANDLEF_IS_PAUSED;
		rc = VQAERR_NONE;
	} else {
		rc = VQAERR_AUDIO;
	}
	LeaveCriticalSection(&handle->CriticalSection);

	return(rc);
}

// FUNCTION: GAMEMD 0x00412320
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

// FUNCTION: GAMEMD 0x004123A0
void CALLBACK AudioCallback ( UINT uTimerID, UINT, DWORD dwUser, DWORD, DWORD )
{
	Ahandle  	*audio;
	DWORD			play_cursor;
	DWORD			write_cursor;
	HRESULT		return_code;
	bool			buffer_stopped = false;
	DWORD			status;

	VQAHandle *vqa = (VQAHandle *)dwUser;
	VQAHandleP *vqap = (VQAHandleP *)dwUser;

	audio = &_handles[vqap->AudioHandleIndex];

	if (InterlockedIncrement(&audio->SuspendAudioCallback) != TRUE || (audio->Flags & AHANDLEF_IS_PAUSED)) {
		InterlockedDecrement(&audio->SuspendAudioCallback);
		return;
	}

	EnterCriticalSection(&audio->CriticalSection);

	if (!audio->SecondaryBufferPtr || audio->TimerHandle != uTimerID)  {
		LeaveCriticalSection(&audio->CriticalSection);
		InterlockedDecrement(&audio->SuspendAudioCallback);
		return;
	}

	return_code = audio->SecondaryBufferPtr->GetStatus(&status);
	if (!(status & DSBSTATUS_PLAYING) && !(status & DSBSTATUS_LOOPING)) {
		LeaveCriticalSection(&audio->CriticalSection);
		InterlockedDecrement(&audio->SuspendAudioCallback);
		return;
	}

	return_code = audio->SecondaryBufferPtr->GetCurrentPosition (&play_cursor , &write_cursor);

	bool write_more = false;

	if (return_code == DSERR_BUFFERLOST) {
		audio->SecondaryBufferPtr->Restore();
		audio->SecondaryBufferPtr->Stop();
		buffer_stopped = true;
		audio->SecondaryBufferPtr->SetCurrentPosition (0);
		audio->LastChunkPosition = 0;
		audio->EndLastAudioChunk = 0;
		write_more = true;
	}

	if (play_cursor < audio->EndLastAudioChunk){
		write_more = true;
	}else{
		if ((play_cursor >= audio->SecondaryBufferSize / 2) && audio->EndLastAudioChunk==0){
			write_more = true;
		}
	}

	if (write_more == true) {
		_AHandleCallbackFunc2((VQAHandle *)vqa, audio->AudioBuf[audio->AudioBufWriteIndex]);
		audio->ChunksMovedToAudioBuffer++;
		int index = audio->AudioBufWriteIndex + 1;
		if (index >= Ahandle::MAX_BUFFERS) {
			index = 0;
		}
		audio->AudioBufInUse[audio->AudioBufWriteIndex] = FALSE;
		_AHandleCallbackFunc1((VQAHandle *)vqa);

		if (audio->AudioBufInUse[index]) {
			audio->AudioBufWriteIndex = index;
		}

		if (buffer_stopped == true) {
			audio->SecondaryBufferPtr->Play(0,0,DSBPLAY_LOOPING);
		}
	}
	LeaveCriticalSection(&audio->CriticalSection);
	InterlockedDecrement(&audio->SuspendAudioCallback);
}

// FUNCTION: GAMEMD 0x00412540
unsigned long __cdecl Simple_Timer_Callback_Audio_Handler(VQAHandle *vqa)
{
	return(Get_Game_Time_50());
}

// FUNCTION: GAMEMD 0x00412550
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

// FUNCTION: GAMEMD 0x004125A0
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
// FUNCTION: GAMEMD 0x004125F0
long __cdecl Lock_Audio_Handler(void)
{

	return(1);
}

// FUNCTION: GAMEMD 0x00412600
long __cdecl Unlock_Audio_Handler(void)
{

	return(1);
}


// FUNCTION: GAMEMD 0x00412610
int ahtoi(const char *str)
{
	int integer = 0;
	while (str) {
		if (!isxdigit(*str)) {
			break;
		}
		char ch = *str;

		integer *= 16;
		str++;

		if (ch >= '0' && ch <= '9') {
			integer += ch - '0';
		} else {
			integer += toupper(ch) - ('A' - 10);
		}
	}
	return(integer);
}

// SYNTHETIC: GAMEMD 0x00412850 SYMBOL
// _$E49
const Rect RECT_NONE_2(0, 0, 0, 0);

// SYNTHETIC: GAMEMD 0x00412660 SYMBOL
// _$E10
// SYNTHETIC: GAMEMD 0x00412690 SYMBOL
// _$E13
// SYNTHETIC: GAMEMD 0x004126B0 SYMBOL
// _$E16
// SYNTHETIC: GAMEMD 0x004126D0 SYMBOL
// _$E19
// SYNTHETIC: GAMEMD 0x004126F0 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x00412710 SYMBOL
// _$E25
// SYNTHETIC: GAMEMD 0x00412730 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x00412770 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x004127A0 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x004127D0 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x00412800 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x00412810 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x00412830 SYMBOL
// _$E46

