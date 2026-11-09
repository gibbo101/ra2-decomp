// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <windows.h>
#include <string.h>
#include "movies.h"
#include "subtitle.h"
#include "surface.h"
#include "dsurface.h"
#include "_surface.h"
#include "keyboard.h"
#include "globals.h"
#include "gscreen.h"
#include "rawfile.h"
#include "mixfile.h"
#include "goptions.h"

extern "C" {
__declspec(dllimport) BinkInfo * __stdcall BinkOpen(void const *, unsigned int);
__declspec(dllimport) int __stdcall BinkSetSoundSystem(void *, unsigned int);
__declspec(dllimport) void * __stdcall BinkOpenDirectSound(unsigned int);
__declspec(dllimport) int __stdcall BinkSetVolume(BinkInfo *, int);
__declspec(dllimport) int __stdcall BinkDDSurfaceType(void *);
__declspec(dllimport) int __stdcall BinkDoFrame(BinkInfo *);
__declspec(dllimport) void __stdcall BinkNextFrame(BinkInfo *);
__declspec(dllimport) void __stdcall BinkClose(BinkInfo *);
__declspec(dllimport) void __stdcall BinkGoto(BinkInfo *, unsigned int, int);
__declspec(dllimport) int __stdcall BinkPause(BinkInfo *, int);
__declspec(dllimport) int __stdcall BinkWait(BinkInfo *);
__declspec(dllimport) char * __stdcall BinkGetError(void);
__declspec(dllimport) int __stdcall BinkCopyToBuffer(BinkInfo *, void *, int, unsigned int, unsigned int, unsigned int, unsigned int);
}

namespace Game { extern bool IsFocused; extern HWND hWnd; }

void __cdecl FUN_004068E0(char const *, ...);
extern int MessageLoopCount;
int FUN_00407000(void);
unsigned int FUN_0040A7A0(void);
extern float BinkVolume;

class InputManagerClass : public KeyboardClass {
	public:
		static InputManagerClass * Instance;
};

int __fastcall Peek_Windows_Message(MSG * msg, HWND window, unsigned int filter_min, unsigned int filter_max, unsigned int remove);
int __fastcall Get_Windows_Message(MSG * msg, HWND window, unsigned int filter_min, unsigned int filter_max);

// FUNCTION: GAMEMD 0x00432690
BinkHandle::BinkHandle(char const * name)
{
	Flag = false;
	Bink = NULL;
	DrawSurface = NULL;
	Subtitles = NULL;
	Finished = false;
	FileHandle = INVALID_HANDLE_VALUE;
	Playing = true;
	FUN_00432750(name);
}

// FUNCTION: GAMEMD 0x004326C0
BinkHandle::BinkHandle(char const * name, Surface * surface)
{
	Flag = false;
	DrawSurface = surface;
	Bink = NULL;
	Subtitles = NULL;
	FileHandle = INVALID_HANDLE_VALUE;
	Playing = true;
	Finished = false;
	FUN_00432750(name);
}

// FUNCTION: GAMEMD 0x00432700
BinkHandle::~BinkHandle(void)
{
	if (Subtitles != NULL) {
		delete Subtitles;
		Subtitles = NULL;
	}
	if (Bink != NULL) {
		BinkClose(Bink);
		Bink = NULL;
	}
	if (FileHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(FileHandle);
		FileHandle = INVALID_HANDLE_VALUE;
	}
}

// FUNCTION: GAMEMD 0x00432750
bool BinkHandle::FUN_00432750(char const * name)
{
	FUN_00432A60();
	StartFrame = 0;
	if (FUN_00407000()) {
		BinkSetSoundSystem(BinkOpenDirectSound, FUN_0040A7A0());
	}
	bool exists = (RawFileClass(name).Is_Available(0) == 1);
	if (exists) {
		Bink = BinkOpen(name, 0);
	} else {
		int offset = 0;
		MixFileClass * mix = NULL;
		if (MixFileClass::Offset(name, NULL, &mix, &offset, NULL) == true) {
			FileHandle = CreateFileA(mix->Filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL, NULL);
			if (FileHandle != INVALID_HANDLE_VALUE) {
				SetFilePointer(FileHandle, offset, NULL, FILE_BEGIN);
				Bink = BinkOpen(FileHandle, 0x800000);
			}
		}
	}
	if (Bink == NULL) {
		FUN_004068E0("Bink Error: %s\n", BinkGetError());
		return false;
	}
	memcpy(&BinkVolume, &Options.VoiceVolume, sizeof(BinkVolume));
	BinkSetVolume(Bink, (int)(BinkVolume * 32768.0f));
	FrameDelay = 60 / (Bink->FrameRate / Bink->FrameRateDiv);
	Subtitles = Subtitles_Open(name);
	Rect draw(0, 0, Bink->Width, Bink->Height);
	if (DrawSurface == NULL) {
		if (Subtitles != NULL && Subtitles->mSubTitles != NULL) {
			DrawSurface = HiddenSurface;
			draw.X = (DrawSurface->Get_Width() - draw.Width) / 2;
			draw.Y = (DrawSurface->Get_Height() - draw.Height) / 2;
		} else {
			DrawSurface = VisibleSurface;
			RECT rc;
			GetClientRect(Game::hWnd, &rc);
			draw.X = (rc.right - rc.left - draw.Width) / 2;
			draw.Y = (rc.bottom - rc.top - draw.Height) / 2;
		}
	}
	DestRect = Intersect(DrawSurface->Get_Rect(), draw);
	CopyFlags = BinkDDSurfaceType(((DSurface *)VisibleSurface)->Get_DD_Surface());
	return true;
}

