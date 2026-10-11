// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include <stdio.h>
#include "vqaplay.h"
#include "vqaplayp.h"
#include "video.h"
#include "ahandle.h"
#include "vqadebug.h"

long VQA_ResetLastFrameNum(VQAHandle *vqa);

void VQA_ResetCache(VQAHandleP *vqap);

long VQA_Configure_Buffer(VQAHandleP *vqap);

void VQA_SetTimer(VQAHandleP *vqap, long time);

unsigned long VQA_GetTime(VQAHandleP *vqap);

void VQA_StartAudio(VQAHandleP *vqap);

void VQA_StopAudio(VQAHandleP *vqap);

long VQA_LoadFrame(VQAHandleP *vqap, long flags);

long User_Update(VQAHandle *vqa);

long VQA_SetLoop(VQAHandle *vqa, int id, int iterations, int mode);

long VQA_SetLoop_Internal(VQAHandle *vqa, int start, int end, int iterations, int mode);

long VQA_ReloadPalette(VQAHandleP *vqap, long framenum, int force);

VQABool VQA_IsFrameStartOfLoop(VQAHandleP *vqap, long framenum);

long VQA_SeekGroup(VQAHandleP *vqap, long framenum, long groupsize, VQABool preloadaudio, VQABool reset_state, VQABool &skipcodebook);

long PrimeBuffers(VQAHandle *vqa);

long VQA_LargestLoop(VQAHandleP *vqap, long);

extern void __cdecl UnVQ_Nop(unsigned char *codebook, unsigned char *pointers,
		unsigned char *buffer, unsigned long blocksperrow,
		unsigned long numrows, unsigned long bufwidth);

// FUNCTION: 0x007C2C60
void VQA_Close(VQAHandle *vqa)
{
	VQAHandleP *vqap;
	VQAConfig *config;
	VQAAudio *audio;

	vqap = (VQAHandleP *)vqa;
	config = &vqap->Config;
	audio = &vqap->Audio;

	#if(VQAAUDIO_ON)

	if (config->OptionFlags & VQAOPTF_AUDIO && vqap->Header.Flags & (VQAHDF_AUDIO|VQAHDF_ALTAUDIO)) {
		VQA_CloseAudio((VQAHandleP *)vqap);
		if (audio->Flags & VQAAUDF_MEMLOCKED) {
			Unlock_Audio_Handler();
			audio->Flags &= ~VQAAUDF_MEMLOCKED;
		}
	}
	#endif

	if (vqap->Flags & VQADATF_ALTIMG && vqap->AltImageBuf != NULL) {
		config->MemoryHandler((VQAHandle *)vqap, VQAMEM_UNLOCK, vqap->AltImageBuf, vqap->AltImageWidth * vqap->AltImageHeight);
		config->MemoryHandler((VQAHandle *)vqap, VQAMEM_FREE, vqap->AltImageBuf, NULL);
	}

	FreeBuffers(vqap);
	config->StreamHandler((VQAHandle *)vqap, VQACMD_CLOSE, NULL, NULL);
	config->MemoryHandler((VQAHandle *)vqap, VQAMEM_UNLOCK, (VQAHandleP *)vqap, sizeof(VQAHandleP));
	config->MemoryHandler((VQAHandle *)vqap, VQAMEM_FREE, (VQAHandleP *)vqap, NULL);
}

