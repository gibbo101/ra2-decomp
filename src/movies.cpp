// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "movies.h"
#include "_map.h"
#include "_surface.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "surface.h"
#include "theme.h"
#include "vqa.h"
#include "vqoption.h"
#include "win.h"
#include <cmath>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EE0A0;
extern const double DAT_007EE0A8;

struct DatShorts { short X; short Y; DatShorts(short x, short y) : X(x), Y(y) {} };
struct DatInts { int X; int Y; int Z; DatInts(int x, int y, int z) : X(x), Y(y), Z(z) {} };

// FUNCTION: 0x005BF5E0 $E66
double DAT_00ABF380 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005BF610 $E69
double DAT_00ABF3E8 = DAT_00ABF380 / DAT_007EE0A0;

// FUNCTION: 0x005BF630 $E72
double DAT_00ABF3D0 = DAT_007E1718 / DAT_00ABF3E8;

// FUNCTION: 0x005BF650 $E75
double DAT_00ABF3C8 = DAT_007E1720 * DAT_007EE0A8;

// FUNCTION: 0x005BF670 $E78
double DAT_00ABF388 = DAT_007E1728 * DAT_007EE0A8;

// FUNCTION: 0x005BF690 $E81
double DAT_00ABF360 = DAT_007E1730 * DAT_007EE0A8;

// FUNCTION: 0x005BF6B0 $E84
int DAT_00ABF3F0 = (int)(DAT_007E1738 * (DAT_00ABF380 * fastmath::tan(DAT_00ABF360 - DAT_00ABF388)));

// FUNCTION: 0x005BF6F0 $E87
double DAT_00ABF3A8 = fastmath::atan(DAT_00ABF3F0 * DAT_007E1740);

// FUNCTION: 0x005BF720 $E90
double DAT_00ABF3C0 = fastmath::atan((DAT_00ABF3F0 * 2) / DAT_00ABF380);

// FUNCTION: 0x005BF750 $E93
int DAT_00ABF3E4 = (int)((DAT_00ABF3F0 * 4) + DAT_007E1738);

// FUNCTION: 0x005BF780 $E96
DatShorts DAT_00ABF378(0, 0);

// FUNCTION: 0x005BF790 $E99
DatInts DAT_00ABF3B0(0, 0, 0);

int MovieInt1 = 0;

extern int FUN_00407000(void);

struct DatInts3 { int X; int Y; int Z; DatInts3(int x, int y, int z) : Z(z), X(x), Y(y) {} };

// FUNCTION: 0x005BF7B0 $E102
DatInts3 DAT_00ABF3D8(0x80, 0x80, 0);

// FUNCTION: 0x005BF7D0 $E107
// FUNCTION: 0x005BF810 $E105
DynamicVectorClass<char const *> DAT_00ABF390;

// FUNCTION: 0x005BF850
void * Movie_Lock_Surface(void)
{
	Surface *surf = CurrentVQ->DrawSurface;
	void *buffptr = surf->Lock();
	VQAClass * vqa = CurrentVQ->VQA;

	vqa->Set_Draw_Buffer(0,
		surf->Stride() / surf->Bytes_Per_Pixel(),
		surf->Get_Height(),
		CurrentVQ->InitialRect.X,
		CurrentVQ->InitialRect.Y
	);

	return(buffptr);
}

// FUNCTION: 0x005BF8B0
bool Movie_Unlock_Surface(void)
{
	return(CurrentVQ->DrawSurface->Unlock());
}

// FUNCTION: 0x005BF8C0
void Movie_Blit_To_Screen(void)
{
	RECT rect;

	if (!Get_Option(OPTION_NO_BUFFER)) {
		GetClientRect(MainWindow, &rect);
		ClientToScreen(MainWindow, (LPPOINT)&rect);

		Rect area;
		area.Set(rect.left + CurrentVQ->StretchRect.X, rect.top + CurrentVQ->StretchRect.Y,
			CurrentVQ->StretchRect.Width, CurrentVQ->StretchRect.Height);

		VisibleSurface->Blit_From(
			area,
			*CurrentVQ->DrawSurface,
			CurrentVQ->InitialRect
		);
	}
}

