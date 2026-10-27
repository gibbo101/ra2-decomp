// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "draw.h"

#include "blit.h"
#include "bsurface.h"
#include "convert.h"
#include "msanim.h"
#include "shapeset.h"

#include "zgrad.hh"

#include <math.h>


// SYNTHETIC: GAMEMD 0x004AED50 SYMBOL
// _$E2

// FUNCTION: GAMEMD 0x004AED70
void Draw_Shape(Surface & surface, ConvertClass & convert, ShapeSet const * shapefile, int shapenum, Point2D const & point, Rect const & window, ShapeFlags_Type flags, unsigned char const * remap, int height_offset, ZGradientType zgrad, int intensity, unsigned short tint_color, ShapeSet const * z_shapefile, int z_shapenum, Point2D z_off)
{
	SHPStruct * shp = (SHPStruct *)shapefile;
	if (shp == NULL) return;
	if (shp->Flags == -1) {
		SHPReference * ref = (SHPReference *)shp;
		if (!ref->Loaded) ref->Load();
		shp = ref->Data;
	}
	if (shp == NULL) return;

	Rect surfrect = surface.Get_Rect();
	Rect bounds = window;
	bounds = Intersect(window, surfrect);
	if (bounds.Width <= 0 || bounds.Height <= 0) return;

	convert.Set_Remap(remap);
	int x = point.X;
	int y = point.Y;
	Rect rect = shp->GetFrameBounds(shapenum);
	void const * buffer = shp->GetPixels(shapenum);
	int width = shp->Get_Width();
	int height = shp->Get_Height();
	BSurface const shape(rect.Width, rect.Height, 1, (void *)buffer);

	Point2D zpoint = z_off;
	BSurface * z_shape;
	SHPStruct * zshp = (SHPStruct *)z_shapefile;
	Rect z_rect;

	if (zshp != NULL) {
		z_rect = zshp->GetFrameBounds(z_shapenum);
		z_shape = new BSurface(z_rect.Width, z_rect.Height, 1, zshp->GetPixels(z_shapenum));
	} else {
		z_shape = NULL;
		z_rect = RECT_NONE;
	}

	if (flags & SHAPE_CENTER) {
		x -= width / 2;
		y -= height / 2;
	}

	if (zshp != NULL) {
		zpoint -= ((Point2D(width, height) / 2) - Point2D(rect.X, rect.Y));
	}

	x += rect.X;
	y += rect.Y;
	rect.X = 0;
	rect.Y = 0;

	if (zshp != NULL) {
		zpoint += z_rect.Top_Left();
		Rect zr(-zpoint.X, -zpoint.Y, z_rect.Width, z_rect.Height);
		rect = Intersect(rect, zr);
		if (rect.Width <= 0 || rect.Height <= 0) goto done;
	}

	if (remap != NULL) {
		flags = ShapeFlags_Type(flags | SHAPE_REMAP);
	}

	if (shp->HasCompression(shapenum)) {
		RLEBlitter const * blitter = convert.RLEBlitter_From_Flags(flags);
		if (blitter != NULL) {
			RLE_Blit(surface, bounds, Rect(x, y, rect.Width, rect.Height), shape, rect, rect, *blitter, height_offset, zgrad, intensity, 0, z_shape, zpoint, tint_color);
		}
	} else {
		Blitter const * blitter = convert.Blitter_From_Flags(flags);
		if (blitter != NULL) {
			Bit_Blit(surface, bounds, Rect(x, y, rect.Width, rect.Height), shape, shape.Get_Rect(), rect, *blitter, height_offset, zgrad, intensity, tint_color, 0);
		}
	}

done:
	if (z_shape != NULL) {
		delete z_shape;
	}
}

// FUNCTION: GAMEMD 0x004AF2A0
void Blit_Block(Surface & surface, ConvertClass & convert, Surface const & source, Rect const & sourcerect, Point2D const & point, Rect const & clipwindow, unsigned char const * remap, Blitter const * blitter, int height_offset, ZGradientType zgrad, int intensity, int tint, int unknown)
{
	convert.Set_Remap(remap);

	if (blitter == NULL) {
		blitter = convert.Blitter_From_Flags((remap != NULL) ? SHAPE_REMAP : SHAPE_NORMAL);
	}

	Bit_Blit(surface, clipwindow, Rect(point.X, point.Y, sourcerect.Width, sourcerect.Height), source, source.Get_Rect(), sourcerect, *blitter, height_offset, zgrad, intensity, tint, unknown);
}
