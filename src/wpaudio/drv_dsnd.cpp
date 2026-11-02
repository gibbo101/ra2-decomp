// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#include "always.h"
#include <stdlib.h>
#include <string.h>
#include "auddsnd.h"

namespace Game { extern HWND hWnd; }

#define vAUD_DRV_MAX_CHANNELS 100
#define vAUD_DRV_POLL_INTERVAL (SECONDS(1) / 30)
#define vAUD_DRV_OVER_SAMPLE 8
#define vAUD_DRV_FRAMES 4
#define vAUD_DRV_LAG_FRAMES 0
#define TIMER_HZ 1000

#define STATE_READ_BLOCK 0
#define STATE_WRITE_BLOCK 1
#define STATE_DECODE_BLOCK 2
#define STATE_INIT_BLOCK 3

struct IMA_DATA
{
	int previousValue;
	int index;
};

struct AudioDriverChannel;

typedef int(__fastcall CB_DECODE)(AudioDriverChannel *);

struct TRANSFER
{
	int state;
	int next_state;
	unsigned char * in;
	int in_bytes;
	int in_bytes_left;
	int in_size;
	int in_size_needed;
	unsigned char * out;
	int out_bytes;
	int out_bytes_left;
	int out_size;
	int out_size_needed;
	int channels;
	int pending;
	int block_size;
	CB_DECODE * decode;
	IMA_DATA ima[2];
	unsigned gap0[2];
};

typedef int(__fastcall CB_TRANSFER)(AudioDriverChannel *, unsigned char * dst, int * dst_bytes, unsigned char * src, int * src_bytes);

struct AudioDriverChannel
{
	AudioChannel * chan;
	unsigned flags;
	volatile int pcm_pos;
	volatile unsigned frame;
	volatile TimeStamp last_poll;
	volatile TimeStamp poll_interval;
	volatile unsigned char * src_start;
	volatile int src_size;
	volatile unsigned char * src_pos;
	volatile int src_left;
	volatile unsigned long dst_pos;
	volatile unsigned long write_pos;
	volatile int dst_left;
	volatile int silence_left;
	int min_fill;
	AudioFormat format;
	LPDIRECTSOUNDBUFFER dsbuf;
	int buf_size;
	int frame_bytes;
	CB_TRANSFER * transfer;
	TRANSFER transfer_data;
	volatile int next_frame_called;
	volatile int service_count;
	volatile int frames_played;
	volatile int original_frame;
	volatile int original_dst_pos;
};

typedef AudioDriverChannel AUD_DRV_CHAN;

struct AudioThread
{
	volatile int quit;
	volatile int count;
	volatile int leaving;
	volatile TimeStamp interval;
	volatile int running;
	HANDLE handle;
	void * data;
	DWORD id;
};

static AudioThread thread;
static AudioLock drv_ref;
static CRITICAL_SECTION thread_access;
static int thread_created;

static int __fastcall audioLoad(AudioSystem * system);
static void __fastcall audioUnload(AudioSystem * system);
static int __fastcall audioOpen(AudioDevice * dev);
static void __fastcall audioClose(AudioDevice * dev);
static int __fastcall audioOpenChannel(AudioChannel * chan);
static int __fastcall audioCloseChannel(AudioChannel * chan);
static int __fastcall audioStart(AudioChannel * chan);
static int __fastcall audioStop(AudioChannel * chan);
static int __fastcall audioPause(AudioChannel * chan);
static int __fastcall audioResume(AudioChannel * chan);
static int __fastcall audioCheck(AudioChannel * chan);
static int __fastcall audioUpdate(AudioChannel * chan);
static int __fastcall audioQueueIt(AudioChannel * chan);
static int __fastcall audioLock(AudioChannel * chan);
static int __fastcall audioUnlock(AudioChannel * chan);

static AudioDriver _AUD_driver = {
	NULL,
	audioOpenChannel,
	audioCloseChannel,
	audioStart,
	audioStop,
	audioPause,
	audioResume,
	audioCheck,
	audioUpdate,
	audioQueueIt,
	audioLock,
	audioUnlock
};

static AudioSystemMaster _AUD_system_master = {
	"Direct Sound Driver V2.0",
	0,
	0,
	0,
	'AUDm',
	audioLoad,
	audioUnload,
	audioOpen,
	audioClose,
	&_AUD_driver
};

LPDIRECTSOUND AUD_sound_object = NULL;
LPDIRECTSOUNDBUFFER AUD_primary_buffer = NULL;

