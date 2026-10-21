// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "movie.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "ccfile.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "gscreen.h"
#include "movies.h"
#include "session.h"
#include "vector.h"
#include "vqa.h"
#include "vq.hh"
#include "_xmouse.h"
#include "msglist.h"
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EE090;
extern const double DAT_007EE098;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x005BEAD0 SYMBOL
// _$E88
double DAT_00ABF300 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x005BEB00 SYMBOL
// _$E91
double DAT_00ABF350 = DAT_00ABF300 / DAT_007EE090;

// SYNTHETIC: GAMEMD 0x005BEB20 SYMBOL
// _$E94
double DAT_00ABF338 = DAT_007E1718 / DAT_00ABF350;

// SYNTHETIC: GAMEMD 0x005BEB40 SYMBOL
// _$E97
double DAT_00ABF330 = DAT_007E1720 * DAT_007EE098;

// SYNTHETIC: GAMEMD 0x005BEB60 SYMBOL
// _$E100
double DAT_00ABF308 = DAT_007E1728 * DAT_007EE098;

// SYNTHETIC: GAMEMD 0x005BEB80 SYMBOL
// _$E103
double DAT_00ABF2E0 = DAT_007E1730 * DAT_007EE098;

// SYNTHETIC: GAMEMD 0x005BEBA0 SYMBOL
// _$E106
int DAT_00ABF358 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABF2E0 - DAT_00ABF308) * DAT_00ABF300));

