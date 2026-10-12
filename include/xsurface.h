// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "surface.h"

class XSurface : public Surface
{
		typedef Surface BASECLASS;

	public:
		XSurface(int width=0, int height=0) : BASECLASS(width, height), LockCount(0) {}

		virtual bool Blit_From(Rect const & dcliprect, Rect const & destrect, Surface const & source, Rect const & scliprect, Rect const & sourcerect, bool trans=false, bool =true) override;
		virtual bool Blit_From(Rect const & destrect, Surface const & source, Rect const & sourcerect, bool trans=false, bool unknown=true) override;
		virtual bool Blit_From(Surface const & source, bool trans=false, bool unknown=true) override;

		virtual bool Fill_Rect(Rect const & rect, int color) override;
		virtual bool Fill_Rect(Rect const & cliprect, Rect const & fillrect, int color) override;
		virtual bool Fill(int color) override;
		virtual bool Fill_Rect_Trans(Rect const & rect, RGBClass const & color, unsigned int opacity) override;

		virtual bool Draw_Ellipse(Point2D point, int radius_x, int radius_y, Rect clip, int color) override;

		void Draw_Circle(Point2D center, int radius, Rect rect, int color);

		virtual bool Put_Pixel(Point2D const & point, int color) override;
		virtual int Get_Pixel(Point2D const & point) const override;

		virtual bool Draw_Line(Point2D const & startpoint, Point2D const & endpoint, int color) override;
		virtual bool Draw_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, int color) override;

		virtual bool Draw_Depth_Shaded_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, unsigned color, int start_depth, int end_depth, bool write_depth = false) override;
		virtual bool Draw_Depth_Glow_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, int glow_strength, int start_depth, int end_depth, bool write_depth = false) override;
		virtual bool Draw_Depth_Antialiased_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass & color, int start_depth, int end_depth, bool write_depth, bool blend_red, bool blend_green, bool blend_blue, float intensity) override;
		virtual bool DrawRGBMultiplyingLine(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass const & color, float intensity, int start_depth, int end_depth) override;

		virtual bool Plot_Line(Rect const &area, Point2D &start, Point2D &end, void(*drawer_callback)(Point2D const &)) override;

		virtual int Draw_Dashed_Line(Point2D const &start, Point2D const &end, unsigned color, bool pattern[], int offset) override;
		virtual int Draw_Masked_Dashed_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool pattern[], int offset, bool draw_on_zero_alpha) override;
		virtual bool Draw_Masked_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool draw_on_zero_alpha) override;

		virtual bool Draw_Rect(Rect const & rect, int color) override;
		virtual bool Draw_Rect(Rect const & cliprect, Rect const & rect, int color) override;

		virtual void * Lock(Point2D = Point2D(0, 0)) const override {LockCount++;return(NULL);}
		virtual bool Unlock(void) const override {LockCount--;return(true);}
		virtual bool Is_Locked(void) const override {return(LockCount != 0);}

		virtual int Bytes_Per_Pixel(void) const override = 0;
		virtual int Stride(void) const override = 0;

		virtual bool Is_Direct_Draw(void) const override {return(false);}

		virtual bool Put_Pixel_Clip(Point2D const & point, int color, Rect const & rect);

		virtual int Get_Pixel_Clip(Point2D const & point, Rect const & rect) const;

		static bool Prep_For_Blit(Surface & dest, Rect & drect, Surface const & source, Rect & srect, bool & overlapped, void * & dbuffer, void * & sbuffer);
		static bool Prep_For_Blit(Surface & dest, Rect const & dcliprect, Rect & drect, Surface const & source, Rect const & scliprect, Rect & srect, bool & overlapped, void * & dbuffer, void * & sbuffer);

		static bool Blit_Trans(Surface & dest, Rect const & destrect, Surface const & source, Rect const & sourcerect);
		static bool Blit_Plain(Surface & dest, Rect const & destrect, Surface const & source, Rect const & sourcerect);

	protected:

		mutable int LockCount;
};

bool Blit_Clip(Rect & drect, Rect const & dwindow, Rect & srect, Rect const & swindow);
bool Clip_Line_To_Rect(Point2D & point1, Point2D & point2, Rect const & rect);
