// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include	"vqaplayp.h"
#include	"unvq.h"
#include	"cmp.h"
#include	"vqapalette.h"
#include	<stdio.h>
#include	<string.h>
#include	"video.h"

_STATIC long Select_Frame(VQAHandleP *vqap);

_STATIC void Prepare_Frame(VQAHandleP *vqap);

long DrawFrame_MCGABuf(VQAHandle *vqa);

long PageFlip_MCGABuf(VQAHandle *vqa);

long DrawFrame_MCGA(VQAHandle *vqa);

long PageFlip_MCGA(VQAHandle *vqa);

void __cdecl UnVQ_Nop(unsigned char *codebook, unsigned char *pointers, unsigned char *buffer, unsigned long blocksperrow, unsigned long numrows, unsigned long bufwidth);

long PageFlip_Nop(VQAHandle *vqa);

void VQA_SetTimer(VQAHandleP *vqap, long time);

void VQA_StepTimer(VQAHandleP *vqap, long step);

unsigned long VQA_GetTime(VQAHandleP *vqap);

_STATIC long VQA_SetCurrentFrameAsLast(VQAHandleP *vqap);

_STATIC void VQA_SetPreviousFrameNode(VQAHandleP *vqap);

_STATIC long VQA_CalcFramesSinceDrawn(VQAHandleP *vqap);

_STATIC long VQA_ComputeDesiredFrame(VQAHandleP *vqap, VQAConfig *config, VQADrawer *drawer, VQAFrameNode *frame, unsigned long time);

void VQA_DispatchFrameChunks(VQAHandleP *vqap, long frame);

long VQA_CalcFramesSinceDrawn(VQAHandleP *vqap)
{
	VQAConfig * config;
	VQADrawer * drawer;
	VQAFrameNode *curframe;
	long cur;
	long loop_end;
	int to_loop_end;
	int from_loop_start;
	int fallback;

	drawer = &vqap->Drawer;
	curframe = drawer->CurFrame;
	config = &vqap->Config;

	cur = curframe->FrameNum;
	loop_end = vqap->LoopEndFrame2;

	if (cur >= drawer->LastFrameNum && cur <= loop_end) {
		return cur - drawer->LastFrameNum;
	}

	fallback = config->FrameRate / 3;

	to_loop_end = loop_end - drawer->LastFrameNum;
	if (to_loop_end < 0) {
		return(fallback);
	}

	from_loop_start = curframe->FrameNum - vqap->LoopStartFrame0;
	if (from_loop_start < 0) {
		return fallback;
	}
	return to_loop_end + from_loop_start + 1;
}

long VQA_SetCurrentFrameAsLast(VQAHandleP *vqap)
{
	VQADrawer * drawer;

	drawer = &vqap->Drawer;

	drawer->LastFrameNum = drawer->CurFrame->FrameNum;

	return(VQAERR_NONE);
}

void VQA_SetPreviousFrameNode(VQAHandleP *vqap)
{
	VQADrawer * drawer;

	drawer = &vqap->Drawer;

	drawer->CurFrame = drawer->CurFrame->Prev;
	drawer->CurFrame->Flags = drawer->CurFrame->PrevFlags;
}

// FUNCTION: GAMEMD 0x007BE660
long VQA_ResetLastFrameNum(VQAHandle *vqa)
{
	VQAHandleP *vqap;
	VQADrawer *drawer;

	vqap = (VQAHandleP *)vqa;
	drawer = &vqap->Drawer;

	drawer->LastFrameNum = -1;
	return(VQAERR_NONE);
}

