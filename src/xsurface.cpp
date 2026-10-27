// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "xsurface.h"
#include "assert.h"
#include "blit.h"
#include "blitblit.h"
#include "swap.h"

typedef int OutCode;

const int CODE_INSIDE = 0;

const int CODE_LEFT = 1;

const int CODE_RIGHT = 2;

const int CODE_BOTTOM = 4;

const int CODE_TOP = 8;

OutCode Compute_Code(double x, double y, Rect const & rect);

// FUNCTION: GAMEMD 0x007BA5E0
bool XSurface::Draw_Line(Point2D const & startpoint, Point2D const & endpoint, int color)
{
	return(XSurface::Draw_Line(Get_Rect(), startpoint, endpoint, color));
}

// FUNCTION: GAMEMD 0x007BA610
bool XSurface::Draw_Line(Rect const & xcliprect, Point2D const & startpoint, Point2D const & endpoint, int color)
{

	// Ensure that the clipping rectangle is legal.
	Rect cliprect = Intersect(xcliprect, Get_Rect());

	// High-speed working variables for the clipping rectangle and clipping operation.
	Point2D start = Bias_To(startpoint, cliprect);
	Point2D end = Bias_To(endpoint, cliprect);

	if (!Clip_Line_To_Rect(start, end, cliprect)) return(false);

	if (start.X > end.X) {
		swap(start, end);
	}

	int bpp = Bytes_Per_Pixel();
	void * buffer = Lock(start);
	if (buffer != NULL) {

		if (start.Y == end.Y) {

			// Simplest of the blits, straight horizontal line.
			if (bpp == 1) {
				memset(buffer, color, end.X - start.X + 1);
			} else {
				for (int i = 0; i <= end.X - start.X; i++) {
					*((unsigned short *)buffer + i) = color;
				}
			}

		} else if (start.X == end.X) {
			int pitch = start.Y > end.Y ? -Stride() : Stride();

			// Straight vertical line.
			int dy = abs(end.Y - start.Y);
			for (int i = 0; i <= dy; i++) {
				if (bpp == 1) {
					*(unsigned char *)buffer = color;
				} else {
					*(unsigned short *)buffer = color;
				}
				buffer = (unsigned char *)buffer + pitch;
			}
		} else {
			// Distances to x and y.
			int dx = end.X - start.X;
			int dy = end.Y - start.Y;
			// The line isn't straight so we need to do some maths.
			int pitch = Stride();
			if (dy < 0) {
				pitch = -pitch;
			}

			dy = abs(dy);
			int dx2 = 2 * dx;
			int dy2 = 2 * dy;

			if (dx > dy) {

				// The slope is not steep.
				int delta = dy2 - dx;

				// Plot low line.
				for (int i = 0; i < dx; i++) {
					if (bpp == 1) {
						*((unsigned char *)buffer + i) = color;
					} else {
						*((unsigned short *)buffer + i) = color;
					}

					if (delta > 0) {
						buffer = (unsigned char *)buffer + pitch;
						delta -= dx2;
					}

					delta += dy2;
				}
			} else {

				// The slope is steep.
				int delta = dx2 - dy;
				int k = 0;

				// Plot high line.
				for (int i = 0; i < dy; i++) {
					if (bpp == 1) {
						*((unsigned char *)buffer + k) = color;
					} else {
						*((unsigned short *)buffer + k) = color;
					}

					if (delta > 0) {
						k++;
						delta -= dy2;
					}

					delta += dx2;
					buffer = (unsigned char *)buffer + pitch;
				}
			}
		}

		Unlock();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x007BA8C0
int XSurface::Draw_Dashed_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool pattern[], int offset)
{
	Point2D start = startpoint;
	Point2D end = endpoint;

	int initial_offset = 1;

	if (start.X > end.X) {
		swap(start, end);
		int norm = abs(start.X - end.X);
		if (norm < abs(start.Y - end.Y) + 1) {
			norm = abs(start.Y - end.Y) + 1;
		}
		offset = (offset + norm) % 16;
		initial_offset *= -1;
	}

	int bpp = Bytes_Per_Pixel();
	void *ptr = Lock(start);

	if (ptr != NULL) {
		if (start.Y == end.Y) {
			// Simplest of the blits, straight horizontal line.
			for (int i = 0; i <= end.X - start.X; i++, offset += initial_offset) {
				if (offset < 0) {
					offset += 16;
				}
				if (offset >= 16) {
					offset -= 16;
				}
				if (pattern[offset]) {
						*((unsigned short *)ptr + i) = color;
				}
			}

		} else if (start.X == end.X) {
			int pitch = start.Y > end.Y ? -Stride() : Stride();
			// Straight vertical line.
			int dy = abs(end.Y - start.Y);
			for (int i = 0; i <= dy; i++, offset += initial_offset) {
				if (offset < 0) {
					offset += 16;
				}
				if (offset >= 16) {
					offset -= 16;
				}
				if (pattern[offset]) {
					if (bpp == 1) {
						*(unsigned char *)ptr = color;
					} else {
						*(unsigned short *)ptr = color;
					}
				}
				ptr = (unsigned char *)ptr + pitch;
			}

		} else {
			// Distances to x and y.
			int dx = end.X - start.X;
			int dy = end.Y - start.Y;
			// The line isn't straight so we need to do some maths.
			int pitch = Stride();
			if (dy < 0) {
				pitch = -pitch;
			}

			dy = abs(dy);
			int dx2 = 2 * dx;
			int dy2 = 2 * dy;

			if (dx > dy) {

				// The slope is not steep.
				int delta = dy2 - dx;

				// Plot low line.
				for (int i = 0; i < dx; i++, offset += initial_offset) {

					if (offset < 0) {
						offset += 16;
					}
					if (offset >= 16) {
						offset -= 16;
					}
					if (pattern[offset]) {
						if (bpp == 1) {
							*((unsigned char *)ptr + i) = color;
						} else {
							*((unsigned short *)ptr + i) = color;
						}
					}

					if (delta > 0) {
						ptr = (unsigned char *)ptr + pitch;
						delta -= dx2;
					}

					delta += dy2;

				}
			} else {

				// The slope is steep.
				int delta = dx2 - dy;
				int k = 0;

				// Plot high line.
				for (int i = 0; i < dy; i++, offset += initial_offset) {

					if (offset < 0) {
						offset += 16;
					}
					if (offset >= 16) {
						offset -= 16;
					}
					if (pattern[offset]) {
						if (bpp == 1) {
							*((unsigned char *)ptr + k) = color;
						} else {
							*((unsigned short *)ptr + k) = color;
						}
					}

					if (delta > 0) {
						k++;
						delta -= dy2;
					}

					delta += dx2;
					ptr = (unsigned char *)ptr + pitch;
				}
			}
		}

		Unlock();
	}

	return(offset);
}

// FUNCTION: GAMEMD 0x007BAB30
int XSurface::Draw_Masked_Dashed_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool pattern[], int offset, bool draw_on_zero_alpha)
{
	return(false);
}

