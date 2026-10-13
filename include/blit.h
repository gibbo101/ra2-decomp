// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "blitblit.h"
#include "rect.h"
#include "surface.h"

#include "zgrad.hh"

bool Bit_Blit(Surface & dest, Rect const & destrect, Surface const & source, Rect const & sourcerect, Blitter const & blitter, int zdepth = 0, ZGradientType zgrad = ZGRAD_135DEG, int alpha = 1000, int = 0);
bool RLE_Blit(Surface & dest, Rect const & destrect, Surface const & source, Rect const & sourcerect, RLEBlitter const & blitter, int zdepth = 0, ZGradientType zgrad = ZGRAD_135DEG, int alpha = 1000, int = 0);

bool Bit_Blit(Surface & dest, Rect const & dcliprect, Rect const & ddrect, Surface const & source, Rect const & scliprect, Rect const & ssrect, Blitter const & blitter, int zdepth = 0, ZGradientType zgrad = ZGRAD_135DEG, int alpha = 1000, int = 0, int = 0);
bool RLE_Blit(Surface & dest, Rect const & dcliprect, Rect const & ddrect, Surface const & source, Rect const & scliprect, Rect const & ssrect, RLEBlitter const & blitter, int zdepth = 0, ZGradientType zgrad = ZGRAD_135DEG, int alpha = 1000, int = 0, Surface * zshape = NULL, Point2D zpoint = Point2D(0, 0), int = 0);

int Buffer_Size(Surface & surface, int width, int height);
bool To_Buffer(Surface const & surface, Rect const & rect, Buffer & buffer);
bool From_Buffer(Surface & surface, Rect const & rect, Buffer const & buffer);
