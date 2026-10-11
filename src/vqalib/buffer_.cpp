// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include	"vqaplayp.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"vqamem.h"

extern long VQA_Set_DrawBuffer(VQAHandle *vqa, unsigned char *buffer, unsigned long width, unsigned long height, long xpos, long ypos);

_STATIC void VQA_BufferPerpareLoop(VQAHandleP *vqap, long * needs_start_cb, long * needs_end_cb);

void VQA_BufferPerpareLoop(VQAHandleP *vqap, long * needs_start_cb, long * needs_end_cb)
{
	VQALoopInfo::DATA *ldata;
	VQACodebookInfo::DATA *cdata;
	int groupsize;
	int lcount;
	int ccount;
	int lindex;
	int cindex;

	lcount = vqap->LoopInfo.Header.Count;
	ldata = vqap->LoopInfo.Data;

	groupsize = vqap->Header.Groupsize;
	if (groupsize > 0) {
		*needs_start_cb = 0;
		*needs_end_cb = 0;

		for (lindex = 0; lindex < lcount; lindex++) {
			if ((ldata[lindex].StartFrame % groupsize)) {
				*needs_start_cb = 1;
				break;
			}
		}

		for (lindex = 0; lindex < lcount; lindex++) {
			if ((ldata[lindex].EndFrame % groupsize)) {
				*needs_end_cb = 1;
				break;
			}
		}
	} else {
		*needs_start_cb = 1;
		*needs_end_cb = 1;
		ccount = vqap->CodebookInfo.Header.Count;
		cdata = vqap->CodebookInfo.Data;

		for (lindex = 0; lindex < lcount; lindex++) {
			if (*needs_start_cb) {
				for (cindex = 0; cindex < ccount; cindex++) {
					if (ldata[lindex].StartFrame == cdata[cindex].Frame) {
						*needs_start_cb = 0;
						break;
					}
				}
			}

			if (*needs_end_cb) {
				for (cindex = 0; cindex < ccount; cindex++) {
					if (ldata[lindex].EndFrame == cdata[cindex].Frame) {
						*needs_end_cb = 0;
						break;
					}
				}
			}

			if (!*needs_start_cb && !*needs_end_cb) {
				break;
			}
		}
	}
}

