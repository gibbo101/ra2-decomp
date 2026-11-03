// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <stdio.h>
#include <string.h>
#include "auddev.h"

struct AudioData
{
	ListHead systemList;
	ListHead devList;
	Lock devListAccess;
	AudioAttribs stdChannelAttribs;
};

static AudioData audioDataStruct;
static AudioData * audioData = &audioDataStruct;
static int audioExclusiveSystemLoaded = 0;
int audioInitialized = 0;
AudioAttribs * audioStdChannelAttribs = 0;
static char * anim = "|/-\\";

static inline void AudioMemFree(void * ptr)
{
	operator delete(ptr);
}

static void __fastcall audioDestroySystem(AudioSystem * system)
{
	system->master->unload(system);
	system->master->flags &= ~mAUDIO_SYSTEM_MASTER_LOADED;
	AudioMemFree(system);
}

#define ListAddToTail(h, n) ListNodeInsert((ListNode *)(h), (n))

static void __fastcall audioDestroyDevice(AudioDevice * dev);
static int __fastcall audioDeviceHandler(AudioDevice * dev);

static void __fastcall audioAddDevice(AudioDevice * dev)
{
	LockAcquire(&audioData->devListAccess);
	ListAddToTail(&audioData->devList, &dev->nd);
	LockRelease(&audioData->devListAccess);
}

static void __fastcall audioRemoveDevice(AudioDevice * dev)
{
	LockAcquire(&audioData->devListAccess);
	ListNodeRemove(&dev->nd);
	LockRelease(&audioData->devListAccess);
}

static AudioDevice * audioCreateDevice(AudioSystem * system, int unit, AudioFormat * format)
{
	AudioDevice * dev;
	AudioSystemMaster * master = system->master;

	dev = (AudioDevice *)operator new(sizeof(AudioDevice));
	memset(dev, 0, sizeof(AudioDevice));

	ListNodeInit(&dev->nd);
	dev->systemUnit = unit;
	dev->system = system;
	LockInit(&dev->lock);
	LockAcquire(&system->lock);
	system->unit[unit] = dev;
	dev->driver = master->driver;
	ListInit(&dev->channelList);
	AudioAttribsInit(&dev->Attribs);
	LockInit(&dev->channelAccess);
	dev->deviceHandler = audioDeviceHandler;
	AudioServiceInfoInit(&dev->attribsUpdate);
	AudioServiceSetInterval(&dev->attribsUpdate, SECONDS(1) / 30);
	AudioServiceSetMustServiceInterval(&dev->attribsUpdate, SECONDS(1) / 30 * 5);
	AudioServiceSetResetInterval(&dev->attribsUpdate, SECONDS(1) / 30 * 5);
	AudioServiceInfoInit(&dev->mixerUpdate);
	AudioServiceSetInterval(&dev->mixerUpdate, SECONDS(1) / 30);
	AudioFormatInit(&dev->DefaultFormat);
	AudioFormatInit(&dev->Format);

	if (format) {
		AudioDeviceSetDefaultFormat(dev, format);
	}

	if (master->open(dev) != vNO_ERROR) {
		goto error;
	}

	return dev;

error:
	if (dev) {
		audioDestroyDevice(dev);
	}

	return 0;
}

static void audioMapDeviceUnit(int unit, AudioSystem ** pSystem, int * pUnit)
{
	AudioSystem * system;
	AudioSearch sh;

	system = AudioFirstSystem(&sh);
	while (system) {
		if (unit < system->numUnits) {
			break;
		}
		unit -= system->numUnits;
		system = AudioNextSystem(&sh);
	}

	if ((*pSystem = system)) {
		*pUnit = unit;
	} else {
		*pUnit = -1;
	}
}

// FUNCTION: GAMEMD 0x00402860
void __fastcall audioAddChannel(AudioDevice * dev, AudioChannel * chan)
{
	chan->Device = dev;
	dev->channels++;
	LockAcquire(&dev->channelAccess);
	ListAddToTail(&dev->channelList, &chan->nd);
	LockRelease(&dev->channelAccess)
}

// FUNCTION: GAMEMD 0x004028B0
void __fastcall audioRemoveChannel(AudioChannel * chan)
{
	AudioDevice * dev = chan->Device;

	ListNodeRemove(&chan->nd);
	dev->channels--;
	LockAcquire(&dev->channelAccess);
	ListNodeRemove(&chan->nd);
	LockRelease(&dev->channelAccess)
}