// SYNTHETIC: GAMEMD 0x005BEBE0 SYMBOL
// _$E109
double DAT_00ABF310 = FUN_004CADE0(DAT_00ABF358 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005BEC10 SYMBOL
// _$E112
double DAT_00ABF328 = FUN_004CADE0((DAT_00ABF358 * 2) / DAT_00ABF300);

// SYNTHETIC: GAMEMD 0x005BEC40 SYMBOL
// _$E115
int DAT_00ABF34C = (int)(DAT_00ABF358 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x005BEC70 SYMBOL
// _$E118
ShortPair DAT_00ABF2F8;

// SYNTHETIC: GAMEMD 0x005BEC80 SYMBOL
// _$E121
IntTriple DAT_00ABF318;

extern int DAT_00ABF340;
extern int DAT_00ABF344;
extern int DAT_00ABF348;
extern bool DAT_00ABF35C;
extern int DAT_008809D8;
extern int DAT_008809DC;
extern void * DAT_0087E758;
extern void * DAT_0087E750;

void __cdecl FUN_004068E0(char const *, ...);
void FUN_007535B0(void);
void FUN_00408200(void);
void FUN_00408270(void);
void __fastcall FUN_00407170(void *, int);
void FUN_00406EA0(void);
void __fastcall FUN_0040A7C0(int, int, int);
void FUN_0040A850(void);
void FUN_00406EC0(void);
void FUN_00408230(void);
void FUN_004082F0(void);
void FUN_004083D0(void);
void FUN_00753620(void);

void Movie_Audio_Resume(void);
void Movie_Audio_Suspend(void);
void __fastcall Ingame_Movie_Subtitle(void * data);

struct BinkPlayer {
	bool Flag;
	char Rest[0x33];
	BinkPlayer * FUN_00432690(char const * name);
	void FUN_00432C70(void);
	void FUN_00432700(void);
};

bool __fastcall FUN_005C0640(char const * name, char * out, bool unused);
MovieHandle * __fastcall OwnerDraw_InitMovieHandle(char const * name, Surface * surface, Rect * bounds);
void Play_Movie(char const * name, ThemeType theme, bool clrscrn_after, bool stretch, bool clrscrn_before, bool fullscreen);

// FUNCTION: GAMEMD 0x005BECA0
void Movie_Init_Tables(void)
{
	DAT_00ABF348 = 0;
	DAT_00ABF340 = 0x80;
	DAT_00ABF344 = 0x80;
}

// FUNCTION: GAMEMD 0x005BECC0
void Rebuild_Interpolated_Palette(unsigned char * interpal)
{
	for (int y=0; y<SIZE_OF_PALETTE - 1; y++) {
		for (int x=y+1; x<SIZE_OF_PALETTE; x++) {
			*(interpal + (y*SIZE_OF_PALETTE+x)) = *(interpal + (x*SIZE_OF_PALETTE+y));
		}
	}
}

// FUNCTION: GAMEMD 0x005BED20
void Play_Movie(char const * name, ThemeType theme, bool clrscrn_after, bool stretch, bool clrscrn_before)
{
	Play_Movie(name, theme, clrscrn_after, stretch, clrscrn_before, true);
}

// FUNCTION: GAMEMD 0x005BED40
void Play_Movie(char const * name, ThemeType theme, bool clrscrn_after, bool stretch, bool clrscrn_before, bool fullscreen)
{
	char buffer[256];
	if (!FUN_005C0640(name, buffer, true)) {
		return;
	}
	if (Session.Type != GAME_NORMAL) {
		return;
	}
	Keyboard->Clear();
	char * ext = strrchr(buffer, '.');
	if (ext != NULL && stricmp(ext, ".bik") == 0) {
		FUN_004068E0("Play_Movie() as Bink!\n");
		MouseCursor->Hide_Mouse();
		if (clrscrn_before) {
			HiddenSurface->Fill(0);
			Update_Visible_Surface(true, HiddenSurface);
		}
		BinkPlayer bink;
		bink.FUN_00432690(buffer);
		if (fullscreen == true) {
			bink.Flag = true;
		}
		Movie_Audio_Suspend();
		FUN_00406EA0();
		FUN_0040A7C0(0xAC44, 0x10, 2);
		bink.FUN_00432C70();
		FUN_0040A850();
		FUN_00406EC0();
		Movie_Audio_Resume();
		if (clrscrn_after == true) {
			HiddenSurface->Fill(0);
			Update_Visible_Surface(true, HiddenSurface);
		}
		Keyboard->Clear();
		MouseCursor->Show_Mouse();
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
		bink.FUN_00432700();
		return;
	}

	Keyboard->Clear();
	VQHandle * vqa = Movie_Create(buffer, AlternateSurface, Rect(0,0,0,0), Rect(0,0,0,0), int(Options.VoiceVolume * 255.0), 1);
	if (vqa != NULL) {
		if (vqa->VQA->Get_VQA_Width() < 320 && vqa->VQA->Get_VQA_Height() < 200) {
			Movie_Destroy(vqa);
			delete vqa;
			return;
		}
		bool dostretch = (stretch == true && Options.StretchMovies == true);
		if (DSurface::AllowStretchBlits == true && dostretch == true) {
			if (vqa->InitialRect.Width < VisibleRect.Width && vqa->InitialRect.Height < VisibleRect.Height) {
				float ratio = ((float)VisibleRect.Width / (float)vqa->InitialRect.Width);
				vqa->StretchRect.Width = VisibleRect.Width;
				vqa->StretchRect.Height = (int)(vqa->InitialRect.Height * ratio);
				vqa->StretchRect.X = 0;
				vqa->StretchRect.Y = (VisibleRect.Height - vqa->StretchRect.Height) / 2;
				FUN_004068E0("Stretching movie %dx%d -> %dx%d\n", vqa->InitialRect.Width, vqa->InitialRect.Height, vqa->StretchRect.Width, vqa->StretchRect.Height);
			}
		}
		MouseCursor->Hide_Mouse();
		if (clrscrn_before) {
			HiddenSurface->Fill(0);
			Update_Visible_Surface(true, HiddenSurface);
		}
		Movie_Audio_Suspend();
		FUN_00406EA0();
		FUN_0040A7C0(0xAC44, 0x10, 2);
		Movie_Play(vqa, true, theme, false);
		FUN_0040A850();
		FUN_00406EC0();
		Movie_Audio_Resume();
		Movie_Destroy(vqa);
		delete vqa;
		if (clrscrn_after == true) {
			HiddenSurface->Fill(0);
			Update_Visible_Surface(true, HiddenSurface);
		}
		MouseCursor->Show_Mouse();
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
		Keyboard->Clear();
	}
}

// FUNCTION: GAMEMD 0x005BF180
void __fastcall Play_Movie_Simple(char const * name)
{
	MovieHandle * movie = OwnerDraw_InitMovieHandle(name, HiddenSurface, NULL);
	if (movie != NULL) {
		HiddenSurface->Fill(0);
		Update_Visible_Surface(true, HiddenSurface);
		Keyboard->Clear();
		Movie_Audio_Suspend();
		movie->Stop();
		Movie_Audio_Resume();
		Keyboard->Clear();
		delete movie;
	}
}

// FUNCTION: GAMEMD 0x005BF260
void Play_Movie(VQType vq, ThemeType theme, bool clrscrn_after, bool stretch, bool clrscrn_before)
{
	if (vq >= 0 && vq < Movies.Count()) {
		bool active = TacticalActive;
		TacticalActive = false;
		Play_Movie(Movies[vq], THEME_NONE, clrscrn_after, stretch, clrscrn_before, false);
		TacticalActive = active;
	}
}

// FUNCTION: GAMEMD 0x005BF2B0
void __fastcall Play_Ingame_Movie(char const * name, bool loop)
{
	if (Session.Type == GAME_NORMAL) {
		MovieHandle * movie = OwnerDraw_InitMovieHandle(name, SidebarSurface, NULL);
		if (movie != NULL) {
			movie->SetPosition(DAT_008809D8 - 3, DAT_008809DC - 1);
			movie->InitSubtitles(Ingame_Movie_Subtitle);
			movie->State = loop;
			IngameVQ.Add(movie);
		}
	}
}

// FUNCTION: GAMEMD 0x005BF360
void __fastcall Ingame_Movie_Subtitle(void * data)
{
	if (data != NULL) {
		Session.Messages.Add_Message(NULL, 0, ((wchar_t **)data)[5], 0, (TextPrintType)0x4046, ((int *)data)[1], false);
	}
}

// FUNCTION: GAMEMD 0x005BF390
void __fastcall Play_Ingame_Movie(VQType vq, bool loop)
{
	if (vq >= 0 && vq < Movies.Count()) {
		char const * name = Movies[vq];
		if (Session.Type == GAME_NORMAL) {
			MovieHandle * movie = OwnerDraw_InitMovieHandle(name, SidebarSurface, NULL);
			if (movie != NULL) {
				movie->SetPosition(DAT_008809D8 - 3, DAT_008809DC - 1);
				movie->InitSubtitles(Ingame_Movie_Subtitle);
				movie->State = loop;
				IngameVQ.Add(movie);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005BF450
void Stop_Ingame_Movie(void)
{
	for (int i = 0; i < IngameVQ.Count(); i++) {
		if (IngameVQ[i] != NULL) {
			delete IngameVQ[i];
		}
	}
	IngameVQ.Clear();
	Movie_Audio_Resume();
}

// FUNCTION: GAMEMD 0x005BF500
void __fastcall Pause_Ingame_Movie(bool pause)
{
	if (IngameVQ.Count() > 0) {
		IngameVQ[0]->Pause(pause);
	}
}

// FUNCTION: GAMEMD 0x005BF520
bool Has_Ingame_Movies(void)
{
	return(IngameVQ.Count() > 0);
}

// FUNCTION: GAMEMD 0x005BF530
void Movie_Audio_Resume(void)
{
	if (DAT_00ABF35C) {
		FUN_00408230();
		FUN_004082F0();
		FUN_004083D0();
		FUN_00407170(DAT_0087E758, 0x4000);
		FUN_00407170(DAT_0087E750, 0x4000);
		FUN_00753620();
		DAT_00ABF35C = false;
	}
}

// FUNCTION: GAMEMD 0x005BF580
void Movie_Audio_Suspend(void)
{
	if (!DAT_00ABF35C) {
		FUN_007535B0();
		FUN_00408200();
		FUN_00408270();
		FUN_00407170(DAT_0087E758, 0x2000);
		FUN_00407170(DAT_0087E750, 0x2000);
		DAT_00ABF35C = true;
	}
}
