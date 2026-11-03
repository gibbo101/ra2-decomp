// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include "coord.h"
#include "rect.h"
#include "geometry.h"
#include <string.h>
#include <stdio.h>
#include "ccrand.h"
#include "voc.h"
#include "audevent.h"
#include "audcache.h"
#include "auddev.h"
#include "audmem.h"

namespace Randomizer { extern Random2Class Global; }

extern AudioAttribs * DAT_0087E748;
extern AudioAttribs * DAT_0087E758;
extern int DAT_00B1D3A8;
extern int DAT_00B1D3AC;
AudioSample * __fastcall eventNextSample(AudioEvent * event);
AudioSample * __fastcall eventFirstSample(AudioEvent * event);
int __fastcall eventLoadSchedule(AudioEvent * event);
AudioChannel * __fastcall audioPrepareChannel(int pri, int flags);
int __fastcall audioEventNextSample(AudioChannel * chan);
int __fastcall audioEventSampleDone(AudioChannel * chan);
int __fastcall audioEventStop(AudioChannel * chan);
int __fastcall AudioEventNeverEnds(AudioEvent * event);
void __fastcall AudioEventService(AudioEvent * event);
VocClass * __fastcall bucketGetWeaker(int priority);
void __fastcall eventLoadAndLockSamples(AudioEvent * event);
int __fastcall audioEventStart(AudioEvent * event);
static void eventWait(AudioEvent * event, TimeStamp delay, int nextState);

// SYNTHETIC: GAMEMD 0x00403CE0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00403D10 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00403D30 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00403D50 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00403D70 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00403D90 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00403DB0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00403DF0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00403E20 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00403E50 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00403E80 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00403E90 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00403EB0 SYMBOL
// _$E41

typedef struct
{
	ListHead bucketVol[NUM_BUCKETS_VOL];
} EClassBucket;

struct EventKeyword
{
	char const * Name;
	int Value;
};

static EventKeyword PriorityKeywords[] = {
	{"LOWEST", 0},
	{"LOW", 1},
	{"NORMAL", 2},
	{"HIGH", 3},
	{"CRITICAL", 4},
	{NULL, 2}
};

static EventKeyword TypeKeywords[] = {
	{"AMBIENT", 0x1000},
	{"VIOLENT", 0x1},
	{"MOVEMENT", 0x2},
	{"QUIET", 0x4},
	{"LOUD", 0x8},
	{"GLOBAL", 0x10},
	{"SCREEN", 0x20},
	{"LOCAL", 0x40},
	{"PLAYER", 0x80},
	{"NORMAL", 0x0},
	{"GUN_SHY", 0x200},
	{"NOISE_SHY", 0x100},
	{"UNSHROUD", 0x400},
	{"SHROUD", 0x800},
	{NULL, 0}
};

static EventKeyword ControlKeywords[] = {
	{"ALL", 0x4},
	{"LOOP", 0x1},
	{"RANDOM", 0x2},
	{"PREDELAY", 0x8},
	{"INTERRUPT", 0x10},
	{"ATTACK", 0x20},
	{"DECAY", 0x40},
	{"AMBIENT", 0x80},
	{NULL, 0}
};

static EClassBucket eClassBucket[NUM_BUCKETS_PRI];
static ListHead audioEvents;
static AudioAttribs audioCompressionAttribs;
static ListHead audioEventClasses;
int AudioEventsCount;
int AudioEventsPeak;
AudioIDXData * AudioIDXData::Instance;
static AudioCache * audioCache;
static AudioDevice * audioDevice;
static int initialized;
static int eventsOK;
static AudioMemoryPool * audioEventPool;
static int frameStamp;
static unsigned bucketStamp;
static int service_in;
static int eventsOn = 1;
static int audioStamp = AUDIO_INVALID_STAMP + 1;

void __fastcall eventReleaseChannel(AudioEvent * event);
void __fastcall eventUnlockSamples(AudioEvent * event);
void __fastcall audioEventDestroy(AudioEvent * event);
void __fastcall AudioEventKill(AudioEvent * event);
void AudioKillAllEvents(void);
void AudioFlushAllDeadEvents(void);
int __fastcall AudioEventIsPaused(AudioEvent * event);

static inline int AudioRandomPick(int lo, int hi)
{
	return Randomizer::Global(lo, hi);
}

static void eventLoadItem(AudioEvent * event, int sampleIndex)
{
	if ((event->item[event->numItems] = AudioCacheLoadItem(audioCache, event->eclass->SampleIndex[sampleIndex]))) {
		AudioCacheItemLock(event->item[event->numItems]);
		event->numItems++;
	}
}

// FUNCTION: GAMEMD 0x00403ED0
int __fastcall AudioEventSetUp(AudioDevice * device, AudioIDXData * idx)
{
	ListInit(&audioEvents);
	ListInit(&audioEventClasses);
	AudioIDXData::Instance = idx;
	audioDevice = device;
	AudioAttribsInit(&audioCompressionAttribs);
	AudioEventsPeak = 0;
	AudioEventsCount = 0;
	initialized = 1;
	eventsOn = 1;
	if (AudioIDXData::Instance) {
		audioCache = AudioCacheCreate(AudioIDXData::Instance, 0x100000, 200, 0x2000);
		audioEventPool = MemoryPoolCreate(AUDIO_MAX_EVENTS, sizeof(AudioEvent));
		FUN_004068E0("event mempool size = %d\n", AUDIO_MAX_EVENTS * sizeof(AudioEvent));
	}
	if (audioCache && AudioIDXData::Instance && audioDevice && audioEventPool) {
		eventsOK = 1;
		return 1;
	}
	return 0;
}

static void eventDestroyAllClasses(void)
{
	VocClass * eclass;
	while ((eclass = (VocClass *)ListFirstItem(&audioEventClasses))) {
		ListNodeRemove(&eclass->Header);
		delete eclass;
	}
}

// FUNCTION: GAMEMD 0x00403FA0
void AudioEventCloseDown(void)
{
	AudioKillAllEvents();
	AudioFlushAllDeadEvents();

	eventDestroyAllClasses();
	if (audioCache) {
		AudioCacheDestroy(audioCache);
		audioCache = NULL;
	}
	if (audioEventPool) {
		MemoryPoolDestroy(audioEventPool);
		audioEventPool = NULL;
	}
	eventsOK = 0;
	initialized = 0;
}