// FUNCTION: 0x005BF950
void Movie_Blit_Subtitled(void)
{
	RECT rect;

	if (!Get_Option(OPTION_NO_BUFFER)) {
		HiddenSurface->Blit_From(CurrentVQ->InitialRect, *CurrentVQ->DrawSurface, CurrentVQ->InitialRect);

		if (CurrentVQ->Subtitles != NULL && CurrentVQ->Subtitles->Active != 0) {
			Surface *sub = CurrentVQ->Subtitles->SubSurface;
			Rect src = sub->Get_Rect();
			Rect dest;
			dest.X = (HiddenSurface->Get_Width() - src.Width) / 2;
			dest.Y = HiddenSurface->Get_Height() - src.Height;
			dest.Width = src.Width;
			dest.Height = src.Height;
			HiddenSurface->Blit_From(dest, *sub, src, true, true);
		}

		GetClientRect(MainWindow, &rect);
		ClientToScreen(MainWindow, (LPPOINT)&rect);

		Rect area;
		area.Set(rect.left + CurrentVQ->StretchRect.X, rect.top + CurrentVQ->StretchRect.Y,
			CurrentVQ->StretchRect.Width, CurrentVQ->StretchRect.Height);

		VisibleSurface->Blit_From(
			area,
			*HiddenSurface,
			CurrentVQ->InitialRect
		);
	}
}

// FUNCTION: 0x005BFAA0
VQHandle * Movie_Create(char const * name, Surface * surface, Rect rect1, Rect rect2, int volume, bool fullscreen)
{
	VQA_SURF_DRAW_CALLBACK callback = NULL;
	VQHandle *handle = new VQHandle;
	if (handle != NULL) {
		if (!CCFileClass(name).Is_Available()) {
			return(NULL);
		}
		int flags = 0;

		if (!FUN_00407000()) {
			Set_Option(OPTION_NO_AUDIO);
		}

		if (Get_Option(OPTION_PLAY_FROM_MIXFILE)) {
			flags |= VQACF_PLAY_FROM_MIXFILE;
		}

		if (MovieInt1 == 1) {
			flags |= VQACF_2;
		}

		if (fullscreen == true) {
			callback = Movie_Blit_Subtitled;
		}

		handle->VQA = new VQAClass(name, flags, Movie_Lock_Surface, Movie_Unlock_Surface, callback);
		if (handle->VQA == NULL) {
			delete handle;
			return(NULL);
		}

		handle->VQA->Set_VQA_Volume(volume);

		if (!handle->VQA->Open_And_Load_Buffers()) {
			delete handle;
			return(NULL);
		}

		int width = handle->VQA->Get_VQA_Width();
		int height = handle->VQA->Get_VQA_Height();
		int cmode = handle->VQA->Get_Desired_Color_Mode();

		if (cmode == 4) {
			cmode = 1;
		}

		if (rect1 == Rect(0,0,0,0)) {
			rect1 = Rect(Point2D(0, 0), width, height);
		}

		if (rect2 == Rect(0,0,0,0)) {
			rect2 = Rect((surface->Get_Width() - width) / 2, (surface->Get_Height() - height) / 2, width, height);
		}

		Rect irect1(0, 0, width, height);
		Rect irect2(0, 0, surface->Get_Width(), surface->Get_Height());

		handle->InitialRect = Intersect(rect1, irect1);
		handle->StretchRect = Intersect(rect2, irect2);

		if (cmode == 1 || cmode == 4) {
			handle->VQA->Set_Primary_Color_Mode(DSurface::Get_Primary_Color_Mode());
		}

		handle->DrawSurface = surface;
		handle->VQA->Set_Draw_Buffer(NULL, surface->Stride() / surface->Bytes_Per_Pixel(), surface->Get_Height());
		handle->Subtitles = Subtitles_Open(name);
		if (handle->Subtitles != NULL) {
			handle->VQA->Subtitles = handle->Subtitles;
		}
		handle->IsInitialized = true;
		return(handle);
	}
	return(NULL);
}