static int AUD_log_table[101] = {
	-10000, -6644, -5644, -5059, -4644, -4322, -4059, -3837, -3644, -3474,
	-3322, -3184, -3059, -2943, -2837, -2737, -2644, -2556, -2474, -2396,
	-2322, -2252, -2184, -2120, -2059, -2000, -1943, -1889, -1837, -1786,
	-1737, -1690, -1644, -1599, -1556, -1515, -1474, -1434, -1396, -1358,
	-1322, -1286, -1252, -1218, -1184, -1152, -1120, -1089, -1059, -1029,
	-1000, -971, -943, -916, -889, -862, -837, -811, -786, -761,
	-737, -713, -690, -667, -644, -621, -599, -578, -556, -535,
	-515, -494, -474, -454, -434, -415, -396, -377, -358, -340,
	-322, -304, -286, -269, -252, -234, -218, -201, -184, -168,
	-152, -136, -120, -105, -89, -74, -59, -44, -29, -14,
	0
};

AudioSystemMaster * AudioSystemDSD = &_AUD_system_master;

static int imaIndexAdjustTable[16] = {
	-1, -1, -1, -1,
	2, 4, 6, 8,
	-1, -1, -1, -1,
	2, 4, 6, 8,
};

static int imaStepSizeTable[89] = {
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34,
	37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143,
	157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494,
	544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552,
	1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026,
	4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442,
	11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623,
	27086, 29794, 32767
};

DWORD WINAPI AUD_service_thread(AudioThread * thr);
static void __fastcall AUD_new_buffer_src(AUD_DRV_CHAN * ci);
static int __fastcall AUD_fill_buffer(AUD_DRV_CHAN * ci, int bytes);
void __fastcall AUD_service_attribs(AudioDevice * dev);
static CB_TRANSFER PCM_transfer;
static CB_TRANSFER ADPCM_transfer;
static CB_DECODE IMA_decode_block;
short __fastcall ImaAdpcmDecode(unsigned char deltaCode, IMA_DATA * state);

static void transfer_init(TRANSFER * trans)
{
	trans->decode = NULL;
	trans->in = NULL;
	trans->out = NULL;
	trans->pending = FALSE;
}

static void AUD_set_wave_format(WAVEFORMATEX * pcmwf, AudioFormat * format)
{
	memset(pcmwf, 0, sizeof(WAVEFORMATEX));
	pcmwf->wFormatTag = WAVE_FORMAT_PCM;
	pcmwf->nChannels = (unsigned short)format->Channels;
	pcmwf->nSamplesPerSec = format->Rate;
	pcmwf->nBlockAlign = (unsigned short)format->Channels * format->SampleWidth;
	pcmwf->nAvgBytesPerSec = pcmwf->nSamplesPerSec * pcmwf->nBlockAlign;
	pcmwf->wBitsPerSample = (unsigned short)format->SampleWidth << 3;
	pcmwf->cbSize = 0;
}

static int AUD_same_format(AudioFormat * f1, AudioFormat * f2)
{
	return (f1->Channels == f2->Channels && f1->Rate == f2->Rate && f1->SampleWidth == f2->SampleWidth);
}

static int AUD_create_pcm_buffer(AUD_DRV_CHAN * ci, AudioFormat * format)
{
	DSBUFFERDESC desc;
	WAVEFORMATEX pcmwf;

	AudioChannelSetFormat(ci->chan, format);
	if (ci->dsbuf) {
		if (AUD_same_format(&ci->format, format)) {
			ci->chan->drv_format_changed = FALSE;
			return NO_ERROR;
		}
		ci->chan->drv_format_changed = TRUE;
	}
	if (ci->dsbuf) {
		ci->dsbuf->Release();
		ci->dsbuf = NULL;
	}
	AUD_set_wave_format(&pcmwf, format);
	ci->format = *format;
	memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = (DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE);
	ci->frame_bytes = ((pcmwf.nAvgBytesPerSec * (vAUD_DRV_POLL_INTERVAL / 10)) / (TIMER_HZ / 10)) * vAUD_DRV_OVER_SAMPLE;
	ci->frame_bytes = ((ci->frame_bytes + 1024 - 1) / 1024) * 1024;
	FUN_004068E0("Sound frame size = %d bytes\n", ci->frame_bytes);
	desc.dwBufferBytes = vAUD_DRV_FRAMES * ci->frame_bytes;
	desc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;
	if (AUD_sound_object->CreateSoundBuffer(&desc, &ci->dsbuf, NULL) != DS_OK) {
		ci->dsbuf = NULL;
		return ERROR_CODE_FAIL;
	}
	ci->buf_size = desc.dwBufferBytes;
	FUN_004068E0("Sound buffer size = %d bytes\n", ci->buf_size);
	return vNO_ERROR;
}

static unsigned AUD_get_current_frame(AUD_DRV_CHAN * ci)
{
	unsigned long pos;

	ci->dsbuf->GetCurrentPosition(&pos, (unsigned long *)&ci->write_pos);
	return pos / ci->frame_bytes;
}

