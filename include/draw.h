// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "surface.h"

#include "draw.hh"
#include "zgrad.hh"

class ConvertClass;
class ShapeSet;
class Blitter;

void Draw_Shape(Surface & surface, ConvertClass & convert, ShapeSet const * shapefile, int shapenum, Point2D const & point, Rect const & window, ShapeFlags_Type flags = SHAPE_NORMAL, unsigned char const * remap = NULL, int height_offset = 0, ZGradientType zgrad = ZGRAD_GROUND, int intensity = 1000, int tint_color = 0, ShapeSet const * z_shapefile = NULL, int z_shapenum = 0, Point2D z_off = Point2D(0,0));
void Blit_Block(Surface & surface, ConvertClass & convert, Surface const & source, Rect const & sourcerect, Point2D const & point, Rect const & clipwindow, unsigned char const * remap = NULL, Blitter const * blitter = NULL, int = 0, ZGradientType zgrad = ZGRAD_GROUND, int = 1000, int = 0);
