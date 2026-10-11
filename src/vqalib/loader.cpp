// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "cmp.h"
#include <stdio.h>
#include <memory.h>
#include "vqaplayp.h"

long Load_FINF(VQAHandleP *vqap, unsigned long iffsize);

long Load_CINF(VQAHandleP *vqap);

long Load_PINF(VQAHandleP *vqap);

long Load_LINF(VQAHandleP *vqap);

long Load_CLIP(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_VQF(VQAHandleP *vqap, unsigned long frame_iffsize, char flags);

_STATIC long Load_CBF0(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_CBFZ(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_CBP0(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_CBPZ(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_CPL0(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_CPLZ(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_VPT0(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_VPTZ(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_SND0(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_SND1(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_SND2(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_SN2J(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_LINH(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_LIND(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_CINH(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_CIND(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_PINH(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long Load_PIND(VQAHandleP *vqap, unsigned long iffsize);

_STATIC long VQA_GetCodebookSize(VQAHandleP *vqap, long framenum);

_STATIC long VQA_MSCIReadData(VQAHandleP *vqap, unsigned long iffsize, long index);

_STATIC long VQA_MFCIIndexFromChunkID(VQAHandleP *vqap, unsigned long chunkid);

_STATIC long VQA_MFCIReadData(VQAHandleP *vqap, unsigned long iffsize, long index);

_STATIC long VQA_MSCIIndexFromChunkID(VQAHandleP *vqap, unsigned long chunkid);

_STATIC long VQA_GetPaletteFrameRange(VQAHandleP *vqap, long framenum, long * first_frame, long * last_frame);

_STATIC long VQA_SeekLoop(VQAHandleP *vqap, long framenum, long flags);

_STATIC long VQA_GetCodebookFrameRange(VQAHandleP *vqap, long framenum, long *first_frame, long *last_frame);

VQABool VQA_IsFrameStartOfLoop(VQAHandleP *vqap, long framenum);

long VQA_ReloadPalette(VQAHandleP *vqap, long framenum, int force);

_STATIC long VQA_LoadLoop(VQAHandleP *vqap, long framenum);

long __cdecl Memory_VQA_Stream_Handler(VQAHandle *vqa, long action, void *buffer, long nbytes);

_STATIC long VQA_LoadFrame_Internal(VQAHandleP *vqap, long flags);

long VQA_SeekGroup(VQAHandleP *vqap, long framenum, long groupsize, VQABool preloadaudio, VQABool reset_state, VQABool &skipcodebook);

long VQA_LoadFrame(VQAHandleP *vqap, long flags)
{
	VQAConfig *config;
	VQALoader *loader;
	VQALoopCache *cache;

	int frame;
	int loop_bytes;
	int scan_frame;
	int tocache;
	VQA_H_FUNC handler;
	unsigned int rc;
	int val4;
	int fsize;
	VQA_H_FUNC oldhandler;
	bool restore_handler;

	oldhandler = NULL;
	long foffset = 0;

	config = &vqap->Config;
	loader = &vqap->Loader;
	long *foff = vqap->Foff;
	frame = loader->CurFrameNum;

	restore_handler = false;

	if ((config->OptionFlags & VQAOPTF_LOOPS) && (vqap->AltBufferFlags & VQAABUFF_ALTLOOP) && vqap->LoopID >= 0) {
		if (frame < vqap->NumFrames) {
			foffset = VQAFRAME_OFFSET(foff[frame]);
		}

		cache = &vqap->LoopCache;
		if (frame == vqap->LoopStartFrame0 && vqap->LoopID != cache->ID) {
			if (frame != cache->Min) {
				cache->Buffer = (char *)foffset;
				cache->Bytes = 0;
				cache->Offset = 0;
			}
			cache->ID = vqap->LoopID;
			cache->Min = vqap->LoopStartFrame0;
			cache->Max = -1;
			loop_bytes = 0;

			scan_frame = vqap->LoopStartFrame0;
			for (;scan_frame <= vqap->LoopEndFrameMode2; scan_frame++) {
				if (scan_frame < vqap->NumFrames - 1) {
					loop_bytes += VQAFRAME_OFFSET(foff[scan_frame+1]) - VQAFRAME_OFFSET(foff[scan_frame]);
				} else {
					if (config->StreamHandler((VQAHandle *)vqap, VQACMD_SIZE, &fsize, 0)) {
						cache->Max = scan_frame - 1;
						break;
					}
					loop_bytes += fsize - VQAFRAME_OFFSET(foff[vqap->LoopEndFrameMode2]);
				}
				if (loop_bytes >= cache->Size) {
					cache->Max = scan_frame - 1;
					break;
				}
			}

			if (loop_bytes < cache->Size && cache->Max == -1) {
				cache->Max = vqap->LoopEndFrameMode2;
			}
		}
		if ( frame >= cache->Min && frame <= cache->Max )
		{
			if (foffset == (long)cache->Buffer + cache->Bytes) {
				if (frame < vqap->NumFrames - 1) {
					tocache = VQAFRAME_OFFSET(foff[frame + 1]) - foffset;
				} else {
					if (!config->StreamHandler((VQAHandle *)vqap, VQACMD_SIZE, &fsize, 0) ) {
						tocache = fsize - foffset;
					} else {
						tocache = 0;
					}
				}

				if (tocache > 0) {
					if (config->StreamHandler != Memory_VQA_Stream_Handler) {
						config->StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, NULL, foffset);
					}
					if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, &cache->Ptr[cache->Offset], tocache)) {
						return -4;
					}
					cache->Bytes += tocache;
				}
			}

			if ( foffset < (long)cache->Buffer + cache->Bytes )
			{
				restore_handler = true;
				cache->Offset = foffset - (long)cache->Buffer;
				oldhandler = config->StreamHandler;
				config->StreamHandler = Memory_VQA_Stream_Handler;
			}
		}
	}

	if (VQA_IsFrameStartOfLoop(vqap, frame)) {
		if ((flags & VQALOADF_NOLFR) == 0) {
			vqap->Loader.FullCB = vqap->Loader.CurCB;
		}
	}

	rc = VQA_LoadFrame_Internal(vqap, flags);

	if (restore_handler == true) {
		config->StreamHandler = oldhandler;
	}

	return rc;
}

long VQA_LoadFrame_Internal(VQAHandleP *vqap, long flags)
{
	VQAConfig     *config;
	VQALoader     *loader;
	VQADrawer     *drawer;
	VQAFrameNode  *curframe;
	ChunkHeader   *chunk;
	static unsigned long iffsize;

	VQABool      frame_loaded = 0;
	VQABool      loop_loaded = 0;

	long          rc;

	config = &vqap->Config;
	loader = &vqap->Loader;
	drawer = &vqap->Drawer;
	curframe = loader->CurFrame;
	chunk = &loader->CurChunkHdr;

	if (config->OptionFlags & VQAOPTF_LOOPS) {
		if (vqap->LoopIterations > 0 || vqap->LoopIterations == -1) {
			if (loader->CurFrameNum > vqap->LoopEndFrameMode2) {
				long endjump = vqap->LoopEndFrameJump;
				long end2 = vqap->LoopEndFrameMode2;

				flags = vqap->StopFrame;

				if (endjump != -1) {
					vqap->LoopEndFrameMode2 = endjump;
					vqap->LoopEndFrameJump = -1;
					vqap->StopFrame = endjump;
				}

				if (loader->CurFrameNum != vqap->LoopStartFrame0) {
					loader->CurFrameNum = vqap->LoopStartFrame0;
					rc = VQA_LoadLoop(vqap, vqap->LoopStartFrame0);
				} else {
					rc = VQAERR_NONE;
				}

				if (rc == VQAERR_NONE || rc == VQAERR_SLEEPING) {
					vqap->Flags |= VQADATF_LOOPED;

					if (endjump == -1) {
						curframe->Flags |= VQAFRMF_LOOPED;
						if (vqap->LoopIterations != -1) {
							vqap->LoopIterations--;
						}
					} else {
						curframe->Flags |= VQAFRMF_LOOPJMP;
						vqap->LoopEndFrame2 = end2;
						vqap->LoopIterations = vqap->LoopIterationsJump;
						vqap->LoopIterationsJump = -1;
						vqap->Flags |= VQADATF_LOOPJMP;
					}

				} else {
					vqap->LoopEndFrameMode2 = end2;
					vqap->LoopEndFrameJump = endjump;
					vqap->StopFrame = flags;
					loader->CurFrameNum = vqap->LoopEndFrameMode2 + 1;
				}
				return(rc);
			}
		}
	}

	if (loader->CurFrameNum > vqap->StopFrame) {
		if (!(flags & VQALOADF_NOSND) && (config->OptionFlags & VQAOPTF_AUDIO) && !(config->OptionFlags & VQAOPTF_ALTAUDIO)) {
			if (CopyAudio(vqap) == VQAERR_SLEEPING) {
				vqap->Flags |= VQADATF_LSLEEP;
				return(VQAERR_SLEEPING);
			}
			vqap->Flags &= (~VQADATF_LSLEEP);
			vqap->Audio.Flags |= VQAAUDF_ISENDOFFILE;
		}
		return(VQAERR_EOF);
	}

	VQABool skip;

	if ((curframe->Flags & VQAFRMF_LOADED) && !(curframe->Flags & VQAFRMF_11)) {
		loader->WaitsOnDrawer++;
		return(VQAERR_NOBUFFER);
	}

	if (!(vqap->Flags & VQADATF_LSLEEP)) {
		loader->FrameSize = 0;

		curframe->Codebook = loader->FullCB;
	}

	while (frame_loaded == 0) {
		if (!(vqap->Flags & VQADATF_LSLEEP)) {
			if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, chunk, 8)) {
				return(VQAERR_EOF);
			}

			iffsize = REVERSE_LONG(chunk->size);
			loader->FrameSize += iffsize;
		}

		skip = true;

		switch (chunk->id) {
			case ID_VQFR:
				if ((flags & (VQALOADF_NOPAL|VQALOADF_NOPTR|VQALOADF_NOPCB|VQALOADF_NOFCB)) != (VQALOADF_NOPAL|VQALOADF_NOPTR|VQALOADF_NOPCB|VQALOADF_NOFCB)) {
					if (Load_VQF(vqap, iffsize, flags) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
				}
				frame_loaded = true;
				break;

			case ID_VQFK:
				if ((flags & (VQALOADF_NOPAL|VQALOADF_NOPTR|VQALOADF_NOPCB|VQALOADF_NOFCB)) != (VQALOADF_NOPAL|VQALOADF_NOPTR|VQALOADF_NOPCB|VQALOADF_NOFCB)) {
					if (Load_VQF(vqap, iffsize, flags) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					curframe->Flags |= VQAFRMF_KEY;
					skip = false;
				}
				frame_loaded = true;
				break;

			case ID_VQFL:
				if ((flags & VQALOADF_NOLFR) == 0) {
					if (Load_VQF(vqap, iffsize, flags) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
					loop_loaded = true;
					if ((vqap->Header.Flags & VQAHDF_4) && flags & VQALOADF_NOPTR) {
						frame_loaded = true;
					}
				}
				break;

			case ID_CBF0:
				if ((flags & VQALOADF_NOFCB) == 0) {
					if (Load_CBF0(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
				}
				break;

			case ID_CBFZ:
				if ((flags & VQALOADF_NOFCB) == 0) {
					if (Load_CBFZ(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
				}
				break;

			case ID_CBP0:
				if ((flags & VQALOADF_NOPCB) == 0) {
					if (Load_CBP0(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
				}
				break;

			case ID_CBPZ:
				if ((flags & VQALOADF_NOPCB) == 0) {
					if (Load_CBPZ(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
				}
				break;

			case ID_CPL0:
				if ((flags & VQALOADF_NOPAL) == 0) {
					if (Load_CPL0(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}

					curframe->Flags |= VQAFRMF_PALETTE;
					skip = false;
				}
				break;

			case ID_CPLZ:
				if ((flags & VQALOADF_NOPAL) == 0) {
					if (Load_CPLZ(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;

					curframe->Flags |= VQAFRMF_PALETTE;
				}
				break;

			case ID_VPT0:
			case ID_VPTD:
				if ((flags & VQALOADF_NOPTR) == 0) {
					if (Load_VPT0(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
				}
				frame_loaded = true;
				break;

			case ID_VPKZ:
				if ((flags & VQALOADF_NOPTR) == 0) {
					if (Load_VPTZ(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;

					curframe->Flags |= VQAFRMF_KEY;
				}
				frame_loaded = true;
				break;

			case ID_VPTK:
				if ((flags & VQALOADF_NOPTR) == 0) {
					if (Load_VPT0(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;

					curframe->Flags |= VQAFRMF_KEY;
				}
				frame_loaded = true;
				break;

			case ID_VPTR:
				if ((flags & VQALOADF_NOPTR) == 0) {
					if (Load_VPT0(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;

					curframe->Flags |= VQAFRMF_RSDCOMP;
				}
				frame_loaded = true;
				break;

			case ID_VPRZ:
				if ((flags & VQALOADF_NOPTR) == 0) {
					if (Load_VPTZ(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;

					curframe->Flags |= VQAFRMF_RSDCOMP;
				}
				frame_loaded = true;
				break;

			case ID_VPTZ:
			case ID_VPDZ:
				if ((flags & VQALOADF_NOPTR) == 0) {
					if (Load_VPTZ(vqap, iffsize) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
				}
				frame_loaded = true;
				break;

			#if(VQAAUDIO_ON)
			case ID_SND0:
				if (!(flags & VQALOADF_NOSND)) {
					if ((config->OptionFlags & VQAOPTF_AUDIO) && !(config->OptionFlags & VQAOPTF_ALTAUDIO)) {
						if (CopyAudio(vqap) == VQAERR_SLEEPING) {
							vqap->Flags |= VQADATF_LSLEEP;
							return(VQAERR_SLEEPING);
						} else {
							vqap->Flags &= (~VQADATF_LSLEEP);
						}

						if (Load_SND0(vqap, iffsize) != VQAERR_NONE) {
							return(VQAERR_READ);
						}
						skip = false;
					}
				}
				break;

			case ID_SNA0:
				if (!(flags & VQALOADF_NOSND)) {
					if ((config->OptionFlags & VQAOPTF_AUDIO) && (config->OptionFlags & VQAOPTF_ALTAUDIO)) {
						if (CopyAudio(vqap) == VQAERR_SLEEPING) {
							vqap->Flags |= VQADATF_LSLEEP;
							return(VQAERR_SLEEPING);
						} else {
							vqap->Flags &= (~VQADATF_LSLEEP);
						}

						if (Load_SND0(vqap, iffsize) != VQAERR_NONE) {
							return(VQAERR_READ);
						}
						skip = false;
					}
				}
				break;

			case ID_SND1:
				if (!(flags & VQALOADF_NOSND)) {
					if ((config->OptionFlags & VQAOPTF_AUDIO) && !(config->OptionFlags & VQAOPTF_ALTAUDIO)) {
						if (CopyAudio(vqap) == VQAERR_SLEEPING) {
							vqap->Flags |= VQADATF_LSLEEP;
							return(VQAERR_SLEEPING);
						} else {
							vqap->Flags &= (~VQADATF_LSLEEP);
						}

						if (Load_SND1(vqap, iffsize) != VQAERR_NONE) {
							return(VQAERR_READ);
						}
						skip = false;
					}
				}
				break;

			case ID_SNA1:
				if ((flags & VQALOADF_NOSND) == 0) {
					if ((config->OptionFlags & VQAOPTF_AUDIO) && (config->OptionFlags & VQAOPTF_ALTAUDIO)) {
						if (CopyAudio(vqap) == VQAERR_SLEEPING) {
							vqap->Flags |= VQADATF_LSLEEP;
							return(VQAERR_SLEEPING);
						} else {
							vqap->Flags &= (~VQADATF_LSLEEP);
						}

						if (Load_SND1(vqap, iffsize) != VQAERR_NONE) {
							return(VQAERR_READ);
						}
						skip = false;
					}
				}
				break;

			case ID_SND2:
				if (!(flags & VQALOADF_NOSND)) {
					if ((config->OptionFlags & VQAOPTF_AUDIO) && !(config->OptionFlags & VQAOPTF_ALTAUDIO)) {
						if (CopyAudio(vqap) == VQAERR_SLEEPING) {
							vqap->Flags |= VQADATF_LSLEEP;
							return(VQAERR_SLEEPING);
						} else {
							vqap->Flags &= (~VQADATF_LSLEEP);
						}

						if (Load_SND2(vqap, iffsize) != VQAERR_NONE) {
							return(VQAERR_READ);
						}
						skip = false;
					}
				}
				break;

			case ID_SNA2:
				if (!(flags & VQALOADF_NOSND)) {
					if ((config->OptionFlags & VQAOPTF_AUDIO) && (config->OptionFlags & VQAOPTF_ALTAUDIO)) {
						if (CopyAudio(vqap) == VQAERR_SLEEPING) {
							vqap->Flags |= VQADATF_LSLEEP;
							return(VQAERR_SLEEPING);
						} else {
							vqap->Flags &= (~VQADATF_LSLEEP);
						}

						if (Load_SND2(vqap, iffsize) != VQAERR_NONE) {
							return(VQAERR_READ);
						}
						skip = false;
					}
				}
				break;

			case ID_SN2J:
				if (!(flags & VQALOADF_NOSND)) {
					if ((config->OptionFlags & VQAOPTF_AUDIO) && !(config->OptionFlags & VQAOPTF_ALTAUDIO)) {
						if (Load_SN2J(vqap, iffsize) != VQAERR_NONE) {
							return(VQAERR_READ);
						}
						skip = false;
					}
				}
				break;
			#endif

			default: {
				long index = VQA_MFCIIndexFromChunkID(vqap, chunk->id);
				if (index >= 0) {
					if (VQA_MFCIReadData(vqap, iffsize, index) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
					break;
				}

				index = VQA_MSCIIndexFromChunkID(vqap, chunk->id);
				if (index >= 0) {
					if (VQA_MSCIReadData(vqap, iffsize, index) != VQAERR_NONE) {
						return(VQAERR_READ);
					}
					skip = false;
					break;
				}

				break;
			}
			}

		if (skip == true) {
			if (config->StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, (void *)SEEK_CUR, PADSIZE(iffsize))) {
				return(VQAERR_SEEK);
			}
		}
	}

	if ((loader->CurFrameNum > 0) && (loader->FrameSize > loader->MaxFrameSize)) {
		loader->MaxFrameSize = loader->FrameSize;
	}

	if (VQA_IsFrameStartOfLoop(vqap, loader->CurFrameNum)
			&& !(flags & VQALOADF_NOLFR) && !loop_loaded) {
		loader->FullCB = loader->CurCB->Prev;
		curframe->Codebook = loader->FullCB;
	}

	if (!(flags & VQALOADF_NOPTR)) {
		curframe->FrameNum = loader->CurFrameNum;

		loader->LastFrameNum = loader->CurFrameNum;

		loader->PrevCB = curframe->Codebook;

		curframe->Flags |= VQAFRMF_LOADED;
		loader->CurFrame = curframe->Next;
	}

	loader->CurFrameNum++;
	return(VQAERR_NONE);
}

long VQA_MFCIIndexFromChunkID(VQAHandleP *vqap, unsigned long chunkid)
{
	VQAMFCInfo::TABLE *table = vqap->MFCInfo.Table;
	unsigned long count = vqap->MFCInfo.Header.Count;
	for (unsigned long i = 0; i < count; i++) {
		if (chunkid == table[i].ChunkID) {
			return(i);
		}
	}
	return(-1);
}

long VQA_MFCIReadData(VQAHandleP *vqap, unsigned long iffsize, long index)
{
	void *buf;
	VQALoader *loader;
	VQAMFCInfo::DATA2 *data2;
	VQAConfig *config;
	unsigned long padsize;
	unsigned long write_index;

	loader = &vqap->Loader;
	config = &vqap->Config;

	padsize = PADSIZE(iffsize);

	data2 = &vqap->MFCInfo.Data2[index];

	write_index = data2->WriteIndex;

	buf = data2->Data[write_index].Buffer;
	data2->Data[write_index].Size = iffsize;
	data2->Data[write_index].Frame = loader->CurFrameNum;
	write_index++;
	if (write_index >= data2->Count) {
		write_index = 0;
	}
	data2->WriteIndex = write_index;

	if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, buf, padsize)) {
		return(VQAERR_READ);
	}

	loader->CurFrame->Flags |= VQAFRMF_CHUNKS;

	return(VQAERR_NONE);
}

long VQA_MSCIIndexFromChunkID(VQAHandleP *vqap, unsigned long chunkid)
{
	VQAMSCInfo *info = &vqap->MSCInfo;
	VQAMSCInfo::HEADER *infohdr = &info->Header;
	VQAMSCInfo::TABLE *table = info->Table;
	unsigned long count = infohdr->Count;
	for (unsigned long i = 0; i < count; i++) {
		if (chunkid == table[i].ChunkID) {
			return(i);
		}
	}
	return(-1);
}

long VQA_MSCIReadData(VQAHandleP *vqap, unsigned long iffsize, long index)
{
	VQAConfig *config;
	VQALoader *loader;
	unsigned long padsize;

	VQAMSCInfo::DATA2 *data2;
	unsigned long write_index;
	void *buf;

	VQAMSCInfo *info = &vqap->MSCInfo;

	loader = &vqap->Loader;
	config = &vqap->Config;

	padsize = PADSIZE(iffsize);

	data2 = &info->Data2[index];

	write_index = data2->WriteIndex;

	buf = data2->Data[write_index].Buffer;
	data2->Data[write_index].Size = iffsize;
	data2->Data[write_index].Frame = loader->CurFrameNum;
	write_index++;
	if (write_index >= data2->Count) {
		write_index = 0;
	}
	data2->WriteIndex = write_index;

	if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, buf, padsize)) {
		return(VQAERR_READ);
	}

	loader->CurFrame->Flags |= VQAFRMF_CHUNKS;

	return(VQAERR_NONE);
}

long VQA_SeekLoop(VQAHandleP *vqap, long framenum, long flags)
{
	long rc = VQAERR_NONE;
	VQAConfig *config;
	VQALoopCache *cache;
	bool needs_seek = false;
	long *foff;

	cache = &vqap->LoopCache;
	foff = vqap->Foff;

	if ((vqap->AltBufferFlags & VQAABUFF_ALTLOOP) && framenum == cache->Min && cache->Bytes != 0) {
		if ((long)(unsigned char *)cache->Buffer + cache->Bytes <= (long)(unsigned char *)VQAFRAME_OFFSET(foff[vqap->LoopEndFrameMode2])) {
			needs_seek = true;
		}
		cache->Offset = 0;
	} else if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, 0, VQAFRAME_OFFSET(foff[framenum])) != 0) {
		return(VQAERR_SEEK);
	}

	if (framenum > 0 && (!(flags & VQALOADF_NOFCB) || !(vqap->Header.Flags & VQAHDF_4))) {
		rc = VQA_LoadFrame(vqap, (flags | VQALOADF_NOSND | VQALOADF_NOPTR));
	}

	if (rc == VQAERR_NONE) {
		if (needs_seek && vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_SEEKPEEK, 0, long((int)cache->Buffer + cache->Bytes)) != 0) {
			return(VQAERR_SEEK);
		}
	}

	return(rc);
}

_STATIC long VQA_LoadLoop(VQAHandleP *vqap, long framenum)
{
	VQALoader *loader;
	VQAHeader *header;
	VQAConfig *config;

	long rc = VQAERR_NONE;
	VQABool skipcodebook = false;

	loader = &vqap->Loader;
	header = &vqap->Header;
	config = &vqap->Config;

	if (loader->CurFrame->Flags & VQAFRMF_LOADED) {
		return(VQAERR_NOBUFFER);
	}

	if (framenum >= vqap->StopFrame || vqap->Foff == NULL) {
		return(VQAERR_SEEK);
	}

	if ((config->OptionFlags & VQAOPTF_AUDIO) && !(vqap->Header.Flags & VQAHDF_SNDJUMP)) {
		return(VQAERR_SEEK);
	}

	if (header->ColorMode == 0 && !VQA_IsFrameStartOfLoop(vqap, framenum)) {
		rc = VQA_ReloadPalette(vqap, framenum, 1);
	}

	if (rc == VQAERR_NONE) {
		rc = VQA_SeekGroup(vqap, framenum, header->Groupsize, false, false, skipcodebook);
	}

	loader->CurFrameNum = framenum;
	if (rc == VQAERR_NONE) {
		long flags = VQALOADF_NOLFR;
		if (skipcodebook) {
			flags |= VQALOADF_NOFCB;
		}

		rc = VQA_LoadFrame(vqap, flags);
	}
	return(rc);
}

long Load_VQF(VQAHandleP *vqap, unsigned long frame_iffsize, char flags)
{
	VQAFrameNode  *curframe;
	ChunkHeader   *chunk;
	unsigned long iffsize;
	unsigned long framesize;
	unsigned long bytes_loaded = 0;
	VQADrawer     *drawer;
	VQAConfig     *config;
	int skip;

	curframe = vqap->Loader.CurFrame;
	drawer = &(vqap->Drawer);
	chunk = &vqap->Loader.CurChunkHdr;
	config = &(vqap->Config);
	framesize = PADSIZE(frame_iffsize);

	while (bytes_loaded < framesize) {
		if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, chunk, sizeof(*chunk))) {
			return(VQAERR_EOF);
		}

		iffsize = REVERSE_LONG(chunk->size);
		bytes_loaded += sizeof(*chunk);
		bytes_loaded += PADSIZE(iffsize);

		skip = true;

		switch (chunk->id) {
			case ID_CBF0:
				if (flags & VQALOADF_NOFCB) {
					break;
				}
				if (Load_CBF0(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				break;

			case ID_CBFZ:
				if (flags & VQALOADF_NOFCB) {
					break;
				}
				if (Load_CBFZ(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				break;

			case ID_CBP0:
				if (flags & VQALOADF_NOPCB) {
					break;
				}
				if (Load_CBP0(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				break;

			case ID_CBPZ:
				if (flags & VQALOADF_NOPCB) {
					break;
				}
				if (Load_CBPZ(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				break;

			case ID_CPL0:
				if (flags & VQALOADF_NOPAL) {
					break;
				}
				if (Load_CPL0(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				#if 0
				if (drawer->CurPalSize == 0) {
					memcpy(drawer->Palette_24,curframe->Palette,curframe->PaletteSize);
					drawer->CurPalSize = curframe->PaletteSize;
				}
				#endif

				curframe->Flags |= VQAFRMF_PALETTE;

				skip = false;
				break;

			case ID_CPLZ:
				if (flags & VQALOADF_NOPAL) {
					break;
				}
				if (Load_CPLZ(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				#if 0
				if (drawer->CurPalSize == 0) {
					drawer->CurPalSize = VQA_LCW_Uncompress((char *)curframe->Palette
							+ curframe->PalOffset, (char *)drawer->Palette_24,
							vqap->Max_Pal_Size);
				}
				#endif

				curframe->Flags |= VQAFRMF_PALETTE;

				skip = false;
				break;

			case ID_VPT0:
			case ID_VPTD:
				if (flags & VQALOADF_NOPTR) {
					break;
				}
				if (Load_VPT0(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				break;

			case ID_VPTZ:
			case ID_VPDZ:
				if (flags & VQALOADF_NOPTR) {
					break;
				}
				if (Load_VPTZ(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				break;

			case ID_VPTK:
				if (flags & VQALOADF_NOPTR) {
					break;
				}
				if (Load_VPT0(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				curframe->Flags |= VQAFRMF_KEY;

				break;

			case ID_VPTR:
				if (flags & VQALOADF_NOPTR) {
					break;
				}
				if (Load_VPT0(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				curframe->Flags |= VQAFRMF_RSDCOMP;

				break;

			case ID_VPKZ:
				if (flags & VQALOADF_NOPTR) {
					break;
				}
				if (Load_VPTZ(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				curframe->Flags |= VQAFRMF_KEY;

				break;

			case ID_VPRZ:
				if (flags & VQALOADF_NOPTR) {
					break;
				}
				if (Load_VPTZ(vqap, iffsize) != VQAERR_NONE) {
					return(VQAERR_READ);
				}

				skip = false;
				curframe->Flags |= VQAFRMF_RSDCOMP;

				break;

			default:
				return(VQAERR_READ);
		}

		if (skip == true) {
			if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, (void *)1, PADSIZE(iffsize))) {
				return(VQAERR_SEEK);
			}
		}
	}

	return(VQAERR_NONE);
}

long Load_LINH(VQAHandleP *vqap, unsigned long iffsize)
{
	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, &vqap->LoopInfo.Header, PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}
	return(VQAERR_NONE);
}

long Load_LIND(VQAHandleP *vqap, unsigned long iffsize)
{
	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, vqap->LoopInfo.Data, PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}
	return(VQAERR_NONE);
}

long VQA_GetCodebookSize(VQAHandleP *vqap, long framenum)
{
	VQACodebookInfo *info = &vqap->CodebookInfo;
	VQACodebookInfo::DATA *data = info->Data;
	int count = info->Header.Count;
	bool found = false;
	int findex;

	for (int i = 0; i < count; i++) {
		if (data[i].Frame > framenum) {
			findex = i;
			found = true;
			break;
		}
	}

	if (found == false) {
		return(0);
	}
	return(data[findex].Size);
}

long Load_CINH(VQAHandleP *vqap, unsigned long iffsize)
{
	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, &vqap->CodebookInfo.Header, PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}
	return(VQAERR_NONE);
}

long Load_CIND(VQAHandleP *vqap, unsigned long iffsize)
{
	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, vqap->CodebookInfo.Data, PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}
	return(VQAERR_NONE);
}

long Load_PINH(VQAHandleP *vqap, unsigned long iffsize)
{
	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, &vqap->PaletteInfo.Header, PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}
	return(VQAERR_NONE);
}

long Load_PIND(VQAHandleP *vqap, unsigned long iffsize)
{
	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, vqap->PaletteInfo.Data, PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}
	return(VQAERR_NONE);
}

long Load_CBF0(VQAHandleP *vqap, unsigned long iffsize)
{
	VQALoader *loader;
	VQACBNode *curcb;

	loader = &vqap->Loader;
	curcb = loader->CurCB;

	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, curcb->Buffer,
			PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}

	loader->NumPartialCB = 0;

	curcb->Flags = 0;
	curcb->CBOffset = 0;
	curcb->CodebookSize = iffsize;
	curcb->Flags |= VQACBF_CBFULL;

	loader->FullCB = curcb;
	loader->CurCB = curcb->Next;

	return(VQAERR_NONE);
}

long Load_CBPZ(VQAHandleP *vqap, unsigned long iffsize)
{
	VQAConfig     *config;
	VQALoader     *loader;
	VQACBNode     *curcb;
	void          *buffer;
	unsigned long padsize;
	VQAHeader     *header;
	long          groupsize;
	int cbfull;

	padsize = PADSIZE(iffsize);
	config = &vqap->Config;
	loader = &vqap->Loader;
	header = &vqap->Header;
	groupsize = header->Groupsize;
	curcb = loader->CurCB;

	if (loader->PartialCBSize == 0) {
		if (groupsize != 0) {
			loader->CBSize = PADSIZE(groupsize) + groupsize * padsize;
		} else {
			loader->CBSize = VQA_GetCodebookSize(vqap, loader->CurFrameNum);
		}
		if (!(vqap->AltBufferFlags & VQAABUFF_ALTCB)) {
			curcb->CBOffset = vqap->Max_CB_Size - loader->CBSize;
		} else {
			curcb->CBOffset = 0;
		}
	}

	buffer = ((curcb->Buffer + curcb->CBOffset) + loader->PartialCBSize);

	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, buffer, padsize)) {
		return(VQAERR_READ);
	}

	loader->PartialCBSize += iffsize;
	loader->NumPartialCB++;
	cbfull = false;

	if (groupsize != 0) {
		if (loader->NumPartialCB == groupsize) {
			cbfull = true;
		}
	} else {
		if (loader->PartialCBSize == loader->CBSize) {
			cbfull = true;
		} else {
			if (loader->PartialCBSize > loader->CBSize) {
				return(VQAERR_READ);
			}
		}
	}

	if (cbfull) {
		loader->NumPartialCB = 0;
		loader->PartialCBSize = 0;

		curcb->Flags = 0;

		curcb->Flags |= VQACBF_CBFULL|VQACBF_CBCOMP;

		loader->FullCB = curcb;
		loader->CurCB = curcb->Next;

		if (header->Version < VQAHD_VER3) {
			curcb->Buffer[loader->CBSize + curcb->CBOffset + 1] = 0x80;
		}
	}

	return(VQAERR_NONE);
}

long Load_CPL0(VQAHandleP *vqap, unsigned long iffsize)
{
	VQADrawer *drawer;
	VQAFrameNode *curframe;

	drawer = &vqap->Drawer;
	curframe = vqap->Loader.CurFrame;

	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, curframe->Palette,
			PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}

	curframe->Flags &= ~VQAFRMF_PALCOMP;
	curframe->PalOffset = 0;
	curframe->PaletteSize = iffsize;

	if (vqap->PaletteInfo.Header.Count == 1) {
		memcpy(drawer->Palette_24, curframe->Palette, curframe->PaletteSize);
		drawer->CurPalSize = curframe->PaletteSize;
		drawer->Flags |= VQADRWF_SETPAL;
	}

	return(VQAERR_NONE);
}

long Load_VPT0(VQAHandleP *vqap, unsigned long iffsize)
{
	VQAFrameNode *curframe;

	curframe = vqap->Loader.CurFrame;

	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, curframe->Pointers,
			PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}

	curframe->Flags &= ~VQAFRMF_PTRCOMP;
	curframe->PtrOffset = 0;

	return(VQAERR_NONE);
}

long Load_VPTZ(VQAHandleP *vqap, unsigned long iffsize)
{
	VQAFrameNode  *curframe;
	void          *buffer;
	unsigned long padsize;
	unsigned long lcwoffset;

	curframe = vqap->Loader.CurFrame;
	padsize = PADSIZE(iffsize);
	lcwoffset = vqap->Max_Ptr_Size - padsize;

	buffer = curframe->Pointers + lcwoffset;

	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, buffer, padsize)) {
		return(VQAERR_READ);
	}

	curframe->Flags |= VQAFRMF_PTRCOMP;
	curframe->PtrOffset = lcwoffset;

	if (vqap->Header.Version < VQAHD_VER3) {
		((unsigned char *)buffer)[iffsize + 1] = 0x80;
	}

	return(VQAERR_NONE);
}

long Load_SN2J(VQAHandleP *vqap, unsigned long iffsize)
{
	unsigned long padsize;
	VQAConfig     *config;
	VQAAudio      *audio;

	audio = &vqap->Audio;
	config = &vqap->Config;

	padsize = PADSIZE(iffsize);

	#pragma pack(push,1)
	struct SNJ2Struct {
		unsigned short wIndex;
		unsigned int dwPredicted;
		unsigned short wIndex2;
		unsigned int dwPredicted2;
	} data;
	#pragma pack(pop)

	#if(VQAVOC_ON && VQAAUDIO_ON)
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0)
			|| (vqap->vocfh != -1) || (audio->Buffer == NULL)) {
	#else
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0)
			|| (audio->Buffer == NULL)) {
	#endif

		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, (void *)SEEK_CUR,
				padsize)) {
			return(VQAERR_SEEK);
		}
		return(VQAERR_NONE);
	}

	if (padsize <= sizeof(data)) {
		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, &data,
			padsize)) {
			return(VQAERR_READ);
		}
	} else {
		return(VQAERR_READ);
	}

	audio->ADPCM_Info.dwPredicted = data.dwPredicted;
	audio->ADPCM_Info.wIndex = data.wIndex;
	if (vqap->Channels == 2) {
		audio->ADPCM_Info.dwPredicted2 = data.dwPredicted2;
		audio->ADPCM_Info.wIndex2 = data.wIndex2;
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C0490
long PrimeBuffers(VQAHandle *vqa)
{
	VQAHandleP *vqap;
	VQAConfig *config;
	long rc;
	long i;

	vqap = ((VQAHandleP *)vqa);
	config = &((VQAHandleP *)vqa)->Config;

	for (i = 0; i < config->NumFrameBufs; i++) {
		if ((rc = VQA_LoadFrame(((VQAHandleP *)vqa), VQALOADF_NOLFR)) == VQAERR_NONE) {
			((VQAHandleP *)vqa)->LoadedFrames++;
		}
		else if ((rc != VQAERR_NOBUFFER) && (rc != VQAERR_SLEEPING)) {
			return(rc);
		}
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C04E0
VQABool VQA_IsFrameStartOfLoop(VQAHandleP *vqap, long framenum)
{
	int i;
	int count;
	VQALoopInfo::DATA *data;

	if (framenum == 0) {
		return(true);
	}

	count = vqap->LoopInfo.Header.Count;

	if (count > 0 && ((vqap->LoopInfo.Header.Flags & VQALOOPF_DATAVALID) || (vqap->LoopInfo.Header.Flags & VQALOOPF_2))) {
		data = vqap->LoopInfo.Data;
		for (i = 0; i < count; i++) {
			if (framenum == data[i].StartFrame) {
				return(true);
			}
		}
	}

	return(false);
}

// FUNCTION: 0x007C0530
long VQA_ReloadPalette(VQAHandleP *vqap, long framenum, int force)
{
	VQALoader *loader;
	VQAFrameNode *curframe;
	VQAConfig *config;

	long rc = -1;

	loader = &vqap->Loader;
	curframe = loader->CurFrame;
	config = &vqap->Config;

	long pal_frame;
	long last_pal_frame;
	int i;

	if ( framenum > vqap->StopFrame || vqap->Foff == NULL) {
		return VQAERR_SEEK;
	}

	if ( vqap->PaletteInfo.Data != 0 )
	{
		VQA_GetPaletteFrameRange(vqap, framenum, &pal_frame, NULL);
		if (!force) {
			VQA_GetPaletteFrameRange(vqap, loader->LastFrameNum, &last_pal_frame, 0);
			if (pal_frame == last_pal_frame) {
				pal_frame = -1;
			}
		}
	} else {
		pal_frame = -1;
		i = framenum;
		for (; i >= 0; i--) {
			if (vqap->Foff[i] & VQAFINF_PAL) {
				pal_frame = i;
				break;
			}
		}

		if (!force) {
			last_pal_frame = -1;
			i = loader->LastFrameNum;
			for (; i >= 0; i--) {
				if (vqap->Foff[i] & VQAFINF_PAL) {
					last_pal_frame = i;
					break;
				}
			}

			if (pal_frame == last_pal_frame) {
				pal_frame = -1;
			}
			}
	}

	if (pal_frame >= 0 ) {
		if (!config->StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, (void *)SEEK_SET, VQAFRAME_OFFSET(vqap->Foff[pal_frame]))) {
			if (force) {
				loader->NumPartialCB = 0;
				loader->PartialCBSize = 0;
				loader->FullCB = vqap->CBData;
				loader->CurCB = vqap->CBData;
				loader->CurFrameNum = 0;
				curframe->Flags = 0;
			}
			rc = VQA_LoadFrame(vqap, VQALOADF_NOSND | VQALOADF_NOPTR | VQALOADF_NOPCB | VQALOADF_NOFCB);
		} else {
			rc = VQAERR_SEEK;
		}
	}
	return rc;
}

// FUNCTION: 0x007C06B0
long VQA_GetPaletteFrameRange(VQAHandleP *vqap, long framenum, long * first_frame, long * last_frame)
{
	VQAPaletteInfo::DATA *data = vqap->PaletteInfo.Data;
	int count = vqap->PaletteInfo.Header.Count;
	unsigned int idx1;
	unsigned int idx2;
	long cur_frame;
	long prev_frame;
	int findex = 0;
	bool found = false;

	for (int i = 1; i < count; i++) {
		cur_frame = data[i].Frame;
		prev_frame = data[i - 1].Frame;
		if (framenum >= prev_frame) {
			if (framenum < cur_frame) {
				found = true;
				break;
			}
		}
		findex++;
	}

	if (!found) {
		findex = count - 1;
	}

	if (first_frame != NULL) {
		*first_frame = data[findex].Frame;
	}

	if (last_frame != NULL) {
		if (findex < count - 1) {
			*last_frame = data[findex + 1].Frame - 1;
		} else {
			*last_frame = vqap->NumFrames;
		}
	}
	return(findex);
}

// FUNCTION: 0x007C0740
long VQA_SeekGroup(VQAHandleP *vqap, long framenum, long groupsize, VQABool preloadaudio, VQABool reset_state, VQABool &skipcodebook)
{
	VQALoader    *loader;
	VQADrawer    *drawer;
	VQAHeader    *header;
	VQAFrameNode *frame;
	VQACBNode    *curcb;
	VQAConfig    *config;
	long         group;
	long         i;

	#if(VQAAUDIO_ON)
	VQAAudio     *audio;;
	#endif

	int audpreload;
	int bytes_per_frame;
	int preload_frames;
	int covered_bytes;
	unsigned int rc;
	bool bool2;
	int loadflags;
	long group_end;
	long group_start;
#define VQA_GROUP_START group_start
#define VQA_GROUP_END group_end
	int group_index;
	int last_group_index;
	VQABool cb_valid;
	long audio_start;

	loader = &vqap->Loader;
	drawer = &vqap->Drawer;
	header = &vqap->Header;
	config = &vqap->Config;

	#if(VQAAUDIO_ON)
	audio = &vqap->Audio;
	#endif

	group_index = 0;

	frame = drawer->CurFrame;
	if (vqap->Foff) {
		group = framenum;
		if (config->NumCBBufs > 1) {
			cb_valid = 0;
			if (groupsize > 0) {
				group_index = framenum / groupsize;
				VQA_GROUP_START = group_index * groupsize;

				if (VQA_GROUP_START >= groupsize) {
					VQA_GROUP_START -= groupsize;
				}
				if (group_index == 0) {
					VQA_GROUP_END = -1;
				} else {
					VQA_GROUP_END = VQA_GROUP_START + groupsize - 1;
				}
				last_group_index = loader->LastFrameNum / groupsize;
			} else {
				group_index = VQA_GetCodebookFrameRange(vqap, framenum, &VQA_GROUP_START, &VQA_GROUP_END);
				last_group_index = VQA_GetCodebookFrameRange(vqap, loader->LastFrameNum, 0, 0);
			}
			group = VQA_GROUP_START;

			if (!reset_state) {
				if (last_group_index == group_index) {
					group = VQA_GROUP_END + 1;
					loader->FullCB = loader->PrevCB;
					loader->CurCB = loader->FullCB->Next;
					cb_valid = 1;
				}
				else if (loader->NumPartialCB == 0) {
					bool dont_set_cur = false;
					curcb = loader->FullCB;
					while (true) {
						if (curcb == frame->Codebook && (frame->Flags & VQAFRMF_LOADED) != 0) {
							dont_set_cur = true;
							break;
						}
						frame = frame->Next;
						if (drawer->CurFrame == frame) {
							break;
						}
					}
					if (!dont_set_cur) {
						loader->CurCB = curcb;
					}
				}
			} else {
				loader->FullCB = vqap->CBData;
				loader->CurCB = vqap->CBData;
			}

			loader->NumPartialCB = 0;
			loader->PartialCBSize = 0;
		} else {
			cb_valid = 1;
			VQA_GROUP_START = framenum;
			VQA_GROUP_END = framenum;
		}

		skipcodebook = cb_valid;

		#if(VQAAUDIO_ON)
			if ((config->OptionFlags & VQAOPTF_AUDIO)
					&& (audio->Buffer != NULL)) {
			int bytes_per_sec = ((vqap->SampleRate * vqap->Channels) * (vqap->BitsPerSample >> 3));
			bytes_per_frame = bytes_per_sec / config->FrameRate;
			audpreload = header->AudioPreload;
			if ((header->Flags & VQAHDF_SNDJUMP) == 0 && audpreload == 0)
			{
				audpreload = (long)(bytes_per_sec / 2);
			}

			if ( preloadaudio && audpreload > 0 )
			{
				preload_frames = audpreload / bytes_per_frame;
				covered_bytes = bytes_per_frame * preload_frames;
				if ( audpreload > covered_bytes )
				{
					++preload_frames;
				}
				audio_start = framenum - preload_frames;
				if (audio_start < 0) {
					audio_start = 0;
					audio->BufferOffset = framenum * bytes_per_frame;
				}
				else
				{
					audio->BufferOffset = bytes_per_frame + covered_bytes - audpreload;
				}

				if ( audio_start < group )
				{
					group = audio_start;
				}
			}
			else
			{
				audio_start = framenum;
				audio->BufferOffset = 0;
			}

			if ( reset_state )
			{
				memset(audio->IsLoaded, 0, audio->NumAudBlocks * sizeof(*audio->IsLoaded));
				memset(audio->Buffer, 0, config->AudioBufSize);
				audio->TempBufLen = 0;

				if (audio_start == 0) {
					audio->AudBufPos = 0;
				} else {
					audio->AudBufPos = audpreload;
					int blocks = int(audio->AudBufPos / config->HMIBufSize);
					for (i = 0; i < blocks; i++) {
						audio->IsLoaded[i] = 1;
					}
				}
			}
		}
		else
		{
			preloadaudio = 0;
			audio_start = framenum;
			audio->BufferOffset = 0;
		}
		#endif

		if (VQA_IsFrameStartOfLoop(vqap, framenum)) {
			if (reset_state) {
				loader->CurFrame->Flags = 0;
			}

			if (cb_valid) {
				loadflags = VQALOADF_NOFCB;
			} else {
				loadflags = 0;
				loader->FullCB = loader->CurCB;
			}
			rc = VQA_SeekLoop(vqap, framenum, loadflags);
			if ( reset_state )
			{
				if (rc == VQAERR_NOBUFFER || rc == VQAERR_SLEEPING) {
					rc = -1;
				}
			}

			return(rc);
		}

		if (!config->StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, (void *)SEEK_SET, VQAFRAME_OFFSET(vqap->Foff[group]))) {
			loader->CurFrameNum = group;
			rc = -1;

			loadflags = (VQALOADF_NOPAL|VQALOADF_NOPTR);
			if (group_index) {
				loadflags |= VQALOADF_NOFCB;
			}
			if (!preloadaudio) {
				loadflags |= VQALOADF_NOSND;
			}

			long loadframe = loader->CurFrameNum;
			for (i = 0; i < (framenum - group); i++) {
				if (reset_state) {
					loader->CurFrame->Flags &= ~VQAFRMF_LOADED;
				}

				if (preloadaudio) {
					if (reset_state) {
						if (loadframe == audio_start) {
							audio->TempBufLen = 0;
						}
					}

					if (loadframe < audio_start) {
						loadflags |= VQALOADF_NOSND;
					} else {
						loadflags &= ~VQALOADF_NOSND;
					}
				}

				if (cb_valid && loadframe <= VQA_GROUP_END) {
					loadflags |= VQALOADF_NOPCB;
				} else {
					loadflags &= ~VQALOADF_NOPCB;
				}

				rc = VQA_LoadFrame(vqap, loadflags);
				if (rc != VQAERR_NONE) {
					if (!reset_state || ((rc != VQAERR_NOBUFFER) && (rc != VQAERR_SLEEPING))) {
						break;
					} else {
						rc = VQAERR_NONE;
					}
				}
				loadframe++;
			}
		} else {
			return(VQAERR_SEEK);
		}
		return(rc);
	}
	return(VQAERR_SEEK);
#undef VQA_GROUP_START
#undef VQA_GROUP_END
}

// FUNCTION: 0x007C1130
long Load_CLIP(VQAHandleP *vqap, unsigned long iffsize)
{
	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, &vqap->Clipper, PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}
	return(VQAERR_NONE);
}

// FUNCTION: 0x007C1150
long Load_LINF(VQAHandleP *vqap)
{
	unsigned long iffsize;
	ChunkHeader chunk;
	VQAConfig *config;

	config = &vqap->Config;

	if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, &chunk, sizeof(chunk))) {
		return(VQAERR_EOF);
	}

	if (chunk.id != ID_LINH) {
		return(VQAERR_READ);
	}

	iffsize = REVERSE_LONG(chunk.size);
	if (Load_LINH(vqap, iffsize) != VQAERR_NONE) {
		return(VQAERR_READ);
	}

	if (vqap->LoopInfo.Header.Count > 0) {
		long size = (unsigned short)vqap->LoopInfo.Header.Count * sizeof(VQALoopInfo::DATA);
		vqap->LoopInfo.Data = (VQALoopInfo::DATA *)config->MemoryHandler((VQAHandle *)vqap, VQAMEM_ALLOC, 0, size);
		if (vqap->LoopInfo.Data == NULL) {
			return(VQAERR_NOMEM);
		}

		config->MemoryHandler((VQAHandle *)vqap, VQAMEM_LOCK, vqap->LoopInfo.Data, size);
		vqap->MemUsed += size;

		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, &chunk, sizeof(chunk))) {
			return(VQAERR_EOF);
		}

		if (chunk.id != ID_LIND) {
			return(VQAERR_READ);
		}

		size = REVERSE_LONG(chunk.size);
		if (Load_LIND(vqap, size) != VQAERR_NONE) {
			return(VQAERR_READ);
		}
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C19F0
long VQA_GetCodebookFrameRange(VQAHandleP *vqap, long framenum, long *first_frame, long *last_frame)
{
	VQACodebookInfo::DATA *data = vqap->CodebookInfo.Data;
	int count = vqap->CodebookInfo.Header.Count;
	unsigned int start_index;
	unsigned int end_index;
	long cur_frame;
	long prev_frame;
	int findex = 0;
	bool found = false;

	for (int i = 1; i < count; i++) {
		cur_frame = data[i].Frame;
		prev_frame = data[i - 1].Frame;
		if (framenum >= prev_frame) {
			if (framenum < cur_frame) {
				found = true;
				break;
			}
		}
		findex++;
	}

	if (!found) {
		findex = count - 1;
	}

	if (findex == 0) {
		start_index = 0;
		end_index = 0;
	} else {
		start_index = findex - 1;
		end_index = findex;
	}

	if (first_frame != NULL) {
		*first_frame = data[start_index].Frame;
	}

	if (last_frame != NULL) {
		*last_frame = data[end_index].Frame - 1;
	}
	return(findex);
}

// FUNCTION: 0x007C1A70
long Load_CINF(VQAHandleP *vqap)
{
	int groupsize;
	long size;
	unsigned long iffsize;
	ChunkHeader chunk;
	VQAConfig *config;

	config = &vqap->Config;

	if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, &chunk, sizeof(chunk))) {
		return(VQAERR_EOF);
	}

	if (chunk.id != ID_CINH) {
		return(VQAERR_READ);
	}

	iffsize = REVERSE_LONG(chunk.size);
	if (Load_CINH(vqap, iffsize) != VQAERR_NONE) {
		return(VQAERR_READ);
	}

	groupsize = 0;
	groupsize = vqap->Header.Groupsize;
	if (groupsize == 0 && vqap->CodebookInfo.Header.Count > 0) {
		size = vqap->CodebookInfo.Header.Count * sizeof(VQACodebookInfo::DATA);
		vqap->CodebookInfo.Data = (VQACodebookInfo::DATA *)config->MemoryHandler((VQAHandle *)vqap, VQAMEM_ALLOC, 0, size);
		if (vqap->CodebookInfo.Data == NULL) {
			return(VQAERR_NOMEM);
		}

		config->MemoryHandler((VQAHandle *)vqap, VQAMEM_LOCK, vqap->CodebookInfo.Data, size);
		vqap->MemUsed += size;

		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, &chunk, sizeof(chunk))) {
			return(VQAERR_EOF);
		}

		if (chunk.id != ID_CIND) {
			return(VQAERR_READ);
		}

		iffsize = REVERSE_LONG(chunk.size);
		if (Load_CIND(vqap, iffsize) != VQAERR_NONE) {
			return(VQAERR_READ);
		}
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C1BE0
long Load_PINF(VQAHandleP *vqap)
{
	unsigned long iffsize;
	ChunkHeader chunk;
	VQAConfig *config;

	config = &vqap->Config;

	if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, &chunk, sizeof(chunk))) {
		return(VQAERR_EOF);
	}

	if (chunk.id != ID_PINH) {
		return(VQAERR_READ);
	}

	iffsize = REVERSE_LONG(chunk.size);
	if (Load_PINH(vqap, iffsize) != VQAERR_NONE) {
		return(VQAERR_READ);
	}

	if (vqap->PaletteInfo.Header.Count > 0) {
		long size = vqap->PaletteInfo.Header.Count * sizeof(VQAPaletteInfo::DATA);
		vqap->PaletteInfo.Data = (VQAPaletteInfo::DATA *)config->MemoryHandler((VQAHandle *)vqap, VQAMEM_ALLOC, 0, size);
		if (vqap->PaletteInfo.Data == NULL) {
			return(VQAERR_NOMEM);
		}

		config->MemoryHandler((VQAHandle *)vqap, VQAMEM_LOCK, vqap->PaletteInfo.Data, size);
		vqap->MemUsed += size;

		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, &chunk, sizeof(chunk))) {
			return(VQAERR_EOF);
		}

		if (chunk.id != ID_PIND) {
			return(VQAERR_READ);
		}

		iffsize = REVERSE_LONG(chunk.size);
		if (Load_PIND(vqap, iffsize) != VQAERR_NONE) {
			return(VQAERR_READ);
		}
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C1D40
long Load_FINF(VQAHandleP *vqap, unsigned long iffsize)
{
	if (vqap->Foff != NULL) {
		if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, vqap->Foff,
				PADSIZE(iffsize))) {
			return(VQAERR_READ);
		}
	} else {
		if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, (void *)SEEK_CUR,
				PADSIZE(iffsize))) {
			return(VQAERR_SEEK);
		}
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C1D90
long Load_CBFZ(VQAHandleP *vqap, unsigned long iffsize)
{
	VQALoader     *loader;
	VQACBNode     *curcb;
	void          *buffer;
	unsigned long padsize;
	unsigned long lcwoffset;

	loader = &vqap->Loader;
	curcb = loader->CurCB;
	padsize = PADSIZE(iffsize);

	lcwoffset = vqap->Max_CB_Size - padsize;
	buffer = curcb->Buffer + lcwoffset;

	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, buffer, padsize)) {
		return(VQAERR_READ);
	}

	loader->NumPartialCB = 0;

	curcb->Flags = 0;

	curcb->Flags |= VQACBF_CBFULL|VQACBF_CBCOMP;
	curcb->CBOffset = lcwoffset;

	loader->FullCB = curcb;
	loader->CurCB = curcb->Next;

	if (vqap->Header.Version < VQAHD_VER3) {
		((unsigned char *)buffer)[iffsize + 1] = 0x80;
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C1E10
long Load_CBP0(VQAHandleP *vqap, unsigned long iffsize)
{
	VQALoader *loader;
	VQACBNode *curcb;
	VQAHeader *header;
	void      *buffer;
	int cbfull;

	loader = &vqap->Loader;
	curcb = loader->CurCB;
	header = &vqap->Header;

	if (header->Groupsize == 0 && loader->PartialCBSize == 0) {
		loader->CBSize = VQA_GetCodebookSize(vqap, loader->CurFrameNum);
	}

	buffer = curcb->Buffer + loader->PartialCBSize;
	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, buffer,
			PADSIZE(iffsize))) {
		return(VQAERR_READ);
	}

	loader->PartialCBSize += iffsize;
	loader->NumPartialCB++;
	cbfull = false;

	if (header->Groupsize != 0) {
		if (loader->NumPartialCB == vqap->Header.Groupsize) {
			cbfull = true;
		}
	} else {
		if (loader->PartialCBSize == loader->CBSize) {
			cbfull = true;
		} else {
			if (loader->PartialCBSize > loader->CBSize) {
				return(VQAERR_READ);
			}
		}
	}

	if (cbfull) {
		curcb->CodebookSize = loader->PartialCBSize;

		loader->NumPartialCB = 0;
		loader->PartialCBSize = 0;

		curcb->Flags = 0;

		curcb->CBOffset = 0;

		curcb->Flags |= VQACBF_CBFULL;

		loader->FullCB = curcb;
		loader->CurCB = curcb->Next;
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C20B0
long Load_CPLZ(VQAHandleP *vqap, unsigned long iffsize)
{
	VQADrawer *drawer;
	VQAFrameNode  *curframe;
	void          *buffer;
	unsigned long padsize;
	unsigned long lcwoffset;

	curframe = vqap->Loader.CurFrame;
	padsize = PADSIZE(iffsize);
	drawer = &vqap->Drawer;

	lcwoffset = vqap->Max_Pal_Size - padsize;
	buffer = curframe->Palette + lcwoffset;

	if (vqap->Config.StreamHandler((VQAHandle *)vqap, VQACMD_READ, buffer, padsize)) {
		return(VQAERR_READ);
	}

	curframe->Flags |= VQAFRMF_PALCOMP;
	curframe->PalOffset = lcwoffset;
	curframe->PaletteSize = iffsize;

	if (vqap->Header.Version < VQAHD_VER3) {
		((unsigned char *)buffer)[iffsize + 1] = 0x80;
	}

	if (vqap->PaletteInfo.Header.Count == 1) {
		curframe->PaletteSize = VQA_LCW_Uncompress((char *)(curframe->Palette + curframe->PalOffset), (char *)curframe->Palette, vqap->Max_Pal_Size);
		curframe->Flags &= ~VQAFRMF_PALCOMP;
		memcpy(drawer->Palette_24, curframe->Palette, curframe->PaletteSize);
		drawer->CurPalSize = curframe->PaletteSize;
		drawer->Flags |= VQADRWF_SETPAL;
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C2180
long Load_SND0(VQAHandleP *vqap, unsigned long iffsize)
{
	VQAAudio      *audio;
	VQAConfig     *config;
	unsigned long padsize;
	unsigned long i;

	audio = &vqap->Audio;
	config = &vqap->Config;
	padsize = PADSIZE(iffsize);

	#if(VQAVOC_ON && VQAAUDIO_ON)
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0)
			|| (vqap->vocfh != -1) || (audio->Buffer == NULL)) {
	#else
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0)
			|| (audio->Buffer == NULL)) {
	#endif

		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, (void *)SEEK_CUR,
				padsize)) {
			return(VQAERR_SEEK);
		} else {
			return(VQAERR_NONE);
		}
	}

	if ((padsize > audio->TempBufSize) && (audio->AudBufPos == 0)) {
		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, audio->Buffer,
				padsize)) {
			return(VQAERR_READ);
		}

		audio->AudBufPos += iffsize;

		for (i = 0; i < (iffsize / config->HMIBufSize); i++) {
			audio->IsLoaded[i] = 1;
		}

		return(VQAERR_NONE);
	} else {
		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, audio->TempBuf,
				padsize)) {
			return(VQAERR_READ);
		}
	}

	audio->TempBufLen = iffsize;

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C2250
long Load_SND1(VQAHandleP *vqap, unsigned long iffsize)
{
	VQAAudio      *audio;
	VQAConfig     *config;
	unsigned char *loadbuf;
	unsigned long padsize;
	ZAPHeader     zap;
	long          i;

	audio = &vqap->Audio;
	config = &vqap->Config;
	padsize = PADSIZE(iffsize);

	#if(VQAVOC_ON && VQAAUDIO_ON)
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0)
			|| (vqap->vocfh != -1) || (audio->Buffer == NULL)) {
	#else
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0)
			|| (audio->Buffer == NULL)) {
	#endif

		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, (void *)SEEK_CUR,
				padsize)) {
			return(VQAERR_SEEK);
		} else {
			return(VQAERR_NONE);
		}
	}

	if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, &zap,
			sizeof(ZAPHeader))) {
		return(VQAERR_READ);
	}

	padsize -= sizeof(ZAPHeader);

	if ((zap.UnCompSize > audio->TempBufSize) && (audio->AudBufPos == 0)) {
		if (zap.UnCompSize == zap.CompSize) {
			if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, audio->Buffer,
					padsize)) {
				return(VQAERR_READ);
			}
		} else {
			loadbuf = (audio->Buffer + config->AudioBufSize) - padsize;

			if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, loadbuf,
					padsize)) {
				return(VQAERR_READ);
			}

			AudioUnzap(loadbuf, audio->Buffer, zap.UnCompSize);
		}

		audio->AudBufPos += zap.UnCompSize;

		for (i = 0; i < (zap.UnCompSize / config->HMIBufSize); i++) {
			audio->IsLoaded[i] = 1;
		}

		return(VQAERR_NONE);
	}

	if (zap.UnCompSize == zap.CompSize) {
		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, audio->TempBuf,
				padsize)) {
			return(VQAERR_READ);
		}
	} else {
		loadbuf = ((audio->TempBuf + audio->TempBufSize) - padsize);

		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, loadbuf, padsize)) {
			return(VQAERR_READ);
		}

		AudioUnzap(loadbuf, audio->TempBuf, zap.UnCompSize);
	}

	audio->TempBufLen = zap.UnCompSize;

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C2410
long Load_SND2(VQAHandleP *vqap, unsigned long iffsize)
{
	VQAAudio      *audio;
	VQAConfig     *config;
	unsigned char *loadbuf;
	unsigned long padsize;
	unsigned long uncomp_size;
	long          i;

	audio = &vqap->Audio;
	config = &vqap->Config;
	padsize = PADSIZE(iffsize);

	#if(VQAVOC_ON && VQAAUDIO_ON)
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0)
			|| (vqap->vocfh != -1) || (audio->Buffer == NULL)) {
	#else
	if (((config->OptionFlags & VQAOPTF_AUDIO) == 0)
			|| (audio->Buffer == NULL)) {
	#endif

		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_SEEK, (void *)SEEK_CUR,
				padsize)) {
			return(VQAERR_SEEK);
		} else {
			return(VQAERR_NONE);
		}
	}

	uncomp_size = iffsize * (vqap->BitsPerSample / 4);

	if ((uncomp_size > audio->TempBufSize) && (audio->AudBufPos == 0)) {
		loadbuf = (audio->Buffer + config->AudioBufSize) - padsize;

		if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, loadbuf, padsize)) {
			return(VQAERR_READ);
		}

		VQA_sosCODECDecompressData(loadbuf, (char *)audio->Buffer, vqap->BitsPerSample, vqap->Channels, uncomp_size, &audio->ADPCM_Info);

		if (audio->BufferOffset) {
			uncomp_size -= audio->BufferOffset;
			memcpy(audio->Buffer, audio->Buffer + audio->BufferOffset, uncomp_size);
			audio->BufferOffset = 0;
		}

		audio->AudBufPos += uncomp_size;

		for (i = 0; i < (uncomp_size / config->HMIBufSize); i++) {
			audio->IsLoaded[i] = true;
		}

		return(VQAERR_NONE);
	}

	loadbuf = ((audio->TempBuf + audio->TempBufSize) - padsize);

	if (config->StreamHandler((VQAHandle *)vqap, VQACMD_READ, loadbuf, padsize)) {
		return(VQAERR_READ);
	}

	VQA_sosCODECDecompressData(loadbuf, (char *)audio->TempBuf, vqap->BitsPerSample, vqap->Channels, uncomp_size, &audio->ADPCM_Info);

	audio->TempBufLen = uncomp_size;

	return(VQAERR_NONE);
}