// FUNCTION: GAMEMD 0x00432A60
void BinkHandle::FUN_00432A60(void)
{
	if (Subtitles != NULL) {
		delete Subtitles;
		Subtitles = NULL;
	}
	if (Bink != NULL) {
		BinkClose(Bink);
		Bink = NULL;
	}
	if (FileHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(FileHandle);
		FileHandle = INVALID_HANDLE_VALUE;
	}
}

// FUNCTION: GAMEMD 0x00432AB0
void BinkHandle::FUN_00432AB0(int x, int y)
{
	DestRect = Intersect(DrawSurface->Get_Rect(), Rect(x, y, Bink->Width, Bink->Height));
}

// FUNCTION: GAMEMD 0x00432BD0
void BinkHandle::FUN_00432BD0(int frame)
{
	BinkGoto(Bink, frame, 1);
	StartFrame = 0;
}

// FUNCTION: GAMEMD 0x00432BF0
int BinkHandle::Frame_Time(void)
{
	return 1000 / (Bink->FrameRate / Bink->FrameRateDiv);
}

// FUNCTION: GAMEMD 0x00432C10
void BinkHandle::Set_Subtitle_Callback(void (__fastcall * callback)(void *))
{
	if (Subtitles != NULL) {
		Subtitles->SetCallback(callback);
	}
}

// FUNCTION: GAMEMD 0x00432C30
void BinkHandle::FUN_00432C30(bool pause)
{
	BinkPause(Bink, pause);
}

// FUNCTION: GAMEMD 0x00432C50
bool BinkHandle::FUN_00432C50(void)
{
	return (Bink->FrameNum >= Bink->Frames || Bink->FrameNum < StartFrame);
}

