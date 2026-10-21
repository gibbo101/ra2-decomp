// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include	"vqaplayp.h"
#include	<stdio.h>
#include	<memory.h>
#include	"ahandle.h"
#include	"vqamem.h"

// FUNCTION: GAMEMD 0x007BD170
long VQA_OpenAudio(VQAHandleP *vqap)
{
	VQAAudio *audio;
	long          rc;

	audio = &vqap->Audio;

	if (audio->Buffer == NULL) {
		return(VQAERR_AUDIO);
	}

	audio->Block1 = 0;
	audio->Block2 = -1;
	audio->BufferPosition = 0;
	vqap->RepeatedBuffers = 0;

	AhandleInitParams params;
	params.SampleRate = vqap->SampleRate;
	params.Channels = vqap->Channels;
	params.BitsPerSample = vqap->BitsPerSample;
	params.Callback1 = VQA_AudioFillCallback;
	params.Callback2 = VQA_AudioDoneCallback;

	rc = vqap->Config.AudioHandler((VQAHandle *)vqap, VQAAUDIO_OPEN, &params, sizeof(params));
	if (rc >= VQAERR_OK || rc == VQAERR_NONE) {
		if ((audio->Flags & VQAAUDF_MODLOCKED) == 0) {
			audio->Flags |= VQAAUDF_MODLOCKED;
		}
		return(VQAERR_NONE);
	}
	return(rc);
}

// FUNCTION: GAMEMD 0x007BD210
void VQA_CloseAudio(VQAHandleP *vqap)
{
	VQAAudio *audio;

	audio = &vqap->Audio;

	if (audio->Flags & VQAAUDF_ISPLAYING) {
		VQA_StopAudio(vqap);
	}
	vqap->Config.AudioHandler((VQAHandle *)vqap, VQAAUDIO_CLOSE, NULL, NULL);

	if ((audio->Flags & VQAAUDF_MODLOCKED) == 1) {
		audio->Flags &= ~VQAAUDF_MODLOCKED;
	}
}

// FUNCTION: GAMEMD 0x007BD270
void VQA_StartAudio(VQAHandleP *vqap)
{
	VQAAudio *audio;

	audio = &vqap->Audio;

	vqap->Config.AudioHandler((VQAHandle *)vqap, VQAAUDIO_START, NULL, NULL);
	audio->Flags |= VQAAUDF_ISPLAYING;
}

// FUNCTION: GAMEMD 0x007BD290
void VQA_PauseAudio(VQAHandleP *vqap)
{
	vqap->Config.AudioHandler((VQAHandle *)vqap, VQAAUDIO_PAUSE, NULL, NULL);
	vqap->Audio.Flags &= ~VQAAUDF_ISPLAYING;
}

// FUNCTION: GAMEMD 0x007BD2B0
void VQA_StopAudio(VQAHandleP *vqap)
{
	vqap->Config.AudioHandler((VQAHandle *)vqap, VQAAUDIO_STOP, NULL, NULL);
	vqap->Audio.Flags &= ~VQAAUDF_ISPLAYING;
}

// FUNCTION: GAMEMD 0x007BD2D0
long CopyAudio(VQAHandleP *vqap)
{
	VQAAudio  *audio;
	VQAConfig *config;
	VQALoader *loader;

	unsigned long startblock;
	unsigned long endblock;
	unsigned long len1,len2;
	unsigned long i;

	unsigned char *tempbuf;
	unsigned long tempbuflen;

	audio = &vqap->Audio;
	config = &vqap->Config;
	loader = &vqap->Loader;

	#if(VQAVOC_ON && VQAAUDIO_ON)
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0) || (vqap->vocfh != -1)
			|| (audio->Buffer == NULL) || (audio->TempBufLen == 0)) {
	#else
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0) || (audio->Buffer == NULL)
			|| (audio->TempBufLen == 0)) {
	#endif

		return(VQAERR_NONE);
	}

	tempbuf = audio->TempBuf + audio->BufferOffset;
	tempbuflen = audio->TempBufLen - audio->BufferOffset;

	startblock = (audio->AudBufPos / config->HMIBufSize);
	endblock = (audio->AudBufPos + tempbuflen) / config->HMIBufSize;

	if (endblock >= audio->NumAudBlocks) {
		endblock -= audio->NumAudBlocks;
	}

	if (audio->IsLoaded[endblock] == 1) {
		loader->WaitsOnAudio++;
		return(VQAERR_SLEEPING);
	}

	if (startblock <= endblock) {
		memcpy((audio->Buffer + audio->AudBufPos), tempbuf,
				tempbuflen);

		audio->AudBufPos += tempbuflen;

		audio->TempBufLen = 0;

		audio->BufferOffset = 0;

		for (i = startblock; i < endblock; i++) {
			audio->IsLoaded[i] = 1;
		}

	} else {
		len1 = config->AudioBufSize - audio->AudBufPos;

		len2 = tempbuflen - len1;

		memcpy((audio->Buffer + audio->AudBufPos), tempbuf, len1);

		memcpy(audio->Buffer, tempbuf + len1, len2);

		audio->AudBufPos = len2;

		audio->TempBufLen = 0;

		audio->BufferOffset = 0;

		for (i = startblock; i < audio->NumAudBlocks; i++) {
			audio->IsLoaded[i] = 1;
		}

		for (i = 0; i < endblock; i++) {
			audio->IsLoaded[i] = 1;
		}
	}

	return(VQAERR_NONE);
}

