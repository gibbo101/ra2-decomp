// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "misc.h"
#include "palette.h"
#include "win.h"
#include "xsurface.h"

enum DSurfaceColorMode {
	COLORMODE_INVALID = -1,
	COLORMODE_555,
	COLORMODE_556,
	COLORMODE_565,
	COLORMODE_655,
};

class DSurface : public XSurface
{
		typedef XSurface BASECLASS;

	public:
		virtual ~DSurface(void) override;

		DSurface(void);

		DSurface(int width, int height, bool system_memory = false, bool device_3d = false);

		DSurface(LPDIRECTDRAWSURFACE surfaceptr);

		HDC GetDC(void);
		int ReleaseDC(HDC hdc);

		static DSurface * Create_Primary(DSurface ** backsurface1=NULL);

		virtual bool Blit_From(Rect const & dcliprect, Rect const & destrect, Surface const & source, Rect const & scliprect, Rect const & sourcerect, bool trans=false, bool unknown=true) override;
		virtual bool Blit_From(Rect const & destrect, Surface const & source, Rect const & sourcerect, bool trans=false, bool unknown=true) override;
		virtual bool Blit_From(Surface const & source, bool trans=false, bool unknown=true) override {return(BASECLASS::Blit_From(source, trans, unknown));}

		virtual bool Fill_Rect(Rect const & rect, int color) override;
		virtual bool Fill_Rect(Rect const & cliprect, Rect const & fillrect, int color) override;
		virtual bool Fill_Rect_Trans(Rect const & xcliprect, RGBClass const & color, unsigned int opacity) override;

		virtual bool Draw_Depth_Shaded_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, unsigned color, int start_depth, int end_depth, bool write_depth = false) override;
		virtual bool Draw_Depth_Glow_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, int glow_strength, int start_depth, int end_depth, bool write_depth = false) override;
		virtual bool Draw_Depth_Antialiased_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass & color, int start_depth, int end_depth, bool write_depth, bool blend_red, bool blend_green, bool blend_blue, float intensity) override;
		virtual bool DrawRGBMultiplyingLine(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass const & color, float intensity, int start_depth, int end_depth) override;

		virtual int Draw_Masked_Dashed_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool pattern[], int offset, bool draw_on_zero_alpha) override;
		virtual bool Draw_Masked_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool draw_on_zero_alpha) override;
		virtual bool Draw_Ping_Pong_Gradient_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass const & start_color, RGBClass const & end_color, float & gradient_step, float & gradient_position) const;
		bool FUN_004BEAC0(Rect const & cliprect, Point2D & startpoint, Point2D & endpoint, RGBClass & color, int alpha, int start_height, int end_height);

		virtual void * Lock(Point2D point = Point2D(0, 0)) const override;
		virtual bool Unlock(void) const override;
		virtual bool Can_Lock(int x = 0, int y = 0) const override;

		virtual int Bytes_Per_Pixel(void) const override;
		virtual int Stride(void) const override;
		bool In_Video_Ram(void) const {return(IsVideoRam);}

		virtual bool Is_Direct_Draw(void) const override {return(true);}

		virtual bool Can_Blit(void) const;

		LPDIRECTDRAWSURFACE Get_DD_Surface(void) { return(SurfacePtr); }

		static Rect ViewBounds;
		static DSurface * Composite;
		static DSurface * Temp;

		static bool AllowStretchBlits;

		static int PrimaryColorMode;

		static bool AllowHWFill;

		static bool OverlappedVideoBlits;

		static int Build_Hicolor_Pixel(int red, int green, int blue);
		static int Build_Hicolor_Pixel(RGBClass const & rgb);
		static RGBClass Deconstruct_Hicolor_Pixel(unsigned short pixel);
		static unsigned short Blend_Pixel(unsigned short src_color, unsigned short dst_color, int level);
		static void Build_Remap_Table(unsigned short * table, int count, PaletteClass const & palette);
		static unsigned short Get_Halfbright_Mask(void) {return(HalfbrightMask);}
		static unsigned short Get_Quarterbright_Mask(void) {return(QuarterbrightMask);}
		static unsigned short Get_Eighthbright_Mask(void) {return(EighthbrightMask);}

		static int Get_Red_Right(void);
		static int Get_Red_Left(void);
		static int Get_Green_Right(void);
		static int Get_Green_Left(void);
		static int Get_Blue_Right(void);
		static int Get_Blue_Left(void);
		static int Get_Primary_Color_Mode(void);

		bool Restore_Check(void) const;
		bool Is_Lost(void) const;

		friend bool Restore_Surface(DSurface const * surface);

	protected:

		mutable int BytesPerPixel;

		mutable void * LockPtr;

		bool IsPrimary;

		mutable bool IsVideoRam;

		LPDIRECTDRAWSURFACE SurfacePtr;
		DDSURFACEDESC * Description;

		static LPDIRECTDRAWCLIPPER Clipper;

		static DDPIXELFORMAT PixelFormat;

	public:

		static int RedRight;
		static int RedLeft;
		static int BlueRight;
		static int BlueLeft;
		static int GreenRight;
		static int GreenLeft;

	protected:
		static unsigned short HalfbrightMask;
		static unsigned short QuarterbrightMask;
		static unsigned short EighthbrightMask;

	private:

		DSurface(DSurface const & rvalue);
		DSurface const operator = (DSurface const & rvalue);
};

inline int DSurface::Build_Hicolor_Pixel(int red, int green, int blue)
{
	return(((red >> RedLeft) << RedRight) | ((green >> GreenLeft) << GreenRight) | ((blue >> BlueLeft) << BlueRight));
}

inline int DSurface::Build_Hicolor_Pixel(RGBClass const & rgb)
{
	return(((rgb.Get_Red() >> RedLeft) << RedRight) | ((rgb.Get_Green() >> GreenLeft) << GreenRight) | ((rgb.Get_Blue() >> BlueLeft) << BlueRight));
}

inline RGBClass DSurface::Deconstruct_Hicolor_Pixel(unsigned short color)
{
	return(RGBClass(
		color >> DSurface::RedRight << DSurface::RedLeft,
		color >> DSurface::GreenRight << DSurface::GreenLeft,
		color >> DSurface::BlueRight << DSurface::BlueLeft));
}
