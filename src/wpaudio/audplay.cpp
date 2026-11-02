// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include "always.h"
#include "wwfile.h"
#include "rawfile.h"
#include "ccfile.h"
#include "audplay.h"
#include "audlist.h"
#include "audlock.h"
#include "audlevel.h"
#include "audattr.h"
#include "audsvc.h"
#include "audsrc.h"

#define AS_PLAYING 0x01
#define AS_PAUSED 0x02
#define AS_FILL 0x04

#define STM_FAIL 0
#define STM_EOF 2

struct Game { static bool __cdecl CallBack(void); static bool IsReadingAudioFile; };

struct StmBlock
{
	unsigned reserved;
	char * Data;
	int Bytes;
};

struct StmStream;
struct StmBuffer;

struct AudioChannel
{
	ListNode nd;
	int Type;
	AudioAttribs ChannelAttribs;
	char pad88[8];
	AudioAttribs * SfxAttribs;
	AudioAttribs * GroupAttribs;
	AudioAttribs * CompAttribs;
	AudioAttribs * FadeAttribs;
	char padA0[0x10];
	int (__fastcall * CB_NextFrame)(AudioChannel * chan);
	char padB4[4];
	int (__fastcall * CB_SampleDone)(AudioChannel * chan);
	int (__fastcall * CB_Stop)(AudioChannel * chan);
	void * Data;
	char padC4[0xac];
	char * frameData;
	int bytesInFrame;
	int bytesRemaining;
	char pad17C[0x2c];
	TimeStamp frameTime;
	TimeStamp bufferTime;
};

struct AudioStreamer
{
	ListNode nd;
	int flags;
	AudioDevice * device;
	AudioChannel * channel;
	TimeStamp buffering;
	TimeStamp end_time_stamp;
	TimeStamp start_time_stamp;
	StmStream * stream;
	StmBlock * in;
	StmBlock * out;
	AudioSample sample;
	AudioFormat format;
	FileClass * file;
	Lock pause;
	int unused84;
	Lock lock;
	int unused8C;
	int pending_bytes;
	int total_bytes;
	int data_start;
	int bytes_left;
	int frame_size;
	char stream_name[80];
};

struct StreamThread
{
	volatile int quit;
	volatile int count;
	volatile int done;
	int unusedC;
	volatile TimeStamp interval;
	volatile int running;
	int unused1C;
	AudioServiceInfo info;
};

AudioChannel * __fastcall AudioDeviceReserveChannel(AudioDevice * dev, int newType);
void __fastcall AudioChannelRelease(AudioChannel * chan);
void __fastcall AudioChannelStop(AudioChannel * chan);
void __fastcall AudioChannelPause(AudioChannel * chan);
void __fastcall AudioChannelResume(AudioChannel * chan);
int __fastcall AudioChannelStart(AudioChannel * chan);
void __fastcall AudioChannelSetSample(AudioChannel * chan, AudioSample * sample);

StmStream * FUN_004087F0(void);
void __fastcall FUN_00408840(StmStream * stream);
void __fastcall FUN_00408970(StmStream * stream);
void __fastcall FUN_00408A70(StmStream * stream);
void __fastcall FUN_00408B10(StmStream * stream, StmBuffer * buffer);
StmBlock * __fastcall FUN_00408C30(StmStream * stream, int id);
unsigned __fastcall FUN_00408D50(StmStream * stream);
StmBuffer * __fastcall FUN_00408D80(unsigned size, int flags);
void __fastcall FUN_00408E70(StmBlock * access);
int __fastcall FUN_00408F80(StmBlock * access, FileClass * file, int bytes, int * transfered);
void __fastcall FUN_004091C0(StmBlock * access, int bytes);
void __fastcall FUN_00409310(StmBlock * access);
void __cdecl FUN_004068E0(const char * format, ...);

static DWORD WINAPI streamerThread(void * data);
static int __fastcall streamNextFrame(AudioChannel * channel);
static int __fastcall streamSampleDone(AudioChannel * channel);
static int __fastcall streamStop(AudioChannel * channel);

static StreamThread thread;
static HANDLE threadHandle;
static DWORD threadId;
static CRITICAL_SECTION streamLock;
static ListHead streams;
static int initialized;
static int threadCreated;

#define max(a, b) (((a) > (b)) ? (a) : (b))

