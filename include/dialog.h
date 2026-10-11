// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "point.h"

#include "color.hh"
#include "dialog.hh"

class FontClass;
class Surface;
class ColorScheme;
template<class T> class TRect;
typedef TRect<int> Rect;

int Format_Window_String(char * string, FontClass const * font, int maxlinelen, int & width, int & height);
Point2D Conquer_Clip_Text_Print(char const *, Surface & surface, Rect const &rect, Point2D const & pt, ColorScheme * fore, int back=TBLACK, TextPrintType flag=TextPrintType(TPF_8POINT|TPF_DROPSHADOW), int width=-1, int const * tabs=0);
void Draw_Box(Rect const & rect, BoxStyleEnum up, bool filled);
Point2D __cdecl Fancy_Text_Print(char const * text, Surface & surface, Rect const & rect, Point2D const &pt, ColorScheme * fore, int back, TextPrintType flag, ...);
Point2D __cdecl Fancy_Text_Print(int text, Surface & surface, Rect const & rect, Point2D const & pt, ColorScheme * fore, int back, TextPrintType flag, ...);
Point2D Simple_Text_Print(char const * text, Surface & surface, Rect const & rect, Point2D const & pt, ColorScheme * scheme, int back, TextPrintType flag, int fore);
Point2D Plain_Text_Print(int text, Surface & surface, Rect const &rect, Point2D const & xy, int , int back, TextPrintType flag, int scheme, int fore);
Point2D Plain_Text_Print(char const * text, Surface & surface, Rect const &rect, Point2D const & pt, int , int back, TextPrintType flag, int scheme, int fore);

FontClass *Font_From_TPF(TextPrintType flags);
unsigned char * Font_Palette(int color);
