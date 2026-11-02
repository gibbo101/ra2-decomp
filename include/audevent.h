// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audlist.h"
#include "audattr.h"
#include "audlock.h"
#include "audtimer.h"

class VocClass;
struct AudioCache;
struct AudioCacheItem;
struct AudioChannel;
struct AudioSample;
struct AudioController;

#define AUDIO_INVALID_STAMP 0
#define vNO_ERROR 0
#define AUDIO_MAX_EVENTS 300
#define EVENT_MIN_DELAY 33
#define NUM_BUCKETS_PRI 7
#define NUM_BUCKETS_VOL 10
#define VOLUME_QUANTIZE (AUDIO_LEVEL_MAX / NUM_BUCKETS_VOL)
#define MAX_AUDIO_EVENT_SAMPLES 32

enum AudioEventState
{
	AUDIO_EVENT_NEW,
	AUDIO_EVENT_START_PLAYING,
	AUDIO_EVENT_WAITING,
	AUDIO_EVENT_PLAYING,
	AUDIO_EVENT_DONE
};

#define mAUDIO_EVENT_DEAD 0x01
#define mAUDIO_EVENT_PLAYING 0x02
#define mAUDIO_EVENT_NO_ATTACK 0x08
#define mAUDIO_EVENT_NO_DECAY 0x10
#define mAUDIO_EVENT_END 0x20
#define mAUDIO_EVENT_DO_END 0x40
#define mAUDIO_EVENT_ALLOCATED 0x80

#define AUDIO_EVENT_CTRL_LOOP 0x01
#define AUDIO_EVENT_CTRL_RANDOM 0x02
#define AUDIO_EVENT_CTRL_ALL 0x04
#define AUDIO_EVENT_CTRL_PREDELAY 0x08
#define AUDIO_EVENT_CTRL_INTERRUPT 0x10
#define AUDIO_EVENT_CTRL_ATTACK 0x20
#define AUDIO_EVENT_CTRL_DECAY 0x40
#define AUDIO_EVENT_CTRL_AMBIENT 0x80

void __cdecl FUN_004068E0(char const * fmt, ...);

class AudioEvent
{
	public:
		void FUN_004052E0(void);
		void FUN_004052F0(void);
		void FUN_004053E0(void);
		void FUN_00405430(void);

		ListNode nd;
		ListNode local;
		volatile int flags;
		volatile int state;
		volatile int nextState;
		VocClass * eclass;
		AudioCacheItem * item[MAX_AUDIO_EVENT_SAMPLES];
		int numItems;
		int currentItem;
		AudioChannel * channel;
		unsigned : 32;
		AudioAttribs attribs;
		unsigned : 32;
		unsigned : 32;
		int stamp;
		volatile unsigned delay;
		volatile TimeStamp timeOut;
		AudioSample * volatile startSample;
		int fshift;
		int vshift;
		int adjustPriority;
		Lock paused;
		unsigned : 32;
		int sequence[MAX_AUDIO_EVENT_SAMPLES];
		int numSequence;
		int loopCount;
		int loadSequence[MAX_AUDIO_EVENT_SAMPLES];
		int numLoadSequence;
		int loadLoopCount;
		int loadNdx;
		int loadSaveNdx;
		AudioController * handle;
		unsigned : 32;
};

AudioEvent * __fastcall AudioEventCreate(VocClass * eclass);