// FUNCTION: GAMEMD 0x007BAB40
bool XSurface::Draw_Masked_Line(Point2D const & startpoint, Point2D const & endpoint, unsigned color, bool draw_on_zero_alpha)
{
	return(false);
}

// FUNCTION: GAMEMD 0x007BAB50
bool XSurface::Draw_Depth_Shaded_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, unsigned color, int start_depth, int end_depth, bool write_depth)
{
	return(false);
}

// FUNCTION: GAMEMD 0x007BAB60
bool XSurface::Draw_Depth_Glow_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, int glow_strength, int start_depth, int end_depth, bool write_depth)
{
	return(false);
}

// FUNCTION: GAMEMD 0x007BAB70
bool XSurface::Draw_Depth_Antialiased_Line(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass & color, int start_depth, int end_depth, bool write_depth, bool blend_red, bool blend_green, bool blend_blue, float intensity)
{
	return(false);
}

// FUNCTION: GAMEMD 0x007BAB80
bool XSurface::DrawRGBMultiplyingLine(Rect const & cliprect, Point2D const & startpoint, Point2D const & endpoint, RGBClass const & color, float intensity, int start_depth, int end_depth)
{
	return(false);
}

// FUNCTION: GAMEMD 0x007BAD90
bool XSurface::Draw_Rect(Rect const & crect, int color)
{
	return(XSurface::Draw_Rect(Get_Rect(), crect, color));
}