// FUNCTION: 0x007BD690
STATIC long AllocBuffers(VQAHandleP *vqap)
{
	long needs_start_cb;
	long needs_end_cb;
	VQACBNode    *cbnode;
	VQACBNode    *this_cb;
	VQAFrameNode *framenode;
	VQAFrameNode *this_frame;
	long         i;
	VQAConfig    *config;
	VQAHeader    *header;

	config = &vqap->Config;
	header = &vqap->Header;

	if (config->NumFrameBufs == 0) {
		return(VQAERR_NOMEM);
	}

	vqap->CBBufferSize = ALIGNUP(header->CBentries * header->BlockWidth * header->BlockHeight + 1, 4);

	if (header->ColorMode == 1 || header->ColorMode == 4) {
		vqap->CBBufferSize *= 2;
	}

	if (header->MaxCBSize > vqap->CBBufferSize) {
		return(VQAERR_OPEN);
	}

	if (header->MaxCBSize == 0) {
		header->MaxCBSize = vqap->CBBufferSize;
	}

	if (config->NumCBBufs <= 0) {
		long groupsize;
		if (vqap->CodebookInfo.Header.Count > 0) {
			groupsize = vqap->CodebookInfo.Header.Groupsize;
		} else {
			groupsize = header->Groupsize;
		}
		if (groupsize == vqap->NumFrames) {
			config->NumCBBufs = 1;
		} else {
			int newcbcount = 0;
			if (config->NumFrameBufs > 1) {
				newcbcount = (config->NumFrameBufs - 2) / groupsize + 1;
				if (config->OptionFlags & VQAOPTF_LOOPS) {
					if (vqap->LoopInfo.Header.Count == 0) {
						newcbcount++;
						newcbcount++;
					} else {
						VQA_BufferPerpareLoop(vqap, &needs_start_cb, &needs_end_cb);
						if (needs_start_cb != 0) {
							newcbcount++;
						}
						if (needs_end_cb != 0) {
							newcbcount++;
						}
					}
				}
			}
			config->NumCBBufs = newcbcount + 2;
		}
	}

	vqap->AltBufferFlags |= VQAABUFF_ALTCB;
	vqap->Max_CB_Size = ALIGNUP(header->MaxCBSize + 13, 4);

	vqap->AltCBBuffer = (void *)config->MemoryHandler((VQAHandle *)vqap, VQAMEM_ALLOC, NULL, vqap->CBBufferSize);

	if (vqap->AltCBBuffer == NULL) {
		FreeBuffers(vqap);
		return(VQAERR_NOMEM);
	}

	config->MemoryHandler((VQAHandle *)vqap, VQAMEM_LOCK, vqap->AltCBBuffer, vqap->CBBufferSize);
	vqap->MemUsed += vqap->CBBufferSize;

	cbnode = NULL;
	this_cb = NULL;

	for (i = 0; i < config->NumCBBufs; i++) {
		cbnode = (VQACBNode *)config->MemoryHandler((VQAHandle*)vqap, VQAMEM_ALLOC, NULL, (sizeof(VQACBNode) + vqap->Max_CB_Size));

		if (cbnode == NULL) {
			FreeBuffers(vqap);
			return(VQAERR_NOMEM);
		}

		config->MemoryHandler((VQAHandle*)vqap, VQAMEM_LOCK, cbnode, (sizeof(VQACBNode) + vqap->Max_CB_Size));

		vqap->MemUsed += (long)(sizeof(VQACBNode) + vqap->Max_CB_Size);

		memset(cbnode, 0, sizeof(VQACBNode));
		cbnode->Buffer = (unsigned char *)cbnode + sizeof(VQACBNode);

		if (this_cb == NULL) {
			vqap->CBData = cbnode;
			this_cb = cbnode;
		} else {
			this_cb->Next = cbnode;
			cbnode->Prev = this_cb;
			this_cb = cbnode;
		}
	}

	if (cbnode != NULL)
	{
		cbnode->Next = vqap->CBData;
		vqap->CBData->Prev = cbnode;
	}

	vqap->Loader.CurCB = vqap->CBData;
	vqap->Loader.FullCB = vqap->CBData;

	vqap->Max_CB_Size -= 4;

	vqap->Max_Pal_Size = ALIGNUP(1024 - 4, 4);

	vqap->PtrBufferSize = ALIGNUP((header->ImageWidth / header->BlockWidth)
			* (header->ImageHeight / header->BlockHeight)
			* sizeof(short) + 1, 4);

	{
		unsigned long maxptr;

		if (header->MaxFramesize == 0) {
			maxptr = vqap->PtrBufferSize;
		} else {
			maxptr = header->MaxFramesize;
			if (header->Version > 2) {
				maxptr <<= 8;
			}
		}

		vqap->AltBufferFlags |= VQAABUFF_ALTPTR;
		vqap->Max_Ptr_Size = ALIGNUP(maxptr + 5, 4);
	}

	vqap->AltPtrBuffer = (void *)config->MemoryHandler((VQAHandle *)vqap, VQAMEM_ALLOC, NULL, vqap->PtrBufferSize);

	if (vqap->AltPtrBuffer == NULL) {
		FreeBuffers(vqap);
		return(VQAERR_NOMEM);
	}

	config->MemoryHandler((VQAHandle *)vqap, VQAMEM_LOCK, vqap->AltPtrBuffer, vqap->PtrBufferSize);
	vqap->MemUsed += vqap->PtrBufferSize;

	framenode = NULL;
	this_frame = NULL;

	for (i = 0; i < config->NumFrameBufs; i++) {
		framenode = (VQAFrameNode *)config->MemoryHandler((VQAHandle*)vqap, VQAMEM_ALLOC, NULL, (sizeof(VQAFrameNode)
				+ vqap->Max_Ptr_Size + vqap->Max_Pal_Size));

		if (framenode == NULL) {
			FreeBuffers(vqap);
			return(VQAERR_NOMEM);
		}

		config->MemoryHandler((VQAHandle*)vqap, VQAMEM_LOCK, framenode, sizeof(VQAFrameNode) + vqap->Max_Ptr_Size
				+ vqap->Max_Pal_Size);

		vqap->MemUsed += (long)(sizeof(VQAFrameNode) + vqap->Max_Ptr_Size
				+ vqap->Max_Pal_Size);

		memset(framenode, 0, sizeof(VQAFrameNode));
		framenode->Pointers = (unsigned char *)framenode + sizeof(VQAFrameNode);
		framenode->Palette = (unsigned char *)framenode + sizeof(VQAFrameNode)
				+ vqap->Max_Ptr_Size;

		framenode->Codebook = NULL;

		if (this_frame == NULL) {
			vqap->FrameData = framenode;
			this_frame = framenode;
		} else {
			this_frame->Next = framenode;
			framenode->Prev = this_frame;
			this_frame = framenode;
		}
	}

	if (framenode != NULL)
	{
		framenode->Next = vqap->FrameData;
		vqap->FrameData->Prev = framenode;
	}

	vqap->Loader.CurFrame = vqap->FrameData;
	vqap->Drawer.CurFrame = vqap->FrameData;
	vqap->Flipper.CurFrame = vqap->FrameData;

	vqap->Max_Pal_Size -= 4;
	vqap->Max_Ptr_Size -= 4;

	#if(VQAAUDIO_ON)
	if ((header->Flags & VQAHDF_AUDIO)
			&& (config->OptionFlags & VQAOPTF_AUDIO)) {
		VQAAudio *audio = &vqap->Audio;

		if (header->Version < VQAHD_VER2) {
			vqap->SampleRate = 22050U;
			vqap->Channels = 1;
			vqap->BitsPerSample = 8;
			audio->BytesPerSec = 22050U;
		} else {
			if ((config->OptionFlags & VQAOPTF_ALTAUDIO)
					&& (header->Flags & VQAHDF_ALTAUDIO)) {
				vqap->SampleRate = header->AltSampleRate;
				vqap->Channels = header->AltChannels;
				vqap->BitsPerSample = header->AltBitsPerSample;
			} else {
				vqap->SampleRate = header->SampleRate;
				vqap->Channels = header->Channels;
				vqap->BitsPerSample = header->BitsPerSample;
			}

			audio->BytesPerSec = ((vqap->SampleRate * vqap->Channels)
					* (vqap->BitsPerSample >> 3));
		}

		#if(0)
		config->HMIBufSize *= (audio->SampleRate / 22050);
		config->HMIBufSize *= audio->Channels * (audio->BitsPerSample >> 3);
		#endif

		if (config->AudioBufSize == -1) {
			i = ((audio->BytesPerSec+(audio->BytesPerSec/2))/config->HMIBufSize);
			config->AudioBufSize = (config->HMIBufSize * i);
		}

		if (config->AudioBufSize > 0) {
			if (config->AudioBuf == NULL) {
				audio->Buffer = (unsigned char *)config->MemoryHandler((VQAHandle*)vqap, VQAMEM_ALLOC, NULL, config->AudioBufSize);

				if (audio->Buffer == NULL) {
					FreeBuffers(vqap);
					return(VQAERR_NOMEM);
				}

				config->MemoryHandler((VQAHandle*)vqap, VQAMEM_LOCK, audio->Buffer, config->AudioBufSize);

				vqap->MemUsed += config->AudioBufSize;
			} else {
				audio->Buffer = config->AudioBuf;
			}

			audio->NumAudBlocks = (config->AudioBufSize / config->HMIBufSize);
			audio->IsLoaded = (bool *)config->MemoryHandler((VQAHandle*)vqap, VQAMEM_ALLOC, NULL, audio->NumAudBlocks * sizeof(*audio->IsLoaded));

			if (audio->IsLoaded == NULL) {
				FreeBuffers(vqap);
				return(VQAERR_NOMEM);
			}

			config->MemoryHandler((VQAHandle*)vqap, VQAMEM_LOCK, audio->IsLoaded, audio->NumAudBlocks * sizeof(*audio->IsLoaded));

			vqap->MemUsed += (audio->NumAudBlocks * sizeof(*audio->IsLoaded));

			memset(audio->IsLoaded, 0, audio->NumAudBlocks * sizeof(*audio->IsLoaded));

			audio->BlockRepeats = (short *)config->MemoryHandler((VQAHandle*)vqap, VQAMEM_ALLOC, NULL, audio->NumAudBlocks * sizeof(*audio->BlockRepeats));

			if (audio->BlockRepeats == NULL) {
				FreeBuffers(vqap);
				return(VQAERR_NOMEM);
			}

			config->MemoryHandler((VQAHandle*)vqap, VQAMEM_LOCK, audio->BlockRepeats, audio->NumAudBlocks * sizeof(*audio->BlockRepeats));

			vqap->MemUsed += (audio->NumAudBlocks * sizeof(*audio->BlockRepeats));

			memset(audio->BlockRepeats, 0, audio->NumAudBlocks * sizeof(*audio->BlockRepeats));

			audio->TempBufSize = ((audio->BytesPerSec / header->FPS) * 2) + 100;
			audio->TempBuf = (unsigned char *)config->MemoryHandler((VQAHandle*)vqap, VQAMEM_ALLOC, NULL, audio->TempBufSize);

			if (audio->TempBuf == NULL) {
				FreeBuffers(vqap);
				return(VQAERR_NOMEM);
			}

			config->MemoryHandler((VQAHandle*)vqap, VQAMEM_LOCK, audio->TempBuf, audio->TempBufSize);

			vqap->MemUsed += audio->TempBufSize;

			audio->HMIBuffer = (unsigned char *)config->MemoryHandler((VQAHandle*)vqap, VQAMEM_ALLOC, NULL, config->HMIBufSize);

			if (audio->HMIBuffer == NULL) {
				FreeBuffers(vqap);
				return(VQAERR_NOMEM);
			}

			config->MemoryHandler((VQAHandle*)vqap, VQAMEM_LOCK, audio->HMIBuffer, config->HMIBufSize);

			memset(audio->HMIBuffer, 0, config->HMIBufSize);

			vqap->MemUsed += config->HMIBufSize;

		} else {
			FreeBuffers(vqap);
			return(VQAERR_NOAUDSIZ);
		}
	}
	#endif

	vqap->Foff = (long *)config->MemoryHandler((VQAHandle *)vqap, VQAMEM_ALLOC, NULL, header->Frames * sizeof(*vqap->Foff));

	if (vqap->Foff == NULL) {
		FreeBuffers(vqap);
		return(VQAERR_NOMEM);
	}

	config->MemoryHandler((VQAHandle *)vqap, VQAMEM_LOCK, vqap->Foff, header->Frames * sizeof(*vqap->Foff));

	vqap->MemUsed += (header->Frames * sizeof(long));

	return(VQAERR_NONE);
}