// FUNCTION: 0x005BFF00
void Movie_Destroy(VQHandle * handle)
{
	if (handle != NULL) {
		if (handle->VQA != NULL) {
			handle->VQA->Close_And_Free_VQA();
			delete handle->VQA;
			handle->VQA = NULL;
		}
		if (handle->Subtitles != NULL) {
			delete handle->Subtitles;
			handle->Subtitles = NULL;
		}
		handle->IsInitialized = false;
	}
}

// FUNCTION: 0x005BFF60
void Movie_Play(VQHandle *movie, bool hide_mouse, ThemeType theme, bool user_break_not_allowed)
{
	if (CurrentVQ == NULL) {
		movie->IsInitialized = true;
		CurrentVQ = movie;
		int start_frame = 0;
		int end_frame = -1;
		int iterations = 0;
		if (Get_Option(OPTION_USE_LOOPS)) {
			start_frame = movie->StartFrame;
			if (movie->EndFrame >= 0) {
				end_frame = movie->EndFrame;
			}
			iterations = movie->Iterations;
			if (movie->PlayMode == 1) {
				if (movie->LoopMode == 0) {
					movie->VQA->Set_Loop(movie->LoopID, movie->Iterations);
				} else {
					movie->VQA->Set_Loop(start_frame, end_frame, movie->Iterations);
				}
			}
		}

		if (hide_mouse) {
			Hide_Mouse();
		}

		if (theme != THEME_NONE) {
			Theme.Queue_Song(theme);
		}

		while (true) {
			if (movie->PlayMode == 2 || start_frame != 0) {
				movie->VQA->Seek_To_Frame(start_frame);
			}
			int ret = movie->VQA->Play_VQA(end_frame, user_break_not_allowed);
			if (movie->PlayMode == 1) {
				iterations = 0;
			}
			movie->VQA->Reset_VQA();
			if (ret == 1) {
				break;
			}
			if (iterations > 0) {
				iterations--;
			}
			if (iterations == 0) {
				break;
			}
		}

		if (hide_mouse) {
			Show_Mouse();
		}

		CurrentVQ = NULL;
	}
}

// FUNCTION: 0x005C0080
bool Movie_Advance_Frame(VQHandle * handle, bool &finished)
{
	if (CurrentVQ != NULL) {
		return(false);
	}
	CurrentVQ = handle;
	bool res = handle->VQA->Advance_Frame(finished);
	CurrentVQ = NULL;
	return(res);
}

// FUNCTION: 0x005C00B0
bool Movie_Is_Playing(void)
{
	return(CurrentVQ != NULL);
}

// FUNCTION: 0x005C00C0
void Movie_Pause(VQHandle * handle)
{
	if (handle != NULL) {
		if (handle->VQA != NULL) {
			handle->VQA->Pause_VQA();
		}
	}
}

// FUNCTION: 0x005C00D0
void Movie_Resume(VQHandle * handle)
{
	if (handle != NULL) {
		if (handle->VQA != NULL) {
			handle->VQA->Resume_VQA();
		}
	}
}

// FUNCTION: 0x005C00E0
void Movie_Update_Visible_Surface(void)
{
	if (CurrentVQ != NULL) {
		AlternateSurface->Fill(0);
		Update_Visible_Surface(false, AlternateSurface);
		Movie_Blit_To_Screen();
	}
}

// FUNCTION: 0x005C01A0
VQHandle::VQHandle(void) :
	VQA(NULL),
	field_4(-1),
	DrawSurface(NULL),
	PlayMode(0),
	StartFrame(0),
	EndFrame(0),
	LoopMode(0),
	Iterations(1),
	LoopID(0),
	InitialRect(0,0,0,0),
	StretchRect(0,0,0,0),
	field_44(false),
	IsInitialized(false),
	Subtitles(NULL)
{
}