static __forceinline int recalcBuffering(AudioStreamer * as, TimeStamp buffering, AudioFormat * format)
{
	unsigned size;
	StmStream * stm;
	StmBuffer * buffer;

	stm = as->stream;
	size = AudioFormatBytes(format, buffering);
	as->frame_size = AudioFormatBytes(format, as->channel->frameTime * 2);
	if (FUN_00408D50(stm) != size) {
		FUN_00408970(stm);
		buffer = FUN_00408D80(size, 0);
		if (buffer) {
			FUN_00408B10(stm, buffer);
		}
	}
	return FUN_00408D50(stm) == size;
}

static int streamFill(AudioStreamer * as, TimeStamp amount)
{
	int bytes;
	int transfered;
	int result;

	EnterCriticalSection(&streamLock);
	if (amount) {
		bytes = AudioFormatBytes(&as->format, amount);
		if (bytes > as->bytes_left) {
			bytes = as->bytes_left;
		}
	} else {
		bytes = as->bytes_left;
	}
	Game::IsReadingAudioFile = true;
	result = FUN_00408F80(as->in, as->file, bytes, &transfered);
	Game::IsReadingAudioFile = false;
	as->bytes_left -= transfered;
	if (as->bytes_left <= 0 || result == STM_EOF || result == STM_FAIL) {
		as->flags &= ~AS_FILL;
	}
	LeaveCriticalSection(&streamLock);
	return transfered;
}

static void streamStopChannel(AudioStreamer * as)
{
	if (NotLocked(&as->lock) && as->channel) {
		EnterCriticalSection(&streamLock);
		AudioChannelStop(as->channel);
		LeaveCriticalSection(&streamLock);
	}
}

// FUNCTION: GAMEMD 0x00407550
int AudioStreamerInit(void)
{
	char buffer[1024];

	ListInit(&streams);
	InitializeCriticalSection(&streamLock);
	thread.quit = 0;
	thread.done = 0;
	thread.running = 0;
	thread.interval = 1000;
	thread.count = 0;
	AudioServiceInfoInit(&thread.info);
	AudioServiceSetInterval(&thread.info, thread.interval);
	AudioServiceSetMustServiceInterval(&thread.info, thread.interval * 4);
	AudioServiceSetResetInterval(&thread.info, thread.interval * 4);
	threadHandle = CreateThread(NULL, 0x1000, (LPTHREAD_START_ROUTINE)streamerThread, &thread, 0, &threadId);
	if (!threadHandle) {
		return 0;
	}
	if (!SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST)) {
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, buffer, sizeof(buffer), NULL);
		FUN_004068E0("Unable to change the priority of the Streamer Thread - %s\n", buffer);
	}
	threadCreated = 1;
	initialized = 1;
	return 1;
}

// FUNCTION: GAMEMD 0x00407680
static DWORD WINAPI streamerThread(void * data)
{
	StreamThread * t = (StreamThread *)data;
	AudioStreamer * as;

	t->running = 1;
	t->done = 0;
	while (!t->quit) {
		AudioServicePerform(&t->info, AudioGetTime());
		EnterCriticalSection(&streamLock);
		for (as = (AudioStreamer *)ListFirstItem(&streams); as; as = (AudioStreamer *)ListNextItem(&as->nd)) {
			if (as->flags & AS_FILL) {
				streamFill(as, 0);
			}
		}
		LeaveCriticalSection(&streamLock);
		Sleep((DWORD)t->interval);
		t->count++;
	}
	t->done = 1;
	return 0;
}

// FUNCTION: GAMEMD 0x00407780
void AudioStreamerDeinit(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return;
	}
	if (threadCreated) {
		thread.quit = 1;
		while (!thread.done) {
			Sleep(0);
		}
		WaitForSingleObject(threadHandle, 5000);
		CloseHandle(threadHandle);
		threadCreated = 0;
	}
	while ((as = (AudioStreamer *)ListFirstItem(&streams))) {
		streamStopChannel(as);
		AudioStreamerDestroy(as);
	}
	DeleteCriticalSection(&streamLock);
	initialized = 0;
}