// FUNCTION: 0x007C2D00
long VQA_Play(VQAHandle *vqa, long mode, int flags)
{
	VQAHandleP *vqabuf;
	VQAConfig *config;
	VQADrawer *drawer;
	VQAAudio  *audio;
	long      rc;
	long      curtime;
	#if(VQASTANDALONE)
	long      key;
	#endif

	rc = VQAERR_NONE;

	vqabuf = ((VQAHandleP *)vqa);
	drawer = &vqabuf->Drawer;
	config = &vqabuf->Config;

	audio = &vqabuf->Audio;

	if (!(vqabuf->Flags & VQADATF_PRIMED)) {
		rc = VQA_ResetLastFrameNum(vqa);
		if (rc != VQAERR_NONE) {
			return(rc);
		}

		if (!(vqabuf->Flags & VQADATF_BUFCONFIG)) {
			VQA_Configure_Buffer((VQAHandleP *)vqa);
		}

		#if(VQAAUDIO_ON)
		if ((config->OptionFlags & VQAOPTF_AUDIO) && vqabuf->Audio.IsLoaded[0]) {
			VQA_StartAudio((VQAHandleP *)vqabuf);
		}
		#endif

		curtime = ((drawer->CurFrame->FrameNum * VQA_TIMETICKS)
				/ config->DrawRate);

		VQA_SetTimer((VQAHandleP *)vqabuf, curtime);
		((VQAHandleP *)vqa)->StartTime = VQA_GetTime((VQAHandleP *)vqabuf);

		#if(VQAMONO_ON)
		if (config->OptionFlags & VQAOPTF_MONO) {
			VQA_InitMono((VQAHandleP *)vqabuf);
		}
		#endif

		vqabuf->Flags |= VQADATF_PRIMED;
	}

	int count;
	if (config->UnusedCallback != NULL) {
		count = config->UnusedCallback(vqa);
	} else {
		count = -1;
	}

	int lastcount = vqabuf->LastCallbackCount;
	if (lastcount == -1) {
		vqabuf->LastCallbackCount = count;
		vqabuf->MaxCallbackDelta = 0;
	} else {
		int delta = count - lastcount;
		vqabuf->LastCallbackCount = count;
		int maxdelta = vqabuf->MaxCallbackDelta;
		if (delta > maxdelta) {
			vqabuf->MaxCallbackDelta = delta;
		} else {
			vqabuf->MaxCallbackDelta = maxdelta;
		}
	}

	switch (mode) {
		case VQAMODE_PAUSE:
			if ((vqabuf->Flags & VQADATF_PAUSED) == 0) {
				vqabuf->Flags |= VQADATF_PAUSED;
				((VQAHandleP *)vqa)->EndTime = VQA_GetTime((VQAHandleP *)vqabuf);

				#if(VQAAUDIO_ON)
				if (vqabuf->Audio.Flags & VQAAUDF_ISPLAYING) {
					VQA_PauseAudio((VQAHandleP *)vqabuf);
				}
				#endif

#ifdef _DEBUG
				DebugString("VQA_Play() - Pausing @ %ld\n", vqabuf->EndTime);
#endif
			}

			if ((flags & VQAPLAYF_REPAINT)) {
				drawer->Flags |= VQADRWF_REPAINT;
				drawer->Flags |= VQADRWF_FORCEDRAW;
				if ((rc = (*(vqabuf->Draw_Frame))(vqa)) == VQAERR_NONE) {
					if ((rc = User_Update(vqa)) != VQAERR_NONE) {
						vqabuf->Flags |= (VQADATF_DDONE|VQADATF_LDONE);
					} else {
						rc = drawer->LastFrameNum;
					}
				} else {
					if (rc == VQAERR_NOBUFFER && (vqabuf->Flags & VQADATF_LDONE)) {
						vqabuf->Flags |= (VQADATF_DDONE);
					}
				}
				drawer->Flags &= ~VQADRWF_STEP;
				drawer->Flags &= ~VQADRWF_REPAINT;
				drawer->Flags &= ~VQADRWF_FORCEDRAW;
			} else {
				rc = VQAERR_PAUSED;
			}

			break;

		case VQAMODE_RUN:
		case VQAMODE_WALK:
		default:

			if (vqabuf->Flags & VQADATF_PAUSED) {
				vqabuf->Flags &= ~VQADATF_PAUSED;

				#if(VQAAUDIO_ON)
				if (config->OptionFlags & VQAOPTF_AUDIO) {
					VQA_StartAudio((VQAHandleP *)vqabuf);
				}
				#endif

				VQA_SetTimer((VQAHandleP *)vqabuf, ((VQAHandleP *)vqa)->EndTime);

#ifdef _DEBUG
				DebugString("VQA_Play() - Restarting @ %ld\n", VQA_GetTime((VQAHandleP *)vqabuf));
#endif
			}

			while ((vqabuf->Flags & (VQADATF_DDONE|VQADATF_LDONE))
					!= (VQADATF_DDONE|VQADATF_LDONE)) {
				if (!(vqabuf->Flags & VQADATF_LDONE)) {
					if ((rc = VQA_LoadFrame(vqabuf, 0)) == VQAERR_NONE) {
						((VQAHandleP *)vqa)->LoadedFrames++;
					}
					else {
						if ((rc != VQAERR_NOBUFFER) && (rc != VQAERR_SLEEPING)) {
							vqabuf->Flags |= VQADATF_LDONE;
						}
					}

					if (config->OptionFlags & VQAOPTF_WAITFILL) {
						if (rc == VQAERR_NOBUFFER || (vqabuf->Flags & VQADATF_LDONE)) {
								if (vqabuf->Flags & VQADATF_AUDIOSYNC) {
#ifdef _DEBUG
									DebugString("VQA_Play() - Drawer waitfill\n");
#endif
									if (!(vqabuf->Flags & VQADATF_FRAMESTALL)) {
										vqabuf->Flags |= VQADATF_REFILLED;
									if (config->OptionFlags & VQAOPTF_AUDIO) {
#ifdef _DEBUG
										DebugString("VQA_Play() - Waitfill StartAudio\n");
#endif
										VQA_StartAudio(vqabuf);
									}
								}
							}
						} else if (!(vqabuf->Flags & VQADATF_LDONE)) {
							if ((vqabuf->Audio.Flags & VQAAUDF_ISSTARVED)) {
#ifdef _DEBUG
									DebugString("VQA_Play() - Loader waitfill\n");
#endif
								if ((vqabuf->Audio.Flags & VQAAUDF_ISPLAYING)) {
#ifdef _DEBUG
										DebugString("VQA_Play() - Waitfill PauseAudio\n");
#endif
									VQA_PauseAudio(vqabuf);
								}
								vqabuf->Flags |= VQADATF_AUDIOSYNC;
								audio->Flags &= ~VQAAUDF_ISSTARVED;
							}
 						}
					}
				}else{
				}

				if ((config->DrawFlags & VQACFGF_NODRAW) == 0) {
					if ((vqabuf->Config.OptionFlags & VQAOPTF_WAITFILL)) {
						if ((vqabuf->Flags & VQADATF_AUDIOSYNC)) {
							if ((vqabuf->Flags & VQADATF_REFILLED)) {
								vqabuf->Flags &= ~VQADATF_AUDIOSYNC;
								vqabuf->Flags &= ~VQADATF_REFILLED;
							} else if ((vqabuf->Flags & VQADATF_FRAMESTALL) == 0) {
								rc = VQAERR_NOT_TIME;
								break;
							}
						}
					}

					if ((flags & VQAPLAYF_STEP)) {
						drawer->Flags |= VQADRWF_STEP;
					} else if ((flags & VQAPLAYF_REPAINT)) {
						drawer->Flags |= VQADRWF_REPAINT;
					}

					if ((rc = ((VQAHandleP *)vqabuf)->Draw_Frame((VQAHandle *)vqabuf)) == VQAERR_NONE) {
						if (!(drawer->Flags & VQADRWF_HOLD)) {
							((VQAHandleP *)vqa)->DrawnFrames++;
						}

						#ifdef WIN32never
						curframe = drawer->CurFrame;
						pal = curframe->Palette;
						palsize = curframe->PaletteSize;
						slowpal = (config->OptionFlags & VQAOPTF_SLOWPAL) ? 1 : 0;
						if ((curframe->Flags & VQAFRMF_PALETTE)
								|| (drawer->Flags & VQADRWF_SETPAL)) {
							setpalette = TRUE;
						}else{
							setpalette = FALSE;
						}
						#endif

						if ((rc = User_Update((VQAHandle *)vqabuf)) != VQAERR_NONE) {
							vqabuf->Flags |= (VQADATF_DDONE|VQADATF_LDONE);
						} else {
							rc = drawer->LastFrameNum;
						}
						#ifdef WIN32never
						if (setpalette) {
							SetPalette(pal, palsize, slowpal);
							curframe->Flags &= ~VQAFRMF_PALETTE;
							drawer->Flags &= ~VQADRWF_SETPAL;
 						}
						#endif

					}else {
						if ((rc == VQAERR_NOBUFFER) && (vqabuf->Flags & VQADATF_LDONE)) {
							vqabuf->Flags |= VQADATF_DDONE;
						}
					}
					drawer->Flags &= ~VQADRWF_STEP;
					drawer->Flags &= ~VQADRWF_REPAINT;
				} else {
					vqabuf->Flags |= VQADATF_DDONE;
					drawer->CurFrame->Flags = 0L;
					drawer->CurFrame = drawer->CurFrame->Next;
				}

				#if(VQAMONO_ON)
				if (config->OptionFlags & VQAOPTF_MONO) {
					VQA_UpdateMono((VQAHandleP *)vqabuf);
				}
				#endif

				if (mode == VQAMODE_WALK) {
					break;
				}
				#if(VQASTANDALONE)
				else {
					if (config->OptionFlags & VQAOPTF_STEP) {
						while ((key = Check_Key()) == 0);
						Get_Key();

						if (key == 27) {
							break;
						}
					}

					if ((key = Check_Key()) != 0) {
						mode = VQAMODE_STOP;
						break;
					}
				}
				#endif
			}
			break;
	}

	if (((vqabuf->Flags & (VQADATF_DDONE|VQADATF_LDONE))
			== (VQADATF_DDONE|VQADATF_LDONE)) || (mode == VQAMODE_STOP)) {
		((VQAHandleP *)vqa)->EndTime = VQA_GetTime((VQAHandleP *)vqabuf);

		#if(VQAAUDIO_ON)
		if (vqabuf->Audio.Flags & VQAAUDF_ISPLAYING) {
			VQA_StopAudio((VQAHandleP *)vqabuf);
		}
		#endif

		rc = VQAERR_EOF;
	}

	return(rc);
}

