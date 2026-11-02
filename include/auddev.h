// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "audlist.h"
#include "audlock.h"
#include "audattr.h"
#include "audsvc.h"
#include "audchan.h"

#define vNO_ERROR 0
#define ERROR_CODE_IN_USE 0x80120000
#define ERROR_CODE_FAIL 0x80040000

#define AUDIO_DEVICE_DEFAULT (-1)
#define mAUDIO_DEV_DONT_SERVICE 2
#define mAUDIO_SYSTEM_MASTER_LOADED 1
#define mAUDIO_SYS_PROP_EXCLUSIVE 2

struct AudioSystem;
struct AudioDevice;

struct AudioSystemMaster
{
	const char * Name;
	unsigned Reserved1;
	unsigned Properties;
	unsigned flags;
	unsigned Reserved2;
	int(__fastcall * load)(AudioSystem * system);
	void(__fastcall * unload)(AudioSystem * system);
	int(__fastcall * open)(AudioDevice * dev);
	void(__fastcall * close)(AudioDevice * dev);
	AudioChannelDriver * driver;
};

struct AudioSystem
{
	ListNode nd;
	AudioSystemMaster * master;
	Lock lock;
	unsigned Reserved;
	int numUnits;
	AudioDevice * unit[17];
};

typedef int(__fastcall AudioDeviceCB)(AudioDevice * dev);

struct AudioDevice
{
	ListNode nd;
	unsigned flags;
	Lock lock;
	unsigned Reserved1;
	AudioSystem * system;
	int systemUnit;
	unsigned Reserved2;
	AudioFormat Format;
	AudioFormat DefaultFormat;
	unsigned Reserved3;
	AudioAttribs Attribs;
	unsigned Reserved4[2];
	AudioAttribs * GroupAttribs;
	int Unit;
	int channels;
	int maxChannels;
	ListHead channelList;
	Lock channelAccess;
	unsigned Reserved8;
	AudioDeviceCB * deviceHandler;
	AudioServiceInfo attribsUpdate;
	unsigned Reserved5[2];
	AudioServiceInfo mixerUpdate;
	unsigned Reserved9[2];
	AudioChannelDriver * driver;
	unsigned Reserved6;
	int Frames;
	int Oversample;
	int Lag;
	unsigned Reserved7;
};

struct AudioSearch
{
	void * item;
	unsigned stamp;
};

void __fastcall FUN_004085A0(AudioFormat * format);

void __fastcall audioAddChannel(AudioDevice * dev, AudioChannel * chan);
void __fastcall audioRemoveChannel(AudioChannel * chan);
void AudioServiceAllDevices(void);
int AudioSetUp(void);
void AudioCloseDown(void);
AudioSystem * __fastcall AudioLoadSystem(AudioSystemMaster * master);
void __fastcall AudioUnloadSystem(AudioSystem * system);
void AudioUnloadAllSystems(void);
AudioSystem * __fastcall AudioFirstSystem(AudioSearch * sh);
AudioSystem * __fastcall AudioNextSystem(AudioSearch * sh);
int AudioNumberOfDevices(void);
AudioDevice * __fastcall AudioOpenDevice(int unit, AudioFormat * format);
void __fastcall AudioDeviceSetDefaultFormat(AudioDevice * dev, AudioFormat * format);
AudioDevice * __fastcall AudioFirstDevice(AudioSearch * sh);
AudioDevice * __fastcall AudioNextDevice(AudioSearch * sh);
void AudioDestroyAllDevices(void);
void __fastcall AudioDeviceClose(AudioDevice * dev);
void __fastcall AudioDeviceDestroy(AudioDevice * dev);
int __fastcall AudioDeviceSetFormat(AudioDevice * dev, AudioFormat * format);
void __fastcall AudioDeviceSetVolume(AudioDevice * dev, int newVolume);
void __fastcall AudioDeviceAdjustVolume(AudioDevice * dev, int newVolume);
int __fastcall AudioDeviceMaxChannels(AudioDevice * dev);
int __fastcall AudioDeviceChannels(AudioDevice * dev);
AudioChannel * __fastcall AudioDeviceCreateChannel(AudioDevice * dev);
int __fastcall AudioDeviceCreateChannels(AudioDevice * dev, int num);
AudioChannel * __fastcall AudioDeviceFirstChannel(AudioDevice * dev, AudioSearch * sh);
AudioChannel * __fastcall AudioDeviceNextChannel(AudioSearch * sh);
AudioChannel * __fastcall AudioDeviceGetChannel(AudioDevice * dev, int type);
AudioChannel * __fastcall AudioDeviceReserveChannel(AudioDevice * dev, int newType);
int __fastcall AudioDeviceDestroyAllChannels(AudioDevice * dev, int type);
void __fastcall AudioDeviceStopAllChannels(AudioDevice * dev, int type);
void __fastcall AudioDevicePauseAllChannels(AudioDevice * dev, int type);
void __fastcall AudioDeviceResumeAllChannels(AudioDevice * dev, int type);
int __fastcall AudioDeviceService(AudioDevice * dev);
void __fastcall AudioDeviceDump(AudioDevice * dev, void(__fastcall * print)(const char * line), int verbose);