// FUNCTION: GAMEMD 0x00402900
void AudioServiceAllDevices(void)
{
	AudioDevice * dev;
	AudioDevice * head;
	AudioData * data = audioData;

	if (!audioInitialized || !NotLocked(&data->devListAccess)) {
		return;
	}

	dev = head = (AudioDevice *)&data->devList;
	while ((dev = (AudioDevice *)dev->nd.next) != head) {
		if (!(dev->flags & mAUDIO_DEV_DONT_SERVICE)) {
			dev->deviceHandler(dev);
		}
	}
}

// FUNCTION: GAMEMD 0x00402940
int AudioSetUp(void)
{
	ListInit(&audioData->systemList);
	ListInit(&audioData->devList);
	LockInit(&audioData->devListAccess);
	audioStdChannelAttribs = &audioData->stdChannelAttribs;
	AudioAttribsInit(audioStdChannelAttribs);
	audioInitialized = 1;
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x00402990
void AudioCloseDown(void)
{
	if (!audioInitialized) {
		return;
	}
	AudioDestroyAllDevices();
	AudioUnloadAllSystems();
	audioInitialized = 0;
}

static AudioSystem * audioCreateSystem(AudioSystemMaster * master)
{
	AudioSystem * system = (AudioSystem *)operator new(sizeof(AudioSystem));
	memset(system, 0, sizeof(AudioSystem));

	ListNodeInit(&system->nd);
	LockInit(&system->lock);
	system->master = master;

	if (master->load(system) != vNO_ERROR) {
		goto error;
	}

	return system;

error:
	if (system) {
		audioDestroySystem(system);
	}

	return 0;
}

// FUNCTION: GAMEMD 0x00402AF0
AudioSystem * __fastcall AudioLoadSystem(AudioSystemMaster * master)
{
	AudioSystem * system;

	if (!(system = audioCreateSystem(master))) {
		goto error;
	}

	master->flags |= mAUDIO_SYSTEM_MASTER_LOADED;
	ListAddToTail(&audioData->systemList, &system->nd);
	if (master->Properties & mAUDIO_SYS_PROP_EXCLUSIVE) {
		audioExclusiveSystemLoaded = 1;
	}
	return system;

error:
	if (system) {
		audioDestroySystem(system);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00402B80
void __fastcall AudioUnloadSystem(AudioSystem * system)
{
	ListNodeRemove(&system->nd);
	audioDestroySystem(system);
	audioExclusiveSystemLoaded = 0;
}

// FUNCTION: GAMEMD 0x00402BB0
void AudioUnloadAllSystems(void)
{
	AudioSystem * sys;
	AudioSystem * next;
	AudioSearch sh;

	sys = AudioFirstSystem(&sh);
	while (sys) {
		next = AudioNextSystem(&sh);
		AudioUnloadSystem(sys);
		sys = next;
	}
}

// FUNCTION: GAMEMD 0x00402C00
AudioSystem * __fastcall AudioFirstSystem(AudioSearch * sh)
{
	sh->item = ListFirstItem(&audioData->systemList);
	return (AudioSystem *)sh->item;
}

// FUNCTION: GAMEMD 0x00402C20
AudioSystem * __fastcall AudioNextSystem(AudioSearch * sh)
{
	if (sh->item) {
		sh->item = ListNextItem((ListNode *)sh->item);
	}
	return (AudioSystem *)sh->item;
}

// FUNCTION: GAMEMD 0x00402C40
int AudioNumberOfDevices(void)
{
	int devices = 0;
	AudioSystem * system;
	AudioSearch sh;

	system = AudioFirstSystem(&sh);
	while (system) {
		devices += system->numUnits;
		system = AudioNextSystem(&sh);
	}
	return devices;
}

// FUNCTION: GAMEMD 0x00402C70
AudioDevice * __fastcall AudioOpenDevice(int unit, AudioFormat * format)
{
	AudioDevice * dev = 0;
	AudioSystem * system;
	int systemUnit;

	if (unit == AUDIO_DEVICE_DEFAULT) {
		unit = 0;
	}

	audioMapDeviceUnit(unit, &system, &systemUnit);
	if (!system) {
		goto error;
	}

	if (!(dev = system->unit[systemUnit])) {
		if (!(dev = audioCreateDevice(system, systemUnit, format))) {
			goto error;
		}
		dev->Unit = unit;
		audioAddDevice(dev);
	}
	LockAcquire(&dev->lock);
	return dev;

error:
	if (dev) {
		AudioDeviceClose(dev);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00402EF0
void __fastcall audioDestroyDevice(AudioDevice * dev)
{
	AudioDeviceDestroyAllChannels(dev, AUDIO_CHANNEL_TYPE_ALL);
	dev->system->master->close(dev);
	dev->system->unit[dev->systemUnit] = 0;
	LockRelease(&dev->system->lock);
	AudioMemFree(dev);
}

// FUNCTION: GAMEMD 0x00402F60
int __fastcall audioDeviceHandler(AudioDevice * dev)
{
	AudioChannel * chan;
	AudioChannel * next;
	AudioChannel * head;
	int change = 0;
	TimeStamp now;

	if (!NotLocked(&dev->channelAccess)) {
		return ERROR_CODE_IN_USE;
	}

	now = AudioGetTime();
	if (!AudioServiceNeeded(&dev->attribsUpdate, now)) {
		return vNO_ERROR;
	}

	AudioServicePerform(&dev->attribsUpdate, now);

	AudioAttribsUpdate(&dev->Attribs);
	if (dev->GroupAttribs) {
		AudioAttribsUpdate(dev->GroupAttribs);
	}
	if (!dev->GroupAttribs || !(change = AudioAttribsChanged(dev->GroupAttribs))) {
		change = AudioAttribsChanged(&dev->Attribs);
	}

	chan = head = (AudioChannel *)&dev->channelList;
	next = (AudioChannel *)chan->nd.next;
	while ((chan = next) != head) {
		next = (AudioChannel *)chan->nd.next;
		if (chan->Control.Status & mAUDIO_CTRL_PLAYING) {
			if (chan->sample) {
				AudioAttribsUpdate(&chan->ChannelAttribs);
				if (change || AudioAttribsChanged(&chan->ChannelAttribs) || (chan->GroupAttribs && AudioAttribsChanged(chan->GroupAttribs)) || (chan->CompAttribs && AudioAttribsChanged(chan->CompAttribs)) || (chan->FadeAttribs && AudioAttribsChanged(chan->FadeAttribs)) || (chan->SfxAttribs && AudioAttribsChanged(chan->SfxAttribs))) {
					audioChannelRecalcAttribs(chan);
					chan->driver->update(chan);
				}
			}
		}
	}

	if (dev->GroupAttribs) {
		AudioAttribsUsed(dev->GroupAttribs);
	}
	AudioAttribsUsed(&dev->Attribs);
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x004031B0
void __fastcall AudioDeviceSetDefaultFormat(AudioDevice * dev, AudioFormat * format)
{
	memcpy(&dev->DefaultFormat, format, sizeof(AudioFormat));
}

// FUNCTION: GAMEMD 0x004031D0
AudioDevice * __fastcall AudioFirstDevice(AudioSearch * sh)
{
	sh->item = ListFirstItem(&audioData->devList);
	return (AudioDevice *)sh->item;
}

// FUNCTION: GAMEMD 0x004031F0
AudioDevice * __fastcall AudioNextDevice(AudioSearch * sh)
{
	if (sh->item) {
		sh->item = ListNextItem((ListNode *)sh->item);
	}
	return (AudioDevice *)sh->item;
}

// FUNCTION: GAMEMD 0x00403210
void AudioDestroyAllDevices(void)
{
	AudioDevice * dev;
	AudioDevice * next;
	AudioSearch sh;

	LockAcquire(&audioData->devListAccess);
	dev = AudioFirstDevice(&sh);
	while (dev) {
		next = AudioNextDevice(&sh);
		AudioDeviceDestroy(dev);
		dev = next;
	}
	LockRelease(&audioData->devListAccess);
}

// FUNCTION: GAMEMD 0x00403310
void __fastcall AudioDeviceClose(AudioDevice * dev)
{
	LockRelease(&dev->lock);
	if (!Locked(&dev->lock)) {
		AudioDeviceDestroy(dev);
	}
}

// FUNCTION: GAMEMD 0x004033C0
void __fastcall AudioDeviceDestroy(AudioDevice * dev)
{
	audioRemoveDevice(dev);
	audioDestroyDevice(dev);
}

// FUNCTION: GAMEMD 0x00403460
int __fastcall AudioDeviceSetFormat(AudioDevice * dev, AudioFormat * format)
{
	return ERROR_CODE_FAIL;
}

// FUNCTION: GAMEMD 0x00403470
void __fastcall AudioDeviceSetVolume(AudioDevice * dev, int newVolume)
{
	AudioLevelSet(&dev->Attribs.VolumeLevel, newVolume);
}

// FUNCTION: GAMEMD 0x00403480
void __fastcall AudioDeviceAdjustVolume(AudioDevice * dev, int newVolume)
{
	AudioLevelAdjust(&dev->Attribs.VolumeLevel, newVolume);
}

// FUNCTION: GAMEMD 0x00403490
int __fastcall AudioDeviceMaxChannels(AudioDevice * dev)
{
	return dev->maxChannels;
}

// FUNCTION: GAMEMD 0x004034A0
int __fastcall AudioDeviceChannels(AudioDevice * dev)
{
	return dev->channels;
}

// FUNCTION: GAMEMD 0x004034B0
AudioChannel * __fastcall AudioDeviceCreateChannel(AudioDevice * dev)
{
	AudioChannel * channel = 0;

	if (dev->channels >= dev->maxChannels) {
		goto error;
	}

	if (!(channel = audioChannelCreate(dev))) {
		goto error;
	}

	audioAddChannel(dev, channel);
	return channel;

error:
	if (channel) {
		AudioChannelDestroy(channel);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00403530
int __fastcall AudioDeviceCreateChannels(AudioDevice * dev, int num)
{
	int count = 0;

	do {
		if (!AudioDeviceCreateChannel(dev)) {
			break;
		}
		count++;
	} while (--num);
	return count;
}

// FUNCTION: GAMEMD 0x004035B0
AudioChannel * __fastcall AudioDeviceFirstChannel(AudioDevice * dev, AudioSearch * sh)
{
	sh->item = ListFirstItem(&dev->channelList);
	return (AudioChannel *)sh->item;
}

// FUNCTION: GAMEMD 0x004035D0
AudioChannel * __fastcall AudioDeviceNextChannel(AudioSearch * sh)
{
	if (sh->item) {
		sh->item = ListNextItem((ListNode *)sh->item);
	}
	return (AudioChannel *)sh->item;
}

// FUNCTION: GAMEMD 0x004035F0
AudioChannel * __fastcall AudioDeviceGetChannel(AudioDevice * dev, int type)
{
	AudioChannel * channel;
	AudioSearch sh;
	int lowestPriorityPlaying;
	int lowestPriorityPaused;
	AudioChannel * lowestPlaying;
	AudioChannel * lowestPaused;
	unsigned lowestPlayingVolume;

	if (type == AUDIO_CHANNEL_TYPE_ANY) {
		type = AUDIO_CHANNEL_TYPE_STD;
	}

	lowestPriorityPlaying = AUD_HIGHEST_PRIORITY;
	lowestPriorityPaused = AUD_HIGHEST_PRIORITY;
	lowestPlaying = 0;
	lowestPaused = 0;
	lowestPlayingVolume = 0xFFFFFFFF;

	channel = AudioDeviceFirstChannel(dev, &sh);
	while (channel) {
		if (channel->Type == type) {
			if (!(channel->Control.Status & mAUDIO_CTRL_BUSY)) {
				break;
			}

			if (channel->Control.Status & mAUDIO_CTRL_PLAYING) {
				if (channel->Control.Priority < lowestPriorityPlaying) {
					lowestPriorityPlaying = channel->Control.Priority;
					lowestPlaying = channel;
					lowestPlayingVolume = channel->attribs.VolumeLevel.level;
				} else if (channel->Control.Priority == lowestPriorityPlaying) {
					if (channel->attribs.VolumeLevel.level < lowestPlayingVolume && (lowestPlayingVolume - channel->attribs.VolumeLevel.level >= AUDIO_LEVEL_MAX / 10)) {
						lowestPriorityPlaying = channel->Control.Priority;
						lowestPlaying = channel;
						lowestPlayingVolume = channel->attribs.VolumeLevel.level;
					}
				}
			} else {
				if (channel->Control.Priority < lowestPriorityPaused) {
					lowestPriorityPaused = channel->Control.Priority;
					lowestPaused = channel;
				}
			}
		}
		channel = AudioDeviceNextChannel(&sh);
	}

	if (!channel) {
		if (!(channel = lowestPaused) || (lowestPriorityPaused > lowestPriorityPlaying)) {
			channel = lowestPlaying;
		}
	}
	return channel;
}

// FUNCTION: GAMEMD 0x004036C0
AudioChannel * __fastcall AudioDeviceReserveChannel(AudioDevice * dev, int newType)
{
	AudioChannel * chan;

	chan = AudioDeviceGetChannel(dev, AUDIO_CHANNEL_TYPE_STD);
	if (chan) {
		AudioChannelReserve(chan, newType);
	}
	return chan;
}

static inline void audioForEachChannel(AudioDevice * dev, int type, void (__fastcall * action)(AudioChannel *))
{
	AudioSearch sh;
	AudioChannel * channel = AudioDeviceFirstChannel(dev, &sh);
	while (channel) {
		// The action can destroy the current channel.
		AudioChannel * next = AudioDeviceNextChannel(&sh);
		if (type == AUDIO_CHANNEL_TYPE_ALL || channel->Type == type) {
			action(channel);
		}
		channel = next;
	}
}

// FUNCTION: GAMEMD 0x004036F0
int __fastcall AudioDeviceDestroyAllChannels(AudioDevice * dev, int type)
{
	audioForEachChannel(dev, type, AudioChannelDestroy);
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x00403730
void __fastcall AudioDeviceStopAllChannels(AudioDevice * dev, int type)
{
	audioForEachChannel(dev, type, AudioChannelStop);
}

// FUNCTION: GAMEMD 0x00403770
void __fastcall AudioDevicePauseAllChannels(AudioDevice * dev, int type)
{
	AudioChannel * channel;
	AudioSearch sh;

	channel = AudioDeviceFirstChannel(dev, &sh);
	while (channel) {
		if (channel->Control.Status & mAUDIO_CTRL_PLAYING) {
			if (type == AUDIO_CHANNEL_TYPE_ALL || channel->Type == type) {
				AudioChannelPause(channel);
			}
		}
		channel = AudioDeviceNextChannel(&sh);
	}
}

// FUNCTION: GAMEMD 0x004037C0
void __fastcall AudioDeviceResumeAllChannels(AudioDevice * dev, int type)
{
	AudioChannel * channel;
	AudioSearch sh;

	channel = AudioDeviceFirstChannel(dev, &sh);
	while (channel) {
		if (channel->Control.Status & mAUDIO_CTRL_PAUSED) {
			if (type == AUDIO_CHANNEL_TYPE_ALL || channel->Type == type) {
				AudioChannelResume(channel);
			}
		}
		channel = AudioDeviceNextChannel(&sh);
	}
}

// FUNCTION: GAMEMD 0x00403810
int __fastcall AudioDeviceService(AudioDevice * dev)
{
	return dev->deviceHandler(dev);
}

// FUNCTION: GAMEMD 0x00403820
void __fastcall AudioServiceInfoInit(AudioServiceInfo * info)
{
	info->Interval = 0;
	info->MustServiceInterval = 0;
	info->Delay = 0;
	info->MaxDelay = 0;
	info->LastTime = 0;
	info->Count = 0;
	info->LastCount = 0;
	info->PeakWindow = 5000;
	info->PeakDelay = 0;
	info->PeakTime = 0;
	info->Anim = 0;
	info->Missed = 0;
}

// FUNCTION: GAMEMD 0x00403870
void __fastcall AudioServiceSetInterval(AudioServiceInfo * info, TimeStamp interval)
{
	info->Interval = interval;
	info->MustServiceInterval = interval;
	info->ResetInterval = info->Interval * 10;
}

// FUNCTION: GAMEMD 0x004038A0
void __fastcall AudioServiceSetMustServiceInterval(AudioServiceInfo * info, TimeStamp interval)
{
	info->MustServiceInterval = interval;
}

// FUNCTION: GAMEMD 0x004038C0
void __fastcall AudioServiceSetResetInterval(AudioServiceInfo * info, TimeStamp interval)
{
	info->ResetInterval = interval;
}

// FUNCTION: GAMEMD 0x004038E0
int __fastcall AudioServiceNeeded(AudioServiceInfo * info, TimeStamp now)
{
	if ((now - info->LastTime) < info->Interval) {
		return 0;
	}
	return 1;
}

// FUNCTION: GAMEMD 0x00403920
void __fastcall AudioServicePerform(AudioServiceInfo * info, TimeStamp now)
{
	TimeStamp delay = now - info->LastTime;

	info->Delay = delay;
	info->Count++;
	if (info->MaxDelay > info->ResetInterval) {
		info->MaxDelay = 0;
	}
	if (delay > info->MaxDelay) {
		info->MaxDelay = delay;
	}
	if (delay > info->MustServiceInterval) {
		info->Missed++;
	}
	if (delay > info->PeakDelay) {
		info->PeakDelay = delay;
	}
	info->LastTime = now;
}

// FUNCTION: GAMEMD 0x004039C0
void __fastcall AudioServiceInfoDump(AudioServiceInfo * info, char * buf)
{
	TimeStamp now = AudioGetTime();

	if (info->Count != info->LastCount) {
		info->Anim++;
		if ((unsigned)info->Anim >= strlen(anim)) {
			info->Anim = 0;
		}
		info->LastCount = info->Count;
	}
	if (info->PeakWindow + info->PeakTime < now) {
		info->PeakDelay = info->Interval;
		info->PeakTime = now;
	}
	sprintf(buf, "%05dms (%04d,%04d,%04d,~%2d) %c \n", (int)info->Interval, (int)info->Delay, (int)info->PeakDelay, (int)info->MaxDelay, info->Missed, anim[info->Anim]);
}

// FUNCTION: GAMEMD 0x00403A80
void __fastcall AudioDeviceDump(AudioDevice * dev, void(__fastcall * print)(const char * line), int verbose)
{
	char line[200];
	char svc[100];
	char name[200];
	char fmt[200];
	AudioChannel * chan;
	AudioSearch sh;
	int index;

	print("Audio Device Dump ---------------------\n");
	sprintf(line, "%s (%d)\n", dev->system->master->Name, dev->Unit);
	print(line);

	AudioServiceInfoDump(&dev->mixerUpdate, svc);
	sprintf(line, "mixer:%s", svc);
	print(line);

	AudioServiceInfoDump(&dev->attribsUpdate, svc);
	sprintf(line, "level:%s", svc);
	print(line);

	sprintf(line, "Frames: %d Oversamp: %d Lag: %d", dev->Frames, dev->Oversample, dev->Lag);
	print(line);

	if (verbose) {
		sprintf(line, "Chan:TYPE: STATE : FORMAT  :PRI :VOL: NAME\n");
	} else {
		sprintf(line, "Chan:TYPE: STATE : FORMAT  :PRI :VOL:\n");
	}
	print(line);

	chan = AudioDeviceFirstChannel(dev, &sh);
	index = 0;
	while (chan) {
		const char * state;
		const char * type;

		if (chan->Control.Status & mAUDIO_CTRL_PLAYING) {
			state = "PLAYING";
		} else if (chan->Control.Status & mAUDIO_CTRL_PAUSED) {
			state = "PAUSED ";
		} else {
			state = "FREE   ";
		}

		if (chan->Type >= 0x1000) {
			type = "USER";
		} else if (chan->Type == AUDIO_CHANNEL_TYPE_STD) {
			type = "STD ";
		} else {
			type = "??? ";
		}

		name[0] = 0;
		sprintf(fmt, "%02d-%2d-%c%c%c", chan->current_format.Rate / 1000, chan->current_format.Compression == 0 ? chan->current_format.SampleWidth << 3 : 4, chan->current_format.Channels != 1 ? 'S' : 'M', chan->format_changed ? '*' : ' ', chan->drv_format_changed ? '+' : ' ');
		sprintf(line, "#%2d :%s:%s:%s:%4d:%3d:%.10s\n", index, type, state, fmt, chan->Control.Priority, ((unsigned)chan->attribs.VolumeLevel.level >> AUDIO_LEVEL_SCALE) * 100 >> 14, name);
		print(line);
		index++;
		chan = AudioDeviceNextChannel(&sh);
	}

	print("--------------------------------------\n");
}