// FUNCTION: 0x007C30D0
long VQA_SeekFrame(VQAHandle *vqa, long framenum, long fromwhere)
{
	VQAHandleP   *vqap;
	VQALoader    *loader;
	VQAHeader    *header;
	VQAFrameNode *frame;
	VQAConfig    *config;
	long         rc = VQAERR_NONE;

	#if(VQAAUDIO_ON)
	VQAAudio     *audio;
	long         audio_on;
 	#endif

	vqap = (VQAHandleP *)vqa;
	loader = &vqap->Loader;
	header = &vqap->Header;
	config = &vqap->Config;

	fromwhere = fromwhere;

	frame = loader->CurFrame;

	#if(VQAAUDIO_ON)
	audio = &vqap->Audio;

	audio_on = (audio->Flags & VQAAUDF_ISPLAYING);
	if (audio_on) {
		VQA_StopAudio(vqap);
	}
	#endif

	if ((framenum <= vqap->StopFrame) && (vqap->Foff != NULL)) {
		VQA_ResetCache(vqap);
		VQA_ResetLastFrameNum(vqa);

		bool has_palette = false;
		if (header->ColorMode == 0 && !VQA_IsFrameStartOfLoop(vqap, framenum)) {
			rc = VQA_ReloadPalette(vqap, framenum, true);
			if (rc == VQAERR_NONE) {
				if (frame->Flags & VQAFRMF_PALETTE) {
					has_palette = true;
				}
			}
		}

		if (rc == VQAERR_NONE) {
			loader->CurFrameNum = framenum;

			bool skipcodebook;
			bool preloadaudio = (((unsigned char)header->Flags) & VQAHDF_SNDJUMP) == 0;

			rc = VQA_SeekGroup(vqap, framenum, header->Groupsize, preloadaudio, 1, skipcodebook);

			if (rc == VQAERR_NONE) {
				loader->CurFrameNum = framenum;

				loader->CurFrame->Flags = 0;

				if (has_palette) {
					loader->CurFrame->Flags |= VQAFRMF_PALETTE;
				}

				frame = loader->CurFrame->Next;

				while (frame != loader->CurFrame) {
					frame->Flags = 0;
					frame = frame->Next;
				}

				vqap->Drawer.CurFrame = loader->CurFrame;

				rc = PrimeBuffers(vqa);

				if ((rc == VQAERR_NONE) || (rc == VQAERR_EOF)) {
					rc = framenum;
					VQA_SetTimer(vqap, VQA_TIMETICKS * framenum / config->DrawRate);
				}
			}
		}
		return(rc);
	} else {
		rc = VQAERR_SEEK;
	}
	return(rc);
}