// FUNCTION: GAMEMD 0x00407860
AudioStreamer * __fastcall AudioStreamerCreate(AudioDevice * dev, TimeStamp buffering)
{
	AudioStreamer * as;

	if (!dev) {
		return NULL;
	}
	if (buffering <= 0 || buffering < SECONDS(7)) {
		buffering = SECONDS(7);
	}
	as = (AudioStreamer *)operator new(sizeof(AudioStreamer));
	memset(as, 0, sizeof(AudioStreamer));
	ListNodeInit(&as->nd);
	as->device = dev;
	as->channel = AudioDeviceReserveChannel(dev, 0x1000);
	as->channel->GroupAttribs = NULL;
	as->channel->SfxAttribs = NULL;
	as->channel->CompAttribs = NULL;
	as->channel->FadeAttribs = NULL;
	as->buffering = buffering;
	AudioSampleInit(&as->sample);
	AudioFormatInit(&as->format);
	LockInit(&as->pause);
	LockInit(&as->lock);
	strcpy(as->stream_name, "Audio Stream");
	if (!as->channel) {
		goto error;
	}
	as->channel->Data = as;
	as->channel->CB_NextFrame = streamNextFrame;
	as->channel->CB_SampleDone = streamSampleDone;
	as->channel->CB_Stop = streamStop;
	as->stream = FUN_004087F0();
	if (!as->stream) {
		goto error;
	}
	EnterCriticalSection(&streamLock);
	ListNodeAppend(&streams, &as->nd);
	LeaveCriticalSection(&streamLock);
	return as;
error:
	AudioStreamerDestroy(as);
	return NULL;
}

// FUNCTION: GAMEMD 0x004079C0
static int __fastcall streamNextFrame(AudioChannel * channel)
{
	AudioStreamer * as = (AudioStreamer *)channel->Data;

	FUN_004091C0(as->out, as->pending_bytes);
	FUN_00409310(as->out);
	if (as->frame_size > as->out->Bytes) {
		as->pending_bytes = as->out->Bytes;
	} else {
		as->pending_bytes = as->frame_size;
	}
	channel->bytesInFrame = as->pending_bytes;
	channel->bytesRemaining = channel->bytesInFrame;
	channel->frameData = as->out->Data;
	return 0;
}

// FUNCTION: GAMEMD 0x00407A30
static int __fastcall streamSampleDone(AudioChannel * channel)
{
	AudioStreamer * as = (AudioStreamer *)channel->Data;

	if (as->flags & (AS_PLAYING | AS_PAUSED)) {
		as->end_time_stamp = AudioGetTime();
	}
	as->flags &= ~(AS_PLAYING | AS_PAUSED | AS_FILL);
	LockInit(&as->pause);
	return 0;
}

// FUNCTION: GAMEMD 0x00407A60
static int __fastcall streamStop(AudioChannel * channel)
{
	AudioStreamer * as = (AudioStreamer *)channel->Data;

	if (as->flags & (AS_PLAYING | AS_PAUSED)) {
		as->end_time_stamp = AudioGetTime();
	}
	as->flags &= ~(AS_PLAYING | AS_PAUSED | AS_FILL);
	LockInit(&as->pause);
	return 0;
}

// FUNCTION: GAMEMD 0x00407A90
void __fastcall AudioStreamerDestroy(AudioStreamer * as)
{
	if (as->nd.prev != &as->nd) {
		EnterCriticalSection(&streamLock);
		ListNodeRemove(&as->nd);
		LeaveCriticalSection(&streamLock);
	}
	streamStopChannel(as);
	if (as->in) {
		FUN_00408E70(as->in);
		as->in = NULL;
	}
	if (as->out) {
		FUN_00408E70(as->out);
		as->out = NULL;
	}
	if (as->stream) {
		FUN_00408840(as->stream);
	}
	if (as->channel) {
		AudioChannelRelease(as->channel);
	}
	if (as->file) {
		delete as->file;
	}
	operator delete(as);
}

// FUNCTION: GAMEMD 0x00407B40
void __fastcall AudioStreamerSetAttribs(AudioStreamer * as, AudioAttribs * attribs)
{
	as->channel->GroupAttribs = attribs;
}

// FUNCTION: GAMEMD 0x00407B50
void __fastcall AudioStreamerSetFadeAttribs(AudioStreamer * as, AudioAttribs * attribs)
{
	as->channel->FadeAttribs = attribs;
}