static void __fastcall AUD_init_buffer(AUD_DRV_CHAN * ci)
{
	ci->src_left = 0;
	ci->pcm_pos = 0;
	ci->silence_left = ci->frame_bytes * 4;
	ci->dsbuf->SetCurrentPosition(0);
	ci->frame = AUD_get_current_frame(ci);
	ci->last_poll = AudioGetTime();
	ci->min_fill = ci->buf_size;
	ci->dsbuf->GetCurrentPosition((unsigned long *)&ci->dst_pos, (unsigned long *)&ci->write_pos);
	ci->dst_left = ci->buf_size - ci->dst_pos;
	ci->service_count = 0;
	ci->original_frame = ci->frame;
	ci->frames_played = 0;
	ci->original_dst_pos = ci->dst_pos;
}

static int AUD_restore_buffer(AUD_DRV_CHAN * ci)
{
	if (ci->dsbuf->Restore() == DS_OK) {
		AUD_init_buffer(ci);
		return TRUE;
	}
	return FALSE;
}

static int AUD_lock_buffer(AUD_DRV_CHAN * ci, unsigned long pos, unsigned long transfer, unsigned char ** dst, unsigned long * bytes1, unsigned char ** dst2, unsigned long * bytes2)
{
	int retval;
	int retry_count = 3;

retry:
	if ((retval = ci->dsbuf->Lock(pos, transfer, (void **)dst, bytes1, (void **)dst2, bytes2, 0)) != DS_OK) {
		if (retval = DSERR_BUFFERLOST) {
			if (!AUD_restore_buffer(ci)) {
				FUN_004068E0("Failed to restore buffer\n");
				return FALSE;
			}
			FUN_004068E0("Restored buffer\n");
			if (retry_count--) {
				goto retry;
			}
		}
		FUN_004068E0("Failed to lock buffer\n");
		return FALSE;
	}
	return TRUE;
}

static void __fastcall AUD_set_buffer_src(AUD_DRV_CHAN * ci)
{
	ci->next_frame_called = FALSE;
	ci->src_pos = (unsigned char *)ci->chan->frameData;
	ci->src_start = (unsigned char *)ci->chan->frameData;
	ci->src_left = ci->src_size = ci->chan->bytesInFrame;
}

static void AUD_channel_stop(AudioChannel * chan)
{
	AUD_DRV_CHAN * ci;

	ci = chan->drvData;
	if (ci->dsbuf) {
		ci->dsbuf->Stop();
	}
	if (chan->drvCBSampleDone) {
		chan->drvCBSampleDone(chan);
	}
}

// FUNCTION: GAMEMD 0x00409470
static int __fastcall audioLoad(AudioSystem * system)
{
	int result;

	InitializeCriticalSection(&thread_access);
	result = DirectSoundCreate(NULL, &AUD_sound_object, NULL);
	if (result != DS_OK) {
		return ERROR_CODE_FAIL;
	}
	result = AUD_sound_object->SetCooperativeLevel(Game::hWnd, DSSCL_PRIORITY);
	system->numUnits = 1;
	if (!thread_created) {
		drv_ref.count = 0;
		thread.quit = 0;
		thread.leaving = 0;
		thread.running = 0;
		thread.interval = vAUD_DRV_POLL_INTERVAL;
		thread.count = 0;
		if (!(thread.handle = CreateThread(NULL, 4 * 1024, (LPTHREAD_START_ROUTINE)AUD_service_thread, &thread, 0, &thread.id))) {
			return ERROR_CODE_FAIL;
		}
		drv_ref.count++;
		thread_created = 1;
		if (!SetThreadPriority(thread.handle, THREAD_PRIORITY_TIME_CRITICAL)) {
			char buffer[1024];

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, buffer, sizeof(buffer), NULL);
			FUN_004068E0("Unable to change the priority of the Audio Thread - %s\n", buffer);
			return 0;
		}
	} else {
		drv_ref.count++;
	}
	return 0;
}

static void AUD_update_buffer(AUD_DRV_CHAN * ci)
{
	unsigned frame = AUD_get_current_frame(ci);
	int dif = frame - ci->frame;
	if (dif) {
		if (dif < 0) {
			dif = frame + (vAUD_DRV_FRAMES - ci->frame);
		}
		if ((dif -= vAUD_DRV_LAG_FRAMES) > 0) {
			if ((ci->frame += dif) >= vAUD_DRV_FRAMES) {
				ci->frame %= vAUD_DRV_FRAMES;
			}
			ci->frames_played += dif;
			dif *= ci->frame_bytes;
			int filled = AUD_fill_buffer(ci, dif);
			if (ci->min_fill > dif) {
				ci->min_fill = dif;
			}
			if ((ci->pcm_pos -= dif) <= 0 || !filled) {
				ci->dsbuf->Stop();
			}
		}
	}
	ci->service_count++;
}