// FUNCTION: 0x007C3230
long VQA_SetUnVQ(VQAHandle *vqa, UNVQ_FUNC unvq1, UNVQ_FUNC unvq2)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;

	if (unvq1 != NULL) {
		vqap->UnVQ1 = unvq1;
	} else {
		vqap->UnVQ1 = UnVQ_Nop;
	}
	if (unvq2 != NULL) {
		vqap->UnVQ2 = unvq2;
	} else {
		vqap->UnVQ2 = UnVQ_Nop;
	}

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C3270
long VQA_Set_DrawBuffer(VQAHandle *vqa, unsigned char *buffer, unsigned long width, unsigned long height, long xpos, long ypos)
{
	long origin;
	VQAHeader *header;
	VQADrawer *drawer;
	VQAConfig *config;

	header = &((VQAHandleP *)vqa)->Header;
	drawer = &((VQAHandleP *)vqa)->Drawer;
	config = &((VQAHandleP *)vqa)->Config;

	origin = (config->DrawFlags & VQACFGF_ORIGIN);

	if (width == 0) {
		width = header->ImageWidth;
	}

	if (height == 0) {
		height = header->ImageHeight;
	}

	if (xpos < 0 && ypos < 0) {
		if (config->DrawFlags & VQACFGF_OFFSET) {
			if (config->DrawFlags & VQACFGF_HALFSIZE) {
				drawer->X1 = (header->Xpos / 2);
				drawer->Y1 = (header->Ypos / 2);
			} else {
				drawer->X1 = header->Xpos;
				drawer->Y1 = header->Ypos;
			}
		} else {
			if (config->DrawFlags & VQACFGF_HALFSIZE) {
				drawer->X1 = (width - (header->ImageWidth / 2)) / 2;
				drawer->Y1 = (height - (header->ImageHeight / 2)) / 2;
			} else {
				drawer->X1 = (width - header->ImageWidth) / 2;
				drawer->Y1 = (height - header->ImageHeight) / 2;
			}
		}
	} else {
		if (config->DrawFlags & VQACFGF_HALFSIZE) {
			xpos /= 2;
			ypos /= 2;
		}

		switch (origin) {
			default:
			case VQACFGF_TOPLEFT:
				drawer->X1 = xpos;
				drawer->Y1 = ypos;
				break;

			case VQACFGF_BOTLEFT:
				drawer->X1 = xpos;
				drawer->Y1 = (height - ypos);
				break;

			case VQACFGF_BOTRIGHT:
				drawer->X1 = (width - xpos);
				drawer->Y1 = (height - ypos);
				break;
		}
	}

	if (drawer->X1 >= 0 && drawer->Y1 >= 0 && (unsigned)drawer->X1 < width && (unsigned)drawer->Y1 < height) {
		long x = drawer->X1;
		long y = drawer->Y1;

		if (header->ColorMode == 1 || header->ColorMode == 4) {
			x *= 2;
			y *= 2;
		}

		drawer->ImageWidth = width;
		drawer->ImageHeight = height;
		drawer->ScreenOffset = ((width * y) + x);

		if (buffer != NULL) {
			drawer->ImageBuf = buffer;
			((VQAHandleP *)vqa)->ImageBuf = buffer;
		}

		((VQAHandleP *)vqa)->Flags |= VQADATF_BUFCONFIG;
		return(VQAERR_NONE);
	}

	return(VQAERR_SETBUFFR);
}