// FUNCTION: GAMEMD 0x007BD470
long __cdecl VQA_AudioFillCallback(VQAHandleP *vqap)
{
	VQAAudio *audio;
	VQAConfig *config;

	audio = &vqap->Audio;
	config = &vqap->Config;

	long size = config->HMIBufSize;

	if (audio->Flags & VQAAUDF_ISDONE) {
		return(0);
	}

	long pos = audio->BufferPosition;
	unsigned long block = audio->Block1;

	if (config->OptionFlags & VQAOPTF_WAITFILL) {
		long nblock = block + 1;
		if ((unsigned)nblock >= audio->NumAudBlocks) {
			nblock = 0;
		}
		if (!(audio->Flags & VQAAUDF_ISENDOFFILE)) {
			if (audio->IsLoaded[nblock] == 0) {
				audio->Flags |= VQAAUDF_ISSTARVED;
			}
		}
	}

	bool repeating = false;
	if (audio->IsLoaded[block] == 1) {
		if (audio->Block2 == -1) {
			audio->Block2 = block;
			audio->PlayPosition = pos;
		}
		audio->BlockRepeats[block] = 0;
		block++;
		long npos = pos + size;
		if (npos >= config->AudioBufSize) {
			npos = 0;
			block = 0;
		}
		audio->BufferPosition = npos;
		audio->Block1 = block;
	} else {
		if (audio->Flags & VQAAUDF_ISENDOFFILE) {
			audio->Flags |= VQAAUDF_ISDONE;

			if ((unsigned)pos < audio->AudBufPos) {
				size = audio->AudBufPos - pos;
			} else {
				size = 0;
			}

		} else {
			audio->Flags |= VQAAUDF_ISREPEATING;
			repeating = true;

			vqap->RepeatedBuffers++;

			pos -= size;

			block--;

			if (pos < 0) {
				pos = config->AudioBufSize - size;
				block = audio->NumAudBlocks - 1;
			}

			audio->BlockRepeats[block]++;
		}
	}

	if (size > 0) {
		if (repeating == true) {
			config->AudioHandler((VQAHandle *)vqap, VQAAUDIO_LOAD, audio->HMIBuffer, size);
		} else {
			config->AudioHandler((VQAHandle *)vqap, VQAAUDIO_LOAD, audio->Buffer + pos, size);
		}
	}
	return(size);
}

// FUNCTION: GAMEMD 0x007BD5C0
long __cdecl VQA_AudioDoneCallback(VQAHandleP *vqap, unsigned long buffer)
{
	VQAConfig *config;
	VQAAudio *audio;
	unsigned long  block;

	audio = &vqap->Audio;
	config = &vqap->Config;

	if ((void *)buffer == audio->Buffer + audio->PlayPosition || (void *)buffer == audio->HMIBuffer) {
		block = audio->Block2;

		if (block != -1) {
			if (!audio->BlockRepeats[block]) {
				audio->IsLoaded[block] = 0;

				block++;

				if (block >= audio->NumAudBlocks) {
					block = 0;
				}

				if (audio->IsLoaded[block] == 1) {
					audio->PlayPosition += config->HMIBufSize;

					if (audio->PlayPosition >= (unsigned)config->AudioBufSize) {
						audio->PlayPosition = 0;
					}
					audio->Block2 = block;
				} else {
					audio->Block2 = -1;
				}

			} else {
				audio->BlockRepeats[block]--;
			}
			return(VQAERR_NONE);
		}
		return(VQAERR_BADBLOCK);
	}
	return(VQAERR_AUDSYNC);
}
