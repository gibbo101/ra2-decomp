// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "vqa.h"
#include "_keyboar.h"
#include "ccfile.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "mixfile.h"
#include "movies.h"
#include "session.h"
#include "setpal.h"
#include "unvqtblc.h"
#include "vector.h"
#include "vqoption.h"
#include "win.h"
#include "special.hh"
#include "ahandle.h"
#include <unvq.h>
#include <vqaplay.h>
#include <vqaplayp.h>
#include <cmath>
#include "fastmath.h"

long __cdecl VQAMixFileHandler(VQAHandle * vqa, long action, void * buffer, long nbytes);
long __cdecl VQACCFileHandler(VQAHandle * vqa, long action, void * buffer, long nbytes);
long __cdecl VQAEventHandler(VQAHandle * vqa, long action, void * buffer, long nbytes);
long __cdecl VQAMemoryHandler(VQAHandle * vqa, long action, void * buffer, long nbytes);

extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F6970;
extern const double DAT_007F6978;

// FUNCTION: 0x00758F90 $E36
double DAT_00B45B30 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00758FC0 $E39
double DAT_00B45B98 = DAT_00B45B30 / DAT_007F6970;

// FUNCTION: 0x00758FE0 $E42
double DAT_00B45B80 = DAT_007E1718 / DAT_00B45B98;

// FUNCTION: 0x00759000 $E45
double DAT_00B45B78 = DAT_007E1720 * DAT_007F6978;

// FUNCTION: 0x00759020 $E48
double DAT_00B45B38 = DAT_007E1728 * DAT_007F6978;

// FUNCTION: 0x00759040 $E51
double DAT_00B45B10 = DAT_007E1730 * DAT_007F6978;

// FUNCTION: 0x00759060 $E54
int DAT_00B45BA0 = (int)(fastmath::tan(DAT_00B45B10 - DAT_00B45B38) * DAT_007E1738 * DAT_00B45B30);

// FUNCTION: 0x007590A0 $E57
double DAT_00B45B40 = fastmath::atan(DAT_00B45BA0 * DAT_007E1740);

// FUNCTION: 0x007590D0 $E60
double DAT_00B45B70 = fastmath::atan((DAT_00B45BA0 * 2) / DAT_00B45B30);

// FUNCTION: 0x00759100 $E63
int DAT_00B45B94 = (int)(DAT_00B45BA0 * 4 + DAT_007E1738);

struct VQAShortPair {
	short A;
	short B;
	VQAShortPair() : A(0), B(0) {}
};

struct VQAIntTriple {
	int A;
	int B;
	int C;
	VQAIntTriple() : A(0), B(0), C(0) {}
};

// FUNCTION: 0x00759130 $E66
VQAShortPair DAT_00B45B28;

// FUNCTION: 0x00759140 $E69
VQAIntTriple DAT_00B45B48;

struct VQAColorInit {
	int A;
	int B;
	int C;
	VQAColorInit() { C = 0; A = 0x80; B = 0x80; }
};

// FUNCTION: 0x00759160 $E72
VQAColorInit DAT_00B45B88;

// FUNCTION: 0x00759180 $E77
// FUNCTION: 0x007591C0 $E75
DynamicVectorClass<MovieHandle *> IngameVQ;

extern int DAT_00AA0430;
int __fastcall FUN_0053E770(MSG *msg, HWND wnd, UINT min, UINT max, UINT flags);
int __fastcall FUN_0053E730(MSG *msg, HWND wnd, UINT min, UINT max);