// FUNCTION: 0x007C3430
long VQA_SetLoop(VQAHandle *vqa, int id, int iterations, int mode)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;
	VQAConfig *config = &vqap->Config;
	VQALoopInfo * info = &vqap->LoopInfo;

	if (id >= vqap->LoopInfo.Header.Count || id < 0) {
		return VQAERR_SETLOOP;
	}

	if ((config->OptionFlags & VQAOPTF_AUDIO) && vqap->LoopInfo.Header.Count == 0) {
		return(VQAERR_SETLOOP);
	}

	VQALoopInfo::DATA *data = &vqap->LoopInfo.Data[id];
	int start = data->StartFrame;
	int end = data->EndFrame;
	vqap->LoopID = id;
	long rc = VQA_SetLoop_Internal(vqa, start, end, iterations, mode);
	if (rc == VQAERR_NONE) {
		vqap->LoopID = id;
	}
	return(rc);
}

// FUNCTION: 0x007C34B0
long VQA_SetLoop_Internal(VQAHandle *vqa, int start, int end, int iterations, int mode)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;

	long rc = VQAERR_NONE;

	if (start < vqap->NumFrames && end < vqap->NumFrames && start < end && mode >= VQALOOP_NORMAL && mode < VQALOOP_3) {
		if (iterations < 0) {
			iterations = INFINITE_LOOP;
		}

		if (vqap->LoopIterations == 0 && mode == VQALOOP_JUMP) {
			mode = VQALOOP_2;
		}
		if (mode != VQALOOP_2) {
			if (vqap->Flags & VQADATF_LOOPED) {
				if (mode == VQALOOP_NORMAL) {
					vqap->LoopStartFrame1 = start;
					vqap->LoopEndFrameNormal = end;
					end = vqap->LoopEndFrameMode2;
				} else {
					vqap->LoopStartFrame1 = start;
					start = vqap->LoopStartFrame0;
					vqap->LoopEndFrameNormal = vqap->LoopEndFrameMode2;
				}
			} else {
				vqap->LoopStartFrame2 = start;
				vqap->LoopEndFrame2 = end;
			}
		}
		vqap->LoopStartFrame0 = start;
		if (mode == VQALOOP_JUMP) {
			vqap->LoopIterationsJump = iterations;
			vqap->LoopEndFrameJump = end;
		} else {
			vqap->LoopIterations = iterations;
			if (mode == VQALOOP_2) {
				vqap->LoopEndFrameMode2 = end;
				vqap->StopFrame = end;
				if (VQA_SeekFrame((VQAHandle *)vqa, start, 0) >= VQAERR_OK) {
					rc = VQAERR_NONE;
				} else {
					rc = VQAERR_SETLOOP;
				}
			}
		}
	} else {
		rc = VQAERR_SETLOOP;
	}

	return(rc);
}