// FUNCTION: GAMEMD 0x00407B60
int __fastcall AudioStreamerPlayWAV(AudioStreamer * as, const char * filename, int flag)
{
	TimeStamp time;
	TimeStamp buf;

	if (Locked(&as->lock)) {
		return 0;
	}
	streamStopChannel(as);
	if (as->file) {
		delete as->file;
		as->file = NULL;
	}
	if (!flag) {
		as->file = new RawFileClass(filename);
		if (as->file && !as->file->Is_Available(0)) {
			delete as->file;
			as->file = NULL;
		}
	}
	if (!as->file) {
		as->file = new CCFileClass(filename);
	}
	if (!as->file) {
		goto error;
	}
	if (!as->file->Is_Available(0) || !as->file->Open(FileClass::READ)) {
		delete as->file;
		as->file = NULL;
		goto error;
	}
	as->file->SkipCDCheck = true;
	if (!AudioFormatReadWaveFile(as->file, &as->format, &as->total_bytes)) {
		goto error;
	}
	as->bytes_left = as->total_bytes;
	as->data_start = as->file->Seek(0, SEEK_CUR);
	if (as->in) {
		FUN_00408E70(as->in);
		as->in = NULL;
	}
	if (as->out) {
		FUN_00408E70(as->out);
		as->out = NULL;
	}
	if (!recalcBuffering(as, as->buffering, &as->format)) {
		goto cleanup;
	}
	FUN_00408A70(as->stream);
	as->in = FUN_00408C30(as->stream, 0);
	as->out = FUN_00408C30(as->stream, 1);
	if (!as->in || !as->out) {
		goto cleanup;
	}
	buf = as->channel->bufferTime * 2;
	time = max(buf, thread.interval * 4);
	time = max(time, (TimeStamp)SECONDS(4));
	if (!streamFill(as, time)) {
		goto cleanup;
	}
	FUN_00409310(as->out);
	as->sample.Data = as->out->Data;
	as->sample.Bytes = AudioFormatBytes(&as->format, as->channel->frameTime);
	if (as->sample.Bytes > as->out->Bytes) {
		as->sample.Bytes = as->out->Bytes;
	}
	as->sample.Format = &as->format;
	as->pending_bytes = as->sample.Bytes;
	AudioLevelSet(&as->channel->ChannelAttribs.VolumeLevel, AUDIO_VOLUME_MAX);
	AudioChannelSetSample(as->channel, &as->sample);
	as->flags |= AS_PLAYING;
	if (AudioChannelStart(as->channel)) {
		as->flags &= ~AS_PLAYING;
		FUN_00408E70(as->in);
		as->in = NULL;
		FUN_00408E70(as->out);
		as->out = NULL;
		goto cleanup;
	}
	as->start_time_stamp = AudioGetTime();
	as->end_time_stamp = as->start_time_stamp;
	as->flags |= AS_FILL;
	return 1;
cleanup:
error:
	if (as->in) {
		FUN_00408E70(as->in);
		as->in = NULL;
	}
	if (as->out) {
		FUN_00408E70(as->out);
		as->out = NULL;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00407F40
void __fastcall AudioStreamerStop(AudioStreamer * as)
{
	streamStopChannel(as);
}

// FUNCTION: GAMEMD 0x00407F80
void __fastcall AudioStreamerLock(AudioStreamer * as)
{
	LockAcquire(&as->lock);
}

// FUNCTION: GAMEMD 0x00407F90
void __fastcall AudioStreamerUnlock(AudioStreamer * as)
{
	LockRelease(&as->lock);
}

// FUNCTION: GAMEMD 0x00407FB0
void __fastcall AudioStreamerPause(AudioStreamer * as)
{
	if (NotLocked(&as->pause) && as->channel) {
		EnterCriticalSection(&streamLock);
		AudioChannelPause(as->channel);
		if (as->flags & AS_PLAYING) {
			as->flags = (as->flags & ~AS_PLAYING) | AS_PAUSED;
		}
		LeaveCriticalSection(&streamLock);
	}
	LockAcquire(&as->pause);
}

// FUNCTION: GAMEMD 0x00408000
void __fastcall AudioStreamerResume(AudioStreamer * as)
{
	if (Locked(&as->pause)) {
		LockRelease(&as->pause);
		if (NotLocked(&as->pause) && as->channel) {
			EnterCriticalSection(&streamLock);
			if (as->flags & AS_PAUSED) {
				as->flags = (as->flags & ~AS_PAUSED) | AS_PLAYING;
			}
			AudioChannelResume(as->channel);
			LeaveCriticalSection(&streamLock);
		}
	}
}

// FUNCTION: GAMEMD 0x00408070
int __fastcall AudioStreamerIsPlaying(AudioStreamer * as)
{
	return as->flags & AS_PLAYING;
}

// FUNCTION: GAMEMD 0x00408080
void __fastcall AudioStreamerSetName(AudioStreamer * as, const char * name)
{
	strncpy(as->stream_name, name, sizeof(as->stream_name));
	as->stream_name[sizeof(as->stream_name) - 1] = 0;
}

// FUNCTION: GAMEMD 0x004080A0
const char * __fastcall AudioStreamerName(AudioStreamer * as)
{
	return as->stream_name;
}

// FUNCTION: GAMEMD 0x004080B0
void __fastcall AudioStreamerFadeIn(AudioStreamer * as)
{
	AudioLevelAdjust(&as->channel->ChannelAttribs.VolumeLevel, AUDIO_VOLUME_MAX);
}

// FUNCTION: GAMEMD 0x004080C0
void __fastcall AudioStreamerFadeOut(AudioStreamer * as)
{
	AudioLevelAdjust(&as->channel->ChannelAttribs.VolumeLevel, 0);
}

// FUNCTION: GAMEMD 0x004080D0
int __fastcall AudioStreamerIsFading(AudioStreamer * as)
{
	return (as->flags & AS_PLAYING) && as->channel->ChannelAttribs.VolumeLevel.level != as->channel->ChannelAttribs.VolumeLevel.newLevel;
}

// FUNCTION: GAMEMD 0x004080F0
void __fastcall AudioStreamerWaitForFade(AudioStreamer * as)
{
	TimeStamp time_out = AudioGetTime() + as->channel->ChannelAttribs.VolumeLevel.duration;

	while (AudioStreamerIsFading(as) && AudioGetTime() < time_out) {
		Game::CallBack();
	}
}

// FUNCTION: GAMEMD 0x00408140
TimeStamp __fastcall AudioStreamerEndTimeStamp(AudioStreamer * as)
{
	return as->end_time_stamp;
}

// FUNCTION: GAMEMD 0x00408150
void AudioStreamerStarve(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return;
	}
	as = (AudioStreamer *)ListFirstItem(&streams);
	EnterCriticalSection(&streamLock);
	while (as) {
		as->flags &= ~AS_FILL;
		as = (AudioStreamer *)ListNextItem(&as->nd);
	}
	LeaveCriticalSection(&streamLock);
}

// FUNCTION: GAMEMD 0x004081A0
void AudioStreamerStopAllStreams(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return;
	}
	as = (AudioStreamer *)ListFirstItem(&streams);
	while (as) {
		streamStopChannel(as);
		as = (AudioStreamer *)ListNextItem(&as->nd);
	}
}

