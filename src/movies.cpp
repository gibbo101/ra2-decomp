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