static int AUD_service_device(AudioDevice * dev)
{
	AudioSearch sh;

	if (!dev->channelAccess.count) {
		EnterCriticalSection(&thread_access);
		TimeStamp now = AudioGetTime();
		AudioServicePerform(&dev->mixerUpdate, now);
		AudioChannel * chan = AudioDeviceFirstChannel(dev, &sh);
		while (chan) {
			if (chan->Status & mAUDIO_CTRL_PLAYING) {
				if (audioCheck(chan)) {
					chan->drvData->poll_interval = now - chan->drvData->last_poll;
					chan->drvData->last_poll = now;
					AUD_update_buffer(chan->drvData);
				} else {
					AUD_channel_stop(chan);
				}
			}
			chan = AudioDeviceNextChannel(&sh);
		}
		AUD_service_attribs(dev);
		LeaveCriticalSection(&thread_access);
		return TRUE;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x004095B0
DWORD WINAPI AUD_service_thread(AudioThread * thr)
{
	thr->running = TRUE;
	thr->leaving = FALSE;
	while (!thr->quit) {
		if (thr->data && !AUD_service_device((AudioDevice *)thr->data)) {
			Sleep(5);
		} else {
			Sleep((DWORD)thr->interval);
		}
		thr->count++;
	}
	thr->leaving = TRUE;
	return 0;
}

// FUNCTION: GAMEMD 0x00409880
static int __fastcall AUD_fill_buffer(AUD_DRV_CHAN * ci, int total_bytes)
{
	unsigned long transfer, bytes_transfer;
	unsigned long bytes;
	unsigned char * lpos1 = NULL, * lpos2 = NULL;
	unsigned long lbytes1 = 0, lbytes2 = 0;
	unsigned char * dst;
	int locked = FALSE;
	int result = FALSE;

	while (total_bytes > 0) {
		if (ci->dst_left <= 0) {
			ci->dst_pos = 0;
			ci->dst_left = ci->buf_size;
		}
		bytes_transfer = 0;
		if ((bytes = (unsigned long)total_bytes) > (unsigned long)ci->dst_left) {
			bytes = ci->dst_left;
		}
		if (locked) {
			ci->dsbuf->Unlock(lpos1, lbytes1, lpos2, lbytes2);
			locked = FALSE;
		}
		if (!AUD_lock_buffer(ci, ci->dst_pos, bytes, &lpos1, &lbytes1, &lpos2, &lbytes2)) {
			goto done;
		}
		locked = TRUE;
		if ((lpos2 != NULL) || (lbytes1 != bytes) || (lbytes2 != 0)) {
			FUN_004068E0("Buffer management out by %d bytes\n", lbytes2);
			goto done;
		}
		dst = lpos1;
		while (bytes) {
			transfer = bytes;
			if (ci->src_left == 0) {
				ci->src_left = -1;
				if (!ci->next_frame_called) {
					ci->chan->drvCBNextFrame(ci->chan);
					ci->next_frame_called = TRUE;
				}
				if (!ci->chan->bytesInFrame) {
					if (!ci->transfer_data.pending) {
						ci->chan->drvCBNextSample(ci->chan);
						if (ci->chan->bytesInFrame) {
							AUD_new_buffer_src(ci);
						}
					}
				}
				if (ci->chan->bytesInFrame) {
					AUD_set_buffer_src(ci);
				}
				if (ci->src_left == 0) {
					ci->src_left = -1;
				}
			}
			if (ci->src_left <= 0 && !ci->transfer_data.pending) {
				if (ci->chan->current_format.SampleWidth == 2) {
					memset(dst, 0x0000, transfer);
				} else {
					memset(dst, 0x80, transfer);
				}
				ci->silence_left -= transfer;
			} else {
				int src_bytes;

				if ((src_bytes = ci->src_left) <= 0) {
					if (!ci->transfer(ci, dst, (int *)&transfer, NULL, NULL)) {
						goto done;
					}
					src_bytes = 0;
					ci->src_left = 0;
				} else {
					if (!ci->transfer(ci, dst, (int *)&transfer, (unsigned char *)ci->src_pos, &src_bytes)) {
						goto done;
					}
				}
				ci->src_pos += src_bytes;
				ci->src_left -= src_bytes;
				ci->pcm_pos += transfer;
			}
			dst += transfer;
			ci->dst_pos += transfer;
			ci->dst_left -= transfer;
			bytes -= transfer;
			bytes_transfer += transfer;
		}
		total_bytes -= bytes_transfer;
	}
	result = TRUE;
done:
	if (locked) {
		ci->dsbuf->Unlock(lpos1, lbytes1, lpos2, lbytes2);
		locked = FALSE;
	}
	return result;
}

// FUNCTION: GAMEMD 0x00409C40
static void __fastcall AUD_new_buffer_src(AUD_DRV_CHAN * ci)
{
	AudioSample * sample = ci->chan->sample;
	AudioFormat * format;
	TRANSFER * data = &ci->transfer_data;

	if (!sample) {
		return;
	}
	format = sample->Format;
	if (!format) {
		format = &ci->chan->Device->DefaultFormat;
	}
	data->out_bytes_left = 0;
	data->in_bytes_left = 0;
	data->in_bytes = 0;
	data->state = STATE_INIT_BLOCK;
	data->in_size_needed = 0;
	data->out_size_needed = 0;
	data->channels = format->Channels;
	data->pending = FALSE;
	switch (format->Compression) {
	case AUDIO_COMPRESS_NONE:
		ci->transfer = PCM_transfer;
		break;
	case AUDIO_COMPRESS_IMA_ADPCM:
		ci->transfer = ADPCM_transfer;
		data->decode = IMA_decode_block;
		data->in_size_needed = format->BlockSize;
		data->block_size = format->BlockSize;
		data->out_size_needed = (format->BlockSize + 32) * 4;
		break;
	case AUDIO_COMPRESS_MS_ADPCM:
		ci->transfer = ADPCM_transfer;
		break;
	}
	if (data->in_size < data->in_size_needed) {
		if (data->in) {
			free(data->in);
		}
		data->in = (unsigned char *)malloc(data->in_size_needed);
		if (!data->in) {
			data->in_size = 0;
		} else {
			data->in_size = data->in_size_needed;
		}
	}
	if (data->out_size < data->out_size_needed) {
		if (data->out) {
			free(data->out);
		}
		data->out = (unsigned char *)malloc(data->out_size_needed);
		if (!data->out) {
			data->out_size = 0;
		} else {
			data->out_size = data->out_size_needed;
		}
	}
}

// FUNCTION: GAMEMD 0x00409D90
static int __fastcall PCM_transfer(AUD_DRV_CHAN *, unsigned char * dst, int * dst_bytes, unsigned char * src, int * src_bytes)
{
	int transfer;

	if ((transfer = *dst_bytes) > *src_bytes) {
		transfer = *src_bytes;
	}
	if (transfer) {
		memcpy(dst, src, transfer);
		*dst_bytes = transfer;
		*src_bytes = transfer;
	}
	return TRUE;
}

// FUNCTION: GAMEMD 0x00409DE0
static int __fastcall ADPCM_transfer(AUD_DRV_CHAN * ci, unsigned char * dst, int * dst_bytes, unsigned char * src, int * src_bytes)
{
	int sbytes = 0;
	int dbytes = 0;
	int stransfered = 0;
	int dtransfered = 0;
	int bytes;
	int done = FALSE;
	TRANSFER * data = &ci->transfer_data;

	if (!data->out || !data->in) {
		return FALSE;
	}
	if (src_bytes) {
		sbytes = *src_bytes;
	}
	if (dst_bytes) {
		dbytes = *dst_bytes;
	}
	while (!done) {
		switch (data->state) {
		case STATE_READ_BLOCK:
			if (!data->in_bytes_left || src == NULL) {
				data->state = data->next_state;
				data->in_bytes_left = 0;
				break;
			}
			if ((bytes = data->in_bytes_left) > sbytes) {
				bytes = sbytes;
			}
			if (bytes == 0) {
				done = TRUE;
				break;
			}
			memcpy(&data->in[data->in_bytes - data->in_bytes_left], src, bytes);
			src += bytes;
			stransfered += bytes;
			sbytes -= bytes;
			data->in_bytes_left -= bytes;
			data->pending = TRUE;
			break;
		case STATE_WRITE_BLOCK:
			if (!data->out_bytes_left) {
				data->state = data->next_state;
				data->pending = FALSE;
				break;
			}
			if ((bytes = data->out_bytes_left) > dbytes) {
				bytes = dbytes;
			}
			if (bytes == 0) {
				done = TRUE;
				break;
			}
			memcpy(dst, &data->out[data->out_bytes - data->out_bytes_left], bytes);
			dst += bytes;
			dtransfered += bytes;
			dbytes -= bytes;
			data->out_bytes_left -= bytes;
			break;
		case STATE_DECODE_BLOCK:
			if (!data->decode(ci)) {
				return FALSE;
			}
			data->state = STATE_WRITE_BLOCK;
			data->out_bytes_left = data->out_bytes;
			data->next_state = STATE_INIT_BLOCK;
			break;
		case STATE_INIT_BLOCK:
			if (src != NULL) {
				data->in_bytes = data->block_size;
				data->in_bytes_left = data->block_size;
				data->state = STATE_READ_BLOCK;
				data->next_state = STATE_DECODE_BLOCK;
			} else {
				done = TRUE;
			}
			break;
		}
	}
	if (src_bytes) {
		*src_bytes = stransfered;
	}
	if (dst_bytes) {
		*dst_bytes = dtransfered;
	}
	return TRUE;
}

// FUNCTION: GAMEMD 0x00409FF0
static void __fastcall audioUnload(AudioSystem *)
{
	if (AUD_sound_object) {
		AUD_sound_object->Release();
		AUD_sound_object = NULL;
	}
	if (thread_created) {
		if (--drv_ref.count < 0) {
			drv_ref.count = 0;
		}
		if (!drv_ref.count) {
			thread.quit = TRUE;
			while (!thread.leaving) {
			}
			WaitForSingleObject(thread.handle, SECONDS(5));
			CloseHandle(thread.handle);
			thread_created = 0;
		}
	}
	DeleteCriticalSection(&thread_access);
}

// FUNCTION: GAMEMD 0x0040A090
static int __fastcall audioOpen(AudioDevice * dev)
{
	DSBUFFERDESC dsbdesc;
	WAVEFORMATEX pcmwf;

	dev->maxChannels = vAUD_DRV_MAX_CHANNELS;
	AudioServiceSetInterval(&dev->mixerUpdate, vAUD_DRV_POLL_INTERVAL);
	AudioServiceSetMustServiceInterval(&dev->mixerUpdate, vAUD_DRV_POLL_INTERVAL * vAUD_DRV_OVER_SAMPLE * vAUD_DRV_FRAMES);
	AudioServiceSetResetInterval(&dev->mixerUpdate, vAUD_DRV_POLL_INTERVAL * vAUD_DRV_OVER_SAMPLE * vAUD_DRV_FRAMES * 3);
	thread.data = dev;
	AUD_set_wave_format(&pcmwf, &dev->DefaultFormat);
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbdesc.dwBufferBytes = 0;
	if (AUD_sound_object->CreateSoundBuffer(&dsbdesc, &AUD_primary_buffer, NULL) != DS_OK) {
		AUD_primary_buffer = NULL;
		return ERROR_CODE_FAIL;
	}
	if (AUD_primary_buffer->SetFormat(&pcmwf) != DS_OK) {
		FUN_004068E0("Unable to set the desired primary format\n");
	}
	{
		DSCAPS caps = { 0 };

		caps.dwSize = sizeof(caps);
		AUD_sound_object->GetCaps(&caps);
	}
	AUD_primary_buffer->Play(0, 0, DSBPLAY_LOOPING);
	dev->frames = vAUD_DRV_FRAMES;
	dev->over_sample = vAUD_DRV_OVER_SAMPLE;
	dev->frame_lag = vAUD_DRV_LAG_FRAMES;
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x0040A210
static void __fastcall audioClose(AudioDevice *)
{
	EnterCriticalSection(&thread_access);
	thread.data = NULL;
	LeaveCriticalSection(&thread_access);
	if (AUD_primary_buffer) {
		AUD_primary_buffer->Stop();
		AUD_primary_buffer->Release();
		AUD_primary_buffer = NULL;
	}
}

// FUNCTION: GAMEMD 0x0040A260
static int __fastcall audioOpenChannel(AudioChannel * chan)
{
	AUD_DRV_CHAN * ci;

	ci = (AUD_DRV_CHAN *)operator new(sizeof(AUD_DRV_CHAN));
	memset(ci, 0, sizeof(AUD_DRV_CHAN));
	chan->drvData = ci;
	ci->chan = chan;
	chan->time_min_frame = vAUD_DRV_POLL_INTERVAL * vAUD_DRV_OVER_SAMPLE;
	chan->time_buffer = vAUD_DRV_FRAMES * chan->time_min_frame;
	transfer_init(&ci->transfer_data);
	AudioChannelSetFormat(ci->chan, &ci->chan->Device->DefaultFormat);
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x0040A2D0
static int __fastcall audioCloseChannel(AudioChannel * chan)
{
	AUD_DRV_CHAN * ci;

	if ((ci = chan->drvData)) {
		if (ci->transfer_data.in) {
			free(ci->transfer_data.in);
			ci->transfer_data.in = NULL;
			ci->transfer_data.in_size = 0;
		}
		if (ci->transfer_data.out) {
			free(ci->transfer_data.out);
			ci->transfer_data.out = NULL;
			ci->transfer_data.out_size = 0;
		}
		if (ci->dsbuf) {
			ci->dsbuf->Release();
			ci->dsbuf = NULL;
		}
		operator delete(ci);
		chan->drvData = NULL;
	}
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x0040A340
static int __fastcall audioStart(AudioChannel * chan)
{
	AUD_DRV_CHAN * ci;
	AudioFormat * format;
	AudioSample * sample;
	DSBUFFERDESC desc;
	WAVEFORMATEX pcmwf;

	ci = chan->drvData;
	sample = chan->sample;
	if (!(format = sample->Format)) {
		format = &chan->Device->DefaultFormat;
	}
	if (AUD_create_pcm_buffer(ci, format) != vNO_ERROR) {
		FUN_004068E0("Failed to create playback buffer\n");
		return ERROR_CODE_FAIL;
	}
	AUD_init_buffer(ci);
	AUD_new_buffer_src(ci);
	AUD_set_buffer_src(ci);
	if (!AUD_fill_buffer(ci, ci->buf_size)) {
		FUN_004068E0("Failed to prefill buffer\n");
		return ERROR_CODE_FAIL;
	}
	audioUpdate(chan);
	if (ci->dsbuf->Play(0, 0, DSBPLAY_LOOPING) != DS_OK) {
		FUN_004068E0("Failed to start playback\n");
		return ERROR_CODE_FAIL;
	}
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x0040A610
static int __fastcall audioStop(AudioChannel * chan)
{
	if (chan->Status & mAUDIO_CTRL_ACTIVE) {
		AUD_channel_stop(chan);
	}
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x0040A650
static int __fastcall audioPause(AudioChannel * chan)
{
	AUD_DRV_CHAN * ci = chan->drvData;

	if (ci->dsbuf) {
		ci->dsbuf->Stop();
	}
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x0040A670
static int __fastcall audioResume(AudioChannel * chan)
{
	AUD_DRV_CHAN * ci = chan->drvData;

	if (ci->dsbuf) {
		if (ci->dsbuf->Play(0, 0, DSBPLAY_LOOPING) == DS_OK) {
			return vNO_ERROR;
		}
	}
	return ERROR_CODE_FAIL;
}

// FUNCTION: GAMEMD 0x0040A6A0
static int __fastcall audioCheck(AudioChannel * chan)
{
	AUD_DRV_CHAN * ci = chan->drvData;
	DWORD status;

	if (ci->dsbuf && (ci->dsbuf->GetStatus(&status) == DS_OK)) {
		return (status & (DSBSTATUS_PLAYING | DSBSTATUS_LOOPING) == (DSBSTATUS_PLAYING | DSBSTATUS_LOOPING));
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x0040A6D0
static int __fastcall audioUpdate(AudioChannel * chan)
{
	AUD_DRV_CHAN * ci;
	int vol;
	int pitch;

	ci = chan->drvData;
	vol = AUDIO_LEVEL_APPLY(&chan->attribs.VolumeLevel, 100);
	ci->dsbuf->SetVolume(AUD_log_table[vol]);
	{
		int pos;

		pos = AUDIO_LEVEL_APPLY(&chan->attribs.PanPosition, 200) - 100;
		if (pos < 0) {
			ci->dsbuf->SetPan(AUD_log_table[100 + pos]);
		} else {
			ci->dsbuf->SetPan(-AUD_log_table[100 - pos]);
		}
	}
	pitch = AudioAttribsCalcPitch(&chan->attribs, ci->chan->current_format.Rate);
	ci->dsbuf->SetFrequency(pitch);
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x0040A770
static int __fastcall audioQueueIt(AudioChannel *)
{
	return ERROR_CODE_FAIL;
}

// FUNCTION: GAMEMD 0x0040A780
static int __fastcall audioLock(AudioChannel *)
{
	EnterCriticalSection(&thread_access);
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x0040A790
static int __fastcall audioUnlock(AudioChannel *)
{
	LeaveCriticalSection(&thread_access);
	return vNO_ERROR;
}

// FUNCTION: GAMEMD 0x0040A7A0
void * GetDirectSoundObject(void)
{
	return (void *)AUD_sound_object;
}

// FUNCTION: GAMEMD 0x0040A7B0
void * GetPrimaryBuffer(void)
{
	return (void *)AUD_primary_buffer;
}

// FUNCTION: GAMEMD 0x0040A7C0
void __fastcall AudioSetPrimaryFormat(int rate, int bits, int channels)
{
	WAVEFORMATEX wf;

	if (AUD_primary_buffer) {
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.nChannels = (unsigned short)channels;
		wf.nSamplesPerSec = rate;
		wf.nBlockAlign = (unsigned short)(bits / 8 * channels);
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
		wf.wBitsPerSample = (unsigned short)bits;
		wf.cbSize = 0;
		AUD_primary_buffer->Stop();
		AUD_primary_buffer->SetFormat(&wf);
		AUD_primary_buffer->Play(0, 0, DSBPLAY_LOOPING);
	}
}

// FUNCTION: GAMEMD 0x0040A850
void AudioSetDeviceFormat(void)
{
	AudioDevice * dev = FUN_00407010();

	if (AUD_primary_buffer && dev) {
		WAVEFORMATEX wf;
		AudioFormat * format = &dev->DefaultFormat;

		AUD_primary_buffer->Stop();
		memset(&wf, 0, sizeof(WAVEFORMATEX));
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.nChannels = (unsigned short)format->Channels;
		wf.nSamplesPerSec = format->Rate;
		wf.nBlockAlign = (unsigned short)format->Channels * format->SampleWidth;
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
		wf.wBitsPerSample = (unsigned short)format->SampleWidth << 3;
		wf.cbSize = 0;
		AUD_primary_buffer->SetFormat(&wf);
		AUD_primary_buffer->Play(0, 0, DSBPLAY_LOOPING);
	}
}

// FUNCTION: GAMEMD 0x0040A900
int __fastcall AudioGetLogVolume(int volume)
{
	int vol = volume * 100 / 256;

	if (vol > 100) {
		vol = 100;
	} else if (vol < 0) {
		vol = 0;
	}
	return AUD_log_table[vol];
}

// FUNCTION: GAMEMD 0x0040A940
void AudioLoseFocus(void)
{
	if (AUD_primary_buffer) {
		AUD_primary_buffer->Stop();
	}
}

// FUNCTION: GAMEMD 0x0040A950
void AudioRegainFocus(void)
{
	if (AUD_primary_buffer) {
		DWORD status;

		AUD_primary_buffer->GetStatus(&status);
		if (status & DSBSTATUS_BUFFERLOST) {
			AUD_primary_buffer->Restore();
		}
		if (!(status & DSBSTATUS_PLAYING)) {
			AUD_primary_buffer->Play(0, 0, DSBPLAY_LOOPING);
		}
	}
}

// FUNCTION: GAMEMD 0x0040A9A0
void __fastcall AUD_service_attribs(AudioDevice * dev)
{
	if (dev) {
		AudioAttribsUpdate(DAT_0087E744);
		AudioAttribsUpdate(DAT_0087E748);
		AudioAttribsUpdate(DAT_0087E740);
		AudioAttribsUpdate(DAT_0087E73C);
		AudioAttribsUpdate(DAT_0087E754);
		AudioAttribsUpdate(DAT_0087E758);
		AudioAttribsUpdate(DAT_0087E750);
		AudioAttribsUpdate(DAT_0087E74C);
		AudioDeviceService(dev);
		AudioAttribsUsed(DAT_0087E748);
		AudioAttribsUsed(DAT_0087E744);
		AudioAttribsUsed(DAT_0087E740);
		AudioAttribsUsed(DAT_0087E73C);
		AudioAttribsUsed(DAT_0087E754);
		AudioAttribsUsed(DAT_0087E758);
		AudioAttribsUsed(DAT_0087E750);
		AudioAttribsUsed(DAT_0087E74C);
	}
}

// FUNCTION: GAMEMD 0x0040AA70
static int __fastcall IMA_decode_block(AUD_DRV_CHAN * ci)
{
	int i, j;
	TRANSFER * data = &ci->transfer_data;
	int bytes = data->in_bytes - data->in_bytes_left;
	unsigned char * in = data->in;
	unsigned short * out;

	data->out_bytes = 0;
	out = (unsigned short *)data->out;
	if (bytes < data->channels * 4) {
		return FALSE;
	}
	for (i = 0; i < data->channels; i++) {
		data->ima[i].previousValue = (int)(in[1] << 8) + (int)in[0];
		if (data->ima[i].previousValue & 0x8000) {
			data->ima[i].previousValue -= 0x10000;
		}
		if ((data->ima[i].index = in[2]) > 88) {
			return FALSE;
		}
		if (in[3]) {
			return FALSE;
		}
		in += 4;
		bytes -= 4;
		*out++ = (unsigned short)data->ima[i].previousValue;
		data->out_bytes += 2;
	}
	if (data->channels == 1) {
		while (bytes > 0) {
			unsigned short * o = (unsigned short *)&data->out[data->out_bytes];

			j = 4;
			while (j--) {
				unsigned char b = *in++;

				*o++ = (unsigned short)ImaAdpcmDecode(b & 0x0f, &data->ima[0]);
				*o++ = (unsigned short)ImaAdpcmDecode((b >> 4) & 0x0f, &data->ima[0]);
			}
			data->out_bytes += 16;
			bytes -= 4;
		}
	} else {
		while (bytes > 0) {
			for (i = 0; i < data->channels; i++) {
				short * o = (short *)&data->out[data->out_bytes + (i << 1)];

				for (j = 0; j < 4; j++) {
					unsigned char b = *in++;

					*o = ImaAdpcmDecode(b & 0x0f, &data->ima[i]);
					o += data->channels;
					*o = ImaAdpcmDecode((b >> 4) & 0x0f, &data->ima[i]);
					o += data->channels;
				}
			}
			data->out_bytes += data->channels << 4;
			bytes -= data->channels << 2;
		}
	}
	if (bytes != 0) {
		return FALSE;
	}
	return TRUE;
}

// FUNCTION: GAMEMD 0x0040ACD0
short __fastcall ImaAdpcmDecode(unsigned char deltaCode, IMA_DATA * state)
{
	int step;
	int difference;

	step = imaStepSizeTable[state->index];
	difference = step >> 3;
	if (deltaCode & 1) {
		difference += step >> 2;
	}
	if (deltaCode & 2) {
		difference += step >> 1;
	}
	if (deltaCode & 4) {
		difference += step;
	}
	if (deltaCode & 8) {
		difference = -difference;
	}
	state->previousValue += difference;
	if (state->previousValue > 32767) {
		state->previousValue = 32767;
	} else if (state->previousValue < -32768) {
		state->previousValue = -32768;
	}
	state->index += imaIndexAdjustTable[deltaCode];
	if (state->index < 0) {
		state->index = 0;
	} else if (state->index > 88) {
		state->index = 88;
	}
	return (short)state->previousValue;
}