// FUNCTION: GAMEMD 0x00408200
void AudioStreamerLockAllStreams(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return;
	}
	as = (AudioStreamer *)ListFirstItem(&streams);
	while (as) {
		LockAcquire(&as->lock);
		as = (AudioStreamer *)ListNextItem(&as->nd);
	}
}

// FUNCTION: GAMEMD 0x00408230
void AudioStreamerUnlockAllStreams(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return;
	}
	as = (AudioStreamer *)ListFirstItem(&streams);
	while (as) {
		LockRelease(&as->lock);
		as = (AudioStreamer *)ListNextItem(&as->nd);
	}
}

// FUNCTION: GAMEMD 0x00408270
void AudioStreamerPauseAllStreams(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return;
	}
	as = (AudioStreamer *)ListFirstItem(&streams);
	while (as) {
		AudioStreamerPause(as);
		as = (AudioStreamer *)ListNextItem(&as->nd);
	}
}

// FUNCTION: GAMEMD 0x004082F0
void AudioStreamerResumeAllStreams(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return;
	}
	as = (AudioStreamer *)ListFirstItem(&streams);
	while (as) {
		AudioStreamerResume(as);
		as = (AudioStreamer *)ListNextItem(&as->nd);
	}
}

// FUNCTION: GAMEMD 0x00408390
void AudioStreamerFadeOutAllStreams(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return;
	}
	as = (AudioStreamer *)ListFirstItem(&streams);
	while (as) {
		AudioStreamerFadeOut(as);
		as = (AudioStreamer *)ListNextItem(&as->nd);
	}
}

// FUNCTION: GAMEMD 0x004083D0
void AudioStreamerFadeInAllStreams(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return;
	}
	as = (AudioStreamer *)ListFirstItem(&streams);
	while (as) {
		AudioStreamerFadeIn(as);
		as = (AudioStreamer *)ListNextItem(&as->nd);
	}
}

// FUNCTION: GAMEMD 0x00408410
int AudioStreamerAllFaded(void)
{
	AudioStreamer * as;

	if (!initialized) {
		return 1;
	}
	as = (AudioStreamer *)ListFirstItem(&streams);
	while (as) {
		if (AudioStreamerIsFading(as)) {
			return 0;
		}
		as = (AudioStreamer *)ListNextItem(&as->nd);
	}
	return 1;
}