// FUNCTION: GAMEMD 0x00404140
void __fastcall eventReleaseChannel(AudioEvent * event)
{
	if (event->channel && event == (AudioEvent *)event->channel->Data) {
		if (event->flags & mAUDIO_EVENT_PLAYING) {
			AudioChannelStop(event->channel);
		}
		AudioChannelNoUse(event->channel);
		event->channel->Data = NULL;
		event->channel->CB_Stop = NULL;
		event->channel->CB_NextSample = NULL;
		event->channel->CB_SampleDone = NULL;
		event->channel->SfxAttribs = NULL;
		event->channel->CompAttribs = NULL;
		event->channel->FadeAttribs = NULL;
		event->channel = NULL;
	}
}

static void eventAddLocalSort(AudioEvent * newEvent)
{
	VocClass * eclass;
	ListNode * node, * head;
	AudioEvent * event;
	unsigned volume, newVolume;

	eclass = newEvent->eclass;
	newVolume = AudioAttribsGetVolume(&newEvent->attribs);
	head = &eclass->local;
	node = head->next;

	while (node != head) {
		int dif, slack;
		unsigned baseLev;
		event = (AudioEvent *)((char *)node - offsetof(AudioEvent, local));
		volume = AudioAttribsGetVolume(&event->attribs);
		baseLev = (unsigned)eclass->Volume.level >> AUDIO_LEVEL_SCALE;
		slack = baseLev / NUM_BUCKETS_VOL;
		dif = volume - newVolume;
		if ((dif >= 0 ? dif : -dif) < slack) {
			if (eclass->Control & AUDIO_EVENT_CTRL_INTERRUPT) {
				if (event->state != AUDIO_EVENT_NEW) {
					break;
				}
			} else {
				if (event->state == AUDIO_EVENT_NEW) {
					break;
				}
			}
		} else if (volume < newVolume) {
			break;
		}
		node = node->next;
	}

	ListNodeInit(&newEvent->local);
	ListNodeInsert(node, &newEvent->local);
	eclass->Count++;
	if (eclass->Limit && (eclass->Count > eclass->Limit)) {
		event = (AudioEvent *)((char *)head->prev - offsetof(AudioEvent, local));
		ListNodeRemove(&event->local);
		AudioEventKill(event);
		eclass->Count--;
	}
}

static void bucketReset(void)
{
	int i, k;
	EClassBucket * ecb;
	for (i = 0, ecb = eClassBucket; i < NUM_BUCKETS_PRI; i++, ecb++) {
		for (k = 0; k < NUM_BUCKETS_VOL; k++) {
			ListInit(&ecb->bucketVol[k]);
		}
	}
	bucketStamp++;
}

static void bucketAdd(AudioEvent * event)
{
	EClassBucket * ecb;
	VocClass * eclass;
	int pri;
	unsigned vol;

	eclass = event->eclass;
	pri = eclass->Priority + event->adjustPriority;
	vol = AudioAttribsGetVolume(&event->attribs) / VOLUME_QUANTIZE;
	ecb = &eClassBucket[pri];

	if (eclass->lastBucketStamp == bucketStamp) {
		if (pri > eclass->maxPri) {
			eclass->maxPri = pri;
			eclass->maxVol = vol;
			ListNodeRemove(&eclass->nodeVol);
			ListNodeInsert(&ecb->bucketVol[vol], &eclass->nodeVol);
		} else if (pri == eclass->maxPri) {
			if (vol > eclass->maxVol) {
				eclass->maxVol = vol;
				ListNodeRemove(&eclass->nodeVol);
				ListNodeInsert(&ecb->bucketVol[vol], &eclass->nodeVol);
			}
		}
	} else {
		eclass->lastBucketStamp = bucketStamp;
		eclass->maxPri = pri;
		eclass->maxVol = vol;
		ListNodeInit(&eclass->nodeVol);
		ListNodeInsert(&ecb->bucketVol[vol], &eclass->nodeVol);
	}
}

static int eventClassSacrifice(AudioEvent * newEvent)
{
	AudioEvent * event, * next, * head;
	VocClass * eclassSacrifice = NULL;
	int killed = 0;
	eclassSacrifice = bucketGetWeaker(newEvent->eclass->Priority + newEvent->adjustPriority);
	if (eclassSacrifice == NULL) {
		return 0;
	}
	ListNodeRemove(&eclassSacrifice->nodeVol);
	head = (AudioEvent *)&audioEvents;
	event = (AudioEvent *)head->nd.next;
	while (event != head) {
		next = (AudioEvent *)event->nd.next;
		if (event->eclass == eclassSacrifice) {
			AudioEventKill(event);
			audioEventDestroy(event);
			killed = 1;
		}
		event = next;
	}
	return killed;
}

// FUNCTION: GAMEMD 0x004041D0
void AudioServiceAllEvents(void)
{
	AudioEvent * event, * next, * head;
	enum { AVERAGE_LOUDNESS = 20, FULL_LOUDNESS = 100 };
	int loudness;
	// The service statistics count active events before each update.
	int processedEvents = 0;

	if (!initialized || service_in) {
		return;
	}
	service_in = 1;
	frameStamp++;
	head = (AudioEvent *)&audioEvents;

	// Sort each class by volume and enforce its event limit.
	event = (AudioEvent *)head->nd.next;
	while (event != head) {
		if (event->eclass->lastFrame != frameStamp) {
			ListInit(&event->eclass->local);
			event->eclass->Count = 0;
			event->eclass->lastFrame = frameStamp;
		}
		AudioAttribsUpdate(&event->attribs);
		if (event->state != AUDIO_EVENT_DONE) {
			eventAddLocalSort(event);
		}
		event = (AudioEvent *)event->nd.next;
	}

	// Service events before measuring the active channels.
	event = (AudioEvent *)head->nd.next;
	while (event != head) {
		if (event->state != AUDIO_EVENT_DONE) {
			processedEvents++;
		}
		AudioEventService(event);
		event = (AudioEvent *)event->nd.next;
	}

	// Remove dead events and compute volume compression.
	event = (AudioEvent *)head->nd.next;
	loudness = 0;
	bucketReset();
	while (event != head) {
		next = (AudioEvent *)event->nd.next;
		if (!event->channel || event != (AudioEvent *)event->channel->Data) {
			AudioEventKill(event);
		}
		if (event->flags & mAUDIO_EVENT_DEAD) {
			audioEventDestroy(event);
		} else {
			if (!AudioEventIsPaused(event)) {
				loudness += AUDIO_LEVEL_APPLY(&event->eclass->Volume, AVERAGE_LOUDNESS);
			}
			bucketAdd(event);
		}
		event = next;
	}

	if (loudness > FULL_LOUDNESS) {
		AudioLevelAdjust(&audioCompressionAttribs.VolumeLevel, (unsigned)(AUDIO_LEVEL_MAX * FULL_LOUDNESS) / loudness);
	} else {
		AudioLevelAdjust(&audioCompressionAttribs.VolumeLevel, AUDIO_LEVEL_MAX);
	}
	AudioAttribsUpdate(&audioCompressionAttribs);

	// Load samples and start events at the new compression level.
	event = (AudioEvent *)head->nd.next;
	while (event != head) {
		next = (AudioEvent *)event->nd.next;
		if (event->state == AUDIO_EVENT_START_PLAYING && !AudioEventIsPaused(event)) {
			eventLoadAndLockSamples(event);
			while (!(event->startSample = eventFirstSample(event))) {
				if (eventClassSacrifice(event)) {
					next = (AudioEvent *)event->nd.next;
					eventLoadAndLockSamples(event);
				} else {
					break;
				}
			}
			if (!(event->startSample = eventFirstSample(event)) || !audioEventStart(event)) {
				AudioEventKill(event);
				audioEventDestroy(event);
			}
		}
		event = next;
	}

	service_in = 0;
}

