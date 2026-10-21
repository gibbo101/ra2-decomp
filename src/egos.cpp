// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "egos.h"
#include "_keyboar.h"
#include "_palette.h"
#include "_rect.h"
#include "_surface.h"
#include "_timer.h"
#include "addon.h"
#include "ccfile.h"
#include "conquer.h"
#include "dialog.h"
#include "dsurface.h"
#include "bitfont.h"
#include "font.h"
#include "gadget.h"
#include "globals.h"
#include "goptions.h"
#include "gscreen.h"
#include "language\language.h"
#include "misc.h"
#include "ownrdraw.h"
#include "scheme.h"
#include "theme.h"
#include "vector.h"
#include "windlg.h"
#include "color.hh"
#include "dialog.hh"
#include <cstring>

namespace OwnerDraw {
int __fastcall PrintTextFixedLength(unsigned color, BitFont * font, Rect const & rect, wchar_t const * text, int length, int x_alignment, int y_alignment, Surface * surface, int unknown);
}

struct Drawing { static int RenderWidth; static int RenderHeight; };
namespace Game { extern bool IsFocused; }
extern Surface * DAT_008A0F58;

// FUNCTION: 0x004C3C10 $E40
// FUNCTION: 0x004C3C50 $E38
DynamicVectorClass<EgoClass *> EgoList;

// FUNCTION: 0x004C3C90
EgoClass::EgoClass (int x, int y, wchar_t *text, TextPrintType flags)
{
	XPos = x;
	YPos = y;
	Flags= flags;
	Text = new wchar_t [wcslen (text)+1];
	wcscpy (Text, text);
}

// FUNCTION: 0x004C3CD0
EgoClass::~EgoClass(void)
{
	delete [] Text;
}

// FUNCTION: 0x004C3CE0
bool EgoClass::Scroll(int distance)
{
	YPos -= distance;
	if (YPos < -20) {
		return(true);
	}else{
		return(false);
	}
}

// FUNCTION: 0x004C3D00
void EgoClass::Render(bool fresh)
{
	if ((YPos < DSurface::Temp->Get_Height() && YPos > DSurface::Temp->Get_Height() - 52) || YPos >= -16 && YPos <= 32 || fresh) {
		Rect textrect;
		textrect.X = (Drawing::RenderWidth - 520) / 2;
		textrect.Width = 520;
		textrect.Y = YPos;
		textrect.Height = Drawing::RenderHeight;

		int alignment = 0;
		if (Flags & TPF_CENTER) {
			alignment = 1;
		}else{
			if (Flags & TPF_RIGHT){
				alignment = 4;
			}
		}

		if (Game::IsFocused) {
			OwnerDraw::PrintTextFixedLength(0x80ffff, 0, textrect, Text, wcslen(Text), alignment, 0, DAT_008A0F58, 0);
		}
	}
}

// FUNCTION: 0x004C3DD0
void EgoClass::Wipe (Surface *background)
{
	BitFont * font = Font_From_TPF(Flags);
	font->FUN_00433ED0(Text, 0);

	Rect r(0, YPos, Drawing::RenderWidth, 18);
	DSurface::Temp->Blit_From(r, *background, r);
}