// FUNCTION: GAMEMD 0x007BE670
long VQA_Configure_Drawer(VQAHandleP *vqap)
{
	VQAConfig *config;
	VQAHeader *header;
	VQADrawer *drawer;
	long      origin;
	long      blkdim;

	drawer = &vqap->Drawer;
	header = &vqap->Header;

	drawer->BlocksPerRow = header->ImageWidth / header->BlockWidth;
	drawer->NumRows = header->ImageHeight / header->BlockHeight;
	drawer->NumBlocks = drawer->BlocksPerRow * drawer->NumRows;
	blkdim = BLOCK_DIM(header->BlockWidth, header->BlockHeight);

	vqap->UnVQ1 = UnVQ_Nop;
	vqap->UnVQ2 = UnVQ_Nop;
	vqap->Page_Flip = PageFlip_Nop;

	origin = (vqap->Config.DrawFlags & VQACFGF_ORIGIN);

	if (!(vqap->Config.DrawFlags & VQACFGF_BUFFER)) {
		vqap->Draw_Frame = DrawFrame_MCGA;
		vqap->Page_Flip = PageFlip_MCGA;
	} else {
		vqap->Draw_Frame = DrawFrame_MCGABuf;
		vqap->Page_Flip = PageFlip_MCGABuf;
	}

	switch (blkdim) {
		case BLOCK_4X2:
			switch (header->ColorMode) {
				case 0:
					vqap->UnVQ1 = ASM_UnVQ_4x2;

					if (header->Flags & VQAHDF_TRANS) {
						if (vqap->Config.DrawFlags & VQACFGF_NOTRANS) {
							vqap->UnVQ2 = UnVQ2_C0_4x2_KEY;
						} else {
							vqap->UnVQ2 = UnVQ2_C0_4x2_TRANS;
						}
					}
					break;

				case 4:
					vqap->UnVQ1 = UnVQ1_C4_4x2;
					vqap->UnVQ2 = UnVQ2_C4_4x2;
					break;
			}
			break;

		case BLOCK_4X4:
			switch (header->ColorMode) {
				case 4:
					vqap->UnVQ1 = UnVQ1_C4_4x4;
					vqap->UnVQ2 = UnVQ2_C4_4x4;
					break;

				case 1:
					vqap->UnVQ1 = ASM_UnVQ1_C1_4x4;
					vqap->UnVQ2 = UnVQ2_C1_4x4;
					break;

				case 0:
					vqap->UnVQ1 = ASM_UnVQ_4x4;

					if (header->Flags & VQAHDF_TRANS) {
						if (vqap->Config.DrawFlags & VQACFGF_NOTRANS) {
							vqap->UnVQ2 = UnVQ2_C0_4x4_KEY;
						} else if (vqap->Config.DrawFlags & VQACFGF_HALFSIZE) {
							vqap->UnVQ2 = UnVQ2_C0_4x4_TRANS_HALF;
						} else {
							vqap->UnVQ2 = UnVQ2_C0_4x4_TRANS;
						}
					} else {
						if (vqap->Config.DrawFlags & VQACFGF_HALFSIZE) {
							vqap->UnVQ1 = ASM_UnVQ_4x4_HALF;
						}
					}
					break;
			}
			break;
	}

	return(VQAERR_NONE);
}