// FUNCTION: GAMEMD 0x00404700
AudioSample * __fastcall eventFirstSample(AudioEvent * event)
{
	int j = 0;
	int i;

	if ((event->numSequence = event->numItems) == 0 || event->flags & mAUDIO_EVENT_END) {
		return NULL;
	}
	if (event->eclass->MinDelay < EVENT_MIN_DELAY) {
		if (event->eclass->Control & AUDIO_EVENT_CTRL_ATTACK) {
			j++;
			event->numSequence--;
			event->currentItem = 0;
		} else {
			event->currentItem = -1;
		}
		if (event->eclass->Control & AUDIO_EVENT_CTRL_DECAY) {
			event->numSequence--;
		}
		if (event->numSequence <= 0) {
			event->eclass->SamplesOK = 0;
			return NULL;
		}
		for (i = 0; i < event->numSequence; i++, j++) {
			event->sequence[i] = j;
		}
		if (event->flags & mAUDIO_EVENT_NO_ATTACK || !(event->eclass->Control & AUDIO_EVENT_CTRL_ATTACK)) {
			return eventNextSample(event);
		}
	}
	return AudioCacheItemSample(event->item[0]);
}

// FUNCTION: GAMEMD 0x004047B0
AudioSample * __fastcall eventNextSample(AudioEvent * event)
{
	int i;
	int control;

	control = event->eclass->Control;
	if (event->eclass->MinDelay >= EVENT_MIN_DELAY && !(event->flags & mAUDIO_EVENT_END)) {
		return NULL;
	}
	if (event->numSequence == 0 || event->flags & mAUDIO_EVENT_END) {
		if (control & AUDIO_EVENT_CTRL_LOOP && !(event->flags & mAUDIO_EVENT_END)) {
			if (!event->eclass->Loop || (event->loopCount < event->eclass->Loop - 1)) {
				event->loopCount++;
				return eventFirstSample(event);
			}
		}
		if ((!(event->eclass->Control & AUDIO_EVENT_CTRL_DECAY) || event->flags & mAUDIO_EVENT_NO_DECAY) || event->numItems < 1) {
			return NULL;
		}
		event->flags |= mAUDIO_EVENT_NO_DECAY;
		return AudioCacheItemSample(event->item[event->numItems - 1]);
	}
	if (control & AUDIO_EVENT_CTRL_RANDOM) {
		int ndx;

		ndx = AudioRandomPick(0, event->numSequence - 1);
		event->currentItem = event->sequence[ndx];
		for (i = ndx; i < event->numSequence - 1; i++) {
			event->sequence[i] = event->sequence[i + 1];
		}
	} else {
		event->currentItem++;
	}
	event->numSequence--;
	return AudioCacheItemSample(event->item[event->currentItem]);
}

