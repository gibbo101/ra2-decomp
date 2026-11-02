// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include <string.h>
#include <new.h>
#include "audchan.h"
#include "auddev.h"

static int __fastcall audioChannelSampleDone(AudioChannel * chan);
static int __fastcall audioChannelNextSample(AudioChannel * chan);
static int __fastcall audioChannelNextFrame(AudioChannel * chan);

// FUNCTION: GAMEMD 0x00402040
AudioChannel * __fastcall audioChannelCreate(AudioDevice * dev)
{
	AudioChannel * chan = NULL;

	chan = (AudioChannel *)operator new(sizeof(AudioChannel));
	memset(chan, 0, sizeof(AudioChannel));

	ListNodeInit(&chan->nd);
	AudioControlInit(&chan->Control);
	AudioAttribsInit(&chan->attribs);
	audioChannelMakeStandard(chan);

	chan->Device = dev;
	chan->driver = dev->driver;

	chan->current_format = dev->DefaultFormat;
	chan->format_changed = 0;
	chan->drv_format_changed = 0;

	if (dev->driver->openChannel(chan) != vNO_ERROR) {
		goto error;
	}

	return chan;

error:
	if (chan) {
		audioChannelDestroy(chan);
	}

	return NULL;
}

// FUNCTION: GAMEMD 0x00402170
void __fastcall audioChannelDestroy(AudioChannel * chan)
{
	chan->driver->closeChannel(chan);
	operator delete(chan);
}

// FUNCTION: GAMEMD 0x00402190
void __fastcall audioChannelMakeStandard(AudioChannel * chan)
{
	chan->drvCBNextFrame = audioChannelNextFrame;
	chan->drvCBNextSample = audioChannelNextSample;
	chan->drvCBSampleDone = audioChannelSampleDone;
	chan->CB_NextFrame = NULL;
	chan->CB_NextSample = NULL;
	chan->CB_SampleDone = NULL;
	chan->CB_Stop = NULL;
	chan->Data = NULL;
	chan->Type = AUDIO_CHANNEL_TYPE_STD;

	AudioControlInit(&chan->Control);
	chan->Control.Priority = AUD_NORMAL_PRIORITY;

	chan->GroupAttribs = audioStdChannelAttribs;
	chan->SfxAttribs = NULL;
	chan->CompAttribs = NULL;
	chan->FadeAttribs = NULL;
	AudioAttribsInit(&chan->ChannelAttribs);

	chan->sample = NULL;
}

// FUNCTION: GAMEMD 0x00402220
void __fastcall audioChannelRecalcAttribs(AudioChannel * chan)
{
	AudioDevice * dev;

	dev = (AudioDevice *)chan->Device;

	AudioAttribsInit(&chan->attribs);
	AudioAttribsApply(&chan->attribs, &chan->ChannelAttribs);
	AudioAttribsUsed(&chan->ChannelAttribs);

	if (chan->SfxAttribs) {
		AudioAttribsApply(&chan->attribs, chan->SfxAttribs);
		AudioAttribsUsed(chan->SfxAttribs);
	}

	if (chan->GroupAttribs) {
		AudioAttribsApply(&chan->attribs, chan->GroupAttribs);
	}

	if (chan->CompAttribs) {
		AudioAttribsApply(&chan->attribs, chan->CompAttribs);
	}

	if (chan->FadeAttribs) {
		AudioAttribsApply(&chan->attribs, chan->FadeAttribs);
	}

	AudioAttribsApply(&chan->attribs, &dev->Attribs);

	if (dev->GroupAttribs) {
		AudioAttribsApply(&chan->attribs, dev->GroupAttribs);
	}
}