// FUNCTION: GAMEMD 0x007BE830
void VQA_DispatchFrameChunks(VQAHandleP *vqap, long frame)
{
	unsigned long chunkid;
	unsigned long count1;
	unsigned long count2;
	int i;
	int j;

	VQAMFCInfo *mfci_info = &vqap->MFCInfo;
	VQAMFCInfo::HEADER *mfci_infohdr = &mfci_info->Header;

	count1 = mfci_infohdr->Count;
	for (i = 0; i < count1; i++) {
		chunkid = mfci_info->Table[i].ChunkID;
		VQAMFCInfo::DATA2::DATA *mfci_data2data = mfci_info->Data2[i].Data;
		count2 = mfci_info->Data2[i].Count;
		for (j = 0; j < count2; j++) {
			if (mfci_data2data[j].Frame == frame) {
				vqap->Config.EventHandler((VQAHandle *)vqap, chunkid, mfci_data2data[j].Buffer, mfci_data2data[j].Size);
			}
		}
	}

	VQAMSCInfo *msci_info = &vqap->MSCInfo;
	VQAMSCInfo::HEADER *msci_infohdr = &msci_info->Header;

	count1 = msci_infohdr->Count;
	for (i = 0; i < count1; i++) {
		chunkid = msci_info->Table[i].ChunkID;
		VQAMSCInfo::DATA2::DATA *msci_data2data = msci_info->Data2[i].Data;
		count2 = msci_info->Data2[i].Count;
		for (j = 0; j < count2; j++) {
			if (msci_data2data[j].Frame == frame) {
				vqap->Config.EventHandler((VQAHandle *)vqap, chunkid, msci_data2data[j].Buffer, msci_data2data[j].Size);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007BE940
void VQA_UnVQFrame(VQAHandleP *vqap, VQAFrameNode *frame)
{
	VQACBNode *codebk;
	VQAConfig    *config;
	VQADrawer    *drawer;

	vqap = (VQAHandleP *)vqap;
	config = &vqap->Config;
	drawer = &vqap->Drawer;
	codebk = frame->Codebook;

	if (!(drawer->Flags & VQADRWF_STEP)) {
		unsigned char * buf = NULL;

		if (config->EventHandler != NULL) {
			buf = (unsigned char *)config->EventHandler((VQAHandle *)vqap, VQAEVENT_LOCK, NULL, 0);
		}

		if (buf == NULL) {
			buf = drawer->ImageBuf;
		}

		if (buf == NULL) {
			return;
		}

		unsigned char * buffer = buf + drawer->ScreenOffset;
		long blocksperrow = drawer->BlocksPerRow;
		long numrows = drawer->NumRows;
		unsigned char * codebook;
		unsigned char * pointers;

		if ((frame->Flags & VQAFRMF_ALTPTR)) {
			pointers =  (unsigned char *)vqap->AltPtrBuffer;
		} else {
			pointers = frame->Pointers;
		}

		if ((codebk->Flags & VQACBF_ALTPTR)) {
			codebook =  (unsigned char *)vqap->AltCBBuffer;
		} else {
			codebook = codebk->Buffer;
		}

		if (config->EventHandler != NULL) {
			if ((codebk->Flags & VQACBF_CBFULL)) {
				codebk->Flags &= ~VQACBF_CBFULL;
				config->EventHandler((VQAHandle *)vqap, VQAEVENT_CODEBOOK, codebook, codebk->CodebookSize);
			}
		}

		if (!(frame->Flags & VQAFRMF_RSDCOMP)) {
			vqap->UnVQ1(codebook, pointers, buffer, blocksperrow, numrows, drawer->ImageWidth);
		} else {
			vqap->UnVQ2(codebook, pointers, buffer, blocksperrow, numrows, drawer->ImageWidth);
		}

		if (config->EventHandler != NULL) {
			config->EventHandler((VQAHandle *)vqap, VQAEVENT_UNLOCK, NULL, 0);
		}
	}
}

// FUNCTION: GAMEMD 0x007BEA40
long DrawFrame_MCGA(VQAHandle *vqa)
{
	VQAHandleP *vqap;
	VQADrawer    *drawer;
	VQAFrameNode *curframe;
	long         rc;

	vqap = (VQAHandleP *)vqa;
	drawer = &vqap->Drawer;

	rc = Select_Frame(vqap);
	if (rc != VQAERR_NONE) {
		return(rc);
	}

	Prepare_Frame(vqap);

	curframe = drawer->CurFrame;

	vqap->Flipper.CurFrame = curframe;

	if (!(drawer->Flags & VQADRWF_HOLD)) {
		drawer->CurFrame = curframe->Next;
	}

	return(VQAERR_NONE);
}

// FUNCTION: GAMEMD 0x007BEA80
STATIC long Select_Frame(VQAHandleP *vqap)
{
	VQADrawer    *drawer;
	VQAConfig    *config;
	VQAFrameNode *curframe;
	long         desiredframe;
	unsigned long curtime;

	config = &vqap->Config;
	drawer = &vqap->Drawer;
	curframe = drawer->CurFrame;

	if ((curframe->Flags & VQAFRMF_LOADED) == 0) {
		drawer->WaitsOnLoader++;

		if ((drawer->Flags & VQADRWF_REPAINT) && !(vqap->Flags & VQADATF_LDONE)) {
			VQA_SetPreviousFrameNode(vqap);
			if ((drawer->Flags & VQADRWF_FORCEDRAW)) {
				drawer->Flags |= VQADRWF_HOLD;
				curframe->Flags |= VQAFRMF_HOLD;
			}
			return(VQAERR_NONE);
		} else {
			return(VQAERR_NOBUFFER);
		}
	}

	if ((drawer->Flags & VQADRWF_FORCEDRAW)) {
		VQA_SetCurrentFrameAsLast(vqap);
		drawer->Flags |= VQADRWF_HOLD;
		curframe->Flags |= VQAFRMF_HOLD;
		return(VQAERR_NONE);
	}

	if (config->OptionFlags & VQAOPTF_STEP) {
		drawer->LastFrameNum = curframe->FrameNum;
		curtime = ((curframe->FrameNum * VQA_TIMETICKS) / config->DrawRate);
		VQA_SetTimer(vqap, curtime);
		return(VQAERR_NONE);
	}

	curtime = VQA_GetTime(vqap);

	long endtime = 0;
	if ( !(drawer->Flags & VQADRWF_STEP)) {
		endtime = drawer->LastFrameNum + 1;
	} else {
		endtime = drawer->LastFrameNum + 2;
	}

	if ((int)curtime < ((endtime * VQA_TIMETICKS) / config->DrawRate)) {
		if (drawer->Flags & VQADRWF_REPAINT) {
			if (VQA_ComputeDesiredFrame(vqap, config, drawer, curframe, curtime) == drawer->LastFrameNum && (curframe->Flags & VQAFRMF_HOLD)) {
				drawer->LastFrameNum = curframe->FrameNum;
				return(VQAERR_NONE);
			} else {
				drawer->LastFrameNum = curframe->FrameNum;
				drawer->Flags |= VQADRWF_HOLD;
				curframe->Flags |= VQAFRMF_HOLD;
				return(VQAERR_NONE);
			}
		} else {
			return(VQAERR_NOT_TIME);
		}
	}

	desiredframe = VQA_ComputeDesiredFrame(vqap, config, drawer, curframe, curtime);

	#if(VQAMONO_ON)
	drawer->DesiredFrame = desiredframe;
	#endif

	if (config->DrawFlags & VQACFGF_NOSKIP) {
		return(VQA_SetCurrentFrameAsLast(vqap));
	}

	long maxskip = (config->FrameRate / 3) - 1;
	if (maxskip < 0) {
		maxskip = 0;
	}

	while (curframe->FrameNum != desiredframe) {
		if (curframe->Flags & VQAFRMF_KEY) {
			break;
		}

		if (VQA_CalcFramesSinceDrawn(vqap) > maxskip) {
			break;
		}

		if (curframe->Flags & VQAFRMF_PALETTE) {
			if (curframe->Flags & VQAFRMF_PALCOMP) {
				curframe->PaletteSize = VQA_LCW_Uncompress((char *)curframe->Palette
						+ curframe->PalOffset, (char *)curframe->Palette,
						vqap->Max_Pal_Size);

				curframe->Flags &= ~VQAFRMF_PALCOMP;
			}

			memcpy(drawer->Palette_24, curframe->Palette, curframe->PaletteSize);
			drawer->CurPalSize = curframe->PaletteSize;
			drawer->Flags |= VQADRWF_SETPAL;
		}

		if (config->EventHandler != NULL) {
			if (curframe->Flags & VQAFRMF_LOOPED) {
				config->EventHandler((VQAHandle *)vqap, VQAEVENT_LOOPED, (void *)curframe->FrameNum, vqap->LoopID);
				curframe->Flags &= ~VQAFRMF_LOOPED;
			}

			if (curframe->Flags & VQAFRMF_LOOPJMP) {
				config->EventHandler((VQAHandle *)vqap, VQAEVENT_LOOPJUMP, (void *)curframe->FrameNum, vqap->LoopID);
				curframe->Flags &= ~VQAFRMF_LOOPJMP;
			}

			if (curframe->Flags & VQAFRMF_CHUNKS) {
				VQA_DispatchFrameChunks(vqap, curframe->FrameNum);
				curframe->Flags &= ~VQAFRMF_CHUNKS;
			}
		}

		if (config->DrawerCallback != NULL) {
			if ((config->DrawerCallback(NULL, curframe->FrameNum)) != 0) {
				return(VQAERR_EOF);
			}
		}

		curframe->PrevFlags = curframe->Flags;
		curframe->Flags = 0L;
		curframe = curframe->Next;
		drawer->CurFrame = curframe;
		vqap->SkippedFrames++;

		if (!(curframe->Flags & VQAFRMF_LOADED)) {
			drawer->WaitsOnLoader++;

			if ((drawer->Flags & VQADRWF_REPAINT) && !(vqap->Flags & VQADATF_LDONE)) {
				VQA_SetPreviousFrameNode(vqap);
				return(VQAERR_NONE);
			}
			return(VQAERR_NOBUFFER);
		}
	}

	return(VQA_SetCurrentFrameAsLast(vqap));
}

// FUNCTION: GAMEMD 0x007BEE20
long VQA_ComputeDesiredFrame(VQAHandleP *vqap, VQAConfig *config, VQADrawer *drawer, VQAFrameNode *frame, unsigned long time)
{
	VQAHeader *header;
	long frame_rate;
	long frame_num;
	long result;
	bool crossed_loop;
	long loop_end;

	header = &vqap->Header;

	frame_rate = config->FrameRate;
	frame_num = frame->FrameNum;

	result = (time * frame_rate) / (int)VQA_TIMETICKS;

	if (drawer->Flags & VQADRWF_STEP) {
		result--;
	}

	crossed_loop = false;

	loop_end = vqap->LoopEndFrameMode2;

	if (vqap->Flags & VQADATF_LOOPED) {
		if ((vqap->Flags & VQADATF_LOOPJMP)) {
			loop_end = vqap->LoopEndFrame2;
		}

		while (true) {
			if ((config->DrawFlags & VQACFGF_NOSKIP) || result <= loop_end) {
				if (!(config->DrawFlags & VQACFGF_NOSKIP) || frame_num != vqap->LoopStartFrame0 || crossed_loop) {
					break;
				}
			}

			int step_frames = vqap->LoopStartFrame0 - loop_end;
			step_frames--;
			crossed_loop = step_frames == 0;
			VQA_StepTimer(vqap, VQA_TIMETICKS * step_frames / frame_rate);
			result = ((int)VQA_GetTime(vqap) * (int)frame_rate) / (int)VQA_TIMETICKS;
			crossed_loop = true;
			if (step_frames == 0) {
				break;
			}
		}
	}

	if (config->DrawFlags & VQACFGF_NOSKIP) {
		if (result > frame_num) {
			int step_frames = (frame_num - result);
			if (!(header->Flags & VQAHDF_AUDIO) || !(config->OptionFlags & VQAOPTF_AUDIO)) {
				VQA_StepTimer(vqap, VQA_TIMETICKS * step_frames / frame_rate);
			}
			result = frame_num;
			vqap->Flags |= VQADATF_FRAMESTALL;
		} else {
			vqap->Flags &= ~VQADATF_FRAMESTALL;
		}
	}

	if (crossed_loop) {
		if ((drawer->Flags & VQADRWF_STEP) && !(config->DrawFlags & VQACFGF_NOSKIP)) {
			result--;
			if (result < vqap->LoopStartFrame0) {
				result = loop_end;
			}
		}
		if (vqap->LoopEndFrameNormal != -1) {
			vqap->LoopStartFrame0 = vqap->LoopStartFrame1;
			vqap->LoopStartFrame1 = -1;
			vqap->LoopEndFrameMode2 = vqap->LoopEndFrameNormal;
			vqap->LoopEndFrameNormal = -1;
		}
		if (!(vqap->Flags & VQADATF_LOOPJMP)) {
			vqap->LoopStartFrame2 = vqap->LoopStartFrame0;
			vqap->LoopEndFrame2 = vqap->LoopEndFrameMode2;
		}
		vqap->Flags &= ~(VQADATF_LOOPED);
		vqap->Flags &= ~(VQADATF_LOOPJMP);
	}

	return(result);
}

// FUNCTION: GAMEMD 0x007BEFE0
STATIC void Prepare_Frame(VQAHandleP *vqap)
{
	VQADrawer    *drawer;
	VQAFrameNode *curframe;
	VQACBNode    *codebook;

	drawer = &vqap->Drawer;
	curframe = drawer->CurFrame;
	codebook = curframe->Codebook;

	if ((drawer->Flags & VQADRWF_STEP)) {
		return;
	}

	if (codebook->Flags & VQACBF_CBCOMP) {
		void *cbbuffer = NULL;
		if (vqap->AltBufferFlags & VQAABUFF_ALTCB) {
			cbbuffer = vqap->AltCBBuffer;
			codebook->Flags |= VQACBF_ALTPTR;
		} else {
			cbbuffer = codebook->Buffer;
		}

		codebook->CodebookSize = VQA_LCW_Uncompress((char *)(codebook->Buffer + codebook->CBOffset), (char *)cbbuffer, vqap->CBBufferSize);

		codebook->Flags &= (~VQACBF_CBCOMP);
	}

	if (curframe->Flags & VQAFRMF_PALCOMP) {
		curframe->PaletteSize = VQA_LCW_Uncompress((char *)curframe->Palette +
				curframe->PalOffset,(char *)curframe->Palette,vqap->Max_Pal_Size);

		curframe->Flags &= ~VQAFRMF_PALCOMP;
	}

	if (curframe->Flags & VQAFRMF_PTRCOMP) {
		void *ptrbuffer = NULL;
		if (vqap->AltBufferFlags & VQAABUFF_ALTPTR) {
			ptrbuffer = vqap->AltPtrBuffer;
			curframe->Flags |= VQAFRMF_ALTPTR;
		} else {
			ptrbuffer = curframe->Pointers;
		}
		VQA_LCW_Uncompress((char *)curframe->Pointers + curframe->PtrOffset,
				(char *)ptrbuffer, vqap->PtrBufferSize);

		curframe->Flags &= ~VQAFRMF_PTRCOMP;
	}
}

// FUNCTION: GAMEMD 0x007BF0B0
STATIC long PageFlip_MCGA(VQAHandle *vqa)
{
	VQAHandleP *vqap;
	VQADrawer     *drawer;
	VQAFrameNode  *curframe;
	VQAConfig     *config;
	unsigned char *pal;
	long          palsize;
	long          slowpal;

	vqap = (VQAHandleP *)vqa;
	config = &vqap->Config;
	drawer = &vqap->Drawer;
	curframe = vqap->Flipper.CurFrame;

	if ((curframe->Flags & VQAFRMF_PALETTE)
			|| (drawer->Flags & VQADRWF_SETPAL)) {
		pal = curframe->Palette;
		palsize = curframe->PaletteSize;
		slowpal = (config->OptionFlags & VQAOPTF_SLOWPAL) ? 1 : 0;

		if (curframe->Flags & VQAFRMF_PALETTE) {
			if (config->EventHandler != NULL) {
				config->EventHandler(vqa, VQAEVENT_PALETTE, (void *)pal, (long)palsize);
			}
		}
		else if (drawer->Flags & VQADRWF_SETPAL) {
			drawer->Flags &= (~VQADRWF_SETPAL);

			if (config->EventHandler != NULL) {
				config->EventHandler(vqa, VQAEVENT_PALETTE, (void *)drawer->Palette_24, (long)drawer->CurPalSize);
			}
		}
	}

	VQA_UnVQFrame((VQAHandleP *)vqa, curframe);

	if (config->DrawerCallback != NULL) {
		if ((config->DrawerCallback(vqa, curframe->FrameNum)) != 0) {
			return(VQAERR_EOF);
		}
	}

	return(VQAERR_NONE);
}

// FUNCTION: GAMEMD 0x007BF140
STATIC long DrawFrame_MCGABuf(VQAHandle *vqa)
{
	long rc;
	VQAHandleP *vqap;
	VQADrawer *drawer;
	VQAFrameNode *curframe;

	vqap = (VQAHandleP *)vqa;
	drawer = &vqap->Drawer;

	rc = Select_Frame(vqap);
	if (rc != VQAERR_NONE) {
		return(rc);
	}

	Prepare_Frame(vqap);

	curframe = drawer->CurFrame;

	VQA_UnVQFrame(vqap, curframe);

	vqap->Flipper.CurFrame = curframe;

	if ((drawer->Flags & VQADRWF_HOLD) == 0) {
		drawer->CurFrame = curframe->Next;
	}

	return(VQAERR_NONE);
}

// FUNCTION: GAMEMD 0x007BF190
STATIC long PageFlip_MCGABuf(VQAHandle *vqa)
{
	VQAHandleP *vqap;
	VQADrawer *drawer;
	VQAFrameNode *curframe;
	VQAConfig *config;
	unsigned char *pal;
	long palsize;
	long slowpal;

	vqap = (VQAHandleP *)vqa;
	config = &vqap->Config;
	drawer = &vqap->Drawer;
	curframe = vqap->Flipper.CurFrame;

	if (curframe->Flags & VQAFRMF_PALETTE || drawer->Flags & VQADRWF_SETPAL) {
		pal = curframe->Palette;
		palsize = curframe->PaletteSize;
		slowpal = (config->OptionFlags & VQAOPTF_SLOWPAL) ? 1 : 0;

		if (curframe->Flags & VQAFRMF_PALETTE) {
			if (config->EventHandler != NULL) {
				config->EventHandler(vqa, VQAEVENT_PALETTE, (void *)pal, (long)palsize);
			}
		} else if (drawer->Flags & VQADRWF_SETPAL) {
			drawer->Flags &= ~VQADRWF_SETPAL;

			if (config->EventHandler != NULL) {
				config->EventHandler(vqa, VQAEVENT_PALETTE, (void *)drawer->Palette_24,
						(long)drawer->CurPalSize);
			}
		}
	}

	if (config->DrawerCallback != NULL) {
		if ((config->DrawerCallback(vqa, curframe->FrameNum)) != 0) {
			return(VQAERR_EOF);
		}
	}

	return(VQAERR_NONE);
}