// FUNCTION: 0x007BE4A0
long VQA_Configure_Buffer(VQAHandleP *vqap)
{
	unsigned char *buf;
	int w;
	int h;
	VQAConfig *config;
	VQAHeader *header;

	header = &vqap->Header;
	config = &vqap->Config;

	if (config->ImageWidth == -1 || config->ImageHeight == -1) {
		w = header->ImageWidth;
		h = header->ImageHeight;
		if (config->DrawFlags & VQACFGF_OFFSET) {
			w += header->Xpos;
			h += header->Ypos;
		}
	} else {
		w = config->ImageWidth;
		h = config->ImageHeight;
	}

	if (config->DrawFlags & VQACFGF_BUFFER) {
		if (config->ImageBuf == NULL) {
			int size = w * h;
			if (header->ColorMode == 1 || header->ColorMode == 4) {
				size *= 2;
			}
			buf = (unsigned char *)config->MemoryHandler(((VQAHandle *)vqap), VQAMEM_ALLOC, NULL, size);
			if (buf == NULL) {
				return(VQAERR_NOMEM);
			}
			vqap->MemUsed += size;

			config->MemoryHandler(((VQAHandle *)vqap), VQAMEM_LOCK, buf, size);
			memset(buf, 0, size);

			vqap->AltImageBuf = buf;
			vqap->AltImageWidth = w;
			vqap->AltImageHeight = h;
			vqap->Flags |= VQADATF_ALTIMG;
		} else {
			buf = config->ImageBuf;
		}
	} else {
		if (config->ImageBuf != NULL) {
			buf = config->ImageBuf;
		} else {
			return(VQAERR_NOBUFFER);
		}
	}

	VQA_Set_DrawBuffer((VQAHandle *)vqap, buf, w, h, config->X1, config->Y1);
	vqap->Flags |= VQADATF_BUFCONFIG;
	return(VQAERR_NONE);
}

// FUNCTION: 0x007BE5D0
long __cdecl VQA_Memory_Handler(VQAHandle *vqa, long action, void *buffer, long nbytes)
{
	long error = 0;

	switch (action) {
		default:
			break;

		case VQAMEM_1:
			break;

		case VQAMEM_ALLOC:
			error = (long)malloc(nbytes);
			break;

		case VQAMEM_FREE:
			free(buffer);
			break;

		case VQAMEM_LOCK:
			error = (long)buffer;
			break;

		case VQAMEM_UNLOCK:
			error = (long)buffer;
			break;

		case VQAMEM_QUERYSIZE:
			*((long *)buffer) = -1;
			break;
	}
	return(error);
}
