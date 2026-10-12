// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "buff.h"
#include "rect.h"
#include "rgb.h"

class Surface
{
	public:

		Surface(int width, int height) : Width(width), Height(height) {}
		virtual ~Surface(void) {};

		virtual bool Blit_From(Rect const & dcliprect, Rect const & destrect, Surface const & source, Rect const & scliprect, Rect const & sourcerect, bool trans=false, bool unknown=true) = 0;
		virtual bool Blit_From(Rect const & destrect, Surface const & source, Rect const & sourcerect, bool trans=false, bool unknown=true) = 0;
		virtual bool Blit_From(Surface const & source, bool trans=false, bool unknown=true) = 0;

		virtual bool Fill_Rect(Rect const & rect, int color) = 0;
		virtual bool Fill_Rect(Rect const & cliprect, Rect const & fillrect, int color) = 0;
		virtual bool Fill(int color) = 0;
		virtual bool Fill_Rect_Trans(Rect const & rect, RGBClass const & color, unsigned int opacity) = 0;

		virtual bool Draw_Ellipse(Point2D point, int radius_x, int radius_y, Rect clip, int color) = 0;

		virtual bool Put_Pixel(Point2D const & point, int color) = 0;
		virtual int Get_Pixel(Point2D const & point) const = 0;

		virtual bool Draw_Line(Point2D const & startpoint, Point2D const & endpoint, int color) = 0;
		virtual bool Draw_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, int color) = 0;
		virtual bool Draw_Depth_Shaded_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, unsigned color, int start_depth, int end_depth, bool write_depth = false) = 0;
		virtual bool Draw_Depth_Glow_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, int glow_strength, int start_depth, int end_depth, bool write_depth = false) = 0;
		virtual bool Draw_Depth_Antialiased_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass & color, int start_depth, int end_depth, bool write_depth, bool blend_red, bool blend_green, bool blend_blue, float intensity) = 0;
		virtual bool DrawRGBMultiplyingLine(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass const & color, float intensity, int start_depth, int end_depth) = 0;

		virtual bool Plot_Line(Rect const &area, Point2D &start, Point2D &end, void(*drawer_callback)(Point2D const &)) = 0;

		virtual int Draw_Dashed_Line(Point2D const &start, Point2D const &end, unsigned color, bool pattern[], int offset) = 0;
		virtual int Draw_Masked_Dashed_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool pattern[], int offset, bool draw_on_zero_alpha) = 0;
		virtual bool Draw_Masked_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool draw_on_zero_alpha) = 0;

		virtual bool Draw_Rect(Rect const & rect, int color) = 0;
		virtual bool Draw_Rect(Rect const & cliprect, Rect const & rect, int color) = 0;

		virtual void * Lock(Point2D point = Point2D(0, 0)) const = 0;
		virtual bool Unlock(void) const = 0;
		virtual bool Can_Lock(int x = 0, int y = 0) const {return(true);}

		virtual bool entry_64(Point2D pt = Point2D(0, 0)) const {return(true);}
		virtual bool Is_Locked(void) const = 0;

		virtual int Bytes_Per_Pixel(void) const = 0;
		virtual int Stride(void) const = 0;
		virtual Rect Get_Rect(void) const {return(Rect(0, 0, Width, Height));}
		virtual int Get_Width(void) const {return(Width);}
		virtual int Get_Height(void) const {return(Height);}

		virtual bool Is_Direct_Draw(void) const {return(false);}

	protected:

		int Width;
		int Height;
};
