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

class Straw;
class SubTitle;

class SubTitleParserClass
{
	public:
		SubTitleParserClass(Straw & input);
		~SubTitleParserClass(void);
		DynamicVectorClass<SubTitle *> * Get_Sub_Titles(char const * moviename);

	private:
		bool Find_Movie_Entry(char const * moviename);
		bool Parse_Sub_Title(wchar_t * string, SubTitle * subtitle);
		wchar_t * Get_Next_Line(void);

		Straw & mInput;
		wchar_t mBuffer[1024];
		unsigned int mLineNumber;
};

class MovieSubtitles
{
	public:
		MovieSubtitles(void);
		~MovieSubtitles(void);
		bool Load_Sub_Titles(char const * moviename);
		bool Process(unsigned long movietime);
		void Reset(void);
		void SetCallback(void (__fastcall * callback)(void *));
		void Draw_Sub_Title(SubTitle * subtitle);

		DynamicVectorClass<SubTitle *> * mSubTitles;
		int mSubTitleIndex;
		SubTitle * mActiveSubTitle;
		void (__fastcall * Callback)(void *);
		Surface * SubSurface;
		Rect CaptionRect;
		Rect PreviousRect;
};

MovieSubtitles * __fastcall Subtitles_Open(char const * name);

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

	MovieSubtitles * Subtitles;
};

extern VQHandle *CurrentVQ;

class MovieHandle
{
	public:
		virtual ~MovieHandle(void);
		virtual bool AdvanceFrame(void) = 0;
		virtual void Waiting(void) = 0;
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
		VQHandle * Player;
};


class VQMovieHandle : public MovieHandle
{
	public:
		VQMovieHandle(VQHandle * player);
		virtual ~VQMovieHandle(void);
		virtual bool AdvanceFrame(void);
		virtual void Waiting(void);
		virtual void Pause(bool pause);
		virtual void Stop(void);
		virtual bool FramesLeft(void);
		virtual void SetPosition(int x, int y);
		virtual void SeekToFrame(int frame);
		virtual void InitSubtitles(void (__fastcall * callback)(void *));
		virtual int Timing(void);
		virtual void Blit(void);

		bool Finished;
};

struct BinkInfo
{
	int Width;
	int Height;
	unsigned int Frames;
	unsigned int FrameNum;
	unsigned int LastFrameNum;
	unsigned int FrameRate;
	unsigned int FrameRateDiv;
};

class BinkHandle
{
	public:
		BinkHandle(char const * name);
		BinkHandle(char const * name, Surface * surface);
		~BinkHandle(void);
		bool FUN_00432750(char const * name);
		void FUN_00432A60(void);
		bool FUN_00432E40(Surface * surface, int x, int y);
		void FUN_00433180(Surface * surface, int x, int y);
		void FUN_004331F0(Surface * surface, int x, int y, bool subtitle);
		void FUN_00433270(Surface * surface);
		void FUN_00433330(Surface * surface);
		int FUN_004333E0(void);
		void FUN_004333F0(void);
		void FUN_00432C30(bool pause);
		void FUN_00432C70(void);
		bool FUN_00432C50(void);
		bool FUN_00433040(void);
		void FUN_00433020(void);
		void FUN_00432AB0(int x, int y);
		void FUN_00432C10(void (__fastcall * callback)(void *));
		void FUN_00432BD0(int frame);
		int FUN_00432BF0(void);
		void FUN_00433060(void);

		bool Flag;
		BinkInfo * Bink;
		int CopyFlags;
		Surface * DrawSurface;
		Rect DestRect;
		MovieSubtitles * Subtitles;
		int FrameDelay;
		void * FileHandle;
		bool Playing;
		bool Finished;
		unsigned int StartFrame;
};

class BinkMovieHandle : public MovieHandle
{
	public:
		BinkMovieHandle(BinkHandle * player);
		virtual ~BinkMovieHandle(void);
		virtual bool AdvanceFrame(void);
		virtual void Waiting(void);
		virtual void Pause(bool pause);
		virtual void Stop(void);
		virtual bool FramesLeft(void);
		virtual void SetPosition(int x, int y);
		virtual void SeekToFrame(int frame);
		virtual void InitSubtitles(void (__fastcall * callback)(void *));
		virtual int Timing(void);
		virtual void Blit(void);
};

extern DynamicVectorClass<MovieHandle *> IngameVQ;