// FUNCTION: GAMEMD 0x004022C0
static int __fastcall audioChannelNextFrame(AudioChannel * chan)
{
	int err;

	if (chan->CB_NextFrame) {
		if ((err = chan->CB_NextFrame(chan)) != vNO_ERROR) {
			chan->bytesInFrame = 0;
			chan->bytesRemaining = 0;
			chan->frameData = NULL;
			return err;
		}

		return vNO_ERROR;
	}

	if (chan->frame) {
		if ((chan->frame = (AudioFrame *)ListNodeNext(&chan->frame->nd))) {
			chan->frameData = (char *)chan->frame->Data;
			chan->bytesInFrame = chan->frame->Bytes;
			chan->bytesRemaining = chan->bytesInFrame;
			return vNO_ERROR;
		}
	}

	chan->bytesInFrame = 0;

	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x00402330
static int __fastcall audioChannelNextSample(AudioChannel * chan)
{
	if (chan->Control.LoopCount) {
		if (chan->Control.LoopCount != AUDIO_CTRL_LOOP_FOREVER) {
			chan->Control.LoopCount--;
		}
		AudioChannelSetSample(chan, chan->sample);
	} else {
		AudioChannelSetSample(chan, NULL);
	}

	if (chan->CB_NextSample) {
		chan->CB_NextSample(chan);
	}

	if (chan->sample) {
		chan->driver->queueIt(chan);
	}

	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x004023F0
static int __fastcall audioChannelSampleDone(AudioChannel * chan)
{
	chan->sample = NULL;
	chan->Control.Status &= ~(mAUDIO_CTRL_PLAYING | mAUDIO_CTRL_PAUSED);
	chan->Control.LoopCount = 0;
	AudioAttribsInit(&chan->ChannelAttribs);

	if (chan->CB_SampleDone) {
		return chan->CB_SampleDone(chan);
	}

	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x00402430
int __fastcall AudioChannelTaken(AudioChannel * chan)
{
	return chan->Control.Status & mAUDIO_CTRL_ALLOCATED;
}

// FUNCTION: GAMEMD 0x00402440
void __fastcall AudioChannelReserve(AudioChannel * chan, AudioChannelType type)
{
	chan->Type = type;

	AudioChannelStop(chan);
	chan->Control.Status |= mAUDIO_CTRL_ALLOCATED;
}

// FUNCTION: GAMEMD 0x004024A0
void __fastcall AudioChannelRelease(AudioChannel * chan)
{
	if (chan->Control.Status & mAUDIO_CTRL_ALLOCATED) {
		chan->Control.Status &= ~mAUDIO_CTRL_ALLOCATED;
		chan->Type = AUDIO_CHANNEL_TYPE_STD;
		audioChannelMakeStandard(chan);
	}
}

// FUNCTION: GAMEMD 0x00402550
void __fastcall AudioChannelDestroy(AudioChannel * chan)
{
	AudioChannelStop(chan);

	audioRemoveChannel(chan);

	audioChannelDestroy(chan);
}

// FUNCTION: GAMEMD 0x004025B0
void __fastcall AudioChannelStop(AudioChannel * chan)
{
	chan->driver->lock(chan);
	if (chan->Control.Status & (mAUDIO_CTRL_PLAYING | mAUDIO_CTRL_PAUSED)) {
		chan->driver->stop(chan);
	}
	chan->driver->unlock(chan);

	if (chan->CB_Stop) {
		chan->CB_Stop(chan);
	}

	chan->sample = NULL;
}

// FUNCTION: GAMEMD 0x00402600
void __fastcall AudioChannelPause(AudioChannel * chan)
{
	chan->driver->lock(chan);
	if (chan->Control.Status & mAUDIO_CTRL_PLAYING) {
		chan->Control.Status &= ~mAUDIO_CTRL_PLAYING;
		chan->driver->pause(chan);
		chan->Control.Status |= mAUDIO_CTRL_PAUSED;
	}
	chan->driver->unlock(chan);
}

// FUNCTION: GAMEMD 0x00402650
void __fastcall AudioChannelResume(AudioChannel * chan)
{
	chan->driver->lock(chan);
	if (chan->Control.Status & mAUDIO_CTRL_PAUSED) {
		chan->driver->resume(chan);
		chan->Control.Status &= ~mAUDIO_CTRL_PAUSED;
		chan->Control.Status |= mAUDIO_CTRL_PLAYING;
	}
	chan->driver->unlock(chan);
}

// FUNCTION: GAMEMD 0x004026A0
void __fastcall AudioChannelLock(AudioChannel * chan)
{
	chan->driver->lock(chan);
}

// FUNCTION: GAMEMD 0x004026B0
void __fastcall AudioChannelUnlock(AudioChannel * chan)
{
	chan->driver->unlock(chan);
}

// FUNCTION: GAMEMD 0x004026C0
void __fastcall AudioChannelUse(AudioChannel * chan)
{
	chan->driver->lock(chan);
	chan->Control.Status |= mAUDIO_CTRL_INUSE;
	chan->driver->unlock(chan);
}

// FUNCTION: GAMEMD 0x004026F0
void __fastcall AudioChannelNoUse(AudioChannel * chan)
{
	chan->driver->lock(chan);
	chan->Control.Status &= ~mAUDIO_CTRL_INUSE;
	chan->driver->unlock(chan);
}

// FUNCTION: GAMEMD 0x00402720
int __fastcall AudioChannelStart(AudioChannel * chan)
{
	int err;

	if ((chan->Control.Status & (mAUDIO_CTRL_PAUSED | mAUDIO_CTRL_PLAYING))) {
		return ERROR_CODE_NO_CHANNEL;
	}
	if (chan->sample == NULL) {
		return ERROR_CODE_MISSING_DATA;
	}

	AudioAttribsUpdate(&chan->ChannelAttribs);
	audioChannelRecalcAttribs(chan);

	chan->driver->lock(chan);
	if ((err = chan->driver->start(chan)) == vNO_ERROR) {
		chan->Control.Status |= mAUDIO_CTRL_PLAYING;
	}
	chan->driver->unlock(chan);

	return err;
}

// FUNCTION: GAMEMD 0x00402790
void __fastcall AudioChannelSetSample(AudioChannel * chan, AudioSample * sample)
{
	if ((chan->sample = sample)) {
		if ((chan->frame = AudioSampleFirstFrame(sample))) {
			chan->frameData = (char *)chan->frame->Data;
			chan->bytesInFrame = chan->frame->Bytes;
			chan->bytesRemaining = chan->bytesInFrame;
			sample->Data = NULL;
		} else {
			chan->frameData = chan->sample->Data;
			chan->bytesInFrame = chan->sample->Bytes;
			chan->bytesRemaining = chan->bytesInFrame;
		}
	}
}

// FUNCTION: GAMEMD 0x00402800
int __fastcall AudioChannelSetFormat(AudioChannel * chan, AudioFormat * new_format)
{
	if (!memcmp(new_format, &chan->current_format, sizeof(AudioFormat))) {
		chan->format_changed = 0;
	} else {
		chan->format_changed = 1;
		chan->current_format = *new_format;
	}

	return chan->format_changed;
}

// FUNCTION: GAMEMD 0x00402850
void __fastcall AudioControlInit(AudioControl * ctrl)
{
	ctrl->LoopCount = 0;
	ctrl->Status = 0;
}
