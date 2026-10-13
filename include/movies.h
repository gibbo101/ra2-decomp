// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "rect.h"

#include "theme.hh"

class VQAClass;
class Surface;
struct VQHandle;
template<class T> class DynamicVectorClass;

void * Movie_Lock_Surface(void);
bool Movie_Unlock_Surface(void);
void Movie_Blit_To_Screen(void);
VQHandle * Movie_Create(char const * name, Surface * surface, Rect rect1, Rect rect2, int volume, bool fullscreen);
void Movie_Destroy(VQHandle * handle);
void Movie_Play(VQHandle *movie, bool hide_mouse, ThemeType theme, bool user_break_not_allowed);
bool Movie_Advance_Frame(VQHandle * handle, bool &finished);
bool Movie_Is_Playing(void);
void Movie_Pause(VQHandle * handle);
void Movie_Resume(VQHandle * handle);
void Movie_Queue_Ingame(VQHandle * handle);
void Movie_Update_Visible_Surface(void);

struct VQHandle
{
	VQHandle(void);
	~VQHandle(void);

	VQAClass *VQA;

	int field_4;

	Surface * DrawSurface;

	int PlayMode;

	int StartFrame;
	int EndFrame;

	int LoopMode;

	int Iterations;

	int LoopID;

	Rect InitialRect;
	Rect StretchRect;

	bool field_44;

	bool IsInitialized;

};

extern VQHandle *CurrentVQ;

class MovieHandle
{
	public:
		virtual ~MovieHandle(void) = 0;
		virtual bool AdvanceFrame(void) = 0;
		virtual bool Waiting(void) = 0;
		virtual void Pause(bool pause) = 0;
		virtual void Stop(void) = 0;
		virtual bool FramesLeft(void) = 0;
		virtual void SetPosition(int x, int y) = 0;
		virtual void SeekToFrame(int frame) = 0;
		virtual void InitSubtitles(void (__fastcall * callback)(void *)) = 0;
		virtual int Timing(void) = 0;
		virtual void Blit(void) = 0;

		bool State;
		int Width;
		int Height;
		void * Player;
};

inline MovieHandle::~MovieHandle(void) {}

extern DynamicVectorClass<MovieHandle *> IngameVQ;