// FUNCTION: 0x00759200
bool VQA_Message_Handler(void)
{
	MSG msg;

	DAT_00AA0430++;
	if (FUN_0053E770(&msg, NULL, 0, 0, 0)) {
		if (FUN_0053E730(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: 0x00759260
VQAClass::VQAClass(char const * filename, int flags, VQA_SURF_LOCK_CALLBACK surface_lock, VQA_SURF_UNLOCK_CALLBACK surface_unlock, VQA_SURF_DRAW_CALLBACK surface_draw, int frame_rate, int draw_rate)
{
	unsigned char buffer;
	VQA_DefaultConfig(&Config);

	Flags = flags;
	Config.Owner = this;
	Config.ImageBuf  = (unsigned char *)&buffer;
	Config.DrawFlags = 0;
	if (Get_Option(OPTION_NO_BUFFER) == false) {
		Config.DrawFlags |= VQACFGF_BUFFER;
	}

	if (Get_Option(OPTION_1) == false) {
		Config.DrawFlags |= VQACFGF_NOSKIP;
	}

	if (Get_Option(OPTION_10) == true) {
		Config.DrawFlags |= VQACFGF_HALFSIZE;
	}

	if (Get_Option(OPTION_12) == true) {
		Config.DrawFlags |= VQACFGF_NOTRANS;
	}

	Config.OptionFlags |= VQAOPTF_PALOFF;

	if ((Flags & VQACF_2)) {
		Config.OptionFlags |= VQAOPTF_LOOPS;
		if ((Flags & VQACF_4)) {
			Config.OptionFlags |= VQAOPTF_LOOPCACHE;
		}
	} else {
		Config.OptionFlags &= ~VQAOPTF_LOOPS;
	}

	if (Get_Option(OPTION_17) == true) {
		Config.OptionFlags |= VQAOPTF_MONO;
	} else {
		Config.OptionFlags &= ~VQAOPTF_MONO;
	}

	if (Get_Option(OPTION_NO_AUDIO) == true) {
		Config.OptionFlags &= ~VQAOPTF_AUDIO;
	} else {
		Config.OptionFlags |= VQAOPTF_AUDIO;
	}

	Config.ImageWidth = -1;
	Config.ImageHeight = -1;
	Config.X1 = 0;
	Config.Y1 = 0;
	Config.DrawerCallback = NULL;
	Config.FrameRate = frame_rate;
	Config.DrawRate = draw_rate;

	if (Get_Option(OPTION_NO_AUDIO) == true) {
		Config.NumFrameBufs = 1;
	} else {
		Config.NumFrameBufs = 14;
	}

	Config.NumCBBufs = -1;

	if (Flags & VQACF_PLAY_FROM_MIXFILE) {
		Config.StreamHandler = VQAMixFileHandler;
	} else {
		Config.StreamHandler = VQACCFileHandler;
	}

	Config.MemoryHandler = VQAMemoryHandler;
	Config.EventHandler = VQAEventHandler;

	if (Get_Option(OPTION_SIMPLE_TIMER_CALLBACK) == true) {
		Config.TimerCallback = Simple_Timer_Callback_Audio_Handler;
		Config.RefreshRate = 60;
	} else {
		Config.TimerCallback = Timer_Callback_Audio_Handler;
		Config.RefreshRate = 60;
		Config.LatencyAdjustment = Options.SoundLatency;
	}

	Config.AudioHandler = Stream_Audio_Handler;
	Config.HMIBufSize = 8192;
	CurrentFrame = 0;

	Event3Frame = 0;
	TotalFrames = 0;
	strcpy(Filename, filename);

	Width = 0;
	Height = 0;
	DrawBufferWidth = 0;
	DrawBufferHeight = 0;
	PrimaryColorMode = -1;
	HasPaletteChanged = false;
	SurfaceLockCallback = surface_lock;
	SurfaceUnlockCallback = surface_unlock;
	SurfaceDrawCallback = surface_draw;
	IsFileOpen = false;
	IsOpen = false;

	IsPaused = false;
	IsAdvanceReady = false;
	Subtitles = NULL;
}

// FUNCTION: 0x00759510
VQAClass::~VQAClass(void)
{
	Hicolor_Clear_Table();
}

// FUNCTION: 0x00759540
long VQAClass::Cache_VQA(unsigned long bytes_to_cache)
{
	if (!IsFileOpen) {
		IsFileOpen = FileHandle.Open(Filename, FileClass::READ) != 0;
		if (!IsFileOpen) {
			return(CACHE_OPEN_FILE_ERROR);
		}
		int size = FileHandle.Size();
		Cache.file_size = size;
		Cache.file_buffer_pos = 0;
		Cache.file_offset = 0;
		Cache.file_buffer = (unsigned char *)malloc(size);
		if (Cache.file_buffer == NULL) {
			FileHandle.Close();
			IsFileOpen = false;
			return(CACHE_FAILED_MEM_ALLOC);
		}
	}

	if (bytes_to_cache == CACHE_REST_OF_FILE) {
		bytes_to_cache = Cache.file_size - Cache.file_offset;
	} else {
		if (Cache.file_offset + bytes_to_cache > Cache.file_size) {
			bytes_to_cache = Cache.file_size - Cache.file_offset;
		}
	}

	if ((unsigned)FileHandle.Read(Cache.file_buffer + Cache.file_offset, bytes_to_cache) != bytes_to_cache) {
		FileHandle.Close();
		IsFileOpen = false;
		Cache.file_buffer_pos = 0;
		Cache.file_offset = 0;
		return(CACHE_READ_ERROR);
	}

	Cache.file_offset += bytes_to_cache;
	if (Cache.file_offset >= Cache.file_size) {
		FileHandle.Close();
		IsFileOpen = false;
		return(CACHE_EOF);
	}
	return(CACHE_NO_ERROR);
}

// FUNCTION: 0x00759670
long VQAClass::CacheHandler(long action, void * buffer, long nbytes)
{
	long rc = 1;

	switch (action) {
		case VQACMD_INIT:
		case VQACMD_CLEANUP:
		case VQACMD_OPEN:
		case VQACMD_CLOSE:
			rc = 0;
			break;

		case VQACMD_READ:
			if (Cache.file_buffer_pos + nbytes > Cache.file_size) {
				rc = 1;
				return(rc);
			} else {
				memcpy(buffer, Cache.file_buffer + Cache.file_buffer_pos, nbytes);
				Cache.file_buffer_pos += nbytes;
				rc = 0;
			}
			break;

		case VQACMD_SEEK:
			switch ((int)buffer) {
				case 1:
					Cache.file_buffer_pos += nbytes;
					rc = 0;
					break;
				case 0:
					Cache.file_buffer_pos = nbytes;
					rc = 0;
					break;
				case 2:
					Cache.file_buffer_pos = Cache.file_size - nbytes - 1;
					rc = 0;
					break;

				default:
					rc = 1;
					break;
			}
			break;

		default:
			rc = 1;
			break;
	}
	return(rc);
}

// FUNCTION: 0x00759760
void VQAClass::Handle_Palette_Event(void *buffer)
{
	memcpy(Palette, buffer, sizeof(Palette));
	HasPaletteChanged = true;
}

// FUNCTION: 0x00759790
void VQAClass::Set_DirectDraw_Palette(void)
{
	Set_Palette(Palette);
}

// FUNCTION: 0x007597A0
bool VQAClass::Open_And_Load_Buffers(void)
{
	if (VQA_Open(Filename, &Config, &Handle) != VQAERR_NONE) {
		IsOpen = false;
		return(false);
	}
	VQAHandleP *vqap = (VQAHandleP *)Handle;
	TotalFrames = vqap->NumFrames;
	DesiredColorMode = vqap->ColorMode;
	Width = vqap->ImageWidth;
	Height = vqap->ImageHeight;

	IsOpen = true;

	return(true);
}

// FUNCTION: 0x00759810
int VQAClass::Get_VQA_Version(void) const
{
	if (Handle != NULL) {
		return((VQAHandleP *)Handle)->Version;
	}
	return(-1);
}

// FUNCTION: 0x00759830
int VQAClass::Set_VQA_Volume(int vol)
{
	int old = Config.Volume;
	if (vol != -1) {
		Config.Volume = vol;
	}
	return(old);
}

// FUNCTION: 0x00759850
bool VQAClass::Set_Loop(int loop_id, int iterations)
{
	if (iterations < 0) {
		iterations = -1;
	}
	long rc = VQA_SetLoop(Handle, loop_id, iterations, 1);
	if (rc == VQAERR_NONE) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00759880
bool VQAClass::Set_Loop(int start, int end, int iterations)
{
	if (start < 0 || start >= TotalFrames) {
		start = 0;
	}

	if (end < 0 || end >= TotalFrames) {
		end = TotalFrames - 1;
	}

	if (iterations < 0) {
		iterations = -1;
	}

	long rc = VQA_SetLoop_Internal(Handle, start, end, iterations, 0);
	if (rc == VQAERR_NONE) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x007598D0
void VQAClass::Seek_To_Frame(int frame)
{
	if (frame == (int)CurrentFrame) {
		return;
	}

	if (VQA_SeekFrame(Handle, frame, SEEK_SET) != VQAERR_SEEK) {
		CurrentFrame = frame;
	}
}

unsigned long __fastcall FUN_007C3990(VQAHandle *vqa);

// FUNCTION: 0x00759900
bool VQAClass::Seek_To_Stored_Frame(void)
{
	if (IsOpen) {
		if (VQA_SeekFrame(Handle, Event3Frame, 0) != VQAERR_SEEK) {
			CurrentFrame = Event3Frame;
			return(true);
		}
		return(false);
	}
	return(true);
}

// FUNCTION: 0x00759940
int VQAClass::Play_VQA(int last_frame_to_play, bool nobreakout)
{
	long errval;

	bool brokeout = false;
	bool sleeping = false;

	if (last_frame_to_play < 0 || last_frame_to_play >= TotalFrames) {
		last_frame_to_play = TotalFrames - 1;
	}
	VQA_SetStop(Handle, last_frame_to_play);

	if (PrimaryColorMode != -1) {
		long blockw;
		long blockh;
		long cmode;
		VQA_GetBlockInfo(Handle, blockw, blockh, cmode);
		if (cmode == 1) {
			Hicolor_Init_Table(PrimaryColorMode);
			if (Get_Option(OPTION_ALTERNATE_UNVQ)) {
				VQA_SetUnVQ(Handle, ASM_UnVQ1_C1_TABLE_ALT, UnVQ2_4x2_Table);
			} else {
				VQA_SetUnVQ(Handle, ASM_UnVQ1_C1_TABLE, UnVQ2_4x4_Table);
			}
		} else if (cmode == 4) {
			Hicolor_Init_Table(PrimaryColorMode);
			if (Get_Option(OPTION_ALTERNATE_UNVQ)) {
				VQA_SetUnVQ(Handle, UnVQ1_4x4_Table, UnVQ1_4x2_Table);
			}
		}
	}

	errval = 0;

	while (errval >= VQAERR_OK || errval == VQAERR_NOT_TIME || errval == VQAERR_NOBUFFER || errval == VQAERR_SLEEPING || errval == VQAERR_NONE) {
		if (brokeout) {
			break;
		}

		VQA_Message_Handler();

		if (sleeping == true) {
			if (!GameInFocus) {
				Sleep((1000/30));
				continue;
			} else {
				sleeping = false;
				IsPaused = false;
				DebugString("Movie is awake\n");
			}
		}

		if (GameInFocus == true && !IsPaused) {
			errval = VQA_Play(Handle, VQAMODE_WALK, 0);

			if (errval >= VQAERR_OK) {
				if (HasPaletteChanged) {
					Set_DirectDraw_Palette();
					HasPaletteChanged = false;
				}
				if (Subtitles != NULL) {
					Subtitles->FUN_006C9C60(FUN_007C3990(Handle));
				}
				if (SurfaceDrawCallback) {
					SurfaceDrawCallback();
				}
			}

			if (errval >= VQAERR_OK) {
				CurrentFrame = errval;
				CurrentFrame++;
			}
		} else {
			if (sleeping == false) {
				sleeping = true;
				if (IsOpen && !IsPaused) {
					VQA_Play(Handle, VQAMODE_PAUSE, 0);
				}
				IsPaused = true;
				DebugString("Movie is sleeping\n");
			}
		}

		if (!nobreakout && Keyboard->Check() && Keyboard->Get() == (KN_ESC|WWKEY_RLS_BIT)) {
			brokeout = true;
		}
	}

	if (brokeout) {
		return(VQA_PLAY_USER_BREAK);
	}
	return(VQA_PLAY_NO_ERROR);
}

// FUNCTION: 0x00759BC0
void VQAClass::Play_VQA_Frame(int frame_number)
{
	if (frame_number == PLAY_LAST_FRAME) {
		frame_number = TotalFrames - 1;
	}

	if ((unsigned)CurrentFrame != (unsigned)frame_number) {
		Seek_To_Frame(frame_number);
		CurrentFrame = frame_number;
	}

	Play_VQA(frame_number, 0);

	if ((unsigned)CurrentFrame >= (unsigned)TotalFrames) {
		CurrentFrame = 0;
	}
}

// FUNCTION: 0x00759C30
bool VQAClass::Advance_Frame(bool & finished)
{
	static bool _state = false;
	finished = false;

	if (!IsAdvanceReady) {
		VQA_SetStop(Handle, TotalFrames);
		if (PrimaryColorMode != -1) {
			long blockw;
			long blockh;
			long cmode;
			VQA_GetBlockInfo(Handle, blockw, blockh, cmode);
			if (cmode == 1 || cmode == 4) {
				Hicolor_Init_Table(PrimaryColorMode);
				if (cmode == 1) {
					VQA_SetUnVQ(Handle, ASM_UnVQ1_C1_TABLE, UnVQ2_4x4_Table);
				}
			}
		}
		DebugString("VQAClass::Advance_Frame() - Ready\n");
		IsAdvanceReady = true;
	}

	if (IsPaused == true) {
		if (!Session.Play && SpecialDialog == SDLG_NONE && GameInFocus == true) {
			IsPaused = false;
			DebugString("VQAClass::Advance_Frame() - Unpause\n");
		}
	}

	if (GameInFocus == true && !IsPaused) {
		int res = VQA_Play(Handle, VQAMODE_WALK, 0);
		if (res >= VQAERR_OK) {
			if (HasPaletteChanged) {
				Set_DirectDraw_Palette();
				HasPaletteChanged = false;
			}
			if (Subtitles != NULL) {
				Subtitles->FUN_006C9C60(FUN_007C3990(Handle));
			}
			if (SurfaceDrawCallback) {
				SurfaceDrawCallback();
			}
			CurrentFrame = res + 1;
			return(true);
		}

		if (res >= VQAERR_SLEEPING && (res <= VQAERR_NOT_TIME || res == VQAERR_NONE)) {
			finished = false;
		} else {
			finished = true;
		}

		return(false);
	}

	if (!_state) {
		Pause_VQA();
		_state = true;
		DebugString("VQAClass::Advance_Frame() - Pause\n");
	}
	return(false);
}

// FUNCTION: 0x00759E10
void VQAClass::Pause_VQA(void)
{
	if (IsOpen) {
		if (!IsPaused) {
			VQA_Play(Handle, VQAMODE_PAUSE, 0);
		}
	}
	IsPaused = true;
}

// FUNCTION: 0x00759E50
void VQAClass::Resume_VQA(void)
{
	IsPaused = false;
}

// FUNCTION: 0x00759E60
void VQAClass::Close_And_Free_VQA(void)
{
	if (IsOpen) {
		VQA_Close(Handle);
	}
}

// FUNCTION: 0x00759E80
void VQAClass::Reset_VQA(void)
{
	VQA_Reset(Handle);
	if (Subtitles != NULL) {
		Subtitles->FUN_006C9D20();
	}
}

// FUNCTION: 0x0075A2D0
long             VQACacheHandler(VQAHandle * vqa, long action, void * buffer, long nbytes)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;
	VQAConfig *config = &vqap->Config;
	VQAClass *_this = config->Owner;

	return(_this->CacheHandler(action, buffer, nbytes));
}

// FUNCTION: 0x0075A2F0
long __cdecl VQAMemoryHandler(VQAHandle * vqa, long action, void * buffer, long nbytes)
{
	long error = 0;

	switch (action) {
		case VQAMEM_ALLOC:
			error = (int)malloc(nbytes);
			break;

		case VQAMEM_FREE:
			free(buffer);
			error = 0;
			break;

		case VQAMEM_LOCK:
		case VQAMEM_UNLOCK:
			error = (int)buffer;
			break;

		default:
			error = 0;
			break;

		case VQAMEM_QUERYSIZE:
			*((int *)buffer) = -1;
			error = 0;
			break;
	}
	return(error);
}