// FUNCTION: GAMEMD 0x004048B0
void __fastcall eventLoadAndLockSamples(AudioEvent * event)
{
	int i, ndx;
	int control;
	VocClass * eclass;

	control = event->eclass->Control;
	eclass = event->eclass;
	if (event->numItems) {
		if (eclass->MinDelay >= EVENT_MIN_DELAY) {
			eventUnlockSamples(event);
		} else {
			return;
		}
	}
	if (eclass->MinDelay < EVENT_MIN_DELAY) {
		if (eclass->Attack) {
			eventLoadItem(event, AudioRandomPick(0, eclass->Attack - 1));
		}
		if (eclass->Control & AUDIO_EVENT_CTRL_ALL) {
			for (i = eclass->Attack; i < eclass->NumSamples - eclass->Decay; i++) {
				eventLoadItem(event, i);
			}
		} else if (eclass->Control & AUDIO_EVENT_CTRL_RANDOM) {
			i = AudioRandomPick(eclass->Attack, eclass->NumSamples - eclass->Decay - 1);
			eventLoadItem(event, i);
		} else {
			eventLoadItem(event, eclass->Attack);
		}
		if (eclass->Decay) {
			eventLoadItem(event, AudioRandomPick(eclass->NumSamples - eclass->Decay, eclass->NumSamples - 1));
		}
	} else {
		int decay_ndx = -1;

		if (eclass->Decay && eclass->Control & AUDIO_EVENT_CTRL_DECAY) {
			decay_ndx = AudioRandomPick(eclass->NumSamples - eclass->Decay, eclass->NumSamples - 1);
		}
		if ((ndx = eventLoadSchedule(event)) >= 0) {
			eventLoadItem(event, ndx);
			if (decay_ndx != -1) {
				eventLoadItem(event, decay_ndx);
			}
		} else {
			if (decay_ndx != -1 && !(event->flags & mAUDIO_EVENT_NO_DECAY)) {
				eventLoadItem(event, decay_ndx);
				event->flags |= mAUDIO_EVENT_NO_DECAY;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00404BB0
int __fastcall eventLoadSchedule(AudioEvent * event)
{
	VocClass * eclass;
	int control;
	int i;
	int item_ndx;

	eclass = event->eclass;
	control = eclass->Control;
	if (event->numLoadSequence == event->loadNdx) {
		int ndx = 0;

		if ((!(eclass->Control & AUDIO_EVENT_CTRL_LOOP) && event->loadLoopCount == 1)
			|| ((eclass->Control & AUDIO_EVENT_CTRL_LOOP) && eclass->Loop && (event->loadLoopCount) >= eclass->Loop)) {
			return -1;
		}
		if (eclass->Attack && !(event->flags & mAUDIO_EVENT_NO_ATTACK)) {
			event->loadSequence[ndx++] = AudioRandomPick(0, eclass->Attack - 1);
		}
		if (eclass->Control & AUDIO_EVENT_CTRL_ALL) {
			for (i = eclass->Attack; i < eclass->NumSamples - eclass->Decay; i++) {
				event->loadSequence[ndx++] = i;
			}
		} else {
			if (eclass->Control & AUDIO_EVENT_CTRL_RANDOM) {
				if (event->loadLoopCount == 0) {
					i = AudioRandomPick(eclass->Attack, eclass->NumSamples - (eclass->Decay + 1));
					event->loadSaveNdx = i;
				} else {
					i = event->loadSaveNdx;
				}
			} else {
				i = eclass->Attack;
			}
			event->loadSequence[ndx++] = i;
		}
		event->numLoadSequence = ndx;
		event->loadNdx = 0;
		event->loadLoopCount++;
	}
	if (control & AUDIO_EVENT_CTRL_RANDOM
		&& ((!eclass->Attack || (event->flags & mAUDIO_EVENT_NO_ATTACK)) || event->loadNdx > 0)) {
		int ndx, currentSampleIndex;

		ndx = AudioRandomPick((!eclass->Attack || (event->flags & mAUDIO_EVENT_NO_ATTACK)) ? 0 : 1, event->numLoadSequence - 1);
		item_ndx = currentSampleIndex = event->loadSequence[ndx];
		for (i = ndx; i < event->numLoadSequence - 1; i++) {
			event->loadSequence[i] = event->loadSequence[i + 1];
		}
		event->numLoadSequence--;
	} else {
		item_ndx = event->loadSequence[event->loadNdx];
		event->loadNdx++;
	}
	return item_ndx;
}

// FUNCTION: GAMEMD 0x00404D80
void __fastcall eventUnlockSamples(AudioEvent * event)
{
	int i;

	for (i = 0; i < event->numItems; i++) {
		AudioCacheItemUnlock(event->item[i]);
		event->item[i] = NULL;
	}
	event->numItems = 0;
}

// FUNCTION: GAMEMD 0x00404DD0
void __fastcall audioEventDestroy(AudioEvent * event)
{
	ListNodeRemove(&event->nd);
	event->stamp = AUDIO_INVALID_STAMP;
	if (event->flags & mAUDIO_EVENT_ALLOCATED) {
		delete event;
	} else {
		MemoryPoolReturnItem(audioEventPool, event);
	}
	AudioEventsCount--;
}

// FUNCTION: GAMEMD 0x00404E20
VocClass * __fastcall bucketGetWeaker(int pri)
{
	EClassBucket * ecb;
	ListNode * node;
	int i, v;

	for (i = 0, ecb = eClassBucket; i < pri; i++, ecb++) {
		for (v = 0; v < NUM_BUCKETS_VOL; v++) {
			node = ListFirstItem(&ecb->bucketVol[v]);
			if (node) {
				VocClass * event = (VocClass *)node;
				event = (VocClass *)((unsigned)event - ((unsigned)&event->nodeVol - (unsigned)event));
				return event;
			}
		}
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x00404E70
void AudioKillAllEvents(void)
{
	AudioEvent * event, * next, * head;

	if (!initialized) {
		return;
	}
	head = (AudioEvent *)&audioEvents;
	event = (AudioEvent *)head->nd.next;
	while (event != head) {
		next = (AudioEvent *)event->nd.next;
		AudioEventKill(event);
		audioEventDestroy(event);
		event = next;
	}
}

// FUNCTION: GAMEMD 0x00404FD0
void AudioPauseAllEvents(void)
{
	AudioEvent * event, * head;

	if (!initialized) {
		return;
	}
	head = (AudioEvent *)&audioEvents;
	event = (AudioEvent *)head->nd.next;
	while (event != head) {
		if (!AudioEventIsPaused(event)) {
			if (event->state == AUDIO_EVENT_WAITING) {
				event->timeOut -= AudioGetTime();
			}
		}
		LockAcquire(&event->paused);
		event = (AudioEvent *)event->nd.next;
	}
}

// FUNCTION: GAMEMD 0x00405040
void AudioResumeAllEvents(void)
{
	AudioEvent * event, * head;

	if (!initialized) {
		return;
	}
	head = (AudioEvent *)&audioEvents;
	event = (AudioEvent *)head->nd.next;
	while (event != head) {
		if (AudioEventIsPaused(event)) {
			LockRelease(&event->paused);
			if (!AudioEventIsPaused(event)) {
				if (event->state == AUDIO_EVENT_WAITING) {
					event->timeOut += AudioGetTime();
				}
			}
		}
		event = (AudioEvent *)event->nd.next;
	}
}

// FUNCTION: GAMEMD 0x004050C0
void AudioFlushAllDeadEvents(void)
{
	AudioEvent * event, * next, * head;

	if (!initialized) {
		return;
	}
	head = (AudioEvent *)&audioEvents;
	event = (AudioEvent *)head->nd.next;
	while (event != head) {
		next = (AudioEvent *)event->nd.next;
		if (event->flags & mAUDIO_EVENT_DEAD) {
			audioEventDestroy(event);
		}
		event = next;
	}
}

// FUNCTION: GAMEMD 0x00405130
int VocClass::FUN_00405130(void)
{
	if (!initialized) {
		return 0;
	}
	return Range;
}

// FUNCTION: GAMEMD 0x00405140
float VocClass::FUN_00405140(void)
{
	if (!initialized) {
		return 0.0f;
	}
	return MinVolume;
}

// FUNCTION: GAMEMD 0x00405160
int VocClass::FUN_00405160(void)
{
	if (!initialized) {
		return 0;
	}
	return Type;
}

// FUNCTION: GAMEMD 0x00405170
char const * VocClass::FUN_00405170(void)
{
	if (!initialized) {
		return "no events";
	}
	return Name;
}

// FUNCTION: GAMEMD 0x00405190
AudioEvent * __fastcall AudioEventCreate(VocClass * eclass)
{
	AudioEvent * event;

	if (!eventsOn || !eventsOK) {
		return NULL;
	}
	if (!eclass->SamplesOK) {
		return NULL;
	}
	if (!(event = (AudioEvent *)MemoryPoolGetItem(audioEventPool))) {
		AudioFlushAllDeadEvents();
		if (!(event = (AudioEvent *)MemoryPoolGetItem(audioEventPool))) {
			return NULL;
		}
	}
	AudioEventsCount++;
	if (AudioEventsCount > AudioEventsPeak) {
		AudioEventsPeak = AudioEventsCount;
	}
	memset(event, 0, sizeof(AudioEvent));
	event->eclass = eclass;
	ListNodeInit(&event->nd);
	event->state = AUDIO_EVENT_NEW;
	event->adjustPriority = 0;
	event->numItems = event->numSequence = event->numLoadSequence = 0;
	AudioAttribsInit(&event->attribs);
	LockInit(&event->paused);
	ListNodeInsert((ListNode *)&audioEvents, &event->nd);
	event->stamp = audioStamp++;
	return event;
}

// FUNCTION: GAMEMD 0x004052E0
void AudioEvent::FUN_004052E0(void)
{
	flags |= mAUDIO_EVENT_NO_ATTACK;
}

// FUNCTION: GAMEMD 0x004052F0
void __fastcall AudioEventKill(AudioEvent * event)
{
	if (event->flags & mAUDIO_EVENT_DEAD) {
		return;
	}
	eventReleaseChannel(event);
	eventUnlockSamples(event);
	event->state = AUDIO_EVENT_DONE;
	event->stamp = AUDIO_INVALID_STAMP;
	event->handle = NULL;
	event->flags |= mAUDIO_EVENT_DEAD;
}

// FUNCTION: GAMEMD 0x004053E0
void AudioEvent::FUN_004053E0(void)
{
	if (!AudioEventIsPaused(this)) {
		if (state == AUDIO_EVENT_WAITING) {
			timeOut -= AudioGetTime();
		}
	}
	LockAcquire(&paused);
}

// FUNCTION: GAMEMD 0x00405430
void AudioEvent::FUN_00405430(void)
{
	if (AudioEventIsPaused(this)) {
		LockRelease(&paused);
		if (!AudioEventIsPaused(this)) {
			if (state == AUDIO_EVENT_WAITING) {
				timeOut += AudioGetTime();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00405490
int __fastcall AudioEventIsPaused(AudioEvent * event)
{
	return event->paused.count;
}

// FUNCTION: GAMEMD 0x004054A0
int __fastcall audioEventStart(AudioEvent * event)
{
	AudioSample * sample;
	unsigned volume;

	if (event != (AudioEvent *)event->channel->Data) {
		return 0;
	}
	AudioChannelLock(event->channel);
	sample = (AudioSample *)event->startSample;
	event->flags |= mAUDIO_EVENT_NO_ATTACK;
	AudioChannelSetSample(event->channel, sample);
	AudioLevelSet(&event->channel->ChannelAttribs.PitchLevel, event->fshift);
	volume = AUDIO_LEVEL_MAX;
	if (event->vshift > 0) {
		volume -= (unsigned)(AUDIO_LEVEL_MAX * event->vshift) / 100;
	}
	AudioLevelSet(&event->channel->ChannelAttribs.VolumeLevel, volume);
	event->state = AUDIO_EVENT_PLAYING;
	event->flags |= mAUDIO_EVENT_PLAYING;
	event->startSample = NULL;
	if (AudioChannelStart(event->channel) != vNO_ERROR) {
		event->state = AUDIO_EVENT_START_PLAYING;
		event->startSample = sample;
		event->flags &= ~mAUDIO_EVENT_PLAYING;
		AudioChannelUnlock(event->channel);
		return 0;
	}
	AudioChannelUnlock(event->channel);
	return 1;
}

// FUNCTION: GAMEMD 0x00405580
void __fastcall AudioEventEnd(AudioEvent * event)
{
	if (event->flags & mAUDIO_EVENT_END) {
		return;
	}
	if (event->channel && event == (AudioEvent *)event->channel->Data) {
		AudioChannelLock(event->channel);
		event->flags |= mAUDIO_EVENT_END | mAUDIO_EVENT_DO_END;
		AudioChannelUnlock(event->channel);
	} else {
		event->flags |= mAUDIO_EVENT_END | mAUDIO_EVENT_DO_END;
	}
}

static int audioEventPrep(AudioEvent * event)
{
	VocClass * eclass;
	int delay;

	eclass = event->eclass;
	if (eclass->NumSamples == 0 || !eclass->SamplesOK) {
		return 0;
	}
	event->loopCount = 0;
	if (!(event->channel = audioPrepareChannel(eclass->Priority + event->adjustPriority, 0))) {
		return 0;
	}
	event->fshift = 100 + AudioRandomPick(eclass->MinFDelta, eclass->MaxFDelta);
	event->vshift = AudioRandomPick(0, eclass->VShift);
	AudioChannelUse(event->channel);
	event->channel->Data = event;
	event->channel->CB_NextSample = audioEventNextSample;
	event->channel->CB_SampleDone = audioEventSampleDone;
	event->channel->CB_Stop = audioEventStop;
	event->state = AUDIO_EVENT_START_PLAYING;
	event->channel->SfxAttribs = &event->attribs;
	event->channel->CompAttribs = &audioCompressionAttribs;
	event->channel->FadeAttribs = DAT_0087E758;
	if (eclass->Control & (AUDIO_EVENT_CTRL_PREDELAY | AUDIO_EVENT_CTRL_AMBIENT)) {
		delay = AudioRandomPick((eclass->Control & AUDIO_EVENT_CTRL_AMBIENT) ? EVENT_MIN_DELAY : eclass->MinDelay, eclass->MaxDelay);
		if (delay >= EVENT_MIN_DELAY) {
			eventWait(event, delay, AUDIO_EVENT_START_PLAYING);
		}
	}
	return 1;
}

static void eventCheckStart(AudioEvent * event, int succeeded)
{
	if (!succeeded) {
		AudioEventKill(event);
	}
}

// FUNCTION: GAMEMD 0x004055C0
void __fastcall AudioEventService(AudioEvent * event)
{
	if (!eventsOn) {
		event->state = AUDIO_EVENT_DONE;
	}

retry:
	switch (event->state) {
	case AUDIO_EVENT_WAITING:
		if (event->flags & mAUDIO_EVENT_DO_END) {
			goto do_ending;
		}
		if (!AudioEventIsPaused(event) && AudioGetTime() > event->timeOut) {
			event->state = event->nextState;
			goto retry;
		}
		break;

	case AUDIO_EVENT_NEW:
		eventCheckStart(event, audioEventPrep(event));
		return;

	case AUDIO_EVENT_PLAYING:
		if ((event->eclass->Control & AUDIO_EVENT_CTRL_LOOP) && !event->eclass->Loop
			&& !(event->flags & mAUDIO_EVENT_END)) {
			AudioController * controller = event->handle;
			if (!controller || controller->GetEvent() != event) {
				AudioEventKill(event);
			}
		}
		if (!(event->flags & mAUDIO_EVENT_DO_END)) {
			break;
		}
do_ending:
		{
			AudioChannel * channel = event->channel;
			if (channel) {
				AudioChannelLock(channel);
				event->flags &= ~mAUDIO_EVENT_DO_END;
				AudioChannelStop(event->channel);
				if ((event->eclass->Control & AUDIO_EVENT_CTRL_DECAY) && !(event->flags & mAUDIO_EVENT_NO_DECAY)) {
					event->state = AUDIO_EVENT_START_PLAYING;
					event->startSample = eventNextSample(event);
					if (!AudioEventIsPaused(event)) {
						eventCheckStart(event, audioEventStart(event));
					}
				}
				AudioChannelUnlock(channel);
			} else {
				event->flags &= ~mAUDIO_EVENT_DO_END;
			}
		}
		break;

	case AUDIO_EVENT_DONE:
		AudioEventKill(event);
		return;
	}
}

// FUNCTION: GAMEMD 0x004059F0
int __fastcall audioEventStop(AudioChannel * chan)
{
	AudioEvent * event = (AudioEvent *)chan->Data;

	event->state = AUDIO_EVENT_DONE;
	return vNO_ERROR;
}

static void eventWait(AudioEvent * event, TimeStamp delay, int nextState)
{
	event->nextState = nextState;
	if (AudioEventIsPaused(event)) {
		event->timeOut = delay;
	} else {
		event->timeOut = AudioGetTime() + delay;
	}
	event->state = AUDIO_EVENT_WAITING;
}

// FUNCTION: GAMEMD 0x00405A00
int __fastcall audioEventSampleDone(AudioChannel * chan)
{
	AudioEvent * event = (AudioEvent *)chan->Data;
	int new_state;

	event->flags &= ~mAUDIO_EVENT_PLAYING;
	if (event->state == AUDIO_EVENT_DONE) {
		return vNO_ERROR;
	}
	if (event->startSample || event->numLoadSequence || event->delay) {
		new_state = AUDIO_EVENT_START_PLAYING;
	} else {
		new_state = AUDIO_EVENT_DONE;
		event->delay = 0;
	}
	if (event->delay) {
		eventWait(event, event->delay, new_state);
		event->delay = 0;
	} else {
		event->state = new_state;
	}
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x00405AC0
int __fastcall audioEventNextSample(AudioChannel * chan)
{
	AudioEvent * event = (AudioEvent *)chan->Data;
	AudioSample * sample;
	unsigned delay;

	sample = eventNextSample(event);
	if (event->eclass->MaxDelay < EVENT_MIN_DELAY) {
		AudioChannelSetSample(chan, sample);
		return vNO_ERROR;
	}
	AudioChannelSetSample(chan, NULL);
	if ((event->eclass->Control & (AUDIO_EVENT_CTRL_PREDELAY | AUDIO_EVENT_CTRL_AMBIENT)) && !sample && !event->numLoadSequence) {
		return vNO_ERROR;
	}
	delay = AudioRandomPick(event->eclass->MinDelay, event->eclass->MaxDelay);
	if (delay < EVENT_MIN_DELAY) {
		AudioChannelSetSample(chan, sample);
		return vNO_ERROR;
	}
	event->delay = delay;
	event->startSample = sample;
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x00405B50
AudioChannel * __fastcall audioPrepareChannel(int pri, int flags)
{
	AudioChannel * chan = NULL;

	if (audioDevice && (chan = AudioDeviceGetChannel(audioDevice, AUDIO_CHANNEL_TYPE_STD))) {
		if (!(chan->Control.Status & (mAUDIO_CTRL_PLAYING | mAUDIO_CTRL_PAUSED | mAUDIO_CTRL_INUSE)) || chan->Control.Priority <= pri) {
			AudioChannelStop(chan);
			chan->GroupAttribs = DAT_0087E748;
			chan->FadeAttribs = DAT_0087E758;
			chan->Control.Priority = pri;
			if (flags & 1) {
				chan->Control.LoopCount = AUDIO_CTRL_LOOP_FOREVER;
			} else {
				chan->Control.LoopCount = 0;
			}
		} else {
			return NULL;
		}
	}
	return chan;
}

// FUNCTION: GAMEMD 0x00405BE0
void AudioController::FUN_00405BE0(void)
{
	AudioIndex = (void **)&AudioIDXData::Instance;
	Event = NULL;
	Stamp = 0;
	EventType = NULL;
}

// FUNCTION: GAMEMD 0x00405C00
void AudioController::FUN_00405C00(void)
{
	Stop();
	AudioIndex = NULL;
}

// FUNCTION: GAMEMD 0x00405D40
void AudioController::Stop(void)
{
	AudioEvent * event = GetEvent();

	if (event) {
		AudioEventKill(event);
		Event = NULL;
	}
	EventType = NULL;
}

// FUNCTION: GAMEMD 0x00405E80
void AudioController::StopLooping(void)
{
	AudioEvent * event = GetEvent();

	if (event && event->eclass->FUN_00406650()) {
		AudioEventKill(event);
		Event = NULL;
	}
	EventType = NULL;
}

// FUNCTION: GAMEMD 0x00405FD0
void AudioController::End(void)
{
	AudioEvent * event = GetEvent();

	if (event) {
		AudioEventEnd(event);
		Event = NULL;
	}
	EventType = NULL;
}

// FUNCTION: GAMEMD 0x00406060
void AudioController::EndLooping(void)
{
	AudioEvent * event = GetEvent();

	if (event && event->eclass->FUN_00406650()) {
		AudioEventEnd(event);
		Event = NULL;
	}
	EventType = NULL;
}

// FUNCTION: GAMEMD 0x004060F0
void __fastcall AudioController::SetEvent(AudioEvent * event, VocClass * type)
{
	if (AudioIndex != (void **)&AudioIDXData::Instance) {
		return;
	}
	EventType = type;
	if ((Event = event)) {
		Stamp = event->stamp;
		if (!type) {
			EventType = event->eclass;
		}
		event->handle = this;
	}
}

// FUNCTION: GAMEMD 0x00406130
AudioEvent * AudioController::GetEvent(void)
{
	AudioEvent * event;

	if (!initialized || AudioIndex != (void **)&AudioIDXData::Instance) {
		return NULL;
	}
	if ((event = Event)) {
		if (EventType != event->eclass || Stamp != event->stamp) {
			Event = event = NULL;
		}
	}
	return event;
}

// FUNCTION: GAMEMD 0x00406170
VocClass * AudioController::GetEventType(void)
{
	if (!initialized || AudioIndex != (void **)&AudioIDXData::Instance) {
		return NULL;
	}
	return EventType;
}

static unsigned eventCalcVolume(AudioEvent * event, int volume)
{
	if (volume > AUDIO_PAN_RIGHT) {
		volume = AUDIO_LEVEL_MAX;
	} else if (volume < 0) {
		volume = 0;
	}
	return AUDIO_LEVEL_APPLY(&event->eclass->Volume, volume);
}

// FUNCTION: GAMEMD 0x00406190
void __fastcall AudioEventSetVolume(AudioEvent * event, int volume)
{
	AudioLevelSet(&event->attribs.VolumeLevel, eventCalcVolume(event, volume));
	AudioLevelUpdate(&event->attribs.VolumeLevel);
}

// FUNCTION: GAMEMD 0x004061D0
void __fastcall AudioController::AdjustAudioEventVolume(AudioEvent * event, unsigned volume)
{
	if (event->state == AUDIO_EVENT_PLAYING) {
		AudioLevelAdjust(&event->attribs.VolumeLevel, eventCalcVolume(event, volume));
	} else {
		AudioEventSetVolume(event, volume);
	}
}

// FUNCTION: GAMEMD 0x00406250
void __fastcall AudioEventSetPan(AudioEvent * event, int pan)
{
	if (pan > AUDIO_PAN_RIGHT) {
		pan = AUDIO_PAN_RIGHT;
	} else if (pan < AUDIO_PAN_LEFT) {
		pan = AUDIO_PAN_LEFT;
	}
	AudioLevelSet(&event->attribs.PanPosition, pan);
}

// FUNCTION: GAMEMD 0x00406270
void __fastcall AudioController::AdjustAudioEventPanning(AudioEvent * event, unsigned panning)
{
	int pan = panning;

	if (event->state == AUDIO_EVENT_PLAYING) {
		if (pan > AUDIO_PAN_RIGHT) {
			pan = AUDIO_PAN_RIGHT;
		} else if (pan < AUDIO_PAN_LEFT) {
			pan = AUDIO_PAN_LEFT;
		}
		AudioLevelAdjust(&event->attribs.PanPosition, pan);
	} else {
		AudioEventSetPan(event, pan);
	}
}

// FUNCTION: GAMEMD 0x004062C0
void __fastcall AudioEventSetAdjustDuration(AudioEvent * event, TimeStamp time)
{
	AudioLevelSetDuration(&event->attribs.VolumeLevel, time, AUDIO_LEVEL_MAX);
	AudioLevelSetDuration(&event->attribs.PitchLevel, time, AUDIO_LEVEL_MAX);
	AudioLevelSetDuration(&event->attribs.PanPosition, time, AUDIO_LEVEL_MAX);
}

// FUNCTION: GAMEMD 0x00406310
VocClass * __fastcall AudioController::FUN_00406310(AudioEvent * event)
{
	return event->eclass;
}

// FUNCTION: GAMEMD 0x00406320
int __fastcall AudioEventNeverEnds(AudioEvent * event)
{
	return event->eclass->FUN_00406650();
}

// FUNCTION: GAMEMD 0x00406340
char const * __fastcall AudioEventName(AudioEvent * event)
{
	if (!initialized) {
		return "no events";
	}
	if (!event->eclass) {
		return "unknown";
	}
	return event->eclass->Name;
}

// FUNCTION: GAMEMD 0x00406360
VocClass * __fastcall AudioEventClassFind(char const * name)
{
	VocClass * eclass;

	if (initialized) {
		for (eclass = (VocClass *)ListFirstItem(&audioEventClasses); eclass; eclass = (VocClass *)ListNextItem(&eclass->Header)) {
			if (!stricmp(name, eclass->Name)) {
				return eclass;
			}
		}
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x004063B0
VocClass * __fastcall FUN_004063B0(char const * name)
{
	VocClass * eclass;

	if (!initialized) {
		return NULL;
	}
	eclass = (VocClass *)ListFirstItem(&audioEventClasses);
	while (eclass) {
		if (!stricmp(name, eclass->Name)) {
			break;
		}
		eclass = (VocClass *)ListNextItem(&eclass->Header);
	}
	if (eclass) {
		return eclass;
	}
	eclass = new VocClass;
	memset(eclass, 0, sizeof(VocClass));
	memset(eclass, 0, sizeof(VocClass));
	ListNodeInit(&eclass->Header);
	eclass->SamplesOK = 1;
	eclass->Control = 0;
	eclass->Type = 0x20;
	eclass->Limit = 3;
	eclass->Priority = 2;
	eclass->Range = 10;
	eclass->MinVolume = 0.2f;
	AudioLevelInit(&eclass->Volume, AUDIO_LEVEL_MAX);
	strncpy(eclass->Name, name, 31);
	eclass->Name[31] = 0;
	ListNodeInsert((ListNode *)&audioEventClasses, &eclass->Header);
	return eclass;
}

// FUNCTION: GAMEMD 0x004064A0
int __fastcall VocClass::FUN_004064A0(char const * sample)
{
	int index;

	if (NumSamples == MAX_AUDIO_EVENT_SAMPLES) {
		return 0;
	}
	if (initialized && AudioIDXData::Instance) {
		while (*sample == '$' || *sample == '#') {
			sample++;
		}
		index = AudioIDXData::Instance->FindSampleIndex(sample);
		if (index != -1) {
			SampleIndex[NumSamples] = index;
			NumSamples++;
			return 1;
		}
		FUN_004068E0("Missing sample %s\n", sample);
		SamplesOK = 0;
	}
	return 1;
}

// FUNCTION: GAMEMD 0x00406520
void __fastcall VocClass::FUN_00406520(int attack)
{
	Attack = attack;
}

// FUNCTION: GAMEMD 0x00406530
void __fastcall VocClass::FUN_00406530(int decay)
{
	Decay = decay;
}

// FUNCTION: GAMEMD 0x00406540
void __fastcall VocClass::FUN_00406540(int priority)
{
	Priority = priority;
}

// FUNCTION: GAMEMD 0x00406550
void __fastcall VocClass::FUN_00406550(int volume)
{
	AudioLevelSet(&Volume, volume);
	AudioLevelUpdate(&Volume);
}

// FUNCTION: GAMEMD 0x00406570
void __fastcall VocClass::FUN_00406570(int control)
{
	Control = control;
	if (control & AUDIO_EVENT_CTRL_ATTACK) {
		if (Attack == 0) {
			Attack = 1;
		}
	} else {
		Attack = 0;
	}
	if (control & AUDIO_EVENT_CTRL_DECAY) {
		if (Decay == 0) {
			Decay = 1;
		}
	} else {
		Decay = 0;
	}
}

// FUNCTION: GAMEMD 0x004065C0
void __fastcall VocClass::FUN_004065C0(int type)
{
	Type = type;
}

// FUNCTION: GAMEMD 0x004065D0
void __fastcall VocClass::FUN_004065D0(int limit)
{
	Limit = limit;
}

// FUNCTION: GAMEMD 0x004065E0
void __fastcall VocClass::FUN_004065E0(int range)
{
	Range = range;
}

// FUNCTION: GAMEMD 0x004065F0
void VocClass::FUN_004065F0(float minVolume)
{
	MinVolume = minVolume;
}

// FUNCTION: GAMEMD 0x00406600
void __fastcall VocClass::FUN_00406600(int minDelay, int maxDelay)
{
	MinDelay = minDelay;
	MaxDelay = maxDelay;
}

// FUNCTION: GAMEMD 0x00406610
void __fastcall VocClass::FUN_00406610(int minShift, int maxShift)
{
	MinFDelta = minShift;
	MaxFDelta = maxShift;
}

// FUNCTION: GAMEMD 0x00406620
void __fastcall VocClass::FUN_00406620(int shift)
{
	if (shift > 100) {
		shift = 100;
	} else if (shift < 0) {
		shift = 0;
	}
	VShift = shift;
}

// FUNCTION: GAMEMD 0x00406640
void __fastcall VocClass::FUN_00406640(int loop)
{
	Loop = loop;
}

// FUNCTION: GAMEMD 0x00406650
int VocClass::FUN_00406650(void)
{
	if ((Control & AUDIO_EVENT_CTRL_LOOP) && !Loop) {
		return 1;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00406670
void __fastcall AudioEventPlayByName(char const * name, int volume)
{
	VocClass * eclass;
	AudioEvent * event;

	if (initialized) {
		for (eclass = (VocClass *)ListFirstItem(&audioEventClasses); eclass; eclass = (VocClass *)ListNextItem(&eclass->Header)) {
			if (!stricmp(name, eclass->Name)) {
				break;
			}
		}
		if (eclass && (event = AudioEventCreate(eclass))) {
			AudioEventSetVolume(event, (int)((__int64)volume * AUDIO_LEVEL_MAX / 256));
		}
	}
}

// FUNCTION: GAMEMD 0x00406730
void AudioEventTurnOff(void)
{
	eventsOn = 0;
}

// FUNCTION: GAMEMD 0x00406740
void AudioEventTurnOn(void)
{
	eventsOn = 1;
}

// FUNCTION: GAMEMD 0x00406750
int AudioEventOn(void)
{
	return eventsOn;
}

// FUNCTION: GAMEMD 0x00406760
void __fastcall AudioEventsDump(void (__fastcall * print)(char const * line))
{
	char buffer[200];

	if (audioCache) {
		sprintf(buffer, "AudioEvents : %05d - Max: %05d", AudioEventsCount, AudioEventsPeak);
		print(buffer);
		sprintf(buffer, "StaticSounds: %05d - Max: %05d", DAT_00B1D3A8, DAT_00B1D3AC);
		print(buffer);
	}
}

// FUNCTION: GAMEMD 0x004067D0
int __fastcall FUN_004067D0(char const * name, int * priority)
{
	EventKeyword * entry;

	for (entry = PriorityKeywords; entry->Name; entry++) {
		if (!stricmp(name, entry->Name)) {
			break;
		}
	}
	if (priority) {
		*priority = entry->Value;
	}
	return entry->Name != NULL;
}

// FUNCTION: GAMEMD 0x00406820
int __fastcall FUN_00406820(int * control, char const * name)
{
	EventKeyword * entry;

	for (entry = ControlKeywords; entry->Name; entry++) {
		if (!stricmp(name, entry->Name)) {
			break;
		}
	}
	if (control) {
		*control |= entry->Value;
	}
	return entry->Name != NULL;
}

// FUNCTION: GAMEMD 0x00406870
int __fastcall FUN_00406870(int * type, char const * name)
{
	EventKeyword * entry;

	for (entry = TypeKeywords; entry->Name; entry++) {
		if (!stricmp(name, entry->Name)) {
			break;
		}
	}
	if (type) {
		if (entry->Value & 0x60) {
			*type &= ~0x60;
		} else if (entry->Value & 0xC00) {
			*type &= ~0xC00;
		}
		*type |= entry->Value;
	}
	return entry->Name != NULL;
}

// FUNCTION: GAMEMD 0x004068E0
void __cdecl FUN_004068E0(char const *, ...)
{
}