// FUNCTION: GAMEMD 0x007BADC0
bool XSurface::Draw_Rect(Rect const & cliprect, Rect const & crect, int color)
{
	Point2D ul(crect.X, crect.Y);
	Point2D ur(crect.X+crect.Width-1, crect.Y);
	Point2D ll(crect.X, crect.Y+crect.Height-1);
	Point2D lr(crect.X+crect.Width-1, crect.Y+crect.Height-1);

	Draw_Line(cliprect, ul, ur, color);
	Draw_Line(cliprect, ul, ll, color);
	Draw_Line(cliprect, ur, lr, color);
	Draw_Line(cliprect, ll, lr, color);
	return(true);
}

// FUNCTION: GAMEMD 0x007BAE60
int XSurface::Get_Pixel(Point2D const & point) const
{
	int color = 0;
	void * pointer = ((Surface*)this)->Lock(point);
	if (pointer != NULL) {
		if (Bytes_Per_Pixel() == 2) {
			color = *((unsigned short*)pointer);
		} else {
			color = *((unsigned char*)pointer);
		}
		((Surface*)this)->Unlock();
	}
	return(color);
}

// FUNCTION: GAMEMD 0x007BAEB0
bool XSurface::Put_Pixel(Point2D const & point, int color)
{
	void * pointer = Lock(point);
	if (pointer != NULL) {
		if (Bytes_Per_Pixel() == 2) {
			*((unsigned short*)pointer) = (unsigned short)color;
		} else {
			*((unsigned char*)pointer) = (unsigned char)color;
		}
		Unlock();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x007BAF10
int XSurface::Get_Pixel_Clip(Point2D const & point, Rect const & rect) const
{
	unsigned pixel = 0;

	if (rect.Is_Point_Within(point)) {
		void *buffptr = Lock(point);

		if (buffptr != NULL) {
			if (Bytes_Per_Pixel() == 2) {
				pixel = *static_cast<unsigned short *>(buffptr);
			} else {
				pixel = *static_cast<unsigned char *>(buffptr);
			}
			Unlock();
		}
	}
	return(pixel);
}

// FUNCTION: GAMEMD 0x007BAF90
bool XSurface::Put_Pixel_Clip(Point2D const & point, int color, Rect const & rect)
{
	if (rect.Is_Point_Within(point)) {
		void *buffptr = Lock(point);

		if (buffptr != NULL) {
			if (Bytes_Per_Pixel() == 2) {
				*static_cast<unsigned short *>(buffptr) = color;
			} else {
				*static_cast<unsigned char *>(buffptr) = color;
			}
			Unlock();
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x007BB020
bool XSurface::Fill_Rect(Rect const & fillrect, int color)
{
	return(XSurface::Fill_Rect(fillrect, Get_Rect(), color));
}

#pragma warning(disable: 4035)
static void *surface_quick_fill(void *buf, int count, int color)
{
	_asm {
		push edi

		mov ecx, [count]
		mov edi, [buf]

		cmp ecx, 0
		jle short $end

		mov eax, [color]
		rep stosd

	$end:
		mov eax, edi
		pop edi
	}
}
#pragma warning(default: 4035)

// FUNCTION: GAMEMD 0x007BB050
bool XSurface::Fill_Rect(Rect const & cliprect, Rect const & fillrect, int color)
{
	if (!fillrect.Is_Valid()) return(false);

	Rect wrect = Intersect(cliprect, Get_Rect());

	Rect crect = Intersect(wrect, fillrect.Bias_To(cliprect));
	if (!crect.Is_Valid()) return(false);

	int height = crect.Height;
	int width = crect.Width;
	int pitch = Stride();

	void * buffer = Lock(crect.Top_Left());
	if (buffer != NULL) {
		if (Bytes_Per_Pixel() == 1) {
			for (int y = 0; y < crect.Height; y++) {
				memset(buffer, color, crect.Width);
				buffer = ((unsigned char *)buffer) + pitch;
			}
		} else {
			switch ((unsigned int)buffer & 3) {
				case 0: {
					int odd_pixel = width & 1;
					width >>= 1;
					int doubled_color = (color << 16) | color;
					for (int i = 0; i < height; i++) {
						void *tail = surface_quick_fill(buffer, width, doubled_color);
						if (odd_pixel) {
							*(unsigned short *)tail = color;
						}
						buffer = (unsigned char *)buffer + pitch;
					}
					break;
				}

				case 2: {
					if (width <= 1) {
						if (width == 1) {
							for (int i = 0; i < height; i++) {
								*(unsigned short *)buffer = color;
								buffer = (unsigned char *)buffer + pitch;
							}
						}
					} else {
						width--;
						int odd_pixel = width & 1;
						width >>= 1;
						int doubled_color = (color << 16) | color;
						for (int i = 0; i < height; i++) {
							*(unsigned short *)buffer = color;
							void * tail = surface_quick_fill((unsigned short *)buffer + 1, width, doubled_color);
							if (odd_pixel) {
								*(unsigned short *)tail = color;
							}
							buffer = (unsigned char *)buffer + pitch;
						}
					}
					break;
				}

				default: {
					for (int y = 0; y < height; y++) {
						for (int x = 0; x < width; x++) {
							((unsigned short*)buffer)[x] = (unsigned short)color;
						}
						buffer = ((unsigned char *)buffer) + pitch;
					}
					break;
				}
			}
		}
		Unlock();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x007BB340
bool XSurface::Fill_Rect_Trans(Rect const & rect, RGBClass const & color, unsigned int opacity)
{
	return(false);
}

// FUNCTION: GAMEMD 0x007BB920
void XSurface::Draw_Circle(Point2D center, int radius, Rect rect, int color)
{
	Point2D pt(radius, 0);

	const int roundness_val = 2;

	int d = 3 - (roundness_val * radius);

	do {
		Draw_Line(rect, center + Point2D(-pt.X,  pt.Y), center + Point2D(pt.X,  pt.Y), color);
		Draw_Line(rect, center + Point2D(-pt.Y,  pt.X), center + Point2D(pt.Y,  pt.X), color);
		Draw_Line(rect, center + Point2D(-pt.X, -pt.Y), center + Point2D(pt.X, -pt.Y), color);
		Draw_Line(rect, center + Point2D(-pt.Y, -pt.X), center + Point2D(pt.Y, -pt.X), color);

		if (d < 0) {
			d += (4 * pt.Y) + 6;

		} else {
			d += 4 * (pt.Y - pt.X) + 10;
			pt.X--;
		}

		pt.Y++;

	} while (pt.X >= pt.Y);
}

// FUNCTION: GAMEMD 0x007BBAB0
bool XSurface::Fill(int color)
{
	return(Fill_Rect(Get_Rect(), Get_Rect(), color));
}

// FUNCTION: GAMEMD 0x007BBAF0
bool XSurface::Blit_From(Surface const & source, bool trans, bool unknown)
{
	Rect drect = Get_Rect();
	drect.X = 0;
	drect.Y = 0;

	Rect srect = source.Get_Rect();
	srect.X = 0;
	srect.Y = 0;
	bool result = Blit_From(drect, source, srect, trans, unknown);

	return(result);
}

// FUNCTION: GAMEMD 0x007BBB90
bool XSurface::Blit_From(Rect const & destrect, Surface const & source, Rect const & sourcerect, bool trans, bool unknown)
{
	return(XSurface::Blit_From(Get_Rect(), destrect, source, source.Get_Rect(), sourcerect, trans, unknown));
}

// FUNCTION: GAMEMD 0x007BBCF0
bool XSurface::Blit_From(Rect const & dcliprect, Rect const & destrect, Surface const & source, Rect const & scliprect, Rect const & sourcerect, bool trans, bool)
{
	Rect drect = destrect;
	Rect srect = sourcerect;

	if (Blit_Clip(drect, dcliprect, srect, scliprect)) {

		if (trans) {
			return(Blit_Trans(*this, drect, source, srect));
		}
		return(Blit_Plain(*this, drect, source, srect));
	}
	return(false);
}

// FUNCTION: GAMEMD 0x007BBE20
bool Blit_Clip(Rect & drect, Rect const & dwindow, Rect & srect, Rect const & swindow)
{
	if (drect.Width == srect.Width && drect.Height == srect.Height) {
		if (drect.X < 0) {
			srect.X += -drect.X;
			srect.Width -= -drect.X;
			drect.Width -= -drect.X;
			drect.X = 0;
		}
		if (drect.Y < 0) {
			srect.Y += -drect.Y;
			srect.Height -= -drect.Y;
			drect.Height -= -drect.Y;
			drect.Y = 0;
		}

		int rightspill = (drect.X+drect.Width) - dwindow.Width;
		if (rightspill > 0) {
			srect.Width -= rightspill;
			drect.Width -= rightspill;
		}
		int bottomspill = (drect.Y+drect.Height) - dwindow.Height;
		if (bottomspill > 0) {
			srect.Height -= bottomspill;
			drect.Height -= bottomspill;
		}

		if (srect.X < 0) {
			drect.X += -srect.X;
			srect.Width -= -srect.X;
			drect.Width -= -srect.X;
			srect.X = 0;
		}
		if (srect.Y < 0) {
			drect.Y += -srect.Y;
			srect.Height -= -srect.Y;
			drect.Height -= -srect.Y;
			srect.Y = 0;
		}

		rightspill = (srect.X+srect.Width) - swindow.Width;
		if (rightspill > 0) {
			srect.Width -= rightspill;
			drect.Width -= rightspill;
		}
		bottomspill = (srect.Y+srect.Height) - swindow.Height;
		if (bottomspill > 0) {
			srect.Height -= bottomspill;
			drect.Height -= bottomspill;
		}

	} else {
		drect = Intersect(drect, dwindow);
		srect = Intersect(srect, swindow);
	}

	return(drect.Is_Valid() && srect.Is_Valid());
}

// FUNCTION: GAMEMD 0x007BBFF0
bool XSurface::Prep_For_Blit(Surface & dest, Rect & drect, Surface const & source, Rect & srect, bool & overlapped, void * & dbuffer, void * & sbuffer)
{
	return(XSurface::Prep_For_Blit(dest, dest.Get_Rect(), drect, source, source.Get_Rect(), srect, overlapped, dbuffer, sbuffer));
}

// FUNCTION: GAMEMD 0x007BC040
bool XSurface::Prep_For_Blit(Surface & dest, Rect const & dcliprect, Rect & drect, Surface const & source, Rect const & scliprect, Rect & srect, bool & overlapped, void * & dbuffer, void * & sbuffer)
{
	overlapped = false;
	dbuffer = NULL;
	sbuffer = NULL;

	if (!drect.Is_Valid() || !dcliprect.Is_Valid() || !srect.Is_Valid() || !scliprect.Is_Valid()) return(false);

	if (!Blit_Clip(drect, dcliprect, srect, scliprect)) {
		return(false);
	}

	overlapped = false;
	if (&source == &dest && srect.Is_Overlapping(drect)) {
		if (srect.Y < drect.Y || (srect.Y == drect.Y && srect.X < drect.X)) {
			overlapped = true;
		}
	}

	dbuffer = dest.Lock(dcliprect.Top_Left() + drect.Top_Left());
	if (dbuffer == NULL) return(false);
	sbuffer = source.Lock(scliprect.Top_Left() + srect.Top_Left());
	if (sbuffer == NULL) {
		dest.Unlock();
		return(false);
	}

	return(true);
}

// FUNCTION: GAMEMD 0x007BC1F0
bool XSurface::Blit_Plain(Surface & dest, Rect const & destrect, Surface const & source, Rect const & sourcerect)
{
	if (dest.Bytes_Per_Pixel() == 1) {
		return(Bit_Blit(dest, destrect, source, sourcerect, BlitPlain<unsigned char>()));
	}
	return(Bit_Blit(dest, destrect, source, sourcerect, BlitPlain<unsigned short>()));
}

// FUNCTION: GAMEMD 0x007BC250
bool XSurface::Blit_Trans(Surface & dest, Rect const & destrect, Surface const & source, Rect const & sourcerect)
{
	if (dest.Bytes_Per_Pixel() == 1) {
		return(Bit_Blit(dest, destrect, source, sourcerect, BlitTrans<unsigned char>()));
	}
	return(Bit_Blit(dest, destrect, source, sourcerect, BlitTrans<unsigned short>()));
}

// FUNCTION: GAMEMD 0x007BC2B0
bool Clip_Line_To_Rect(Point2D & point1, Point2D & point2, Rect const & rect)
{
	int outcode0;
	int outcode1;
	int outcodeOut;

	double x = rect.X;
	double y = rect.Y;

	double x0 = point1.X;
	double y0 = point1.Y;
	double x1 = point2.X;
	double y1 = point2.Y;

	double slope_y = (x1 - x0) / (y1 - y0);
	double slope_x = (y1 - y0) / (x1 - x0);

	outcode0 = Compute_Code(x0, y0, rect);
	outcode1 = Compute_Code(x1, y1, rect);

AGAIN:
	if (outcode0 == CODE_INSIDE && outcode1 == CODE_INSIDE) {
		point1.X = (int)x0;
		point1.Y = (int)y0;
		point2.X = (int)x1;
		point2.Y = (int)y1;
		return(true);
	}

	if (outcode0 & outcode1) {
		return(false);
	}

	outcodeOut = (outcode0 != CODE_INSIDE) ? outcode0 : outcode1;

	if (outcodeOut & CODE_TOP) {
		x = (rect.Y - y0) * slope_y + x0;
		y = rect.Y;
	} else if (outcodeOut & CODE_BOTTOM) {
		x = ((rect.Y + rect.Height - 1) - y0) * slope_y + x0;
		y = (rect.Height + rect.Y - 1);
	} else if (outcodeOut & CODE_RIGHT) {
		y = ((rect.X + rect.Width - 1) - x0) * slope_x + y0;
		x = (rect.Width + rect.X - 1);
	} else if (outcodeOut & CODE_LEFT) {
		y = (rect.X - x0) * slope_x + y0;
		x = rect.X;
	}

	if (outcodeOut == outcode0) {
		x0 = x;
		y0 = y;
		outcode0 = Compute_Code(x0, y0, rect);
	} else {
		x1 = x;
		y1 = y;
		outcode1 = Compute_Code(x1, y1, rect);
	}
	goto AGAIN;

	return(false);
}

// FUNCTION: GAMEMD 0x007BC580
OutCode Compute_Code(double x, double y, Rect const & rect)
{
	OutCode code = CODE_INSIDE;

	int xx = rect.X + rect.Width;
	if (x >= xx) {
		code |= CODE_RIGHT;
	} else if (x < rect.X) {
		code |= CODE_LEFT;
	}

	int yy = rect.Y + rect.Height;
	if (y >= yy) {
		code |= CODE_BOTTOM;
	} else if (y < rect.Y) {
		code |= CODE_TOP;
	}

	return(code);
}

// TEMPLATE: GAMEMD 0x007BC610
// ?BlitForward@?$BlitTrans@E@@UBEXPAXPBXHH00HH@Z
// SYNTHETIC: GAMEMD 0x007BC640
// ??_G?$BlitPlain@E@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007BC660
// ??_G?$BlitPlain@G@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007BC680
// ??_G?$BlitTrans@E@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007BC6A0
// ??_G?$BlitTrans@G@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x007BC6C0
// ?BlitBackward@?$BlitTrans@E@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x007BC6F0
// ?Blit_Copy_Tinted@?$BlitTrans@E@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x007BC720
// ?Blit_Move_Tinted@?$BlitTrans@E@@UBEXPAXPBXHH00HG@Z
// TEMPLATE: GAMEMD 0x007BC750
// ?BlitForward@?$BlitPlain@E@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x007BC780
// ?BlitBackward@?$BlitPlain@E@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x007BC7A0
// ?Blit_Copy_Tinted@?$BlitPlain@E@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x007BC7D0
// ?Blit_Move_Tinted@?$BlitPlain@E@@UBEXPAXPBXHH00HG@Z
// TEMPLATE: GAMEMD 0x007BC7F0
// ?BlitForward@?$BlitPlain@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x007BC820
// ?BlitBackward@?$BlitPlain@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x007BC840
// ?Blit_Copy_Tinted@?$BlitPlain@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x007BC870
// ?Blit_Move_Tinted@?$BlitPlain@G@@UBEXPAXPBXHH00HG@Z
// TEMPLATE: GAMEMD 0x007BC890
// ?BlitForward@?$BlitTrans@G@@UBEXPAXPBXHH00HH@Z
// TEMPLATE: GAMEMD 0x007BC8C0
// ?BlitBackward@?$BlitTrans@G@@UBEXPAXPBXHH00H@Z
// TEMPLATE: GAMEMD 0x007BC8F0
// ?Blit_Copy_Tinted@?$BlitTrans@G@@UBEXPAXPBXHH00HHG@Z
// TEMPLATE: GAMEMD 0x007BC920
// ?Blit_Move_Tinted@?$BlitTrans@G@@UBEXPAXPBXHH00HG@Z
template class BlitPlain<unsigned char>;
template class BlitPlain<unsigned short>;
template class BlitTrans<unsigned char>;
template class BlitTrans<unsigned short>;