// FUNCTION: 0x007C35B0
int VQA_GetLoopCount(VQAHandleP *vqap)
{
	return(vqap->LoopInfo.Header.Count);
}

// FUNCTION: 0x007C3670
void VQA_ResetCache(VQAHandleP *vqap)
{
	vqap->LoopCache.ID = -1;
	vqap->LoopCache.Min = -1;
	vqap->LoopCache.Max = -1;
}

// FUNCTION: 0x007C3690
long VQA_SetStop(VQAHandle *vqa, long stop)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;
	long      oldstop = -1;
	VQAHeader *header;

	header = &vqap->Header;

	if ((stop > 0) && (header->Frames >= stop)) {
		oldstop = vqap->StopFrame;
		vqap->StopFrame = (unsigned short)stop;
	}

	return(oldstop);
}

// FUNCTION: 0x007C36D0
long VQA_GetClipping(VQAHandleP *vqap, int & clipw, long & cliph)
{
	if (vqap->Clipper.Width > 0) {
		clipw = vqap->Clipper.Width;
		cliph = vqap->Clipper.Height;
	} else {
		clipw = vqap->ImageWidth;
		cliph = vqap->ImageHeight;
	}
	return(VQAERR_NONE);
}

// FUNCTION: 0x007C3710
long VQA_GetBlockInfo(VQAHandle *vqa, long & blockw, long & blockh, long & clrmode)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;
	VQAHeader *header;

	header = &vqap->Header;

	blockw = header->BlockWidth;
	blockh = header->BlockHeight;
	clrmode = header->ColorMode;
	return(VQAERR_NONE);
}

// FUNCTION: 0x007C3840
long VQA_GetXYPos(VQAHandleP *vqap, int & x, long & y)
{
	VQAHeader *header;

	header = &vqap->Header;

	x = header->Xpos;
	y = header->Ypos;

	return(VQAERR_NONE);
}

// FUNCTION: 0x007C3860
long VQA_LargestLoop(VQAHandleP *vqap, long streamsize)
{
	VQALoopInfo::HEADER *infohdr = &vqap->LoopInfo.Header;
	long largest = 0;

	int count = infohdr->Count;
	for (int i = 0; i < count; i++) {
		VQALoopInfo::DATA *data = &vqap->LoopInfo.Data[i];
		int start = data->StartFrame;
		int stop = data->EndFrame;

		long start_offset;
		long end_offset;

		long loopsize = 0;
		if (stop != vqap->NumFrames - 1) {
			end_offset = VQAFRAME_OFFSET(vqap->Foff[stop + 1]);
			start_offset = VQAFRAME_OFFSET(vqap->Foff[start]);
			loopsize = end_offset - start_offset;
		} else if (streamsize > 0) {
			end_offset = streamsize;
			start_offset = VQAFRAME_OFFSET(vqap->Foff[start]);
			loopsize = end_offset - start_offset;
		} else {
			end_offset = VQAFRAME_OFFSET(vqap->Foff[stop]);
			start_offset = VQAFRAME_OFFSET(vqap->Foff[start]);
			loopsize = end_offset - start_offset;
		}
		if (loopsize > largest) {
			largest = loopsize;
		}
	}
	return(largest);
}