// FUNCTION: GAMEMD 0x00432C70
void BinkHandle::FUN_00432C70(void)
{
	if (Bink == NULL) {
		FUN_004068E0("Bink Error: %s\n", BinkGetError());
		return;
	}
	InputManagerClass::Instance->Clear();
	Playing = true;
	Finished = false;
	for (;;) {
		if (FUN_00432C50()) {
			break;
		}
		MessageLoopCount++;
		MSG msg;
		if (Peek_Windows_Message(&msg, 0, 0, 0, 0)) {
			if (!Get_Windows_Message(&msg, 0, 0, 0)) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		if (!Playing) {
			Sleep(33);
		}
		if (DrawSurface == VisibleSurface) {
			RECT rc;
			GetClientRect(Game::hWnd, &rc);
			ClientToScreen(Game::hWnd, (POINT *)&rc);
			int px = DestRect.X + rc.left;
			int py = DestRect.Y + rc.top;
			FUN_00432E40(DrawSurface, px, py);
		} else if (FUN_00432E40(DrawSurface, DestRect.X, DestRect.Y)) {
			RECT rc;
			GetClientRect(Game::hWnd, &rc);
			ClientToScreen(Game::hWnd, (POINT *)&rc);
			Rect dest = DestRect;
			dest.X += rc.left;
			dest.Y += rc.top;
			VisibleSurface->Blit_From(dest, *DrawSurface, DestRect, false, true);
		}
		if (InputManagerClass::Instance->Check() != KN_NONE && Flag == false) {
			if (InputManagerClass::Instance->Get() == (KN_ESC | KN_RLSE_BIT)) {
				break;
			}
		}
	}
	InputManagerClass::Instance->Clear();
}

// FUNCTION: GAMEMD 0x00432E40
bool BinkHandle::FUN_00432E40(Surface * surface, int x, int y)
{
	if (BinkVolume != Options.VoiceVolume) {
		memcpy(&BinkVolume, &Options.VoiceVolume, sizeof(BinkVolume));
		BinkSetVolume(Bink, (int)(BinkVolume * 32768));
	}
	if (!Game::IsFocused) {
		if (Playing == true) {
			FUN_004068E0("Pause bink movie\n");
			Playing = false;
			BinkPause(Bink, 1);
		}
	} else if (!Playing) {
		FUN_004068E0("Resume bink movie\n");
		Playing = true;
		BinkPause(Bink, 0);
		if (DrawSurface == VisibleSurface) {
			RECT rc;
			GetClientRect(Game::hWnd, &rc);
			ClientToScreen(Game::hWnd, (POINT *)&rc);
			int px = DestRect.X + rc.left;
			int py = DestRect.Y + rc.top;
			Copy_Frame(DrawSurface, px, py, true);
		} else {
			Copy_Frame(DrawSurface, DestRect.X, DestRect.Y, true);
		}
	}
	bool ready = Finished;
	if (!ready) {
		ready = BinkWait(Bink) == 0;
	}
	if (!ready) {
		return false;
	}
	do {
		BinkDoFrame(Bink);
		Finished = false;
		bool subtitle = false;
		if (Subtitles != NULL) {
			subtitle = Subtitles->Process(Bink->FrameNum * FrameDelay);
			if (subtitle) {
				Erase_Subtitle(surface);
			}
		}
		void * buffer = surface->Lock(Point2D(0, 0));
		if (buffer != NULL) {
			int pitch = surface->Stride();
			int height = surface->Get_Height();
			unsigned int flags = CopyFlags;
			if (subtitle) {
				flags |= 0x80000000;
			}
			BinkCopyToBuffer(Bink, buffer, pitch, height, x, y, flags);
			surface->Unlock();
		}
		Draw_Subtitle(surface);
		StartFrame = Bink->FrameNum;
		BinkNextFrame(Bink);
	} while (BinkWait(Bink) == 0);
	return true;
}

// FUNCTION: GAMEMD 0x00433020
void BinkHandle::Check_Frame_Ready(void)
{
	if (BinkWait(Bink) == 0) {
		Finished = true;
	}
}

// FUNCTION: GAMEMD 0x00433040
bool BinkHandle::Render_Frame(void)
{
	return FUN_00432E40(DrawSurface, DestRect.X, DestRect.Y);
}

// FUNCTION: GAMEMD 0x00433060
void BinkHandle::FUN_00433060(void)
{
	if (DrawSurface == VisibleSurface) {
		RECT rc;
		GetClientRect(Game::hWnd, &rc);
		ClientToScreen(Game::hWnd, (POINT *)&rc);
		int x = DestRect.X + rc.left;
		int y = DestRect.Y + rc.top;
		Redraw_Frame(DrawSurface, x, y);
	} else {
		Redraw_Frame(DrawSurface, DestRect.X, DestRect.Y);
	}
}

// FUNCTION: GAMEMD 0x00433180
void BinkHandle::Redraw_Frame(Surface * surface, int x, int y)
{
	Erase_Subtitle(surface);
	void * buffer = surface->Lock(Point2D(0, 0));
	if (buffer != NULL) {
		int pitch = surface->Stride();
		int height = surface->Get_Height();
		BinkCopyToBuffer(Bink, buffer, pitch, height, x, y, CopyFlags | 0x80000000);
		surface->Unlock();
	}
	Draw_Subtitle(surface);
}

// FUNCTION: GAMEMD 0x004331F0
void BinkHandle::Copy_Frame(Surface * surface, int x, int y, bool subtitle)
{
	if (subtitle) {
		Erase_Subtitle(surface);
	}
	void * buffer = surface->Lock(Point2D(0, 0));
	if (buffer != NULL) {
		int pitch = surface->Stride();
		int height = surface->Get_Height();
		unsigned int flags = CopyFlags;
		if (subtitle) {
			flags |= 0x80000000;
		}
		BinkInfo * bink = Bink;
		BinkCopyToBuffer(bink, buffer, pitch, height, x, y, flags);
		surface->Unlock();
	}
	Draw_Subtitle(surface);
}

// FUNCTION: GAMEMD 0x00433270
void BinkHandle::Draw_Subtitle(Surface * surface)
{
	MovieSubtitles * subtitles = Subtitles;
	if (subtitles != NULL && subtitles->mSubTitles != NULL) {
		Rect * caption = &subtitles->CaptionRect;
		if (caption->Width > 0 && caption->Height > 0) {
			Surface * image = subtitles->SubSurface;
			Rect size = image->Get_Rect();
			Rect dest = *caption;
			int dx = (DestRect.Width - size.Width) / 2;
			int dy = DestRect.Height - size.Height;
			dest.X += dx;
			dest.Y += dy;
			dest.X += DestRect.X;
			dest.Y += DestRect.Y;
			surface->Blit_From(dest, *image, *caption, true, true);
		}
	}
}

// FUNCTION: GAMEMD 0x00433330
void BinkHandle::Erase_Subtitle(Surface * surface)
{
	if (Subtitles != NULL && Subtitles->mSubTitles != NULL) {
		Rect rect = Subtitles->PreviousRect;
		if (rect.Width > 0 && rect.Height > 0) {
			Rect size = Subtitles->SubSurface->Get_Rect();
			int dx = (DestRect.Width - size.Width) / 2;
			int dy = DestRect.Height - size.Height;
			rect.X += dx;
			rect.Y += dy;
			rect.X += DestRect.X;
			rect.Y += DestRect.Y;
			surface->Fill_Rect(rect, 0);
		}
	}
}

// FUNCTION: GAMEMD 0x004333E0
int BinkHandle::Current_Time(void)
{
	return Bink->FrameNum * FrameDelay;
}

// FUNCTION: GAMEMD 0x004333F0
void BinkHandle::Update_Focus(void)
{
	if (!Game::IsFocused) {
		if (Playing == true) {
			FUN_004068E0("Pause bink movie\n");
			Playing = false;
			BinkPause(Bink, 1);
		}
	} else if (!Playing) {
		FUN_004068E0("Resume bink movie\n");
		Playing = true;
		BinkPause(Bink, 0);
		FUN_00433060();
	}
}

struct BitCursor {
	unsigned char * Ptr;
	unsigned char Bit;
};

// FUNCTION: GAMEMD 0x00433560
unsigned char __fastcall Bits_Open_Read(BitCursor * cursor, unsigned char * ptr)
{
	cursor->Ptr = ptr;
	cursor->Bit = 0;
	unsigned char first = *ptr;
	first &= 1;
	return first;
}

// FUNCTION: GAMEMD 0x00433570
bool __fastcall Bits_Read(BitCursor * cursor)
{
	cursor->Bit++;
	if (cursor->Bit == 8) {
		cursor->Bit = 0;
		cursor->Ptr++;
	}
	int mask = 1 << cursor->Bit;
	int v = *cursor->Ptr;
	unsigned char r = ((*cursor->Ptr & (1 << cursor->Bit)) > 0);
	if ((*cursor->Ptr & (1 << cursor->Bit)) > 0) return 1;
	return 0;
}

// FUNCTION: GAMEMD 0x004335A0
void __fastcall Bits_Open_Write(BitCursor * cursor, unsigned char * ptr, unsigned char value)
{
	cursor->Ptr = ptr;
	cursor->Bit = 0;
	*ptr = value;
}

// FUNCTION: GAMEMD 0x004335B0
void __fastcall Bits_Write(BitCursor * cursor, unsigned char bit)
{
	cursor->Bit++;
	if (cursor->Bit == 8) {
		cursor->Bit = 0;
		cursor->Ptr++;
		*cursor->Ptr = 0;
	}
	*cursor->Ptr |= (unsigned char)(bit << cursor->Bit);
}

// FUNCTION: GAMEMD 0x004335E0
int __fastcall Bit_Stuff(unsigned char * source, unsigned char * dest, int count)
{
	BitCursor in;
	BitCursor out;
	int stuffed = 1;
	unsigned int index = 0;
	Bits_Open_Write(&out, dest, Bits_Open_Read(&in, source));
	while (--count != 0) {
		index++;
		if (index % 7 == 0) {
			Bits_Write(&out, 1);
			stuffed++;
		}
		Bits_Write(&out, Bits_Read(&in));
	}
	return stuffed;
}

// FUNCTION: GAMEMD 0x004336C0
unsigned int __fastcall Bit_Unstuff(unsigned char * source, unsigned char * dest, unsigned int count)
{
	BitCursor in;
	BitCursor out;
	unsigned int bytes = (count + 7) >> 3;
	Bits_Open_Write(&out, dest, Bits_Open_Read(&in, source));
	unsigned int index = 0;
	while (--count != 0) {
		index++;
		if (index % 7 == 0) {
			Bits_Read(&in);
		}
		Bits_Write(&out, Bits_Read(&in));
	}
	return bytes;
}

// FUNCTION: GAMEMD 0x00433770
unsigned int __fastcall Bit_Unstuff_String(unsigned char * source, wchar_t * dest, unsigned int count)
{
	BitCursor in;
	BitCursor out;
	Bits_Open_Write(&out, (unsigned char *)dest, Bits_Open_Read(&in, source));
	unsigned int index = 0;
	int bits = 0;
	unsigned int words = 0;
	while (words < count) {
		++index;
		if (index % 7 == 0) Bits_Read(&in);
		Bits_Write(&out, Bits_Read(&in));
		if (++bits % 16 == 0) {
			++words;
			if (*(wchar_t *)(out.Ptr - 2) == 0) return words;
		}
	}
	dest[count - 1] = 0;
	return words;
}
